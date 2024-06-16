#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<getopt.h>
#include<sys/socket.h>
#include<sys/time.h>
#include<arpa/inet.h>

#define MAX_MAS_LENGTH		512

#define MAX_MAS_LENGTH		512
#define TIME_SUB_MS(tv1, tv2)  ((tv1.tv_sec - tv2.tv_sec) * 1000 + (tv1.tv_usec - tv2.tv_usec) / 1000)

int send_msg(int connfd,char *msg,int length){
    int res=send(connfd,msg,length,0);
    if(res<0){
        perror("send");
        exit(1);
    }
    return res;
}

int recv_msg(int connfd,char *msg,int length){
    int res=recv(connfd,msg,length,0);
    if(res<0){
        perror("recv");
        exit(1);
    }
    return res;
}

void equals(char *pattern, char *result, char *casename) {

	if (strcmp(pattern, result) == 0) {
		//printf("==> PASS --> %s\n", casename);
	} else {
		printf("==> FAILED --> %s, '%s' != '%s'\n", casename, pattern, result);
	}
}


void test_case(int connfd,char *msg,char *pattern,char *casename){
    if(!msg || !pattern || !casename)return;

    send_msg(connfd,msg,strlen(msg));

    char result[MAX_MAS_LENGTH]={0};

    recv_msg(connfd,result,MAX_MAS_LENGTH);

    equals(pattern,result,casename);
}

void array_testcase(int connfd){
    test_case(connfd, "SET Name Jerry", "SUCCESS\n", "SETCase");
	test_case(connfd, "GET Name", "Jerry\n", "GETCase");
	test_case(connfd, "MOD Name Tom", "SUCCESS\n", "MODCase");
	test_case(connfd, "GET Name", "Tom\n", "GETCase");
	test_case(connfd, "DEL Name", "SUCCESS\n", "DELCase");
	test_case(connfd, "GET Name", "NO EXIST\n", "GETCase");
}

void array_testcase_10w(int connfd){
    //int count=100000;
    int count=100000;
    int i=0;
    while(i++<count){
        array_testcase(connfd);
    }
}

void rbtree_testcase(int connfd) {

	test_case(connfd, "RSET Name King", "SUCCESS\n", "SETCase");
	test_case(connfd, "RGET Name", "King\n", "GETCase");
	test_case(connfd, "RMOD Name Darren", "SUCCESS\n", "MODCase");
	test_case(connfd, "RGET Name", "Darren\n", "GETCase");
	test_case(connfd, "RDEL Name", "SUCCESS\n", "DELCase");
	test_case(connfd, "RGET Name", "NO EXIST\n", "GETCase");
}

void rbtree_testcase_10w(int connfd) { // 10w

	int count = 100000;
	int i = 0;

	while (i ++ < count) {
		rbtree_testcase(connfd);
	}

}

void hash_testcase(int connfd) {

	test_case(connfd, "RSET Name King", "SUCCESS\n", "SETCase");
	test_case(connfd, "RGET Name", "King\n", "GETCase");
	test_case(connfd, "RMOD Name Darren", "SUCCESS\n", "MODCase");
	test_case(connfd, "RGET Name", "Darren\n", "GETCase");
	test_case(connfd, "RDEL Name", "SUCCESS\n", "DELCase");
	test_case(connfd, "RGET Name", "NO EXIST\n", "GETCase");
}

void hash_testcase_10w(int connfd) { // 10w

	int count = 100000;
	int i = 0;

	while (i ++ < count) {
		hash_testcase(connfd);
	}

}


int connection_tcpserver(const char* ip,unsigned short port){
    int connfd=socket(AF_INET,SOCK_STREAM,0);

    struct sockaddr_in tcpserver_addr;
    memset(&tcpserver_addr,0,sizeof(tcpserver_addr));

    tcpserver_addr.sin_family=AF_INET;
    tcpserver_addr.sin_port=htons(port);
    tcpserver_addr.sin_addr.s_addr=inet_addr(ip);

    int ret=connect(connfd,(struct sockaddr*)&tcpserver_addr,sizeof(struct sockaddr));
    if(ret){
        perror("connect");
        return -1;
    }
    return connfd;
}

// ./testcase -s 192.168.186.138 -p 9096 -m 1
int main(int argc,char*argv[]){
    int ret=0;

    char ip[16]={0};
    int port=0;
    int mode=1;

    int opt;
    while((opt=getopt(argc,argv,"s:p:m:?"))!=-1){
        switch (opt) {

			case 's':
				strcpy(ip, optarg);
				break;

			case 'p':
				port = atoi(optarg);
				break;

			case 'm':
				mode = atoi(optarg);
				break;

			default:
				return -1;
		
		}

    }

    int connfd=connection_tcpserver(ip,port);

    if (mode & 0x1) { // array

		struct timeval tv_begin;
		gettimeofday(&tv_begin, NULL);
		
		array_testcase_10w(connfd);

		struct timeval tv_end;
		gettimeofday(&tv_end, NULL);

		int time_used = TIME_SUB_MS(tv_end, tv_begin);
		
		printf("array testcase--> time_used: %d, qps: %d\n", time_used, 600000 * 1000 / time_used);
	}

    if (mode & 0x2) { // rbtree

		struct timeval tv_begin;
		gettimeofday(&tv_begin, NULL);
		
		//rbtree_testcase_10w(connfd);
		
		rbtree_testcase_10w(connfd);

		struct timeval tv_end;
		gettimeofday(&tv_end, NULL);

		int time_used = TIME_SUB_MS(tv_end, tv_begin);
		
		printf("rbtree testcase-->  time_used: %d, qps: %d\n", time_used, 600000 * 1000 / time_used);
	
	}

	if (mode & 0x4) { // hash

		struct timeval tv_begin;
		gettimeofday(&tv_begin, NULL);
		
		//hash_testcase(connfd);
		hash_testcase_10w(connfd);

		struct timeval tv_end;
		gettimeofday(&tv_end, NULL);

		int time_used = TIME_SUB_MS(tv_end, tv_begin);
		
		printf("hash testcase-->  time_used: %d, qps: %d\n", time_used, 600000 * 1000 / time_used);

		
	
	}



    return 0;
}