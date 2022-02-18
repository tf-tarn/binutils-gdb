/* Disassemble tarn instructions.
   Copyright 2008
   Free Software Foundation, Inc.

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
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston,
   MA 02110-1301, USA.  */


#include "sysdep.h"
#include <stdio.h>
#define STATIC_TABLE
#define DEFINE_TABLE

#include "opcode/tarn.h"
#include "dis-asm.h"

static fprintf_ftype fpr;
static void *stream;

int
print_insn_tarn (bfd_vma addr, struct disassemble_info *info)
{
    int status;
    stream = info->stream;
    const tarn_opc_info_t *opcode = NULL;
    bfd_byte buffer[2];
    unsigned short iword;
    fpr = info->fprintf_func;

    if ((status = info->read_memory_func (addr, buffer, 2, info)))
        goto fail;
    iword = bfd_getb16(buffer);


    /*
  if (opcode->inst_id == TARN_NOP) {
      // Allocate space in the fragment for the opcode.
      p = frag_more (2);
      md_number_to_chars(p, 0, 2);
      return;
  }

  if (opcode->inst_id == TARN_JUMP) {
      // Allocate space in the fragment for the opcode.
      p = frag_more (2);
      md_number_to_chars(p, 0x0400, 2);
      return;
  }

  if (opcode->inst_id == TARN_JNZ) {
      // Allocate space in the fragment for the opcode.
      p = frag_more (2);
      md_number_to_chars(p, 0x0500, 2);
      return;
  }

  if (opcode->inst_id == TARN_RETI) {
      // Allocate space in the fragment for the opcode.
      p = frag_more (2);
      md_number_to_chars(p, 0x4000, 2);
      return;
  }

     */

    for (int i = 0; i < TARN_OPC_COUNT; ++i) {
        if (tarn_opc_info[i].fixed >= 0 && iword == tarn_opc_info[i].fixed) {
            opcode = &tarn_opc_info[i];
            break;
        }
    }

    if (!opcode) {
        opcode = &tarn_opc_info[TARN_OPC_COUNT - 1];
        unsigned char sreg = (iword >> 8) >> 4;
        unsigned char dreg = (iword >> 8) & 0xf;
        const char *sreg_name = NULL;
        const char *dreg_name = NULL;
        for (int i = 0; i < TARN_SRC_REG_COUNT; ++i) {
            if (sreg == tarn_src_registers[i].num) {
                sreg_name = tarn_src_registers[i].name;
                break;
            }
        }
        for (int i = 0; i < TARN_DEST_REG_COUNT; ++i) {
            if (dreg == tarn_dest_registers[i].num) {
                dreg_name = tarn_dest_registers[i].name;
                break;
            }
        }
        if (!sreg_name || !dreg_name) {
            if (!sreg_name)
                sreg_name = "(?)";
            if (!dreg_name)
                dreg_name = "(?)";
            fpr (stream, "%s\t%s\t%s\t,0x%02x\t\t# Bad instruction.",
                 opcode->name, dreg_name, sreg_name, iword & 0xff);
        } else {
            fpr (stream, "%s\t%s\t%s\t,0x%02x",
                 opcode->name, dreg_name, sreg_name, iword & 0xff);
        }
    } else {
        fpr (stream, "%s", opcode->name);
    }

    return 2; // returns length of the instruction

 fail:
    info->memory_error_func (status, addr, info);
    return -1;
}
