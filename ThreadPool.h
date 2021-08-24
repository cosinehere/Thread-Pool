#pragma once

#ifndef _THREADPOOL_H_
#define _THREADPOOL_H_

#include <queue>

#if defined(WIN32)
#include <windows.h>
#include <process.h>
#elif defined(linux)
#include <pthread.h>
#endif

namespace threadpool {

#if defined(WIN32)
typedef HANDLE ThreadType;
#define CREATE_THREAD(thread, fn, arg) thread = (HANDLE)_beginthread(fn, 0, arg)
#define EXIT_THREAD(thread) _endthread()
#define JOIN_THREAD(thread) WaitForSingleObject(thread, INFINITE)
#define TYPE_THREAD void
#define RETURN_THREAD return

typedef HANDLE MutexType;
#define INITIAL_MUTEX(mtx) mtx = CreateMutex(NULL, FALSE, NULL)
#define LOCK_MUTEX(mtx) WaitForSingleObject(mtx, INFINITE)
#define UNLOCK_MUTEX(mtx) ReleaseMutex(mtx)
#define DELETE_MUTEX(mtx) CloseHandle(mtx)

typedef HANDLE ConditionType;
#define INITIAL_COND(cond) cond = CreateEvent(NULL, FALSE, FALSE, NULL)
#define WAIT_COND(cond, timeout, mtx) WaitForSingleObject(cond, timeout)
#define SIGNAL_COND(cond) SetEvent(cond)
#define BROADCAST_COND(cond) SetEvent(cond)
#define DELETE_COND(cond) CloseHandle(cond)

#elif defined(linux)
typedef pthread_t ThreadType;
#define CREATE_THREAD(thread, fn, arg) pthread_create(&thread, nullptr, fn, arg)
#define EXIT_THREAD(thread) pthread_exit(nullptr)
#define JOIN_THREAD(thread) pthread_join(thread, nullptr)
#define TYPE_THREAD void *
#define RETURN_THREAD return nullptr

typedef pthread_mutex_t MutexType;
#define INITIAL_MUTEX(mtx) pthread_mutex_init(&mtx, nullptr)
#define LOCK_MUTEX(mtx) pthread_mutex_lock(&mtx)
#define UNLOCK_MUTEX(mtx) pthread_mutex_unlock(&mtx)
#define DELETE_MUTEX(mtx) pthread_mutex_destroy(&mtx)

typedef pthread_cond_t ConditionType;
#define INITIAL_COND(cond) pthread_cond_init(&cond, nullptr)
#define WAIT_COND(cond, timeout, mtx) pthread_cond_wait(&cond, &mtx)
#define SIGNAL_COND(cond) pthread_cond_signal(&cond)
#define BROADCAST_COND(cond) pthread_cond_broadcast(&cond)
#define DELETE_COND(cond) pthread_cond_destroy(&cond)

#endif

using std::queue;

class CTask {
public:
    virtual ~CTask() {};

    virtual void run() = 0;
};

class CPool;

struct _THREAD {
    ThreadType thread;
    bool run;
    CPool *pool;
};

class CPoolMutex {
public:
    CPoolMutex() { INITIAL_MUTEX(_mtx); }
    ~CPoolMutex() { DELETE_MUTEX(_mtx); }
    void Lock() { LOCK_MUTEX(_mtx); }
    void Unlock() { UNLOCK_MUTEX(_mtx); }

public:
    MutexType _mtx;
};

class CPoolCond {
public:
    CPoolCond() { INITIAL_COND(_cond); }
    ~CPoolCond() { DELETE_COND(_cond); }

    void Wait(int timeout, MutexType &mtx) { WAIT_COND(_cond, timeout, mtx); }

    void Notify() { SIGNAL_COND(_cond); }
    void NotifyAll() { BROADCAST_COND(_cond); }
public:
    ConditionType _cond;
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

