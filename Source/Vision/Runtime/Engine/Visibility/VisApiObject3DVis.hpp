/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiObject3DVis.hpp

#ifndef VIS_DEFINE_VISAPIOBJECT3DVISDATA_INCLUDED
#define VIS_DEFINE_VISAPIOBJECT3DVISDATA_INCLUDED

#include <Vision/Runtime/Base/Math/BoundingVolume/hkvAlignedBBox.h>

class VisRenderContext_cl;
class VisVisibilityZone_cl;
class VisPortal_cl;

/// \brief
///   Class used to handle visibility zone assignment of dynamic scene elements.
/// 
/// A VisObject3DVisData_cl instance can be added to a VisObject3D_cl object (or instances of
/// classes derived from VisObject3D_cl) in order to allow the VisObject3D_cl to keep track of its
/// assignment to visibility zones.
/// 
/// By default, VisObject3DVisData_cl objects will automatically be added to an internal list of
/// modified instances whenever the VisObject3D_cl they belong to moves. At the end of the update
/// loop of the VisionApp, these objects are processed, and their visibility zone assignments are
/// updated.
/// 
/// Each VisObject3DVisData_cl (and, therefore, each VisObject3D_cl) can belong to an arbitrary
/// number of visibility zones.
/// 
/// Note that VisObject3DVisData_cl objects can transition between visibility zones directly
/// connected to each other without restrictions. For zones connected through portals, however,
/// zone transitions require the VisObject3DVisData_cl to move through the portal between the two
/// zones. If this is not the case, incorrect zone assignments (and thus incorrect visibility
/// classifications) will result.
/// 
/// If objects need to be able to move "through walls", use the RecomputeVisibilityZoneAssignment
/// method to ensure that the zone assignment is recomputed.
class VisObject3DVisData_cl
{
public:

  /// \brief
  ///   Constructor of the VisObject3DVisData_cl class.
  /// 
  /// \param pObj3D
  ///   VisObject3D_cl instance the VisObject3DVisData_cl belongs to (note that this has to be a
  ///   1:1 relation).
  /// 
  /// \param bAutomaticUpdate
  ///   If true, visibility zone transitions are handled automatically, which is the best solution
  ///   in most cases.
  VISION_APIFUNC VisObject3DVisData_cl(VisObject3D_cl *pObj3D, bool bAutomaticUpdate = true);

  /// \brief
  ///   Destructor of the VisObject3DVisData_cl class.
  VISION_APIFUNC ~VisObject3DVisData_cl();

  /// \brief
  ///   Recomputes the visibility zone assignment.
  /// 
  /// This method has to be called whenever the object "teleports" from one place to another, since
  /// this would otherwise cause the portal/zone tracing mechanism to return incorrect results.
  /// 
  /// This method is typically not called directly. Use the VisObject3D_cl::ReComputeVisibility
  /// function instead.
  VISION_APIFUNC void RecomputeVisibilityZoneAssignment();

  /// \brief
  ///   Returns the number of visibility zones this object is currently in.
  inline unsigned int GetNumVisibilityZones() const {return m_iNumVisibilityZones;}
  
  /// \brief
  ///   Returns the visibility zone with the spefied index [0..GetNumVisibilityZones()-1]
  inline VisVisibilityZone_cl *GetVisibilityZone(int iIndex) const
  {
    VASSERT(iIndex>=0 && iIndex<m_iNumVisibilityZones);
    return m_VisibilityZones.GetDataPtr()[iIndex];
  }

  /// \brief
  ///   Returns a list of all visibility zones this object is currently in.
  /// 
  /// The first zone in the returned list is the node the origin of the object is contained in.
  /// 
  /// \param ppVisZoneList
  ///   Pointer to an array of visibility zone the returned zones will be stored in.
  /// 
  /// \param iMaxNum
  ///   The maximum number of zones to return (typically the size of the ppVisZoneList array).
  /// 
  /// \return
  ///   unsigned int: The actual number of visibility zones returned in the list.
  VISION_APIFUNC unsigned int GetVisibilityZones(VisVisibilityZone_cl **ppVisZoneList, unsigned int iMaxNum) const;

  /// \brief
  ///   Returns a list of all visibility zones this object is currently in.
  /// 
  /// The first zone in the returned list is the node the origin of the object is contained in.
  /// 
  /// \param pppVisZoneList
  ///   Pointer in which a pointer to an array of visibility zone the returned zones will be stored
  ///   in.
  /// 
  /// \return
  ///   unsigned int: The actual number of visibility zones in the list.
  inline unsigned int GetVisibilityZonesPtr(VisVisibilityZone_cl* **pppVisZoneList) const { *pppVisZoneList = m_VisibilityZones.GetDataPtr(); return m_iNumVisibilityZones; }

