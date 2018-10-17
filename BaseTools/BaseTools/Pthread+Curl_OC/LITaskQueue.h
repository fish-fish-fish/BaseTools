//
//  LITaskQueue.h
//  BaseTools
//
//  Created by 李冠军 on 2018/10/17.
//  Copyright © 2018年 liguanjun. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

typedef void(^OCBlock)(void);
@interface LITaskQueue : NSObject
@property(class, nonatomic, strong, readonly) LITaskQueue *globalQueue;
- (instancetype)init;
- (instancetype)initWithMaxConcurrentCount:(NSInteger)maxConcurrentCount;

- (void)pushTask:(OCBlock)task;
- (void)addSubQueue:(LITaskQueue *)subQueue;
@end

NS_ASSUME_NONNULL_END
