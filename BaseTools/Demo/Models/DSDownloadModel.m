//
//  DSDownloadModel.m
//  BaseTools
//
//  Created by liguanjun on 2017/4/12.
//  Copyright © 2017年 liguanjun. All rights reserved.
//

#import "DSDownloadModel.h"
#import <time.h>
#import "LICurlTask.h"
#import "LIDownloadTask.h"
#import "DSManager.h"

#define UsingCurlTask 0

static NSString *libraryPath = nil;
@interface DSDownloadModel () <LIBaseCurlTaskDelegate>
{
    time_t _lastTimeOfUpdate;
}
@property(nonatomic, strong) id<LIBaseCurlTaskPtotocol> task;
@property(nonatomic, assign) long long rangeFrom;
@property(nonatomic, strong) NSFileHandle *tempFile;
@end

@implementation DSDownloadModel

- (instancetype)initWithDict:(NSDictionary *)dict {
    if (self = [super initWithDict:dict]) {
        if (libraryPath == nil) {
            libraryPath = NSSearchPathForDirectoriesInDomains(NSLibraryDirectory, NSUserDomainMask,YES).firstObject;
            NSLog(@"libraryPath: %@", libraryPath);
        }
        NSString *fullPath = [[DSManager shared].rootDir stringByAppendingPathComponent:self.name];
        if ([[NSFileManager defaultManager] fileExistsAtPath:fullPath]) {
            _absPath = fullPath;
            _totalSize = -1;
            _progress = 1.0;
            _progressInfo = @"已完成下载";
            _status = DSDownloadModelStatus_completed;
            
        }else if ([[NSFileManager defaultManager] fileExistsAtPath:[fullPath stringByAppendingString:@".temp"]]) {
            _tempAbsPath = [fullPath stringByAppendingString:@".temp"];
            NSDictionary<NSFileAttributeKey, id> *attr = [[NSFileManager defaultManager] attributesOfItemAtPath:_tempAbsPath error:nil];
            _rangeFrom = attr.fileSize;
            _totalSize = -1;
            _progress = -1.0;
            _progressInfo = @"下载未完成";
//            _progress = (float)attr.fileSize / (float)self.size;
//            _progressInfo = [NSString stringWithFormat:@"%llu / %ld",attr.fileSize, (long)self.size];
            _status = DSDownloadModelStatus_failed;
        }else{
            _absPath = nil;
            _tempAbsPath = nil;
            _rangeFrom = 0;
            _totalSize = -1;
            _progress = 0.0;
            _progressInfo = @"未下载";
            _status = DSDownloadModelStatus_none;
        }
    }
    return self;
}

- (id<LIBaseCurlTaskPtotocol>)task {
    if (_task == nil) {
        // 支持断点续传
        NSDictionary<NSFileAttributeKey, id> *attr = [[NSFileManager defaultManager] attributesOfItemAtPath:_tempAbsPath error:nil];
        _rangeFrom = attr.fileSize;
        
        if (UsingCurlTask) {
            _task = [[LICurlTask alloc] initWithUrl:self.url rangeFrom:self.rangeFrom];
        } else {
            _task = [[LIDownloadTask alloc] initWithUrl:self.url rangeFrom:self.rangeFrom];
        }
        _task.delegate = self;
    }
    return _task;
}

- (void)start {
    [self.task start];    
}

- (NSFileHandle *)tempFile {
    if (_tempFile == nil) {
        if (_tempAbsPath == nil) {
            _tempAbsPath = [[[DSManager shared].rootDir stringByAppendingPathComponent:self.name] stringByAppendingString:@".temp"];
        }
        NSLog(@"_tempAbsPath : %@", _tempAbsPath);
        
        if (![[NSFileManager defaultManager] fileExistsAtPath:[_tempAbsPath stringByDeletingLastPathComponent]]) {
            [[NSFileManager defaultManager] createDirectoryAtPath:[_tempAbsPath stringByDeletingLastPathComponent] withIntermediateDirectories:YES attributes:nil error:nil];
        }
        
        if (![[NSFileManager defaultManager] fileExistsAtPath:_tempAbsPath]) {
            [[NSFileManager defaultManager] createFileAtPath:_tempAbsPath contents:nil attributes:nil];
        }
        _tempFile = [NSFileHandle fileHandleForWritingAtPath:_tempAbsPath];
    }
    return _tempFile;
}

