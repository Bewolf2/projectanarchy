/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiVisibilityObject.hpp

#ifndef _VISAPIVISIBILITYOBJECT_HPP_INCLUDED_
#define _VISAPIVISIBILITYOBJECT_HPP_INCLUDED_

//#include "VisApiElementVisData.hpp"

#include <Vision/Runtime/Engine/SceneElements/VisApiSortedRenderCollection.hpp>
#include <Vision/Runtime/Engine/Renderer/OcclusionQuery/VisApiOcclusionQueryObject.hpp>
#include <Vision/Runtime/Engine/Visibility/VisApiVisibilityData.hpp>

// visibility test flag constants
#define VISTESTFLAGS_FRUSTUMTEST                  0x0001
#define VISTESTFLAGS_HARDWAREOCCLUSIONQUERY       0x0002
#define VISTESTFLAGS_PORTALTEST                   0x0004
#define VISTESTFLAGS_FARCLIPTEST                  0x0008
#define VISTESTFLAGS_PERFORM_ALL_TESTS            0x00ff
#define VISTESTFLAGS_ACTIVE                       0x0100
#define VISTESTFLAGS_DEBUG_RENDER                 0x0200
#define VISTESTFLAGS_OCCLUDERTEST                 0x0400
#define VISTESTFLAGS_BITMASK                      0x1000
#define VISTESTFLAGS_NOTRELEVANTFORCONTEXT        0x2000
#define VISTESTFLAGS_CONTEXTINACTIVE              0x4000
#define VISTESTFLAGS_TESTVISIBLE_NOT_OVERRIDDEN   0x8000

#define VIS_VISOBJ_LASTFRAMERENDERED            0x0001
#define VIS_VISOBJ_GETOCCQUERYRESULT            0x0002

class IVisVisibilityCollector_cl;
class VisVisibilityObject_cl;
class VisVisibilityObjectAABox_cl;
class VisVisibilityObjectCollection_cl;
class VisRenderContext_cl;
typedef VSmartPtr<VisVisibilityObject_cl> VisVisibilityObjectPtr;
typedef VSmartPtr<VisVisibilityObjectAABox_cl> VisVisibilityObjectAABoxPtr;

/// \brief
///   Abstract base class for visibility objects.
/// 
/// Visibility objects can be used to perform visibility tests, including portal/frustum tests and
/// hardware occlusion queries. Visibility objects are derived from VisObject3D_cl and  can track
/// their assignment to visibility zones automatically. Since arbitrary VisObject3D_cl instances
/// can be attached to visibility objects, they are an ideal tool for low-overhead visibility
/// determination of dynamic scene elements. By default, this feature of visibility objects is used
/// for mesh buffer objects and particle groups.
/// 
/// Since this class has some pure virtual functions only derived classes
/// (VisVisibilityObjectAABox_cl) can be created directly.
/// 
/// Furthermore they can be used for custom (e.g. hierarchical) visibility tests.
class VisVisibilityObject_cl : public VisObject3D_cl, public VVisibilityData, public VisElementManager_cl<VisVisibilityObject_cl *>
{
public:

  /// \brief
  ///   Constructor of the visibility object base class
  /// 
  /// \param iFlags
  ///   the flags for the tests. These flags can be changed at any time using SetVisTestFlags For a
  ///   decription of valid flags, see SetVisTestFlags
  /// 
  /// \sa VisVisibilityObject_cl::SetVisTestFlags
  VISION_APIFUNC VisVisibilityObject_cl(int iFlags=VISTESTFLAGS_PERFORM_ALL_TESTS);

  /// \brief
  ///   Destructor.
  VISION_APIFUNC virtual ~VisVisibilityObject_cl();

  /// \brief
  ///   Overridden VisTypedEngineObject_cl function to remove this instance from the scene
  VISION_APIFUNC VOVERRIDE void DisposeObject();

  /// \brief
  ///   Sets the flags for the visibility tests
  /// 
  /// \param iFlags
  ///   the flags for the tests. The flags are a bit combination of the following constants:
  ///   \li VISTESTFLAGS_FRUSTUMTEST : If specified, the engine performs a view frustum check on
  ///     the bounding box of this object.  View frustum checks are fast so it makes sense to
  ///     specify this flag for early out. This automatically takes frustum clipping due to portals
  ///     into account.
  /// 
  ///   \li VISTESTFLAGS_HARDWAREOCCLUSIONQUERY : If specified, the engine performs a hardware
  ///     occlusion query test. Hardware occlusion query is pixel accurate, but the result arrives
  ///     with a delay of one frame.
  /// 
  ///   \li VISTESTFLAGS_FARCLIPTEST :  If specified, a far clip test is performed. The distance at
  ///     which the object is clipped can be set via the VisVisibilityObject_cl::SetFarClipDistance
  ///     function.
  /// 
  ///   \li VISTESTFLAGS_ACTIVE :     This flag should be specified to activate the visibility
  ///     object. Visibility tests are performed only if the visibility object is active. 
  /// 
  ///   \li VISTESTFLAGS_DEBUG_RENDER : If specified, the visibility shape (e.g. bounding box) is
  ///     rendered as a wireframe box.
  /// 
  ///   \li VISTESTFLAGS_PERFORM_ALL_TESTS : This is always a combination of all supported test
  ///     methods
  /// 
  /// \sa VisVisibilityObject_cl::GetVisTestFlags
  /// \sa VisVisibilityObject_cl::ScheduleVisTest
  /// \sa VisVisibilityObject_cl::WasVisibleLastFrame
  VISION_APIFUNC void SetVisTestFlags(int iFlags);

