/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VSingleton.hpp

#ifndef VSINGLETON_HPP_INCLUDED
#define VSINGLETON_HPP_INCLUDED

/// \brief
///   Template for Singleton pattern.
template <class T_CLASS> class VSingleton
{
public:

  /// \brief
  ///   Gets the singleton instance (creates it if not present yet).
  static T_CLASS& GetInstance()
  {
    if (!g_pInstance)
      g_pInstance = new T_CLASS();
    return *g_pInstance;
  }

  /// \brief
  ///   Deinitializes the singleton by destroying the object instance.
  static void DeInit()
  {
    V_SAFE_DELETE(g_pInstance);
  }

protected:
  VSingleton() {};

private:
  // avoid creation
  VSingleton(const VSingleton&);
  VSingleton& operator=(const VSingleton&);

private:
  static T_CLASS *g_pInstance;
};

#endif //  VSINGLETON_HPP_INCLUDED

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
