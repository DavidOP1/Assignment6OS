
#include <pthread.h>
#include <stdlib.h>

typedef struct Reactor
{
    int jucn;
    void *(*mypt)(void *);
    pthread_t myproc;

} * myPre, myRe;

typedef struct event
{
    int jucn;
    myPre mypre;
} * myPev, myevent;

void installHandler(myPre mypre, void *(mypt)(void *), int jucn)
{
    mypre->mypt = mypt;
    mypre->jucn = jucn;
    myPev mypev = (myPev)malloc(sizeof(myevent));
    mypev->mypre = mypre;
    mypev->jucn = jucn;
    pthread_create(&mypre->myproc, NULL, mypt, mypev);
}

void RemoveHandler(myPre mypre)
{
    pthread_join(mypre->myproc, NULL);
    mypre->mypt = nullptr;
    mypre->jucn = -1;
}

myPre newReactor()
{
    myPre mypre = (myPre)(malloc(sizeof(myRe)));
    return mypre;
}
