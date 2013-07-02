/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file Projector.hpp

#ifndef VPROJECTOR_HPP_INCLUDED
#define VPROJECTOR_HPP_INCLUDED

class VProjectedWallmark;
typedef VProjectedWallmark VisProjectedWallmark_cl;


#define PROJECTOR_AFFECTS_STATICMESHES  (1<<STATIC_GEOMETRY_TYPE_MESHINSTANCE)
#define PROJECTOR_AFFECTS_TERRAIN       (1<<STATIC_GEOMETRY_TYPE_TERRAIN)
#define PROJECTOR_AFFECTS_ENTITIES      (V_BIT(0))
#define PROJECTOR_AFFECTS_DEFAULT      (PROJECTOR_AFFECTS_STATICMESHES|PROJECTOR_AFFECTS_TERRAIN)


/// \brief
///   Class that represents a projected wallmark.
/// 
/// Projected wallmarks perform a real projection similar to a spotlight.
/// 
/// Projected wallmarks look significantly better than particle based wallmarks, but they are by
/// far more performance critical, since affected geometry is to be rendered again for each
/// projector.
/// 
/// Projected wallmarks are provided in vForge through the "Projector" shape.
class VProjectedWallmark : public VisTypedEngineObject_cl, public VRefCounter
{

public:
  ///
  /// @name Constructor
  /// @{
  ///

  /// \brief
  ///   Component destructor
  virtual ~VProjectedWallmark();

  ///
  /// @}
  ///


  ///
  /// @name Base Properties
  /// @{
  ///

  /// \brief
  ///   Overridden function
  inline void SetColor(VColorRef iColor)
  {
    m_iColor = iColor;
    m_fInitialAlpha = (float)m_iColor.a;
  }


  /// \brief
  ///   Overridden function
  inline hkvVec3 GetCenterPos() const
  {
    return m_vDestPos;
  }

  /// \brief
  ///   Overridden function
  inline hkvVec3 GetNormal() const
  {
    return -m_vDirection;
  }

  /// \brief
  ///   Overridden function
  inline VColorRef GetColor() const
  {
    return m_iColor;
  }

  /// \brief
  ///   Overridden function
  inline VIS_TransparencyType GetTransparencyType() const
  {
    return m_eTransp;
  }

  /// \brief
  ///   Overridden function
  inline BOOL IsFromFile() const
  {
    return m_bFromFile;
  }

  /// \brief
  ///   Indicates whether this wallmark should use lightmapping
  inline BOOL IsLightmapped() const
  {
    return m_bLightmapped;
  }

  /// \brief
  ///   Changes the lightmapping status of this projected wallmark
  inline void SetLightmapped(BOOL bStatus)
  {
    m_bLightmapped = bStatus==TRUE;
  }

  ///
  /// @}
  ///

  ///
  /// @name Rendering Helpers
  /// @{
  ///

  /// \brief
  ///   Defines a filter mask that determines which geometry types are affected by the projector
  ///
  /// By default a projector affects only static geometry (including terrain), but not entities.
  ///
  /// \param iMask
  ///   New filter mask. The following constants can be combined PROJECTOR_AFFECTS_STATICMESHES, PROJECTOR_AFFECTS_TERRAIN, PROJECTOR_AFFECTS_ENTITIES
  inline void SetGeometryTypeFilterMask(int iMask)
  {
    if (m_iGeometryTypeFilter==iMask)
      return;
    m_iGeometryTypeFilter = iMask;
    m_bPrimDirty = true;
  }

  /// \brief
  ///   Returns the filter mask previously set via SetGeometryTypeFilterMask
  inline int GetGeometryTypeFilterMask() const
  {
    return m_iGeometryTypeFilter;
  }

  /// \brief
  ///   Returns the collection of affected world static geometry instances
  inline const VisStaticGeometryInstanceCollection_cl& GetStaticGeometryCollection() const
  {
    return m_GeoInstanceList;
  }
  

  /// \brief
  ///   Has to be called before rendering to ensure the primitives are valid. Used internally.
  EFFECTS_IMPEXP void PrepareForRendering();

  /// \brief
  ///   Fills the passed plane references with the projection planes (q is normalized, a and b are
  ///   scaled according to cone angles). Used internally.
  EFFECTS_IMPEXP void GetProjectionPlanes(hkvPlane& s, hkvPlane& t, hkvPlane& q, hkvPlane& att) const;


  /// \brief
  ///   Handles lifetime etc. A return value of TRUE indicates that the wallmark is to be disposed
  EFFECTS_IMPEXP BOOL TickFunction(float dtime);

  ///
  /// @}
  ///

  ///
  /// @name Projector Specific
  /// @{
  ///

  /// \brief
  ///   Sets the visible bitmask for this instance. This bitmask is tested against the context's
  ///   render filter mask.
  inline void SetVisibleBitmask(unsigned int iMask)
  {
    m_iVisibleBitmask = iMask;
  }

