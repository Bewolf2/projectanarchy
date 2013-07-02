/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiStaticGeometryInstance.hpp

#ifndef DEFINE_VISAPISTATICGEOMETRYINSTANCE
#define DEFINE_VISAPISTATICGEOMETRYINSTANCE

#include <Vision/Runtime/Base/Math/BoundingVolume/hkvAlignedBBox.h>
#include <Vision/Runtime/Engine/Visibility/VisApiVisibilityData.hpp>


class VisStaticGeometryInstance_cl;


/// \brief
///   Data object that is sent by the static VisStaticGeometryInstanceObject_cl::OnStaticGeometryInstanceCreated and
///   OnStaticGeometryInstanceDestroyed callback. It has a m_pInstance member pointer. 
class VisStaticGeometryInstanceObject_cl : public IVisCallbackDataObject_cl
{
public:
  inline VisStaticGeometryInstanceObject_cl(VCallback *pSender, VisStaticGeometryInstance_cl *pInstance) : IVisCallbackDataObject_cl(pSender)
  {
    m_pInstance = pInstance;
  }
  VisStaticGeometryInstance_cl *m_pInstance;
};

/// \brief
///   Class for static geometry instances, including Vision's static submesh instances, terrain,
///   and custom geometry instances
/// 
/// Abstract base class for static geometry instances, including Vision's static submesh instances,
/// terrain, and custom geometry instances
/// 
/// All static geometry classes should be derived from this class. For instance, Vision's static
/// submesh instances and terrain sectors are subclasses of VisStaticGeometryInstance_cl.
/// 
/// Unlike scene elements derived from VisObject3D_cl, subclasses of VisStaticGeometryInstance_cl
/// should not alter their position or orientation dynamically. They do not perform visibility zone
/// tracking.
/// 
/// Static Geometry Instances are kept in a global element manager.
/// 
/// Custom static geometry instance types have to use geometry types starting with
/// STATIC_GEOMETRY_TYPE_FIRSTCUSTOM.
class VisStaticGeometryInstance_cl : public VUserDataObj, public VVisibilityData, public VisElementManager_cl<class VisStaticGeometryInstance_cl *>
{
  V_DECLARE_SERIALX( VisStaticGeometryInstance_cl, VISION_APIFUNC )

public:

  ///
  /// @name Construction and Destruction
  /// @{
  ///

  /// \brief
  ///   Constructor of the static geometry instance base class.
  VISION_APIFUNC VisStaticGeometryInstance_cl();

  /// \brief
  ///   Destructor of the static geometry instance base class.
  VISION_APIFUNC virtual ~VisStaticGeometryInstance_cl();

  ///
  /// @}
  ///

  ///
  /// @name Virtual Functions
  /// @{
  ///

  /// \brief
  ///   Virtual render method. Needs to be implemented in subclasses.
  /// 
  /// The Vision engine will call this method for each object inside a
  /// VisStaticGeometryInstanceCollection_cl passed to one of the appropriate rendering methods in
  /// the VisRenderLoopHelper_cl class.
  /// 
  /// The order in which the instances' render methods are called depends on their sorting key,
  /// which can be set using the m_iSortingKey member. See the documentation of ComputeSortingKey
  /// for more details.
  /// 
  /// The collection and index parameters can be used inside the Render function to reduce state
  /// setup overhead by checking which instance types are being rendered before and after the
  /// current geometry instance.
  /// 
  /// Typically, rendering should be performed using the BeginMeshRendering/EndMeshRendering
  /// functionality provided by the VisRenderLoopHelper_cl class.
  /// 
  /// \param pShader
  ///   Shader with which to render the geometry.
  /// 
  /// \param allInstances
  ///   Reference to the collection of static geometry instances being rendered in this draw call.
  /// 
  /// \param iThisIndex
  ///   Index of this static geometry instance within the passed collection.
  /// 
  /// \param ePassType
  ///   Passtype (one of the VPassType_e enumeration members).
  VISION_APIFUNC virtual void Render(VCompiledShaderPass *pShader, const VisStaticGeometryInstanceCollection_cl &allInstances, int iThisIndex, VPassType_e ePassType) {}

  /// \brief
  ///   Virtual tracing method. Needs to be implemented in subclasses.
  /// 
  /// The Vision engine will call this method from trace line operations executed within the
  /// engine. Note that it will not be used by external physics modules like Havok Physics; for
  /// physics and physics trace lines, you will have to add support for custom geometry types to
  /// the physics modules.
  /// 
  /// Vision will perform a coarse early-out check by bounding box before calling this function.
  /// 
  /// \param startOfLine
  ///   Start of the ray segment to test against.
  /// 
  /// \param endOfLine
  ///   End of the ray segment to test against. geometry instances being rendered in this draw
  ///   call.
  /// 
  /// \param iStoreResultCount
  ///   Index in the trace info table at which this result is to be stored.
  /// 
  /// \param pFirstTraceInfo
  ///   Trace info table to fill. Should be ignored if NULL is passed.
  /// 
  /// \return
  ///   int iNumberOfHits: Number of hits found.
  VISION_APIFUNC virtual int TraceTest(const hkvVec3& startOfLine, const hkvVec3& endOfLine, int iStoreResultCount=0, VisTraceLineInfo_t *pFirstTraceInfo=NULL) {return 0;}

