/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VBase.hpp

#ifndef DC_VBASE_HPP
#define DC_VBASE_HPP


//#pragma warning(disable:4819)   // Since Visual Studio complains a lot about non Unicode characters on Asian machines



//Main include file for using vBase. This gives you most of the vBase headers you need
//-Include it as the first file from your project's precompiled header file
// so it can override the windows.h deprecations
//-Define VBASE_LIB in your project to statically link instead of using the DLL

#include <Vision/Runtime/Base/Math/hkvDeprecationMacros.h>

//Add all the external headers that vBase needs as input in here
#include <Vision/Runtime/Base/VBaseIncludes.hpp>

#include <Vision/Runtime/Base/Platform/hkvPlatformDefines.h>

#if defined(__GCC__) || defined(__SNC__) || defined(__GNUC__) || (_MANAGED == 1) || (_M_CEE == 1)
  #define VISION_CDECL
  #define VISION_FASTCALL
  #define VISION_STDCALL
#elif defined(__CWCC__)
  #define VISION_CDECL    __cdecl
  #define VISION_FASTCALL ///< CWCC doesn't seem to a have a specific fastcall calling convention - wouldn't be compatible to others anyway.
  #define VISION_STDCALL  __stdcall
#else
  #define VISION_CDECL    __cdecl
  #define VISION_FASTCALL __fastcall
  #define VISION_STDCALL  __stdcall
#endif


#if defined(__SNC__)
  #define VISION_FORCEINLINE __attribute__((always_inline))
#elif defined(__GCC__) || defined(__GNUC__)
  #define VISION_FORCEINLINE inline
#elif defined(__CWCC__)
  #define VISION_FORCEINLINE inline ///< Metrowerks CodeWarrior has a pragma always_inline, though.
#else
  #define VISION_FORCEINLINE __forceinline
#endif

#if ( ((defined(__GCC__) || defined(__GNUC__)) && !defined(_VISION_WIIU)) || defined(__SNC__) ) && !defined(_VISION_POSIX) /* No TLS on iOS devices available */

  //Creates an instance in the source
  #define VISION_THREADLOCAL_INST(TYPE,VAR,VAL)   __thread   TYPE VAR = VAL;
  //Declares the variable in the header
  #define VISION_THREADLOCAL_DECL(TYPE,VAR)       __thread   TYPE VAR;
  //Declares a TYPE array
  #define VISION_THREADLOCAL_DECL_CHARBUFF(SIZE,VAR)  __thread   char VAR[SIZE];
 
#elif defined(_VISION_POSIX)
  // uses library functions to access thread specific data. TODO: Use OSThread functions.
  #define VISION_THREADLOCAL_INST(TYPE,VAR,VAL) TYPE VAR = VAL;
  #define VISION_THREADLOCAL_DECL(TYPE,VAR)     TYPE VAR;
  #define VISION_THREADLOCAL_DECL_CHARBUFF(SIZE,VAR) char  VAR[SIZE];

#else

  #if (defined(VBASE_LIB) && !defined(_VISION_WIIU) && !defined(WIN32) ) || defined(_VISION_XENON) || defined(_VISION_PS3) || defined(_VISION_PSP2) || defined(_VISION_WINRT)
    //Using TLS:  Doesn't work for dynamically loaded DLLs

    //Creates an instance in the source
    #define VISION_THREADLOCAL_INST(TYPE,VAR,VAL)   __declspec(thread)  TYPE VAR = VAL;
    //Declares the variable in the header
    #define VISION_THREADLOCAL_DECL(TYPE,VAR)       __declspec(thread)  TYPE VAR;
    //Declares the array variable
    #define VISION_THREADLOCAL_DECL_CHARBUFF(SIZE,VAR) __declspec(thread) char VAR[SIZE];

  #else
    //Using TlsAlloc/TlsFree for DLL
    //Remember to call FreeThreadData for each thread variable in DllMain on thread detach
    
    #include <Vision/Runtime/Base/System/Threading/Thread/VThreadVariable.hpp>
    //Creates an instance in the source
    #define VISION_THREADLOCAL_INST(TYPE,VAR,VAL)   VThreadVariable<TYPE> VAR(VAL);
    //Declares the variable in the header
    #define VISION_THREADLOCAL_DECL(TYPE,VAR)       VThreadVariable<TYPE> VAR;
    //Declares the variable in the header
    #define VISION_THREADLOCAL_DECL_CHARBUFF(SIZE,VAR) VThreadBuffer<char,SIZE> VAR;    
    
  #endif
  
