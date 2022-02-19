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

  uint8_t inst_h = inst >> 8;
  uint8_t inst_l = inst & 0xff;

  uint8_t reg_src = inst_h >> 4;
  uint8_t reg_dest = inst_h & 0xf;

  // Low instruction byte is loaded into IL
  cpu->registers[11] = inst_l;
  //printf("# 0x%04x [step]\n", pc);

  sim_cia opc = pc;
  if (reg_src == 0 && reg_dest == 0) { // NOP
      // do nothing
      //printf("# 0x%04x: NOP\n", pc);
      pc += 2;
  } else if (reg_src == TARN_SREG_JUMP) { // JUMP
      if (inst == 0x9999) {
          // Fake halt instruction for simulation only.
          sim_engine_halt (sd, cpu, NULL, pc, sim_exited, 0);
      } else if (reg_dest != 0) {
          printf("Invalid jump opcode: %02x %02x", inst_h, inst_l);
          sim_engine_halt (sd, cpu, NULL, pc, sim_stopped, SIM_SIGILL);
      } else {
          pc = (cpu->registers[3] << 8) + cpu->registers[4];
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
      uint8_t src_value;
      switch (reg_src) {
      case TARN_SREG_MEM:
          src_value = sim_core_read_aligned_1 (cpu, pc, read_map, (cpu->registers[TARN_DREG_ADH] << 8) + cpu->registers[TARN_DREG_ADL]);
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
      default:
          src_value = cpu->registers[reg_src];
          break;
      }

      cpu->registers[reg_dest] = src_value;

      /* printf("# 0x%04x: [%02x %02x] (%x %x) %s -> %s; %02x\n", */
      /*        opc, inst_h, inst_l, reg_src, reg_dest, from_registers[reg_src], to_registers[reg_dest], src_value); */

      switch (reg_dest) {
      case TARN_DREG_P1:
          printf("OUTPUT1 %d\n", src_value);
          break;
      case TARN_DREG_P0:
          printf("OUTPUT0 %d\n", src_value);
          break;
      case TARN_DREG_ARA:
      case TARN_DREG_ARB:
          {
              // update adder
              uint16_t sum = (uint16_t)cpu->registers[TARN_DREG_ARA] + (uint16_t)cpu->registers[TARN_DREG_ARB];
              // In the actual HW, status isn't updated until it gets
              // a load signal, so we need a dummy instruction later
              // to load the value into the actual register.
              cpu->status = sum > 0xff ? 1 : 0;
              cpu->registers[TARN_SREG_ARC] = sum & 0xff;
              // printf("# 0x%04x: ARC = %d\n", pc, cpu->registers[TARN_SREG_ARC]);
          }
          break;
      case TARN_DREG_STATUS:
          // status is not really writable, but the hardware requires
          // that an instruction load it for it to be updated.
          cpu->registers[TARN_SREG_STATUS] = cpu->status;
          break;
      case TARN_DREG_MEM:
          {
              uint16_t addr = (cpu->registers[TARN_DREG_ADH] << 8) + cpu->registers[TARN_DREG_ADL];
              sim_core_write_aligned_1 (cpu, pc, write_map, addr, cpu->registers[TARN_DREG_MEM]);
          }
          break;
      default:
          // nothing to do
          break;
      }
      pc += 2;
  }

  ++cpu->inst_count;

  if (pc > 0x4000 || cpu->inst_count > 1000000) {
      sim_engine_halt (sd, cpu, NULL, pc, sim_stopped, SIM_SIGILL);
  }

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
  memset (cpu->registers, 0, sizeof (cpu->registers));
  cpu->pc = 0;
  cpu->inst_count = 0;
  /* Make sure it's initialized outside of the 16-bit address space.  */
  // cpu->sp = 0x80000;

  CPU_PC_FETCH (cpu) = pc_get;
  CPU_PC_STORE (cpu) = pc_set;
}
