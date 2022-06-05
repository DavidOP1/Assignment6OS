#include <sys/mman.h>
#include <pthread.h>
#include <iostream>
#include <stdlib.h>
using namespace std;

pthread_mutex_t closer;
template <typename T>
class singelton
{
private:
    inline static singelton<T> *helper = NULL;
    ~singelton()
    {
        helper = NULL;
    };
    singelton() {}

public:
    static void Destroy()
    {
        pthread_mutex_lock(&closer);
        helper = NULL;
        delete helper;
        pthread_mutex_unlock(&closer);
    }
    static singelton<T> *Instance()
    {
        pthread_mutex_lock(&closer);
        if (helper == NULL)
        {
            helper = new singelton<T>;
        }
        return helper;
    }
};

int main()
{
    singelton<int> *exampOne = singelton<int>::Instance();
    singelton<int> *exampTwo = singelton<int>::Instance();
    void *mineOne = mmap(exampOne, 1, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
    void *mineTwo = mmap(exampTwo, 1, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
    cout << "SingletonClass instance created!" << endl;
    cout << exampOne << endl;
    cout << exampTwo << endl;
}