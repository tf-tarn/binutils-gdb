/* Simulator for the ggx processor
   Copyright (C) 2008 Free Software Foundation, Inc.
   Contributed by Anthony Green

This file is part of GDB, the GNU debugger.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#include <signal.h>
#include "sysdep.h"
#include <sys/times.h>
#include <sys/param.h>
#include <netinet/in.h>	/* for byte ordering macros */
#include "bfd.h"
#include "gdb/callback.h"
#include "libiberty.h"
#include "gdb/remote-sim.h"

#include "sim-main.h"
/* #include "sim-base.h" */
/* #include "sim-options.h" */
/* #include "sim-io.h" */

typedef int word;

static unsigned long  heap_ptr = 0;
host_callback *       callback;

unsigned long
tarn_extract_unsigned_integer (addr, len)
     unsigned char * addr;
     int len;
{
  unsigned long retval;
  unsigned char * p;
  unsigned char * startaddr = (unsigned char *)addr;
  unsigned char * endaddr = startaddr + len;

  if (len > (int) sizeof (unsigned long))
    printf ("That operation is not available on integers of more than %d bytes.",
	    sizeof (unsigned long));

  /* Start at the most significant end of the integer, and work towards
     the least significant.  */
  retval = 0;

  for (p = endaddr; p > startaddr;)
    retval = (retval << 8) | * -- p;

  return retval;
}

static void
tarn_store_unsigned_integer (unsigned char *addr, int len, unsigned long val)
{
  unsigned char * p;
  unsigned char * startaddr = (unsigned char *)addr;
  unsigned char * endaddr = startaddr + len;

  for (p = endaddr; p > startaddr;)
    {
      * -- p = val & 0xff;
      val >>= 8;
    }
}

/* The machine state.

   This state is maintained in host byte order.  The fetch/store
   register functions must translate between host byte order and the
   target processor byte order.  Keeping this data in target byte
   order simplifies the register read/write functions.  Keeping this
   data in native order improves the performance of the simulator.
   Simulation speed is deemed more important.  */

#define NUM_TARN_REGS 16

/* /\* The ordering of the tarn_regset structure is matched in the */
/*    gdb/config/tarn/tm-tarn.h file in the REGISTER_NAMES macro.  *\/ */
/* struct tarn_regset */
/* { */
/*   unsigned char	  regs[NUM_TARN_REGS];	/\* primary registers *\/ */
/*   word		  pc;			/\* the program counter *\/ */
/*   int		  exception; */
/*   unsigned long   msize; */
/*   unsigned long   insts;                /\* instruction counter *\/ */
/* }; */

/* union */
/* { */
/*   struct tarn_regset asregs; */
/*   word asints [1];		/\* but accessed larger... *\/ */
/* } _cpu; */

static char *myname;
static SIM_OPEN_KIND sim_kind;
static int issue_messages = 0;

SIM_CPU *cpu;

/* static void */
/* init_pointers () */
/* { */
/*   if (cpu.asregs.msize != (1 << sim_memory_size)) */
/*     sim_size (sim_memory_size); */
/* } */

static void
set_initial_gprs ()
{
  int i;
  long space;
  unsigned long memsize;

  /* Set up machine just out of reset.  */
  cpu->pc = 0;

  /* Clean out the register contents.  */
  for (i = 0; i < NUM_TARN_REGS; i++)
    cpu->registers[i] = 0;
}

static void
interrupt ()
{
  cpu->exception = SIGINT;
}

static int tracing = 1;

const char *from_registers[] = {
    "NOP",
    "P0",
    "INTL",
    "INTH",
    "RETI",
    "IL",
    "STACK",
    "PIC",
    "<bad register number>",
    "<bad register number>",
    "MEM",
    "R",
    "<bad register number>",
    "ZERO",
    "ONE",
    "ALUC"
};

const char *to_registers[] = {
    "NOP",
    "P0",
    "JMPL",
    "JMPH",
    "JUMP",
    "JNZ",
    "STACK",
    "PIC",
    "ADL",
    "ADH",
    "MEM",
    "R",
    "BS",
    "ALUSEL",
    "ALUA",
    "ALUB"
};


