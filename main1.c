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

#define true 1
#define false 0

pthread_mutex_t mutex;
pthread_cond_t empty;

char letters[26]={'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z'};

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
    void (*func1)();
    void (*func2)();
}activeObject;

/////////////////////////////////////////////////////////////////INIT QUEUE///////////////////////////////////////////////////////////////////
void createQ(queue *q){
pthread_mutex_lock(&mutex);
q->head=NULL;
q->tail=NULL;
pthread_mutex_unlock(&mutex);
}

/////////////////////////////////////////////////////////////////DESTROY QUEUE///////////////////////////////////////////////////////////////////
void destroyQ(queue *q){
pthread_mutex_lock(&mutex);
printf("asdasdasdasd\n");
//Here we'll delete while the deQ doesn't return false
while (q->head!=NULL)
{
    //print for testing purposes
    //Essesentially , we don't have to do anything here.
    printf("Still going\n");
    //head of q
    Node *temp = q->head;

    //first item in q
    void * result = temp->val;

    q->head=q->head->next;
    if(q->head==NULL){
    q->tail=NULL;
    }   
    printf("%s shalom\n",(char *)result);
    free(temp);
}
pthread_mutex_unlock(&mutex);
}


/////////////////////////////////////////////////////////////////ENTER NODE///////////////////////////////////////////////////////////////////
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

/////////////////////////////////////////////////////////////////Remove first node from queue (FIFO)/////////////////////////////////////////
void * deQ(queue *q){ 
pthread_mutex_lock(&mutex);
printf("Entered deQ\n");

//Checking if q is empty, return has no meaning , just to exit function, and for deleting the queue purposes.
if(q->head==NULL) { 
    printf("Empty\n");
    pthread_cond_wait(&empty,&mutex);
    }
printf("Still going\n");
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
//Unlocking the thread lock



/////////////////////////////////////////////////////////////////////////////CREATE AO/////////////////////////////////////////////////////
// returns a pointer to the new activeObject struct created.
activeObject * newAO(queue * qu,void (*function1)(),void (*function2)()){
  activeObject * ao = (activeObject*)malloc(sizeof(activeObject));
  ao->q=qu;
  ao->func1=function1;
  ao->func2=function2;
  queue qTemp;
  createQ(&qTemp);
  //The queue will be empty after calling all
  while (ao->q->head!=NULL)
  {
    //asd
    void * temp = deQ(ao->q);
    ao->func1(temp);//here cast to whatever datatype you want
    enQ(&qTemp,temp);
  }
  while ((&qTemp)->head!=NULL)
  {
    //asd
    ao->func2(deQ(&qTemp));
  }destroyQ(&qTemp);
  return ao;
}

/////////////////////////////////////////////////////////////////////////////DESTORY AO/////////////////////////////////////////////////////

void destroyAO(activeObject * ao){
  destroyQ(ao->q);
}

void print1(char *  num){
  printf("function1 : %s\n",num);
}

void print2(char * num){
  printf("function2 : %s\n",num);
}



int main(){

}