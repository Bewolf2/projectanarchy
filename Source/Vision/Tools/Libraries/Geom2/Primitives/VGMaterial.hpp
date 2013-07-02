/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#pragma once

class VGVisionImporterExporter;

/// \brief
///   Material class
///
/// Describes and sets up a material for usage and processing inside vGeom2.
class VGEOM2_IMPEXP_CLASS VGMaterial : public VGNameable
{
  friend class VGVisionImporterExporter;
public:
  /// \brief
  ///   Material map types
  ///
  /// Used to query different map types of the material
  enum MaterialMap
  {
    VGMM_DIFFUSE,
    VGMM_OPACITY,
    VGMM_SPECULAR,
    VGMM_NORMAL,
    VGMM_NUM          // Number of maps.
  };

  /// \brief
  ///   Material color types
  ///
  /// Used to query different color types of the material
  enum MaterialColor
  {
    VGMC_DIFFUSE,
    VGMC_AMBIENT,
    VGMC_EMISSIVE,
    VGMC_NUM          // Number of colors.
  };

  /// \brief
  ///   Material lighting types
  ///
  /// Used to specify different lighting types
  enum MaterialLighting
  {
    VGML_FULLBRIGHT,
    VGML_LIGHTMAP,
    VGML_LIGHTGRID,
    VGML_DYNAMIC_ONLY
  };

  /// \brief
  ///   Material flags
  ///
  /// Stores material specific flags, e.g. if maps should be clamped, etc.
  enum MaterialFlags
  {
    VGMF_NOTHING                   = 0,
    VGMF_DOUBLESIDED               = V_BIT(0),
    VGMF_CLAMP_U                   = V_BIT(1),
    VGMF_CLAMP_V                   = V_BIT(2),
    VGMF_FORCE_PRIMARYOPAQUEPASS   = V_BIT(3),
    VGMF_FORCE_TRANSPARENTPASS     = V_BIT(4),
    VGMF_FORCE_SECONDARYOPAQUEPASS   = V_BIT(5),
    VGMF_DISABLE_CAST_STATIC_SHADOWS = V_BIT(6),
    VGMF_DISABLE_DEPTH_WRITE         = V_BIT(7)
  };

  /// \brief
  ///   UV mapping transformation order (don't change values)
  ///
  /// S = scale, R = rotation, T = translation/offset
  enum UVMappingAlgorithm
  {
    VGMA_SRT          = 0,  // assumes pivot being in top-left for scale and offset, center for rotations, positive rotation is counter-clockwise
    VGMA_TRS          = 1,  // assumes pivot being in top-left for scale and offset, center for rotations, positive rotation is counter-clockwise
    VGMA_3DSMAX_STYLE = 2,  // assumes pivot being in center for scale, rotation and offset, positive rotation is clockwise, offsets are negated, order is SRT
    VGMA_MAYA_STYLE   = 3   // assumes pivot being in top-left for scale and offset, center for rotations, positive rotation is clockwise, order is @@@???RST???@@@
  };


public:
  VGMaterial();
  virtual ~VGMaterial();

  /// \brief
  ///   Returns whether the engine runtime relevant settings are equal. Settings that are only present in vForge (lightmapping etc) are not serialized
  ///   in the engine code and are ignored by the compare function.
  bool operator== (const VGMaterial& rhs) const;

  /// \brief
  ///   Gets the name of a map referenced by this material
  ///
  /// \param type
  ///   Type of map to return
  ///
  /// \return
  ///   Map's name
  inline const VString&            GetMapName(MaterialMap type) const throw()                 { VASSERT(VGMM_NUM); return m_maps[type]; }

  /// \brief
  ///   Sets the name of a map referenced by this material
  ///
  /// \param type
  ///   Type of map to assign a new name
  ///
  /// \param name
  ///   Map name to assign
  inline void                      SetMapName(MaterialMap type, const VString& name)          { VASSERT(VGMM_NUM); m_maps[type] = name; }



