#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <pthread.h>
#include <ctype.h>

#define false 0
#define true 1

#define PORT "3490"  // the port users will be connecting to
#define BACKLOG 10   // how many pending connections queue will hold
#define SIZE 4096

pthread_mutex_t mutex;
pthread_cond_t empty;

char letters[26]={'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z'};

/////////////////////////////////////////////////////////////////////////////////////////////active object ,queue , node//////////////////////////////////////////////////////////

typedef struct node{
    void * val;
    struct Node *next;
}Node;

typedef struct{
    Node * head;
    Node *tail;
}queue;

typedef struct AO{
    queue * q;
    void * (*func1)();
    void * (*func2)();
}activeObject;

///////////////////////////////////////////////////////////////////////////////////QUEUE FUNCTIONS////////////////////////////////////////////////////////////////////////////////

//init the queue
void createQ(queue *q){
pthread_mutex_lock(&mutex);
q->head=NULL;
q->tail=NULL;
pthread_mutex_unlock(&mutex);
}

//Destroy and delete the queue
void destroyQ(queue *q){
pthread_mutex_lock(&mutex);
//printf("asdasdasdasd\n");
//Here we'll delete while the deQ doesn't return false
while (q->head!=NULL)
{
    //print for testing purposes
    //Essesentially , we don't have to do anything here.
    //printf("Still going\n");
    //head of q
    Node *temp = q->head;

    //first item in q
    void * result = temp->val;

    q->head=q->head->next;
    if(q->head==NULL){
    q->tail=NULL;
    }   
    //printf("%s shalom\n",(char *)result);
    free(temp);
}
pthread_mutex_unlock(&mutex);
}

//Enter a new node to the queue
int enQ(queue * q, void * val){
pthread_mutex_lock(&mutex);
Node * newNode = (Node*)malloc(sizeof(Node));
if(newNode==NULL) {pthread_mutex_unlock(&mutex); return false;}
newNode->val=val;
newNode->next=NULL;

//ASD
if(q->tail!=NULL){
q->tail->next=newNode;
}
q->tail=newNode;
if (q->head==NULL)
{
  q->head=newNode;
}
//return true;
pthread_cond_signal(&empty);
pthread_mutex_unlock(&mutex);
}

//Remove first node from queue (FIFO)
void * deQ(queue *q){ 
pthread_mutex_lock(&mutex);

//Checking if q is empty, return has no meaning , just to exit function, and for deleting the queue purposes.
if(q->head==NULL) { 
    pthread_cond_wait(&empty,&mutex);
    }
//head of q
Node *temp = q->head;

//first item in q
void * result = temp->val;

q->head=q->head->next;
if(q->head==NULL){
    q->tail=NULL;
}
free(temp);
pthread_mutex_unlock(&mutex);
return result;
}

/////////////////////////////////////////////////////////////////////////////////////////////Create AO////////////////////////////////////////////////////////////////////////////

// returns a pointer to the new activeObject struct created.
activeObject * newAO(queue * qu,void  (*function1)(),void (*function2)(), int sock){
  activeObject * ao = (activeObject*)malloc(sizeof(activeObject));
  ao->q=qu;
  ao->func1=function1;
  ao->func2=function2;
  queue qTemp;
  createQ(&qTemp);
  if(ao->func1!=NULL){
  while (ao->q->head!=NULL)
  {
    //asd
    void * temp = deQ(ao->q);
    char * val = ao->func1((char *)temp);//here cast to whatever datatype you want
    enQ(&qTemp,val);
  }}else{
    while (ao->q->head!=NULL)
  {
    //asd
    void * temp = deQ(ao->q);
    enQ(&qTemp,temp);
  }
  }
  if(ao->func2!=NULL){
  while ((&qTemp)->head!=NULL)
  {
    //asd
    ao->func2((char *)deQ(&qTemp),sock);
    sleep(2);
  }destroyQ(&qTemp);}
  //The queue will be empty after calling all
  return ao;
}

void destroyAO(activeObject * ao){
  destroyQ(ao->q);
}


/////////////////////////////////////////////////////////////////////////////////////////////SERVER///////////////////////////////////////////////////////////////////////////////

void sigchld_handler(int s)
{
    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;

    while(waitpid(-1, NULL, WNOHANG) > 0);

    errno = saved_errno;
}


// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}
////////////////////////////////////////////////////////////CIPHER AND CONVERT/////////////////////////////////////////////////////////////////////////////////////////////////////////

