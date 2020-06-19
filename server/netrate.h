#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <dirent.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>

#ifdef debugprintf
	#define debugpri(mesg, args...) fprintf(stderr, "[NetRate print:%s:%d:] " mesg "\n", __FILE__, __LINE__, ##args)
#else
	#define debugpri(mesg, args...)
#endif

int GetNetRate(FILE* fd,char *interface,long *recv,long *send)
{
	char buf[1024];
	char *p;
	char flow[32];
	int i = 0;
	char tempstr[16][16]={0};

	memset(buf,0,sizeof(buf));
	memset(tempstr,0,sizeof(tempstr));
	fseek(fd, 0, SEEK_SET);
	int nBytes = fread(buf,1, sizeof(buf)-1,fd);
	if (-1 == nBytes)
	{
		debugpri("fread error");
		fclose(fd);
		return -1;
	}
	buf[nBytes] = '\0';
	char* pDev = strstr(buf, interface);
	if (NULL == pDev)
	{
		printf("don't find dev %s\n", interface);
		fclose(fd);
		return -1;
	}
	sscanf(pDev,"%[^' ']\t%[^' ']\t%[^' ']\t%[^' ']\t%[^' ']\t%[^' ']\t%[^' ']\t%[^' ']\t%[^' ']\t%[^' ']\t%[^' ']\t%[^' ']\t",\
	tempstr[0],tempstr[1],tempstr[2],tempstr[3],tempstr[4],tempstr[5],tempstr[6],tempstr[7],tempstr[8],tempstr[9],tempstr[10],tempstr[11]);
	*recv = atol(tempstr[1]);
	*send = atol(tempstr[9]);
}

void netrate(char* device){
    struct timeval tv_now,tv_pre;
	char netdevice[16]={0};
	int nDevLen;
	long recvpre = 0,recvcur = 0;
	long sendpre = 0,sendcur = 0;
	double sendrate;
	double recvrate;
    sprintf(netdevice,"%s",device);
    FILE* fd = fopen("/proc/net/dev","r");
	if (NULL == fd)
	{
		debugpri("/proc/net/dev not exists!\n");
		return -1;
	}
	while(1)
	{
		gettimeofday(&tv_pre,NULL);
		GetNetRate(fd,netdevice,&recvpre,&sendpre);
		sleep(2);
		gettimeofday(&tv_now,NULL);
		GetNetRate(fd,netdevice,&recvcur,&sendcur);
		recvrate= (recvcur - recvpre)/(1024*(tv_now.tv_sec+tv_now.tv_usec*0.000001-tv_pre.tv_sec+tv_pre.tv_usec*0.000001));
		if(recvrate<0)
		{
			recvrate = 0;
		}
		sendrate= (sendcur - sendpre)/(1024*(tv_now.tv_sec+tv_now.tv_usec*0.000001-tv_pre.tv_sec+tv_pre.tv_usec*0.000001));
		if(sendrate<0)
		{
			sendrate = 0;
		}
		system("clear");
        printf("Server starts!\n");
        printf("Press ESC to quit!\n");
		printf("Server is running, here is the netrate:\n");
		printf("Net Device	receive rate	send rate\n");
		printf("%-10s\t%-6.2fKB/sec\t%-6.2fKB/sec\n",netdevice,recvrate,recvrate);
	}
	fclose(fd);
}

void executeCMD(const char *cmd, char *result)
{
    char buf_ps[1024];
    char ps[1024]={0};
    FILE *ptr;
    strcpy(ps, cmd);
    if((ptr=popen(ps, "r"))!=NULL)
    {
        while(fgets(buf_ps, 1024, ptr)!=NULL)
        {
           strcat(result, buf_ps);
           if(strlen(result)>1024)
               break;
        }
        pclose(ptr);
        ptr = NULL;
    }
    else
    {
        printf("popen %s error\n", ps);
    }
}
