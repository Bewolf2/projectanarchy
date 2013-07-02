/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VWallmarkManager.hpp

#ifndef VWALLMARKMANAGER_HPP_INCLUDED
#define VWALLMARKMANAGER_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/EffectsModule.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/Projector.hpp>


class VParticleWallmarkGroup;

/// \brief
///   Class that represents a single particle-based wallmark.
///
/// Instances must be created through VWallmarkManager::CreateWallmark.
class VParticleWallmark : public Particle_t
{
public:
  inline bool IsValid() const {return valid!=0;}

  /// \brief
  ///   Removes this wallmark instance. After that, the instance pointer should not be used anymore.
  EFFECTS_IMPEXP void Remove();

  /// \brief
  ///   Returns the current color including the current fade value in the alpha channel.
  inline VColorRef GetColor() const
  {
    return color;
  }

  /// \brief
  ///   Sets a new wallmark color.
  inline void SetColor(VColorRef col)
  {
    color = col;
    velocity[2] = (float)color.a; ///< original alpha (255 based)
  }

  /// \brief
  ///   Returns the remaining wallmark lifetime in seconds.
  inline float GetRemainingLifeTime() const
  {
    return velocity[0];
  }

  /// \brief
  ///   Sets the remaining wallmark lifetime in seconds.
  inline void SetRemainingLifeTime(float fTime)
  {
    velocity[0] = fTime;
  }

  /// \brief
  ///   Gets the current world space position of the wallmark.
  inline hkvVec3 GetPosition() const
  {
    return hkvVec3 (pos[0],pos[1],pos[2]);
  }

  /// \brief
  ///   Returns the normal vector (normalized).
  inline hkvVec3 GetNormal() const
  {
    return hkvVec3 (normal[0],normal[1],normal[2]);
  }

  /// \brief
  ///   Called by the simulation code to perform the fading.
  void TickFunction(float dtime);

  /// \brief
  ///   Internal function to add the particle's world space bounding box to the passed bounding box.
  void InflateBoundingBox(hkvAlignedBBox &bbox);

  VParticleWallmarkGroup *m_pOwner; ///< The owner wallmark group.
private:

  // prevent direct creation
  VParticleWallmark() {}
  ~VParticleWallmark() {}
};



#define MAX_WALLMARK_CACHE_SIZE     64
#define INITIAL_WALLMARKS_PERGROUP  64

/// \brief
///   Internal class that holds a collection of wallmark particles.
class VParticleWallmarkGroup : public VisParticleGroup_cl
{
public:
  VParticleWallmarkGroup(int iCount, VTextureObject *pTexture, VIS_TransparencyType eBlending, bool bApplyDeferredLighting);
  VParticleWallmark* TryGetFreeParticle();

  void TickFunction(float dtime);
  inline bool HasFreeParticles()
  {
    if (m_iCacheCount==0)
      FillCache();
    return m_bHasFreeParticles;
  }

protected:
  friend class VParticleWallmark;
  friend class VWallmarkManager;
  void FillCache();
  void RemoveWallmark(VParticleWallmark *p);
  void RecomputeBoundingBox();

  short m_iCacheCount;
  bool m_bHasFreeParticles, m_bBBoxValid, m_bApplyDeferredLighting;
  short m_iCacheIndex[MAX_WALLMARK_CACHE_SIZE];
  hkvAlignedBBox m_BoundingBox;
  VisVisibilityObjectAABoxPtr m_spVisObj;
};


/// \brief
///   Global wallmark manager that handles both particle based wallmarks and projected wallmarks.
///
/// Wallmark related functions are piped through one global instance of this manager that can be accessed via
/// VWallmarkManager::GlobalManager().
class VWallmarkManager : public IVisCallbackHandler_cl
{
public:
  VWallmarkManager();

  /// \brief
  ///   Static function to access the global wallmark manager.
  static EFFECTS_IMPEXP VWallmarkManager& GlobalManager();

  ///
  /// @name Particle wallmarks
  /// @{
 

  /// \brief
  ///   Static helper function that can be used to retrieve best alignment for particle based wallmarks.
  ///
  /// The parameter created via this function are compatible with the input values for CreateWallmark.
  /// This function utilizes the Raycast methods on the installed physics module to perform the alignment.
  ///
  /// \param vCenter
  ///   The wanted center of the particle wallmark.
  ///
  /// \param vNormal
  ///   The wanted normal of the particle wallmark.
  ///
  /// \param fSize
  ///   The wanted size of the particle wallmark.
  ///
  /// \param fRotation
  ///   The wanted rotation (in degrees) of the particle wallmark.
  ///
  /// \param vNewCenter
  ///   If successful this value receives the new position that is aligned to the geometry.
  ///
  /// \param alignment
  ///   If successful this value receives the new rotation. This includes the rotation angle and also the size (scaling).
  ///
  /// \param fEpsilon
  ///   Epsilon tolerance for alignment. If no geometry is inside this tolerance the function returns FALSE.
  ///
  /// \return
  ///   TRUE if the new alignment parameters are valid.
  static EFFECTS_IMPEXP BOOL TryAlignWallmark(const hkvVec3& vCenter, const hkvVec3& vNormal, float fSize, float fRotation, hkvVec3& vNewCenter, hkvMat3& alignment, float fEpsilon = 0.1f);