/* Read 2 bytes from memory.  */
static INLINE int
rsat (sim_cpu *scpu, unsigned short pc, word x)
{
  address_word cia = CPU_PC_GET (scpu);

  return (sim_core_read_aligned_2 (scpu, cia, read_map, x));
}

void
sim_resume (sd, step, siggnal)
     SIM_DESC sd;
     int step, siggnal;
{
  unsigned short pc, opc;
  unsigned long insts;
  unsigned short inst;
  unsigned char inst_h;
  unsigned char inst_l;
  unsigned char reg_src;
  unsigned char reg_dest;
  void (* sigsave)();

  sigsave = signal (SIGINT, interrupt);
  cpu->exception = step ? SIGTRAP: 0;
  pc = cpu->pc;
  insts = cpu->insts;
  /* Run instructions here. */

  if (tracing)
      printf("# set pc to 0x%04x\n",
             pc);

  do
    {
      opc = pc;
      inst = rsat(cpu, pc, pc);
      inst_h = inst >> 8;
      inst_l = inst & 0xff;

      printf("# at 0x%04x: %02x %02x\n", pc, inst_h, inst_l);

      /* Fetch the instruction at pc.  */
      reg_src = inst_h >> 4;
      reg_dest = inst_h & 0xf;

      // Low instruction byte is loaded into IL (0x5)
      cpu->registers[0x5] = inst_l;


      if (reg_src == 0 && reg_dest == 0) { // NOP
          // do nothing
          printf("# 0x%04x: NOP\n", opc);
          pc += 2;
      } else if (reg_src == 0x4) { // JUMP
          if (reg_dest != 0) {
              cpu->exception = SIGILL;
              break;
          } else {
              pc = (cpu->registers[3] << 8) + cpu->registers[2];
              printf("# 0x%04x: jump to %04x\n",
                     opc, pc);
          }
      } else if (reg_src == 0x5) { // JNZ
          cpu->exception = SIGILL;
          break;
      } else if (reg_dest == 0x4) { // RETI
          cpu->exception = SIGILL;
          break;
      } else {
          // Execute generic instruction.
          // TODO: implement adder and stuff .....
          cpu->registers[reg_dest] = cpu->registers[reg_src];
          printf("# 0x%04x: send %s to %s\n",
                 opc, from_registers[reg_src], to_registers[reg_dest]);
          pc += 2;
      }



      /* /\* Decode instruction.  *\/ */
      /* if (inst & (1 << 15)) */
      /*   { */
      /*     /\* This is a Form 2 instruction.  *\/ */
      /*     /\* We haven't implemented any yet, so just SIGILL for now.  *\/ */
      /*     cpu->exception = SIGILL; */
      /*     break; */
      /*   } */
      /* else */
      /*   { */
      /*     /\* This is a Form 1 instruction.  *\/ */
      /*     int opcode = inst >> 9; */
      /*     switch (opcode) */
      /*       { */
      /*       case 0x00: /\* ld.w (immediate) *\/ */
      /*         { */
      /*   	int reg = (inst >> 6) & 0x7; */

      /*   	unsigned int val = ((memory[pc + 2] << 24) */
      /*   			    + (memory[pc + 3] << 16) */
      /*   			    + (memory[pc + 4] << 8) */
      /*   			    + (memory[pc + 5])); */
      /*   	cpu->registers[reg] = val; */
      /*   	pc += 4; */

      /*   	if (tracing) */
      /*   	  callback->printf_filtered (callback, */
      /*   				     "# 0x%08x: $r%d = 0x%x\n", */
      /*   				     opc, reg, val); */
      /*         } */
      /*         break; */
      /*       case 0x01: /\* mov (register-to-register) *\/ */
      /*         { */
      /*   	int dest  = (inst >> 6) & 0x7; */
      /*   	int src = (inst >> 3) & 0x7; */
      /*   	cpu->registers[dest] = cpu->registers[src]; */

      /*   	if (tracing) */
      /*   	  callback->printf_filtered (callback, */
      /*   				     "# 0x%08x: $r%d = $r%d (0x%x)\n", */
      /*   				     opc, dest, src, cpu->registers[src]); */
      /*         } */
      /*         break; */
      /*       default: */
      /*         cpu->exception = SIGILL; */
      /*         break; */
      /*       } */
      /*   } */

      insts++;

    } while (!cpu->exception);

  /* Hide away the things we've cached while executing.  */
  cpu->pc = pc;
  cpu->insts += insts;		/* instructions done ... */

  signal (SIGINT, sigsave);
}



