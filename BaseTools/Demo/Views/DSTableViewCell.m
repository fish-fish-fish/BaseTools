//
//  DSTableViewCell.m
//  BaseTools
//
//  Created by liguanjun on 2017/4/10.
//  Copyright © 2017年 liguanjun. All rights reserved.
//

#import "DSTableViewCell.h"

const CGFloat padding = 6.0;
const CGFloat Height = 36.0;

@interface DSTableViewCell ()
@property(nonatomic, weak) UILabel *titleLable;
@property(nonatomic, weak) UILabel *sizeLab;
@property(nonatomic, weak) UILabel *loadRateLabel;
@property(nonatomic, weak) UIButton *handleButton;
@property(nonatomic, weak) UIButton *removeButton;
@property(nonatomic, weak) UIProgressView *progressView;
@end

@implementation DSTableViewCell

- (instancetype)initWithStyle:(UITableViewCellStyle)style reuseIdentifier:(NSString *)reuseIdentifier {
    if (self = [super initWithStyle:style reuseIdentifier:reuseIdentifier]) {
        UILabel *title = [[UILabel alloc] init];
        [self.contentView addSubview:title];
        _titleLable = title;
        
        UILabel *sizeLab = [[UILabel alloc] init];
        [self.contentView addSubview:sizeLab];
        _sizeLab = sizeLab;
        _sizeLab.textAlignment = NSTextAlignmentLeft;
        
        UILabel *rate = [[UILabel alloc] init];
        [self.contentView addSubview:rate];
        _loadRateLabel = rate;
        _loadRateLabel.textAlignment = NSTextAlignmentRight;
        
        UIColor *btnColor = [UIColor colorWithHue:210.0f / 360.0f saturation:0.92f brightness:1.0f alpha:1.0f];
        
        UIButton *handle = [UIButton buttonWithType:UIButtonTypeSystem];
        [handle setTitle:@"开始下载" forState:UIControlStateNormal];
        [handle setTitleColor:[UIColor whiteColor] forState:UIControlStateNormal];
        [handle setTitle:@"下载完成" forState:UIControlStateDisabled];
        [handle setTitleColor:[UIColor colorWithRed:0.8 green:0.8 blue:0.8 alpha:1.0] forState:UIControlStateDisabled];
        [handle.titleLabel setFont:[UIFont systemFontOfSize:14.0]];
        handle.layer.cornerRadius = 3.0;
        [handle setBackgroundColor:btnColor];
        [self.contentView addSubview:handle];
        _handleButton = handle;
        [_handleButton addTarget:self action:@selector(btnClicked:) forControlEvents:UIControlEventTouchUpInside];
        
        UIButton *remove = [UIButton buttonWithType:UIButtonTypeSystem];
        [remove setTitle:@"删除数据" forState:UIControlStateNormal];
        [remove setTitleColor:[UIColor whiteColor] forState:UIControlStateNormal];
        [remove setTitleColor:[UIColor grayColor] forState:UIControlStateDisabled];
        [remove.titleLabel setFont:[UIFont systemFontOfSize:14.0]];
        remove.layer.cornerRadius = 3.0;
        [remove setBackgroundColor:btnColor];
        [self.contentView addSubview:remove];
        _removeButton = remove;
        [_removeButton addTarget:self action:@selector(btnClicked:) forControlEvents:UIControlEventTouchUpInside];
        
        UIProgressView *progress = [[UIProgressView alloc] initWithProgressViewStyle:UIProgressViewStyleDefault];
        progress.backgroundColor = [UIColor clearColor];
        progress.trackTintColor = [UIColor lightGrayColor];
        progress.progressTintColor = [UIColor blueColor];
        [self.contentView addSubview:progress];
        _progressView = progress;
        
    }
    return self;
}

+ (instancetype)cellWithTableView:(UITableView *)tableView {
    static NSString *reuseId = @"DSTableViewCell";
    DSTableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:reuseId];
    if (cell == nil) {
        cell = [[DSTableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:reuseId];
    }
    return cell;
}

