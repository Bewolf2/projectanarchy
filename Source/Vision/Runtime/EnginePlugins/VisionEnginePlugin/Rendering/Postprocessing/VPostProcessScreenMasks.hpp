/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VPostProcessScreenMasks.hpp

#ifndef VPOST_PROCESS_SCREEN_MASKS_HPP_INCLUDED
#define VPOST_PROCESS_SCREEN_MASKS_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Postprocessing/PostProcessBase.hpp>

/// \brief
///   Post processor that renders screen masks, triggers GUI render hooks and handles debug text and geometry.
class VPostProcessScreenMasks : public VPostProcessingBaseComponent
{
public:

  /// \brief
  ///   Constructor of the screen masks postprocessor class.
  EFFECTS_IMPEXP VPostProcessScreenMasks();

  /// \brief
  ///   Destructor of the screen masks postprocessor class.
  EFFECTS_IMPEXP ~VPostProcessScreenMasks();

  /// \brief
  ///   Internal function to set up the required target render context
  EFFECTS_IMPEXP void SetupContext();

  /// \brief
  ///   Internal function to tear down the required target render context
  EFFECTS_IMPEXP void RemoveContext();

  /// \brief
  ///   Specifies whether 2d geometry should be rendered by the postprocessor
  inline void SetRender2dElements(bool bStatus) { m_bRender2D = bStatus; };

  /// \brief
  ///   Returns whether 2d geometry is rendered by the postprocessor
  inline bool GetRender2dElements(bool bStatus) const { return m_bRender2D; };

  /// \brief
  ///   Specifies whether 2d geometry should be rendered by the postprocessor
  inline void SetRender3dElements(bool bStatus) { m_bRender3D = bStatus; };

  /// \brief
  ///   Returns whether 2d geometry is rendered by the postprocessor
  inline bool GetRender3dElements(bool bStatus) const { return m_bRender3D; };

#ifndef _VISION_DOC
  V_DECLARE_SERIAL_DLLEXP( VPostProcessScreenMasks,  EFFECTS_IMPEXP );
  V_DECLARE_VARTABLE(VPostProcessScreenMasks, EFFECTS_IMPEXP);
  EFFECTS_IMPEXP virtual void Serialize( VArchive &ar ) HKV_OVERRIDE;
#endif

protected:
  EFFECTS_IMPEXP virtual void InitializePostProcessor() HKV_OVERRIDE;
  EFFECTS_IMPEXP virtual void DeInitializePostProcessor() HKV_OVERRIDE;
  EFFECTS_IMPEXP virtual void Execute() HKV_OVERRIDE;

  bool m_bRender2D;
  bool m_bRender3D;
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
