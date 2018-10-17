//
//  LITaskQueue.m
//  BaseTools
//
//  Created by 李冠军 on 2018/10/17.
//  Copyright © 2018年 liguanjun. All rights reserved.
//

#import "LITaskQueue.h"
#import "TaskQueue.hpp"

using namespace std;
using namespace CXXThread;

class BlockTask: public QueueTask {
public:
    BlockTask(OCBlock block) {
        m_block = [block copy];
    }
    
    ~BlockTask() {
        m_block = nil;
    }
protected:
    virtual void operator() () {
        if (m_block) {
            m_block();
        }
    }
    
private:
    __strong OCBlock m_block;
};


@implementation LITaskQueue
{
    std::shared_ptr<TaskQueue> _queue;
}

+(LITaskQueue *)globalQueue {
    static dispatch_once_t onceToken;
    static LITaskQueue *_globalQueue;
    dispatch_once(&onceToken, ^{
        _globalQueue = [[self alloc] initGlobalQueue];
    });
    return _globalQueue;
}

- (instancetype)initGlobalQueue {
    self = [super init];
    if (self) {
        _queue = TaskQueue::globalQueue();
    }
    return self;
}

- (instancetype)init {
    self = [super init];
    if (self) {
        _queue = std::make_shared<TaskQueue>(1);
    }
    return self;
}

- (instancetype)initWithMaxConcurrentCount:(NSInteger)maxConcurrentCount {
    self = [super init];
    if (self) {
        _queue = std::make_shared<TaskQueue>(maxConcurrentCount);
    }
    return self;
}

- (void)pushTask:(void (^)())task {
    BlockTask *blockTask = new BlockTask(task);
    std::shared_ptr<QueueTask> p(blockTask);
    p->inQueue = _queue;
    _queue->push(p);
}

- (void)addSubQueue:(LITaskQueue *)subQueue {
    if (subQueue) {
        subQueue->_queue->setSuperQueue(_queue);
        _queue->addSubQueue(subQueue->_queue);
    }
}

@end
