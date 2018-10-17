//
//  CXXThreadPool.cpp
//  ios-learn
//
//  Created by liguanjun on 2018/5/14.
//  Copyright © 2018年 liguanjun. All rights reserved.
//

#include "CXXThreadPool.hpp"
#include "TaskQueue.hpp"

namespace CXXThread {
    
#pragma mark - ThreadPool
    static ThreadPool *pInstance;
    ThreadPool& ThreadPool::sharedInstance() {
        static pthread_once_t once;
        pthread_once(&once, &ThreadPool::init);
        return *pInstance;
    }
    
    void ThreadPool::init() {
        pInstance = new ThreadPool();
    }
    
    
    ThreadPool::ThreadPool(): m_threads(), m_mutex(), m_condition(m_mutex) {
        m_taskQueue = std::make_shared<TaskQueue>(1);
    }
    
    void ThreadPool::start(int numThreads) {
        for (int i=0; i < numThreads; ++i) {
            std::unique_ptr<TaskThread> thread(new TaskThread());
            thread->start();
            m_threads.push_back( std::move(thread) );
        }
    }
    
#pragma mark - TaskThread
    void TaskThread::body() {
        while (1) {
            std::shared_ptr<QueueTask> task = TaskQueue::globalQueue()->pop();
            if (task != nullptr) {
                task->beforeRun();
                task->operator()();
                task->afterRun();
            } else {
                TaskQueue::globalQueue()->waitSignal();
            }
        }
    }
    
}
