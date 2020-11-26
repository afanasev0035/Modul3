/* Для модели клиент-сервер схема мультиплексирования сообщений через одну очередь 
сообщений: Пусть сервер получает из очереди сообщений 
только сообщения с типом 1. В состав сообщений с типом 1, посылаемых серверу, 
процессы-клиенты включают значение своих идентификаторов процесса. Приняв 
сообщение с типом 1, сервер анализирует его содержание, выявляет идентификатор 
процесса, пославшего запрос, и отвечает клиенту, посылая сообщение с типом 
равным идентификатору запрашивавшего процесса. Процесс-клиент после посылки 
запроса ожидает ответа в виде сообщения с типом равным своему идентификатору. 
Поскольку идентификаторы процессов в системе различны, и ни один пользовательский 
процесс не может иметь PID равный 1, все сообщения могут быть прочитаны только 
теми процессами, которым они адресованы. Если обработка запроса занимает 
продолжительное время, сервер может организовывать параллельную обработку 
запросов, порождая для каждого запроса новый процесс-ребенок или новую нить 
исполнения.*/
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/msg.h>
#include<string.h>
#include<stdio.h>
#define LAST_MESSAGE 255

struct mymsgbuf
{
	long mtype;
	int id;
	char message[100];
}mybuf;

int main()
{
	int msqid,maxlen,i,len,pid;
	char pathname[]="myfile";
	key_t key;
	
	/* Генерируем IPC ключ из имени файла 09-1a.c в текущей директории
	и номера экземпляра очереди сообщений 0. */
	if((key=ftok(pathname,0))<0)
	{
		printf("Error: can't generate key\n");
	}
	
	/* Пытаемся получить доступ по ключу к очереди сообщений, если она существует,
	или создать ее, если она еще не существует, с правами доступа
	read & write для всех пользователей */
	if((msqid=msgget(key, 0666 | IPC_CREAT))<0)
	{
		printf("Error: can't get msqid\n");
	}

	if((pid=fork()) < 0)
	{
		printf("Error: can't fork.\n");
	}
	else if(pid == 0)
	{
		/*Child*/

		mybuf.mtype=1;
		mybuf.id=getpid();
		len=sizeof(mybuf);
		if(msgsnd(msqid,(struct msgbuf*)&mybuf,len,0)<0)
		{
			printf("Error: Can't send message to queue\n");
			msgctl(msqid,IPC_RMID,(struct msqid_ds*)NULL);
		}
		printf("Client %d sent!\n",getpid());
		
		i=0;
		while(i!=1)
		{
			maxlen=sizeof(mybuf);
			if((len=msgrcv(msqid,(struct msgbuf*)&mybuf,maxlen,getpid(),0))<0)
			{
			printf("Error: can't recive message from queue\n");
			}
			i=1;
			printf("message type = %ld, id is %d message %s\n",mybuf.mtype,mybuf.id,mybuf.message);			
		}
		return 0;
	}	
	else
	{
		/*Parent*/

		mybuf.mtype=1;
		mybuf.id=getpid();
		len=sizeof(mybuf);
		if(msgsnd(msqid,(struct msgbuf*)&mybuf,len,0) < 0)
		{
			printf("Error: Can't send message to queue\n");
			msgctl(msqid,IPC_RMID,(struct msqid_ds*)NULL);
		}
		printf("Client %d sent!\n",getpid());
		
		i=0;
		while(i!=1)
		{
			maxlen=sizeof(mybuf);
			if((len=msgrcv(msqid,(struct msgbuf*)&mybuf,maxlen,getpid(),0)) < 0)
			{
			printf("Error: can't recive message from queue\n");
			}
			i=1;
			printf("message type = %ld, id is %d message %s\n",mybuf.mtype,mybuf.id,mybuf.message);
		}
		return 0;
	}
}
