/* Stack executability handling for GNU dynamic linker.  Stub version.
   Copyright (C) 2003, 2004 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307 USA.  */

#include <ldsodefs.h>
#include <errno.h>
#include <sys/mman.h>
#include <unistd.h>
#include <ros/memlayout.h>

/* There is no portable way to know the bounds of the initial thread's stack
   so as to mprotect it.  */

int
internal_function
_dl_make_stack_executable (void **stack_endp)
{
  *stack_endp = 0;
  GL(dl_stack_flags) |= PF_X;

  int size = USTACK_NUM_PAGES*PGSIZE;
  void *bottom = USTACKTOP-size;

  return mprotect(bottom,size,PROT_READ|PROT_WRITE|PROT_EXEC);
}
rtld_hidden_def (_dl_make_stack_executable)
