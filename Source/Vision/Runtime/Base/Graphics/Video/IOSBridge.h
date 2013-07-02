/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */


#ifndef __IOS_BRIDGE_INCLUDED__
#define __IOS_BRIDGE_INCLUDED__

#ifdef __cplusplus
#define FUNCTION_DEFINITION extern "C"
#else
#define FUNCTION_DEFINITION 
#endif

extern unsigned int gIOSFrameBuffer;
extern unsigned int gIOSColorRenderBuffer;
extern unsigned int gIOSDepthRenderBuffer;
extern unsigned int gIOSStencilRenderBuffer;


FUNCTION_DEFINITION bool IOSCreateGLES2Context();

FUNCTION_DEFINITION void IOSDestroyGLES2Context();

FUNCTION_DEFINITION bool IOSHasContext();

FUNCTION_DEFINITION bool IOSCreateGLES2FrameBuffer(unsigned int uiDepthbufferBits, int* puiFrameBufferWidth, int* puiFrameBufferHeight, bool* pbLandscapeMode);

FUNCTION_DEFINITION void IOSReleaseGLES2FrameBuffer();

FUNCTION_DEFINITION bool IOSSetCurrentContext();

FUNCTION_DEFINITION bool IOSPresent();



FUNCTION_DEFINITION void IOSMotionSetAcceleration(float fAccelerationX, float fAccelerationY, float fAccelerationZ);


FUNCTION_DEFINITION void IOSTouchAddNewTouch(void* pTouchObject, int iXAbsolute, int iYAbsolute);

FUNCTION_DEFINITION void IOSTouchRemoveTouch(void* pTouchObject);

FUNCTION_DEFINITION void IOSTouchUpdateTouch(void* pTouchObject, int iXAbsolute, int iYAbsolute, int iXPrevLocAbsolute, int iYPrevLocAbsolute);


FUNCTION_DEFINITION int IOSGetProcessorCount();  


FUNCTION_DEFINITION void IOSShowVirtualKeyboard();

FUNCTION_DEFINITION void IOSHideVirtualKeyboard();

FUNCTION_DEFINITION void IOSVirtualKeyboardInsertText(const char* pUTF8Text);

FUNCTION_DEFINITION void IOSVirtualKeyboardDeleteKey();

FUNCTION_DEFINITION bool IOSVirtualKeyboardHasText();

FUNCTION_DEFINITION void IOSShowAlert(const char* pTitle, const char* pMessage);

FUNCTION_DEFINITION float IOSGetContentScaleFactorFloat();

FUNCTION_DEFINITION int IOSGetContentScaleFactorInt();

FUNCTION_DEFINITION float IOSGetDisplayDensity();

#ifndef __cplusplus
void IOSSetCALayer(CAEAGLLayer* pLayer);

void IOSSetEAGLView(UIView* pEAGLView);

void IOSSetLandscapeMode(bool bEnabled);

void IOSSetAlertDelegate(id<UIAlertViewDelegate> delegate);
#endif

#endif

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
