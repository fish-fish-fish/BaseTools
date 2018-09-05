//
//  DSCellModel.h
//  BaseTools
//
//  Created by liguanjun on 2017/4/10.
//  Copyright © 2017年 liguanjun. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface DSCellModel : NSObject
@property(nonatomic, strong, readonly) NSString *name;
@property(nonatomic, strong, readonly) NSString *url;

- (instancetype)initWithDict:(NSDictionary *)dict;
@end