int
sim_store_register (sd, rn, memory, length)
     SIM_DESC sd;
     int rn;
     unsigned char * memory;
     int length;
{
    //init_pointers ();

  if (rn < NUM_TARN_REGS && rn >= 0)
    {
      if (length == 1)
	{
	  long ival;

	  /* misalignment safe */
	  ival = tarn_extract_unsigned_integer (memory, 1);
          cpu->registers[rn] = ival;
	}

      return 1;
    }
  else
    return 0;
}

int
sim_fetch_register (sd, rn, memory, length)
     SIM_DESC sd;
     int rn;
     unsigned char * memory;
     int length;
{
    //init_pointers ();

  if (rn < NUM_TARN_REGS && rn >= 0)
    {
      if (length == 1)
	{
	  long ival = cpu->registers[rn];

	  /* misalignment-safe */
	  tarn_store_unsigned_integer (memory, 1, ival);
	}

      return 4;
    }
  else
    return 0;
}


int
sim_trace (sd)
     SIM_DESC sd;
{
  tracing = 1;

  sim_resume (sd, 0, 0);

  tracing = 0;

  return 1;
}

#define PARM1 1
#define PARM2 2
#define PARM3 3


static int
tarn_reg_store (SIM_CPU *scpu, int rn, unsigned char *memory, int length)
{
  if (rn < NUM_TARN_REGS && rn >= 0)
    {
      if (length == 1)
	{
	  long ival;

	  /* misalignment safe */
	  ival = tarn_extract_unsigned_integer (memory, 1);
	  cpu->registers[rn] = ival;
	}

      return 1;
    }
  else
    return 0;
}

static int
tarn_reg_fetch (SIM_CPU *scpu, int rn, unsigned char *memory, int length)
{
  if (rn < NUM_TARN_REGS && rn >= 0)
    {
      if (length == 1)
	{
	  long ival = cpu->registers[rn];

	  /* misalignment-safe */
	  tarn_store_unsigned_integer (memory, 1, ival);
	}

      return 1;
    }
  else
    return 0;
}

static sim_cia
tarn_pc_get (sim_cpu *cpu)
{
  return cpu->pc;
}

static void
tarn_pc_set (sim_cpu *cpu, sim_cia pc)
{
    cpu->registers[18] = pc;
}

static void
free_state (SIM_DESC sd)
{
  if (STATE_MODULES (sd) != NULL)
    sim_module_uninstall (sd);
  sim_cpu_free_all (sd);
  sim_state_free (sd);
}

SIM_DESC
sim_open (SIM_OPEN_KIND kind, host_callback *cb,
	  struct bfd *abfd, char * const *argv)
{
  int i;
  SIM_DESC sd = sim_state_alloc (kind, cb);
  SIM_ASSERT (STATE_MAGIC (sd) == SIM_MAGIC_NUMBER);

  /* The cpu data is kept in a separately allocated chunk of memory.  */
  if (sim_cpu_alloc_all (sd, 1, /*cgen_cpu_max_extra_bytes ()*/0) != SIM_RC_OK)
    {
      free_state (sd);
      return 0;
    }

  STATE_WATCHPOINTS (sd)->pc = &cpu->pc;
  STATE_WATCHPOINTS (sd)->sizeof_pc = sizeof (unsigned short);

  if (sim_pre_argv_init (sd, argv[0]) != SIM_RC_OK)
    {
      free_state (sd);
      return 0;
    }

  /* The parser will print an error message for us, so we silently return.  */
  if (sim_parse_args (sd, argv) != SIM_RC_OK)
    {
      free_state (sd);
      return 0;
    }

  // sim_do_command(sd," memory region 0x0000,0x8000") ;
  // sim_do_command(sd," memory region 0xE0000000,0x10000") ;

  /* Check for/establish the a reference program image.  */
  if (sim_analyze_program (sd,
			   (STATE_PROG_ARGV (sd) != NULL
			    ? *STATE_PROG_ARGV (sd)
			    : NULL), abfd) != SIM_RC_OK)
    {
      free_state (sd);
      return 0;
    }

