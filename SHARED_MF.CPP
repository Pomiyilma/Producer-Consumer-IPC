
/********************************************************/
/* Unit : Shared buffer					*/
/* Author : Dawit Bekele				*/
/* Date : 04/11/99					*/
/*							*/ 
/*	Defines a buffer of integer of size NBUFS that  */
/*	can be shared between several processes		*/
/*	The buffer will be called BUFFER		*/
/*							*/
/* How to use:						*/
/*	The BUFFER is an array of int type elements 	*/
/*	and its size should be initialized by the parent*/
/*      process before the creation of the processes    */
/*      that use the buffer. The initialization function*/
/*      is:						*/
/*		initialise_buffer (int buffer_size);	*/
/*	The buffer can only be used by related processes*/
/*	(ex. parent and child). 			*/
/*	The buffer should then be destroyed by the last	*/
/*	process that uses it using:			*/
/*		close_buffer ();			*/
/********************************************************/

#include <stdio.h>  //for perror()
#include <sys/shm.h>  //system v shared memory functions (shmget, shmat, shmctl)

int *BUFFER, *shmptr, shmid2; 
//global variables. *BUFFER = points to the start of buffer, *shmptr = temporary pointer returned by shmat, shmid2 = shared mem segment ID
 
/* Initiliases the buffer. The size of the buffer nbuf should be given */
void initialise_buffer (int nbuf)
{
if ((shmid2 = shmget (IPC_PRIVATE, nbuf * sizeof (int) , 0644))== -1) //shmget asks the kernel to create a new shared mem segment
   perror ("shmget failed");   //if its id is -1 it failed

//if ((int) (shmptr = (int *) shmat (shmid2, 0, 0)) == -1)
if ((int ) *(shmptr = (int *) shmat (shmid2, 0, 0)) == -1) 
   perror ("shmat failed");

BUFFER = shmptr;
}

/* Destroys the buffer */
void close_buffer ()
{
if (shmctl (shmid2, IPC_RMID, 0) == -1)
   perror ("shmctl IPC_RMID failed");
}
