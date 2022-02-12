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

void
tarn_store_unsigned_integer (addr, len, val)
     unsigned char * addr;
     int len;
     unsigned long val;
{
  unsigned char * p;
  unsigned char * startaddr = (unsigned char *)addr;
  unsigned char * endaddr = startaddr + len;

  for (p = startaddr; p < endaddr;)
    {
      * p ++ = val & 0xff;
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

#define NUM_TARN_REGS 8

/* The ordering of the tarn_regset structure is matched in the
   gdb/config/tarn/tm-tarn.h file in the REGISTER_NAMES macro.  */
struct tarn_regset
{
  word		  regs[NUM_TARN_REGS];	/* primary registers */
  word		  pc;			/* the program counter */
  int		  exception;
  unsigned long   msize;
  unsigned char * memory;
  unsigned long   insts;                /* instruction counter */
};

union
{
  struct tarn_regset asregs;
  word asints [1];		/* but accessed larger... */
} cpu;

static char *myname;
static SIM_OPEN_KIND sim_kind;
static int issue_messages = 0;

/* Default to a 8 Mbyte (== 2^23) memory space.  */
static int sim_memory_size = 23;

#define	MEM_SIZE_FLOOR	64
void
sim_size (power)
     int power;
{
  sim_memory_size = power;
  cpu.asregs.msize = 1 << sim_memory_size;

  if (cpu.asregs.memory)
    free (cpu.asregs.memory);

  /* Watch out for the '0 count' problem. There's probably a better
     way.. e.g., why do we use 64 here?  */
  if (cpu.asregs.msize < 64)	/* Ensure a boundary.  */
    cpu.asregs.memory = (unsigned char *) calloc (64, (64 + cpu.asregs.msize) / 64);
  else
    cpu.asregs.memory = (unsigned char *) calloc (64, cpu.asregs.msize / 64);

  if (!cpu.asregs.memory)
    {
      if (issue_messages)
	fprintf (stderr,
		 "Not enough VM for simulation of %d bytes of RAM\n",
		 cpu.asregs.msize);

      cpu.asregs.msize = 1;
      cpu.asregs.memory = (unsigned char *) calloc (1, 1);
    }
}

static void
init_pointers ()
{
  if (cpu.asregs.msize != (1 << sim_memory_size))
    sim_size (sim_memory_size);
}


static void
set_initial_gprs ()
{
  int i;
  long space;
  unsigned long memsize;

  init_pointers ();

  /* Set up machine just out of reset.  */
  cpu.asregs.pc = 0;

  memsize = cpu.asregs.msize / (1024 * 1024);

  if (issue_messages > 1)
    fprintf (stderr, "Simulated memory of %d Mbytes (0x0 .. 0x%08x)\n",
	     memsize, cpu.asregs.msize - 1);

  /* Clean out the register contents.  */
  for (i = 0; i < NUM_TARN_REGS; i++)
    cpu.asregs.regs[i] = 0;
}

static void
interrupt ()
{
  cpu.asregs.exception = SIGINT;
}

static int tracing = 0;

void
sim_resume (sd, step, siggnal)
     SIM_DESC sd;
     int step, siggnal;
{
  word pc, opc;
  unsigned long insts;
  unsigned short inst;
  void (* sigsave)();

  sigsave = signal (SIGINT, interrupt);
  cpu.asregs.exception = step ? SIGTRAP: 0;
  pc = cpu.asregs.pc;
  insts = cpu.asregs.insts;
  unsigned char *memory = cpu.asregs.memory;

  /* Run instructions here. */

  if (tracing)
    callback->printf_filtered (callback, "# set pc to 0x%x\n",
			       pc);

  do
    {
      opc = pc;

      /* Fetch the instruction at pc.  */
      inst = (memory[pc] << 8) + memory[pc + 1];

      /* Decode instruction.  */
      if (inst & (1 << 15))
	{
	  /* This is a Form 2 instruction.  */
	  /* We haven't implemented any yet, so just SIGILL for now.  */
	  cpu.asregs.exception = SIGILL;
	  break;
	}
      else
	{
	  /* This is a Form 1 instruction.  */
	  int opcode = inst >> 9;
	  switch (opcode)
	    {
	    case 0x00: /* ld.w (immediate) */
	      {
		int reg = (inst >> 6) & 0x7;

		unsigned int val = ((memory[pc + 2] << 24)
				    + (memory[pc + 3] << 16)
				    + (memory[pc + 4] << 8)
				    + (memory[pc + 5]));
		cpu.asregs.regs[reg] = val;
		pc += 4;

		if (tracing)
		  callback->printf_filtered (callback,
					     "# 0x%08x: $r%d = 0x%x\n",
					     opc, reg, val);
	      }
	      break;
	    case 0x01: /* mov (register-to-register) */
	      {
		int dest  = (inst >> 6) & 0x7;
		int src = (inst >> 3) & 0x7;
		cpu.asregs.regs[dest] = cpu.asregs.regs[src];

		if (tracing)
		  callback->printf_filtered (callback,
					     "# 0x%08x: $r%d = $r%d (0x%x)\n",
					     opc, dest, src, cpu.asregs.regs[src]);
	      }
	      break;
	    default:
	      cpu.asregs.exception = SIGILL;
	      break;
	    }
	}

      insts++;
      pc += 2;

    } while (!cpu.asregs.exception);

  /* Hide away the things we've cached while executing.  */
  cpu.asregs.pc = pc;
  cpu.asregs.insts += insts;		/* instructions done ... */

  signal (SIGINT, sigsave);
}

int
sim_write (sd, addr, buffer, size)
     SIM_DESC sd;
     SIM_ADDR addr;
     unsigned char * buffer;
     int size;
{
  int i;
  init_pointers ();

  memcpy (& cpu.asregs.memory[addr], buffer, size);

  return size;
}

int
sim_read (sd, addr, buffer, size)
     SIM_DESC sd;
     SIM_ADDR addr;
     unsigned char * buffer;
     int size;
{
  int i;
  init_pointers ();

  memcpy (buffer, & cpu.asregs.memory[addr], size);

  return size;
}


int
sim_store_register (sd, rn, memory, length)
     SIM_DESC sd;
     int rn;
     unsigned char * memory;
     int length;
{
  init_pointers ();

  if (rn < NUM_TARN_REGS && rn >= 0)
    {
      if (length == 4)
	{
	  long ival;

	  /* misalignment safe */
	  ival = tarn_extract_unsigned_integer (memory, 4);
	  cpu.asints[rn] = ival;
	}

      return 4;
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
  init_pointers ();

  if (rn < NUM_TARN_REGS && rn >= 0)
    {
      if (length == 4)
	{
	  long ival = cpu.asints[rn];

	  /* misalignment-safe */
	  tarn_store_unsigned_integer (memory, 4, ival);
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

void
sim_stop_reason (sd, reason, sigrc)
     SIM_DESC sd;
     enum sim_stop * reason;
     int * sigrc;
{
  if (cpu.asregs.exception == SIGQUIT)
    {
      * reason = sim_exited;
      * sigrc = cpu.asregs.regs[PARM1];
    }
  else
    {
      * reason = sim_stopped;
      * sigrc = cpu.asregs.exception;
    }
}


int
sim_stop (sd)
     SIM_DESC sd;
{
  cpu.asregs.exception = SIGINT;
  return 1;
}


void
sim_info (sd, verbose)
     SIM_DESC sd;
     int verbose;
{
  callback->printf_filtered (callback, "\n\n# instructions executed  %10d\n",
			     cpu.asregs.insts);
}


SIM_DESC
sim_open (kind, cb, abfd, argv)
     SIM_OPEN_KIND kind;
     host_callback * cb;
     struct bfd * abfd;
     char ** argv;
{
  int osize = sim_memory_size;
  myname = argv[0];
  callback = cb;

  if (kind == SIM_OPEN_STANDALONE)
    issue_messages = 1;

  /* Discard and reacquire memory -- start with a clean slate.  */
  sim_size (1);		/* small */
  sim_size (osize);	/* and back again */

  set_initial_gprs ();	/* Reset the GPR registers.  */

  /* Fudge our descriptor for now.  */
  return (SIM_DESC) 1;
}

void
sim_close (sd, quitting)
     SIM_DESC sd;
     int quitting;
{
  /* nothing to do */
}

SIM_RC
sim_load (sd, prog, abfd, from_tty)
     SIM_DESC sd;
     char * prog;
     bfd * abfd;
     int from_tty;
{

  /* Do the right thing for ELF executables; this turns out to be
     just about the right thing for any object format that:
       - we crack using BFD routines
       - follows the traditional UNIX text/data/bss layout
       - calls the bss section ".bss".   */

  extern bfd * sim_load_file (); /* ??? Don't know where this should live.  */
  bfd * prog_bfd;

  {
    bfd * handle;
    asection * s_bss;
    handle = bfd_openr (prog, 0);	/* could be "tarn" */

    if (!handle)
      {
	printf("``%s'' could not be opened.\n", prog);
	return SIM_RC_FAIL;
      }

    /* Makes sure that we have an object file, also cleans gets the
       section headers in place.  */
    if (!bfd_check_format (handle, bfd_object))
      {
	/* wasn't an object file */
	bfd_close (handle);
	printf ("``%s'' is not appropriate object file.\n", prog);
	return SIM_RC_FAIL;
      }

    /* Look for that bss section.  */
    s_bss = bfd_get_section_by_name (handle, ".bss");

    if (!s_bss)
      {
	printf("``%s'' has no bss section.\n", prog);
	return SIM_RC_FAIL;
      }

    /* Appropriately paranoid would check that we have
       a traditional text/data/bss ordering within memory.  */

    /* figure the end of the bss section */
    heap_ptr = ((unsigned long) bfd_get_section_vma (handle, s_bss)
		+ (unsigned long) bfd_section_size (handle, s_bss));

    /* Clean up after ourselves.  */
    bfd_close (handle);

    /* XXX: do we need to free the s_bss and handle structures? */
  }

  /* from sh -- dac */
  prog_bfd = sim_load_file (sd, myname, callback, prog, abfd,
                            sim_kind == SIM_OPEN_DEBUG,
                            0, sim_write);
  if (prog_bfd == NULL)
    return SIM_RC_FAIL;

  if (abfd == NULL)
    bfd_close (prog_bfd);

  return SIM_RC_OK;
}

SIM_RC
sim_create_inferior (sd, prog_bfd, argv, env)
     SIM_DESC sd;
     struct bfd * prog_bfd;
     char ** argv;
     char ** env;
{
  char ** avp;
  int l;

  /* Set the initial register set.  */
  l = issue_messages;
  issue_messages = 0;
  set_initial_gprs ();
  issue_messages = l;

  cpu.asregs.pc = bfd_get_start_address (prog_bfd);

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
     char * cmd;
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
