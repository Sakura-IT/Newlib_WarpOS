/*
 * $Id$
 *
 * Adapted for NewLib3 by Dennis van der Boon in 2019
 */

#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>

#pragma pack(push,2)
#include <powerpc/powerpc_protos.h>
#pragma pack(pop)

#define MAGIC_COOKIE 0x07041776

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
  size = nbytes + align + 32;
  a = AllocPooledPPC(__mempool, size);
  if (!a)
    return a;
  ptr = (void*)((ULONG)a + align+31 & -align);

  char *b = ptr;
  int *memloc = (int*)(&b[-4]);
  int *memsize = (int*)(&b[-8]);
  int *cookie = (int*)(&b[-12]);
  *cookie = (ULONG)MAGIC_COOKIE;
  *memloc = (ULONG)a;
  *memsize = (ULONG)size;			//Original WOS saves size. MOS emulation of
  return ptr;					//WOS apparently does not...
}


void free(void *ptr)
{
  if ((ptr != NULL))
  {
    void* a;
    char *b = ptr;
    ULONG c;
    int *cookie = (int*)(&b[-12]);
    if (*cookie != MAGIC_COOKIE)
    	asm(".long 0\n"); //Memory corruption
    int *memloc = (int*)(&b[-4]);
    int *memsize = (int*)(&b[-8]);
    a = (void*)*memloc;
    c = (ULONG)*memsize;
    FreePooledPPC(__mempool, a, c);
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

  __asm__ (".section .init \n .long __initmalloc, 110 \n .section .text\n");
  __asm__ (".section .fini \n .long __exitmalloc, 110 \n .section .text\n");
