#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
 
#define WAIT_SECOND 3	//暂停时间，单位为“秒”
#define NET_NAME "eno1"
 
int getCurrentDownloadRates(char *netname,long int * save_rate,long int * tx_rate); //获取当前的流量，参数为将获取到的流量保存的位置
 
int main(int argc, char * argv[])
{
    long int start_rcv_rates = 0;	//保存开始时的流量计数
    long int end_rcv_rates = 0;	//保存结束时的流量计数
    long int start_tx_rates = 0;   //保存开始时的流量计数
    long int end_tx_rates = 0; //保存结束时的流量计数
 
    while(1)
    {
        getCurrentDownloadRates(NET_NAME,&start_rcv_rates,&start_tx_rates);//获取当前流量，并保存在start_rcv_rates 里
        sleep(WAIT_SECOND);	//休眠多少秒，这个值根据宏定义中的WAIT_SECOND的值来确定
                                                 //sleep的头文件在unistd.h
        getCurrentDownloadRates(NET_NAME,&end_rcv_rates,&end_tx_rates);//获取当前流量，并保存在end_rcv_rates end_tx_rates里
        printf("rcv is : %.2lf Bytes/s\n", (float)(end_rcv_rates-start_rcv_rates)/WAIT_SECOND );//打印结果
        printf("tx is : %.2lf Bytes/s\n", (float)(end_rcv_rates-start_rcv_rates)/WAIT_SECOND );//打印结果
    }
    exit(EXIT_SUCCESS);
}
 
 int getCurrentDownloadRates(char *netname,long int * save_rate,long int * tx_rate)
 {
    FILE * net_dev_file;    //文件指针
    char buffer[1024];  //文件中的内容暂存在字符缓冲区里
    size_t bytes_read;  //实际读取的内容大小
    char * match;    //用以保存所匹配字符串及之后的内容
    int counter = 0;
    int i = 0;
    char tmp_value[128];
 
    if((NULL == netname)||(NULL == save_rate)||(NULL == tx_rate))
    {
        printf("bad param\n");
        return -1;    
    }
 
    if ( (net_dev_file=fopen("/proc/net/dev", "r")) == NULL ) //打开文件/pro/net/dev/，我们要读取的数据就是它啦
    {
        printf("open file /proc/net/dev/ error!\n");
        return -1;
    }
    memset(buffer,0,sizeof(buffer));
 
    while(fgets(buffer,sizeof(buffer),net_dev_file) != NULL)
    {
        match = strstr(buffer,netname);
 
        if(NULL == match)
        {
            //printf("no eth0 keyword to find!\n");
            continue;
        }
        else
        {
            //printf("%s\n",buffer);
            match = match + strlen(netname) + strlen(":");/*地址偏移到冒号*/
            sscanf(match,"%ld ",save_rate);
            memset(tmp_value,0,sizeof(tmp_value));
            sscanf(match,"%s ",tmp_value);
            match = match + strlen(tmp_value);
            for(i=0;i<strlen(buffer);i++)
            {
                if(0x20 == *match)
                {
                    match ++;
                }
                else
                {
                    if(8 == counter)
                    {
                        sscanf(match,"%ld ",tx_rate);
                    }
                    memset(tmp_value,0,sizeof(tmp_value));
                    sscanf(match,"%s ",tmp_value);
                    match = match + strlen(tmp_value);
                    counter ++;
                }
            }
            //printf("%s save_rate:%ld tx_rate:%ld\n",netname,*save_rate,*tx_rate);
        }
    }
 
    return 0;/*返回成功*/
}
