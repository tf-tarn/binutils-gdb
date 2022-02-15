/* gcore ELF support for BFD.
   Copyright 2008 Free Software Foundation, Inc.

   This file is part of BFD, the Binary File Descriptor library.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street - Fifth Floor, Boston, MA 02110-1301, USA.  */

#ifndef _ELF_TARN_H
#define _ELF_TARN_H

#include "elf/reloc-macros.h"

/* Relocation types.  */
START_RELOC_NUMBERS (elf_tarn_reloc_type)
  RELOC_NUMBER (R_TARN_NONE,  0)
  RELOC_NUMBER (R_TARN_DIR8,  1)
  RELOC_NUMBER (R_TARN_8_LO8, 2)
  RELOC_NUMBER (R_TARN_8_HI8, 3)
END_RELOC_NUMBERS (R_TARN_max)

#endif
