//
//  TaskQueue.cpp
//  CBaseTools
//
//  Created by 李冠军 on 2018/10/15.
//  Copyright © 2018年 liguanjun. All rights reserved.
//

#include "TaskQueue.hpp"

namespace CXXThread {
    
    void QueueTask::beforeRun() {
        std::shared_ptr<TaskQueue> queue = inQueue.lock();
        if (queue) {
            queue->increaseRunningCount();
        }
    }
    
    void QueueTask::afterRun() {
        std::shared_ptr<TaskQueue> queue = inQueue.lock();
        if (queue) {
            queue->decreaseRunningCount();
        }
    }
    
    static std::shared_ptr<GlobalQueue> s_globalQueue;
    static void init() {
        s_globalQueue = std::shared_ptr<GlobalQueue>(new GlobalQueue(2));
    }
    std::shared_ptr<GlobalQueue> TaskQueue::globalQueue() {
        static pthread_once_t once;
        pthread_once(&once, init);
        return s_globalQueue;
    }
    
    TaskQueue::TaskQueue(int maxConcurrentCount): m_maxConcurrentCount(maxConcurrentCount), m_currentQueueIndex(0) {
        
    }
    
    void TaskQueue::addSubQueue(std::shared_ptr<TaskQueue>& subQueue) {
        if (subQueue != nullptr) {
            MutexLockGuard guard(m_lock);
            m_subQueues.push_back(subQueue);
            if (m_runningTaskCount < m_maxConcurrentCount) {
                signal();
            }
        }
    }
    
    void TaskQueue::setSuperQueue(std::shared_ptr<TaskQueue>& superQueue) {
        m_superQueue = superQueue;
    }
    
    void TaskQueue::push(std::shared_ptr<QueueTask> &task) {
        if (task != nullptr) {
            MutexLockGuard guard(m_lock);
            m_taskQueue.push(task);
            signal();
        }
    }
    
    std::shared_ptr<QueueTask> TaskQueue::pop() {
        MutexLockGuard guard(m_lock);
        if (runningTaskCount() >= m_maxConcurrentCount) {
            return nullptr;
            
        } else if(m_currentQueueIndex > 0 && m_currentQueueIndex <= m_subQueues.size()) {
            int currentIndex = m_currentQueueIndex;
            do {
                if (m_currentQueueIndex == 0) {
                    std::shared_ptr<QueueTask> task = popQueueTask();
                    if (task != nullptr) {
                        return task;
                    }
                } else {
                    std::shared_ptr<QueueTask> task = m_subQueues[m_currentQueueIndex-1]->pop();
                    if (task != nullptr) {
                        return task;
                    }
                }
                if (m_currentQueueIndex >= m_subQueues.size()) {
                    m_currentQueueIndex = 0;
                } else {
                    m_currentQueueIndex++;
                }
            } while (currentIndex != m_currentQueueIndex);
            return nullptr;
            
        } else {
            m_currentQueueIndex = 0;
            std::shared_ptr<QueueTask> task = popQueueTask();
            if (task == nullptr) {
                m_currentQueueIndex++;
                return nullptr;
            } else {
                return task;
            }
            
        }
    }
    
    std::shared_ptr<QueueTask> TaskQueue::popQueueTask() {
        if (true == m_taskQueue.empty()) {
            return nullptr;
        } else {
            std::shared_ptr<QueueTask> task = m_taskQueue.front();
            m_taskQueue.pop();
            return task;
        }
    }
    
    int TaskQueue::runningTaskCount() {
        int count = m_runningTaskCount;
        for (int index = 0; index < m_subQueues.size(); index++) {
            count += m_subQueues[index]->runningTaskCount();
        }
        return count;
    }
    
    void TaskQueue::increaseRunningCount() {
        MutexLockGuard guard(m_lock);
        m_runningTaskCount++;
    }
    
    void TaskQueue::decreaseRunningCount() {
        MutexLockGuard guard(m_lock);
        m_runningTaskCount--;
    }
    
    void TaskQueue::signal() {
        std::shared_ptr<TaskQueue> superQueue = m_superQueue.lock();
        if (superQueue != nullptr && m_runningTaskCount < m_maxConcurrentCount) {
            superQueue->signal();
        }
    }
    
    GlobalQueue::GlobalQueue(int maxConcurrentCount): TaskQueue(maxConcurrentCount), m_mutex(), m_condition(m_mutex) {
        
    }
    
    void GlobalQueue::signal() {
        MutexLockGuard guard(m_mutex);
        m_condition.notifyOnce();
//        m_condition.notifyAll();
    }
    
    void GlobalQueue::waitSignal() {
        MutexLockGuard guard(m_mutex);
        m_condition.wait();
    }
    
}
