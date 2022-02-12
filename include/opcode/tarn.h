/* Definitions for decoding the tarn opcode table.
   Copyright 2008 Free Software Foundation, Inc.
   Contributed by Tarn.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston, MA
02110-1301, USA.  */

/*
  Form 1 instructions come in different flavors:

    Some have no arguments              (TARN_F1_NARG)
    Some only use the A operand         (TARN_F1_A)
    Some use A and B registers          (TARN_F1_AB)
    Some use A, B and C registers       (TARN_F1_ABC)

  Form 2 instructions also come in different flavors:

    Some ignore the 12-bit immediate operand (TARN_F2_NARG)
    Some use the 12-bit immediate operand    (TARN_F2_12V)
*/

/*
  Instructions are of the form:
  struct opcode {
    unsigned src_register  : 4;
    unsigned dest_register : 4;
    unsigned literal       : 8; // loaded into IL
  }
 */

typedef struct tarn_opc_info_t
{
    unsigned char opcode;
    unsigned char from : 4;
    unsigned char to : 4;
    const char *name;
} tarn_opc_info_t;

extern const tarn_opc_info_t tarn_opc_info[256];
