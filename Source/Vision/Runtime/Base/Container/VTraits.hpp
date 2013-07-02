/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#pragma once
#ifndef VTRAITS_HPP
#define VTRAITS_HPP

/// \brief
///  Type traits for compile time checks
namespace VTraits 
{
  #if !defined _VISION_DOC
  typedef char Yes; 
  typedef int No;
  #endif

  /// \brief
  ///   Test if one type is implicitly convertible into another.
  ///   Usage: test if a is convertible into b
  ///   VTraits::IsImplicitlyConvertible<a,b>::value
  template<class FROM, class TO>  
  struct IsImplicitlyConvertible
  {
#if !defined _VISION_DOC
  private:
    static Yes Test( TO );
    static No Test( ... );
    static FROM TestType();
  public:
    static const bool value = (sizeof(Test(TestType())) == sizeof(Yes)) ? true : false;
#endif // _VISION_DOC
  };

  /// \brief Checks if a class is derived from another class.
  template <typename B, typename D>
  struct IsBaseOf
  {
#if !defined _VISION_DOC
  private:
    static Yes Test(B*);
    static No Test(...);

    static D* TestType(void);

  public:
    static const bool value = sizeof(Test(TestType())) == sizeof(Yes);
#endif
  };

  /// \brief Makes a type from a pointer-to-type.
  template<class T> struct RemovePointer
  {
    typedef T type;
  };

#if !defined(_VISION_DOC)
  template<class T> struct RemovePointer<T*>
  {
    typedef T type;
  };

  template<class T> struct RemovePointer<const T*>
  {
    typedef T type;
  };

  template<class T> struct RemovePointer<volatile T*>
  {
    typedef T type;
  };

  template<class T> struct RemovePointer<const volatile T*>
  {
    typedef T type;
  };
#endif
}

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
