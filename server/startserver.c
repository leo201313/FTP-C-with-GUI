#include <stdio.h>
#include <stdlib.h>
#include <sys/prctl.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include "netrate.h"
#include "kbhit.h"


int main(){
    pid_t pid_server,pid_netrate;
    char buffer[100];
    char s_port[10],s_path[50],s_maxcon[10];
    printf("Welcome to C-FTP!\n");
    printf("Please set port (usage: 21) :\n");
    scanf("%s",s_port);
    printf("Please set root path (usage: /tmp) :\n");
    scanf("%s",s_path);
    printf("Please set max connections (default: 128)\n" );
    scanf("%s",s_maxcon );
    printf("Port:%s, Root Path:%s, Max Connections:%s\n",s_port,s_path,s_maxcon);
    //File* fd=fopen("server_log.txt","w+");
    pid_server = fork();
    if(pid_server<0){
        perror("fork");
        exit(1);
    }
    if(pid_server == 0){
        prctl(PR_SET_PDEATHSIG,SIGKILL);
        //char buffer[100];
        //sprintf(buffer,"./server -port %s -root %s -maxcon %s >> server_log.txt",s_port,s_path,s_maxcon);
        //system("touch server_log.txt");
        //system("chmod 777 server_log.txt");
        //system(buffer);
       if( execlp("./server","./server","-port",s_port,"-root",s_path,"-maxcon",s_maxcon,NULL)<0){
           perror("execlp");
           exit(1);
       }
    }
    else{
        pid_netrate = fork();
        if (pid_netrate<0) {
            perror("fork");
            exit(1);
        }
        if(pid_netrate==0){
            prctl(PR_SET_PDEATHSIG,SIGKILL);
            char device[20]={0};
            executeCMD( "cat /proc/net/dev | awk '{i++;if(i>2){print $1}}' | sed 's/^[\t]*//g' | sed 's/[:]*$//g' | sed -e '/lo/d' | head -n 1",device);
            int devlen;
            devlen = strlen(device);
            for(int i=0;i<devlen;i++){
                if(device[i]=='\n'){
                    device[i]='\0';
                    break;
                }
            }
            printf("%s\n",device );
            netrate(device);
        }
        else{
            char get_c;
            while(get_c!=27){
                if(kbhit()) get_c=getchar();
            }
            printf("Quit command is confirmed! Now quit....\n");
            return 0;
        }
    }
    return 0;
}
