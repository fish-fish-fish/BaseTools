//
//  LIBaseCurlTaskPtotocol.h
//  ios-learn
//
//  Created by liguanjun on 2017/4/12.
//  Copyright © 2017年 liguanjun. All rights reserved.
//

#import <Foundation/Foundation.h>

@protocol LIBaseCurlTaskDelegate;

/**
 Curl下载任务模型协议
 */
@protocol LIBaseCurlTaskPtotocol <NSObject>

@property(nonatomic, readonly) NSString *url;

@property(nonatomic, readonly) NSUInteger startPosition;

@property(nonatomic, weak) id <LIBaseCurlTaskDelegate> delegate;

@property(nonatomic, readonly) BOOL isPaused;

+(instancetype)taskWithUrl:(NSString *)url;
+(instancetype)taskWithUrl:(NSString *)url rangeFrom:(NSUInteger)startPosition;

-(instancetype)initWithUrl:(NSString *)url;
-(instancetype)initWithUrl:(NSString *)url rangeFrom:(NSUInteger)startPosition;

-(void)start;

-(void)resum;

-(void)pause;

-(void)cancel;

@end

/**
 Curl下载任务回调协议
 */
@protocol LIBaseCurlTaskDelegate <NSObject>
@required
-(void)downloadTask:(id <LIBaseCurlTaskPtotocol>)task recivedData:(char *)buffer size:(NSUInteger)size;

@optional
-(void)downloadTask:(id <LIBaseCurlTaskPtotocol>)task recivedResponse:(NSString *)response;

-(void)downloadTask:(id <LIBaseCurlTaskPtotocol>)task updateProgress:(NSUInteger)dlnow total:(NSUInteger)dltotal;

-(void)downloadTaskComleted:(id <LIBaseCurlTaskPtotocol>)task;

-(void)downloadTask:(id <LIBaseCurlTaskPtotocol>)task failed:(NSInteger)code;

-(void)downloadTaskDidResumed:(id <LIBaseCurlTaskPtotocol>)task;

-(void)downloadTaskDidPaused:(id <LIBaseCurlTaskPtotocol>)task;
@end
