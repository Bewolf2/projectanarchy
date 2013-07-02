/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiVertexDeformerStack.hpp

#ifndef VIS_MESH_DEFORMER_STACK_HPP_INCLUDED
#define VIS_MESH_DEFORMER_STACK_HPP_INCLUDED

#include <Vision/Runtime/Engine/Animation/VisApiVertexDeformer.hpp>
#include <Vision/Runtime/Engine/Animation/VisApiSkeletalAnimResult.hpp>
#include <Vision/Runtime/Engine/Mesh/VisApiModel.hpp>

// predefines
class VisVertexDeformerStack_cl;
typedef VSmartPtr<VisVertexDeformerStack_cl>  VisVertexDeformerStackPtr;


/// \brief
///   The Vertex deformer stack class, holds a list with all mesh deformers that are applied to
///   the vertices.
/// 
/// Note that the modifiers are applied in same order as they were added. Because of
/// that all deformers have consequences on the following deformers and a different order creates
/// also different results. For example skinning modifiers should be added after vertex animations
/// for single animated body parts!
class VisVertexDeformerStack_cl : public VRefCounter, public VisTypedEngineObject_cl
{
public:

  /// \brief
  ///   Constructor
  VISION_APIFUNC VisVertexDeformerStack_cl();


  /// \brief
  ///   Destructor
  VISION_APIFUNC ~VisVertexDeformerStack_cl();


  /// \brief
  ///   GetLastUpdateSceneCount returns the last frame the state was updated.
  /// 
  /// Compare this value against VisGame_cl::GetUpdateSceneCount.
  /// 
  /// \return
  ///   unsigned int iFrame: frame number
  unsigned int GetLastUpdateSceneCount() const
  {
    return m_fLastFrameUpdatedVertexState;
  }


  /// \brief
  ///   Returns the number of deformers on the stack.
  /// 
  /// \return
  ///   int iCount: mesh deformer count
  inline int GetDeformerCount() const
  {
    return m_MeshDeformerList.GetLength();
  }


  /// \brief
  ///   Returns a pointer to the deformer with specified index.
  /// 
  /// \param iIndex
  ///   index of deformer. Must be in valid range [0..GetDeformerCount()-1]
  /// 
  /// \return
  ///   IVisVertexDeformer_cl*: pointer to mesh deformer
  inline IVisVertexDeformer_cl* GetDeformer(int iIndex)
  {
    VASSERT((iIndex>=0)&&(iIndex<m_MeshDeformerList.GetLength()));
    VASSERT(m_MeshDeformerList[iIndex]);
    return (IVisVertexDeformer_cl*)m_MeshDeformerList[iIndex];
  }


  /// \brief
  ///   Adds a deformer to the stack.
  /// 
  /// \param pMeshDeformer
  ///   deformer to be added
  VISION_APIFUNC void AddDeformer(IVisVertexDeformer_cl* pMeshDeformer);


  /// \brief
  ///   Replaces a deformer in the list with a new one.
  /// 
  /// The old one gets deleted.
  /// 
  /// \param iInputNumber
  ///   index of deformer
  /// 
  /// \param pMeshDeformer
  ///   pointer to new deformer
  /// 
  /// \return
  ///   bool bResult: true if successful
  VISION_APIFUNC bool SetDeformer(int iInputNumber, IVisVertexDeformer_cl* pMeshDeformer);


  /// \brief
  ///   Removes deformer with specified index.
  /// 
  /// \param iInputNumber
  ///   index of deformer
  /// 
  /// \return
  ///   bool bResult: true if successful
  VISION_APIFUNC bool RemoveDeformer(int iInputNumber);


  /// \brief
  ///   Updates the vertex state of the deformer stack.
  /// 
  /// It is only called once per simulation frame by the config and should never be called
  /// manually. The call updates the state with all deformers on stack.
  /// 
  /// \param fTimeDelta
  ///   passed time since last state update
  /// 
  /// \return
  ///   VisAnimState_cl* pState: updated state.
  const VisAnimState_cl* UpdateAnimState(float fTimeDelta);


  /// \brief
  ///   Returns the current vertex anim result.
  /// 
  /// The function updates the result with all deformers on stack.
  /// 
  /// \param pVertexAnimResult
  ///   result to be updated.
  /// 
  /// \return
  ///   nothing.
  VISION_APIFUNC void UpdateAnimResult(VisVertexAnimResult_cl* pVertexAnimResult);


  /// \brief
  ///   Sets a reference to the skinning result to use.
  /// 
  /// It gets called by the anim config when final result has been changed.
  /// 
  /// \param pSkinningResult
  ///   references to skinning result.
  /// 
  /// \return
  ///   Nothing.
  inline void SetSkinningResult(const VisSkeletalAnimResult_cl* pSkinningResult)
  {
    m_pSkinningResult = pSkinningResult;
  }

    
  /// \brief
  ///   Returns the pointer to the used skinning result.
  /// 
  /// \return
  ///   VisSkeletalAnimResult_cl* pSkinningResult: pointer to skinning result.
  inline const VisSkeletalAnimResult_cl* GetSkinningResult() const
  {
    return m_pSkinningResult;
  }

  inline bool HasVertexAnimations() { return m_bContainsVertexAnimations; }

protected:

  void CheckForVertexAnimations();


  unsigned int m_fLastFrameUpdatedVertexState;// last frame the state was updated
  VPList m_MeshDeformerList;                  ///< sorted list of mesh deformers, applied to mesh in same order
  VisAnimState_cl m_LocalAnimState;           ///< used to keep AnimState

  const VisSkeletalAnimResult_cl* m_pSkinningResult;    ///< bone result used for skinning

  bool m_bContainsVertexAnimations;

  // serialization
  V_DECLARE_SERIAL_DLLEXP( VisVertexDeformerStack_cl, VISION_APIDATA )
  VISION_APIFUNC virtual void Serialize( VArchive &ar );
};

#endif //VIS_MESH_DEFORMER_STACK_HPP_INCLUDED

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
