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

const char comment_chars[]        = "#;";
const char line_separator_chars[] = "";
const char line_comment_chars[]   = "#;";

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

////////////////////////////////////////////////////////////////////////////////

const exp_mod_data_t exp_mod_data[] =
{
  /* Default, must be first.  */
  { "", 0, BFD_RELOC_16, "" },
  /* The following are used together with tarn-gcc's __memx address space
     in order to initialize a 24-bit pointer variable with a 24-bit address.
     For address in flash, hlo8 will contain the flash segment if the
     symbol is located in flash. If the symbol is located in RAM; hlo8
     will contain 0x80 which matches tarn-gcc's notion of how 24-bit RAM/flash
     addresses linearize address space.  */
  { "lo8",  1, BFD_RELOC_TARN_8_LO,  "`lo8' "  },
  { "hi8",  1, BFD_RELOC_TARN_8_HI,  "`hi8' "  },
};

#define EXP_MOD_NAME(i)       exp_mod[i].name
#define EXP_MOD_RELOC(i)      exp_mod[i].reloc
#define EXP_MOD_NEG_RELOC(i)  exp_mod[i].neg_reloc
#define HAVE_PM_P(i)          exp_mod[i].have_pm

struct exp_mod_s
{
  const char *                    name;
  bfd_reloc_code_real_type  reloc;
  bfd_reloc_code_real_type  neg_reloc;
  int                       have_pm;
};

static struct exp_mod_s exp_mod[] =
{
  {"hi8",    BFD_RELOC_TARN_8_HI,    BFD_RELOC_TARN_8_HI,    1},
  {"lo8",    BFD_RELOC_TARN_8_LO,    BFD_RELOC_TARN_8_LO,    1},
};

/* A union used to store indices into the exp_mod[] array
   in a hash table which expects void * data types.  */
typedef union
{
  void * ptr;
  int    index;
} mod_index;

/* Reloc modifiers hash control (hh8,hi8,lo8,pm_xx).  */
static htab_t tarn_mod_hash;


static inline char *
skip_space (char *s)
{
  while (*s == ' ' || *s == '\t')
    ++s;
  return s;
}

/* Parse special CONS expression: pm (expression) or alternatively
   gs (expression).  These are used for addressing program memory.  Moreover,
   define lo8 (expression), hi8 (expression) and hlo8 (expression).  */

const exp_mod_data_t *
tarn_parse_cons_expression (expressionS *exp, int nbytes)
{
  char *tmp;
  unsigned int i;

  tmp = input_line_pointer = skip_space (input_line_pointer);

  /* The first entry of exp_mod_data[] contains an entry if no
     expression modifier is present.  Skip it.  */

  fprintf(stderr, "tarn_parse_cons_expression: called\n");

  for (i = 0; i < ARRAY_SIZE (exp_mod_data); i++)
    {
      const exp_mod_data_t *pexp = &exp_mod_data[i];
      int len = strlen (pexp->name);

      fprintf(stderr, "tarn_parse_cons_expression: i=%d nbytes=%d ilp=%s\n", i, pexp->nbytes, input_line_pointer);

      if (nbytes == pexp->nbytes
          && strncasecmp (input_line_pointer, pexp->name, len) == 0)
	{
	  input_line_pointer = skip_space (input_line_pointer + len);

	  if (*input_line_pointer == '(')
	    {
	      input_line_pointer = skip_space (input_line_pointer + 1);
	      expression (exp);

	      if (*input_line_pointer == ')')
		{
		  ++input_line_pointer;
		  return pexp;
		}
	      else
		{
		  as_bad (_("`)' required"));
		  return &exp_mod_data[0];
		}
	    }

	  input_line_pointer = tmp;

          break;
	}
    }

  expression (exp);
  return &exp_mod_data[0];
}

