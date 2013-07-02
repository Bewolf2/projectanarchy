/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiVertexDeformer.hpp

#ifndef VIS_VERTEX_DEFORMER_HPP_INCLUDED
#define VIS_VERTEX_DEFORMER_HPP_INCLUDED

#include <Vision/Runtime/Engine/Animation/VisApiVertexAnimResult.hpp>

class VisVertexDeformerStack_cl;

/// \brief
///   The vertex deformer class is a parent class of all classes added to the vertex deformer stack
/// 
/// Every class that you want to add to the deformer stack to modify the models vertices has to be
/// derived from this class.
/// 
/// You have to implement UpdateDeformerState and UpdateDeformerResult in your subclass.
/// 
/// There you can apply every modification to the vertices you want. For example perform vertex
/// animation, facial animation or do a custom skinning.
class IVisVertexDeformer_cl : public VBaseObject, public VisTypedEngineObject_cl
{
public:

  /// \brief
  ///   Constructor
  /// 
  /// \param pVertexDeformerStack
  ///   pointer to the stack the deformer gets added to.
  VISION_APIFUNC IVisVertexDeformer_cl(VisVertexDeformerStack_cl* pVertexDeformerStack);


  /// \brief
  ///   Destructor
  VISION_APIFUNC virtual ~IVisVertexDeformer_cl();


  /// \brief
  ///   Pure virtual function that gets called by the animation system to update the state of this
  ///   deformer.
  /// 
  /// \param fTimeDelta
  ///   time since last update
  /// 
  /// \param pAnimState
  ///   current state
  /// 
  /// \return
  ///   bool bResult: True if successful
  VISION_APIFUNC virtual bool UpdateDeformerState(float fTimeDelta, VisAnimState_cl* pAnimState) = 0;


  /// \brief
  ///   Pure virtual function that gets called by the animation system to update the state of this
  ///   deformer.
  /// 
  /// \param pVertexAnimResult
  ///   result that keeps pointers to modified vertices
  /// 
  /// \return
  ///   bool bResult: True if successful
  VISION_APIFUNC virtual bool UpdateDeformerResult(VisVertexAnimResult_cl* pVertexAnimResult) = 0;


  /// \brief
  ///   Empty constructor for serialization. Do not use
  VISION_APIFUNC IVisVertexDeformer_cl();

  // only for internal use
  VISION_APIFUNC virtual void Debug_ShowHierarchy(IVRenderInterface *pRI, int iGeneration, float &x, float &y);

  VisVertexDeformerStack_cl* m_pVertexDeformerStack;  ///< pointer to stack this deformer has been add to
};

#endif //VIS_VERTEX_DEFORMER_HPP_INCLUDED

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
