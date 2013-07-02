/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file Vulpine.hpp



//
// ** Vulpine.h **
//
// This file contains the important definitions for any Trinigy project.
//

#ifndef VULPINE_H_INCLUDED
#define VULPINE_H_INCLUDED

typedef signed    long  int   SLONG;  ///< 32 bit signed.
typedef unsigned  long  int   ULONG;  ///< 32 bit unsigned.

typedef signed          int   SINT;  ///< 32 bit signed.
typedef unsigned        int   UINT;  ///< 32 bit unsigned.

typedef signed    short int   SSHORT; ///< 16 bit signed.
typedef unsigned  short int   USHORT; ///< 16 bit unsigned.

typedef signed          char  SBYTE;  ///< 8 bit signed.
typedef unsigned        char  UBYTE;  ///< 8 bit unsigned.

typedef                 SLONG RETVAL; ///< Return value, with error codes

typedef unsigned char         BYTE;   ///<  8 bit unsigned
typedef unsigned short        WORD;   ///< 16 bit unsigned
typedef unsigned long         DWORD;  ///< 32 bit unsigned

typedef                 int   BOOL;   ///< 32 bit boolean
typedef                 int   VBool;  ///< Vision 32bit bool

#ifndef SPU
typedef                 INT_PTR   VWND;   ///< 32/64-bit window handle (HWND replacement)
#endif


#ifndef NULL
  #define NULL 0
#endif

#ifndef TRUE
  #define TRUE 1
#endif

#ifndef FALSE
  #define FALSE 0
#endif

#ifndef VIS_INVALID
  #define VIS_INVALID ((unsigned int)0xFFFFFFFF)
#endif




// Default values, some redundant
const SLONG VERR_NOERROR        =     0;   ///< Everything is okay
const SLONG VERR_ERROR          =    -1;   ///< Generic error. Something went wrong.
const SLONG VERR_UNKNOWN        =    -2;   ///< Unknown error. I don't know what's happening.
const SLONG VERR_QUIT           =    -3;   ///< Application has (or wants to) quit

const SLONG VERR_RANDOM         =   -17;   ///< Really arbitrary error

//
// *** Standard errors **
// 

// Numerical errors
const SLONG VERR_ILLEGALVALUE   =  -100;   ///< An illegal value was either 
const SLONG VERR_NAN            =  -101;   ///< The answer is not a number
const SLONG VERR_INF            =  -102;   ///< The answer is infinity

// File errors
const SLONG VERR_FILEERROR      =  -200;   ///< Generic file error
const SLONG VERR_FILENOTFOUND   =  -201;   ///< File could not be found
const SLONG VERR_CANTOPEN       =  -202;   ///< Can't open file
const SLONG VERR_CANTREAD       =  -203;   ///< Can't read from file
const SLONG VERR_CANTWRITE      =  -204;   ///< Can't write to file
const SLONG VERR_NOTREADY       =  -205;   ///< Device not ready when writing
const SLONG VERR_PATHNOTFOUND   =  -206;   ///< The path could not be found (probably illegal)
const SLONG VERR_EOF            =  -207;   ///< Can't read beyond end of file
const SLONG VERR_FILEEXISTS     =  -208;   ///< file or directory already exists
const SLONG VERR_NOTEMPTY       =  -209;   ///< Given path is not a directory; directory is not empty; or directory is either current working directory or root directory. 
const SLONG VERR_READONLY       =  -210;   ///< file is read only
const SLONG VERR_NOTSUPPORTED   =  -211;   ///< function not supported by the file system
const SLONG VERR_WRONGVERSION   =  -212;   ///< file has the wrong version

// Memory errors
const SLONG VERR_MEMERROR       =  -300;   ///< Generic memory error
const SLONG VERR_NOMEM          =  -301;   ///< Out of memory. No memory is left

// Graphics errors
const SLONG VERR_GRAPHERROR     =  -400;   ///< Generic graphics error
const SLONG VERR_WRONGMODE      =  -401;   ///< Wrong screen mode
const SLONG VERR_NOSCREENMODE   =  -402;   ///< No (such) screen mode available
const SLONG VERR_CHANGERESTART  =  -403;   ///< You need to restart the computer to change to this mode
const SLONG VERR_PIXELFORMAT    =  -404;   ///< Pixel format descriptor error

