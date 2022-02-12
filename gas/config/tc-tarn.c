/* tc-tarn.c -- Assemble code for tarn
   Copyright 2008
   Free Software Foundation, Inc.

   This file is part of GAS, the GNU Assembler.

   GAS is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   GAS is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GAS; see the file COPYING.  If not, write to
   the Free Software Foundation, 51 Franklin Street - Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* Contributed by Anthony Green <green@spindazzle.org>.  */

#include "as.h"
#include "safe-ctype.h"
#include "opcode/tarn.h"

#include <assert.h>

// extern const tarn_opc_info_t tarn_opc_info[256];

const char comment_chars[]        = "#";
const char line_separator_chars[] = ";";
const char line_comment_chars[]   = "#";

static int pending_reloc;
static htab_t opcode_hash_control;
static htab_t src_reg_hash_control;
static htab_t dest_reg_hash_control;

const pseudo_typeS md_pseudo_table[] =
{
  {0, 0, 0}
};

const char FLT_CHARS[] = "rRsSfFdDxXpP";
const char EXP_CHARS[] = "eE";

void
md_operand (expressionS *op __attribute__((unused)))
{
  /* Empty for now. */
}

/* This function is called once, at assembler startup time.  It sets
   up the hash table with all the opcodes in it, and also initializes
   some aliases for compatibility with other assemblers.  */

void
md_begin (void)
{
  int count;
  const tarn_opc_info_t *opcode;
  opcode_hash_control = str_htab_create ();
  src_reg_hash_control = str_htab_create ();
  dest_reg_hash_control = str_htab_create ();

  /* Insert names into hash table.  */
  for (count = 0, opcode = tarn_opc_info; count++ < TARN_OPC_COUNT; opcode++)
      str_hash_insert (opcode_hash_control, opcode->name, opcode, 0);

  tarn_reg_list_entry_t *entry;
  for (count = 0, entry = tarn_src_registers; count++ < TARN_SRC_REG_COUNT; entry++)
      str_hash_insert (src_reg_hash_control, entry->name, entry, 0);

  for (count = 0, entry = tarn_dest_registers; count++ < TARN_DEST_REG_COUNT; entry++)
      str_hash_insert (dest_reg_hash_control, entry->name, entry, 0);

  bfd_set_arch_mach (stdoutput, TARGET_ARCH, 0);
}

/* Parse an expression and then restore the input line pointer.  */

static char *
parse_exp_save_ilp (char *s, expressionS *op)
{
  char *save = input_line_pointer;

  input_line_pointer = s;
  expression (op);
  s = input_line_pointer;
  input_line_pointer = save;
  return s;
}

/* This is the guts of the machine-dependent assembler.  STR points to
   a machine dependent instruction.  This function is supposed to emit
   the frags/bytes it assembles to.  */

void
md_assemble (char *str)
{
  char *op_start;
  char *op_end;

  tarn_opc_info_t *opcode;
  char *p;
  char pend;

  int nlen = 0;

  /* Drop leading whitespace.  */
  while (*str == ' ')
    str++;

  /* Find the op code end.  */
  op_start = str;
  for (op_end = str; *op_end && !is_end_of_line[*op_end & 0xff] && *op_end != ' '; ++op_end)
    nlen++;

  pend = *op_end;
  *op_end = 0;

  if (nlen == 0)
    as_bad (_("can't find opcode "));

  opcode = (tarn_opc_info_t *) str_hash_find (opcode_hash_control, op_start);
  *op_end = pend;

  if (opcode == NULL) {
      as_bad (_("unknown opcode %s (%p, %lu)"), op_start, op_start, op_end - op_start);
      return;
  }

  if (opcode->opcode == TARN_NOP) {
      // Allocate space in the fragment for the opcode.
      p = frag_more (2);
      md_number_to_chars(p, 0, 2);
      return;
  }

  if (opcode->opcode == TARN_JUMP) {
      // Allocate space in the fragment for the opcode.
      p = frag_more (2);
      md_number_to_chars(p, 0x0400, 2);
      return;
  }

  if (opcode->opcode == TARN_JNZ) {
      // Allocate space in the fragment for the opcode.
      p = frag_more (2);
      md_number_to_chars(p, 0x0500, 2);
      return;
  }

  if (opcode->opcode == TARN_RETI) {
      // Allocate space in the fragment for the opcode.
      p = frag_more (2);
      md_number_to_chars(p, 0x4000, 2);
      return;
  }

  if (opcode->opcode == TARN_MOV) {

      while (ISSPACE (*op_end)) ++op_end;
      op_start = op_end;
      for (; *op_end && !is_end_of_line[*op_end & 0xff] && *op_end != ' '; ++op_end)
          nlen++;

      pend = *op_end;
      *op_end = 0;
      tarn_reg_list_entry_t* reg1 = (tarn_reg_list_entry_t *) str_hash_find(src_reg_hash_control, op_start);
      *op_end = pend;

      if (reg1 == NULL) {
          as_bad (_("unknown source register %s (%p, %lu)"), op_start, op_start, op_end - op_start);
          return;
      }

      while (ISSPACE (*op_end)) ++op_end;
      op_start = op_end;
      for (; *op_end && !is_end_of_line[*op_end & 0xff] && *op_end != ' ' && *op_end != ','; ++op_end)
          nlen++;

      pend = *op_end;
      *op_end = 0;
      tarn_reg_list_entry_t* reg2 = (tarn_reg_list_entry_t *) str_hash_find(dest_reg_hash_control, op_start);
      *op_end = pend;

      if (reg2 == NULL) {
          as_bad (_("unknown dest register %s (%p, %lu)"), op_start, op_start, op_end - op_start);
          return;
      }

      // Allocate space in the fragment for the opcode.
      p = frag_more (1);

      // Whatever the argument is, we know the opcode, so store that in
      // the current fragment.
      char inst = (reg1->num << 4) + reg2->num;
      md_number_to_chars(p, inst, 1);

      {
          // uh, save expression as a fixup/reloc?
          expressionS arg;
          char *where;

          while (ISSPACE (*op_end)) ++op_end;

          if (*op_end != ',') {
              as_bad (_("need comma followed by IL value %s (%p, %lu)"), op_start, op_start, op_end - op_start);
              ignore_rest_of_line ();
              return;
          }
          ++op_end;

          while (ISSPACE (*op_end)) ++op_end;

          op_end = parse_exp_save_ilp(op_end, &arg);
          where = frag_more (1);
          fix_new_exp (frag_now,
                       (where - frag_now->fr_literal),
                       1,
                       &arg,
                       0,
                       BFD_RELOC_8);
      }

      while (ISSPACE (*op_end)) ++op_end;

      return;
  }

  as_bad (_("unhandled opcode %s (%p, %lu)"), op_start, op_start, op_end - op_start);
  ignore_rest_of_line ();
  return;

  if (*op_end != 0)
    as_warn ("extra stuff on line ignored");

  if (pending_reloc)
    as_bad ("Something forgot to clean up\n");
}

