#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <string.h>
#include <string>
#include <pthread.h>
#include <time.h>
using namespace std;

//char* IPaddr;

typedef struct _tagValue{
	char IP[40];
	int start;
	int end;
}PortNums;

void create_socket(const char* IP, int StartPort, int EndPort){
	int sockfd;
    struct sockaddr_in serv_addr;

    /*create socket*/
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        cerr << "server: can't open stream socket\n";
        exit(0);
    }

    /*generate serv_addr*/
    bzero((char*)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET; 
    if (inet_pton(AF_INET, IP, &serv_addr.sin_addr) <= 0){
    	cerr << "inet_pton error for: " << IP << endl;
    	exit(-1);
    }
	
	for (int i = StartPort; i < EndPort; i++){
		serv_addr.sin_port = htons(i);
		/*connect*/
    	if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
        	continue;
    	}
    	cout << " " << i;
	}
	//cout << endl;
	close(sockfd);
}

void* beginThreadFunc(void* lpParam){
	PortNums *pnInt = (PortNums*)lpParam;
    pthread_detach(pthread_self());
	create_socket(pnInt->IP, pnInt->start, pnInt->end);
	return NULL;
}

void get_hostname(const char* IP){
    hostent *host;
    char p[30];
    if (inet_pton(AF_INET, IP, p) <= 0){
    	cerr << "inet_pton error for: " << IP;
    	exit(-1);
    }
    if ((host = gethostbyaddr(p, strlen(p), AF_INET)) == NULL) {
        return;
    }
    cout << host->h_name << " " << IP;
    if (strcmp("_gateway", host->h_name)==0) {
    	cout << endl;
    	return;
    }
    
    /*threads*/
    PortNums *m1 = new PortNums();
    int err;
    pthread_t tid;
    strcpy(m1->IP, IP);
    m1->start = -100;
    m1->end = 0;
    for (int i = 0; i < 655; i++){
    	m1->start += 100;
    	m1->end += 100;
        //printf("Create thread\n");
    	err = pthread_create(&tid, NULL, beginThreadFunc, m1);
    	if (err != 0){
    		cerr << "thread: can't create thread" << err << endl;
    		break;
    	}
    	usleep(2000);
    }
    m1->start = m1->end;
    m1->end += 36;
    pthread_create(&tid, NULL, beginThreadFunc, m1);
    usleep(2000);
    cout << endl;
}

/*scan IP and Port*/
void scan(string ip_prefix){
    for (int i = 2; i < 256; i++) {
        string ip = ip_prefix + '.' + to_string(i);
        //cout << ip << endl;
        get_hostname(ip.c_str());
    }
}

int main(){
    printf("=================scanner=================\n");
    printf("host_id          IP           port\n");
	scan("192.168.102");
    printf("=========================================\n");
    return 0;
}

