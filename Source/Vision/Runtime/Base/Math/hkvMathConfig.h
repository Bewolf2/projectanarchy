/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef VBASE_HKVMATH_HKVMATHCONFIG_H
#define VBASE_HKVMATH_HKVMATHCONFIG_H

#include <Vision/Runtime/Base/Math/hkvDeprecationMacros.h>

#ifdef SPU
#include <Vision/Runtime/Base/System/VSpuDefs.hpp>
#endif

// this is currently only a placeholder and will be replaced by HKV_FORCE_INLINE in the near future
#define HKV_FORCE_INLINE inline 
// this is currently only a placeholder and will be replaced by HKV_RESTRICT in the near future
#define HKV_RESTRICT

// Types defined in hkBase, redefined here until we include that header
typedef unsigned int    hkUint32;
typedef          int    hkInt32;
typedef unsigned short  hkUint16;
typedef          short  hkInt16;
typedef unsigned char   hkUint8;
typedef signed   char   hkInt8;

// this is currently only a placeholder and will be replaced by hkAddByteOffset in the near future
template <typename TYPE>
TYPE* hkvAddByteOffset( TYPE* base, hkUint32 offset )
{
  return reinterpret_cast<TYPE*>( reinterpret_cast<char*>(base) + offset );
}

/// \brief a simple success/failure enum.
enum hkvResultEnum
{
  HKV_SUCCESS = 0,
  HKV_FAILURE = 1
};

/// brief this is currently only a placeholder and will be replaced by hkResult in the near future
struct hkvResult
{
  /// \brief Default Constructor, does not initialize the result type.
  HKV_FORCE_INLINE hkvResult()
  {
  }

  /// \brief Copy Constructor.
  HKV_FORCE_INLINE hkvResult (hkvResultEnum b)
  {
    m_enum = b;
  }

  /// \brief Assignment operator.
  HKV_FORCE_INLINE hkvResult& operator=(hkvResultEnum e)
  {
    m_enum = e;
    return *this;
  }

  /// \brief Compares for equality.
  HKV_FORCE_INLINE bool operator==(hkvResultEnum e) const
  {
    return m_enum == e;
  }

  /// \brief Compares for inequality.
  HKV_FORCE_INLINE bool operator!=(hkvResultEnum e) const
  {
    return m_enum != e ;
  }

private:
  hkvResultEnum m_enum;
};
  
// Vision Engine Math Classes
class VArchive;
class hkvMath;
class hkvVec2;
class hkvVec3;
class hkvVec3d;
class hkvVec4;
class hkvMat3;
class hkvMat4;
class hkvPlane;
class hkvBoundingSphere;
class hkvAlignedBBox;
class hkvQuat;

// *** OLD STUFF ***
class VTriangle;

// If this is defined several math functions will strictly validate their input data (in debug mode only).
// E.g. a function that requires a normalized vector as input, will check this and fire an assert, if
// that prerequisite is not met.
// Since this can often lead to a large number of asserts being fired (due to invalid data being passed
// to these functions, especially during startup), this switch is disabled by default.
// The Vision Engine is tested with this and thus should not assert with it being enabled.
// Enabling this switch will allow to more easily find usage errors that might result in corrupted states.
//#define HKVMATH_ENABLE_STRICT_VALIDATION


// If defined all the default constructors of the math classes are deprecated.
// That means people must use constructors that specify exactly with what value
// the class should be initialized.
// This should be used to convert between old and new initialization methods.
// Also see the macros HKV_DISABLE_DEPRECATION and HKV_ENABLE_DEPRECATION.
//#define HKVMATH_DEPRECATE_DEFAULT_CONSTRUCTOR

// Setting this flag will deprecate all functions from the old interface that are
// very critical to get rid of (because they are interacting with classes that are not supported anymore)
// Also see the macros HKV_DISABLE_DEPRECATION and HKV_ENABLE_DEPRECATION.
#define HKVMATH_DEPRECATE_OLD_INTERFACE_STAGE_1

// Setting this flag will deprecate all functions from the old interface that are
// critical to get rid of (because they are really bad)
// Also see the macros HKV_DISABLE_DEPRECATION and HKV_ENABLE_DEPRECATION.
#define HKVMATH_DEPRECATE_OLD_INTERFACE_STAGE_2

// Setting this flag will deprecate all functions from the old interface that are
// less critical to get rid of (because they are usually just a different name for some function)
// Also see the macros HKV_DISABLE_DEPRECATION and HKV_ENABLE_DEPRECATION.
#define HKVMATH_DEPRECATE_OLD_INTERFACE_STAGE_3

// Setting this flag will deprecate all functions from the old interface that are still in 'use' by some legacy code.
//#define HKVMATH_DEPRECATE_OLD_INTERFACE_STAGE_4


// the old behavior is that all math classes initialize their data in the constructor
// this is performance wise not very good, but for compatibility it is currently still enabled
// this setting is deprecated and in the future all constructors will leave their data uninitialized
#define HKVMATH_DEFAULTCONSTRUCTORS_INITIALIZEDATA

//#define HKVMATH_ENABLE_OLD_OPERATORS

// enables operator* (Matrix, Matrix) with the new (consistent) conventions
// without this one has to use 'multiply' for matrix multiplications
// this should not be enabled too soon, otherwise customers might port code that still uses the old convention
// and it will compile (but not work)
//#define HKVMATH_ENABLE_NEW_OPERATORS

//#define HKVMATH_ENABLE_NEW_SERIALIZATION_OPERATORS


#ifdef HKVMATH_DEPRECATE_DEFAULT_CONSTRUCTOR
  #define HKVMATH_DEFAULT_CONSTRUCTOR HKV_DEPRECATED
#else
  #define HKVMATH_DEFAULT_CONSTRUCTOR
#endif

#ifdef HKVMATH_DEPRECATE_OLD_INTERFACE_STAGE_1
  #define HKVMATH_DEPRECATED_STAGE1 HKV_DEPRECATED
#else
  #define HKVMATH_DEPRECATED_STAGE1
#endif

#ifdef HKVMATH_DEPRECATE_OLD_INTERFACE_STAGE_2
  #define HKVMATH_DEPRECATED_STAGE2 HKV_DEPRECATED
#else
  #define HKVMATH_DEPRECATED_STAGE2
#endif

#ifdef HKVMATH_DEPRECATE_OLD_INTERFACE_STAGE_3
  #define HKVMATH_DEPRECATED_STAGE3 HKV_DEPRECATED
#else
  #define HKVMATH_DEPRECATED_STAGE3
#endif

#ifdef HKVMATH_DEPRECATE_OLD_INTERFACE_STAGE_4
  #define HKVMATH_DEPRECATED_STAGE4 HKV_DEPRECATED
#else
  #define HKVMATH_DEPRECATED_STAGE4
#endif

#ifdef HKVMATH_ENABLE_STRICT_VALIDATION
  #define HKVMATH_ASSERT(exp, msg) VASSERT_ALWAYS_MSG(exp, msg)
#else
  #define HKVMATH_ASSERT(exp, msg) 
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