  /// \brief
  ///   Virtual function computing the sorting key for this static geometry instance.
  /// 
  /// Sorting keys can be used to influence the order in which geometry is drawn. Since sorting
  /// keys of static geometry instances should not be changed every frame, this functionality is
  /// mostly present  to ensure low state change overhead by grouping similar instances together.
  /// For example, instances with the same texture, lightmap, or tranformation matrix may have
  /// identical sorting keys in order to ensure they are drawn right after each other.
  ///
  /// The sorting key computation of the abstract VisStaticGeometryInstance_cl base class stores
  /// the following data in the sorting key:
  /// \li Bit  31: 1 = Opaque, 0 = any transparency type
  /// \li Bits 29, 30: Geometry type (static mesh, terrain, custom geometry)
  /// \li Bits 25 to 28: Surface sorting key (lower four bits)
  /// \li Bits 17 to 24: Source Effect of the applied shader technique
  /// \li Bits 9 to 16: Surface Pointer
  /// Derived classes should first call this method in their own ComputeSortingKey() implementations.
  /// The lower ten bits can be set in derived classes if desired. Derived classes should NEVER
  /// overwrite the upper seven bits (geometry type, surface sorting key), though they may overwrite
  /// Source Effect and Surface Pointer if desired.
  /// 
  /// Do not use this functionality to perform frame-by-frame sorting (e.g. front to back).
  VISION_APIFUNC virtual void ComputeSortingKey();

#ifdef SUPPORTS_SNAPSHOT_CREATION
  /// \brief
  ///   Virtual function returning the resource dependencies for this static geometry instance.
  VISION_APIFUNC virtual void GetDependencies(VResourceSnapshot &snapshot);
#endif

  /// \brief
  ///   Overridable for profiling purposes (used for displaying in
  ///   DEBUGRENDERFLAG_OBJECT_TRIANGLECOUNT mode). Specifies the number of primitives to be
  ///   rendered
  VISION_APIFUNC virtual int GetNumPrimitives() {return -1;}

  /// \brief
  ///   Overridable for accessing the supported vertex stream mask. This function is not used for rendering
  VISION_APIFUNC virtual int GetSupportedStreamMask() {return 0;}

  ///
  /// @}
  ///

  ///
  /// @name Setters and Getters For Common Properties
  /// @{
  ///

  /// \brief
  ///   Empty overridable to render the collision mesh in line mode
  VISION_APIFUNC virtual void DebugRenderCollisionMesh(VColorRef iMeshColor, VColorRef iNormalColor, float fNormalLength) {}



  /// \brief
  ///   Assigns a visibility object to this geometry instance.
  ///
  ///The assigned object will additionally filter out geometry instances in the visibility collector. See implementation in
  ///VisionVisibilityCollector_cl::PostProcessVisibilityResults().
  ///There can only be one visibility object assigned. By default no visibility object is assigned so that frustum/portal culling
  ///is applied. Visibility objects can be assigned visually inside vForge.
  ///
  /// \param pVisObj
  ///   Visibility object to assign. Can be NULL to remove the reference
  void SetVisibilityObject(VisVisibilityObject_cl *pVisObj)
  { 
    m_spVisObject = pVisObj;
  }


  /// \brief
  ///   Returns the visibility object that has been set via SetVisibilityObject
  VisVisibilityObject_cl *GetVisibilityObject() const
  { 
    return m_spVisObject;
  }

  /// \brief
  ///   Sets the light influence bitmask for this geometry instance. This can be used for
  ///   fine-grained control regarding which scene elements are affected by which light source.
  inline void SetLightInfluenceBitmask(unsigned short iLightMask) { m_iLightMask=iLightMask; }

  /// \brief
  ///   Returns the previously set light influence bitmask for this geometry instance.
  inline unsigned short GetLightInfluenceBitmask() const { return m_iLightMask; }

  /// \brief
  ///   Sets the trace bitmask for this geometry instance. This can be used for fine-grained
  ///   control regarding which dynamic scene elements from static geometry instance are traced.
  inline void SetTraceBitmask(unsigned short iTraceMask) { m_iTraceMask=iTraceMask; }

