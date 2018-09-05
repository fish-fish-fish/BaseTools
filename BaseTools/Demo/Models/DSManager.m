//
//  DSManager.m
//  BaseTools
//
//  Created by liguanjun on 2017/4/10.
//  Copyright © 2017年 liguanjun. All rights reserved.
//

#import "DSManager.h"

@implementation DSManager
+ (instancetype)shared {
    static dispatch_once_t onceToken;
    static DSManager *manager = nil;
    dispatch_once(&onceToken, ^{
        manager = [[DSManager alloc] initWithPrivate];
    });
    return manager;
}

- (instancetype)initWithPrivate {
    if (self = [super init]) {
        NSString *rootDir = [NSSearchPathForDirectoriesInDomains(NSLibraryDirectory, NSUserDomainMask, YES).firstObject stringByAppendingPathComponent:@"Download_Root"];
        if (![[NSFileManager defaultManager] fileExistsAtPath:rootDir]) {
            [[NSFileManager defaultManager] createDirectoryAtPath:rootDir withIntermediateDirectories:YES attributes:nil error:nil];
        }
        _rootDir = rootDir;
    }
    return self;
}

- (void)loadModelsIfNeeded {
    if (_cellModels != nil) {
        return;
    }
    NSString* path = [[NSBundle mainBundle] pathForResource:@"UrlItems" ofType:@"plist"];
    NSArray *urlItems = [NSArray arrayWithContentsOfFile:path];
    NSMutableArray<DSDownloadModel *> *cellItems = [[NSMutableArray alloc] init];
    for (NSDictionary *dict in urlItems) {
        DSDownloadModel *item = [[DSDownloadModel alloc] initWithDict:dict];
        [cellItems addObject:item];
    }
    _cellModels = cellItems;
}

@end
