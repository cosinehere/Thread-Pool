#pragma once

#ifndef _THREADPOOL_H_
#define _THREADPOOL_H_

#include <queue>

#ifdef linux
#include <pthread.h>
#endif

namespace threadpool {

using std::queue;

class CTask {
public:
    virtual ~CTask() {};

    virtual void run() = 0;
};

class CPool;

struct _THREAD {
    pthread_t thread;
    bool run;
    CPool *pool;
};

class CPoolMutex {
    public:
        CPoolMutex() { pthread_mutex_init(&_mtx, nullptr); }
        ~CPoolMutex() { pthread_mutex_destroy(&_mtx); }
        void Lock() { pthread_mutex_lock(&_mtx); }
        void Unlock() { pthread_mutex_unlock(&_mtx); }

    public:
        pthread_mutex_t _mtx;
};

class CPoolCond {
    public:
        CPoolCond() { pthread_cond_init(&_cond, nullptr); }
        ~CPoolCond() { pthread_cond_destroy(&_cond); }

        void Wait(CPoolMutex &mtx) { pthread_cond_wait(&_cond, &mtx._mtx); }
        void Notify() { pthread_cond_signal(&_cond); }
        void NotifyAll() { pthread_cond_broadcast(&_cond); }
    public:
        pthread_cond_t _cond;
};

class CPool {
public:
    CPool();
    ~CPool();

    bool Create(int threadnum);
    bool Destroy();

    bool PushTask(CTask *task);

    void Loop(void *arg);

private:
    bool p_run;
    int p_threadnum;

    _THREAD *p_threads;
    queue<CTask *> p_taskque;

    CPoolMutex p_mtx;
    CPoolCond p_cond;
};

} // namespace threadpool

#endif // _THREADPOOL_H_

