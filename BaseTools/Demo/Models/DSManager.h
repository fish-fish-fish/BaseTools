//
//  DSManager.h
//  BaseTools
//
//  Created by liguanjun on 2017/4/10.
//  Copyright © 2017年 liguanjun. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "DSDownloadModel.h"

@interface DSManager : NSObject
@property (class, readonly) DSManager *shared;
@property (nonatomic, strong, readonly) NSArray<DSDownloadModel *> *cellModels;
@property (nonatomic, strong, readonly) NSString *rootDir;
- (instancetype)init NS_UNAVAILABLE;

/**
 载入模型
 */
- (void)loadModelsIfNeeded;
@end
