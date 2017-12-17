//
// Created by zkl on 12/17/17.
//

#include "MyThreadPool.h"


namespace MyThreadPoolSpace {
    MyThreadPool *MyThreadPool::threadPool = nullptr;

    pthread_mutex_t MyThreadPool::initMutex = PTHREAD_MUTEX_INITIALIZER;

    void *threadFunc(void *arg) {

        MyThreadPool *pool = static_cast<MyThreadPool *>(arg);

        while (1) {
            pthread_mutex_lock(&pool->workMutex);
            cout << "thread : " << pthread_self() << " into loop " << endl;
            pthread_cond_wait(&pool->workCondMutex, &pool->workMutex);

            cout << "thread : " << pthread_self() << " will unlock" << endl;
            pthread_mutex_unlock(&pool->workMutex);

            if (!pool->startFlag) {
                cout << "thread : " << pthread_self() << " will end " << endl;
                break;
            }
            if (pool->workQueue.size() == 0) {
                // nothing to do , just wait
                cout << "thread : " << pthread_self() << " nothing can do " << endl;
                continue;
            }

            // ==================thread safe ===================
            pthread_mutex_lock(&pool->workCountMutex);
            Work *work = pool->workQueue.front();
            pool->workQueue.pop();
            ++pool->workCount;
            pthread_mutex_unlock(&pool->workCountMutex);
            // ==================thread safe ===================

            cout << "thread : " << pthread_self() << " start work" << endl;
            work->func(work->arg);

            // ==================thread safe ===================
            pthread_mutex_lock(&pool->workCountMutex);
            --pool->workCount;
            pthread_mutex_unlock(&pool->workCountMutex);
            // ==================thread safe ===================

            if (!pool->startFlag) {
                cout << "after work thread : " << pthread_self() << " will end" << endl;
                break;
            } else {
                // notify thread again to check another work
                cout << "=========check again work count : " << pool->getWorkCount() << endl;
                pthread_cond_broadcast(&pool->workCondMutex);
            }
        }
        cout << "thread : " << pthread_self() << " is end " << endl;
    }

    MyThreadPool *MyThreadPool::getInstance() {

        if (threadPool == nullptr) {
            pthread_mutex_lock(&initMutex);

            threadPool = new MyThreadPool;

            // create thread
            threadPool->initThread();

            pthread_mutex_unlock(&initMutex);
        }

        return threadPool;
    }

/**
 * already been locked so thread safe
 */
    void MyThreadPool::initThread() {

        if (this->startFlag) {
            cout << "start flag is already true" << endl;
            return;
        }

        this->startFlag = true;

        if (threads.size() == 0) {
            threads.clear();

            while (workQueue.size() > 0) {
                workQueue.pop();
            }

            for (int x = 0; x < this->maxCount; ++x) {
                pthread_t tid;
                pthread_create(&tid, nullptr, threadFunc, this);

                // add into threads
                threads.push_back(tid);
            }

        } else {
            cout << "pool full error" << endl;
        }
    }

    int MyThreadPool::getWorkCount(){
        return this->workCount;
    }

    void MyThreadPool::addWork(Work *work) {
        pthread_mutex_lock(&this->addWorkMutex);

        cout << "will push " << (int) (intptr_t) work->arg << endl;
        workQueue.push(work);

        pthread_mutex_unlock(&this->addWorkMutex);

        cout << "will notify" << endl;
        // notify
        pthread_cond_broadcast(&this->workCondMutex);
    }

    void MyThreadPool::recylerThread() {
        for (int x = 0; x < this->threads.size(); ++x) {
            cout << "thread join : " << this->threads[x] << endl;
            pthread_join(this->threads[x], nullptr);
        }
        cout << "recyler Thread over" << endl;
    }

    void MyThreadPool::stop() {
        cout << "will to stop" << endl;
        pthread_mutex_lock(&this->stopMutex);

        this->startFlag = false;

        cout << "send broadcast" << endl;
        // broadcast
        pthread_cond_broadcast(&this->workCondMutex);

        pthread_mutex_unlock(&this->stopMutex);

        cout << "send broadcast over" << endl;

        this->recylerThread();
    }
}