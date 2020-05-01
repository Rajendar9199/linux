#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <string.h>

#define MY_KEY 19920809
#define SHM_SIZE 0x1000

void toggleCase(char *buf,int cnt);

union semun {
     int              val;    /* Value for SETVAL */
     struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
     unsigned short  *array;  /* Array for GETALL, SETALL */
     struct seminfo  *__buf;  /* Buffer for IPC_INFO
                                           (Linux specific) */
};
union semun sem_union; /* Used internally by the kernel */
int main()
{
	int semId, shmId;
	char *pShm;
	struct sembuf smop;
	
	/*Create a semaphore set, containing two semaphores*/

	semId = semget(MY_KEY, 2, 0660|IPC_CREAT);
	if(semId < 0)
	{	
		printf("Could not create semaphore\n");
		return (1);
	}
	else
		printf("opened a semaphore ID is %d\n",semId);
	
	/*Set initial token count of both semaphores to zeros */
	semctl(semId, 0, SETVAL, 0);
	semctl(semId, 1, SETVAL, 0);
	
	/*Create a shared memory segment*/
	shmId = shmget(MY_KEY, SHM_SIZE, 0660 | IPC_CREAT);
	if(shmId<0)
	{
		printf("Could not create shared segment \n");
		return (2);
	}
	
	/*Attach shared memory segment to process address space*/
	pShm = shmat(shmId, NULL, 0);
	if(!pShm)
	{
		printf("Could not attach shared memory segment.\n");
		return (3);
	}
	
	while(1)
	{
		/*wait for a token from semaphore 0*/
		smop.sem_num =0;
		smop.sem_op = -1;
		smop.sem_flg = 0;
				
		semop(semId, &smop, 1);
		
		/**Process the message avaialabel in shared memory ******/
		printf("Got the semaphore\n");
		strcpy(pShm+256, pShm);
		toggleCase(pShm+256, strlen(pShm+256));
		printf("Processed the request message and placed response\n");
		/*Send token to semaphore 1 **/
		smop. sem_num =1;
		smop.sem_op =1;
		smop.sem_flg =0;
		
		semop(semId, &smop, 1);
	}
} 

void toggleCase(char *buf,int cnt)
{
	int ii;
	for(ii=0;ii<cnt;ii++)
	{
		if((buf[ii] >= 'A') && (buf[ii] <= 'Z'))
			buf[ii] += 0x20;
		else if((buf[ii] >= 'a') && (buf[ii] <= 'z'))
			buf[ii] -= 0x20;
	}
}

	
