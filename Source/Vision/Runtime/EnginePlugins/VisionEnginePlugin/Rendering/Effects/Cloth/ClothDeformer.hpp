/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file ClothDeformer.hpp

#ifndef VIS_CLOTH_DEFORMER_HPP_INCLUDED
#define VIS_CLOTH_DEFORMER_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/Cloth/ClothMesh.hpp>

class VisVertexDeformerStack_cl;
extern VModule g_VisionEngineModule;

/// \brief
///   Helper class to incorporate a cloth mesh into the animation system. Used internally by the cloth simulation entity.
class VisClothDeformer_cl: public IVisVertexDeformer_cl, public VRefCounter
{
public:

  /// \brief
  ///   Constructor of the cloth deformer class
  VisClothDeformer_cl(VisVertexDeformerStack_cl* pMeshDeformerStack, VClothMesh *pMesh, VisObject3D_cl *pParentObject);

  /// \brief
  ///    Overridden function to update the state with current cloth mesh.
  bool UpdateDeformerState(float fTimeDelta, VisAnimState_cl* m_LocalAnimState);


  /// \brief
  ///    Overridden function to update the result with current cloth mesh.
  bool UpdateDeformerResult(VisVertexAnimResult_cl* pVertexAnimResult);

protected:
  // serialization
  VisClothDeformer_cl() {}
  V_DECLARE_SERIAL_DLLEXP( VisClothDeformer_cl, EFFECTS_IMPEXP );
  EFFECTS_IMPEXP virtual void Serialize( VArchive &ar ) HKV_OVERRIDE;

private:

  VClothMeshPtr m_spMesh;
  VisObject3D_cl *m_pParentObject;
};

#endif //VIS_CLOTH_DEFORMER_HPP_INCLUDED

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
