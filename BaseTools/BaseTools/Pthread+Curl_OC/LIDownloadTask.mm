//
//  LIDownloadTask.mm
//  ios-learn
//
//  Created by liguanjun on 2017/4/12.
//  Copyright © 2017年 liguanjun. All rights reserved.
//

#import "LIDownloadTask.h"
#include "DownloadTask.hpp"
#import "AutoRelease.mm"
using namespace CXXThread;
using namespace std;

class DownloadTaskDelegate: public DownloadTask
{
private:
    Handle _native;
public:
    DownloadTaskDelegate(Handle handle, string url):DownloadTask(url)
    {
        _native = handle;
    }
    
    void didResumed()
    {
        LIDownloadTask *task = (LIDownloadTask *)_native;
        AutoRelease _(task);
        if (task && [task.delegate respondsToSelector:@selector(downloadTaskDidResumed:)]) {
            [task.delegate downloadTaskDidResumed:task];
        }
    }
    
    void didPaused()
    {
        LIDownloadTask *task = (LIDownloadTask *)_native;
        AutoRelease _(task);
        if (task && [task.delegate respondsToSelector:@selector(downloadTaskDidPaused:)]) {
            [task.delegate downloadTaskDidPaused:task];
        }
    }
    
    void receiveResponse(const string &response)
    {
        LIDownloadTask *task = (LIDownloadTask *)_native;
        AutoRelease _(task);
        if (task && [task.delegate respondsToSelector:@selector(downloadTask:recivedResponse:)]) {
            [task.delegate downloadTask:task recivedResponse:[NSString stringWithUTF8String:response.c_str()]];
        }
    }
    void receiveData(char *buffer, size_t size)
    {
        LIDownloadTask *task = (LIDownloadTask *)_native;
        AutoRelease _(task);
        if (task && [task.delegate respondsToSelector:@selector(downloadTask:recivedData:size:)]) {
            [task.delegate downloadTask:task recivedData:buffer size:size];
        }
    }
    void updateProgress(const double dltotal, const double dlnow)
    {
        LIDownloadTask *task = (LIDownloadTask *)_native;
        AutoRelease _(task);
        if (task && [task.delegate respondsToSelector:@selector(downloadTask:updateProgress:total:)]) {
            [task.delegate downloadTask:task updateProgress:dlnow total:dltotal];
        }
    }
    void downloadComleted()
    {
        LIDownloadTask *task = (LIDownloadTask *)_native;
        AutoRelease _(task);
        if (task && [task.delegate respondsToSelector:@selector(downloadTaskComleted:)]) {
            [task.delegate downloadTaskComleted:task];
        }
    }
    void downloadFailed(int code)
    {
        LIDownloadTask *task = (LIDownloadTask *)_native;
        AutoRelease _(task);
        if (task && [task.delegate respondsToSelector:@selector(downloadTask:failed:)]) {
            [task.delegate downloadTask:task failed:code];
        }
    }
};



@interface LIDownloadTask ()
@property(nonatomic, retain) NSString *url;

@property(nonatomic, assign) NSUInteger startPosition;

@property(nonatomic, weak) id <LIBaseCurlTaskDelegate> curlTaskDelegate;

@property(nonatomic, assign) BOOL isPaused;
@end

@implementation LIDownloadTask
{
    std::unique_ptr<DownloadTaskDelegate> _taskDelegate;
}
- (instancetype)init
{
    return nil;
}

+(instancetype)taskWithUrl:(NSString *)url
{
    return [[[LIDownloadTask alloc] initWithUrl:url] autorelease];
}

+(instancetype)taskWithUrl:(NSString *)url rangeFrom:(NSUInteger)startPosition
{
    return [[[LIDownloadTask alloc] initWithUrl:url rangeFrom:startPosition] autorelease];
}

-(instancetype)initWithUrl:(NSString *)url
{
    if (self = [super init]) {
        _url = [url copy];
        _startPosition = 0;
        _taskDelegate = unique_ptr<DownloadTaskDelegate>(nullptr);
    }
    return self;
}

-(instancetype)initWithUrl:(NSString *)url rangeFrom:(NSUInteger)startPosition
{
    if (self = [super init]) {
        _url = [url copy];
        _startPosition = startPosition;
        _taskDelegate = unique_ptr<DownloadTaskDelegate>(nullptr);
    }
    return self;
}

-(void)dealloc
{
    [_url release];
    _url = nil;
    
    [super dealloc];
}

-(std::unique_ptr<DownloadTaskDelegate>&)taskDelegate
{
    if (_taskDelegate.get() == nullptr) {
        _taskDelegate =  unique_ptr<DownloadTaskDelegate>(new DownloadTaskDelegate((Handle)self,string([self.url cStringUsingEncoding:NSUTF8StringEncoding])));
    }
    return _taskDelegate;
}

-(void)setDelegate:(id<LIBaseCurlTaskDelegate>)delegate {
    self.curlTaskDelegate = delegate;
}

- (id<LIBaseCurlTaskDelegate>)delegate {
    return self.curlTaskDelegate;
}

-(BOOL)isPaused
{
    return self.taskDelegate->isPaused();
}

-(void)start
{
    self.taskDelegate->start();
}

-(void)resum
{
    self.taskDelegate->resum();
}

-(void)pause
{
    self.taskDelegate->pause();
}

-(void)cancel
{
    self.taskDelegate->cancel();
}

@end
