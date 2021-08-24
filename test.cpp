#include <cstdio>

#include "ThreadPool.h"

using threadpool::CTask;
using threadpool::CPool;

class CTest : public CTask {
    public:
        CTest(int x): num(x) {}
        virtual ~CTest() {}
    void run() {
        printf("thread %d\n", num);
    }
    private :
        int num;
};

int main() {

    CPool pool;
    pool.Create(16);

    for (int i = 0; i < 2000; ++i) {
        pool.PushTask(new CTest(i));
    }

    getchar();

    pool.Destroy();

    return 0;
}