  /// \brief
  ///   Creates a new particle wallmark at specified position/orientation.
  ///
  /// This function takes the position and orientation as-is with no additional validation whether the transformation
  //  aligns properly with the geometry. The static TryAlignWallmark function can be used get proper transformation.
  ///
  /// \param pTexture
  ///   The texture object that should be used for the wallmark.
  ///
  /// \param eBlending
  ///   The blend mode (e.g. VIS_TRANSP_ALPHA).
  ///
  /// \param vCenter
  ///   The actual particle center.
  ///
  /// \param alignment
  ///   The actual alignment matrix (encodes rotation and size of the wallmark as scaling).
  ///
  /// \param color
  ///   The initial modulation color.
  ///
  /// \param fLifetime
  ///   The lifetime in seconds. A value <0 can be passed for infinite lifetimes.
  ///
  /// \param fFadeOutTime
  ///   Additional time to fade out the particle (on top of fLifetime). If fLifetime<0 this value should be 0.
  ///
  /// \param bApplyDeferredLighting
  ///   If true, the particle will be part of the G-buffer so it will be affected by dynamic lighting. Must be false for all non-deferred modes.
  ///
  /// \return
  ///   A new particle wallmark instance. This instance can be used to modify the wallmark.
  EFFECTS_IMPEXP VParticleWallmark* CreateWallmark (VTextureObject *pTexture, VIS_TransparencyType eBlending, 
    const hkvVec3& vCenter, const hkvMat3 &alignment, VColorRef color, float fLifetime, float fFadeOutTime, bool bApplyDeferredLighting=false);

  ///
  /// @}
  ///

  ///
  /// @name Projected wallmark
  /// @{


  /// \brief
  ///   Creates a new projected wallmark at specified position/orientation.
  ///
  /// This wallmark type renders the affected geometry again using a projection shader. This is very similar to
  /// projected dynamic lights. Accordingly this type is quite performance critical when many instances are used.
  /// The VProjectedWallmark instances correspond to the projector shape in vForge.
  ///
  /// \param vDestPos
  ///   Projection center (i.e. target position on the geometry)
  ///
  /// \param vOrigin
  ///   Projection origin
  ///
  /// \param radius
  ///   Radius of the wallmark at the target position
  ///
  /// \param depth
  ///   Fade out depth on the direction axis vDestPos-vOrigin. Usually same as radius
  ///
  /// \param pTexture
  ///   Texture to use for the projection
  ///
  /// \param drawtype
  ///   Blend mode, e.g. VIS_TRANSP_ALPHA
  ///
  /// \param iColor
  ///   Modulation color
  ///
  /// \param rotation
  ///   Rotation in degree around the vDestPos-vOrigin axis
  ///
  /// \param fLifeTime
  ///   Life time in seconds. Values <0 can be used for infinite lifetimes.
  ///
  /// \param fFadeOutTime
  ///   Additional fade out time. If fLifeTime<0 then this value should be 0.
  ///
  /// \return
  ///   Instance that represents the projected wallmark.
  EFFECTS_IMPEXP VProjectedWallmark *CreateProjectedWallmark(const hkvVec3& vDestPos, const hkvVec3& vOrigin, float radius, float depth, VTextureObject* pTexture, VIS_TransparencyType drawtype, VColorRef iColor, float rotation, float fLifeTime, float fFadeOutTime);

  /// \brief
  ///   Helper function to retrieve a prepared shader pass for given projected wallmark. This shader can be used for rendering.
  EFFECTS_IMPEXP VProjectorShaderPass *GetWallmarkShader(const VProjectedWallmark *pWallmark, VisStaticGeometryType_e eGeomType);

  /// \brief
  ///   Internal function to render the wallmarks. Gets called automatically.
  EFFECTS_IMPEXP void RenderProjectedWallmarks(VPassType_e ePassType = VPT_TransparentPass);

  ///
  /// @}
  ///


  ///
  /// @name Access collections
  /// @{


  /// \brief
  ///   Access the collection of particle groups used for particle based wallmarks. Each group instance in this collection can hold a large number of particle wallmarks.
  EFFECTS_IMPEXP VRefCountedCollection<VParticleWallmarkGroup>& ParticleGroupInstances();

  /// \brief
  ///   Access the collection of all active projected wallmarks
  EFFECTS_IMPEXP VRefCountedCollection<VProjectedWallmark>& ProjectedWallmarkInstances();

  ///
  /// @}
  ///
  EFFECTS_IMPEXP void OneTimeInit();
  EFFECTS_IMPEXP void OneTimeDeInit();
  EFFECTS_IMPEXP virtual void OnHandleCallback(IVisCallbackDataObject_cl *pData);

