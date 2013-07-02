/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file TriggerBoxEntity.hpp

#ifndef TRIGGERBOXENTITY_HPP_INCLUDED
#define TRIGGERBOXENTITY_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/EffectsModule.hpp>
#include <Vision/Runtime/Engine/SceneElements/VisApiTriggerComponent.hpp>

#define TRIGGERBOX_ONCAMERAENTER    "OnCameraEnter"
#define TRIGGERBOX_ONCAMERALEAVE    "OnCameraLeave"
#define TRIGGERBOX_ONOBJECTENTER    "OnObjectEnter"
#define TRIGGERBOX_ONOBJECTLEAVE    "OnObjectLeave"

#define TRIGGERBOXOBSERVERFLAGS_NONE          0
#define TRIGGERBOXOBSERVERFLAGS_MAINCAMERA    V_BIT(0)
#define TRIGGERBOXOBSERVERFLAGS_ENTITYLIST    V_BIT(1)
#define TRIGGERBOXOBSERVERFLAGS_ALL_ENTITIES  V_BIT(2)

#define TRIGGERBOX_VERSION_0                                   0     // Initial version
#define TRIGGERBOX_VERSION_1                                   1     // Version 1
#define TRIGGERBOX_VERSION_2                                   2     // Version 2
#define TRIGGERBOX_VERSION_3                                   3     // Version 3 (local bbox is always loaded)
#define TRIGGERBOX_VERSION_4                                   4     // Version 4 (enabled flag)
#define TRIGGERBOX_CURRENT_VERSION TRIGGERBOX_VERSION_4              // Current version


/// \brief
///   Simple trigger box class. This class is the native counterpart for the TriggerBox shape in vForge
class TriggerBoxEntity_cl : public VisBaseEntity_cl, public IVisCallbackHandler_cl
{
public:
  EFFECTS_IMPEXP TriggerBoxEntity_cl();
  EFFECTS_IMPEXP virtual ~TriggerBoxEntity_cl();

  /// \brief
  ///   Determines which events trigger this box. See flags TRIGGERBOXOBSERVERFLAGS_MAINCAMERA and TRIGGERBOXOBSERVERFLAGS_ENTITYLIST
  EFFECTS_IMPEXP void SetObserverFlags(int iFlags);

  /// \brief
  ///   Returns the value set wit SetObserverFlags
  inline int GetObserverFlags() const {return m_iTriggerObserverFlags;}

  /// \brief
  ///    Sets the active status.
  ///
  /// \param bEnabled
  ///     The new active status.
  ///
  /// When set to false, the trigger box will not perform any intersection tests or trigger events.
  /// This means that placing an entity inside the box and then enabling it will trigger an OnEnter event,
  /// but disabling the trigger box will not automatically trigger OnLeave events.
  void SetEnabled(bool bEnabled)
  {
    m_bEnabled = bEnabled;
  }


  /// \brief Check whether the trigger box is set to enabled.
  bool IsEnabled() const
  {
    return m_bEnabled;
  }

  // overridden entity functions
  EFFECTS_IMPEXP virtual void InitFunction() HKV_OVERRIDE;
  EFFECTS_IMPEXP virtual void DeInitFunction() HKV_OVERRIDE;
  EFFECTS_IMPEXP virtual void ThinkFunction() HKV_OVERRIDE;

  /// \brief
  ///   Globally adds an entity to the observer list, i.e. entities that can trigger boxes that have the TRIGGERBOXOBSERVERFLAGS_ENTITYLIST flag
  static EFFECTS_IMPEXP void AddObservedEntity(VisBaseEntity_cl *pEnt);

  /// \brief
  ///   Removes an entity again from the global list
  static EFFECTS_IMPEXP void RemoveObservedEntity(VisBaseEntity_cl *pEnt);

  virtual void OnHandleCallback(IVisCallbackDataObject_cl *pData) HKV_OVERRIDE
  {
    // for the all entities mode we listen to the VisObject3D_cl::OnObject3DDestroyed callback
    VisObject3D_cl *pObj = ((VisObject3DDataObject_cl *)pData)->m_pObject3D;
    m_EntitiesInside.Remove(pObj);
  }

  /// \brief
  ///   When the trigger box is moved/rotated, e.g. via SetPosition()
  ///   the bounding box has to be updated.
  virtual void OnObject3DChanged(int iO3DFlags) HKV_OVERRIDE
  {
    // set flag that trigger boxes position/orientation has changed
    m_bIsChanged = true; 

    hkvMat3 rotmat = GetRotationMatrix();

    if(m_bIsSphere)
    {
      hkvVec3 pos(hkvNoInitialization);
      GetPosition(pos);

      // this equals SetSphere() without changing the size of it
      m_LocalBoundingBox.m_vMin.set(pos.x, pos.y, pos.z);		  
    } else
    {
      // Orientation might have changed 
      m_bIsOriented = !rotmat.isIdentity ();

      // set absolute bounding box (using local box+orientation)
      // in case of sphere, m_AbsBoundingBox is not required since
      // calculations are done only via GetSphere()
      m_AbsBoundingBox.setInvalid();
      //m_AbsBoundingBox.Inflate(m_LocalBoundingBox, rotmat, GetPosition());

      hkvAlignedBBox temp = m_LocalBoundingBox;
      hkvMat4 mTransform (rotmat, GetPosition());
      temp.transformFromOrigin (mTransform);
      m_AbsBoundingBox.expandToInclude (temp);
    }

    VisBaseEntity_cl::OnObject3DChanged(iO3DFlags);
  }  

