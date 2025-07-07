
#ifndef __PTHREAD_HPP__
#define __PTHREAD_HPP__

#include <string>
#include <atomic>
#include <pthread.h>


/**
 *
 *
 *
 *
 *
*/


template <typename T>
class PThread  //TODO: Find a better name.
{
    friend T;
    public:

        int         Create();
        int         Create(const std::string &name) { _name = name; return Create();}

        void        Join();
        std::string Name() const                    { return _name; } ;
        void        Name(const std::string &name)   { _name = name; } ;

        //virtual void Run() = 0;

    protected:
        static void* RunTh(void *pThData);
    private:
        pthread_t           _th;
        std::string         _name;
        std::atomic_bool    _isJoinable = false;
};




template <typename T>
int PThread<T>::Create()
{
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    int ret = pthread_create( &(_th), &attr, &(PThread::RunTh), this);
    if (0 != ret)
    {
    }

    pthread_attr_destroy(&attr);
    return ret;
}


template <typename T>
void* PThread<T>::RunTh(void *pThData)
{
    T& pThis = *(static_cast <T*>(pThData));

    pThis._isJoinable = true;
    pThis.Run();
    pThis._isJoinable = false;

    return nullptr;
}


template <typename T>
void PThread<T>::Join()
{
    void *stat  = nullptr;

    if (_isJoinable)
        pthread_join(_th, &stat);
    _isJoinable = false;
}




#endif //__PTHREAD_HPP__
