/* kprintf.c - kprintf */

#include <conf.h>
#include <kernel.h>
#include <stdarg.h>

/*------------------------------------------------------------------------
 *  kprintf  --  kernel printf: formatted, unbuffered output to STDERR
 *------------------------------------------------------------------------
 */
void
kprintf(const char *fmt, ...)
{
	va_list		args;

	va_start(args, fmt);
        doprnt(fmt, args);
	va_end(args);
}

