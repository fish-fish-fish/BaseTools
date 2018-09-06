//
//  AutoRelease.m
//  ios-learn
//
//  Created by liguanjun on 2018/5/17.
//  Copyright © 2018年 liguanjun. All rights reserved.
//

#import <Foundation/Foundation.h>

// 利用栈上对象自动释放特性，正确释放OC对象
class AutoRelease {
public:
    AutoRelease(NSObject *obj) {
        m_obj = [obj retain];
    }
    ~AutoRelease() {
        [m_obj autorelease];
    }
private:
    NSObject *m_obj;
};
