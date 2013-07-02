/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiSkinningDeformer.hpp

#ifndef VIS_SKINNING_DEFORMER_HPP_INCLUDED
#define VIS_SKINNING_DEFORMER_HPP_INCLUDED

#include <Vision/Runtime/Engine/Animation/VisApiVertexDeformer.hpp>

/// \brief
///   Skinning deformer class, performs the skinning with the internal skinning matrices
/// 
///  The skinning result from the animation tree has to be set on this component every frame.
/// 
/// Implements the IVisVertexDeformer_cl interface.
class VisSkinningDeformer_cl: public IVisVertexDeformer_cl
{
public:


  /// \brief
  ///   Constructor
  VISION_APIFUNC VisSkinningDeformer_cl(VisVertexDeformerStack_cl* pMeshDeformerStack);


  /// \brief
  ///   Destructor
  VISION_APIFUNC ~VisSkinningDeformer_cl();


  /// \brief
  ///   Overidden UpdateDeformerState function to update the state of the skinning deformer.
  /// 
  /// \param fTimeDelta
  ///   time since last update.
  /// 
  /// \param m_LocalAnimState
  ///   state that gets updated.
  /// 
  /// \return
  ///   bool bResult : true if successful.
  VISION_APIFUNC virtual bool UpdateDeformerState(float fTimeDelta, VisAnimState_cl* m_LocalAnimState);


  /// \brief
  ///   Overidden UpdateDeformerResult function to apply skinning on the vertices.
  /// 
  /// \param pVertexAnimResult
  ///   vertex result the skinning is applied to
  VISION_APIFUNC virtual bool UpdateDeformerResult(VisVertexAnimResult_cl* pVertexAnimResult);


protected:

  // serialization
  V_DECLARE_SERIAL_DLLEXP( VisSkinningDeformer_cl, VISION_APIDATA )
  VISION_APIFUNC virtual void Serialize( VArchive &ar );
  VISION_APIFUNC VisSkinningDeformer_cl();
  VISION_APIFUNC virtual void Debug_ShowHierarchy(IVRenderInterface *pRI, int iGeneration, float &x, float &y);

};

#endif //VIS_SKINNING_DEFORMER_HPP_INCLUDED

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
