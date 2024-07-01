
#include "PThread.hpp"

/**
 *
 *
 *
 *
 *
*/

int PThread::Create()
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

void* PThread::RunTh(void *pThData)
{
    PThread *pThis = static_cast <PThread*>(pThData);

    pThis->_isJoinable = true;
    pThis->Run();
    pThis->_isJoinable = false;

    return nullptr;
}


void PThread::Join()
{
    void *stat  = nullptr;

    if (_isJoinable)
        pthread_join(_th, &stat);
    _isJoinable = false;
}