//Encrypt received string with caeser encryption with shift 1
char * cipher(char * string){
  //A==65==0 , B===66==1 (ASCII VALUE -65) WE ARE GOING TO USE %25
  int i=0;
  char * temp=(char *)malloc(sizeof(string));
  while (*(string+i)!=NULL)
  {
     int a= *(string+i)-65;
     if (a>=0&&a<=25)
     {
        temp[i]=letters[(a+1)%26];
        //printf("%d \n", a);
        //printf("char : %c \n ",letters[(a+1)%26]);
     }else{
         temp[i]=*(string+i);
     }i+=1;
  }
  //printf("enc = %s\n",temp);
  //free(temp);
  //IMPORTANT dont forget to free temp!!!!!!!!!!
  return temp;
}

//function to convert big letters to small and vice versa
char * convert(char * string){
 int i=0;
  char * temp=malloc(sizeof(string));
  memset(temp,0,sizeof(temp));
  while (*(string+i)!=NULL)
  {
     int a= *(string+i);
     if (a>=65&&a<=90)
     {
        temp[i]=tolower(*(string+i));
     }else if (a>=97&&a<=122){
         temp[i]=toupper(*(string+i));
     }else{
         temp[i]=*(string+i);
     }i+=1;
  }
  //printf("converted string: %s\n",temp);
  //important don't forget to free temp!!!!!!
  return temp;
}


////////////////////////////////////////////////////////////SEND ANSWER TO SERVER//////////////////////////////////////////////////////////////////////////////////////////////////////


void sendAnswer(char * answer, int sock){
  if (send(sock,answer, strlen(answer), 0)== -1)
        perror("send"); 
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void * sendMessage(void * tempSock){
    int numbytes;
    char buf2[SIZE];
    int numbytes2;
    int sock = *(int*) tempSock;
    if (send(sock, "connected", strlen("connected"), 0)== -1)
        perror("send"); 
    int live=1;
    queue q1,q2,q3;
    while (live)
    {
       memset(buf2,0,sizeof(buf2));
       if ((numbytes = recv(sock , buf2, SIZE , 0)) == -1) {
        perror("recv");
        exit(1);
        }buf2[strlen(buf2)-1] = '\0';
        char * temp=malloc(strlen(buf2));
        strcpy(temp,buf2);
        char * temp2=malloc(strlen(buf2));
        strcpy(temp2,buf2);
        char * temp3=malloc(strlen(buf2));
        strcpy(temp3,buf2);
        if(strcmp(buf2,"FINISH")){
        //printf("size of : %d\n",strcmp(buf2,"FINISH"));
       // printf("received from %d string: %s\n",sock,buf2);
        enQ(&q1,temp);
        enQ(&q2,temp2);
        enQ(&q3,temp3);
        }else if(!strcmp(buf2,"FINISH")){
            ///printf("entered finish\n");
            activeObject * ao1=newAO(&q1,&cipher,&sendAnswer,sock);
            activeObject * ao2=newAO(&q2,&convert,&sendAnswer,sock);
            activeObject * ao3=newAO(&q3,NULL,&sendAnswer,sock);
            live=0;
    }
    }
    while(1){sleep(10000000000000);}
}
/////////////////////////////////////////////////////////////////////////////////////////////MAIN///////////////////////////////////////////////////////////////////////////////

int main(void)
{
       int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    struct sigaction sa;
    int yes=1;
    char s[INET6_ADDRSTRLEN];
    int rv;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("server: bind");
            continue;
        }

        break;
    }

    freeaddrinfo(servinfo); // all done with this structure

    if (p == NULL)  {
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }

    if (listen(sockfd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }

    sa.sa_handler = sigchld_handler; // reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    printf("server: waiting for connections...\n");
    pthread_t threads[50];
    int j=0;
    while(1) {  // main accept() loop
        sin_size = sizeof their_addr;
        new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
        if (new_fd == -1) {
            perror("accept");
            continue;
        }

        inet_ntop(their_addr.ss_family,
            get_in_addr((struct sockaddr *)&their_addr),
            s, sizeof s);
        printf("server: got connection from %s\n", s);
        //printf("thread value : %p\n",&threads[j]);
        //printf("SOCK FD VALUE = %p\n",&new_fd);
        if(pthread_create(&threads[j++],NULL,sendMessage,&new_fd)!=0){
            printf("Thread creation error!\n");
        }
        if(j>=50){
            j=0;
            while (j<50)
            {
                pthread_join(threads[j++],NULL);
            }
            j=0;
        } 
        // pthread_exit(NULL);
    }
    pthread_mutex_destroy(&mutex);
    return 0;
}