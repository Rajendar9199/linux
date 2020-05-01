#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

//Copy wrapper functions of semsrv.c file here.

int mysem_post(int semid)
{
	struct sembuf sb;
	sb.sem_num = 1;
	sb.sem_op = 1;
	sb.sem_flg = 0;
	return semop(semid,&sb,1);
}
int mysem_open(int key)
{
	return semget(key,1,0);
}
int main()
{
	int semid;
	semid = mysem_open(1234);
	if(semid < 0)
	{
		perror("Sem open failed");
		exit(1);
	}
	printf("Semid = %d\n",semid);
	while(1)
	{
		printf("Press enter key to send semaphore\n");
		getchar();
		mysem_post(semid);
	}
}

