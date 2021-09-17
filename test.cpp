#include <cstdio>
#include <thread>

//#include "ThreadPool.h"

//using threadpool::CTask;
//using threadpool::CPool;

#include <iostream>
#include "ThreadPool11.hpp"

//class CTest : public CTask {
//    public:
//        CTest(int x): num(x) {}
//        virtual ~CTest() {}
//    void run() {
//        printf("thread %d\n", num);
//    }
//    private :
//        int num;
//};

void func1() {
    std::cout << std::this_thread::get_id() << std::endl;
}

int main() {

//    CPool pool;
//    pool.Create(16);

//    for (int i = 0; i < 2000; ++i) {
//        pool.PushTask(new CTest(i));
//    }

//    getchar();

//    pool.Destroy();

    CPool11 pool;
    pool.startup(16);

    for(size_t i = 0; i < 20000; ++i) {
        pool.append(std::bind(func1));
    }

    pool.shutdown();

    return 0;
}
