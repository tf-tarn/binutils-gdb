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
  The tarn processor's 16-bit instructions come in two forms:

  FORM 1 instructions start with a 0 bit...

    0ooooooaaabbbccc
    0              F

   oooooo - form 1 opcode number
   aaa    - operand A
   bbb    - operand B
   ccc    - operand C

  FORM 2 instructions start with a 1 bit...

    1ooovvvvvvvvvvvv
    0              F

   ooo          - form 2 opcode number
   vvvvvvvvvvvv - 12-bit immediate value

*/

const tarn_opc_info_t tarn_form1_opc_info[64] =
  {
    { 0x00, TARN_F1_NARG, "bad" },
    { 0x01, TARN_F1_NARG, "bad" },
    { 0x02, TARN_F1_NARG, "bad" },
    { 0x03, TARN_F1_NARG, "bad" },
    { 0x04, TARN_F1_NARG, "bad" },
    { 0x05, TARN_F1_NARG, "bad" },
    { 0x06, TARN_F1_NARG, "bad" },
    { 0x07, TARN_F1_NARG, "bad" },
    { 0x08, TARN_F1_NARG, "bad" },
    { 0x09, TARN_F1_NARG, "bad" },
    { 0x0a, TARN_F1_NARG, "bad" },
    { 0x0b, TARN_F1_NARG, "bad" },
    { 0x0c, TARN_F1_NARG, "bad" },
    { 0x0d, TARN_F1_NARG, "bad" },
    { 0x0e, TARN_F1_NARG, "bad" },
    { 0x0f, TARN_F1_NARG, "bad" },
    { 0x10, TARN_F1_NARG, "bad" },
    { 0x11, TARN_F1_NARG, "bad" },
    { 0x12, TARN_F1_NARG, "bad" },
    { 0x13, TARN_F1_NARG, "bad" },
    { 0x14, TARN_F1_NARG, "bad" },
    { 0x15, TARN_F1_NARG, "bad" },
    { 0x16, TARN_F1_NARG, "bad" },
    { 0x17, TARN_F1_NARG, "bad" },
    { 0x18, TARN_F1_NARG, "bad" },
    { 0x19, TARN_F1_NARG, "bad" },
    { 0x1a, TARN_F1_NARG, "bad" },
    { 0x1b, TARN_F1_NARG, "bad" },
    { 0x1c, TARN_F1_NARG, "bad" },
    { 0x1d, TARN_F1_NARG, "bad" },
    { 0x1e, TARN_F1_NARG, "bad" },
    { 0x1f, TARN_F1_NARG, "bad" },
    { 0x20, TARN_F1_NARG, "bad" },
    { 0x21, TARN_F1_NARG, "bad" },
    { 0x22, TARN_F1_NARG, "bad" },
    { 0x23, TARN_F1_NARG, "bad" },
    { 0x24, TARN_F1_NARG, "bad" },
    { 0x25, TARN_F1_NARG, "bad" },
    { 0x26, TARN_F1_NARG, "bad" },
    { 0x27, TARN_F1_NARG, "bad" },
    { 0x28, TARN_F1_NARG, "bad" },
    { 0x29, TARN_F1_NARG, "bad" },
    { 0x2a, TARN_F1_NARG, "bad" },
    { 0x2b, TARN_F1_NARG, "bad" },
    { 0x2c, TARN_F1_NARG, "bad" },
    { 0x2d, TARN_F1_NARG, "bad" },
    { 0x2e, TARN_F1_NARG, "bad" },
    { 0x2f, TARN_F1_NARG, "bad" },
    { 0x30, TARN_F1_NARG, "bad" },
    { 0x31, TARN_F1_NARG, "bad" },
    { 0x32, TARN_F1_NARG, "bad" },
    { 0x33, TARN_F1_NARG, "bad" },
    { 0x34, TARN_F1_NARG, "bad" },
    { 0x35, TARN_F1_NARG, "bad" },
    { 0x36, TARN_F1_NARG, "bad" },
    { 0x37, TARN_F1_NARG, "bad" },
    { 0x38, TARN_F1_NARG, "bad" },
    { 0x39, TARN_F1_NARG, "bad" },
    { 0x3a, TARN_F1_NARG, "bad" },
    { 0x3b, TARN_F1_NARG, "bad" },
    { 0x3c, TARN_F1_NARG, "bad" },
    { 0x3d, TARN_F1_NARG, "bad" },
    { 0x3e, TARN_F1_NARG, "bad" },
    { 0x3f, TARN_F1_NARG, "bad" }
  };

const tarn_opc_info_t tarn_form2_opc_info[8] =
  {
    { 0x01, TARN_F2_NARG, "bad" },
    { 0x01, TARN_F2_NARG, "bad" },
    { 0x02, TARN_F2_NARG, "bad" },
    { 0x03, TARN_F2_NARG, "bad" },
    { 0x04, TARN_F2_NARG, "bad" },
    { 0x05, TARN_F2_NARG, "bad" },
    { 0x06, TARN_F2_NARG, "bad" },
    { 0x07, TARN_F2_NARG, "bad" }
  };