  /* Configure/verify the target byte order and other runtime
     configuration options.  */
  if (sim_config (sd) != SIM_RC_OK)
    {
      sim_module_uninstall (sd);
      return 0;
    }

  if (sim_post_argv_init (sd) != SIM_RC_OK)
    {
      /* Uninstall the modules to avoid memory leaks,
	 file descriptor leaks, etc.  */
      sim_module_uninstall (sd);
      return 0;
    }

  /* CPU specific initialization.  */
  for (i = 0; i < MAX_NR_PROCESSORS; ++i)
    {
      cpu = STATE_CPU (sd, i);

      CPU_REG_FETCH (cpu) = tarn_reg_fetch;
      CPU_REG_STORE (cpu) = tarn_reg_store;
      CPU_PC_FETCH (cpu) = tarn_pc_get;
      CPU_PC_STORE (cpu) = tarn_pc_set;

      set_initial_gprs (cpu);	/* Reset the GPR registers.  */
    }

  return sd;
}


/* Write a 2 byte value to memory.  */
static INLINE void
wsat (sim_cpu *scpu, word pc, word x, word v)
{
  address_word cia = CPU_PC_GET (scpu);

  sim_core_write_aligned_2 (scpu, cia, write_map, x, v);
}

SIM_RC
sim_create_inferior (sd, prog_bfd, argv, env)
     SIM_DESC sd;
     struct bfd * prog_bfd;
     char * const* argv;
     char * const* env;
{
  char ** avp;
  int l;

  /* Set the initial register set.  */
  l = issue_messages;
  issue_messages = 0;
  set_initial_gprs ();
  issue_messages = l;

  int argc, i, tp;
  sim_cpu *scpu = STATE_CPU (sd, 0); /* FIXME */

  if (prog_bfd != NULL) {
      cpu->pc = bfd_get_start_address (prog_bfd);
      printf("start addr is %04x\n", cpu->pc);
  }

  /* /\* Copy args into target memory.  *\/ */
  /* avp = argv; */
  /* for (argc = 0; avp && *avp; avp++) */
  /*     argc++; */

  /* /\* Target memory looks like this: */
  /*    0x00000000 zero word */
  /*    0x00000002 argc word */
  /*    0x00000004 start of argv */
  /*    . */
  /*    0x0000???? end of argv */
  /*    0x0000???? zero word */
  /*    0x0000???? start of data pointed to by argv  *\/ */

  /* wsat (scpu, 0, 0, 0); */
  /* wsat (scpu, 0, 2, argc); */

  /* /\* tp is the offset of our first argv data.  *\/ */
  /* tp = 2 + 2 + argc * 2 + 2; */

  /* for (i = 0; i < argc; i++) */
  /*     { */
  /*         /\* Set the argv value.  *\/ */
  /*         wsat (scpu, 0, 2 + 2 + i * 2, tp); */

  /*         /\* Store the string.  *\/ */
  /*         sim_core_write_buffer (sd, scpu, write_map, argv[i], */
  /*                                tp, strlen(argv[i])+1); */
  /*         tp += strlen (argv[i]) + 1; */
  /*     } */

  /* wsat (scpu, 0, 2 + 2 + i * 2, 0); */

  //  load_dtb (sd, DTB);


  return SIM_RC_OK;
}

void
sim_kill (sd)
     SIM_DESC sd;
{
  /* nothing to do */
}

void
sim_do_command (sd, cmd)
     SIM_DESC sd;
     const char * cmd;
{
  /* Nothing there yet; it's all an error.  */

  if (cmd != NULL)
    {
      char ** simargv = buildargv (cmd);
      if (strcmp (simargv[0], "verbose") == 0)
	{
	  issue_messages = 2;
	}
      else
	{
	  fprintf (stderr,"Error: \"%s\" is not a valid tarn simulator command.\n",
		   cmd);
	}
    }
  else
    {
      fprintf (stderr, "tarn sim commands: \n");
      fprintf (stderr, "  verbose\n");
    }
}

void
sim_set_callbacks (ptr)
     host_callback * ptr;
{
  callback = ptr;
}