  ///
  /// @name Getters and setters
  /// @{
  /// \brief Sets the render order for the transparent pass type in the OnRenderHook handle.
  EFFECTS_IMPEXP void SetTransparentPassRenderOrder(int iOrder){ m_iTransparentPassRenderOrder=iOrder; }
  /// \brief Sets the render order for the primary opaque pass type in the OnRenderHook handle.
  EFFECTS_IMPEXP void SetPrimaryOpaquePassRenderOrder(int iOrder){ m_iPrimaryOpaquePassRenderOrder=iOrder; }
  /// \brief Sets the render order for the secondary opaque pass type in the OnRenderHook handle
  EFFECTS_IMPEXP void SetSecondaryOpaquePassRenderOrder(int iOrder){ m_iSecondaryOpaquePassRenderOrder=iOrder; }
  /// \brief Gets the current render order for the transparent pass type in the OnRenderHook handle.
  EFFECTS_IMPEXP int GetTransparentPassRenderOrder(){ return m_iTransparentPassRenderOrder; }
  /// \brief Gets the current render order for the primary opaque pass type in the OnRenderHook handle.
  EFFECTS_IMPEXP int GetPrimaryOpaquePassRenderOrder(){ return m_iPrimaryOpaquePassRenderOrder; }
  /// \brief Gets the current render order for the secondary opaque pass type in the OnRenderHook handle.
  EFFECTS_IMPEXP int GetSecondaryOpaquePassRenderOrder(){ return m_iSecondaryOpaquePassRenderOrder; }
  /// @}
private:
  
  inline void EnsureSimulationCallbackRegistered()
  {
    if (!m_bSimulationCallbackRegistered)
    {
      Vision::Callbacks.OnUpdateSceneFinished += this;
      m_bSimulationCallbackRegistered = true;
    }
    EnsureUnloadWorldCallbackRegistered();
  }
  inline void EnsureRenderCallbackRegistered()
  {
    if (!m_bRenderCallbackRegistered)
    {
      Vision::Callbacks.OnRenderHook += this;
      VisZoneResourceManager_cl::GlobalManager().OnResourceChanged += this;
      VisStaticGeometryInstance_cl::OnStaticGeometryInstanceDestroyed += this;
      m_bRenderCallbackRegistered = true;
    }
    EnsureUnloadWorldCallbackRegistered();
  }
  inline void EnsureUnloadWorldCallbackRegistered()
  {
    if (!m_bUnloadWorldCallbackRegistered)
    {
      Vision::Callbacks.OnWorldDeInit += this;
      m_bUnloadWorldCallbackRegistered = true;
    }
  }

  friend class VProjectedWallmark;
  static bool IsTracePointOnPlane(const hkvVec3& vPos, const hkvVec3& vNormal, float fTraceRad, float fEpsilon, hkvVec3& vNewNormal);
  VParticleWallmark* CreateParticle(VTextureObject *pTexture, VIS_TransparencyType eBlending, bool bApplyDeferredLighting, const hkvVec3& vCenter, bool bCheckBest);
  VRefCountedCollection<VParticleWallmarkGroup> m_AllWallmarkGroups;

  static inline __int64 GetGeomHashMask(VisStaticGeometryInstance_cl *pGeom)
  {
#if defined (WIN32)  || defined (_VISION_XENON)   || defined (_VISION_PS3)   || defined (_VISION_PSP2) 
    __int64 val = (__int64)(unsigned __int64)(ULONG)pGeom; // the cast avoids C4826, see http://msdn.microsoft.com/en-us/library/ms235307.aspx
#elif defined(_VISION_POSIX)  || defined(_VISION_WIIU)
    __int64 val = (uint64_t)pGeom;

#else
  #error Undefined platform!
#endif
    return (__int64)1<<(val%63);
  }
  void OnZoneLoaded(VisZoneResource_cl *pZone);
  void OnZoneUnloading(VisZoneResource_cl *pZone);
  void OnStaticGeometryDeleted(VisStaticGeometryInstance_cl *pGeom);

  void DeleteWallmarkShaders();

  VRefCountedCollection<VProjectedWallmark> m_AllProjectedWallmarks;
  VRefCountedCollection<VProjectedWallmark> m_FadingProjectedWallmarks;

  static VWallmarkManager g_WallmarkManager;

  VCompiledTechniquePtr m_spWallmarkTechnique[STATIC_GEOMETRY_TYPE_FIRSTCUSTOM][2]; ///< array for every engine type and then 0:fullbright, 1:lightmapped

  VisStaticGeometryInstanceCollection_cl m_TempGeoInstanceCollection;
  VisEntityCollection_cl m_TempEntityCollection;

  bool m_bSimulationCallbackRegistered,m_bRenderCallbackRegistered, m_bUnloadWorldCallbackRegistered;
  UBYTE m_eProjectedWMPassTypes;
  __int64 m_iGeomRefHashMask;
  int m_iPrimaryOpaquePassRenderOrder;
  int m_iSecondaryOpaquePassRenderOrder;
  int m_iTransparentPassRenderOrder;

  static int PROFILING_WALLMARK_CREATION;
  static int PROFILING_WALLMARK_SIMULATION;
  static int PROFILING_WALLMARK_RENDERING;
  static int PROFILING_WALLMARK_CLEANUP;
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
