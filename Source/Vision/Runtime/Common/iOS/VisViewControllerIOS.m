/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */


#import <QuartzCore/QuartzCore.h>

#import <Vision/Runtime/Common/iOS/VisViewControllerIOS.h>
#import <Vision/Runtime/Common/iOS/VisEAGLViewIOS.h>
#import <Vision/Runtime/Base/Graphics/Video/IOSBridge.h>

VisionAppSettings g_VisionAppSettings = { NULL, NULL, NULL, 60, VisOrientationAllLandscape };

@interface VisViewController ()
@property (nonatomic, assign) CADisplayLink *displayLink;
@end

@implementation VisViewController

@synthesize animating, displayLink;

- (void)awakeFromNib
{
  UIAccelerometer* accelerometer = [UIAccelerometer sharedAccelerometer];
  accelerometer.delegate = self;
  accelerometer.updateInterval = 0.1;
  
  int orientations = 0;
  orientations |= (g_VisionAppSettings.iSupportedOrientations & VisOrientationPortrait) ? (1 << UIInterfaceOrientationPortrait) : 0;
  orientations |= (g_VisionAppSettings.iSupportedOrientations & VisOrientationPortraitUpsideDown) ? (1 << UIInterfaceOrientationPortraitUpsideDown) : 0;
  orientations |= (g_VisionAppSettings.iSupportedOrientations & VisOrientationLandscapeRight) ? (1 << UIInterfaceOrientationLandscapeRight) : 0;
  orientations |= (g_VisionAppSettings.iSupportedOrientations & VisOrientationLandscapeLeft) ? (1 << UIInterfaceOrientationLandscapeLeft) : 0;  
  supportedOrientations = orientations;  
  
  IOSSetCALayer((CAEAGLLayer*)self.view.layer);
  
  animating = FALSE;
  animationFrameInterval = 60 / g_VisionAppSettings.iSyncFrameRate;
  self.displayLink = nil;
}

- (void)dealloc
{
  [super dealloc];
}

- (void)viewWillAppear:(BOOL)animated
{
  [self startAnimation];
  
  [super viewWillAppear:animated];
}

- (void)viewWillDisappear:(BOOL)animated
{
  [self stopAnimation];
  
  [super viewWillDisappear:animated];
}

- (void)viewDidLoad
{
  [super viewDidLoad];
  if (![self shouldAutorotateToInterfaceOrientation:UIInterfaceOrientationPortrait] && 
      ![self shouldAutorotateToInterfaceOrientation:UIInterfaceOrientationPortraitUpsideDown])
  {
    IOSSetLandscapeMode(true);
    self.view.bounds = CGRectMake(0.0f, 0.0f, self.view.bounds.size.height, self.view.bounds.size.width);
  }
}

- (void)viewDidUnload
{
	[super viewDidUnload];
	
}

- (NSInteger)animationFrameInterval
{
  return animationFrameInterval;
}

- (void)setAnimationFrameInterval:(NSInteger)frameInterval
{
    /*
	 Frame interval defines how many display frames must pass between each time the display link fires.
	 The display link will only fire 30 times a second when the frame internal is two on a display that refreshes 60 times a second. The default frame interval setting of one will fire 60 times a second when the display refreshes at 60 times a second. A frame interval setting of less than one results in undefined behavior.
	 */
  if (frameInterval >= 1)
  {
    animationFrameInterval = frameInterval;
    
    if (animating)
    {
      [self stopAnimation];
      [self startAnimation];
    }
  }
}

- (void)startAnimation
{
  if (!animating)
  {
    CADisplayLink *aDisplayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(runApp)];
    [aDisplayLink setFrameInterval:animationFrameInterval];
    [aDisplayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
    self.displayLink = aDisplayLink;
    
    animating = TRUE;
  }
}

- (void)stopAnimation
{
  if (animating)
  {
    [self.displayLink invalidate];
    self.displayLink = nil;
    animating = FALSE;
  }
}

- (void)runApp
{
  [self.view setHidden:FALSE];
  if (!VisionRunFunction())
  {
    [self stopAnimation];
    _exit(0);
  }
}

- (void)didReceiveMemoryWarning
{
  [super didReceiveMemoryWarning];
  
  VisionMemoryWarningFunction();
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
  if (supportedOrientations & (1 << interfaceOrientation))
    return YES;
  
  return NO;
}

- (void)didRotateFromInterfaceOrientation:(UIInterfaceOrientation)fromInterfaceOrientation
{
  if (UIInterfaceOrientationIsLandscape(fromInterfaceOrientation) == 
      UIInterfaceOrientationIsLandscape(self.interfaceOrientation))
    return;
  
  IOSSetLandscapeMode(UIInterfaceOrientationIsLandscape(self.interfaceOrientation));
  VisionResizeFunction((int)self.view.bounds.size.width, (int)self.view.bounds.size.width);
}

- (void)accelerometer:(UIAccelerometer *)accelerometer
        didAccelerate:(UIAcceleration *)acceleration
{
  switch ( self.interfaceOrientation )
  {
  case UIInterfaceOrientationLandscapeRight:
    IOSMotionSetAcceleration( -acceleration.y, acceleration.x, acceleration.z);
    break;
  case UIInterfaceOrientationLandscapeLeft:
    IOSMotionSetAcceleration( acceleration.y, -acceleration.x, acceleration.z);
    break;
  case UIInterfaceOrientationPortrait:
    IOSMotionSetAcceleration( acceleration.x, acceleration.y, acceleration.z );
    break;
  case UIInterfaceOrientationPortraitUpsideDown:
    IOSMotionSetAcceleration( -acceleration.x, -acceleration.y, acceleration.z );
    break;
  }
}

- (void)willPresentAlertView:(UIAlertView *)alertView
{
  [self stopAnimation];
}

- (void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex
{
  [self startAnimation];
}

@end

/*
 * Havok SDK - Base file, BUILD(#20130624)
 * 
 * Confidential Information of Havok.  (C) Copyright 1999-2013
 * Telekinesys Research Limited t/a Havok. All Rights Reserved. The Havok
 * Logo, and the Havok buzzsaw logo are trademarks of Havok.  Title, ownership
 * rights, and intellectual property rights in the Havok software remain in
 * Havok and/or its suppliers.
 * 
 * Use of this software for evaluation purposes is subject to and indicates
 * acceptance of the End User licence Agreement for this product. A copy of
 * the license is included with this software and is also available from salesteam@havok.com.
 * 
 */
