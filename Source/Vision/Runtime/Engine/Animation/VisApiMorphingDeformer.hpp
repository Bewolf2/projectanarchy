/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiMorphingDeformer.hpp

#ifndef VIS_MORPHING_DEFORMER_HPP_INCLUDED
#define VIS_MORPHING_DEFORMER_HPP_INCLUDED

#include <Vision/Runtime/Engine/Animation/VisApiVertexDeformer.hpp>
#include <Vision/Runtime/Engine/Animation/VisApiVertexAnimControl.hpp>
#include <Vision/Runtime/Engine/Animation/VisApiVertexAnimResult.hpp>

// predefines
class VisMorphingDeformer_cl;
typedef VSmartPtr<VisAnimConfig_cl>  VisAnimConfigPtr;

/// \brief
///   An instance of this class applies different weighted morphing targets
/// 
///  It implements the IVisVertexDeformer_cl interface.
/// 
/// The mesh deformer class holds a list of vertex animation targets that are updated and applied
/// to the mesh. The animation data consist of vertex deltas, so several deltas add up to an even
/// stronger vertex movement.
class VisMorphingDeformer_cl: public IVisVertexDeformer_cl
{
public:

  /// \brief
  ///   Constructor of the morphing deformer class
  /// 
  /// \param pMeshDeformerStack
  ///   deformer stack this deformer will be added to
  VISION_APIFUNC VisMorphingDeformer_cl(VisVertexDeformerStack_cl* pMeshDeformerStack);


  /// \brief
  ///   Destructor
  VISION_APIFUNC ~VisMorphingDeformer_cl();


  /// \brief
  ///   Returns the morph target with specified index.
  /// 
  /// \param iIndex
  ///   morph target index
  /// 
  /// \return
  ///   VisVertexAnimSequence_cl* pSequence: pointer to morph target
  VISION_APIFUNC VisVertexAnimSequence_cl* GetMorphTarget(int iIndex);


  /// \brief
  ///   Returns weight that is applied to morph target before it is added to the meshs vertices.
  /// 
  /// \param iIndex
  ///   index of the morph target
  /// 
  /// \return
  ///   float fWeight: morph target weight
  VISION_APIFUNC float GetMorphTargetWeight(int iIndex);


  /// \brief
  ///   Sets weight for a morph target.
  /// 
  /// \param iIndex
  ///   morph target index
  /// 
  /// \param fWeight
  ///   morph target weighting
  /// 
  /// \return
  ///   Nothing.
  VISION_APIFUNC void SetMorphTargetWeight(int iIndex, float fWeight);


  /// \brief
  ///   Replaces a morph target at specified index.
  /// 
  /// \param iIndex
  ///   morph target index
  /// 
  /// \param pVertexAnimSequence
  ///   pointer to morph target
  /// 
  /// \param fMorphTargetWeight
  ///   morph target weight
  VISION_APIFUNC void SetMorphTarget(int iIndex, VisVertexAnimSequence_cl* pVertexAnimSequence, float fMorphTargetWeight = 1.f);


  /// \brief
  ///   Adds a morph target to the list.
  /// 
  /// \param pMorphTarget
  ///   pointer to morph target
  /// 
  /// \param fMorphTargetWeight
  ///   morph target weight
  /// 
  /// \return
  ///   int iIndex: index of added morph target
  VISION_APIFUNC int AddMorphTarget(VisVertexAnimSequence_cl* pMorphTarget, float fMorphTargetWeight = 1.f);


  /// \brief
  ///   Removes a morph target from the list.
  /// 
  /// \param iIndex
  ///   index of morph target to be removed
  /// 
  /// \return
  ///   bool bResult: true if successful
  VISION_APIFUNC bool RemoveMorphTarget(int iIndex);


  /// \brief
  ///   Overridden function to update the state with all morphing frames.
  /// 
  /// \param fTimeDelta
  ///   time since last state update
  /// 
  /// \param m_LocalAnimState
  ///   state that gets updated
  /// 
  /// \return
  ///   bool bResult: True if successful
  VISION_APIFUNC bool UpdateDeformerState(float fTimeDelta, VisAnimState_cl* m_LocalAnimState);


  /// \brief
  ///   Overridden function to update the result with all morphing frame deltas.
  /// 
  /// \param pVertexAnimResult
  ///   includes vertices to modify
  /// 
  /// \return
  ///   bool bResult: True if successful
  VISION_APIFUNC bool UpdateDeformerResult(VisVertexAnimResult_cl* pVertexAnimResult);


  /// \brief
  ///   Returns the number of morph target that have been added
  /// 
  /// \return
  ///   int iCount: morph target count
  inline int GetMorphTargetCount() const
  {
    return m_iMorphTargetCount;
  }

protected:
  // serialization
  V_DECLARE_SERIAL_DLLEXP( VisMorphingDeformer_cl, VISION_APIDATA );
  VISION_APIFUNC virtual void Serialize( VArchive &ar );
  VISION_APIFUNC VisMorphingDeformer_cl();
  VISION_APIFUNC virtual void Debug_ShowHierarchy(IVRenderInterface *pRI, int iGeneration, float &x, float &y);

private:

  // list with pointers on morph targets that are all applied to the vertex result
  DynArray_cl<float>m_fMorphTargetWeights;                 ///< sorted list of morph target weights
  DynArray_cl<VisVertexAnimSequence_cl*>m_pMorphTargets;   ///< sorted list of morph targets
  int m_iMorphTargetCount;
  int m_iAppliedVertexDeltas[256]; ///< temporary member. Does not get serialized.
  VisVertexDeltaKeyFrame_cl *m_pVertexDeltaKeyFrames[256]; ///< Does not get serialized because data is copied from m_pMorphTargets

  /// \brief
  ///   Used to apply all morph targets according to their weight on the vertex result.
  /// 
  /// \param pVertexAnimResult
  ///   result to apply the morphing modifier to.
  /// 
  /// \return
  ///   Nothing.
  void ApplyMorphTargets(VisVertexAnimResult_cl* pVertexAnimResult);
};

#endif //VIS_MORPHING_DEFORMER_HPP_INCLUDED

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
