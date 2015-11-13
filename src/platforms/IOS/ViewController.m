//
//  ViewController.m
//  PlutoBoy
//
//  Created by Ross Meikleham on 24/10/2015.
//  Copyright © 2015 Ross Meikleham. All rights reserved.
//

#import "ViewController.h"

#import "../../core/emu.h"

@interface ViewController ()

@property (nonatomic, strong) NSString *filename;
@property (nonatomic, strong) NSString *fileContents;
@property (weak, nonatomic) IBOutlet UITextView *binTextView;
@property (weak, nonatomic) IBOutlet UITextView *binTextView2;

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view, typically from a nib.
    
    self.filename = [[NSBundle mainBundle] pathForResource:@"rom" ofType:@"gb"];
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (IBAction)dumpBytesPressed:(id)sender {
    NSLog(@"Button Pressed\n");
    NSData *data = [NSData dataWithContentsOfFile:self.filename];
    if ([data length] > 0) {
        const uint8_t *bytes = (const uint8_t *)[data bytes];
        uint8_t byte = bytes[0];
        NSLog(@"%d\n", byte);
        
        self.binTextView.text = @"test";
        for (int i = 0; i < 1000; i++) {
            self.binTextView.text =[self.binTextView.text stringByAppendingString:@(bytes[i]).stringValue];
            
        }
        
        if (init([self.filename cStringUsingEncoding:[NSString defaultCStringEncoding]], 0, 0, NO_CONNECT))  {
            NSLog(@"Loaded successfully");
        } else {
            NSLog(@"Failed to load file");
        }
    
    } else {
        NSLog(@"Empty File\n");
    }
  
}

@end
