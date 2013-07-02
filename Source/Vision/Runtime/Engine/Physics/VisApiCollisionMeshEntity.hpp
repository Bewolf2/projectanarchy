/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiCollisionMeshEntity.hpp

#ifndef VISAPICOLLISIONMESHENTITY_HPP_INCLUDED
#define VISAPICOLLISIONMESHENTITY_HPP_INCLUDED

#include <Vision/Runtime/Engine/Physics/VisApiCollisionMeshBase.hpp>
#include <Vision/Runtime/Engine/Physics/VisApiCollisionMesh.hpp>

class VAnimatedCollisionMesh;
typedef VSmartPtr<VAnimatedCollisionMesh> VAnimatedCollisionMeshPtr;

/// \brief
///   Implements IVCollisionMesh for animated (skinned) meshes. See VisAnimConfig_cl::GetTraceMesh().
class VAnimatedCollisionMesh : public IVCollisionMesh
{
public:
  VAnimatedCollisionMesh(VisAnimConfig_cl *pAnimConfig, VBaseMesh *pMesh, VisCollisionMeshSource_e eSource);

  VOVERRIDE ~VAnimatedCollisionMesh();

  VISION_APIFUNC virtual VBaseSubmesh **GetTriangleSubmeshPtr(bool bForceCreate = false) HKV_OVERRIDE;
  VISION_APIFUNC virtual unsigned char *GetTriangleFlagsPtr(bool bForceCreate = false) HKV_OVERRIDE;
  VISION_APIFUNC virtual hkvVec2* GetVertexBaseUV(bool bForceCreate = false) HKV_OVERRIDE;
  VISION_APIFUNC virtual hkvPlane* GetTrianglePlanesPtr(char **ppLeastSig, bool bForceCreate = false) HKV_OVERRIDE;
  VISION_APIFUNC virtual hkvAlignedBBox* GetTriangleBoxesPtr(bool bForceCreate = false) HKV_OVERRIDE;
  VISION_APIFUNC virtual BOOL Reload();

  void VISION_APIFUNC Update();

  // Internal:
  inline void FlagDependentDataAsOutdated() 
  { 
    m_bTrianglePlanesNeedUpdate = true; 
    m_bTriangleBoxesNeedUpdate = true; 
    m_iLastUpdated = Vision::Game.GetUpdateSceneCount();
  }


protected:
  VRigidCollisionMeshPtr m_spStaticCollisionMesh;
  VisAnimConfig_cl *m_pAnimConfig;
  int m_iLastUpdated;

  bool m_bTrianglePlanesNeedUpdate;
  bool m_bTriangleBoxesNeedUpdate;

};






#endif //VISAPICOLLISIONMESHENTITY_HPP_INCLUDED

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