#endif

#if !defined(SET_RESOURCE_DEBUG_NAME)
  // No platform or build target defined this, so comment it out
  #define SET_RESOURCE_DEBUG_NAME(pRes, szName) 
#endif


#if defined(__GCC__) || defined(__SNC__) || defined(__GNUC__)
  #define VISION_ALIGN(X) __attribute__((aligned(X)))
#else
  // Managed C++ vs2008 code doesn't allow aligned unmanaged data in managed code
  #if defined(_MSC_VER) && (_MSC_VER >= 1500)
    #define VISION_ALIGN(X)
  #else
    #define VISION_ALIGN(X) __declspec (align(X))
  #endif
#endif

// VISION_ALIGN_NOWIN : Some alignments should only be defined on console versions
#ifdef WIN32
  #define VISION_ALIGN_NOWIN(X) 
#else
  #define VISION_ALIGN_NOWIN(X) VISION_ALIGN(X)
#endif


// Force CodeWarrior to use old friend lookup
#if defined(__CWCC__)
  #pragma old_friend_lookup on
#endif


//Force our time to be 64-bit to avoid VC2003 and VC2005 issues
typedef __time64_t vtime_t;


#if defined(_USE_32BIT_TIME_T)
  #error("You should not define _USE_32BIT_TIME_T anymore")
#endif

// Define intprt_t if not already defined.
#if !defined(_INTPTR_T_DEFINED) && !defined(__CWCC__) && !defined(__GCC__) && !defined(_INTPTR_T)
typedef int intptr_t;
#endif

// Defines for DLL linkage (used in the engine plugin for example)
#if !defined(VBASE_LIB) && !defined(VISION_LINK_STATIC)

  #if (defined(WIN32) || defined(_VISION_XENON)) && !defined(_VISION_WINRT) 
    #define V_DYNLINK_EXPORT __declspec(dllexport)
    #define V_DYNLINK_IMPORT __declspec(dllimport)
    #define V_DYNLINK_NONE __declspec()
  #else
    #error "Missing code for dynamic linkage!"
  #endif

  #define V_DYNLINK_ENABLED
  #undef V_STATICLINK_ENABLED

#elif defined(VBASE_LIB) || defined(LINK_WITH_VISION)

  #define V_STATICLINK_ENABLED
  #undef V_DYNLINK_ENABLED

#endif

// Memory copy helpers (specialized versions for platforms, maps to regular memcpy if no specialized version available)
#ifdef _VISION_XENON
  #define vMemCpyToUncached(dst, src, count) vMemCpyToUncachedXenon(dst, src, count) // src has to be in cachable memory, dst may be any type
  #define vMemCpyToCached(dst, src, count)   vMemCpyToCachedXenon(dst, src, count) // both dst and src have to be in cachable memory
#else
  #define vMemCpyToUncached(dst, src, count) memcpy(dst, src, count) 
  #define vMemCpyToCached(dst, src, count)   memcpy(dst, src, count)
#endif

// Memory allocation helpers (intended to be used exclusively throughout the system for raw memory and C-strings).
#define vMemAlloc(size)                   VBaseAlloc(size)
#define vMemAlignedAlloc(size, alignment) VBaseAlignedAlloc(size, alignment)
#define vMemFree(ptr)                     VBaseDealloc(ptr)
#define vMemAlignedFree(ptr)              VBaseAlignedDealloc(ptr)
#define vStrFree(the_string)              { vMemFree(the_string); the_string=NULL; }

