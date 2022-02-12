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
    const tarn_opc_info_t *opcode;
    bfd_byte buffer[2];
    unsigned short iword;
    fpr = info->fprintf_func;

    if ((status = info->read_memory_func (addr, buffer, 2, info)))
        goto fail;
    iword = bfd_getb16(buffer);

    opcode = &tarn_opc_info[iword >> 8];
    fpr (stream, "%s\t0x%02x", opcode->name, iword & 0xff);

    return 2; // returns length of the instruction

 fail:
    info->memory_error_func (status, addr, info);
    return -1;
}
