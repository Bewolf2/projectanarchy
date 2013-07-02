/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiOcclusionQueryObjectPixelCounter.hpp

#ifndef _VISAPIOCCLUSIONOBJECTPIXELCOUNTER_HPP_INCLUDED_
#define _VISAPIOCCLUSIONOBJECTPIXELCOUNTER_HPP_INCLUDED_

#include <Vision/Runtime/Base/Math/BoundingVolume/hkvAlignedBBox.h>
#include <Vision/Runtime/Engine/Renderer/OcclusionQuery/VisApiOcclusionQuery.hpp>

class VOcclusionQueryObjectPixelCounter;
class VLensFlareComponent;

/// \brief
///   Abstract base class for handling occlusion queries across different scene element types.
/// 
/// This class takes care of occlusion query rendering. It does not perform any bookkeeping for
/// query results; results and status information are instead stored in the VisRenderContext_cl
/// class.
/// 
/// The virtual Render function can be implemented differently depending on the scene element type.
class VOcclusionQueryObjectPixelCounter : public VisElementManager_cl<VOcclusionQueryObjectPixelCounter *>
{
public:

  /// \brief
  ///   VOcclusionQueryObjectPixelCounter constructor
  VISION_APIFUNC VOcclusionQueryObjectPixelCounter();

  /// \brief
  ///   VOcclusionQueryObjectPixelCounter constructor
  VISION_APIFUNC virtual ~VOcclusionQueryObjectPixelCounter();

  /// \brief
  ///   Virtual Render function of the occlusion query object. 
  /// 
  /// Implemented differently depending on the scene element type. 
  /// 
  /// This function is called from the render context inside
  /// VisRenderContect_cl::RenderScheduledOcclusionQueries calls.
  /// 
  /// \param query
  ///   The VOcclusionQuery object handled by the VisRenderContext_cl to be used in this query.
  ///
  /// \param safeBox
  ///   A box around the camera inside which occlusion queries won't be rendered.
  /// 
  /// \return
  ///   A return value of false indicates that the occlusion query has
  ///   not been executed because the object overlaps the passed safeBox.
  ///   On true, the rendering did succeed.
  VISION_APIFUNC virtual bool Render(VOcclusionQuery &query, const hkvAlignedBBox &safeBox) = 0;

  /// \brief
  ///   Turns off occlusion query rendering. Has to be called when normal rendering is desired
  ///   after rendering occlusion queries.
  VISION_APIFUNC static void ResetState();

  /// \brief
  ///   Sets the render state required for performing occlusion queries. Currently only called
  ///   internally.
  VISION_APIFUNC static void SetState(VisQueryRenderState_e eState);

  /// \brief
  ///   Specifies whether the CPU should wait for the GPU to finish the query.
  /// 
  /// If set to true, the CPU will wait for the GPU to finish the query.  This may hurt CPU/GPU
  /// parallelism and waste CPU time, but guarantees correct results.
  /// 
  /// If set to false, the CPU won't wait for the GPU to finish the query, and simply use the previous
  /// query results until the new results becomes available.
  /// 
  /// The default value is true. However, the behavior for newly created occlusion query objects
  /// can be changed through the static function SetWaitForFinishDefault. Note that this will not
  /// affect already existing objects, only newly created ones!
  /// 
  /// \param bWait
  ///   true to enable waiting, false to disable it (default).
  inline void SetWaitForFinish(bool bWait) { m_bWait = bWait; }

  /// \brief
  ///   Returns the value previously set with SetWaitForFinish.
  inline bool GetWaitForFinish() const { return m_bWait; }


  /// \brief
  ///   Sets the default value for CPU-side waiting for all newly created objects (see
  ///   Set/GetWaitForFinish).
  inline static void SetWaitForFinishDefault(bool bWaitDefault) { s_bWaitDefault = bWaitDefault; }

  /// \brief
  ///   Returns the default value for CPU-side waiting for all newly created objects (see
  ///   Set/GetWaitForFinish).
  inline static bool GetWaitForFinishDefault() { return s_bWaitDefault; }

protected:
  bool m_bWait;

  static bool s_bWaitDefault;
};

VISION_ELEMENTMANAGER_TEMPLATE_DECL(VOcclusionQueryObjectPixelCounter)

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
