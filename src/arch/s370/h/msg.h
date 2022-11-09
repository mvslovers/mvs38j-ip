/* msg.h -  */

#ifndef H_MSG_H
#define H_MSGC_H		// only generate once

//------------------------------------------------------------------------------msg.h

#define	send(x,y) xsend(x,y)	// MVS-Xinu: Systems/C has send function

SYSCALL receive(void);		// wait for a message and return it
SYSCALL recvclr(void);		// clear messages, returning waiting message (if any)
SYSCALL recvtim(int);		// wait to receive a message or timeout and return result
SYSCALL xsend(int, WORD);	// send a message to another process
SYSCALL	sendf(int, int);	// sendf a message to another process, forcing delivery

// sendn not provided in PC-Xinu or MVS-Xinu

//------------------------------------------------------------------------------msg.h
#endif /* H_MSG_H */
