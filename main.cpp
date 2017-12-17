#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include "./MyThreadPool.h"

using namespace std;
using namespace MyThreadPoolSpace;

void * threadFunc(void * arg){

    int num = (int)(intptr_t)arg;

    int start = num;

    cout << "in function start : " << start << endl;

    while(num < (start + 5)){
        sleep(1);
        if(start % 2 == 0){
            cout << "==> " << num << endl;
        }else{
            cout << num << endl;
        }
        ++ num;
    }
}

int main() {

    MyThreadPool *thread = MyThreadPool::getInstance();

    sleep(2);

    for(int x = 0 ; x < 10; ++ x){
        Work * work = new Work;
        work->arg = (void *)(intptr_t)(20 + x);
        cout << (int)(intptr_t)work->arg << endl;
        work->func = threadFunc;

        thread->addWork(work);
    }

    sleep(15);

    thread->stop();

    return 0;
}