  /// \brief
  ///   Gets a list of auxiliary texture names referenced by this material
  ///
  /// \return
  ///   Auxiliary texture names
  inline       VStrList&           GetAuxiliaryTextures()                                     { return m_auxTextures; }
  inline const VStrList&           GetAuxiliaryTextures() const                               { return m_auxTextures; }

  /// \brief
  ///   Adds an auxiliary texture name to this material
  ///
  /// \param name
  ///   Auxiliary texture name to add to material
  inline void                      AddAuxiliaryTexture(const VString& name)                   { m_auxTextures.AddString(name); }



  /// \brief
  ///   Gets specular multiplier value
  ///
  /// \return
  ///   Specular multiplier value
  inline float                     GetSpecularMultiplier() const throw()                      { return m_specMul; }

  /// \brief
  ///   Gets specular exponent value
  ///
  /// \return
  ///   Specular exponent value
  inline float                     GetSpecularExponent() const throw()                        { return m_specExp; }

  /// \brief
  ///   Sets specular multiplier and exponent values
  ///
  /// \param mul
  ///   Value to set as specular multiplier
  ///
  /// \param exp
  ///   Value to set as specular exponent
  inline void                      SetSpecularParameters(float mul, float exp) throw()        { m_specMul = mul; m_specExp = exp; }



  /// \brief
  ///   Gets this material's type of transparency
  ///
  /// \return
  ///   Type of material's transparency
  inline VIS_TransparencyType      GetTransparencyType() const throw()                        { return m_transparency; }

  /// \brief
  ///   Sets this material's type of transparency
  ///
  /// \param tp
  ///   Transparency type to assign
  inline void                      SetTransparencyType(VIS_TransparencyType tp) throw()       { m_transparency = tp; }

  /// \brief
  ///   Returns the custom alpha threshold.
  inline float GetCustomAlphaThreshold() const throw() { return m_fCustomAlphaThreshold; }

  /// \brief
  ///   Sets the custom alpha threshold.
  inline void SetCustomAlphaThreshold(float fCustomAlphaThreshold) throw() { m_fCustomAlphaThreshold = fCustomAlphaThreshold; }

  /// \brief
  ///   Gets depth bias value
  ///
  /// \return
  ///   depth bias value
  inline float                     GetDepthBias() const throw()                               { return m_depthBias; }

  /// \brief
  ///   Sets depth bias value
  ///
  /// \param fBias
  ///   depth bias value
  inline void                      SetDepthBias(float fBias) throw()                          { m_depthBias = fBias; }

  /// \brief
  ///   Sets clamp value for the depth bias
  ///
  /// \param fClamp
  ///   clamp value
  inline void                      SetDepthBiasClamp(float fClamp) throw()                    { m_depthBiasClamp = fClamp; }

  /// \brief
  ///   Gets clamp value for the depth bias
  ///
  /// \return
  ///   depth bias clamp value
  inline float                     GetDepthBiasClamp() const throw()                          { return m_depthBiasClamp; }

  /// \brief
  ///   Sets the depth bias scaling for slopes
  ///
  /// \param fScale
  ///   clamp value
  inline void                      SetSlopeScaledDepthBias(float fScale) throw()              { m_slopeScaledDepthBias = fScale; }

  /// \brief
  ///   Gets the depth bias scaling for slopes
  ///
  /// \return
  ///   depth slope scaling value
  inline float                     GetSlopeScaledDepthBias() const throw()                    { return m_slopeScaledDepthBias; }

  /// \brief
  ///   Gets a color of this material, depending on the color's type
  ///
  /// \param type
  ///   Type of color to return
  ///
  /// \return
  ///   Color value referenced by type
  inline const hkvVec4&           GetColor(MaterialColor type) const                         { VASSERT(VGMC_NUM); return m_colors[type]; }

  /// \brief
  ///   Sets a color of this material, depending on the color's type
  ///
  /// \param type
  ///   Type of color to set
  ///
  /// \param color
  ///   Color value to set
  inline void                      SetColor(MaterialColor type, const hkvVec4& color)        { VASSERT(VGMC_NUM); m_colors[type] = color; }



