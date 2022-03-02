/* Example synacor simulator.

   Copyright (C) 2005-2021 Free Software Foundation, Inc.
   Contributed by Mike Frysinger.

   This file is part of simulators.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* This file contains the main simulator decoding logic.  i.e. everything that
   is architecture specific.  */

#include "config.h"

#include "sim-main.h"
#include "opcode/tarn.h"

/* Decode & execute a single instruction.  */
void step_once (SIM_CPU *cpu)
{
  SIM_DESC sd = CPU_STATE (cpu);
  uint16_t inst;
  sim_cia pc = sim_pc_get (cpu);

  inst = sim_core_read_aligned_2 (cpu, pc, exec_map, pc);
  if (inst == TARN_INST_HALT_SIM) {
      // Fake halt instruction for simulation only.
      sim_engine_halt (sd, cpu, NULL, pc, sim_exited, 0);
  }

  uint8_t inst_h = inst >> 8;
  uint8_t inst_l = inst & 0xff;

  uint8_t reg_src = inst_h >> 4;
  uint8_t reg_dest = inst_h & 0xf;

  // Low instruction byte is loaded into IL
  cpu->sregs[TARN_SREG_IL] = inst_l;
  //printf("# 0x%04x [step]\n", pc);

  sim_cia opc = pc;
  if (reg_src == 0 && reg_dest == 0) { // NOP
      // do nothing
      //printf("# 0x%04x: NOP\n", pc);
      pc += 2;
  } else if (reg_src == TARN_SREG_JUMP) { // JUMP
      if (reg_dest != 0) {
          printf("Invalid jump opcode: %02x %02x", inst_h, inst_l);
          sim_engine_halt (sd, cpu, NULL, pc, sim_stopped, SIM_SIGILL);
      } else {
          pc = (cpu->dregs[TARN_DREG_JMPH] << 8) + cpu->dregs[TARN_DREG_JMPL];
          // printf("# 0x%04x: jump to %04x\n", opc, pc);
      }
  /* } else if (reg_src == 0x5) { // JNZ */
  /*     printf("Invalid jnz opcode: %02x\n", inst_h); */
  /*     sim_engine_halt (sd, cpu, NULL, pc, sim_stopped, SIM_SIGILL); */
  /* } else if (reg_dest == 0x4) { // RETI */
  /*     printf("Invalid reti opcode: %02x\n", inst_h); */
  /*     sim_engine_halt (sd, cpu, NULL, pc, sim_stopped, SIM_SIGILL); */
  } else {
      // Execute generic instruction.
      // TODO: implement adder and stuff .....
      uint8_t src_value = 0;
      switch (reg_src) {
#if TARN_VERSION==TARN_VERSION_V30
      case TARN_SREG_ALUC:
          {
              // Calculate the correct value based on the ALU mode.
              switch (cpu->dregs[TARN_DREG_ALUS] & TARN_ALU_MODE_MASK) {
              case TARN_ALU_MODE_SUM:
                  src_value = cpu->dregs[TARN_DREG_ALUA] + cpu->dregs[TARN_DREG_ALUB];
                  break;
              case TARN_ALU_MODE_AND:
                  src_value = cpu->dregs[TARN_DREG_ALUA] & cpu->dregs[TARN_DREG_ALUB];
                  break;
              case TARN_ALU_MODE_STATUS:
// a > b => status = 0000 0010
// a < b => status = 0000 1000
// a = b => status = 0000 0100
// a + b => sum = 1111 1111 , status = 0000 0010
// a + b => sum = 0000 0000 , status = 0000 0011

                  {
                      uint8_t a = cpu->dregs[TARN_DREG_ALUA];
                      uint8_t b = cpu->dregs[TARN_DREG_ALUB];
                      uint8_t l = a < b  ? (1 << 3) : 0;
                      uint8_t e = a == b ? (1 << 2) : 0;
                      uint8_t g = a > b  ? (1 << 1) : 0;
                      uint8_t c = (uint16_t)a + (uint16_t)b > 0xff ? 1 : 0;
                      src_value = g | l | e | c;
                  }
                  break;
              default:
                  printf("Unimplemented ALU mode %d\r\n", cpu->dregs[TARN_DREG_ALUS]);
                  break;
              }
          }
          break;
#endif
      case TARN_SREG_P0:
          src_value = getchar();
          break;
      case TARN_SREG_MEM:
          src_value = sim_core_read_aligned_1 (cpu, pc, read_map, (cpu->dregs[TARN_DREG_ADH] << 8) + cpu->dregs[TARN_DREG_ADL]);
          break;
      case TARN_SREG_IL:
          src_value = inst_l;
          break;
      case TARN_SREG_ONE:
          src_value = 1;
          break;
      case TARN_SREG_ZERO:
          src_value = 0;
          break;
      case TARN_SREG_R:
          src_value = cpu->dregs[TARN_DREG_R];
          break;
      case TARN_SREG_X:
          src_value = cpu->dregs[TARN_DREG_X];
          break;
      default:
          src_value = cpu->sregs[reg_src];
          break;
      }

      cpu->dregs[reg_dest] = src_value;

           /* printf("# 0x%04x: [%02x %02x] (%x %x) %s -> %s; %02x\n", */
           /*   opc, inst_h, inst_l, reg_src, reg_dest, tarn_src_registers[reg_src].name, tarn_dest_registers[reg_dest].name, src_value); */

      switch (reg_dest) {
#if TARN_VERSION==TARN_VERSION_V23
      case TARN_DREG_P1:
          //          printf("OUTPUT1 %d\n", src_value);
          break;
      case TARN_DREG_ARA:
      case TARN_DREG_ARB:
          {
              // update adder
              uint16_t sum = (uint16_t)cpu->dregs[TARN_DREG_ARA] + (uint16_t)cpu->dregs[TARN_DREG_ARB];
              // In the actual HW, status isn't updated until it gets
              // a load signal, so we need a dummy instruction later
              // to load the value into the actual register.
              cpu->status = sum > 0xff ? 1 : 0;
              cpu->sregs[TARN_SREG_ARC] = sum & 0xff;
              // printf("# 0x%04x: ARC = %d\n", pc, cpu->sregs[TARN_SREG_ARC]);
          }
          break;
      case TARN_DREG_STATUS:
          // status is not really writable, but the hardware requires
          // that an instruction load it for it to be updated.
          cpu->sregs[TARN_SREG_STATUS] = cpu->status;
          break;
#endif
      case TARN_DREG_P0:
          //          printf("OUTPUT0 %d\n", src_value);
          putchar(src_value);
          break;
      case TARN_DREG_MEM:
          {
              uint16_t addr = (cpu->dregs[TARN_DREG_ADH] << 8) + cpu->dregs[TARN_DREG_ADL];
              sim_core_write_aligned_1 (cpu, pc, write_map, addr, cpu->dregs[TARN_DREG_MEM]);
              printf("mem write %02x to %04x\r\n", cpu->dregs[TARN_DREG_MEM], addr);
          }
          break;
      default:
          // nothing to do
          break;
      }
      pc += 2;
  }

  ++cpu->inst_count;

  /* if (pc > 0x4000 || cpu->inst_count > 1000000) { */
  /*     printf("Simulation too long; stopping.\r\n"); */
  /*     sim_engine_halt (sd, cpu, NULL, pc, sim_stopped, SIM_SIGILL); */
  /* } */

  TRACE_REGISTER (cpu, "PC = %#x", pc);
  sim_pc_set (cpu, pc);
}

/* Return the program counter for this cpu. */
static sim_cia
pc_get (sim_cpu *cpu)
{
  return cpu->pc;
}

/* Set the program counter for this cpu to the new pc value. */
static void
pc_set (sim_cpu *cpu, sim_cia pc)
{
  cpu->pc = pc;
}

/* Initialize the state for a single cpu.  Usuaully this involves clearing all
   registers back to their reset state.  Should also hook up the fetch/store
   helper functions too.  */
void initialize_cpu (SIM_DESC sd, SIM_CPU *cpu)
{
  memset (cpu->sregs, 0, sizeof (cpu->sregs));
  memset (cpu->dregs, 0, sizeof (cpu->dregs));
  cpu->pc = 0;
  cpu->inst_count = 0;
  /* Make sure it's initialized outside of the 16-bit address space.  */
  // cpu->sp = 0x80000;

  CPU_PC_FETCH (cpu) = pc_get;
  CPU_PC_STORE (cpu) = pc_set;
}
