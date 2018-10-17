//
//  LIThreadPool.m
//  BaseTools
//
//  Created by liguanjun on 2018/9/6.
//  Copyright © 2018年 liguanjun. All rights reserved.
//

#import "LIThreadPool.h"
#import "CXXThreadPool.hpp"
#import "TaskQueue.hpp"

using namespace CXXThread;
using namespace std;

@implementation LIThreadPool
{
    ThreadPool *_pool;
}

+ (LIThreadPool *)shared {
    static dispatch_once_t onceToken;
    static LIThreadPool *pool = nil;
    dispatch_once(&onceToken, ^{
        pool = [[self alloc] initWithPrivate];
    });
    return pool;
}

- (instancetype)init {
    return nil;
}

- (instancetype)initWithPrivate {
    self = [super init];
    if (self) {
        _pool = &ThreadPool::sharedInstance();
    }
    return self;
}

- (void)startWithThreads:(NSUInteger)count {
    _pool->start((int)count);
}
@end