- (void)setModel:(DSDownloadModel *)model {
    if (model == nil) return;
    _model = model;
    
    self.titleLable.text = model.name;
    if (model.totalSize > 0) {
        self.sizeLab.text = [NSString stringWithFormat:@"大小：%@", [NSByteCountFormatter stringFromByteCount:model.totalSize countStyle:NSByteCountFormatterCountStyleMemory]];
    } else {
        self.sizeLab.text = [NSString stringWithFormat:@"大小：未知"];
    }
    self.loadRateLabel.text = [NSString stringWithFormat:@"已下载:%ld%%", (NSInteger)(model.progress * 100)];
    self.progressView.progress = model.progress;
    switch (model.status) {
        case DSDownloadModelStatus_none:
        {
            [self.handleButton setTitle:@"开始下载" forState:UIControlStateNormal];
            self.handleButton.enabled = YES;
            self.removeButton.hidden = YES;
        }
            break;
        case DSDownloadModelStatus_queueing:
        {
            [self.handleButton setTitle:@"排队中" forState:UIControlStateDisabled];
            self.handleButton.enabled = NO;
            self.removeButton.hidden = YES;
        }
            break;
        case DSDownloadModelStatus_paused:
        {
            [self.handleButton setTitle:@"继续下载" forState:UIControlStateNormal];
            self.handleButton.enabled = YES;
            [self.removeButton setTitle:@"取消下载" forState:UIControlStateNormal];
            self.removeButton.hidden = NO;
        }
            break;
        case DSDownloadModelStatus_downloading:
        {
            [self.handleButton setTitle:@"暂停下载" forState:UIControlStateNormal];
            self.handleButton.enabled = YES;
            [self.removeButton setTitle:@"取消下载" forState:UIControlStateNormal];
            self.removeButton.hidden = NO;
        }
            break;
        case DSDownloadModelStatus_completed:
        {
            [self.handleButton setTitle:@"下载完成" forState:UIControlStateDisabled];
            self.handleButton.enabled = NO;
            [self.removeButton setTitle:@"删除数据" forState:UIControlStateNormal];
            self.removeButton.hidden = NO;
        }
            break;
        case DSDownloadModelStatus_failed:
        {
            [self.handleButton setTitle:@"断点续传" forState:UIControlStateNormal];
            self.handleButton.enabled = YES;
            [self.removeButton setTitle:@"删除数据" forState:UIControlStateNormal];
            self.removeButton.hidden = NO;
        }
            break;
            
        default:
            break;
    }
}

+ (CGFloat)cellHeight {
    return padding + Height + Height + padding + 5.0 +padding;
}

- (void)layoutSubviews {
    [super layoutSubviews];
    
    self.titleLable.frame = CGRectMake(padding, padding, CGRectGetWidth(self.contentView.frame)*0.5, 40);
    self.sizeLab.frame = CGRectMake(padding, CGRectGetMaxY(self.titleLable.frame), CGRectGetWidth(self.titleLable.frame), Height);
    
    CGFloat btnW = 60;
    _handleButton.frame = CGRectMake(CGRectGetMaxX(self.contentView.frame) - btnW - padding, padding, btnW, Height);
    
    _removeButton.frame = CGRectMake(CGRectGetMinX(_handleButton.frame) - padding - btnW, padding, btnW, Height);
    _loadRateLabel.frame = CGRectMake(CGRectGetMaxX(self.contentView.frame) - 130, CGRectGetMaxY(_handleButton.frame) + padding, 120, Height);
    
    CGFloat progH = 5.0;
    _progressView.frame = CGRectMake(padding, CGRectGetMaxY(self.contentView.frame) -padding - progH, CGRectGetWidth(self.contentView.frame)-padding*2, progH);
}

- (void)awakeFromNib {
    [super awakeFromNib];
    // Initialization code
}

- (void)setSelected:(BOOL)selected animated:(BOOL)animated {
    [super setSelected:selected animated:animated];

    // Configure the view for the selected state
}

#pragma mark - button clicked
- (void)btnClicked:(UIButton *)sender {
        if ([sender.titleLabel.text isEqualToString:@"开始下载"] || [sender.titleLabel.text isEqualToString:@"断点续传"]) {
            [self.model start];
        }else if([sender.titleLabel.text isEqualToString:@"继续下载"]) {
            [self.model resum];
        }else if([sender.titleLabel.text isEqualToString:@"暂停下载"]) {
            [self.model pause];
        }else if ([sender.titleLabel.text isEqualToString:@"取消下载"]) {
            [self.model cancel];
        }else if ([sender.titleLabel.text isEqualToString:@"删除数据"]) {
            [self.model remove];
        }
}

@end
