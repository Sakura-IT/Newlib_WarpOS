/*
 * $Id$
 *
 * Adapted for NewLib3 by Dennis van der Boon in 2018
 */


#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>

#pragma pack(2)
#include <powerpc/powerpc_protos.h>
#pragma pack()

static void *__mempool;

void *memamiga(size_t align, size_t nbytes)
{
  void *a = NULL;
  void *ptr;
  size_t size;

  if (align >= 0 && align <32)
    align = 32;
  if ((align & -align) != align)
    return NULL;
  size = nbytes + align + 4;
  a = AllocPooledPPC(__mempool, size);
  ptr = (void*)((ULONG)a + align+3 & -align);
  char *b = ptr;
  int *memloc = (int*)(&b[-4]);
  *memloc = (ULONG)a;
  return ptr;
}


void free(void *ptr)
{
  if (ptr != NULL)
  {
    void* a;
    char *b = ptr;
    int *memloc = (int*)(&b[-4]);
    a = (void*)*memloc;
    FreePooledPPC(__mempool, a, 0);
  }
}


void __initmalloc(void)
{
	__mempool = CreatePoolPPC(MEMF_ANY, 0x100000, 0x80000);
}


void __exitmalloc(void)
{
	DeletePoolPPC(__mempool);
}

  __asm__ (".section .init \n .long __initmalloc, -55 \n .section .text\n");
  __asm__ (".section .fini \n .long __exitmalloc, -55 \n .section .text\n");
