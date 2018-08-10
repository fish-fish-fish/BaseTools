//
//  AutoRelease.m
//  ios-learn
//
//  Created by zsy on 2018/5/17.
//  Copyright © 2018年 liguanjun. All rights reserved.
//

#import <Foundation/Foundation.h>

class AutoRelease {
public:
    AutoRelease(NSObject *obj) {
        m_obj = obj;
    }
    ~AutoRelease() {
        m_obj = nil;
    }
private:
    __strong NSObject *m_obj;
};
