/*******************************************************
 * wischk: the "wise" checkers playing program         *
 * Copyright (C) 1993  John J. Wiseman                 *
 * See file COPYING for license and (lack of) warranty *
 *******************************************************/

#include <stdlib.h>
#ifdef __MSDOS__
#include <io.h>
#else
#include <unistd.h>
#endif
#ifdef MEMDEBUG
#include <string.h>
#include <new.h>
#include <stdio.h>

static char rcsid[] = "$Id: memhand.cc,v 2.0 1993/11/14 17:52:11 jjw Rel $";

static char memdbg_buffer[30];

/*
 	Structure of the following was taken from Effective C++, by
Scott Meyers, P. 25.
 */

typedef void (*PEHF)();

void* operator new(size_t size)
{
  static unsigned num_allocs;
  void *a;

  num_allocs++;
  while (1) {
    if (a = malloc(size)) {
      // allocated memory
      sprintf(memdbg_buffer, "* AL %5d %p *\n", size, a);
      write(2, memdbg_buffer, strlen(memdbg_buffer));
      return a;
    }

    // unable to allocate memory, call function set with
    // set_new_handler(), if any, until 
    PEHF current_handler = set_new_handler(0);
    set_new_handler(current_handler);
    if (current_handler)
      (*current_handler)();
    else
      return 0;
  }
}
    

void operator delete(void *a)
{
  sprintf(memdbg_buffer, "* DE %p *\n", a);
  write(2, memdbg_buffer, strlen(memdbg_buffer));
  free(a);
}

#endif


// This gets called if we run out of memory anywhere.
// hopefully cerr will be in good enough shape to use
void out_of_memory()
{
  write(2, "OUT OF MEMORY\n ", 15);
  abort();
}
