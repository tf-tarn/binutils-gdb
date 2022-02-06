/* tarn support for 32-bit ELF
   Copyright 2008 Free Software Foundation, Inc.

   This file is part of BFD, the Binary File Descriptor library.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston,
   MA 02110-1301, USA.  */

#include "sysdep.h"
#include "bfd.h"
#include "libbfd.h"
#include "elf-bfd.h"

#include "elf/tarn.h"

/* This function is used for normal relocs.  This is like the COFF
   function, and is almost certainly incorrect for other ELF targets.  */

static bfd_reloc_status_type
tarn_elf_reloc (bfd *abfd,
		 arelent *reloc_entry,
		 asymbol *symbol_in,
		 void * data,
		 asection *input_section,
		 bfd *output_bfd,
		 char **error_message ATTRIBUTE_UNUSED)
{
  unsigned long insn;
  bfd_vma sym_value;
  enum elf_tarn_reloc_type r_type;
  bfd_vma addr = reloc_entry->address;
  bfd_byte *hit_data = addr + (bfd_byte *) data;

  r_type = (enum elf_tarn_reloc_type) reloc_entry->howto->type;

  if (output_bfd != NULL)
    {
      /* Partial linking--do nothing.  */
      reloc_entry->address += input_section->output_offset;
      return bfd_reloc_ok;
    }

  if (symbol_in != NULL
      && bfd_is_und_section (symbol_in->section))
    return bfd_reloc_undefined;

  if (bfd_is_com_section (symbol_in->section))
    sym_value = 0;
  else
    sym_value = (symbol_in->value +
		 symbol_in->section->output_section->vma +
		 symbol_in->section->output_offset);

  switch (r_type)
    {
    case R_TARN_DIR8:
      insn = bfd_get_8 (abfd, hit_data);
      insn += sym_value + reloc_entry->addend;
      bfd_put_8 (abfd, (bfd_vma) insn, hit_data);
      break;

    default:
      abort ();
      break;
    }

  return bfd_reloc_ok;
}

static reloc_howto_type tarn_elf_howto_table[] =
{
  /* No relocation.  */
  HOWTO (R_TARN_NONE,		/* type */
	 0,			/* rightshift */
	 0,			/* size (0 = byte, 1 = short, 2 = long) */
	 0,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 tarn_elf_reloc,	        /* special_function */
	 "R_TARN_NONE",	        /* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0,			/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* 8 bit absolute relocation.  Setting partial_inplace to TRUE and
     src_mask to a non-zero value is similar to the COFF toolchain.  */
  HOWTO (R_TARN_DIR8,	        /* type */
	 0,			/* rightshift */
	 0,			/* size (0 = byte, 1 = short, 2 = long) */
	 8,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_bitfield, /* complain_on_overflow */
	 tarn_elf_reloc,		/* special_function */
	 "R_TARN_DIR8",		/* name */
	 TRUE,			/* partial_inplace */
	 0xff,		/* src_mask */
	 0xff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* lo8-part to use in  .byte lo8(sym).  */
  HOWTO (R_TARN_8_LO8,		/* type */
	 0,			/* rightshift */
	 0,			/* size (0 = byte, 1 = short, 2 = long) */
	 8,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_TARN_8_LO8",		/* name */
	 FALSE,			/* partial_inplace */
	 0xffffff,		/* src_mask */
	 0xffffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */
  /* hi8-part to use in  .byte hi8(sym).  */
  HOWTO (R_TARN_8_HI8,		/* type */
	 8,			/* rightshift */
	 0,			/* size (0 = byte, 1 = short, 2 = long) */
	 8,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_TARN_8_HI8",		/* name */
	 FALSE,			/* partial_inplace */
	 0xffffff,		/* src_mask */
	 0xffffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */
};

/* This structure is used to map BFD reloc codes to tarn elf relocs.  */

struct elf_reloc_map
{
  bfd_reloc_code_real_type bfd_reloc_val;
  unsigned char elf_reloc_val;
};

/* An array mapping BFD reloc codes to tarn elf relocs.  */

static const struct elf_reloc_map tarn_reloc_map[] =
{
    { BFD_RELOC_NONE,      R_TARN_NONE },
    { BFD_RELOC_8,         R_TARN_DIR8 },
    { BFD_RELOC_TARN_8_LO, R_TARN_8_LO8 },
    { BFD_RELOC_TARN_8_HI, R_TARN_8_HI8 },
};

/* Given a BFD reloc code, return the howto structure for the
   corresponding tarn elf reloc.  */

static reloc_howto_type *
tarn_elf_reloc_type_lookup (bfd *abfd ATTRIBUTE_UNUSED,
			     bfd_reloc_code_real_type code)
{
  unsigned int i;

  for (i = 0; i < sizeof (tarn_reloc_map) / sizeof (struct elf_reloc_map); i++)
    if (tarn_reloc_map[i].bfd_reloc_val == code)
      return & tarn_elf_howto_table[(int) tarn_reloc_map[i].elf_reloc_val];

  return NULL;
}

static reloc_howto_type *
tarn_elf_reloc_name_lookup (bfd *abfd ATTRIBUTE_UNUSED,
			  const char *r_name)
{
  unsigned int i;

  for (i = 0;
       i < sizeof (tarn_elf_howto_table) / sizeof (tarn_elf_howto_table[0]);
       i++)
    if (tarn_elf_howto_table[i].name != NULL
	&& strcasecmp (tarn_elf_howto_table[i].name, r_name) == 0)
      return &tarn_elf_howto_table[i];

  return NULL;
}

/* Given an ELF reloc, fill in the howto field of a relent.  */

static void
tarn_elf_info_to_howto (bfd *abfd ATTRIBUTE_UNUSED,
		      arelent *cache_ptr,
		      Elf_Internal_Rela *dst)
{
  unsigned int r;

  r = ELF32_R_TYPE (dst->r_info);

  BFD_ASSERT (r < (unsigned int) R_TARN_max);

  cache_ptr->howto = &tarn_elf_howto_table[r];
}

#define ELF_ARCH		bfd_arch_tarn
#define ELF_MACHINE_CODE	EM_TARN
#define ELF_MAXPAGESIZE  	1

#define TARGET_BIG_SYM		tarn_elf32_vec
#define TARGET_BIG_NAME		"elf32-tarn"

#define elf_info_to_howto		tarn_elf_info_to_howto
#define bfd_elf32_bfd_reloc_type_lookup tarn_elf_reloc_type_lookup
#define bfd_elf32_bfd_reloc_name_lookup tarn_elf_reloc_name_lookup

#include "elf32-target.h"
