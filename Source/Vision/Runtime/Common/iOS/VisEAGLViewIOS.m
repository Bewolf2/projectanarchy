/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */


#import <QuartzCore/QuartzCore.h>

#import <Vision/Runtime/Common/iOS/VisEAGLViewIOS.h>
#import <Vision/Runtime/Base/Graphics/Video/IOSBridge.h>

@implementation VisEAGLView

// You must implement this method
+ (Class)layerClass
{
  return [CAEAGLLayer class];
}

//The EAGL view is stored in the nib file. When it's unarchived it's sent -initWithCoder:.
- (id)initWithCoder:(NSCoder*)coder
{
  self = [super initWithCoder:coder];
  if (self)
  {
    CAEAGLLayer* eaglLayer = (CAEAGLLayer*)self.layer;
    
    const bool bAllowRetinaDisplay = true;
    if ( bAllowRetinaDisplay )
    {
      if([[UIScreen mainScreen] respondsToSelector: NSSelectorFromString(@"scale")])
      {
        if([self respondsToSelector: NSSelectorFromString(@"contentScaleFactor")])
        {
          self.contentScaleFactor = [[UIScreen mainScreen] scale];
        }
      }
    }
    
    eaglLayer.opaque = TRUE;
    eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
                                     [NSNumber numberWithBool:FALSE], kEAGLDrawablePropertyRetainedBacking,
                                     kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat,
                                     nil];
    
    [self setMultipleTouchEnabled:YES];
  }
  
  IOSSetEAGLView(self);
  
  return self;
}

- (void)dealloc
{
  [super dealloc];
}

/* Multitouch Implementation */
-(void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
  for(UITouch *touch in touches)
  {
    CGPoint ptAbsolutePosition = [touch locationInView:self];
    
    IOSTouchAddNewTouch((void*)touch, (int)ptAbsolutePosition.x, (int)ptAbsolutePosition.y);
  }  
}

-(void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
  for(UITouch *touch in touches)
  {
    CGPoint ptAbsolutePosition = [touch locationInView:self];
    CGPoint ptAbsolutePreviousPosition = [touch previousLocationInView:self];
    
    IOSTouchUpdateTouch((void*)touch, (int)ptAbsolutePosition.x, (int)ptAbsolutePosition.y, (int)ptAbsolutePreviousPosition.x, (int)ptAbsolutePreviousPosition.y);   
  }
}

-(void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
  for(UITouch *touch in touches)
  {
    IOSTouchRemoveTouch((void*)touch);
  }
}

-(void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event
{
  for(UITouch *touch in touches)
  {
    IOSTouchRemoveTouch((void*)touch);
  }  
}

-(void)insertText:(NSString *)text
{
  IOSVirtualKeyboardInsertText([text UTF8String]);
}

- (void)deleteBackward
{
  IOSVirtualKeyboardDeleteKey();
}

-(BOOL)hasText
{
  return IOSVirtualKeyboardHasText() ? YES : NO;
}

-(BOOL)canBecomeFirstResponder
{
  return YES;
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
