
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


class PThread  //TODO: Find a better name.
{
    public:

        int         Create();
        int         Create(const std::string &name) { _name = name; return Create();}

        void        Join();
        std::string Name() const                    { return _name; } ;
        void        Name(const std::string &name)   { _name = name; } ;

        virtual void Run() = 0;

    protected:
        static void* RunTh(void *pThData);
    private:
        pthread_t           _th;
        std::string         _name;
        std::atomic_bool    _isJoinable = false;
};


#endif //__PTHREAD_HPP__
