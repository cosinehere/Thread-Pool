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

CPool::~CPool() {
    if (p_threads != nullptr) {
        Destroy();
    }
}

void *threadinit(void *arg) {
    _THREAD *thread = static_cast<_THREAD*>(arg);
    thread->pool->Loop(arg);

    return nullptr;
}

bool CPool::Create(int threadnum) {
    if (p_threads != nullptr || threadnum <= 0 || threadnum > 16) { return false; }

    p_threadnum = threadnum;

    p_threads = new _THREAD[threadnum];

    for (int i = 0; i < threadnum; ++i) {
        p_threads[i].run = true;
        p_threads[i].pool = this;
        pthread_create(&p_threads[i].thread, nullptr, threadinit, &p_threads[i]);
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