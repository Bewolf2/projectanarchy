/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file SimpleCopyPostprocess.hpp

#ifndef SIMPLECOPYPOSTPROCESS_HPP_INCLUDED
#define SIMPLECOPYPOSTPROCESS_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Postprocessing/PostProcessBase.hpp>

/// \brief
///   Simple copy post-processor.
///
/// This post-processor copies the renderer node's color buffer into its target context.
/// 
class VSimpleCopyPostprocess : public VPostProcessingBaseComponent
{
public:

  /// \brief
  ///   Constructor of the Simple copy post-processor class.
  EFFECTS_IMPEXP VSimpleCopyPostprocess();

  /// \brief
  ///   Destructor of the Simple copy post-processor class.
  EFFECTS_IMPEXP virtual ~VSimpleCopyPostprocess();

  /// \brief
  ///   Internal function to set up the required target render context
  EFFECTS_IMPEXP void SetupContext();

  /// \brief
  ///   Internal function to tear down the required target render context
  EFFECTS_IMPEXP void RemoveContext();

#ifndef _VISION_DOC
  V_DECLARE_SERIAL_DLLEXP( VSimpleCopyPostprocess,  EFFECTS_IMPEXP );
  V_DECLARE_VARTABLE(VSimpleCopyPostprocess, EFFECTS_IMPEXP);
  EFFECTS_IMPEXP virtual void Serialize( VArchive &ar ) HKV_OVERRIDE;
#endif

protected:
  EFFECTS_IMPEXP virtual void InitializePostProcessor() HKV_OVERRIDE;
  EFFECTS_IMPEXP virtual void DeInitializePostProcessor() HKV_OVERRIDE;
  EFFECTS_IMPEXP virtual void Execute() HKV_OVERRIDE;

  int m_iWidth, m_iHeight;
};



#endif

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