#define V_SAFE_DELETE(x)                  { if (x) { delete    x;        x=NULL; } }
#define V_SAFE_DELETE_ARRAY(x)            { if (x) { delete [] x;        x=NULL; } }
#define V_SAFE_FREE(x)                    { if (x) { vMemFree(x);        x=NULL; } }
#define V_SAFE_FREE_ALIGNED(x)            { if (x) { vMemAlignedFree(x); x=NULL; } }
#define V_SAFE_STRFREE(x)                 { if (x) { vStrFree(x);        x=NULL; } }
#define V_SAFE_RELEASE(x)                 { if (x) { x->Release();       x=NULL; } }
#define V_SAFE_DISPOSE(x)                 { if (x) { x->Dispose();       x=NULL; } }
#define V_SAFE_DISPOSEOBJECT(x)           { if (x) { if (!x->IsDisposed()) x->DisposeObject(); x=NULL; } }
#define V_SAFE_REMOVE(x)                  { if (x) { x->Remove();        x=NULL; } }

#define V_VERIFY_MALLOC(p)                { if (!p) Vision::Error.FatalError("malloc failed (variable: %s)",#p); ANALYSIS_ASSUME(p) }

#define V_BIT(_iBit)                      ( 1U<<(_iBit) )
#define V_STRINGIZE_AUX(s)                #s
#define V_STRINGIZE(s)                    V_STRINGIZE_AUX(s)
#define V_CAT_AUX(a, b)                   a##b
#define V_CAT(a, b)                       V_CAT_AUX(a, b)

#ifndef V_PRECACHE
  #define V_PRECACHE(offset, ptr)
#endif

//macro to determine the size of a static array
namespace 
{
  template <typename T, size_t N>
  char (*ArraySizeHelper( T (&)[N] ))[N];
}
// Add zero to avoid warnings when multiplying
// ARRAYSIZE and sizeof(arr[0])
#define V_ARRAY_SIZE(A) (sizeof(*::ArraySizeHelper(A))+0)

template<class T> void V_IGNORE_UNUSED( const T& ) { }

// Prevent classes from being 'optimized' away by some linkers.
#define FORCE_LINKDYNCLASS(_classname) {if (_classname::GetClassTypeId()==NULL) {_classname temp;}}
#define FORCE_LINKDYNCLASS_ARG(_classname,arg) {if (_classname::GetClassTypeId()==NULL) {_classname temp(arg);}}


////////////////////////////////////////////////////////////////
//  Vision includes
////////////////////////////////////////////////////////////////

//VBase include
#include <Vision/Runtime/Base/vBaseImpExp.hpp>
#include <Vision/Runtime/Base/System/Memory/VBaseMem.hpp>
#include <Vision/Runtime/Base/Vulpine.hpp>

inline char* vStrDup(const char* the_string)         
{
  if (the_string)
  {
    char* szTempBuffer = (char*)vMemAlloc(strlen(the_string)+1);
    VASSERT(szTempBuffer);
	#pragma warning(push)
	#pragma warning(disable:4996)
    return (strcpy(szTempBuffer, the_string)); 
	#pragma warning(pop)
  }
  return (NULL);
}
inline wchar_t* vwStrDup(const wchar_t* the_string)
{
  if (the_string) 
  {
    wchar_t* szTempBuffer = (wchar_t*)vMemAlloc((wcslen(the_string)+1)*sizeof(wchar_t));
    VASSERT(szTempBuffer);
	#pragma warning(push)
	#pragma warning(disable:4996)
    return (wcscpy(szTempBuffer, the_string)); 
	#pragma warning(pop)
  }
    return (NULL);
}
inline void vStrDupOver(char*& dst, const char* src)  {vStrFree(dst); if(src) dst=vStrDup(src); else dst=NULL;}

V_COMPILE_ASSERT(sizeof(vtime_t)==8);  ///< time_t must be 64-bits (see above)

