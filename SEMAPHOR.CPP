/****************************************************************/
/* Unit : Semaphore						*/
/* Author : Dawit Bekele					*/
/* Date : 04/11/99						*/
/* 	Creates and enables to manipulate three semaphores	*/
/*	called FIRST, SECOND and THIRD				*/
/*								*/
/*								*/
/* How to use:							*/
/*	1- The three semaphores do not need declaration. They 	*/
/*	   are automatically defined and their names are always	*/
/*	   FIRST, SECOND and THIRD. However, you have to	*/
/*         initialize the three semaphores using:		*/
/*	      initialise_sem (int first, int second, int third) */
/*	   where first, second and third are the initial values */
/*	   of FIRST, SECOND and THIRD semaphores respectively.	*/
/*	   If you need to use only some of the semaphores you   */
/*	   still have to initialise the others with any		*/
/*	   arbitrary values.					*/
/*								*/
/*	2- The function:					*/
/*	       up (int semname)					*/
/*	   where semname is either FIRST, SECOND or THIRD	*/
/*         enables to do an up of the specified semaphore	*/
/*								*/
/*	3- The function:					*/
/*	       down (int semname)				*/
/*	   where semname is either FIRST, SECOND or THIRD	*/
/*         enables to do a down of the specified semaphore	*/
/*								*/
/*	4- You should close the semaphores after using them by  */
/*		close_sem ();					*/
/*								*/
/*	   The last process that uses the semaphores should	*/
/*	   close the semaphores. DONT FORGET TO CLOSE.		*/ 
/*	   Since if you do forget, the system resource that is	*/
/*	   allocated for the semaphore will not be deallocated.	*/
/*	   After some time UNIX will be unable to create any new*/
/*	   semaphore for the users of the system.		*/
/*	   If you execute a program that does not deallocate	*/
/*	   the resource properly use the following shell 	*/
/*	   commands to deallocate the resources:		*/
/*	   	ipcs	displays not yet deallocated resources  */
/*		ipcrm sem semid		removes the sempahore	*/
/*					having the id semid	*/
/***************************************************************/

		
/****************************************************************/

#include <stdio.h>    //inter-process coomunication headers for semaphores (all 5)
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdlib.h>
       #if defined(__GNU_LIBRARY__) && !defined(_SEM_SEMUN_UNDEFINED)
       /* union semun is defined by including <sys/sem.h> */
       #else
       /* according to X/OPEN we have to define it ourselves */
       union semun {
               int val;                    /* value for SETVAL */
               struct semid_ds *buf;       /* buffer for IPC_STAT, IPC_SET */
               unsigned short int *array;  /* array for GETALL, SETALL */
               struct seminfo *__buf;      /* buffer for IPC_INFO */
       };
       #endif  

#define MAINERROR 1
#define NSEMS 3
#define FIRST 0    //semaphores are Internally stored in an array
#define SECOND 1
#define THIRD 2

int mysemid, sem_number, shmid;   //mysemid = Id of semaphore set (created by the kernel)
ushort semarray [NSEMS];
union semun semvals;    //semvals = to set/get Semaphore valuess

/* Initialises the three semaphore structures */
void initialise_sem (int first, int second, int third)
{

   /* no buffering used */
   setbuf (stdout, NULL);   //prints immediately and makes the outputs unbuffered
  
   /* create the semaphores */
   if ((mysemid = semget (IPC_PRIVATE, NSEMS, 0644))== -1)  //asks kernel to create a new set of 3 semaphores. (1, N, 0). -1 means failure
   {
      perror ("semget failed");
      exit (MAINERROR);
   }
   semarray [FIRST] = first;
   semarray [SECOND] = second;
   semarray [THIRD] = third;

   semvals.array = semarray;

   //semctl(..., SETALL,...) applies the initial values to all 3 semaphores

   if (semctl (mysemid, 0, SETALL, semvals) == -1)   
   {
      perror ("semctl SETALL failed");
      exit (MAINERROR);
   }
   else
      if (semctl (mysemid, 0, GETALL, semvals)==-1)
   {
      perror ("semctl GETALL failed");
      exit (MAINERROR);
   }
#ifdef DEBUG
    else
   {
   printf ("semval of FIRST = %d \n", semarray [FIRST]);
   printf ("semval of SECOND = %d \n", semarray [SECOND]);
   printf ("semval of THIRD = %d \n", semarray [THIRD]);
   }
#endif
}

/* Destroys the semaphore structure using IPC_RMID*/
void close_sem ()
{
  semvals.val = 0;

  if (semctl (mysemid, 0, IPC_RMID, semvals) == -1)
   {
      perror ("semctl IPC_RMID failed");
      exit (MAINERROR);
   }
#ifdef DEBUG
   else
      printf ("semid removed");
#endif
}

/* Up should be used with FIRST, SECOND or THIRD as parameter */
void up (int sem)
{
struct sembuf sops [1];

   sops[0].sem_num = sem;  //which semaphore 0, 1, 2
   sops[0].sem_op = 1;  //+1 increment
   sops[0].sem_flg = 0;
   if (semop (mysemid, sops, 1)== -1)
      printf ("error in up");
      if (semctl (mysemid, 0, GETALL, semvals)==-1)
   {
      perror ("semctl GETALL failed");
      exit (MAINERROR);
   }

}

/* Down should be used with FIRST, SECOND or THIRD as parameter */
void down (int sem)
{

struct sembuf sops [1];

      if (semctl (mysemid, 0, GETALL, semvals)==-1)
   {
      perror ("semctl GETALL failed");
      exit (MAINERROR);
   }
   sops[0].sem_num = sem;
   sops[0].sem_op = -1;  //-1 decrement
   sops[0].sem_flg = 0;
   if (semop (mysemid, sops, 1) == -1)  //if value becomes negatice, process sleeps / nothing to consume
      printf ("get semop failed");
   if (semctl (mysemid, 0, GETALL, semvals)==-1)
   {
      perror ("semctl GETALL failed");
      exit (MAINERROR);
   }
}