  /// \brief
  ///   Returns the previously set trace bitmask for this geometry instance.
  inline unsigned short GetTraceBitmask() const { return m_iTraceMask; }

  /// \brief
  ///   Specifies whether this geometry instance casts dynamic (stencil) shadows.
  inline void SetCastDynamicShadows(bool bStatus) {m_bCastDynamicShadows=bStatus;}

  /// \brief
  ///   Returns whether this static geometry instance casts dynamic (stencil) shadows.
  inline bool GetCastDynamicShadows() const {return m_bCastDynamicShadows;}

  /// \brief
  ///   Returns the sorting key of this static geometry instance previously computed by
  ///   ComputeSortingKey.
  inline int GetSortingKey() const { return m_iSortingKey; }

  /// \brief
  ///   Returns this static geometry instance's geometry type.
  /// 
  /// For legal values, see the VisStaticGeometryType_e enumeration.
  /// 
  /// The m_eGeometryType member should be set inside the constructor/initializer of custom
  /// subclasses of the VisStaticGeometryInstance_cl class.
  /// 
  /// \return
  ///   VisStaticGeometryType_e: Geometry type of this static mesh instance.
  inline VisStaticGeometryType_e GetGeometryType() const { return m_eGeometryType; }

  /// \brief
  ///   Sets the surface (material) of this static geometry instance. If the material is
  ///   translucent, the translucent flag is set as well.
  VISION_APIFUNC void SetSurface(VisSurface_cl *pSurface);

  /// \brief
  ///   Returns the surface (material) of this static geometry instance.
  inline VisSurface_cl *GetSurface() const { return m_pSurface; }

  /// \brief
  ///   Assign a custom uder ID to this instance. This value is ignored by the engine but it is serialized to VArchive in SerializeX
  inline void SetCustomID(__int64 iID) {m_iCustomID=iID;}

  /// \brief
  ///   Returns the value that has been set with SetCustomID. The default value is 0
  inline __int64 GetCustomID() const {return m_iCustomID;}

  ///
  /// @}
  ///

  ///
  /// @name Visibility-related Functionality
  /// @{
  ///

  /// \brief
  ///   Removes this static mesh instance from all visibility zones. Note that this operation can
  ///   consume some time.
  VISION_APIFUNC void RemoveFromAllVisibilityZones();

  ///
  /// @}
  ///

  ///
  /// @name Streaming Zones
  /// @{
  ///

  /// \brief
  ///   Overridable to return the owner zone. This default implementation returns NULL. However,
  ///   the VisStaticSubmeshInstance_cl::GetParentZone override returns the parent zone of the
  ///   static mesh instance
  VISION_APIFUNC virtual VisZoneResource_cl *GetParentZone() 
  {
    return NULL;
  }

  ///
  /// @}
  ///

  ///
  /// @name Tagging
  /// @{
  ///

  /// \brief
  ///   Tags this static mesh instance.
  inline void Tag() { m_iTagged = m_iTagCtr; }

  /// \brief
  ///   Returns true if this mesh instance is currently tagged.
  inline bool IsTagged() { return (m_iTagged == m_iTagCtr); }

  /// \brief
  ///   Resets the tagged states for all static geometry instances.
  static inline void ResetTags() { m_iTagCtr++; }
 
  /// \brief
  ///   Returns the current global tag index that is used. It is not increased.
  static inline unsigned int GetCurrentGlobalTag() {return m_iTagCtr;}

  ///
  /// @}
  ///

  ///
  /// @name Lightmapping/Lightmask-related Functionality
  /// @{
  ///

  /// \brief
  ///   Internal function called by vForge.
  VISION_APIFUNC virtual bool ApplyLightmapInfo(VLightmapSceneInfo &sceneInfo, VLightmapPrimitive *pPrim, int iMaterialIndex);

  /// \brief
  ///   Internal function called by vForge.
  VISION_APIFUNC virtual void ResetStaticLighting();

  /// \brief
  ///   Sets the lightmap texture for this geometry index. The iSubIndex parameter is used to
  ///   specify the index of the lightmap for dot3 lightmapping (0=regular lightmap, 1/2/3=texture
  ///   channels for dot3 lightmapping).
  inline void SetLightmapTexture(VTextureObject *pTex, int iSubIndex) { VASSERT(iSubIndex>=0 && iSubIndex<4); m_spLightmapTexture[iSubIndex]=pTex; }

  /// \brief
  ///   Returns the lightmap texture previously set with SetLightmapTexture.
  inline VTextureObject* GetLightmapTexture(int iSubIndex) const { VASSERT(iSubIndex>=0 && iSubIndex<4); return  m_spLightmapTexture[iSubIndex]; }

  /// \brief
  ///   Sets the scaling and offset for this static geometry instance on the lightmap texture.
  inline void SetLightmapScaleOffset(const hkvVec4& vScaleOffs) { m_vLightmapScaleOffs = vScaleOffs; }