  /// \brief
  ///   Gets the current flags set for the visibility tests.
  /// 
  /// \return
  ///   int iFlags : the current flags for the tests. See SetVisTestFlags for supported flags
  /// 
  /// \sa VisVisibilityObject_cl::SetVisTestFlags
  /// \sa VisVisibilityObject_cl::ScheduleVisTest
  /// \sa VisVisibilityObject_cl::WasVisibleLastFrame
  inline int GetVisTestFlags() const
  {
    return m_iFlags;
  }
  
  /// \brief
  ///   Activates visibility testing for this visibility object.
  /// 
  /// The vistest can also be activated using the VISTESTFLAGS_ACTIVE flag in the SetVisTestFlags
  /// function.
  /// 
  /// \param bStatus
  ///   new activation status
  /// 
  /// \sa VisVisibilityObject_cl::ScheduleVisTest
  /// \sa VisVisibilityObject_cl::SetVisTestFlags
  VISION_APIFUNC void SetActivate(BOOL bStatus);
  
  /// \brief
  ///   Queries the result from the last visibility test for a specific context's visibility
  ///   collector.
  /// 
  /// If there was no test scheduled, it returns the result from the last test that has been
  /// performed for this context's visibility collector.
  /// 
  /// \param pContext
  ///   Render Context for which the visibility status is to be queried. If the context is NULL,
  ///   the currently active render context is used.
  /// 
  /// \return
  ///   BOOL bResult : TRUE, if the object was visible.
  /// 
  /// \sa VisVisibilityObject_cl::ScheduleVisTest
  VISION_APIFUNC BOOL WasVisibleLastFrame(VisRenderContext_cl *pContext=NULL) const;

  /// \brief
  ///   Queries the result from the last visibility test(s) and returns whether the visibility
  ///   object was visible in the last rendered frame of any active context.
  /// 
  /// \return
  ///   BOOL bResult : TRUE, if the object was visible in any active context.
  /// 
  /// \sa VisVisibilityObject_cl::ScheduleVisTest
  /// \sa VisVisibilityObject_cl::WasVisibleLastFrame
  VISION_APIFUNC BOOL WasVisibleInAnyLastFrame() const;

  /// \brief
  ///   Sets the bounding box of this visibility object in the object's local space.
  /// 
  /// This will overwrite previously set bounding boxes.
  /// 
  /// There is a certain overhead associated with calling this function. Avoid unnecessary bounding
  /// box updates.
  /// 
  /// \param bbox
  ///   New local space bounding box for this visibility object.
  /// 
  /// \sa VisVisibilityObject_cl::SetWorldSpaceBoundingBox
  VISION_APIFUNC void SetLocalSpaceBoundingBox(const hkvAlignedBBox &bbox);

  /// \brief
  ///   Returns the bounding box of this visibility object in the object's local space coordinates.
  inline const hkvAlignedBBox &GetLocalSpaceBoundingBox() const
  {
    return m_LocalSpaceBoundingBox;
  }


  /// \brief
  ///   Returns a pointer to the bounding box of this visibility object in the object's local space
  ///   coordinates.
  inline const hkvAlignedBBox* GetLocalSpaceBoundingBoxPtr() const 
  {
    return &m_LocalSpaceBoundingBox;
  }

  /// \brief
  ///   Sets the bounding box of this visibility object in world space coordinates.
  /// 
  /// Note that this will cause the visibility object to be repositioned to the center of the
  /// passed bounding box.
  /// 
  /// This method is significantly more expensive than calling SetLocalSpaceBoundingBox. In cases
  /// where frequent updates of bounding boxes are necessary, use SetLocalSpaceBoundingBox instead.
  /// 
  /// This will overwrite previously set bounding boxes.
  /// 
  /// \param bbox
  ///   New world space bounding box for this visibility object.
  /// 
  /// \param pCustomPivot
  ///   Defines a custom point inside a visibility zone to start traversing. Can be NULL to use the center of the passed box
  /// 
  /// \sa VisVisibilityObject_cl::SetWorldSpaceBoundingBox
  VISION_APIFUNC void SetWorldSpaceBoundingBox(const hkvAlignedBBox &bbox, const hkvVec3* pCustomPivot=NULL);

