/* Example synacor simulator.

   Copyright (C) 2005-2022 Free Software Foundation, Inc.
   Contributed by Mike Frysinger.

   This file is part of simulators.

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

#include "sim-basics.h"
#include "sim-base.h"

#include "opcode/tarn.h"

#define NUM_TARN_REGS 16

struct _sim_cpu {
    /* ... simulator specific members ... */
    uint8_t sregs[TARN_SRC_REG_COUNT];
    uint8_t dregs[TARN_DEST_REG_COUNT];
    uint8_t status;
    sim_cia pc;
    unsigned inst_count;

  /* This isn't a real register, and the stack is not directly addressable,
     so use memory outside of the 16-bit address space.  */

    // also, it doesn't exist at all
    // uint32_t sp;

  sim_cpu_base base;
};

struct sim_state {
    sim_cpu *cpu[MAX_NR_PROCESSORS];

  /* ... simulator specific members ... */
  sim_state_base base;
};


extern void step_once (SIM_CPU *);
extern void initialize_cpu (SIM_DESC, SIM_CPU *);

#endif
