/* tc-tarn.h -- Header file for tc-tarn.c.

   Copyright 2008 Free Software Foundation, Inc.

   This file is part of GAS, the GNU Assembler.

   GAS is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   GAS is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along
   with GAS; see the file COPYING.  If not, write to the Free Software
   Foundation, 51 Franklin Street - Fifth Floor, Boston, MA 02110-1301, USA.  */

#define TC_TARN 1
#define TARGET_BYTES_BIG_ENDIAN 1
#define WORKING_DOT_WORD

/* This macro is the BFD target name to use when creating the output
   file.  This will normally depend upon the `OBJ_FMT' macro.  */
#define TARGET_FORMAT "elf32-tarn"

/* This macro is the BFD architecture to pass to `bfd_set_arch_mach'.  */
#define TARGET_ARCH bfd_arch_tarn

////////////////////////////////////////////////////////////////////////////////

typedef struct
{
  /* Name of the expression modifier allowed with .byte, .word, etc.  */
  const char *name;

  /* Only allowed with n bytes of data.  */
  int nbytes;

  /* Associated RELOC.  */
  bfd_reloc_code_real_type reloc;

  /* Part of the error message.  */
  const char *error;
} exp_mod_data_t;

extern const exp_mod_data_t exp_mod_data[];
#define TC_PARSE_CONS_RETURN_TYPE const exp_mod_data_t *
#define TC_PARSE_CONS_RETURN_NONE exp_mod_data

/* You may define this macro to parse an expression used in a data
   allocation pseudo-op such as `.word'.  You can use this to
   recognize relocation directives that may appear in such directives.  */
#define TC_PARSE_CONS_EXPRESSION(EXPR,N) tarn_parse_cons_expression (EXPR, N)
extern const exp_mod_data_t *tarn_parse_cons_expression (expressionS *, int);

/* We don't want gas to fixup the following program memory related relocations.
   We will need them in case that we want to do linker relaxation.
   We could in principle keep these fixups in gas when not relaxing.
   However, there is no serious performance penalty when making the linker
   make the fixup work.  Check also that fx_addsy is not NULL, in order to make
   sure that the fixup refers to some sort of label.  */
#define TC_VALIDATE_FIX(FIXP,SEG,SKIP)                       \
  if (   (FIXP->fx_r_type == BFD_RELOC_TARN_8_LO             \
       || FIXP->fx_r_type == BFD_RELOC_TARN_8_HI)           \
      && FIXP->fx_addsy != NULL				     \
      && FIXP->fx_subsy == NULL)			     \
    {							     \
      symbol_mark_used_in_reloc (FIXP->fx_addsy);	     \
      goto SKIP;					     \
    }

/* You may define this macro to generate a fixup for a data
   allocation pseudo-op.  */
#define TC_CONS_FIX_NEW tarn_cons_fix_new
extern void tarn_cons_fix_new (fragS *,int, int, expressionS *,
			      const exp_mod_data_t *);

////////////////////////////////////////////////////////////////////////////////

#define md_undefined_symbol(NAME)           0

/* These macros must be defined, but is will be a fatal assembler
   error if we ever hit them.  */
#define md_estimate_size_before_relax(A, B) (as_fatal (_("estimate size\n")),0)
#define md_convert_frag(B, S, F)            (as_fatal (_("convert_frag\n")), 0)

/* If you define this macro, it should return the offset between the
   address of a PC relative fixup and the position from which the PC
   relative adjustment should be made.  On many processors, the base
   of a PC relative instruction is the next instruction, so this
   macro would return the length of an instruction.  */
#define MD_PCREL_FROM_SECTION(FIX, SEC) md_pcrel_from (FIX)
extern long md_pcrel_from (struct fix *);


#define md_section_align(SEGMENT, SIZE)     (SIZE)

// This makes local symbols relocatable with lo8(), hi8()
#define tc_fix_adjustable(fixp) tc_tarn_fix_adjustable(fixp)
