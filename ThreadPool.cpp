#include "ThreadPool.h"

namespace threadpool {

CPool::CPool() {
    while (!p_taskque.empty()) {
        p_taskque.pop();
    }

    p_run = false;
    p_threadnum = 0;
    p_threads = nullptr;
}

CPool::~CPool() {
    if (p_threads == nullptr) {
        Destroy();
    }
}

TYPE_THREAD threadinit(void *arg) {
    _THREAD *thread = static_cast<_THREAD*>(arg);
    thread->pool->Loop();

    EXIT_THREAD;
    RETURN_THREAD;
}

bool CPool::Create(int threadnum) {
    if (p_threads != nullptr || threadnum <= 0 || threadnum > 16) { return false; }

    p_threadnum = threadnum;

    p_threads = new _THREAD[threadnum];

    p_run = true;

    for (int i = 0; i < threadnum; ++i) {
        p_threads[i].pool = this;
        CREATE_THREAD(p_threads[i].thread, threadinit, &p_threads[i]);
    }

    return true;
}

bool CPool::Destroy() {
    if (p_threads == nullptr) { return false; }

    p_run = false;

    for (int i = 0; i < p_threadnum; ++i) {
        p_cond.NotifyAll();
        JOIN_THREAD(p_threads[i].thread);
        DELETE_THREAD(p_threads[i].thread);
    }

    delete[] p_threads;
    p_threads = nullptr;

    return true;
}

bool CPool::PushTask(CTask *task) {
    if (task == nullptr) { return false; }

    p_mtx.Lock();

    p_taskque.push(task);

    p_cond.Notify();
    p_mtx.Unlock();

    return true;
}

void CPool::Loop() {
    while (p_run) {
        p_mtx.Lock();
        while (p_run && p_taskque.size() == 0) {
#if defined(WIN32)
            p_mtx.Unlock();
#endif
            p_cond.Wait(10, p_mtx._mtx);
#if defined(WIN32)
            p_mtx.Lock();
#endif
        }

        if (!p_run) {
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
