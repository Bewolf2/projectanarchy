/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiOcclusionQueryObject.hpp

#ifndef _VISAPIOCCLUSIONOBJECT_HPP_INCLUDED_
#define _VISAPIOCCLUSIONOBJECT_HPP_INCLUDED_

#include <Vision/Runtime/Base/Math/BoundingVolume/hkvAlignedBBox.h>
#include <Vision/Runtime/Engine/Renderer/OcclusionQuery/VisApiOcclusionQuery.hpp>

class VisOcclusionQueryObject_cl;
class VisVisibilityZone_cl;
class VisVisibilityObject_cl;
class VisBaseEntity_cl;
class VisPortal_cl;

#ifndef _VISION_DOC

#define VISQUERY_RENDER_PRIORITY_ENTITY     0
#define VISQUERY_RENDER_PRIORITY_VISOBJECT  10000000
#define VISQUERY_RENDER_PRIORITY_VISZONE    11000000
#define VISQUERY_RENDER_PRIORITY_PORTAL     12000000

#endif


/// \brief
///   Abstract base class for handling occlusion queries across different scene element types.
/// 
/// This class takes care of occlusion query rendering. It does not perform any bookkeeping for
/// query results; results and status information are instead stored in the VisRenderContext_cl
/// class from which the queries were executed.
/// 
/// The virtual Render function can be implemented differently depending on the scene element type.
class VisOcclusionQueryObject_cl : public VisElementManager_cl<VisOcclusionQueryObject_cl *>
{
public:

  /// \brief
  ///   VisOcclusionQueryObject_cl constructor
  VISION_APIFUNC VisOcclusionQueryObject_cl();

  /// \brief
  ///   VisOcclusionQueryObject_cl constructor
  VISION_APIFUNC virtual ~VisOcclusionQueryObject_cl();

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
  ///   Sets the minimum number of pixels that have to be visible in order for this object to pass
  ///   the occlusion test. The default value is 5.
  inline void SetPixelThreshold(unsigned int iThreshold) { m_iPixelThreshold = iThreshold; }

  /// \brief
  ///   Returns the pixel threshold as previously set with SetPixelThreshold.
  inline unsigned int GetPixelThreshold() const { return m_iPixelThreshold; }

  /// \brief
  ///   Sets the priority of this occlusion test. 
  /// 
  /// Occlusion tests are rendered in the order of descending priorities. This means that if many
  /// occlusion tests are performed and there is an insufficient number of free query handles,
  /// queries with high priorities are more likely to get rendered. Vision automatically sets the
  /// query priorities for built-in object types such as visibility objects, portals and entities
  /// depending on how "heavyweight" they are. Furthermore, querying the results in roughly the
  /// same order as the objects are rendered ensures best CPU-GPU parallelism.
  /// 
  /// \param iPrio
  ///   Priority value.
  inline void SetPriority(unsigned int iPrio) { m_iPriority = iPrio; }

  /// \brief
  ///   Returns the priority of this occlusion test as previously set with SetPriority.
  inline unsigned int GetPriority() const { return m_iPriority; }

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
  VISION_APIFUNC static void SetWaitForFinishDefault(bool bWaitDefault);

  /// \brief
  ///   Returns the default value for CPU-side waiting for all newly created objects (see
  ///   Set/GetWaitForFinish).
  VISION_APIFUNC static bool GetWaitForFinishDefault();

protected:
  unsigned int m_iPriority;
  unsigned int m_iPixelThreshold;
  bool m_bWait;

  static VisQueryRenderState_e eQueryRenderState;
  static bool s_bWaitDefault;
  static const float s_fOcclusionQueryInflateFactor;
};


VISION_ELEMENTMANAGER_TEMPLATE_DECL(VisOcclusionQueryObject_cl)

/// \brief
///   Derived occlusion query object for entities
///   
class VisOcclusionQueryObjectEntity_cl : public VisOcclusionQueryObject_cl
{
public:
  VISION_APIFUNC VOVERRIDE bool Render(VOcclusionQuery &query, const hkvAlignedBBox &safeBox);

private:
  VisBaseEntity_cl *m_pEntity;

  friend class VisBaseEntity_cl;
};


/// \brief
///   Derived occlusion query object for visibility objects 
class VisOcclusionQueryObjectVisObj_cl : public VisOcclusionQueryObject_cl
{
public:
  VISION_APIFUNC VOVERRIDE bool Render(VOcclusionQuery &query, const hkvAlignedBBox &safeBox);

private:
  VisVisibilityObject_cl *m_pVisObject;

  friend class VisVisibilityObject_cl;
};

/// \brief
///   Derived occlusion query object for portals
class VisOcclusionQueryObjectPortal_cl : public VisOcclusionQueryObject_cl
{
public:
  VISION_APIFUNC VOVERRIDE bool Render(VOcclusionQuery &query, const hkvAlignedBBox &safeBox);

private:
  VisPortal_cl *m_pPortal;

  friend class VisPortal_cl;
};

/// \brief
///   Derived occlusion query object for visibility zones
class VisOcclusionQueryObjectVisZone_cl : public VisOcclusionQueryObject_cl
{
public:
  VISION_APIFUNC VOVERRIDE bool Render(VOcclusionQuery &query, const hkvAlignedBBox &safeBox);

private:
  VisVisibilityZone_cl *m_pVisZone;

  friend class VisVisibilityZone_cl;
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
