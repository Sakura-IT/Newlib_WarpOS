/*
 * Copyright (c) 1990 The Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by the University of California, Berkeley.  The name of the
 * University may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */
/* doc in sprintf.c */

#include <_ansi.h>
#include <reent.h>
#include <stdio.h>
#include <stdarg.h>
#include "local.h"

#ifdef WARPUP
#pragma pack(push,2)
#include <powerpc/powerpc_protos.h>
#include <powerpc/semaphoresPPC.h>
#pragma pack(pop)

static struct SignalSemaphorePPC write_sem;
#endif

int
_printf_r (struct _reent *ptr,
       const char *__restrict fmt, ...)
{
  int ret;
  va_list ap;

  _REENT_SMALL_CHECK_INIT (ptr);
  va_start (ap, fmt);
  ret = _vfprintf_r (ptr, _stdout_r (ptr), fmt, ap);
  va_end (ap);
  return ret;
}

#ifdef _NANO_FORMATTED_IO
int
_iprintf_r (struct _reent *, const char *, ...)
       _ATTRIBUTE ((__alias__("_printf_r")));
#endif

#ifndef _REENT_ONLY

int
printf (const char *__restrict fmt, ...)
{
  int ret;
  va_list ap;
#ifdef WARPUP
  ObtainSemaphorePPC(&write_sem);
#endif
  struct _reent *ptr = _REENT;

  _REENT_SMALL_CHECK_INIT (ptr);
  va_start (ap, fmt);
  ret = _vfprintf_r (ptr, _stdout_r (ptr), fmt, ap);
  va_end (ap);
#ifdef WARPUP
  ReleaseSemaphorePPC(&write_sem);
#endif
  return ret;
}

#ifdef WARPUP
void __init_sem()
{
  InitSemaphorePPC(&write_sem);
}

void __exit_sem()
{
  FreeSemaphorePPC(&write_sem);
}

  __asm__ (".section .init \n .long __init_sem, 140 \n .section .text\n");
  __asm__ (".section .fini \n .long __exit_sem, 140 \n .section .text\n");
#endif

#ifdef _NANO_FORMATTED_IO
int
iprintf (const char *, ...)
       _ATTRIBUTE ((__alias__("printf")));
#endif
#endif /* ! _REENT_ONLY */
