/* Target-dependent definitions for SuperH running NetBSD, for GDB.
   Copyright 2002 Free Software Foundation, Inc.
   Contributed by Wasabi Systems, Inc.

   This file is part of GDB.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

#ifndef SH_NBSD_TDEP_H
#define SH_NBSD_TDEP_H

void	sh_nbsd_supply_registers (char *);
void	sh_nbsd_supply_register (char *, int);

void	sh_nbsd_fill_registers (char *);
void	sh_nbsd_fill_register (char *, int);

#endif /* SH_NBSD_TDEP_H */