  /// \brief
  ///   Returns the lightmap scaling and offset parameters previously set with
  ///   SetLightmapScaleOffset.
  inline const hkvVec4& GetLightmapScaleOffset() const { return m_vLightmapScaleOffs; }

  /// \brief
  ///   Returns the number of lightmask entries for this instance, i.e. the number of modifiable
  ///   light sources that affect this instance
  inline int GetNumLightMaskEntries() const { return m_iNumLightMaskEntries; }

  /// \brief
  ///   Returns the lightmask entry with specified index (must be in range
  ///   [0..GetNumLightMaskEntries()-1])
  inline VisLightMask_cl &GetLightMaskEntry(int iIndex) const 
  { 
    VASSERT(iIndex>=0 && iIndex<m_iNumLightMaskEntries);
    return m_pLightMaskEntries[iIndex];
  }

  /// \brief
  ///   Returns the lightmask entry that represents the light source. The return value can be NULL
  inline VisLightMask_cl *GetLightMaskEntry(const VisLightSource_cl *pLight) const
  {
    VisLightMask_cl *pSM = m_pLightMaskEntries;
    for (int i=0;i<m_iNumLightMaskEntries;i++,pSM++)
    {
      if (pSM->m_pLight==pLight)
        return pSM;
    }
    return NULL;
  }


  ///
  /// @}
  ///

  ///
  /// @name Static Callbacks
  /// @{
  ///

  /// \brief
  ///   Static callback that gets triggered inside the constructor of a VisStaticGeometryInstance_cl instance.
  ///   The data object can be casted to VisStaticGeometryInstanceObject_cl which contains the instance pointer.
  VISION_APIDATA static VisCallback_cl OnStaticGeometryInstanceCreated;

  /// \brief
  ///   Static callback that gets triggered inside the destructor of a VisStaticGeometryInstance_cl instance.
  ///   The data object can be casted to VisStaticGeometryInstanceObject_cl which contains the instance pointer.
  VISION_APIDATA static VisCallback_cl OnStaticGeometryInstanceDestroyed;

  ///
  /// @}
  ///


  // INTERNAL:
  VISION_APIFUNC void SerializeX( VArchive &ar );

  unsigned int m_iTraceCount;                   ///< Internal Tracing Operations

  inline static unsigned int GetVisDataOffset() { return offsetof(VisStaticGeometryInstance_cl, m_BoundingBox); }

  inline int GetVisibilityZoneAssignmentCount() const {return m_iVisibilityZoneAssignmentCount;}
  VISION_APIFUNC bool IsAssignedToVisibilityZone(VisVisibilityZone_cl *pZone);
  VISION_APIFUNC void AddVisibilityZone(VisVisibilityZone_cl *pZone);
  VISION_APIFUNC void RemoveVisibilityZone(VisVisibilityZone_cl *pZone);
  inline VisVisibilityZone_cl *GetVisibilityZone(int iIndex) { 
    VASSERT(iIndex < m_iVisibilityZoneAssignmentCount);
    if (iIndex==0)
      return m_pVisibilityZone;
    else
      return m_pAdditionalVisZones[iIndex-1];
  }

  VISION_APIFUNC void DebugShowObjectRenderOrder();

  VISION_APIFUNC void DebugShowMaterialShader();

protected:
  friend class VisVisibilityZone_cl;
  friend class VisStateHandler_cl;
  friend class VisionVisibilityCollector_cl;

  VISION_APIFUNC void SetPassType(VisSurface_cl& surface);

  VisStaticGeometryType_e m_eGeometryType;

  VisSurface_cl *m_pSurface;
  unsigned short m_iLightMask;
  unsigned short m_iTraceMask;
  
  unsigned int m_iTagged;

  unsigned int m_iSortingKey;

  VISION_APIDATA static unsigned int m_iTagCtr;

  VisVisibilityObjectPtr m_spVisObject; ///< visibility object
  VisVisibilityZone_cl *m_pVisibilityZone;
  DynArray_cl<VisVisibilityZone_cl *>m_pAdditionalVisZones;

  // Lightmap/LightMask information
  hkvVec4 m_vLightmapScaleOffs;
  VTextureObjectPtr m_spLightmapTexture[4]; ///< diffuse and 3 dot3 lightmaps
  bool m_bCastDynamicShadows,m_bCastStaticShadows;
  short m_iVisibilityZoneAssignmentCount;
  short m_iNumLightMaskEntries;
  VisLightMask_cl *m_pLightMaskEntries;
  __int64 m_iCustomID;

  ///
  /// @}
  ///

};


VISION_ELEMENTMANAGER_TEMPLATE_DECL(VisStaticGeometryInstance_cl)



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