  /// \brief
  ///   Gets mapping u-scale value (unit, pivot, etc. not specified, see VGProcessor_Baker and backend for Vision conventions)
  ///
  /// \return
  ///   Value of mapping's u-scale
  inline float                     GetMappingScaleU() const throw()                           { return m_scale.u; }

  /// \brief
  ///   Gets mapping v-scale value (unit, pivot, etc. not specified, see VGProcessor_Baker and backend for Vision conventions)
  ///
  /// \return
  ///   Value of mapping's v-scale
  inline float                     GetMappingScaleV() const throw()                           { return m_scale.v; }

  /// \brief
  ///   Sets mapping scale values (unit, pivot, etc. not specified, see VGProcessor_Baker and backend for Vision conventions)
  ///
  /// \param u
  ///   Value to set as mapping's u-scale
  ///
  /// \param v
  ///   Value to set as mapping's v-scale
  inline void                      SetMappingScale(float u, float v) throw()                  { m_scale.u = u; m_scale.v = v; }



  /// \brief
  ///   Gets mapping u-offset value (unit, pivot, etc. not specified, see VGProcessor_Baker and backend for Vision conventions)
  ///
  /// \return
  ///   Value of mapping's u-offset
  inline float                     GetMappingOffsetU() const throw()                          { return m_offset.u; }

  /// \brief
  ///   Gets mapping v-offset value (unit, pivot, etc. not specified, see VGProcessor_Baker and backend for Vision conventions)
  ///
  /// \return
  ///   Value of mapping's v-offset
  inline float                     GetMappingOffsetV() const throw()                          { return m_offset.v; }

  /// \brief
  ///   Sets mapping offset values (unit, pivot, etc. not specified, see VGProcessor_Baker and backend for Vision conventions)
  ///
  /// \param u
  ///   Value to set as mapping's u-offset
  ///
  /// \param v
  ///   Value to set as mapping's v-offset
  inline void                      SetMappingOffset(float u, float v) throw()                 { m_offset.u = u; m_offset.v = v; }



  /// \brief
  ///   Gets mapping rotation value (unit, pivot, etc. not specified, see VGProcessor_Baker and backend for Vision conventions)
  ///
  /// \return
  ///   Value of mapping's rotation (unit unspecified, backends use radians by convention)
  inline float                     GetMappingRotation() const throw()                         { return m_rotation; }

  /// \brief
  ///   Sets mapping rotation value (unit, pivot, etc. not specified, see VGProcessor_Baker and backend for Vision conventions)
  ///
  /// \param rot
  ///   Value to set as mapping's rotation (unit unspecified, backends use radians by convention)
  inline void                      SetMappingRotation(float rot) throw()                      { m_rotation = rot; }



  /// \brief
  ///   Gets mapping algorithm to use
  ///
  /// \return
  ///   Mapping algorithm
  inline UVMappingAlgorithm        GetMappingAlgorithm() const throw()                        { return m_ma; }

  /// \brief
  ///   Sets mapping algorithm to use
  ///
  /// \param ma
  ///   Value to set as mapping's rotation (unit unspecified, backends use radians by convention)
  inline void                      SetMappingAlgorithm(UVMappingAlgorithm ma) throw()         { m_ma = ma; }



  /// \brief
  ///   Gets parallax mapping scale value
  ///
  /// \return
  ///   Value of parallax mapping's scale
  inline float                     GetParallaxMappingScale() const throw()                    { return m_parallax.scale; }

  /// \brief
  ///   Gets parallax mapping bias value
  ///
  /// \return
  ///   Value of parallax mapping's bias
  inline float                     GetParallaxMappingBias () const throw()                    { return m_parallax.bias;  }

  /// \brief
  ///   Sets parallax mapping values
  ///
  /// \param scale
  ///   Value to set as parallax mapping's scale
  ///
  /// \param bias
  ///   Value to set as parallax mapping's bias
  inline void                      SetParallaxMappingParams(float scale, float bias) throw()  { m_parallax.scale = scale; m_parallax.bias = bias; }