  /// \brief
  ///   Returns the visible bitmask that has ben set via SetVisibleBitmask. The default mask is
  ///   0xffffffff, thus rendered in all contexts.
  inline unsigned int GetVisibleBitmask() const
  {
    return m_iVisibleBitmask;
  }

  /// \brief
  ///   Sets the affect meshes bitmask for this instance. This bitmask is tested against the meshes visible mask.
  inline void SetInfluenceBitmask(unsigned int iMask)
  {
    m_InfluenceBitmask = iMask;
    m_bPrimDirty = true;
  }

  /// \brief
  ///   Returns the visible bitmask that has ben set via SetVisibleBitmask. The default mask is
  ///   0xffffffff, thus rendered in all meshes.
  inline unsigned int GetInfluenceBitmask() const
  {
    return m_InfluenceBitmask;
  }

  /// \brief
  ///   Sets a clip distance in units at which the wallmark is not rendered anymore. The default value is <0.f which means no clipping
  inline void SetFarClipDistance(float fDistance)
  {
    m_fFarClipDistance = fDistance;
  }

  /// \brief
  ///   Returns the clip distance that has been set via SetFarClipDistance
  inline float GetFarClipDistance() const
  {
    return m_fFarClipDistance;
  }


  /// \brief
  ///   Sets the origin of the projector
  EFFECTS_IMPEXP void SetPosition(const hkvVec3& vPos);

  /// \brief
  ///   Sets orientation of the projector, i.e, the cone direction.
  EFFECTS_IMPEXP void SetOrientation(const hkvMat3 &vMat);

  /// \brief
  ///   Sets the cone opening angles (in degrees)
  EFFECTS_IMPEXP void SetConeAngles(float fAngleX, float fAngleY);

  /// \brief
  ///   Sets the radius
  ///
  /// \param fRadius
  ///   Raidus of the projector.
  ///
  /// \sa VWallmarkManager::CreateProjectedWallmark
  EFFECTS_IMPEXP void SetRadius(float fRadius);

  /// \brief
  ///   Sets the projected texture file
  EFFECTS_IMPEXP VTextureObject* SetTexture(const char *szFilename);

  /// \brief
  ///   Sets the texture object used for the projection.
  EFFECTS_IMPEXP void SetTexture(VTextureObject* pTexture);

  /// \brief
  ///   Returns the texture object used for the projection.
  EFFECTS_IMPEXP VTextureObject* GetCurrentTexture() const;

  /// \brief
  ///   Sets the transparency type
  inline void SetTransparency(VIS_TransparencyType eTransp)
  {
    m_eTransp=eTransp;
  }

  /// \brief
  ///   Assigns a custom shader effect to this projector which is used to render affected geometry
  ///
  /// Custom projector shaders can be written in the HLSL shader editor of vForge or created with the VSE. When manually created, each 
  /// shader pass in the techniques must be of class VProjectorShaderPass, otherwise this function will reject the effect.
  /// The passed effect can have a sub-technique with inclusion tag 'VTerrain' which specifies a different technique that is used for 
  /// terrain rendering.
  ///
  /// \param pFX
  ///   The new effect. Can be NULL to use default projector shaders
  ///
  /// \return
  ///   true, if the new effect was accepted, or false if it is invalid.
  EFFECTS_IMPEXP bool SetCustomShaderEffect(VCompiledEffect *pFX);

  /// \brief
  ///   Returns ther custom shader technique associated with the passed geometry type. The return value is NULL unless a custom effect has been assigned through SetCustomShaderEffect
  inline VCompiledTechnique* GetCustomShaderEffect(VisStaticGeometryType_e eGeomType) const
  {
    return m_spCustomTechnique[(int)eGeomType];
  }


  /// \brief
  ///   Sets the distance to the projection target. The affected range is GetLength() +/-
  ///   GetFadeOutRange()
  EFFECTS_IMPEXP void SetLength(float fLen);

  /// \brief
  ///   Returns the length (set via SetLength)
  EFFECTS_IMPEXP float GetLength() const;

  /// \brief
  ///   Sets the radius around the target position to fade out the projection
  EFFECTS_IMPEXP void SetFadeOutRange(float fDist);

  /// \brief
  ///   Returns the range set via SetFadeOutRange
  inline float GetFadeOutRange() const
  {
    return m_fDepth;
  }

  /// \brief
  ///   Returns the position of the origin
  inline const hkvVec3& GetPosition() const
  {
    return m_vOrigin;
  }

  /// \brief
  ///   Returns the normalized cone direction
  inline const hkvVec3& GetDirection() const
  {
    return m_vDirection;
  }

  /// \brief
  ///   Returns the normalized up direction of the cone rotation matrix
  inline const hkvVec3& GetDirection_Up() const
  {
    return m_vUpDir;
  }

