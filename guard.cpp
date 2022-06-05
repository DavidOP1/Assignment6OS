#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
// #include <Guard.cpp>

pthread_mutex_t lock2 = PTHREAD_MUTEX_INITIALIZER;

class Guard {      // The class
  private:             // Access specifier
    pthread_mutex_t *lock;
    public:
    Guard(pthread_mutex_t* lock) {  
    this->lock = lock;
    pthread_mutex_lock(this->lock);
    }
    ~Guard() { pthread_mutex_unlock(this->lock); } 
};




int c;
void *print_message_function( void *ptr )
{
    Guard g(&lock2);
    for (int i = 0; i < 200; i++)
    {
        c++;
        printf("%s : %d\n",(char*) ptr,c);
    } 
}
int main(int argc, char const *argv[])
{
     pthread_t thread1, thread2;
     char *message1 = "Thread 1";
     char *message2 = "Thread 2";
     int  iret1, iret2;

    /* Create independent threads each ofw which will execute function */
     /* args:
     1.thread id
     2.attributes to the thread (joinable, scheduling priority, stack address)
     3.pointer to execute function (similar to run in java)
     4.arguments to the function, if needed multiple use a struct
     */
     iret1 = pthread_create( &thread1, NULL, print_message_function, (void*) message1);
     iret2 = pthread_create( &thread2, NULL, print_message_function, (void*) message2);
     
     printf("%d\n",iret2);
     /* Wait till threads are complete before main continues. Unless we  */
     /* wait we run the risk of executing an exit which will terminate   */
     /* the process and all threads before the threads have completed.   */

     pthread_join( thread1, NULL);
     pthread_join( thread2, NULL); 

     printf("Thread 1 returns: %d\n",thread1);
     printf("Thread 2 returns: %d\n",thread2);
     exit(0);
}