#ifdef SPU
  #include <Vision/Runtime/Base/Graphics/VColor.hpp>
  #include "VMath/VMatrix.hpp"
  #include <Vision/Runtime/Base/Math/Primitive/VLine.hpp>
  #include <Vision/Runtime/Base/Math/Deprecated/VPlane.hpp>
  #include <Vision/Runtime/Base/Physics/VIntersect.hpp>
  #include <Vision/Runtime/Base/Math/Deprecated/VMappingVertex.hpp>
  #include <Vision/Runtime/Base/Math/Deprecated/VVertex2.hpp>
  #include <Vision/Runtime/Base/Math/Primitive/VRectanglef.hpp>

  #include <Vision/Runtime/Base/Math/hkvMath.h>
  #include <Vision/Runtime/Base/Math/Vector/hkvVec2.h>
  #include <Vision/Runtime/Base/Math/Vector/hkvVec3.h>
  #include <Vision/Runtime/Base/Math/Vector/hkvVec3d.h>
  #include <Vision/Runtime/Base/Math/Vector/hkvVec4.h>
  #include <Vision/Runtime/Base/Math/Matrix/hkvMat3.h>
  #include <Vision/Runtime/Base/Math/Matrix/hkvMat4.h>
  #include <Vision/Runtime/Base/Math/Plane/hkvPlane.h>
  #include <Vision/Runtime/Base/Math/BoundingVolume/hkvAlignedBBox.h>
  #include <Vision/Runtime/Base/Math/BoundingVolume/hkvBoundingSphere.h>
