//
//  LIThreadPool.h
//  BaseTools
//
//  Created by liguanjun on 2018/9/6.
//  Copyright © 2018年 liguanjun. All rights reserved.
//

#import <Foundation/Foundation.h>

typedef void(^OCBlock)(void);
@interface LIThreadPool : NSObject
@property(class, nonatomic, readonly) LIThreadPool *shared;

- (void)startWithThreads:(NSUInteger)count;

- (void)pushTask:(OCBlock)task;

- (instancetype)init NS_UNAVAILABLE;
@end