  /// \brief
  ///   Sets the radius of this object for visibility zone tracking.
  /// 
  /// The radius should be set by visibility-enabled scene elements so that, using the
  /// VisObject3D_cl's position as the center, the resulting bounding sphere is the smallest sphere
  /// completely containing the scene element. For the default scene elements in the Vision engine
  /// (light sources, entities, and visibility objects), this is done automatically.
  /// 
  /// \param fRadius
  ///   radius to set.
  VISION_APIFUNC inline void SetRadius(float fRadius) { m_fRadius = fRadius; }

  /// \brief
  ///   Returns the radius previously set with SetRadius.
  VISION_APIFUNC inline float GetRadius() const { return m_fRadius; }

  /// \brief
  ///   Enables/disables automatic updating of the visibility zone assignment of this scene
  ///   element.
  /// 
  /// If automatic update is enabled, the Vision engine will take care of updating the scene
  /// element's visibility zone assignments whenever the object moves.
  /// 
  /// If automatic update is disabled, you are fully responsible for handling visibility zone
  /// transitions, i.e. if an object is created or if it moves, you will have to assign it to the
  /// correct visibility zone.
  /// 
  /// \param bStatus
  ///   true to enable automatic updating, false to disable it.
  inline void SetAutomaticUpdate(bool bStatus) 
  { 
    m_bAutomaticUpdate = bStatus; 
  }

  /// \brief
  ///   Returns the status of automatic updating (previously set with SetAutomaticUpdate).
  VISION_APIFUNC inline bool GetAutomaticUpdate() const { return m_bAutomaticUpdate; }

  /// \brief
  ///   Returns the visibility zone the origin of the scene element is in.
  VISION_APIFUNC inline VisVisibilityZone_cl *GetVisibilityZoneOfOrigin() { 
    if (m_iNumVisibilityZones>0) 
    {
      VisVisibilityZone_cl *pZone = m_VisibilityZones.GetDataPtr()[0]; 
      return pZone;
    }
    return NULL;
  }

  /// \brief
  ///   Static function handling all node transitions of dynamic scene elements since the last call
  ///   to this function.
  /// 
  /// Objects which move during a game tick are automatically added to an internal list which is
  /// then processed by this method.
  /// 
  /// By default, this function is called near the end of VisionApp_cl::OnUpdateScene.
  /// 
  /// This function has to be called after all scene element positions in a game tick have been
  /// updated, but before visibility determination starts.
  VISION_APIFUNC static void HandleAllNodeTransitions();
  

  /// \brief
  ///   Removes the passed zone reference from the internal list
  VISION_APIFUNC void RemoveVisibilityZone(VisVisibilityZone_cl *pZone);

  /// \brief
  ///   indicates whether this object references the passed visibility zone
  inline bool ContainsVisibilityZone(VisVisibilityZone_cl *pZone) const
  {
    return m_VisibilityZones.GetElementPos(pZone, m_iNumVisibilityZones)>=0;
  }

  /// \brief
  ///   Sets a new list of visibility zones. If used externally, SetAutomaticUpdate should be set to false so that this list isn't overwritten next frame
  VISION_APIFUNC void UpdateNodeList(int iNumNewVisibilityZones, VisVisibilityZone_cl **ppNewVisibilityZones);

  // Internally called only
  inline unsigned int GetLastVisibilityZoneRecomputation() const { return m_iLastVisibilityZoneRecomputation; }
  VISION_APIFUNC void Invalidate();
  VISION_APIFUNC void HandleAdjacentNodes();
  VISION_APIFUNC void OnChanged();
  VISION_APIFUNC void HandleNodeTransition();
  void TestPortalTransition(VisVisibilityZone_cl *pZone, const hkvVec3& startPos, const hkvVec3& endPos, VisVisibilityZone_cl **ppNewVisibilityZones, int &iNumVisibilityZones, int iMaxNumVisibilityZones, const hkvAlignedBBox &totalBox);
  VISION_APIFUNC void AddVisibilityZone(VisVisibilityZone_cl *pZone);
  VISION_APIFUNC static void TestOverlappingVisibilityZones(VisVisibilityZone_cl *pZone, const hkvBoundingSphere& bsphere, VisVisibilityZone_cl **ppNewVisibilityZones, int &iNumVisibilityZones, int iMaxNumVisibilityZones);

  VISION_APIFUNC void SerializeX(VArchive &ar);
  VISION_APIFUNC void ShowAssignmentInfo(VisRenderContext_cl *pContext, IVRender2DInterface *pRI, VColorRef iTextCol, int iYOfs=0);
private:
  friend class VisObject3D_cl;

  VisObject3D_cl *m_pObject3D;
  bool m_bAutomaticUpdate;    ///< if automatic update is disabled, portal tracing etc. won't be handled automatically any more!
  bool m_bIsInChangedList;

  hkvVec3 m_vLastPos;
  unsigned int m_iLastUpdate;
  unsigned int m_iLastVisibilityZoneRecomputation;

  float m_fRadius;

  DynArray_cl<VisVisibilityZone_cl *> m_VisibilityZones;
  int m_iNumVisibilityZones;

  static DynArray_cl<VisObject3DVisData_cl *>g_ChangedElements;
  static int m_iNumChangedElements;
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
