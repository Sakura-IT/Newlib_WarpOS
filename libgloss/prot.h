/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2003,2006
 *
 * $Id$
 *
 * Adapted for NewLib3 by Dennis van der Boon in 2018
 */


#include <sys/types.h>

mode_t __prot2mode(unsigned long);
unsigned long __mode2prot(mode_t);
int __set_prot(char *,mode_t);
int __set_masked_prot(char *,mode_t);
mode_t __get_masked_prot(mode_t);