- (void)resum {
    [self.task resum];
}

- (void)pause {
    [self.task pause];
}

- (void)cancel {
    [self.task cancel];
    [self.tempFile synchronizeFile];
    [self.tempFile closeFile];
    _tempFile = nil;
    _status = DSDownloadModelStatus_failed;
    
    [self updateCell];
}

- (void)remove {
    if (_status != DSDownloadModelStatus_completed && _status != DSDownloadModelStatus_failed) {
        return;
    }
    if (self.absPath) {
        [[NSFileManager defaultManager] removeItemAtPath:self.absPath error:nil];
        _absPath = nil;
    }
    if (self.tempAbsPath) {
        [[NSFileManager defaultManager] removeItemAtPath:self.tempAbsPath error:nil];
        _tempAbsPath = nil;
    }
    
    _progress = 0;
    _progressInfo = @"未下载";
    _status = DSDownloadModelStatus_none;
    
    [self updateCell];
}

- (void)updateCell {
    [self.delegate downloadModelDidUpdated:self];
    _lastTimeOfUpdate = time(NULL);
}

#pragma mark - LIDownloadTaskDelegate
- (void)downloadTask:(id<LIBaseCurlTaskPtotocol>)task recivedData:(char *)buffer size:(NSUInteger)size {
    [self.tempFile seekToEndOfFile];
    [self.tempFile writeData:[[NSData alloc] initWithBytesNoCopy:(void *)buffer length:size freeWhenDone:NO]];
}

- (void)downloadTask:(id<LIBaseCurlTaskPtotocol>)task recivedResponse:(NSString *)response {
    NSLog(@"%@",response);
}

- (void)downloadTask:(id<LIBaseCurlTaskPtotocol>)task updateProgress:(NSUInteger)dlnow total:(NSUInteger)dltotal {
    _totalSize = dltotal;
    _progressInfo = [NSString stringWithFormat:@"%lu / %lu",(unsigned long)dlnow, (unsigned long)dltotal];
    _progress = (float)dlnow / (float)dltotal;
    _status = DSDownloadModelStatus_downloading;
    
    time_t nowtime = time(NULL);
    if (nowtime > _lastTimeOfUpdate) {
        [self updateCell];
    }
}

- (void)downloadTaskComleted:(id<LIBaseCurlTaskPtotocol>)task {
    [self.tempFile synchronizeFile];
    [self.tempFile closeFile];
    _tempFile = nil;
    _absPath = [[DSManager shared].rootDir stringByAppendingPathComponent:self.name];
    [[NSFileManager defaultManager] moveItemAtPath:self.tempAbsPath toPath:self.absPath error:nil];
    _tempAbsPath = nil;
    _progressInfo = @"下载已完成";
    _status = DSDownloadModelStatus_completed;
    _task.delegate = nil;
    _task = nil;
    
    [self updateCell];
    
}

- (void)downloadTask:(id<LIBaseCurlTaskPtotocol>)task failed:(NSInteger)code {
    [self.tempFile synchronizeFile];
    [self.tempFile closeFile];
    _tempFile = nil;
    _progressInfo = @"下载发生错误";
    _status = DSDownloadModelStatus_failed;
    _task.delegate = nil;
    _task = nil;
    
    [self updateCell];
}

- (void)downloadTaskDidResumed:(id<LIBaseCurlTaskPtotocol>)task {
    _status = DSDownloadModelStatus_downloading;
    
    [self updateCell];
}

- (void)downloadTaskDidPaused:(id<LIBaseCurlTaskPtotocol>)task {
    _status = DSDownloadModelStatus_paused;
    
    [self updateCell];
}

@end