void
tarn_cons_fix_new (fragS *frag,
		  int where,
		  int nbytes,
		  expressionS *exp,
		  const exp_mod_data_t *pexp_mod_data)
{
  int bad = 0;

  switch (pexp_mod_data->reloc)
    {
    default:
      if (nbytes == 1)
	fix_new_exp (frag, where, nbytes, exp, FALSE, BFD_RELOC_8);
      else if (nbytes == 2)
	fix_new_exp (frag, where, nbytes, exp, FALSE, BFD_RELOC_16);
      else if (nbytes == 4)
	fix_new_exp (frag, where, nbytes, exp, FALSE, BFD_RELOC_32);
      else
	bad = 1;
      break;

    case BFD_RELOC_TARN_8_LO:
    case BFD_RELOC_TARN_8_HI:
      if (nbytes == pexp_mod_data->nbytes)
        fix_new_exp (frag, where, nbytes, exp, FALSE, pexp_mod_data->reloc);
      else
        bad = 1;
      break;
    }

  if (bad)
    as_bad (_("illegal %s relocation size: %d"), pexp_mod_data->error, nbytes);
}

////////////////////////////////////////////////////////////////////////////////

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

  const tarn_reg_list_entry_t *entry;
  for (count = 0, entry = tarn_src_registers; count++ < TARN_SRC_REG_COUNT; entry++)
      str_hash_insert (src_reg_hash_control, entry->name, entry, 0);

  for (count = 0, entry = tarn_dest_registers; count++ < TARN_DEST_REG_COUNT; entry++)
      str_hash_insert (dest_reg_hash_control, entry->name, entry, 0);

  tarn_mod_hash = str_htab_create ();

  for (int i = 0; i < ARRAY_SIZE (exp_mod); ++i)
    {
      mod_index m;

      m.index = i + 10;
      str_hash_insert (tarn_mod_hash, EXP_MOD_NAME (i), m.ptr, 0);
    }

  bfd_set_arch_mach (stdoutput, TARGET_ARCH, 0);
}

/* Parse an expression and then restore the input line pointer.  */

static char *
parse_exp_save_ilp (char *s, expressionS *op)
{
  char *save = input_line_pointer;

  fprintf(stderr, "parsing and saving expression \"%s\"\n", s);
  input_line_pointer = s;
  expression (op);
  s = input_line_pointer;
  input_line_pointer = save;
  return s;
}

