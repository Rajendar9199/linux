#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define MY_KEY 19920809
#define MSG_LEN 256
#define SRV_MSG_TYPE 1

void toggleCase(char *buf, int cnt);

int main()
{
	int mqId;
	int msgLen;
	char rxmsg[MSG_LEN];
	char txmsg[MSG_LEN];
	
	mqId = msgget(MY_KEY,0660 | IPC_CREAT);
	if(mqId < 0)
	{
		printf("Could not create message queue\n");
		return(1);
	}
	else
		printf("Opened message queue. Id is %d\n",mqId);
	
	while(1)
	{
		printf("Server: waiting for message\n");
		msgLen = msgrcv(mqId, rxmsg, MSG_LEN, SRV_MSG_TYPE,0);
		if(msgLen == -1)
		{
			perror("msgrcv");
			return 1;
		}
		printf("Received message of size %d from client\n", msgLen);
		toggleCase(rxmsg+8,msgLen-4);
               // puts(rxmsg+8);
		memcpy(txmsg, rxmsg+4, msgLen);
		msgsnd(mqId, txmsg, msgLen-4,0);
		printf("Sent processed message to %s client\n",txmsg+4);
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