  /// \brief
  ///   Gets this material's lightning type
  ///
  /// \return
  ///   Material's lightning type
  inline MaterialLighting          GetMaterialLighting() const throw()                        { return m_lighting; }

  /// \brief
  ///   Sets this material's lightning type
  ///
  /// \param l
  ///   Material lightning type to set
  inline void                      SetMaterialLighting(MaterialLighting l) throw()            { m_lighting = l; }



  /// \brief
  ///   Gets this material's flags
  ///
  /// \return
  ///   Material's flags
  inline MaterialFlags             GetMaterialFlags() const throw()                           { return m_flags; }

  /// \brief
  ///   Sets this material's flags
  ///
  /// \param f
  ///   Material flags to set
  inline void                      SetMaterialFlags(MaterialFlags f) throw()                  { m_flags = f; }

  
  /// \brief
  ///   Gets this material's user data string
  ///
  /// \return
  ///   Material's user data string
  inline const VString&            GetUserDataString() const throw()                          { return m_userDataString; }

  /// \brief
  ///   Sets this material's user data string
  ///
  /// \param userData
  ///   User data string to assign
  inline void                      SetUserDataString(const VString& userData)                 { m_userDataString = userData; }

  /// \brief
  ///   Gets this material's user data flags
  ///
  /// \return
  ///   Material's user data flags
  inline int                       GetUserDataFlags() const throw()                           { return m_userDataFlags; }

  /// \brief
  ///   Sets this material's user data flags
  ///
  /// \param userData
  ///   User data flags to assign
  inline void                      SetUserDataFlags(int userData)                             { m_userDataFlags = userData; }



  /// \brief
  ///   Gets this material's sorting order value
  ///
  /// \return
  ///   Sorting order index
  inline int                       GetSortOrder() const throw()                               { return m_sortOrder; }

  /// \brief
  ///   Sets this material's sorting order value
  ///
  /// \param sortOrder
  ///   Material's sorting order index
  inline void                      SetSortOrder(int sortOrder) throw()                        { m_sortOrder = sortOrder; }

  /// \brief
  ///   Gets this material's deferred material ID (8 bit value)
  ///
  /// \return
  ///   Deferred material ID
  inline UBYTE                     GetDeferredID() const throw()                              { return m_deferredID; }

  /// \brief
  ///   Sets this material's deferred material ID (8 bit value)
  ///
  /// \param iID
  ///   Deferred material ID
  inline void                      SetDeferredID(UBYTE iID) throw()                           { m_deferredID = iID; }



  /// \brief
  ///   Gets lightmap brightness value
  ///
  /// \return
  ///   Material's lightmap brightness value
  inline int                       GetLightmapBrightness() const throw()                      { return m_lightmap.brightness; }

  /// \brief
  ///   Gets lightmap page index
  ///
  /// \return
  ///   Material's lightmap page index
  inline int                       GetLightmapPageMergeID() const throw()                     { return m_lightmap.pageMergeID; }

  /// \brief
  ///   Gets lightmap page x-size
  ///
  /// \return
  ///   Material's lightmap page x-size
  inline short                     GetLightmapPageSizeX() const throw()                       { return m_lightmap.pageSize.x; }

  /// \brief
  ///   Gets lightmap page y-size
  ///
  /// \return
  ///   Material's lightmap page y-size
  inline short                     GetLightmapPageSizeY() const throw()                       { return m_lightmap.pageSize.y; }

  /// \brief
  ///   Sets lightmap brightness value
  ///
  /// \param b
  ///   Lightmap brightness value to set
  inline void                      SetLightmapBrightness(int b) throw()                       { m_lightmap.brightness = b; }

  /// \brief
  ///   Sets lightmap page index
  ///
  /// \param mID
  ///   Lightmap page merge ID index
  inline void                      SetLightmapPageMergeID(int mID) throw()                    { m_lightmap.pageMergeID = mID; }

