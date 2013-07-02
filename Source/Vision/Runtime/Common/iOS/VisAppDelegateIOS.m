/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */


#import <Vision/Runtime/Common/iOS/VisAppDelegateIOS.h>
#import <Vision/Runtime/Common/iOS/VisViewControllerIOS.h>

#import <Vision/Runtime/Base/Graphics/Video/IOSBridge.h>

#include <unistd.h>

@implementation VisAppDelegate

@synthesize window;
@synthesize viewController;

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
  // Get documents directory
  NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
  NSString *documentsDirectory = [paths objectAtIndex:0];
  NSString *appPath = [[NSBundle mainBundle] bundlePath];
  
  NSArray *cachesPaths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
  NSString *cachesDirectory = [cachesPaths objectAtIndex:0];
  
  g_VisionAppSettings.szAppDirectory = [appPath fileSystemRepresentation];
  g_VisionAppSettings.szDocumentsDirectory = [documentsDirectory fileSystemRepresentation];
  g_VisionAppSettings.szCachesDirectory = [cachesDirectory fileSystemRepresentation];
  
  if (!VisionInitFunction())
    return NO;
  
  self.viewController.animationFrameInterval = 60 / g_VisionAppSettings.iSyncFrameRate;
  [self.window addSubview:self.viewController.view];

  // When you compile with iOS SDK < 6.0 you may need to uncomment the following line
  self.window.rootViewController = self.viewController;
  
  g_VisionAppSettings.szAppDirectory = NULL;
  g_VisionAppSettings.szDocumentsDirectory = NULL;
  g_VisionAppSettings.szCachesDirectory = NULL;
  
  return YES;
}

- (void)applicationWillResignActive:(UIApplication *)application
{
  [self.viewController stopAnimation];
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
  [self.viewController startAnimation];
}

- (void)applicationWillTerminate:(UIApplication *)application
{
  [self.viewController stopAnimation];
  
  VisionDeInitFunction();
  _exit(0);
}

- (void)applicationDidEnterBackground:(UIApplication *)application
{
  [self.viewController stopAnimation];
  
  VisionEnterBackgroundFunction();
}

- (void)applicationWillEnterForeground:(UIApplication *)application
{
  VisionEnterForegroundFunction();
  
  [self.viewController startAnimation];
}

- (void)applicationDidReceiveMemoryWarning:(UIApplication *)application
{
  VisionMemoryWarningFunction();
}

- (void)dealloc
{
  [viewController release];
  [window release];
  
  [super dealloc];
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