  /// \brief
  ///   Returns the bounding box of this visibility object in world space coordinates.
  inline const hkvAlignedBBox &GetWorldSpaceBoundingBox() const
  {
    return m_BoundingBox;
  }

  /// \brief
  ///   Returns a pointer to the bounding box of this visibility object in world space coordinates.
  inline const hkvAlignedBBox* GetWorldSpaceBoundingBoxPtr() const 
  {
    return &m_BoundingBox;
  }

  /// \brief
  ///   Returns the occlusion query object for this visibility object. Used for visibility
  ///   determination.
  inline const VisOcclusionQueryObject_cl &GetOccQueryObject() const 
  { 
    return m_OccQueryObject; 
  }

  /// \brief
  ///   Specifies how many pixels of this visibility object have to be visible to pass the
  ///   occlusion test.
  /// 
  /// This value is ignored if occlusion query is not used for this visibility object.
  /// 
  /// The default count is 5. This number has proven to be useful to avoid visibility
  /// over-estimation for objects behind solid walls with t-junction geometry.
  /// 
  /// \param iPixelCount
  ///   Number of pixels that have to pass the occlusion test.
  inline void SetOcclusionQueryPixelThreshold(unsigned int iPixelCount)
  {
    m_iPixelThreshold = iPixelCount;
    m_OccQueryObject.SetPixelThreshold(iPixelCount);
  }

  /// \brief
  ///   Returns the threshold value set via SetOcclusionQueryPixelThreshold.
  /// 
  /// \return
  ///   unsigned int: Number of pixels that have to pass the occlusion test.
  inline unsigned int GetOcclusionQueryPixelThreshold() const
  {
    return m_iPixelThreshold;
  }

  /// \brief
  ///   Sets the reschedule mask for this visibility object.
  /// 
  /// The reschedule mask is used to determine whether it is time to schedule a visibility object
  /// that was previously classified as visible for a new occlusion test.
  /// 
  /// If an object has been classified as visible during the last occlusion test, it is likely that
  /// it will remain visible for some more frames. Since occlusion query operations are not free,
  /// it is therefore common practice to render occlusion queries only every n frames as long as
  /// they are classified as visible.
  /// 
  /// In order to ensure a roughly even distribution of occlusion query rendering operations across
  /// frames, Vision uses the occlusion query object's element manager index and the current render
  /// count of the render context to determine whether it is time to reschedule the test.
  /// 
  /// The default mask value of 3 means that the object will be rescheduled every fourth frame. A
  /// mask of 7, for instance, would mean rescheduling every eighth, a value of 15 every 16th
  /// frame.
  /// 
  /// Legal values for iMask are 1, 3, 7, 15, 31, 63, 127, and 255.
  /// 
  /// \param iMask
  ///   The new reschedule mask.
  /// 
  /// \sa VisRenderContext_cl::ShouldScheduleOcclusionTest
  inline void SetRescheduleMask(char iMask) 
  { 
    m_iRescheduleMask = iMask; 
  }

  /// \brief
  ///   Returns the reschedule mask for this visibility object.
  inline char GetRescheduleMask() const 
  { 
    return m_iRescheduleMask; 
  }

  /// \brief
  ///   Adds a VisObject3D_cl instance to this visibility object.
  /// 
  /// Visibility objects are typically used to perform visibility determination for one or more
  /// objects which do not have their own visibility functionality.
  /// 
  /// Using this method, you can attach arbitrary VisObject3D_cl instances to a visibility object.
  /// Render loop implementations, for example, can then iterate over all visible visibility
  /// objects and thereby determine all visible VisObject3D_cl instances.
  /// 
  /// \param pObject
  ///   The VisObject3D_cl instance to add.
  /// 
  /// \return
  ///   Nothing.
  VISION_APIFUNC void AddObject3D(VisObject3D_cl *pObject);

  /// \brief
  ///   Removes the passed object from the list of referenced VisObject3D_cl instances.
  VISION_APIFUNC void RemoveObject3D(VisObject3D_cl *pObject);

  /// \brief
  ///   Returns the number of VisObject3D_cl instances attached to this visibility object.
  inline int GetObjectCount() const 
  { 
    return m_ReferencedObjects.GetNumEntries(); 
  }

