#include "ThreadPool.h"
#include <pthread.h>

namespace threadpool {

CPool::CPool() {
    while (!p_taskque.empty()) {
        p_taskque.pop();
    }

    p_threadnum = 0;
    p_threads = nullptr;
}

CPool::~CPool() {}

#if defined(WIN32)
unsigned int threadinit(void *arg) {
#elif defined(linux)
void *threadinit(void *arg) {
#endif
    _THREAD *thread = static_cast<_THREAD*>(arg);
    thread->pool->Loop(arg);

    return 0;
}

bool CPool::Create(int threadnum) {
    if (p_threads != nullptr || threadnum <= 0 || threadnum > 16) { return false; }

    p_threadnum = threadnum;

    p_threads = new _THREAD[threadnum];

    for (int i = 0; i < threadnum; ++i) {
        p_threads[i].run = true;
        p_threads[i].pool = this;
#if defined(WIN32)
        p_threads[i].thread = (HANDLE)_beginthreadex(nullptr, 0, threadinit, &p_threads[i], 0, nullptr);
#elif defined(linux)
        pthread_create(&p_threads[i].thread, nullptr, threadinit, &p_threads[i]);
        pthread_detach(p_threads[i].thread);
#endif
    }

    return true;
}

bool CPool::Destroy() {
    if (p_threads == nullptr) { return false; }

    for (int i = 0; i < p_threadnum; ++i) { p_threads[i].run = false; }

    p_mtx.Lock();
    p_cond.NotifyAll();
    p_mtx.Unlock();

    delete[] p_threads;

    return true;
}

bool CPool::PushTask(CTask *task){
    if (task == nullptr) { return false; }

    p_mtx.Lock();

    p_taskque.push(task);

    p_cond.Notify();
    p_mtx.Unlock();

    return true;
}

void CPool::Loop(void *arg) {
    _THREAD *thread = static_cast<_THREAD*>(arg);

    while (thread->run) {
        p_mtx.Lock();
        while (p_taskque.size() == 0) {
            if (!thread->run) { break; }
            p_cond.Wait(p_mtx);
        }

        if (!thread->run) {
            p_mtx.Unlock();
            break;
        }

        CTask *task = p_taskque.front();
        p_taskque.pop();
        
        p_mtx.Unlock();

        task->run();

        delete task;
    }
}

} // namespace threadpool