// Generic device errors
const SLONG VERR_DEVICEERROR    =  -500;   ///< Generic device error
const SLONG VERR_NODEVICE       =  -501;   ///< No such device is available
const SLONG VERR_DEVICENOTINIT  =  -502;   ///< Requested device is not [properly] initialized

// VFS errors:
const SLONG VERR_INVALIDVMF     =  -600;   ///< Package is invalid or wrong package version

// Other errors


// OpenGL errors
const SLONG VERR_GLERROR        = -1000;   ///< Generic OpenGL error
const SLONG VERR_NOOPENGL       = -1001;   ///< OpenGL could not be found (or initialized)
const SLONG VERR_NOGLCONTEXT    = -1002;   ///< No OpenGL Context was found

// Windows errors
const SLONG VERR_WINERROR       = -2000;   ///< Generic windows error
const SLONG VERR_NOWINDOW       = -2001;   ///< Window is not present or could not be created

// DirectX errors
const SLONG VERR_DXERROR        = -2100;   ///< Generic DirectX error
const SLONG VERR_NODX           = -2101;   ///< DirectX could not be found (or initialized)
const SLONG VERR_WRONGDXVER     = -2102;   ///< Wrong DirectX version (too old)

const SLONG VERR_DDRAWERROR     = -2110;   ///< Generic DirectDraw error
const SLONG VERR_NODDRAW        = -2111;   ///< DirectDraw could not be found (or initialized)
const SLONG VERR_WRONGDDRAWVER  = -2112;   ///< Wrong DirectDraw version (too old)

const SLONG VERR_DINPUTERROR    = -2120;   ///< Generic DirectInput error
const SLONG VERR_NODINPUT       = -2121;   ///< DirectInput could not be found (or initialized)
const SLONG VERR_WRONGDINPUTVER = -2122;   ///< Wrong DirectInput version (too old)
const SLONG VERR_NODINPUTDEVICE = -2123;   ///< No (such) DirectInput device was found (or initialized)

const SLONG VERR_DSOUNDERROR    = -2130;   ///< Generic DirectSound error
const SLONG VERR_NODSOUND       = -2131;   ///< DirectSound could not be found (or initialized)
const SLONG VERR_WRONGDSOUNDVER = -2132;   ///< Wrong DirectSound version (too old)

const SLONG VERR_DPLAYERROR     = -2140;   ///< Generic DirectPlay error
const SLONG VERR_NODPLAY        = -2141;   ///< DirectPlay could not be found (or initialized)
const SLONG VERR_WRONGDPLAYVER  = -2142;   ///< Wrong DirectPlay version (too old)

const SLONG VERR_DMUSICERROR    = -2150;   ///< Generic DirectMusic error
const SLONG VERR_NODMUSIC       = -2151;   ///< DirectMusic could not be found (or initialized)
const SLONG VERR_WRONGDMUSICVER = -2152;   ///< Wrong DirectMusic version (too old)

const SLONG VERR_D3DERROR       = -2160;   ///< Generic Direct3d error
const SLONG VERR_NOD3D          = -2161;   ///< Direct3d could not be found (or initialized)
const SLONG VERR_WRONGD3DVER    = -2162;   ///< Wrong Direct3d version (too old)

const SLONG VERR_DGPFERROR      = -2190;   ///< Generic DirectGPF error
const SLONG VERR_NODGPF         = -2191;   ///< DirectGPF could not be found (or initialized)
const SLONG VERR_WRONGDGPFVER   = -2192;   ///< Wrong DirectGPF version (too old)

// Linux errors

const SLONG VERR_LINUXMAGIC     = -4711;   ///< Magic linux arbitrary error

// Mac errors

// Other platform errors

//
// *** ***
//


#if defined (_VISION_WIIU)
  void WiiUSysBreak();
#endif

#if defined (_WIN64)
  //64-bit
/*
  #include <intrin.h>
  void __break(int);
  #pragma intrinsic (__break)
  #define VDBGBREAK __break(0x80016)
*/
  #define VDBGBREAK __debugbreak()

#elif defined (WIN32)
  // This produces an debug interrupt on 32-bit Intel/AMD processors
  #ifndef VDBGBREAK
    #if (defined(_MANAGED) && (_MSC_VER >= 1400)) || defined(_M_ARM)   ///<MSVC 8.0
      #define VDBGBREAK { __debugbreak(); }
    #else
      #define VDBGBREAK { __asm { int 3 } }
    #endif
  #endif

