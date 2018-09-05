//
//  DSTableViewCell.h
//  BaseTools
//
//  Created by liguanjun on 2017/4/10.
//  Copyright © 2017年 liguanjun. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "DSDownloadModel.h"

@interface DSTableViewCell : UITableViewCell

@property(nonatomic, strong) DSDownloadModel *model;

+ (instancetype)cellWithTableView:(UITableView *)tableView;

+ (CGFloat)cellHeight;
@end
