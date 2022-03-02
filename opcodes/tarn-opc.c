/* tarn-opc.c -- Definitions for tarn opcodes.
   Copyright 2008 Free Software Foundation, Inc.
   Contributed by Anthony Green (green@spindazzle.org).

   This file is part of the GNU opcodes library.

   This library is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   It is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   You should have received a copy of the GNU General Public License
   along with this file; see the file COPYING.  If not, write to the
   Free Software Foundation, 51 Franklin Street - Fifth Floor, Boston,
   MA 02110-1301, USA.  */

#include "sysdep.h"
#include "opcode/tarn.h"

/*

std::vector<register_info_t> readable_registers = {
    { 0,  "NOP",   "NOP" },
    { 1,  "P0",    "ARITHMETIC_C_SEND" },
    { 2,  "INTL",  "STATUS_SEND" },
    { 3,  "INTH",  "INST_MEM_DATA_SEND_WITH_PC" },
    { 4,  "RETI",  "PERIPHERAL_SEND0" },
    { 5,  "IL",    "CR_MODE_INC" },
    { 6,  "STACK", "CR_SEND" },
    { 7,  "PIC",   "R_SEND" },
    { 8,  "",      "INST_PC_INCREMENT" },
    { 9,  "",      "DO_JUMP" },
    { 10, "MEM",   "IH_SEND" },
    { 11, "R",     "IL_SEND" },
    { 12, "",      "MEM_DATA_SEND" },
    { 13, "ZERO",  "PERIPHERAL_SEND1" },
    { 14, "ONE",   "INST_CONST_SEND_0" },
    { 15, "ALUC",  "CONST_SEND_1" },
};

std::vector<register_info_t> writable_registers = {
    { 0,  "NOP",    "NOP" },
    { 1,  "P0",     "PERIPHERAL_LOAD0" },
    { 2,  "JMPL",   "STATUS_LOAD" },
    { 3,  "JMPH",   "PCH_TEMP_LOAD" },
    { 4,  "JUMP",   "PCL_TEMP_LOAD" },
    { 5,  "JNZ",    "CR_LOAD" },
    { 6,  "STACK",  "DO_CREMENT" },
    { 7,  "PIC",    "R_LOAD" },
    { 8,  "ADL",    "ADH_LOAD" },
    { 9,  "ADH",    "ADL_LOAD" },
    { 10, "MEM",    "INST_IH_LOAD" },
    { 11, "R",      "INST_IL_LOAD" },
    { 12, "BS",     "MEM_DATA_LOAD" },
    { 13, "ALUSEL", "PERIPHERAL_LOAD1" },
    { 14, "ALUA",   "ARITHMETIC_A_LOAD" },
    { 15, "ALUB",   "ARITHMETIC_B_LOAD" },
};

from_registers = [
 (0,  "NOP"),
 (1,  "P0"),
 (2,  "INTL"),
 (3,  "INTH"),
 (4,  "RETI"),
 (5,  "IL"),
 (6,  "STACK"),
 (7,  "PIC"),
 (8,  ""),
 (9,  ""),
 (10, "MEM"),
 (11, "R"),
 (12, ""),
 (13, "ZERO"),
 (14, "ONE"),
 (15, "ALUC")
]

to_registers = [
 (0,  "NOP"),
 (1,  "P0"),
 (2,  "JMPL"),
 (3,  "JMPH"),
 (4,  "JUMP"),
 (5,  "JNZ"),
 (6,  "STACK"),
 (7,  "PIC"),
 (8,  "ADL"),
 (9,  "ADH"),
 (10, "MEM"),
 (11, "R"),
 (12, "BS"),
 (13, "ALUSEL"),
 (14, "ALUA"),
 (15, "ALUB")
]

def is_bad(fn, tn):
  if tn == 'JUMP' and fn != 'NOP':
    return True
  if tn == 'JNZ' and fn != 'NOP':
    return True
  if fn == 'RETI' and tn != 'NOP':
    return True
  if tn == 'JUMP' and fn == 'NOP':
    return False
  if tn == 'JNZ' and fn == 'NOP':
    return False
  if fn == 'RETI' and tn == 'NOP':
    return False
  if (tn == 'NOP') != (fn == 'NOP'):
    return True

for fi, fn in from_registers:
  for ti, tn in to_registers:
    if fn and tn:
      if is_bad(fn, tn):
        print('{ 0x%02x, 0x%1x, 0x%1x, "BAD" },' % ((fi << 4) + ti, fi, ti))
      else:
        print('{ 0x%02x, 0x%1x, 0x%1x, "%s_%s" },' % ((fi << 4) + ti, fi, ti, fn, tn))

*/

#if TARN_VERSION==TARN_VERSION_VFUTURE

const tarn_opc_info_t tarn_opc_info[TARN_OPC_COUNT] = {
    { TARN_NOP,  0x0000, "nop" },
    { TARN_JUMP, 0x0400, "jump" },
    { TARN_JNZ,  0x0500, "jnz" },
    { TARN_RETI, 0x4000, "reti" },
    { TARN_MOV,  -1, "mov" },
};

const tarn_reg_list_entry_t tarn_src_registers[TARN_SRC_REG_COUNT] = {
 { 1,  "p0" },
 { 2,  "intl" },
 { 3,  "inth" },
 { 5,  "il" },
 { 6,  "stack" },
 { 7,  "pic" },
 { 10, "mem" },
 { 11, "r" },
 { 13, "zero" },
 { 14, "one" },
 { 15, "aluc" }
 };

