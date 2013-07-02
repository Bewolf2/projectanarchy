/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file vHavokRigidBody.hpp

// ***********************************************************************************************
// vHavok binding for Vision that uses Havok for physics
// Copyright (C) Trinigy GmbH. All rights reserved.
// ***********************************************************************************************
#ifndef VHAVOKUSERDATA_HPP_INCLUDED
#define VHAVOKUSERDATA_HPP_INCLUDED


///
/// \brief
///   Enum that defines the type of object that is encoded into a user data pointer.
///
enum vHavokUserDataType_e
{
  V_USERDATA_UNDEFINED    = 0,  ///< Collider type is undefined
  V_USERDATA_OBJECT       = 1,  ///< Havok rigid body object (can be dynamic or static) or Havok character controller
  V_USERDATA_STATIC       = 2,  ///< Static mesh
  V_USERDATA_TERRAIN      = 3,  ///< Terrain mesh

  V_USERDATA_TYPEMASK     = 3,  ///< Bitmask used to extract the type from the pointer
};


///
/// \brief
///   Internal class to hold two Havok user data void pointers.
///
/// \internal
///
struct vHavokUserDataPointerPair_t
{
  void *m_pUserData[2];

  inline vHavokUserDataType_e GetDataType(int iIndex) const
  {
    return (vHavokUserDataType_e)(((INT_PTR)m_pUserData[iIndex]) & V_USERDATA_TYPEMASK);
  }

  inline void* GetTypAndPointer(int iIndex, vHavokUserDataType_e &eType) const
  {
    return ExtractTypeAndPointer(m_pUserData[iIndex],eType);
  }

  static inline void* CombineTypeAndPointer(void *pPtr, vHavokUserDataType_e eType)
  {
    VASSERT_MSG((((hkUlong)pPtr)&V_USERDATA_TYPEMASK) == 0, "pointer must be 4-byte aligned");
    return (void *)(((hkUlong)pPtr) | eType); ///< the lowest 2 bits are used to store the type
  }

  static inline void* ExtractTypeAndPointer(void *pUserData, vHavokUserDataType_e &eType)
  {
    hkUlong iUserData = (hkUlong)pUserData;
    eType = (vHavokUserDataType_e)(iUserData & V_USERDATA_TYPEMASK);
    return (void *)(iUserData & ~(hkUlong)(V_USERDATA_TYPEMASK));
  }
};

#endif // VHAVOKUSERDATA_HPP_INCLUDED

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