#elif defined (_VISION_PS3)
  //MK: This was described as a valid way of raising a debug break. The PPC
  //  manual also mentions it as a shortcut mnemonic for "tw 31,0,0", which
  //  is an unconditional trap.
  #define VDBGBREAK { __asm__ volatile("tw 31,0,0"); }

#elif defined (_VISION_WIIU)
  #define VDBGBREAK OSDebug();

#elif defined(_VISION_PSP2)
  #include <libdbg.h>
  #define VDBGBREAK SCE_BREAK();

#elif defined (_VISION_LINUX)
  #define VDBGBREAK asm("int $3");

#elif defined(_VISION_IOS)
  #define VDBGBREAK __builtin_trap();

#elif defined(_VISION_ANDROID)
  #define VDBGBREAK raise(SIGTRAP);
#else
  //XBox 360, etc.
  #define VDBGBREAK DebugBreak()
#endif

// Visual studio 2010 predefined constant
#ifdef CODE_ANALYSIS
#define _VISION_STATIC_ANALYSIS
#endif

#ifdef _VISION_STATIC_ANALYSIS
//The !! is to make it work with smart pointers
#define ANALYSIS_ASSUME(x) __analysis_assume(!!(x));
#define ANALYSIS_NO_RETURN __analysis_noreturn
#define ANALYSIS_IGNORE_WARNING_ONCE(x) __pragma(warning(suppress:x))
#define ANALYSIS_IGNORE_WARNING_BLOCK_START(x) __pragma(warning(push)) __pragma(warning(disable:x))
#define ANALYSIS_IGNORE_WARNING_BLOCK_END __pragma(warning(pop))
#define ANALYSIS_IGNORE_ALL_START __pragma(warning(push)) \
  __pragma(warning(disable: 6211 6001 6326 6011 6385 6031 6387 6246 6386 4251))
#define ANALYSIS_IGNORE_ALL_END __pragma(warning(pop))
#else
#define ANALYSIS_ASSUME(x)
#define ANALYSIS_NO_RETURN
#define ANALYSIS_IGNORE_WARNING_ONCE(x)
#define ANALYSIS_IGNORE_WARNING_BLOCK_START(x)
#define ANALYSIS_IGNORE_WARNING_BLOCK_END
#define ANALYSIS_IGNORE_ALL_START
#define ANALYSIS_IGNORE_ALL_END
#endif


// assertion that is present in debug and release code
#ifndef VASSERT_ALWAYS

  BOOL VBASE_IMPEXP VAssert(const char* szFile, int iLine, const char* pszText, const char* pszMsg = 0);

  typedef BOOL (*VAssertHandler)(const char* szFile, int iLine, const char* pszText, const char* pszMsg);
  VAssertHandler VBASE_IMPEXP VAssertSetHandler(VAssertHandler pfHandler);

#ifndef _VISION_WIIU

