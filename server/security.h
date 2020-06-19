#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct SecurityGroup{
    char ipaddr[40];
    int subnet;
    struct SecurityGroup* next;
}SecurityGroup;

int c2i_(char* s) {
    int i, num;
    for (num = 0, i = 0; i < strlen(s); i++) {
        if (s[i] > '9' && s[i] < '0') {
            printf("number out of range\n");
            return -1;
        }
        num = num * 10 + s[i] - '0';
    }
    return num;
}


void ip2b(char* ip_str,char* bin_str){
    memset(bin_str,0,sizeof(bin_str));
    int i1, i2, i3, i4;
    unsigned ip;
    int i;
    sscanf(ip_str, "%d.%d.%d.%d", &i1, &i2, &i3, &i4);
    ip=(i1<<24) + (i2<<16) + (i3<<8) + i4;
    for (i = 31; i >= 0; i--){
        sprintf(bin_str, "%s%d", bin_str, (ip&(1<<i))>>i);
    }
}

void ip_split(char* src,const char* separator,char* ip_str,char* subnet_str) {
     char *pNext;
     if (src == NULL || strlen(src) == 0) //如果传入的地址为空或长度为0，直接终止
        return;
     if (separator == NULL || strlen(separator) == 0) //如未指定分割的字符串，直接终止
        return;
     pNext = (char *)strtok(src,separator); //必须使用(char *)进行强制类型转换(虽然不写有的编译器中不会出现指针错误)
     strcpy(ip_str,pNext);
     pNext = (char *)strtok(NULL,separator);
     strcpy(subnet_str,pNext);
     int slen = strlen(subnet_str);
     subnet_str[slen-1] = '\0';
}


SecurityGroup* loadSecurity(char *addr){
    SecurityGroup* head = (SecurityGroup *)malloc(sizeof(SecurityGroup));
    memset(head, 0, sizeof(*head));
    head->next = NULL;
    char line[256];
    FILE *f = fopen(addr, "rb");
    while (!feof(f) && !ferror(f)) {
        SecurityGroup* node;
        SecurityGroup* tmp;
        char temp_addr1[40],temp_addr2[40];
        node = (SecurityGroup *)malloc(sizeof(SecurityGroup));
        memset(node, 0, sizeof(*node));
        strcpy(line, "\n");
        fgets(line, sizeof(line), f);
        if(strlen(line)<2) break;
        ip_split(line,",",temp_addr1,temp_addr2);
        //printf("%s",temp_addr1);
        strcpy(node->ipaddr,temp_addr1);
        node->subnet = c2i_(temp_addr2);
        tmp = head->next;
        head->next = node;
        node->next = tmp;
    }
    fclose(f);
    return head;
}

int ip_compare(char* ip1,char* ip2,int num){
    int flag = 1;
    for(int i=0;i<num;i++){
        if(ip1[i]!=ip2[i]){
            flag=0;
            break;
        }
    }
    return flag;
}

int securitycheck(SecurityGroup* securitygroup,char* ip_str){
    char des_ip_b[40];
    int flag = 1;
    ip2b(ip_str,des_ip_b);
    SecurityGroup* prob;
    prob = securitygroup->next;
    while(prob!=NULL){
        char sec_ip_b[40];
        ip2b(prob->ipaddr,sec_ip_b);
        if(ip_compare(des_ip_b,sec_ip_b,prob->subnet)){
            flag=0;
            break;
        }
        prob = prob->next;
    }
    return flag;
}
