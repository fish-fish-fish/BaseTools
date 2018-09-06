//
//  DSViewController.m
//  BaseTools
//
//  Created by liguanjun on 17/3/17.
//  Copyright © 2017年 liguanjun. All rights reserved.
//

#import "DSViewController.h"
#import "DSTableViewCell.h"
#import "DSManager.h"

@interface DSViewController ()<UITableViewDataSource, UITableViewDelegate, DSDownloadModelDelegate>
@property(nonatomic, strong) UITableView *tableView;

@end

@implementation DSViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    self.title = @"下载测试";
    [[DSManager shared] loadModelsIfNeeded];
    
    self.tableView = [[UITableView alloc] initWithFrame:self.view.frame style:UITableViewStyleGrouped];
    self.tableView.dataSource = self;
    self.tableView.delegate   = self;
    self.tableView.rowHeight = [DSTableViewCell cellHeight];
    [self.view addSubview:self.tableView];
    
    // test OC风格的 Block task 
    [LIThreadPool.shared pushTask:^{
        int index = 10;
        while (index-- > 0) {
            sleep(1);
            NSLog(@"%s, index:%d", __func__, index);
        }
    }];
}

- (void)viewWillLayoutSubviews {
    self.tableView.frame = self.view.frame;
    [self.tableView setNeedsLayout];
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

#pragma mark - UITableViewDataSource
- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
    return [DSManager shared].cellModels.count;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    DSTableViewCell *cell = [DSTableViewCell cellWithTableView:tableView];
    
    DSDownloadModel *model = [DSManager shared].cellModels[indexPath.row];
    
    cell.model = model;
    model.cell = cell;
    model.delegate = self;

    return cell;
}

- (void)tableView:(UITableView *)tableView didEndDisplayingCell:(UITableViewCell *)cell forRowAtIndexPath:(NSIndexPath *)indexPath {
    if ([cell isKindOfClass:[DSTableViewCell class]]) {
        ((DSTableViewCell *)cell).model.cell = nil;
        ((DSTableViewCell *)cell).model = nil;
    }
}

#pragma mark - UITableViewDelegate
- (NSIndexPath *)tableView:(UITableView *)tableView willSelectRowAtIndexPath:(NSIndexPath *)indexPath {
    return nil;
}

#pragma mark - DSDownloadModelDelegate
- (void)downloadModelDidUpdated:(DSDownloadModel *)model {
    dispatch_async(dispatch_get_main_queue(), ^{
        model.cell.model = model;
    });
}

@end
