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

#define TARN_VERSION_V23     23
#define TARN_VERSION_V29     29
#define TARN_VERSION_VFUTURE 666

#define TARN_VERSION TARN_VERSION_V23

typedef struct tarn_opc_info_t
{
    unsigned char opcode;
    int fixed;
    const char *name;
} tarn_opc_info_t;

typedef struct {
    char num;
    char *name;
} tarn_reg_list_entry_t;


#if TARN_VERSION==TARN_VERSION_VFUTURE

#pragma message "tarn version future"

#define TARN_OPC_COUNT 5

#define TARN_NOP  0
#define TARN_JUMP 1
#define TARN_JNZ  2
#define TARN_RETI 3
#define TARN_MOV  4

#define TARN_SRC_REG_COUNT 11
#define TARN_DEST_REG_COUNT 13

#elif TARN_VERSION==TARN_VERSION_V29

#error "Unsupported tarn version 2.9"

#elif TARN_VERSION==TARN_VERSION_V23

#define TARN_OPC_COUNT 3

#define TARN_NOP  0
#define TARN_JUMP 1
#define TARN_MOV  2

#define TARN_SRC_REG_COUNT 16
#define TARN_DEST_REG_COUNT 16

#define TARN_SREG_NOP          0
#define TARN_SREG_ARC          1
#define TARN_SREG_STATUS       2
#define TARN_SREG_IHMEM        3
#define TARN_SREG_P0           4
#define TARN_SREG_CR_INC       5
#define TARN_SREG_CR           6
#define TARN_SREG_R            7
#define TARN_SREG_PC_INC       8
#define TARN_SREG_JUMP         9
#define TARN_SREG_IH          10
#define TARN_SREG_IL          11
#define TARN_SREG_MEM         12
#define TARN_SREG_P1          13
#define TARN_SREG_ZERO        14
#define TARN_SREG_ONE         15

#define TARN_DREG_NOP        0
#define TARN_DREG_P0         1
#define TARN_DREG_STATUS     2
#define TARN_DREG_JMPH       3
#define TARN_DREG_JMPL       4
#define TARN_DREG_CR         5
#define TARN_DREG_DO_CREMENT 6
#define TARN_DREG_R          7
#define TARN_DREG_ADH        8
#define TARN_DREG_ADL        9
#define TARN_DREG_IH         10
#define TARN_DREG_IL         11
#define TARN_DREG_MEM        12
#define TARN_DREG_P1         13
#define TARN_DREG_ARA        14
#define TARN_DREG_ARB        15


#else

#error "Unsupported tarn version"

#endif


extern const tarn_opc_info_t tarn_opc_info[TARN_OPC_COUNT];
extern const tarn_reg_list_entry_t tarn_src_registers[TARN_SRC_REG_COUNT];
extern const tarn_reg_list_entry_t tarn_dest_registers[TARN_DEST_REG_COUNT];
