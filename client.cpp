/*
** client.c -- a stream socket client demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <pthread.h>
#include <arpa/inet.h>

#define PORT "3490" // the port client will be connecting to 

#define SIZE 4096 // max number of bytes we can get at once 

int moneRecv=0,moneSend=0,moneFinished=0;

///////////////////////////////////////////////////////////////////////////////COMMUNICATING//////////////////////////////////////////////////////////////////
//Create here an extra which waits for messages from server.
void * communication(void * sock){
    char buff2[SIZE];
    int sockfd = *(int*) sock;
    while (1)
    {
        memset(buff2,0,sizeof(buff2));
        printf("Send message to server : ");
        if (fgets(buff2,SIZE,stdin)==NULL)
      {
        printf("!Error in getting command!");
      }else{
         if (send(sockfd,buff2,SIZE, 0)== -1)
              perror("send");
         if(!strcmp(buff2,"FINISH\n")){
             moneFinished=1;
             if(moneSend==0){
               exit(1);
             }
             while (1)
             {
                 sleep(10000000);}
         }else{
             moneSend+=1;
         }
      }
    }
    

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void  * recvMesg(void * tempSock){
 int numbytes;
 char buf[SIZE];
 int sockfd = *(int*) tempSock;
 while(1){
 memset(buf,0,sizeof(buf));
 if (moneRecv==3*moneSend&&moneFinished)
 {
    exit(1);
 }
 
 if ((numbytes = recv(sockfd, buf, SIZE-1, 0)) == -1) {
        perror("recv");
        exit(1);
 }else{
     printf("\nrecv: %s \n",buf);
     moneRecv+=1;
 }
 }
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char *argv[])
{
    int sockfd, numbytes;  
    char buf[SIZE];
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];

    if (argc != 2) {
        fprintf(stderr,"usage: client hostname\n");
        exit(1);
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and connect to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("client: connect");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
        return 2;
    }
    pthread_t threads[2];
    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
            s, sizeof s);
    printf("client: connecting to %s\n", s);

    freeaddrinfo(servinfo); // all done with this structure

    if ((numbytes = recv(sockfd, buf, SIZE-1, 0)) == -1) {
        perror("recv");
        exit(1);
    }

    buf[numbytes] = '\0';

    printf("client: received '%s'\n",buf);
    if(pthread_create(&threads[0],NULL,recvMesg,&sockfd)!=0){
            printf("Thread creation error!\n");
        }
    if(pthread_create(&threads[1],NULL,communication,&sockfd)!=0){
            printf("Thread creation error!\n");
    }

    pthread_join(threads[0],NULL);
    pthread_join(threads[1],NULL);
    while (1)
    {
        sleep(100000000);
    }
    
    return 0;
}