/* Turn a string in input_line_pointer into a floating point constant
   of type type, and store the appropriate bytes in *LITP.  The number
   of LITTLENUMS emitted is stored in *SIZEP .  An error message is
   returned, or NULL on OK.  */

const char *md_atof (int type, char *litP, int *sizeP)
{
  int prec;
  LITTLENUM_TYPE words[4];
  char *t;
  int i;

  switch (type)
    {
    case 'f':
      prec = 2;
      break;

    case 'd':
      prec = 4;
      break;

    default:
      *sizeP = 0;
      return _("bad call to md_atof");
    }

  t = atof_ieee (input_line_pointer, type, words);
  if (t)
    input_line_pointer = t;

  *sizeP = prec * 2;

  for (i = prec - 1; i >= 0; i--)
    {
      md_number_to_chars (litP, (valueT) words[i], 2);
      litP += 2;
    }

  return NULL;
}

const char *md_shortopts = "";

struct option md_longopts[] =
{
  {NULL, no_argument, NULL, 0}
};
size_t md_longopts_size = sizeof (md_longopts);

/* We have no target specific options yet, so these next
   two functions are empty.  */
int md_parse_option (int c ATTRIBUTE_UNUSED, const char *arg ATTRIBUTE_UNUSED)
{
  return 0;
}

void
md_show_usage (FILE *stream ATTRIBUTE_UNUSED)
{
}

/* Apply a fixup to the object file.  */

void
md_apply_fix (fixS *fixP ATTRIBUTE_UNUSED, valueT * valP ATTRIBUTE_UNUSED, segT seg ATTRIBUTE_UNUSED)
{
  char *buf = fixP->fx_where + fixP->fx_frag->fr_literal;
  long val = *valP;
  long max, min;

  max = min = 0;
  switch (fixP->fx_r_type)
    {
    case BFD_RELOC_8:
      *buf++ = val;
      break;

    default:
      abort ();
    }

  if (max != 0 && (val < min || val > max))
    as_bad_where (fixP->fx_file, fixP->fx_line, _("offset out of range"));

  if (fixP->fx_addsy == NULL && fixP->fx_pcrel == 0)
    fixP->fx_done = 1;
}

/* Put number into target byte order (big endian).  */

void
md_number_to_chars (char *ptr, valueT use, int nbytes)
{
  number_to_chars_bigendian (ptr, use, nbytes);
}

/* Translate internal representation of relocation info to BFD target
   format.  */

arelent *
tc_gen_reloc (asection *section ATTRIBUTE_UNUSED, fixS *fixp)
{
  arelent *rel;
  bfd_reloc_code_real_type r_type;

  rel = xmalloc (sizeof (arelent));
  rel->sym_ptr_ptr = xmalloc (sizeof (asymbol *));
  *rel->sym_ptr_ptr = symbol_get_bfdsym (fixp->fx_addsy);
  rel->address = fixp->fx_frag->fr_address + fixp->fx_where;

  r_type = fixp->fx_r_type;
  rel->addend = fixp->fx_addnumber;
  rel->howto = bfd_reloc_type_lookup (stdoutput, r_type);

  if (rel->howto == NULL)
    {
      as_bad_where (fixp->fx_file, fixp->fx_line,
		    _("Cannot represent relocation type %s"),
		    bfd_get_reloc_code_name (r_type));
      /* Set howto to a garbage value so that we can keep going.  */
      rel->howto = bfd_reloc_type_lookup (stdoutput, BFD_RELOC_32);
      assert (rel->howto != NULL);
    }

  return rel;
}


/* Decide from what point a pc-relative relocation is relative to,
   relative to the pc-relative fixup.  Er, relatively speaking.  */
long
md_pcrel_from (fixS *fixP)
{
  valueT addr = fixP->fx_where + fixP->fx_frag->fr_address;

  fprintf (stderr, "md_pcrel_from 0x%d\n", fixP->fx_r_type);

  switch (fixP->fx_r_type)
    {
    case BFD_RELOC_8:
      return addr + 1;
    default:
      abort();
      return addr;
    }
}