const tarn_reg_list_entry_t tarn_dest_registers[TARN_DEST_REG_COUNT] = {
 { 1,  "p0" },
 { 2,  "jmpl" },
 { 3,  "jmph" },
 { 6,  "stack" },
 { 7,  "pic" },
 { 8,  "adl" },
 { 9,  "adh" },
 { 10, "mem" },
 { 11, "r" },
 { 12, "bs" },
 { 13, "alusel" },
 { 14, "alua" },
 { 15, "alub" }
 };

#elif TARN_VERSION==TARN_VERSION_V30

const tarn_opc_info_t tarn_opc_info[TARN_OPC_COUNT] = {
    { TARN_NOP,  0x0000, "nop" },
    { TARN_JUMP, 0xc000, "jump" },
    { TARN_JNZ,  0x9000, "jnz" },
    { TARN_RETI, 0x4000, "reti" },
    { TARN_MOV,  -1, "mov" },
};

const tarn_reg_list_entry_t tarn_dest_registers[TARN_DEST_REG_COUNT] = {
    { 0,  "nop"},
    { 1,  "p0" },
    { 2,  "jmpl" }, // jump target address low byte
    { 3,  "jmph" }, // jump target address high byte
    { 4,  "test" }, // test register for jnz
    { 5,  "x" },
    { 6,  "stack" },
    { 7,  "pic" },
    { 8,  "adl" },
    { 9,  "adh" },
    { 10, "mem" },
    { 11, "r" },
    { 12, "bs" },
    { 13, "alus" },
    { 14, "alua" },
    { 15, "alub" }
};

const tarn_reg_list_entry_t tarn_src_registers[TARN_SRC_REG_COUNT] = {
    { 0,  "nop" },
    { 1,  "p0" },
    { 2,  "intl" },
    { 3,  "inth" },
    { 4,  "do_reti" },
    { 5,  "x" },
    { 6,  "stack" },
    { 7,  "pic" },
    { 8,  "il" },
    { 9,  "jnz" },
    { 10, "mem" },
    { 11, "r" },
    { 12, "jump" },
    { 13, "zero" },
    { 14, "one" },
    { 15, "aluc" }
};

#elif TARN_VERSION==TARN_VERSION_V29
const tarn_opc_info_t tarn_opc_info[TARN_OPC_COUNT] = {
    { TARN_NOP,  0x0000, "nop" },
    { TARN_JUMP, 0x0400, "jump" },
    { TARN_JNZ,  0x0500, "jnz" },
    { TARN_RETI, 0x4000, "reti" },
    { TARN_MOV,  -1, "mov" },
};

const tarn_reg_list_entry_t tarn_dest_registers[TARN_DEST_REG_COUNT] = {
    { 0,  "p0" },
    { 1,  "p1" },
    { 2,  "status" },
    { 3,  "jmph" },
    { 4,  "jmpl" },
    { 5,  "alusel" },
    { 6,  "p2" },
    { 7,  "r" },
    { 8,  "adh" },
    { 9,  "adl" },
    { 10, "ih" },
    { 11, "il" },
    { 12, "mem" },
    { 13, "p3" },
    { 14, "alua" }
    { 15, "alub" }
};

const tarn_reg_list_entry_t tarn_src_registers[TARN_SRC_REG_COUNT] = {
    { 0,  "nop")
    { 1,  "aluc" },
    { 2,  "p0" },
    { 3,  "ihmem" },
    { 4,  "p1" },
    { 5,  "p2" },
    { 6,  "cr" },
    { 7,  "r" },
    { 8,  "pc_increment" },
    { 9,  "_jump" }
    { 10, "ih" },
    { 11, "il" },
    { 12, "mem" }
    { 13, "p3" }
    { 14, "zero" }
    { 15, "one" }
};


#elif TARN_VERSION==TARN_VERSION_V23

#define TARN_INST_HALT_SIM 0x9999 // invalid instruction to halt sim

const tarn_opc_info_t tarn_opc_info[TARN_OPC_COUNT] = {
    { TARN_NOP,  0x0000, "nop" },
    { TARN_JUMP, 0x9000, "jump" },
    { TARN_MOV,  -1, "mov" },
};

const tarn_reg_list_entry_t tarn_dest_registers[TARN_DEST_REG_COUNT] = {
    { 0,    "nop" },
    { 1,    "p0" },
    { 2,    "status" },
    { 3,    "jmph" },
    { 4,    "jmpl" },
    { 5,    "cr" },
    { 6,    "do_crement" },
    { 7,    "r" },
    { 8,    "adh" },
    { 9,    "adl" },
    { 10,    "ih" },
    { 11,    "il" },
    { 12,    "mem" },
    { 13,    "p1" },
    { 14,    "ara" },
    { 15,    "arb" }
};

const tarn_reg_list_entry_t tarn_src_registers[TARN_SRC_REG_COUNT] = {
    { 0,  "nop" },
    { 1,  "arc" },
    { 2,  "status" },
    { 3,  "ihmem" },
    { 4,  "p0" },
    { 5,  "cr_inc" },
    { 6,  "cr" },
    { 7,  "r" },
    { 8,  "pc_inc" },
    { 9,  "jump" },
    { 10,  "ih" },
    { 11,  "il" },
    { 12,  "mem" },
    { 13,  "p1" },
    { 14,  "zero" },
    { 15,  "one" }
};

#else
error "Unsupported tarn version " TARN_VERSION
#endif