  /// \brief
  ///   Returns a list of all VisObject3D_cl instances attached to this visibility object.
  /// 
  /// \return
  ///   VisObject3D_cl **: Pointer to the list of attached VisObject3D_cl instances.
  /// 
  /// \sa AddObject3D
  inline VisObject3D_cl **GetObjectList() 
  { 
    return m_ReferencedObjects.GetDataPtr(); 
  }


  /// \brief
  ///   Overridable that is called by a collector when this object is determined to be visible
  ///
  /// Since visibility can be performed inside a thread, this function can be called from outside the main
  /// thread and also multiple times at the same time.
  /// Derived classes should not call this base implementation as this deactivates the callback.
  ///
  /// \param pCollector
  ///   The visibility collector instance that determines the visibility
  ///
  /// \param pFrustum
  ///   The current view frustum for testing - might be a frustum deeper in the portal frustum stack
  ///
  /// \return
  ///   If FALSE, the object is not flagged as visible
  VISION_APIFUNC virtual BOOL OnTestVisible(IVisVisibilityCollector_cl *pCollector, const VisFrustum_cl *pFrustum)
  {
    m_iFlags |= VISTESTFLAGS_TESTVISIBLE_NOT_OVERRIDDEN;
    return TRUE;
  }

  void UpdateVisDataRadius();
  void UpdateWorldSpaceBoundingBox();
  VISION_APIFUNC VOVERRIDE void OnObject3DChanged(int iO3DFlags);
#ifdef SUPPORTS_SNAPSHOT_CREATION
  VISION_APIFUNC VOVERRIDE void GetDependencies(VResourceSnapshot &snapshot); ///< object3d override
#endif
  inline static unsigned int GetVisDataOffset() { return offsetof(VisVisibilityObject_cl, m_BoundingBox); }
 
private:
  friend class VisRenderContext_cl;
  friend class VisGame_cl;
  friend class VisMeshBufferObject_cl;
  friend class VisParticleGroup_cl;
  friend class VisVisibilityObjectCollection_cl;
  friend class VisRenderLoopHelper_cl;
  friend inline char IsVisibilityObjectOccluder(VisVisibilityObject_cl *pObj);
  friend inline int DoesLightAffectVisObject(VisVisibilityObject_cl *pVisObject, int iLight);

protected:
  VISION_APIFUNC VOVERRIDE void Serialize( VArchive &ar );
  void CreateHWHandle();

  VisObject3DCollection_cl m_ReferencedObjects;

  VisOcclusionQueryObjectVisObj_cl m_OccQueryObject;


  // Make sure the size of the following data is larger than sizeof(float) + sizeof(hkvVec3)
  hkvAlignedBBox m_LocalSpaceBoundingBox;
  unsigned int m_iTagCtr;

  unsigned int m_iPixelThreshold;
  int m_iFlags;
  bool m_bHasMoved;
  bool m_bBoundingBoxSet, m_bIsWorldSpaceBBox;

  char m_iRescheduleMask;

};



/// \brief
///   Instanceable derived visibility object class for axis aligned bounding boxes.
/// 
/// For more information, see base class VisVisibilityObject_cl
class VisVisibilityObjectAABox_cl : public VisVisibilityObject_cl
{
public:


  /// \brief
  ///   Constructor of the bounding box visibility object class
  /// 
  /// \param iFlags
  ///   the flags for the tests. For a decription of valid flags,  see
  ///   VisVisibilityObject_cl::SetVisTestFlags
  /// 
  /// \sa VisVisibilityObject_cl::SetVisTestFlags
  VISION_APIFUNC VisVisibilityObjectAABox_cl(int iFlags=VISTESTFLAGS_PERFORM_ALL_TESTS);


protected:

  // serialization
  V_DECLARE_SERIAL_DLLEXP( VisVisibilityObjectAABox_cl, VISION_APIDATA )
  VISION_APIFUNC VOVERRIDE void Serialize( VArchive &ar );
};



/// \brief
///   Collection class for visibility objects.
class VisRefCountedVisibilityObjectCollection_cl : public VRefCountedCollection<VisVisibilityObject_cl>
{
public:

  /// \brief
  ///   Useful helper function that returns the first visibility object in this collection (or
  ///   NULL) that is visible in the passed render context.
  VISION_APIFUNC VisVisibilityObject_cl* IsAnyVisible(VisRenderContext_cl *pContext=NULL);

  /// \brief
  ///   Useful helper function that returns the first visibility object in this collection (or
  ///   NULL) that is visible in any render context.
  VISION_APIFUNC VisVisibilityObject_cl* IsAnyVisibleInAnyContext();

  V_DECLARE_SERIALX( VisRefCountedVisibilityObjectCollection_cl, VISION_APIFUNC )
  VISION_APIFUNC void SerializeX( VArchive &ar );
};

VISION_ELEMENTMANAGER_TEMPLATE_DECL(VisVisibilityObject_cl)

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
