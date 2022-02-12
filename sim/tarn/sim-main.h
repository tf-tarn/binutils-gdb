/* Tarn Simulator definition.
   Copyright (C) 2009-2021 Free Software Foundation, Inc.
   Contributed by Anthony Green <green@tarnlogic.com>

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

#ifndef SIM_MAIN_H
#define SIM_MAIN_H

/* Simulator state pseudo baseclass.

   Each simulator is required to have the file ``sim-main.h''.  That
   file includes ``sim-basics.h'', defines the base type ``sim_cia''
   (the data type that contains complete current instruction address
   information), include ``sim-base.h'':

     #include "sim-basics.h"
     /-* If `sim_cia' is not an integral value (e.g. a struct), define
         CIA_ADDR to return the integral value.  *-/
     /-* typedef struct {...} sim_cia; *-/
     /-* #define CIA_ADDR(cia) (...) *-/
     #include "sim-base.h"

   finally, two data types `struct _sim_cpu' and `struct sim_state'
   are defined:

     struct _sim_cpu {
        ... simulator specific members ...
        sim_cpu_base base;
     };

     struct sim_state {
       sim_cpu *cpu[MAX_NR_PROCESSORS];
       ... simulator specific members ...
       sim_state_base base;
     };

   Note that `base' appears last.  This makes `base.magic' appear last
   in the entire struct and helps catch miscompilation errors. */

#include "sim-basics.h"
#include "sim-base.h"
// #include "bfd.h"


#define NUM_TARN_REGS 16

struct _sim_cpu {

    /* The following are internal simulator state variables: */

    /* To keep this default simulator simple, and fast, we use a direct
       vector of registers. The internal simulator engine then uses
       manifests to access the correct slot. */

    unsigned char registers[NUM_TARN_REGS];
    unsigned char pc;
    int exception;
    unsigned long insts;

  sim_cpu_base base;
};

struct sim_state {

  sim_cpu *cpu[MAX_NR_PROCESSORS];

  sim_state_base base;
};

#endif
