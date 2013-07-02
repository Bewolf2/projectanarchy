/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiAnimResult.hpp

#ifndef VIS_ANIM_RESULT_HPP_INCLUDED
#define VIS_ANIM_RESULT_HPP_INCLUDED

// predefines
class VisAnimResult_cl;
typedef VSmartPtr<VisAnimResult_cl>  VisAnimResultPtr;

/// \brief
///   Base animation result class with common attributes and methods for animation results
/// 
/// Every type of animation also generates a distinct animation result.
/// 
/// This anim result class is a base class which includes results that are needed for all animation
/// result types.
class VisAnimResult_cl : public VRefCounter, public VisTypedEngineObject_cl
{
public:

  /// \brief
  ///   Constructor of the anim result class.
  VISION_APIFUNC VisAnimResult_cl();


  /// \brief
  ///   Destructor
  VISION_APIFUNC ~VisAnimResult_cl();

public:
    
  /// \brief
  ///   Fills result with data from another result.
  /// 
  /// \param pSrc
  ///   Source result to copy from
  VISION_APIFUNC void CopyFrom(const VisAnimResult_cl *pSrc);


  /// \brief
  ///   Weights another result and adds it to this result.
  /// 
  /// \param pSrcResult
  ///   Source result to add
  /// 
  /// \param fAnimWeight
  ///   weight to apply on source result before adding
  /// 
  /// \return
  ///   bool bResult: true if everything worked
  VISION_APIFUNC virtual bool AddAnimResult(const VisAnimResult_cl* pSrcResult, float fAnimWeight);

  /// \brief
  ///   Overridable that layers results together.
  /// 
  /// The specified result is layered under the current result.
  /// 
  /// \param pResult
  ///   Other result to layer
  /// 
  /// \param fAnimWeight
  ///   weighting factor for the overlay
  /// 
  /// \param fLayerWeight
  ///   weighting factor for weighting accumulation
  /// 
  /// \return
  ///   bool bResult: true if successful
  VISION_APIFUNC virtual bool AddSubLayerAnimResult(const VisAnimResult_cl* pResult, float fAnimWeight, float fLayerWeight);


  /// \brief
  ///   Clears all data structures for new additions.
  /// 
  /// \return
  ///   Nothing.
  VISION_APIFUNC virtual void ClearResult();

    
  /// \brief
  ///   Overridable function to compare two results.
  /// 
  /// \param pResult
  ///   result to compare this with.
  /// 
  /// \return
  ///   bool bResult: true if this result matches the other one
  VISION_APIFUNC bool EqualsResult(const VisAnimResult_cl* pResult) const;


  /// \brief
  ///   Normalizes the result after several results have been added.
  /// 
  /// \return
  ///   bool bResult: true if normalization was successful.
  VISION_APIFUNC bool NormalizeResult();


  /// \brief
  ///   HasResults indicates whether there is a valid result.
  /// 
  /// \return
  ///   bool bResult: true if there is a valid result.
  inline bool HasResults(void) const 
  {
    return m_bHasResults;
  }

  // serialization
  V_DECLARE_SERIAL_DLLEXP( VisAnimResult_cl, VISION_APIDATA );
  VISION_APIFUNC virtual void Serialize( VArchive &ar );
  VISION_APIFUNC virtual void Debug_ShowHierarchy(IVRenderInterface *pRI, int iGeneration, float &x, float &y);

protected:
  bool m_bHasResults;   ///< has any result been added or multiplied
};

#endif //VIS_ANIM_RESULT_HPP_INCLUDED

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
