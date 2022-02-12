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
    { 0x00, 0x0, 0x0, "nop" },
    { 0x01, 0x0, 0x1, "bad" },
    { 0x02, 0x0, 0x2, "bad" },
    { 0x03, 0x0, 0x3, "bad" },
    { 0x04, 0x0, 0x4, "jump" },
    { 0x05, 0x0, 0x5, "jnz" },
    { 0x06, 0x0, 0x6, "bad" },
    { 0x07, 0x0, 0x7, "bad" },
    { 0x08, 0x0, 0x8, "bad" },
    { 0x09, 0x0, 0x9, "bad" },
    { 0x0a, 0x0, 0xa, "bad" },
    { 0x0b, 0x0, 0xb, "bad" },
    { 0x0c, 0x0, 0xc, "bad" },
    { 0x0d, 0x0, 0xd, "bad" },
    { 0x0e, 0x0, 0xe, "bad" },
    { 0x0f, 0x0, 0xf, "bad" },
    { 0x10, 0x1, 0x0, "bad" },
    { 0x11, 0x1, 0x1, "p0_p0" },
    { 0x12, 0x1, 0x2, "p0_jmpl" },
    { 0x13, 0x1, 0x3, "p0_jmph" },
    { 0x14, 0x1, 0x4, "bad" },
    { 0x15, 0x1, 0x5, "bad" },
    { 0x16, 0x1, 0x6, "p0_stack" },
    { 0x17, 0x1, 0x7, "p0_pic" },
    { 0x18, 0x1, 0x8, "p0_adl" },
    { 0x19, 0x1, 0x9, "p0_adh" },
    { 0x1a, 0x1, 0xa, "p0_mem" },
    { 0x1b, 0x1, 0xb, "p0_r" },
    { 0x1c, 0x1, 0xc, "p0_bs" },
    { 0x1d, 0x1, 0xd, "p0_alusel" },
    { 0x1e, 0x1, 0xe, "p0_alua" },
    { 0x1f, 0x1, 0xf, "p0_alub" },
    { 0x20, 0x2, 0x0, "bad" },
    { 0x21, 0x2, 0x1, "intl_p0" },
    { 0x22, 0x2, 0x2, "intl_jmpl" },
    { 0x23, 0x2, 0x3, "intl_jmph" },
    { 0x24, 0x2, 0x4, "bad" },
    { 0x25, 0x2, 0x5, "bad" },
    { 0x26, 0x2, 0x6, "intl_stack" },
    { 0x27, 0x2, 0x7, "intl_pic" },
    { 0x28, 0x2, 0x8, "intl_adl" },
    { 0x29, 0x2, 0x9, "intl_adh" },
    { 0x2a, 0x2, 0xa, "intl_mem" },
    { 0x2b, 0x2, 0xb, "intl_r" },
    { 0x2c, 0x2, 0xc, "intl_bs" },
    { 0x2d, 0x2, 0xd, "intl_alusel" },
    { 0x2e, 0x2, 0xe, "intl_alua" },
    { 0x2f, 0x2, 0xf, "intl_alub" },
    { 0x30, 0x3, 0x0, "bad" },
    { 0x31, 0x3, 0x1, "inth_p0" },
    { 0x32, 0x3, 0x2, "inth_jmpl" },
    { 0x33, 0x3, 0x3, "inth_jmph" },
    { 0x34, 0x3, 0x4, "bad" },
    { 0x35, 0x3, 0x5, "bad" },
    { 0x36, 0x3, 0x6, "inth_stack" },
    { 0x37, 0x3, 0x7, "inth_pic" },
    { 0x38, 0x3, 0x8, "inth_adl" },
    { 0x39, 0x3, 0x9, "inth_adh" },
    { 0x3a, 0x3, 0xa, "inth_mem" },
    { 0x3b, 0x3, 0xb, "inth_r" },
    { 0x3c, 0x3, 0xc, "inth_bs" },
    { 0x3d, 0x3, 0xd, "inth_alusel" },
    { 0x3e, 0x3, 0xe, "inth_alua" },
    { 0x3f, 0x3, 0xf, "inth_alub" },
    { 0x40, 0x4, 0x0, "reti" },
    { 0x41, 0x4, 0x1, "bad" },
    { 0x42, 0x4, 0x2, "bad" },
    { 0x43, 0x4, 0x3, "bad" },
    { 0x44, 0x4, 0x4, "bad" },
    { 0x45, 0x4, 0x5, "bad" },
    { 0x46, 0x4, 0x6, "bad" },
    { 0x47, 0x4, 0x7, "bad" },
    { 0x48, 0x4, 0x8, "bad" },
    { 0x49, 0x4, 0x9, "bad" },
    { 0x4a, 0x4, 0xa, "bad" },
    { 0x4b, 0x4, 0xb, "bad" },
    { 0x4c, 0x4, 0xc, "bad" },
    { 0x4d, 0x4, 0xd, "bad" },
    { 0x4e, 0x4, 0xe, "bad" },
    { 0x4f, 0x4, 0xf, "bad" },
    { 0x50, 0x5, 0x0, "bad" },
    { 0x51, 0x5, 0x1, "il_p0" },
    { 0x52, 0x5, 0x2, "il_jmpl" },
    { 0x53, 0x5, 0x3, "il_jmph" },
    { 0x54, 0x5, 0x4, "bad" },
    { 0x55, 0x5, 0x5, "bad" },
    { 0x56, 0x5, 0x6, "il_stack" },
    { 0x57, 0x5, 0x7, "il_pic" },
    { 0x58, 0x5, 0x8, "il_adl" },
    { 0x59, 0x5, 0x9, "il_adh" },
    { 0x5a, 0x5, 0xa, "il_mem" },
    { 0x5b, 0x5, 0xb, "il_r" },
    { 0x5c, 0x5, 0xc, "il_bs" },
    { 0x5d, 0x5, 0xd, "il_alusel" },
    { 0x5e, 0x5, 0xe, "il_alua" },
    { 0x5f, 0x5, 0xf, "il_alub" },
    { 0x60, 0x6, 0x0, "bad" },
    { 0x61, 0x6, 0x1, "stack_p0" },
    { 0x62, 0x6, 0x2, "stack_jmpl" },
    { 0x63, 0x6, 0x3, "stack_jmph" },
    { 0x64, 0x6, 0x4, "bad" },
    { 0x65, 0x6, 0x5, "bad" },
    { 0x66, 0x6, 0x6, "stack_stack" },
    { 0x67, 0x6, 0x7, "stack_pic" },
    { 0x68, 0x6, 0x8, "stack_adl" },
    { 0x69, 0x6, 0x9, "stack_adh" },
    { 0x6a, 0x6, 0xa, "stack_mem" },
    { 0x6b, 0x6, 0xb, "stack_r" },
    { 0x6c, 0x6, 0xc, "stack_bs" },
    { 0x6d, 0x6, 0xd, "stack_alusel" },
    { 0x6e, 0x6, 0xe, "stack_alua" },
    { 0x6f, 0x6, 0xf, "stack_alub" },
    { 0x70, 0x7, 0x0, "bad" },
    { 0x71, 0x7, 0x1, "pic_p0" },
    { 0x72, 0x7, 0x2, "pic_jmpl" },
    { 0x73, 0x7, 0x3, "pic_jmph" },
    { 0x74, 0x7, 0x4, "bad" },
    { 0x75, 0x7, 0x5, "bad" },
    { 0x76, 0x7, 0x6, "pic_stack" },
    { 0x77, 0x7, 0x7, "pic_pic" },
    { 0x78, 0x7, 0x8, "pic_adl" },
    { 0x79, 0x7, 0x9, "pic_adh" },
    { 0x7a, 0x7, 0xa, "pic_mem" },
    { 0x7b, 0x7, 0xb, "pic_r" },
    { 0x7c, 0x7, 0xc, "pic_bs" },
    { 0x7d, 0x7, 0xd, "pic_alusel" },
    { 0x7e, 0x7, 0xe, "pic_alua" },
    { 0x7f, 0x7, 0xf, "pic_alub" },
    { 0xa0, 0xa, 0x0, "bad" },
    { 0xa1, 0xa, 0x1, "mem_p0" },
    { 0xa2, 0xa, 0x2, "mem_jmpl" },
    { 0xa3, 0xa, 0x3, "mem_jmph" },
    { 0xa4, 0xa, 0x4, "bad" },
    { 0xa5, 0xa, 0x5, "bad" },
    { 0xa6, 0xa, 0x6, "mem_stack" },
    { 0xa7, 0xa, 0x7, "mem_pic" },
    { 0xa8, 0xa, 0x8, "mem_adl" },
    { 0xa9, 0xa, 0x9, "mem_adh" },
    { 0xaa, 0xa, 0xa, "mem_mem" },
    { 0xab, 0xa, 0xb, "mem_r" },
    { 0xac, 0xa, 0xc, "mem_bs" },
    { 0xad, 0xa, 0xd, "mem_alusel" },
    { 0xae, 0xa, 0xe, "mem_alua" },
    { 0xaf, 0xa, 0xf, "mem_alub" },
    { 0xb0, 0xb, 0x0, "bad" },
    { 0xb1, 0xb, 0x1, "r_p0" },
    { 0xb2, 0xb, 0x2, "r_jmpl" },
    { 0xb3, 0xb, 0x3, "r_jmph" },
    { 0xb4, 0xb, 0x4, "bad" },
    { 0xb5, 0xb, 0x5, "bad" },
    { 0xb6, 0xb, 0x6, "r_stack" },
    { 0xb7, 0xb, 0x7, "r_pic" },
    { 0xb8, 0xb, 0x8, "r_adl" },
    { 0xb9, 0xb, 0x9, "r_adh" },
    { 0xba, 0xb, 0xa, "r_mem" },
    { 0xbb, 0xb, 0xb, "r_r" },
    { 0xbc, 0xb, 0xc, "r_bs" },
    { 0xbd, 0xb, 0xd, "r_alusel" },
    { 0xbe, 0xb, 0xe, "r_alua" },
    { 0xbf, 0xb, 0xf, "r_alub" },
    { 0xd0, 0xd, 0x0, "bad" },
    { 0xd1, 0xd, 0x1, "zero_p0" },
    { 0xd2, 0xd, 0x2, "zero_jmpl" },
    { 0xd3, 0xd, 0x3, "zero_jmph" },
    { 0xd4, 0xd, 0x4, "bad" },
    { 0xd5, 0xd, 0x5, "bad" },
    { 0xd6, 0xd, 0x6, "zero_stack" },
    { 0xd7, 0xd, 0x7, "zero_pic" },
    { 0xd8, 0xd, 0x8, "zero_adl" },
    { 0xd9, 0xd, 0x9, "zero_adh" },
    { 0xda, 0xd, 0xa, "zero_mem" },
    { 0xdb, 0xd, 0xb, "zero_r" },
    { 0xdc, 0xd, 0xc, "zero_bs" },
    { 0xdd, 0xd, 0xd, "zero_alusel" },
    { 0xde, 0xd, 0xe, "zero_alua" },
    { 0xdf, 0xd, 0xf, "zero_alub" },
    { 0xe0, 0xe, 0x0, "bad" },
    { 0xe1, 0xe, 0x1, "one_p0" },
    { 0xe2, 0xe, 0x2, "one_jmpl" },
    { 0xe3, 0xe, 0x3, "one_jmph" },
    { 0xe4, 0xe, 0x4, "bad" },
    { 0xe5, 0xe, 0x5, "bad" },
    { 0xe6, 0xe, 0x6, "one_stack" },
    { 0xe7, 0xe, 0x7, "one_pic" },
    { 0xe8, 0xe, 0x8, "one_adl" },
    { 0xe9, 0xe, 0x9, "one_adh" },
    { 0xea, 0xe, 0xa, "one_mem" },
    { 0xeb, 0xe, 0xb, "one_r" },
    { 0xec, 0xe, 0xc, "one_bs" },
    { 0xed, 0xe, 0xd, "one_alusel" },
    { 0xee, 0xe, 0xe, "one_alua" },
    { 0xef, 0xe, 0xf, "one_alub" },
    { 0xf0, 0xf, 0x0, "bad" },
    { 0xf1, 0xf, 0x1, "aluc_p0" },
    { 0xf2, 0xf, 0x2, "aluc_jmpl" },
    { 0xf3, 0xf, 0x3, "aluc_jmph" },
    { 0xf4, 0xf, 0x4, "bad" },
    { 0xf5, 0xf, 0x5, "bad" },
    { 0xf6, 0xf, 0x6, "aluc_stack" },
    { 0xf7, 0xf, 0x7, "aluc_pic" },
    { 0xf8, 0xf, 0x8, "aluc_adl" },
    { 0xf9, 0xf, 0x9, "aluc_adh" },
    { 0xfa, 0xf, 0xa, "aluc_mem" },
    { 0xfb, 0xf, 0xb, "aluc_r" },
    { 0xfc, 0xf, 0xc, "aluc_bs" },
    { 0xfd, 0xf, 0xd, "aluc_alusel" },
    { 0xfe, 0xf, 0xe, "aluc_alua" },
    { 0xff, 0xf, 0xf, "aluc_alub" },
};