  /// \brief
  ///   Returns the normalized right direction of the cone rotation matrix
  inline const hkvVec3& GetDirection_Right() const
  {
    return m_vRight;
  }
  
  /// \brief
  ///   Returns the bounding box of affected primitives
  inline const hkvAlignedBBox &GetBoundingBox() const
  {
    return m_BoundingBox;
  }

  ///
  /// @}
  ///

  ///
  /// @name Serialization
  /// @{
  ///

  /// \brief
  ///   Empty constructor for de-serialization
  EFFECTS_IMPEXP VProjectedWallmark();

  /// \brief
  ///   Fully serializes this projector instance from archive or to archive. For more information
  ///   about serialization, see the Vision API Documentation.
  EFFECTS_IMPEXP virtual void Serialize( VArchive& ar );

#ifdef SUPPORTS_SNAPSHOT_CREATION
  /// \brief
  ///   Overridden function that gets the resources used for this instance.
  EFFECTS_IMPEXP virtual void GetDependencies(VResourceSnapshot &snapshot);
#endif

  /// \brief
  ///   Overridden VisTypedEngineObject_cl function to remove this instance from the scene. Just
  ///   calls Remove
  EFFECTS_IMPEXP virtual void DisposeObject();


  V_DECLARE_SERIAL_DLLEXP( VProjectedWallmark, EFFECTS_IMPEXP )

  ///
  /// @}
  ///

private:
friend class VWallmarkManager;

  VProjectedWallmark(
    const hkvVec3& origin, 
    const hkvVec3& destpos, 
    const hkvVec3& up, 
    const hkvVec3& right, 
    float fConeFactorX, float fConeFactorY, float fDepth, 
    VTextureObject* pTexture, const char *szKey, 
    VIS_TransparencyType eTransp, 
    VColorRef iColor, float fLifeTime, float fFadeOutTime);



protected: ///< data
  friend class VProjectorShaderPass;

  float m_fLifeTime, m_fFadeOutTime;

  hkvVec3 m_vOrigin;         ///< origin
  hkvVec3 m_vDestPos;        ///< destination pos
  hkvVec3 m_vDirection;      ///< normalized direction
  hkvVec3 m_vUpDir, m_vRight; ///< remaining vectors of coordinate system

  float m_fDepth;                 ///< depth radius
  VTextureObjectPtr m_spTexture;  ///< wallmark texture
  VisTextureAnimInstancePtr m_spTexAnim;  ///< for animated textures
  VisStaticGeometryInstanceCollection_cl m_GeoInstanceList;
  VColorRef m_iColor;               ///< modulation color
  float m_fInitialAlpha;            ///< 255-based alpha value at creation time
  bool   m_bFromFile;               ///< wallmark is from file
  bool   m_bLightmapped;            ///< if set, use lightmapped shaders instead
  bool   m_bPrimDirty;              ///< internal flag to determine whether primitive refs must be updated
  UBYTE  m_ePassType;               ///< casted to VPassType_e
  VIS_TransparencyType m_eTransp;   ///< VIS_SurfaceManip_DrawType
  float m_fConeFactorX, m_fConeFactorY; ///< tan of cone angles
  unsigned int m_iVisibleBitmask;   ///< context visible bitmask
  unsigned int m_InfluenceBitmask;   ///< context visible bitmask
  hkvAlignedBBox m_BoundingBox;
  int m_iGeometryTypeFilter;        ///< bits in this member are of type 1<<VisStaticGeometryType_e
  float m_fFarClipDistance;

  VCompiledEffectPtr m_spCustomEffect;
  VCompiledTechniquePtr m_spCustomTechnique[STATIC_GEOMETRY_TYPE_FIRSTCUSTOM]; ///< array for every engine type
};




/// \brief
///   Shader class with projector update capabilities
class VProjectorShaderPass : public VCompiledShaderPass
{
public:
  VProjectorShaderPass()
  {
    m_iVSRegRefPlaneS = m_iVSRegRefPlaneT = m_iVSRegRefPlaneQ = m_iVSRegRefPlaneAtt = m_iPSRegColor = m_iVSRegColor = -1;
  }

  /// \brief
  ///   Updates the shader instance with projector specific properties
  void SetProperties(const VProjectedWallmark *pProjector);

  V_DECLARE_SERIAL_DLLEXP( VProjectorShaderPass, EFFECTS_IMPEXP )
protected:
  virtual void PostCompileFunction(VShaderEffectResource *pSourceFX,VShaderPassResource *pSourceShader);

  int m_iVSRegRefPlaneS,m_iVSRegRefPlaneT,m_iVSRegRefPlaneQ, m_iVSRegRefPlaneAtt, m_iVSRegColor, m_iPSRegColor;
  int m_iRegProjTexSampler;
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
