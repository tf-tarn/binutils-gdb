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
  unsigned short iword;
  fpr = info->fprintf_func;

  if ((status = info->read_memory_func (addr, &iword, 2, info)))
    goto fail;

  /* Form 1 instructions have the high bit set to 0.  */
  if ((iword & (1<<15)) == 0)
    {
      /* Extract the Form 1 opcode.  */
      opcode = &tarn_form1_opc_info[iword >> 9];
      switch (opcode->itype)
	{
	case TARN_F1_NARG:
	  fpr (stream, "%s", opcode->name);
	  break;
	default:
	  abort();
	}
    }
  else /* this is a Form 2 instruction.  */
    {
      /* Extract the Form 2 opcode.  */
      opcode = &tarn_form2_opc_info[(iword >> 12) & 7];
      switch (opcode->itype)
	{
	case TARN_F2_NARG:
	  fpr (stream, "%s", opcode->name);
	  break;
	default:
	  abort();
	}
    }
 return 2;

 fail:
  info->memory_error_func (status, addr, info);
  return -1;
}
