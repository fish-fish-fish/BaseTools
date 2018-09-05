//
//  LICurlTask.m
//  ios-learn
//
//  Created by liguanjun on 2018/5/17.
//  Copyright © 2018年 liguanjun. All rights reserved.
//

#import "LICurlTask.h"
#include "CurlTask.hpp"
#import "AutoRelease.mm"
using namespace CXXThread;
using namespace std;

class _CurlTaskDelegate: public CurlTaskDelegate {
private:
    LICurlTask *task;
public:
    _CurlTaskDelegate(LICurlTask *task) {
        this->task = task;
    }
    
    virtual void didResumed() {
        AutoRelease((NSObject *)task);
        if (task && [task.delegate respondsToSelector:@selector(downloadTaskDidResumed:)]) {
            [task.delegate downloadTaskDidResumed:task];
        }
    }
    virtual void didPaused() {
        AutoRelease((NSObject *)task);
        if (task && [task.delegate respondsToSelector:@selector(downloadTaskDidPaused:)]) {
            [task.delegate downloadTaskDidPaused:task];
        }
    }
    virtual void receiveResponse(const std::string &response) {
        AutoRelease((NSObject *)task);
        if (task && [task.delegate respondsToSelector:@selector(downloadTask:recivedResponse:)]) {
            [task.delegate downloadTask:task recivedResponse:[NSString stringWithUTF8String:response.c_str()]];
        }
    }
    virtual void receiveData(char *buffer, size_t size) {
        AutoRelease((NSObject *)task);
        if (task && [task.delegate respondsToSelector:@selector(downloadTask:recivedData:size:)]) {
            [task.delegate downloadTask:task recivedData:buffer size:size];
        }
    }
    virtual void updateProgress(const double dltotal, const double dlnow) {
        AutoRelease((NSObject *)task);
        if (task && [task.delegate respondsToSelector:@selector(downloadTask:updateProgress:total:)]) {
            [task.delegate downloadTask:task updateProgress:dlnow total:dltotal];
        }
    }
    virtual void downloadComleted() {
        AutoRelease((NSObject *)task);
        if (task && [task.delegate respondsToSelector:@selector(downloadTaskComleted:)]) {
            [task.delegate downloadTaskComleted:task];
        }
    }
    virtual void downloadFailed(int code) {
        AutoRelease((NSObject *)task);
        if (task && [task.delegate respondsToSelector:@selector(downloadTaskDidResumed:)]) {
            [task.delegate downloadTask:task failed:code];
        }
    }
};


@interface LICurlTask ()
@property(nonatomic, copy) NSString *url;

@property(nonatomic, assign) NSUInteger startPosition;

@property(nonatomic, weak) id <LIBaseCurlTaskDelegate> curlTaskDelegate;

@property(nonatomic, assign) BOOL isPaused;
@end

@implementation LICurlTask
{
    std::shared_ptr<CurlTask> _task;
    std::shared_ptr<_CurlTaskDelegate> _taskDelegate;
}

- (instancetype)init {
    return nil;
}

+ (instancetype)taskWithUrl:(NSString *)url {
    return [[[LICurlTask alloc] initWithUrl:url] autorelease];
}

+ (instancetype)taskWithUrl:(NSString *)url rangeFrom:(NSUInteger)startPosition {
    return [[[LICurlTask alloc] initWithUrl:url rangeFrom:startPosition] autorelease];
}

- (instancetype)initWithUrl:(NSString *)url {
    if (self = [super init]) {
        _url = [url copy];
        _startPosition = 0;
        _task = shared_ptr<CurlTask>(nullptr);
        _taskDelegate = shared_ptr<_CurlTaskDelegate>(nullptr);
    }
    return self;
}

- (instancetype)initWithUrl:(NSString *)url rangeFrom:(NSUInteger)startPosition {
    if (self = [super init]) {
        _url = [url copy];
        _startPosition = startPosition;
        _task = shared_ptr<CurlTask>(nullptr);
        _taskDelegate = shared_ptr<_CurlTaskDelegate>(nullptr);
    }
    return self;
}

- (void)dealloc {
    [_url release];
    _url = nil;
    
    [super dealloc];
}

- (std::shared_ptr<CurlTask>&)task {
    if (_task.get() == nullptr) {
        _task =  unique_ptr<CurlTask>(new CurlTask(std::string([self.url cStringUsingEncoding:NSUTF8StringEncoding]), self.startPosition));
        std::shared_ptr<CurlTaskDelegate> delegate = std::static_pointer_cast<CurlTaskDelegate>([self taskDelegate]);
        _task->setDelegate(delegate);
    }
    return _task;
}

- (std::shared_ptr<_CurlTaskDelegate>&)taskDelegate {
    if (_taskDelegate.get() == nullptr) {
        _taskDelegate =  shared_ptr<_CurlTaskDelegate>(new _CurlTaskDelegate(self));
    }
    return _taskDelegate;
}

- (void)setDelegate:(id<LIBaseCurlTaskDelegate>)delegate {
    self.curlTaskDelegate = delegate;
}

- (id<LIBaseCurlTaskDelegate>)delegate {
    return self.curlTaskDelegate;
}

- (BOOL)isPaused {
    return self.task->isPaused();
}

- (void)start {
    shared_ptr<CurlTask>& curlTask = [self task];
    shared_ptr<Task> task = std::static_pointer_cast<Task>(curlTask);
    ThreadPool::sharedInstance().push(task);
}

- (void)resum {
    self.task->resum();
}

- (void)pause {
    self.task->pause();
}

- (void)cancel {
    self.task->cancel();
}


@end