int check_for_tarn_ops(char *where, char **opname, char **oparg) {
    char *_opname = where;
    while (*where && !ISSPACE(*where) && *where != '(') ++where;
    if (*where != '(') {
        return 0;
    }
    *where = 0;
    ++where;
    char *_oparg = where;
    while (ISSPACE (*where)) ++where;
    while (*where && !ISSPACE(*where) && *where != ')') ++where;
    while (ISSPACE (*where)) ++where;
    if (*where != ')') {
        as_bad (_("missing close parenthesis in operator expression \"%s\""), _oparg);
        return 0;
    }
    *where = 0;
    *opname = _opname;
    *oparg = _oparg;
    return 1;
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

  #if TARN_VERSION==TARN_VERSION_VFUTURE
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
  #endif

  if (opcode->opcode == TARN_MOV) {

      while (ISSPACE (*op_end)) ++op_end;
      op_start = op_end;
      for (; *op_end && !is_end_of_line[*op_end & 0xff] && *op_end != ' '; ++op_end)
          nlen++;

      pend = *op_end;
      *op_end = 0;
      tarn_reg_list_entry_t* reg2 = (tarn_reg_list_entry_t *) str_hash_find(dest_reg_hash_control, op_start);

      if (reg2 == NULL) {
          as_bad (_("unknown dest register %s (%p, %lu)"), op_start, op_start, op_end - op_start);
          *op_end = pend;
          return;
      }
      *op_end = pend;

      while (ISSPACE (*op_end)) ++op_end;
      op_start = op_end;
      for (; *op_end && !is_end_of_line[*op_end & 0xff] && *op_end != ' ' && *op_end != ','; ++op_end)
          nlen++;

      pend = *op_end;
      *op_end = 0;
      tarn_reg_list_entry_t* reg1 = (tarn_reg_list_entry_t *) str_hash_find(src_reg_hash_control, op_start);

      if (reg1 == NULL) {
          as_bad (_("unknown source register %s (%p, %lu)"), op_start, op_start, op_end - op_start);
          *op_end = pend;
          return;
      }
      *op_end = pend;

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

          if (*op_end == ',') {
              ++op_end;

              while (ISSPACE (*op_end)) ++op_end;
              char *opname = NULL, *oparg = NULL;
              where = frag_more (1);
              if (check_for_tarn_ops(op_end, &opname, &oparg)) {
                  bfd_reloc_code_real_type r_type;

                  mod_index m;

                  m.ptr = str_hash_find (tarn_mod_hash, opname);
                  int mod = m.index;
                  if (mod) {
                      mod -= 10;
                      parse_exp_save_ilp(oparg, &arg);
                      fix_new_exp (frag_now,
                                   (where - frag_now->fr_literal),
                                   1,
                                   &arg,
                                   0,
                                   EXP_MOD_RELOC (mod));
                  } else {
                      as_bad(_("unknown pseudo-operator \"%s\""), opname);
                  }
              } else {
                  parse_exp_save_ilp(op_end, &arg);
                  fix_new_exp (frag_now,
                               (where - frag_now->fr_literal),
                               1,
                               &arg,
                               0,
                               BFD_RELOC_8);
              }
          } else {
              if (is_end_of_line[*op_end & 0xff]) {
                  // An unstated IL value is taken to be zero.
                  p = frag_more (1);
                  md_number_to_chars(p, 0, 1);
                  ignore_rest_of_line ();
                  return;
              }
              as_bad (_("need comma followed by IL value %s (%p, %lu)"), op_start, op_start, op_end - op_start);
              ignore_rest_of_line ();
              return;
          }

      }
      ignore_rest_of_line ();
      return;
  }

  if (*op_end != 0)
    as_warn ("extra stuff on line ignored");

  if (pending_reloc)
    as_bad ("Something forgot to clean up\n");

  as_bad (_("unhandled opcode %s (%p, %lu)"), op_start, op_start, op_end - op_start);
  ignore_rest_of_line ();
  return;

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
  unsigned long insn;
  unsigned char *where;

  max = min = 0;
  /* switch (fixP->fx_r_type) */
  /*   { */
  /*   case BFD_RELOC_8: */
  /*     *buf++ = val; */
  /*     break; */

  /*   default: */
  /*     abort (); */
  /*   } */

  if (max != 0 && (val < min || val > max))
    as_bad_where (fixP->fx_file, fixP->fx_line, _("offset out of range"));

  if (fixP->fx_addsy == NULL && fixP->fx_pcrel == 0)
    fixP->fx_done = 1;

  where = (unsigned char *) fixP->fx_frag->fr_literal + fixP->fx_where;
  switch (fixP->fx_r_type) {
  default:
      fixP->fx_no_overflow = 1;
      break;
    case BFD_RELOC_32:
    case BFD_RELOC_16:
      break;
  }

  if (fixP->fx_done) {
      /* Fetch the instruction, insert the fully resolved operand
         value, and stuff the instruction back again.  */
      where = (unsigned char *) fixP->fx_frag->fr_literal + fixP->fx_where;
      insn = bfd_getl16 (where);

      switch (fixP->fx_r_type) {
      case BFD_RELOC_32:
	  bfd_putl32 ((bfd_vma) val, where);
	  break;

      case BFD_RELOC_16:
	  bfd_putl16 ((bfd_vma) val, where);
	  break;

      case BFD_RELOC_8:
          if (val > 255 || val < -128)
              as_warn_where (fixP->fx_file, fixP->fx_line,
                             _("operand out of range: %ld"), val);
          *where = val;
	  break;


      case BFD_RELOC_TARN_8_LO:
          *where = 0xff & val;
          break;

      case BFD_RELOC_TARN_8_HI:
          *where = 0xff & (val >> 8);
          break;

      default:
          as_fatal (_("line %d: unknown relocation type: 0x%x"),
                    fixP->fx_line, fixP->fx_r_type);
          break;
      }
  }

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