#define VASSERT_ALWAYS(x) { ANALYSIS_ASSUME(x) \
  if( !(x) ) \
  if( VAssert(__FILE__, __LINE__, #x) ) \
    VDBGBREAK; }

#define VASSERT_ALWAYS_MSG(x, msg,...) { ANALYSIS_ASSUME(x)                    \
  if( !(x) )                                                                   \
  {                                                                            \
    VString assert_macro_msg;                                             \
    assert_macro_msg.Format((msg), ##__VA_ARGS__);                          \
    if( VAssert(__FILE__, __LINE__, #x, assert_macro_msg) )               \
      VDBGBREAK;                                                               \
  }}

#else

#define VASSERT_ALWAYS(x) { ANALYSIS_ASSUME(x) \
  if( !(x) ) \
  if( VAssert(__FULL_FILE__, __LINE__, #x) ) \
  VDBGBREAK; }

#define VASSERT_ALWAYS_MSG(x, msg,...) { ANALYSIS_ASSUME(x)                    \
  if( !(x) )                                                                   \
  {                                                                            \
    VString assert_macro_msg;                                             \
    assert_macro_msg.Format((msg), ##__VA_ARGS__);                          \
    if( VAssert(__FULL_FILE__, __LINE__, #x, assert_macro_msg) )          \
      VDBGBREAK;                                                               \
  }}

#endif

#endif // ifndef(VASSERT_ALWAYS)


#if defined(HK_DEBUG)
  // normal assertion that is only present in debug build 
  #ifndef VASSERT
    #define VASSERT(x) VASSERT_ALWAYS(x)
    #define VASSERT_MSG(x,msg,...) VASSERT_ALWAYS_MSG(x,msg,##__VA_ARGS__)

    //This assert is triggered only once and if ignored won't show up again.
    #define VASSERT_ONCE(_exp) \
      { ANALYSIS_ASSUME(_exp) static bool s_once = true; \
      if(s_once && !(_exp)) { VASSERT(_exp); s_once = false; } }

    #define VASSERT_ONCE_MSG(_exp,_msg,...) \
      { ANALYSIS_ASSUME(_exp) static bool s_once = true; \
        if(s_once && !(_exp)) { VASSERT_MSG(_exp,_msg,##__VA_ARGS__); s_once = false; } \
      } 

  #endif
#else
  #ifndef VASSERT
    #define VASSERT(x) { ANALYSIS_ASSUME(x) }
    #define VASSERT_MSG(x,msg,...) { ANALYSIS_ASSUME(x) }
    #define VASSERT_ONCE(_exp) { ANALYSIS_ASSUME(_exp) }
    #define VASSERT_ONCE_MSG(_exp,_msg,...) { ANALYSIS_ASSUME(_exp) }
  #endif
#endif

//Paranoid asserts (could be much slower)
#ifdef PARANOID
  #define VASSERT_PARANOID(x)  VASSERT(x)
#else
  #define VASSERT_PARANOID(x)  VASSERT(x)
#endif

#ifndef ASSERT
  #define ASSERT(x) VASSERT(x)
#endif


// verify (which is kind of an assertion), but the expression is always executed!
#ifndef VVERIFY
  #define VVERIFY(exp) ANALYSIS_ASSUME(exp) \
    { \
      if (!(exp)) \
      { \
        VASSERT(FALSE && (exp)); \
      } \
    }
#endif

#ifndef VVERIFY_MSG
  #define VVERIFY_MSG(exp,msg) ANALYSIS_ASSUME(exp) \
    { \
      if (!(exp)) \
      { \
        VASSERT_MSG(FALSE && (exp),msg); \
      } \
    }
#endif


// always returns if expression is false, but asserts only in debug build
#ifndef VVERIFY_OR_RET
  #define VVERIFY_OR_RET(exp) ANALYSIS_ASSUME(exp) \
    { \
      if (!(exp)) \
        { \
          VASSERT(FALSE && (exp)); \
          return; \
        } \
    }
#endif


// always returns the passed value if expression is false, but asserts only in debug build
#ifndef VVERIFY_OR_RET_VAL
  #define VVERIFY_OR_RET_VAL(exp,returnvalue) ANALYSIS_ASSUME(exp) \
    { \
    if (!(exp)) \
      { \
        VASSERT(FALSE && (exp)); \
        return returnvalue; \
      } \
    }
#endif    




#if defined(WIN32) && !defined(_VISION_WINRT) // Win32 Desktop Apps 

  #define V_IS_VALID_READ_PTR(ptr)        (!::IsBadReadPtr((void*)(ptr), 4))
  #define V_IS_VALID_WRITE_PTR(ptr)       (!::IsBadWritePtr((void*)(ptr), 4))
  #define V_IS_VALID_CODE_PTR(ptr)        (!::IsBadCodePtr( (FARPROC)(void*)(ptr) ) )
  #define V_IS_VALID_READ_RANGE(ptr,size)  (!::IsBadReadPtr((void*)(ptr), size))
  #define V_IS_VALID_WRITE_RANGE(ptr,size) (!::IsBadWritePtr((void*)(ptr), size))
  
  //Is this VC specific ? I think so....
  #ifdef _WIN64
    #define V_IS_VALID_PTR(ptr)  (  ( (ptr) != 0) && ((uintptr_t) (ptr) > 4096) && ((void*)(ptr) != (void*)0xCDCDCDCDCDCDCDCD) && ((void*)(ptr) != (void*)0xCCCCCCCCCCCCCCCC) && ((void*)(ptr) != (void*)0xFDFDFDFDFDFDFDFD) && ((void*)(ptr) != (void*)0xFCFCFCFCFCFCFCFC) && ((void*)(ptr) != (void*)0xDDDDDDDDDDDDDDDD) )
  #else
    #define V_IS_VALID_PTR(ptr)  (  ( (ptr) != 0) && ((uintptr_t) (ptr) > 4096) && ((intptr_t)(ptr) != 0xCDCDCDCD) && ((intptr_t)(ptr) != 0xCCCCCCCC) && ((intptr_t)(ptr) != 0xFDFDFDFD) && ((intptr_t)(ptr) != 0xFCFCFCFC) && ((intptr_t)(ptr) != 0xDDDDDDDD) )
  #endif
#else
  //TODO imlement for Mac/Linux/...
  #define V_IS_VALID_READ_PTR(ptr)          (TRUE)
  #define V_IS_VALID_WRITE_PTR(ptr,size)    (TRUE)
  #define V_IS_VALID_CODE_PTR(ptr)          (TRUE)

  #define V_IS_VALID_READ_RANGE(ptr,size)   (TRUE)
  #define V_IS_VALID_WRITE_RANGE(ptr,size)  (TRUE)

  #define V_IS_VALID_PTR(ptr)               ( ptr != 0)
#endif


// Compile time output. The macro chMSG writes message provided as its argument
// into the build window together with source file name and line number at compile time.
// This macro has to be used in connection with the pragma directive. The main usage
// is to allow messages which can point to some temporary, todo or hacked code - e.g.
// if you make any hack which shoudn't be forgotten to be removed later, you can
// put the chMSG statement into the code near the hack to remind you to remove it.
// If you do that then message shows in the build window at compile time.
//
// Syntax: #pragma chMSG( ... your text here ...)
//
// Sample: the following statement writes the text "Hello world" into the build window:
//         #pragma chMSG( Hello world )
#define V_QUOTE(token) #token
#define chSTR(x) V_QUOTE(x)
#define chMSG(desc) message(__FILE__ "(" chSTR(__LINE__) "): " #desc)


  
// This macro should be considered deprecated. Use virtual instead and mark
// implementations in derived classes by HKV_OVERRIDE.
// Do not search+replace VOVERRIDE by virtual in existing code; rather check
// each individual occurrence and annotate with HKV_OVERRIDE as appropriate.
#define VOVERRIDE virtual

// Macro for the override c++ qualifier which is now supported by some of the compilers
// and checks if a function actually overrides

//Visual Studio 2008
#if defined(_MSC_VER) && _MSC_VER >= 1500
#define HKV_OVERRIDE override
#endif
//fall back
#ifndef HKV_OVERRIDE
#define HKV_OVERRIDE
#endif


// version stuff
#define MAKE_VERSION(major, minor) (((major)<<8) + (minor))

#define GET_VERSION_MAJOR(version) (version >> 8)
#define GET_VERSION_MINOR(version) (version & 0xFF)

#define GET_VERSION(version, major, minor) ( (major) = GET_VERSION_MAJOR(version); (minor) = GET_VERSION_MINOR(version); )
  
  
    
/// \brief Compile time assertion
/// Used to generate errors during compilation time if an assert fails
/// Doesn't need to be used in a code block, can be used anywhere in a header
///
/// Syntax V_COMPILE_ASSERT( ...compiletime condition... );
///
/// Sample: V_COMPILE_ASSERT(sizeof(VYourClass*) == 4 ); // pointer are 32 bits or 4 bytes
/// Sample: V_COMPILE_ASSERT(sizeof(VYourClass) == 16 ); // asserts if your class size is not 16 bytes
#if defined(_VISION_DOC) || (defined(_MSC_VER) && _MSC_VER >= 1600)
#define V_COMPILE_ASSERT(cond) static_assert(cond, "Compile time assertion failed: "#cond) // static_assert supported since VS2010 or GCC 4.3
#else
#define V_COMPILE_ASSERT(cond)   typedef int CompileTimeAssert[(cond) ? 1 : -1]
#endif


///////////////////////////////////////////////////////////
// If this gives an error during compiling then you forgot
// to change your project settings! (Properties-> C/C++ ->
// Code Generation->Structure Member Alignment)
// The Vision Engine is now compiled to use the default 
// structure member alignment and NOT 4 bytes anymore.
///////////////////////////////////////////////////////////

#ifndef _VISION_DOC
  // TODO (Platform Team): Fix this
  #ifndef _VISION_IOS
    class Vision_MemAlignTest { int a; double d; };
    V_COMPILE_ASSERT(sizeof(Vision_MemAlignTest) == 16);
  #endif
#endif



#ifndef va_copy
#define va_copy(dest, source) (dest) = (source)
#endif

#endif // #ifndef VULPINE_H_INCLUDED

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
