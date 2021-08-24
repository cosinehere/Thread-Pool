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
    pool.Create(4);

    for (int i = 0; i < 100; ++i) {
        pool.PushTask(new CTest(i));
    }

    getchar();

    pool.Destroy();

    return 0;
}