#else

  //The order of these includes are important, these classes depend on each other
  #include <Vision/Runtime/Base/System/IO/Serialization/VSerialX.hpp>
  #include <Vision/Runtime/Base/String/VString.hpp>
  
  #include <Vision/Runtime/Base/Container/DynArray.hpp>
  #include <Vision/Runtime/Base/Container/VLinkedList.hpp>
  #include <Vision/Runtime/Base/Types/VType.hpp>
  #include <Vision/Runtime/Base/Container/VPList.hpp>
  #include <Vision/Runtime/Base/Container/VCollection.hpp>
  #include <Vision/Runtime/Base/System/VRefCounter.hpp>

  #include <Vision/Runtime/Base/System/IO/System/VFileAccess.hpp>
  #include <Vision/Runtime/Base/System/IO/Stream/IVFileStream.hpp>
  #include <Vision/Runtime/Base/System/IO/Serialization/VSerializationProxy.hpp>
  #include <Vision/Runtime/Base/System/IO/Serialization/VArchive.hpp>
  
  

  #include <Vision/Runtime/Base/Graphics/VColor.hpp>

  //misc includes  
  #include <Vision/Runtime/Base/Graphics/Textures/VTex.hpp>
  #include <Vision/Runtime/Base/System/ModuleSystem/VModule.hpp>
  #include <Vision/Runtime/Base/Types/VTypeManager.hpp>
  #include <Vision/Runtime/Base/Action/VAction.hpp>
  #include <Vision/Runtime/Base/Action/VActionManager.hpp>
  #include <Vision/Runtime/Base/System/VArgList.hpp>
  #include <Vision/Runtime/Base/System/VWeakPtr.hpp>
  #include <Vision/Runtime/Base/System/VUserDataObj.hpp>
  #include <Vision/Runtime/Base/Profiling/VProfiling.hpp>
  #include <Vision/Runtime/Base/System/VProgressStatus.hpp>
  #include <Vision/Runtime/Base/System/VSingleton.hpp>
  #include <Vision/Runtime/Base/System/vEndianSwitch.h>
  #include <Vision/Runtime/Base/Types/VTypedObjectReference.hpp>

  #include <Vision/Runtime/Base/System/Command/VCommand.hpp>
  #include <Vision/Runtime/Base/System/Command/VCommandManager.hpp>
  #include <Vision/Runtime/Base/String/VString.hpp>
  #include <Vision/Runtime/Base/String/VStringUtil.hpp>
  #include <Vision/Runtime/Base/System/VVarType.hpp>
  #include <Vision/Runtime/Base/String/VStringTokenizer.hpp>
  #include <Vision/Runtime/Base/System/VLocale.hpp>

  #include <Vision/Runtime/Base/System/VNameValueListParser.hpp>
  #include <Vision/Runtime/Base/System/VRestoreValue.hpp>
  #include <Vision/Runtime/Base/System/IO/Serialization/VArchive.hpp>


  #include <Vision/Runtime/Base/Container/vstrlist.hpp>
  #include <Vision/Runtime/Base/Container/VMaps.hpp>
  #include <Vision/Runtime/Base/Container/VArray.hpp>
  #include <Vision/Runtime/Base/Container/VElementCache.hpp>
  #include <Vision/Runtime/Base/Container/VRawDataBlock.hpp>

  #include <Vision/Runtime/Base/Physics/VIntersect.hpp>

  #include <Vision/Runtime/Base/Math/hkvMath.h>
  #include <Vision/Runtime/Base/Math/Vector/hkvVec2.h>
  #include <Vision/Runtime/Base/Math/Vector/hkvVec3.h>
  #include <Vision/Runtime/Base/Math/Vector/hkvVec3d.h>
  #include <Vision/Runtime/Base/Math/Vector/hkvVec4.h>
  #include <Vision/Runtime/Base/Math/Matrix/hkvMat3.h>
  #include <Vision/Runtime/Base/Math/Matrix/hkvMat4.h>
  #include <Vision/Runtime/Base/Math/Plane/hkvPlane.h>
  #include <Vision/Runtime/Base/Math/BoundingVolume/hkvAlignedBBox.h>
  #include <Vision/Runtime/Base/Math/BoundingVolume/hkvBoundingSphere.h>

  #include <Vision/Runtime/Base/Math/hkvMathHelpers.h>
  #include <Vision/Runtime/Base/Math/Primitive/VLine.hpp>


  #include <Vision/Runtime/Base/Graphics/Shader/vShaderFXParser.hpp>
  #include <Vision/Runtime/Base/Graphics/Shader/vShaderIncludes.hpp>

  #include <Vision/Runtime/Base/Graphics/Shader/vCompiledEffect.hpp>
  #include <Vision/Runtime/Base/Graphics/Shader/vCompiledShaderPass.hpp>
  #include <Vision/Runtime/Base/Graphics/Shader/vConstantBuffer.hpp>
  #include <Vision/Runtime/Base/Graphics/Shader/vCompiledShaderManager.hpp>
  #include <Vision/Runtime/Base/Graphics/Shader/vShaderEffectLib.hpp>
  #include <Vision/Runtime/Base/Graphics/Shader/vShaderParam.hpp>
  #include <Vision/Runtime/Base/Graphics/Shader/vShaderPassResource.hpp>
  #include <Vision/Runtime/Base/Graphics/Shader/vTechniqueConfig.hpp>
  #include <Vision/Runtime/Base/Graphics/Shader/vSimpleRenderState.hpp>
  #include <Vision/Runtime/Base/Graphics/Shader/vEffectAssignment.hpp>

  #include <Vision/Runtime/Base/Graphics/Textures/VTextureObject.hpp>

  

  #include <Vision/Runtime/Base/Graphics/Video/VRenderInterface.hpp>
  #include <Vision/Runtime/Base/Graphics/Video/VVideo.hpp>
  #include <Vision/Runtime/Base/Graphics/Video/VVideoCaps.hpp>
  #include <Vision/Runtime/Base/Graphics/Video/VVertexDescriptor.hpp>

  #include <Vision/Runtime/Base/System/IO/Stream/VMemoryStream.hpp>
  #include <Vision/Runtime/Base/System/IO/RevisionControl/IVRevisionControlSystem.hpp>
  #include <Vision/Runtime/Base/System/IO/System/VFileAccessManager.hpp>
  
  #include <Vision/Runtime/Base/Graphics/Textures/VTextureManager.hpp>
  #include <Vision/Runtime/Base/System/Resource/VResourceSnapshot.hpp>

  #include <Vision/Runtime/Base/System/VCallbacks.hpp>

  #include <Vision/Runtime/Base/VGL/VGL.hpp>

  #ifdef WIN32

    // WIN32 vBase only includes
    #include <Vision/Runtime/Base/System/Param/VParam.hpp>
    #include <Vision/Runtime/Base/System/Param/VParamArray.hpp>
    #include <Vision/Runtime/Base/System/Param/VParamContainer.hpp>
    #include <Vision/Runtime/Base/System/Param/VParamBlock.hpp>
    #include <Vision/Runtime/Base/System/Param/VParamComposite.hpp>

  #endif

  #include <Vision/Runtime/Base/Test/vTestUnit.hpp>
  #include <Vision/Runtime/Base/Test/vTestClassImpl.hpp>
  #include <Vision/Runtime/Base/Test/VImageComparison.hpp>

  #include <Vision/Runtime/Base/System/Threading/ThreadManager/VThreadedTask.hpp>
  #include <Vision/Runtime/Base/System/Threading/ThreadManager/VManagedThread.hpp>
  #include <Vision/Runtime/Base/System/Threading/ThreadManager/VThreadManager.hpp>
    
  VBASE_IMPEXP VModule *VBaseInit();
  VBASE_IMPEXP VModule *VBaseDeInit();


  //Input Handler
  #include <Vision/Runtime/Base/Input/VInput.hpp>
  #include <Vision/Runtime/Base/Input/VInputTouch.hpp>

  #ifdef SUPPORTS_SOFTKEYBOARD
    #include <Vision/Runtime/Base/Input/VSoftKeyboardAdapter.hpp>
  #endif

  #ifdef WIN32
    #if !defined(_VISION_NO_XINPUT) && !defined(_VISION_APOLLO)
      #include <Vision/Runtime/Base/Input/VInputXI.hpp>
    #endif
    #if !defined(_VISION_WINRT)  // No DirectInput
      #include <Vision/Runtime/Base/Input/VInputPC.hpp>
    #else
      #include <Vision/Runtime/Base/Input/VInputWinRT.hpp>
    #endif

  #elif defined(_VISION_XENON)
    #include <Vision/Runtime/Base/Input/VInputXI.hpp>
    #include <Vision/Runtime/Base/Input/VInputXenon.hpp>
  
  #elif defined(_VISION_PS3)
    #include <Vision/Runtime/Base/Input/VInputPS3.hpp>
    #include <Vision/Runtime/Base/System/Threading/SpursHandler/VSpursHandler.hpp>
    #include <Vision/Runtime/Base/System/Threading/SpursHandler/VSpuPrintfService.h>
  
  #elif defined(_VISION_LINUX)
    #include <Vision/Runtime/Base/Input/VInputX.hpp>

  #elif defined(_VISION_PSP2)
    #include <Vision/Runtime/Base/Input/VInputPSP2.hpp>

  #elif defined(_VISION_IOS)
    #include <Vision/Runtime/Base/Input/VInputIOS.hpp>

  #elif defined(_VISION_ANDROID)
    #include <Vision/Runtime/Base/Input/VInputAndroid.hpp>

  #elif defined(_VISION_WIIU)
    #include <Vision/Runtime/Base/Input/VInputWiiU.hpp>

  #else
    #error Undefined platform!
  #endif

#endif  //SPU

// helper variable to store the Vision::Editor.IsInEditor state also accessible in vBase
namespace VEditor
{
  VBASE_IMPEXP void SetInEditor(bool bStatus);
  extern bool g_bIsInEditor;
}

// if the linker complains that the following functions are missing
// you did link a release build against a debug version or vise versa
#ifdef HK_DEBUG_SLOW
VBASE_IMPEXP void vBaseInterDebugReleaseLinkingCheck();
#else
VBASE_IMPEXP void vBaseInterReleaseDebugLinkingCheck();
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
