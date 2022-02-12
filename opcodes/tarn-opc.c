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