  /// \brief
  ///   Tests whether the passed bounding box touches the trigger box/sphere
  inline BOOL TestInside(const hkvAlignedBBox &bbox) const
  {
    if (m_bIsSphere)
    {
      const hkvVec4 sphere = GetSphere();
      float fDist = bbox.getDistanceToSquared (sphere.getAsVec3 ());
      return fDist <= sphere.w*sphere.w;
    }
    // surrounding bbox early out
    if (!m_AbsBoundingBox.overlaps(bbox))
      return FALSE;

    if (m_bIsOriented)
      return TestOrientedBoxOverlap(bbox,m_LocalBoundingBox, m_vPosition, m_cachedRotMatrix);

    return TRUE;
  }

  /// \brief
  ///   Tests whether the passed position is in the trigger box/sphere
  inline BOOL TestInside(const hkvVec3& pos) const
  {
    if (m_bIsSphere)
    {
      const hkvVec4 sphere = GetSphere();
      hkvVec3 vDiff = sphere.getAsVec3 () - pos;
      float fDistSqr = vDiff.getLengthSquared ();
      return fDistSqr <= sphere.w * sphere.w;
    }
    // surrounding bbox early out
    if (!m_AbsBoundingBox.contains (pos))
      return FALSE;

    if (m_bIsOriented)
    {
      hkvMat3 transposed(m_cachedRotMatrix);
      transposed.transpose();
      hkvVec3 vLocal = pos - m_vPosition;
      vLocal = transposed.transformDirection(vLocal);
      return m_LocalBoundingBox.contains (vLocal);
    }

    return TRUE;
  }

  /// \brief Returns the bounding sphere around the trigger. Only use when m_bIsSphere is set to true.
  inline const hkvVec4 GetSphere() const
  {
    return m_LocalBoundingBox.m_vMin.getAsVec4 (m_fSphereRadius);
  }

  /// \brief Sets the bounding sphere of the trigger. You should also set m_bIsSphere to true.
  inline void SetSphere(const hkvVec4& sphere)
  {
    m_LocalBoundingBox.m_vMin.set (sphere.x, sphere.y, sphere.z);
    m_fSphereRadius = sphere.w;
  }

  /// \brief
  ///   Renders the box/sphere as displayed in the editor
  EFFECTS_IMPEXP void DebugRender(IVRenderInterface* pRenderer, VColorRef iColor);

public:
  V_DECLARE_SERIAL_DLLEXP( TriggerBoxEntity_cl,  EFFECTS_IMPEXP );
  EFFECTS_IMPEXP virtual void Serialize( VArchive &ar ) HKV_OVERRIDE;
  IMPLEMENT_OBJ_CLASS(TriggerBoxEntity_cl);

  hkvAlignedBBox m_AbsBoundingBox;          ///< the absolute bounding box in world space
  hkvAlignedBBox m_LocalBoundingBox;        ///< for oriented trigger boxes; Sphere uses min.xyz + m_fSphereRadius

  float m_fSphereRadius;                    ///< radius of the trigger sphere; only used when m_bIsSphere is set 
  bool m_bCameraIsInside;                   ///< true if the camera is inside the trigger volume, false if not
  bool m_bDestroyOnTrigger;                 ///< if set to true the trigger box is destroyed after the first trigger event
  bool m_bIsSphere;                         ///< specifies whether the trigger shape is a sphere or a box
  bool m_bIsOriented;                       ///< specifies if the trigger box is oriented or world space aligned
  bool m_bEnabled;                          ///< when disabled, the trigger box will never trigger events
  VisEntityCollection_cl m_EntitiesInside;  ///< stores all entities that are inside

  int m_iTriggerObserverFlags;
  VSmartPtr<VisTriggerSourceComponent_cl> m_spOnCameraEnter; ///< Component that triggers the OnCameraEnter event
  VSmartPtr<VisTriggerSourceComponent_cl> m_spOnCameraLeave; ///< Component that triggers the OnCameraLeave event
  VSmartPtr<VisTriggerSourceComponent_cl> m_spOnObjectEnter; ///< Component that triggers the OnObjectEnter event
  VSmartPtr<VisTriggerSourceComponent_cl> m_spOnObjectLeave; ///< Component that triggers the OnObjectLeave event

  VisBaseEntity_cl *m_pCurrentEntity; ///< only valid inside loop
protected:
  // observer List
  EFFECTS_IMPEXP static BOOL TestOrientedBoxOverlap(const hkvAlignedBBox &aabox,const hkvAlignedBBox &obox, const hkvVec3& vPos, const hkvMat3 &rot);

  // These 3 return true if triggers have occurred
  bool HandleSortedEntityList(const VisEntityCollection_cl& observedEntities);
  bool HandleUpdatedEntityList(const VisEntityCollection_cl& UpdatedEntities);
  bool HandleUpdatedTriggerBox();

  static bool g_bObservedEntityListNeedsSorting;
  static VisEntityCollection_cl g_ObservedEntities;
  bool m_bIsChanged; // Flag if trigger box has been moved, rotated
};

#endif //TRIGGERBOXENTITY_HPP_INCLUDED

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
