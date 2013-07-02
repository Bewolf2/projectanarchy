/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef __VISION_LOD_OBJECT_HPP
#define __VISION_LOD_OBJECT_HPP

#include <Vision/Samples/Engine/CharacterLOD/EntityLODComponent.hpp>

// ***********************************************************************************************
// Class: LODObject
// Copyright (C) Havok.com Inc. All rights reserved.
// ***********************************************************************************************
//
// Simple entity class that implements LOD via the re-usable EntityLODComponent class
//
// ***********************************************************************************************


class LODObject : public VisBaseEntity_cl
{
public:
  V_DECLARE_SERIAL( LODObject, )
  IMPLEMENT_OBJ_CLASS(LODObject);

  // constructor
  LODObject(void);

  //VisBaseEntity_cl overrides
  VOVERRIDE void InitFunction();
  VOVERRIDE void PreThinkFunction();

  // triggers animation
  void SetPlayAnimation(bool bStatus)
  {
    m_LOD.SetFrozen(!bStatus);
    if (bStatus)
      m_spRootAnimCtrl->Play();
    else
      m_spRootAnimCtrl->Pause();
  }

  VisSkeletalAnimControlPtr m_spRootAnimCtrl;

public:
  EntityLODComponent m_LOD;

};

#endif //__VISION_LOD_OBJECT_HPP

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
