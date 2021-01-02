
#import <UIKit/UIKit.h>
#include "screen_dimensions.h"

void get_dimensions(uint32_t* x0, uint32_t* y0, uint32_t* w, uint32_t* h)
{
    UIScreen* screen = [UIScreen mainScreen];
    
    UIView* view = [[UIApplication sharedApplication] keyWindow];
    CGRect newFrame = view.frame;

    newFrame.size.width = 200;
    newFrame.size.height = 200;
    [view setFrame:newFrame];
    
    [view layoutIfNeeded];
    
    
    *w = (uint32_t)(screen.bounds.size.width * screen.scale);
    *h = (uint32_t)(screen.bounds.size.height * screen.scale);
    *x0 = 0;
    *y0 = 0;
    
    // Deal with "notches" in iPhone X etc.
    if (@available(iOS 11.0, *)) {
        NSLog(@"Available!");
        struct UIEdgeInsets insets = [UIApplication sharedApplication].keyWindow.safeAreaInsets;
   
        
                                       NSLog(@"%f %f %f %f", insets.top, insets.bottom, insets.left, insets.right);
        *y0 = (uint32_t)(insets.top * screen.scale);
        *h = *h - *y0 - (uint32_t)(insets.bottom * screen.scale);
        
        *x0 = (uint32_t)(insets.left * screen.scale);
        *w = *w - *x0 - (uint32_t)(insets.right * screen.scale);
    }
    else
    {
        *x0 = 0;
        *y0 = 0;
        
    }
    
    // Check and adjust for orientation
    if (*h > *w)
    {
        uint32_t tmp = *h;
        *h = *w;
        *w = tmp;
        
        tmp = *x0;
        *x0 = *y0;
        *y0 = *x0;
    }
    
    
    NSLog(@"BOUNDS: x0:%u, y0:%u, w:%u, h:%u", *x0, *y0, *w, *h);
}
