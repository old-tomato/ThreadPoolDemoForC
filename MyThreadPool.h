//
// Created by zkl on 12/17/17.
//

#ifndef TEST_THREAD_POOL_MYTHREADPOOL_H
#define TEST_THREAD_POOL_MYTHREADPOOL_H

#include <pthread.h>
#include <iostream>
#include <vector>
#include <queue>

using namespace std;

namespace MyThreadPoolSpace{

    struct Work{
        void * arg;
        void * (* func)(void * arg);
    };

    class MyThreadPool {
        friend void * threadFunc(void * arg);
    private:

        int maxCount =3;
        int workCount = 0;

        bool startFlag = false;

        static pthread_mutex_t initMutex;

        pthread_mutex_t workMutex = PTHREAD_MUTEX_INITIALIZER;

        pthread_cond_t workCondMutex = PTHREAD_COND_INITIALIZER;

        pthread_mutex_t addWorkMutex = PTHREAD_MUTEX_INITIALIZER;

        pthread_mutex_t stopMutex = PTHREAD_MUTEX_INITIALIZER;

        pthread_mutex_t workCountMutex = PTHREAD_MUTEX_INITIALIZER;

        vector<pthread_t> threads;

        queue<Work *> workQueue;

        static MyThreadPool * threadPool;

        MyThreadPool(){};

        void initThread();

        void recylerThread();

    public:

        static MyThreadPool * getInstance();

        void addWork(Work * work);

        void stop();

        int getWorkCount();
    };
}


#endif //TEST_THREAD_POOL_MYTHREADPOOL_H