  /// \brief
  ///   Sets lightmap page size
  ///
  /// \param x
  ///   Lightmap page x-size value to set
  ///
  /// \param y
  ///   Lightmap page y-size value to set
  inline void                      SetLightmapPageSize(short x, short y) throw()              { m_lightmap.pageSize.x = x; m_lightmap.pageSize.y = y; }



  /// \brief
  ///   Gets material's shader assignment
  ///
  /// \return
  ///   Reference to material's shader assignment
  inline const VGShaderAssignment& GetShaderAssignment() const throw()                        { return m_shaderAssignment; }

  /// \brief
  ///   Sets material's shader assignment
  ///
  /// \param sa
  ///   Shader assignment to assign to this material
  inline void                      SetShaderAssignment(const VGShaderAssignment& sa)          { m_shaderAssignment = sa; }



  /// \brief
  ///   Gets path of material's .materiallib override (if set)
  ///
  /// \return
  ///   Path of material's .materiallib override (or empty string if not set)
  inline const VString&            GetOverride_MaterialLibPath() const throw()                { return m_materialLibOverride_Path; }

  /// \brief
  ///   Sets path of material's .materiallib override
  ///
  /// \param s
  ///   Path of material's .materiallib override (or empty string to unset)
  inline void                      SetOverride_MaterialLibPath(const VString& s)              { m_materialLibOverride_Path = s; }

  /// \brief
  ///   Gets name of overridden material (out of .materiallib)
  ///
  /// \return
  ///   Material name
  inline const VString&            GetOverride_MaterialName() const throw()                   { return m_materialLibOverride_Name; }

  /// \brief
  ///   Sets name of overridden material (out of .materiallib)
  ///
  /// \param s
  ///   Material name
  inline void                      SetOverride_MaterialName(const VString& s)                 { m_materialLibOverride_Name = s; }

  /// \brief
  ///   Sets a bounding box margin that is added in all directions on top of the respective vertices' bounding box. Can be left to default (0,0,0, 0,0,0). To set a margin of 1 unit, pass (-1,-1,-1, 1,1,1)
  inline void     SetBoundingBoxMargin(float fNegX, float fNegY, float fNegZ, float fPosX, float fPosY, float fPosZ)  {m_BBoxMargin.setWithoutValidityCheck(hkvVec3(fNegX,fNegY,fNegZ), hkvVec3(fPosX,fPosY,fPosZ));}

  /// \brief
  ///   Returns the bounding box margin used for submeshes with this material
  inline const hkvAlignedBBox&     GetBoundingBoxMargin() const                               {return m_BBoxMargin;}


private:
  // Material's maps.
  VString  m_maps[VGMM_NUM];
  VStrList m_auxTextures;

  // Specularity parameters.
  float m_specMul;
  float m_specExp;

  // Transparency method.
  VIS_TransparencyType m_transparency;

  // Material colors.
  hkvVec4 m_colors[VGMC_NUM];

  // Other rendering related material properties.
  float m_fCustomAlphaThreshold;
  float m_depthBias;
  float m_depthBiasClamp;
  float m_slopeScaledDepthBias;

  // Mapping parameters.
  struct { float u, v; } m_scale, m_offset;
  float m_rotation;
  UVMappingAlgorithm m_ma;

  // Parallax/offset mapping parameters.
  struct { float scale, bias; } m_parallax;

  // Material properties and flags.
  MaterialLighting m_lighting;
  MaterialFlags    m_flags;

  int m_sortOrder;

  UBYTE m_deferredID;

  struct
  {
    int brightness;     // Brightness of emitted light.
    int pageMergeID;
    struct { short x, y; } pageSize;
  } m_lightmap;


  // Custom user data string and flags.
  VString m_userDataString;
  int     m_userDataFlags;

  // Shader.
  VGShaderAssignment m_shaderAssignment;

  // Material override settings.
  VString m_materialLibOverride_Path;
  VString m_materialLibOverride_Name;

  hkvAlignedBBox m_BBoxMargin; // defaults to (0,0,0)-(0,0,0) and defines how much the bounding box of this material needs to be extruded in all directions
};

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
