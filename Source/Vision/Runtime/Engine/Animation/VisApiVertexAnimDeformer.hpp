/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiVertexAnimDeformer.hpp

#ifndef VIS_VERTEX_ANIM_DEFORMER_HPP_INCLUDED
#define VIS_VERTEX_ANIM_DEFORMER_HPP_INCLUDED

#include <Vision/Runtime/Engine/Animation/VisApiVertexDeformer.hpp>
#include <Vision/Runtime/Engine/Animation/VisApiVertexAnimControl.hpp>
#include <Vision/Runtime/Engine/Animation/VisApiVertexAnimResult.hpp>

/// \brief
///   A mesh deformer class that applies different vertex animations. Implements
///   IVisVertexDeformer_cl.
/// 
/// The mesh deformer class holds a list of vertex animations that are updated and applied to the
/// mesh. The animation data consist of vertex deltas, so several positive deltas add up to an even
/// stronger vertex movement.
class VisVertexAnimDeformer_cl: public IVisVertexDeformer_cl
{
public:

  /// \brief
  ///   Constructor
  VISION_APIFUNC VisVertexAnimDeformer_cl(VisVertexDeformerStack_cl* pMeshDeformerStack);


  /// \brief
  ///   Destructor
  VISION_APIFUNC ~VisVertexAnimDeformer_cl();


  /// \brief
  ///   Returns the vertex animation control with specified index.
  /// 
  /// \param iIndex
  ///   control's index, must be in valid range [0..GetVertexAnimControlCount()-1]
  /// 
  /// \return
  ///   VisVertexAnimControl_cl* pControl: pointer to vertex animation control
  inline VisVertexAnimControl_cl* GetVertexAnimControl(int iIndex) const
  {
    VASSERT((iIndex>=0)&&(iIndex<m_iVertexAnimControlCount));
    VASSERT(m_pVertexAnimControl.GetDataPtr()[iIndex]!=NULL);

    return m_pVertexAnimControl.GetDataPtr()[iIndex];
  }


  /// \brief
  ///   Returns weight for vertex animation with specified index.
  /// 
  /// The weight is applied to vertex animation result before it is added to the mesh's vertices.
  /// 
  /// \param iIndex
  ///   control's index, must be in valid range [0..GetVertexAnimControlCount()-1]
  /// 
  /// \return
  ///   float fWeight: animation weight
  inline float GetVertexAnimControlWeight(int iIndex) const
  {
    VASSERT((iIndex>=0.f)&&(iIndex<m_iVertexAnimControlCount));
    return m_fControlWeight.GetDataPtr()[iIndex];
  }


  /// \brief
  ///   Sets a vertex animation control at specified index.
  /// 
  /// The current control at this index is replaced.
  /// 
  /// \param iIndex
  ///   index of the control
  /// 
  /// \param pVertexAnimControl
  ///   pointer to vertex animation
  /// 
  /// \param fControlWeight
  ///   animation weight
  VISION_APIFUNC void SetVertexAnimControl(int iIndex, VisVertexAnimControl_cl* pVertexAnimControl, float fControlWeight = 1.f);


  /// \brief
  ///   Returns the number of vertex animation controls.
  /// 
  /// \return
  ///   int iCount: number of vertex animation controls
  inline int GetVertexAnimControlCount() const
  {
    return m_iVertexAnimControlCount;
  }


  /// \brief
  ///   Adds a vertex animation control to the list.
  /// 
  /// \param pVertexAnimControl
  ///   pointer to vertex animation
  /// 
  /// \param fControlWeight
  ///   animation weight
  /// 
  /// \return
  ///   int iIndex: index of added animation
  VISION_APIFUNC int AddVertexAnimControl(VisVertexAnimControl_cl* pVertexAnimControl, float fControlWeight = 1.f);


  /// \brief
  ///   Removes a vertex animation control from list.
  /// 
  /// \param iIndex
  ///   valid index of vertex animation to be removed
  /// 
  /// \return
  ///   bool bResult: true if removing was successful
  VISION_APIFUNC bool RemoveVertexAnimControl(int iIndex);


  /// \brief
  ///   Overridden UpdateDeformerState function that updates the state with all vertex animations.
  /// 
  /// \param fTimeDelta
  ///   time since last state update
  /// 
  /// \param m_LocalAnimState
  ///   state that gets updated
  /// 
  /// \return
  ///   bool bResult : true if successful
  VISION_APIFUNC virtual bool UpdateDeformerState(float fTimeDelta, VisAnimState_cl* m_LocalAnimState);


  /// \brief
  ///   Overridden UpdateDeformerResult function that updates the result with all vertex animation
  ///   deltas.
  /// 
  /// \param pVertexAnimResult
  ///   includes vertices to modify
  /// 
  /// \return
  ///   bool bResult : true if successful
  VISION_APIFUNC virtual bool UpdateDeformerResult(VisVertexAnimResult_cl* pVertexAnimResult);

protected:

  // serialization
  V_DECLARE_SERIAL_DLLEXP( VisVertexAnimDeformer_cl, VISION_APIDATA )
  VISION_APIFUNC virtual void Serialize( VArchive &ar );
  VISION_APIFUNC VisVertexAnimDeformer_cl();
  VISION_APIFUNC virtual void Debug_ShowHierarchy(IVRenderInterface *pRI, int iGeneration, float &x, float &y);

private:

  // list with animation controls that are all applied to the mesh
  DynArray_cl<float>m_fControlWeight;                           ///< sorted list of vertex control weights
  DynArray_cl<VisVertexAnimControl_cl*>m_pVertexAnimControl;    ///< sorted list of vertex controls
  int m_iVertexAnimControlCount;
  VBitfield m_AnimatedBitField;   ///< keeps a bit for every vertex that could be animated
  int m_iMinBitFieldIndex;
  
  int m_iAppliedFloorVertexDeltas[256];   ///< keeps the numbers of deltas that already where applied
  int m_iAppliedCeilingVertexDeltas[256]; ///< at the moment there can't be more than 256 vertexanimations on one character playing parallel
  
  /// \brief
  ///   Gets called by the engine to update the animation results with the sequences.
  /// 
  /// \param pVertexAnimResult
  ///   result that will be modified.
  /// 
  /// \return
  ///   Nothing.
  void ApplyVertexAnimations(VisVertexAnimResult_cl* pVertexAnimResult);


  /// \brief
  ///   UpdateAnimBitField gets called by the engine when animations are added or removed from the
  ///   list.
  /// 
  /// The combination of all vertex animation sequence bitfields is used for optimizing the vertex
  /// animations.
  /// 
  /// \return
  ///   Nothing.
  void UpdateAnimBitField(void);
};

#endif //VIS_VERTEX_ANIM_DEFORMER_HPP_INCLUDED

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
