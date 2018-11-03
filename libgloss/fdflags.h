/*
 * $Id$
 *
 * Adapted for NewLib3 by Dennis van der Boon in 2018
 */


#ifndef FILEDESC_FLAGS
#define FILEDESC_FLAGS

#define FDFL_STDIO            (1<<0)  /* never close */
#define FDFL_INTERACTIVE      (1<<1)  /* tty */
#define FDFL_DIRLOCK          (1<<2)  /* file is a lock, not a handle! */
#define FDFL_SOCKET           (1<<3)  /* file is a socket fd (AmiTCP) */
#define FDFL_MODESET          (1<<4)  /* try to set mode on close() */
#define FDFL_FLOCKED          (1<<5)  /* locked by flock() */

#endif