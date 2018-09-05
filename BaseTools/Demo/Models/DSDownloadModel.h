//
//  DSDownloadModel.h
//  BaseTools
//
//  Created by liguanjun on 2017/4/12.
//  Copyright © 2017年 liguanjun. All rights reserved.
//

#import "DSCellModel.h"
#import "LIBaseCurlTaskPtotocol.h"

typedef enum DSDownloadModelStatus {
    DSDownloadModelStatus_none = 0,
    DSDownloadModelStatus_queueing,
    DSDownloadModelStatus_downloading,
    DSDownloadModelStatus_paused,
    DSDownloadModelStatus_completed,
    DSDownloadModelStatus_failed,
} DSDownloadModelStatus;

@protocol DSDownloadModelDelegate;
@class DSTableViewCell;
@interface DSDownloadModel : DSCellModel
@property(nonatomic, weak) id <DSDownloadModelDelegate> delegate;
@property(nonatomic, assign, readonly) NSInteger totalSize;
@property(nonatomic, assign, readonly) float progress;
@property(nonatomic, strong, readonly) NSString *progressInfo;
@property(nonatomic, strong, readonly) NSString *absPath;
@property(nonatomic, strong, readonly) NSString *tempAbsPath;
@property(nonatomic, assign, readonly) DSDownloadModelStatus status;
@property(nonatomic, weak) DSTableViewCell *cell;

- (void)start;
- (void)resum;
- (void)pause;
- (void)cancel;
- (void)remove;
@end

@protocol DSDownloadModelDelegate <NSObject>

@required
- (void)downloadModelDidUpdated:(DSDownloadModel *)model;

@end
