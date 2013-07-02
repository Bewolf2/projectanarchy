/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file SkyLayer.hpp

#ifndef _SKYLAYER_HPP_INCLUDED
#define _SKYLAYER_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/EffectsModule.hpp>

class VSky;
class VSkyBase;

/// \brief
///   Class that represents a single sky layer and provides functionality to set up
///   the sky layer properties.
/// 
/// All layer properties can be changed during runtime without any performance impact
class VSkyLayer : public VBaseObject
{
public:

  ///
  /// @name Constructor/destructor
  /// @{
  ///

  /// \brief
  ///   Constructor of the Sky Layer
  EFFECTS_IMPEXP VSkyLayer();

  /// \brief
  ///   Destructor of the sky layer class.
  /// 
  /// The destructor of the sky layer class releases the allocated memory.
  EFFECTS_IMPEXP virtual ~VSkyLayer();

  /// \brief
  ///   Assignment operator that copies the properties of one layer to another.
  EFFECTS_IMPEXP VSkyLayer &operator = (const VSkyLayer &other);

  ///
  /// @}
  ///

  ///
  /// @name Layer Mapping and Textures
  /// @{
  ///

  /// \brief
  ///   Sets the mapping type of the sky layer
  /// 
  /// SetMapping sets the mapping type of the sky layer. The sky system supports spherical and
  /// cube-mapped sky layers. 
  /// 
  /// \param mapping
  ///   mapping type of the sky layer. Supported type constants are:
  ///   \li VIS_SKYMAPPING_SPHERICAL
  /// 
  ///   \li VIS_SKYMAPPING_CUBEMAP
  /// 
  /// \sa VSkyLayer::SetTextureFile
  /// \sa VSkyLayer::SetCubemapTextureFiles
  EFFECTS_IMPEXP void SetMapping(VIS_SKY_MappingType mapping);

  /// \brief
  ///   Returns the mapping type of the sky layer.
  /// 
  /// \return
  ///   VIS_SKY_MappingType mapping: mapping type of the sky layer
  /// 
  /// \sa SetMapping
  inline VIS_SKY_MappingType GetMappingType() const;

  /// \brief
  ///   Sets the texture file of this sky layer.
  /// 
  /// The parameter has to be the name  of a
  /// valid texture file. The texture specified with this method will be used for spherical mapping
  /// only. If the layer has mapping type VIS_SKYMAPPING_CUBEMAP, you have to specify 6 textures
  /// representing the cube with the VSkyLayer::SetCubemapTextureFiles function. The texture specified using
  /// this method will be ignored in this case.
  /// 
  /// A texture object can be set via SetTexture
  /// 
  /// \param pszTexFile
  ///   texture file name
  /// 
  /// \sa VSkyLayer::SetCubemapTextureFiles
  /// 
  /// \example
  ///   \code
  ///   skyLayer->SetTextureFile("darksky.dds");
  ///   \endcode
  EFFECTS_IMPEXP void SetTextureFile(const char *pszTexFile);

  /// \brief
  ///   Set the texture object that is used for spherical mapping
  EFFECTS_IMPEXP void SetTexture(VTextureObject *pTex);

  /// \brief
  ///   Sets the 6 texture file names which define a cubemap for cube-mapped sky layers.
  /// 
  /// Each parameter has to be a valid texture filename of an existing texture.
  /// 
  /// the mapping type for this layer has to be set to VIS_SKYMAPPING_CUBEMAP, otherwise setting
  /// the cubemap  textures won't have any effect. Setting bottomTex to NULL is allowed and may be
  /// desired if the bottom texture of the sky cube will never be visible. The remaining 5 sides
  /// (including top) however have to be  specified.
  /// 
  /// Scaling and movement will be ignored for cube map layers.
  /// 
  /// \param rightTex
  ///   Texture filename for the right side of the cube
  /// 
  /// \param leftTex
  ///   Texture filename for the left side of the cube
  /// 
  /// \param frontTex
  ///   Texture filename for the front side of the cube
  /// 
  /// \param backTex
  ///   Texture filename for the back side of the cube
  /// 
  /// \param topTex
  ///   Texture filename for the top side of the cube
  /// 
  /// \param bottomTex
  ///   Texture filename for the bottom side of the cube, may be NULL
  /// 
  /// \sa VSkyLayer::SetMapping
  /// 
  /// \example
  ///   \code
  ///   skyLayer->SetCubemapTextureFiles("cube_face_posx.bmp","cube_face_negx.bmp","cube_face_posy.bmp","cube_face_negy.bmp","cube_face_posz.bmp","cube_face_negz.bmp");
  ///   \endcode
  EFFECTS_IMPEXP void SetCubemapTextureFiles(const char *rightTex, const char *leftTex, const char *frontTex, const char *backTex, const char *topTex, const char *bottomTex=NULL);
  
  /// \brief
  ///   Set the 6 texture objects for cube-mapped sky layers. The pBottom object can be NULL
  EFFECTS_IMPEXP void SetCubemapTextures(VTextureObject *pRight, VTextureObject *pLeft, VTextureObject *pFront, VTextureObject *pBack, VTextureObject *pTop, VTextureObject *pBottom=NULL);

  ///
  /// @}
  ///

  ///
  /// @name Layer Appearance
  /// @{
  ///

  /// \brief
  ///   Set a RGBA color that is used to tint this layer
  ///   Each color component is in float range [0..1]; the alpha component is used for blending intensity.
  inline void SetColor(const hkvVec4 &vColor);

  /// \brief
  ///   This version of SetColor takes a VColorRef and converts it to float based color, which is
  ///   the internal representation
  inline void SetColor(VColorRef iColor);

  /// \brief
  ///   Returns the float based RGBA color of this layer
  inline const hkvVec4& GetColor() const;

  /// \brief
  ///   Sets the layer's texture scroll speed (texture size per second) for spherical layers.
  inline void SetScrollSpeed(float fSpeedU, float fSpeedV);

  /// \brief
  ///   Gets the layer's texture scroll speed. See SetScrollSpeed
  inline void GetScrollSpeed(float &fSpeedU, float &fSpeedV) const;

  /// \brief
  ///   Sets the current texture scroll position (spherical layers only)
  inline void SetScrollPosition(float fU, float fV);

  /// \brief
  ///   Gets the current texture scroll position (spherical layers only)
  inline void GetScrollPosition(float &fU, float &fV) const;

  /// \brief
  ///   Sets the layer's texture tiling factors (spherical layers only)
  inline void SetTextureScaling(float fScaleU, float fScaleV);

  /// \brief
  ///   Sets the layer's texture tiling factors (spherical layers only)
  inline void GetTextureScaling(float &fScaleU, float &fScaleV) const;

  /// \brief
  ///   Sets the height scaling of this sky layer (spherical layers only).
  /// 
  /// A value of 1.0 means that the sky
  /// layer's texture coordinates are computed so that the sky forms a sphere. A value <1.0 means
  /// that the sky layer sphere is stretched vertically, a value >1.0 means that the sky layer
  /// appears flat. The default value for the heightScale is 2.5f.
  /// 
  /// This setting will only have an effect on spherically mapped sky layers.
  ///
  /// \param heightScale
  ///   Height scaling value.
  inline void SetHeightScale(float heightScale);

  /// \brief
  ///   Returns the height scaling of this sky layer. See SetHeightScale
  inline float GetHeightScale() const;
  
  /// \brief
  ///   Sets the height position of this sky layer (spherical layers only).
  /// 
  /// This property shifts the spherical coordinates up or down.
  /// 
  /// The default value is 0.0
  ///
  /// \param heightPos
  ///   Height position value.
  inline void SetHeightPos(float heightPos);
    
  /// \brief
  ///   Returns the height position of this sky layer. See SetHeightPos
  inline float GetHeightPos() const;

  /// \brief
  ///   Wraps around setting the alpha component of the color. See SetColor
  inline void SetIntensity(float intensity);

  /// \brief
  ///   Returns the intensity of the sky layer which is the alpha component of the color (see
  ///   GetColor)
  inline float GetIntensity() const;

  /// \brief
  ///   Returns the texture
  inline VTextureObjectPtr GetTexture() const;

  /// \brief
  ///   Returns a given cube map face
  inline VTextureObjectPtr GetCubemapFace(unsigned int nFace) const;
  
  /// \brief
  ///   DEPRECATED; use SetScrollSpeed instead
  inline void SetUMovement(float uMovement);
  
  
  /// \brief
  ///   DEPRECATED; use SetScrollSpeed instead
  EFFECTS_IMPEXP void SetVMovement(float vMovement);
  
  
  /// \brief
  ///   DEPRECATED; use SetTextureScaling instead
  EFFECTS_IMPEXP void SetTextureUScale(float textureUScale);

  
  /// \brief
  ///   DEPRECATED; use SetTextureScaling instead
  EFFECTS_IMPEXP void SetTextureVScale(float textureVScale);
  
  /// \brief
  ///   DEPRECATED; use GetScrollSpeed instead
  inline float GetUMovement() const;
  
  /// \brief
  ///   DEPRECATED; use GetScrollSpeed instead
  inline float GetVMovement() const;
  
  /// \brief
  ///   DEPRECATED; use GetTextureScaling instead
  inline float GetTextureUScale() const;

  /// \brief
  ///   DEPRECATED; use GetTextureScaling instead
  inline float GetTextureVScale() const;


  V_DECLARE_SERIALX( VSkyLayer, EFFECTS_IMPEXP )
  EFFECTS_IMPEXP void SerializeX( VArchive &ar );

  inline void Tick(float dtime);
  ///
  /// @}
  ///

  inline void SetParentSky(VSkyBase *pSky)
  {
    m_pOwnerSky = pSky;
  }

private:

  VSkyBase *m_pOwnerSky;

//  float widthPos;                 // width pos
  VIS_SKY_MappingType m_eMapping;    ///< mapping mode
  hkvVec4 m_vColor;           ///< modulation color
  hkvVec4 m_vMappingParams;  ///< x=mapping type, y=heightscale, z=heightofs

  // texture transformation
  hkvVec4 m_vTextureTransform; ///< xy:scale,zw=ofs
  hkvVec2 m_vScrollSpeed;

  VTextureObjectPtr m_spTexture;
  VTextureObjectPtr m_spCubemapFace[6];
};


inline void VSkyLayer::Tick(float dtime)
{
  m_vTextureTransform.z = hkvMath::mod (m_vTextureTransform.z+m_vScrollSpeed.x*dtime,1.f);
  m_vTextureTransform.w = hkvMath::mod (m_vTextureTransform.w+m_vScrollSpeed.y*dtime,1.f);
}


inline void VSkyLayer::SetColor(VColorRef iColor)
{
  m_vColor.r = (float)iColor.r*(1.f/255.f);
  m_vColor.g = (float)iColor.g*(1.f/255.f);
  m_vColor.b = (float)iColor.b*(1.f/255.f);
  m_vColor.a = (float)iColor.a*(1.f/255.f);
}

inline void VSkyLayer::SetColor(const hkvVec4 &vColor)
{
  m_vColor = vColor;
}

inline const hkvVec4& VSkyLayer::GetColor() const
{
  return m_vColor;
}

inline void VSkyLayer::SetScrollSpeed(float fSpeedU, float fSpeedV)
{
  m_vScrollSpeed.set(fSpeedU,fSpeedV);
}

inline void VSkyLayer::SetScrollPosition(float fU, float fV)
{
  m_vTextureTransform.z = fU;
  m_vTextureTransform.w = fV;
}

inline void VSkyLayer::GetScrollPosition(float &fU, float &fV) const
{
  fU = m_vTextureTransform.z;
  fV = m_vTextureTransform.w;
}

inline void VSkyLayer::SetTextureScaling(float fScaleU, float fScaleV)
{
  m_vTextureTransform.x = fScaleU;
  m_vTextureTransform.y = fScaleV;
}

inline void VSkyLayer::GetTextureScaling(float &fScaleU, float &fScaleV) const
{
  fScaleU = m_vTextureTransform.x;
  fScaleV = m_vTextureTransform.y;
}

inline void VSkyLayer::GetScrollSpeed(float &fSpeedU, float &fSpeedV) const
{
  fSpeedU = m_vScrollSpeed.x;
  fSpeedV = m_vScrollSpeed.y;
}


inline void VSkyLayer::SetHeightScale(float heightScale)
{
  m_vMappingParams.y = heightScale;
}


inline void VSkyLayer::SetHeightPos(float heightPos)
{
  m_vMappingParams.z  = heightPos;
}

inline float VSkyLayer::GetHeightScale() const
{
  return m_vMappingParams.y;
}

    
inline float VSkyLayer::GetHeightPos() const
{
  return m_vMappingParams.z;
}

inline VIS_SKY_MappingType VSkyLayer::GetMappingType() const
{
  return m_eMapping;
}


/////////////////////////////////////////////////////////////////////////////////////////
// Deprecated inlines
/////////////////////////////////////////////////////////////////////////////////////////

inline void VSkyLayer::SetUMovement(float uMovement)
{
  m_vScrollSpeed.x = uMovement;
}


inline void VSkyLayer::SetVMovement(float vMovement)
{
  m_vScrollSpeed.y = vMovement;
}


inline void VSkyLayer::SetTextureUScale(float textureUScale)
{
  m_vTextureTransform.x = textureUScale;
}


inline void VSkyLayer::SetTextureVScale(float textureVScale)
{
  m_vTextureTransform.y = textureVScale;
}

inline void VSkyLayer::SetIntensity(float intensity)
{
  m_vColor.a = intensity;
}

inline float VSkyLayer::GetIntensity() const
{
  return m_vColor.a;
}

inline VTextureObjectPtr VSkyLayer::GetTexture() const
{
  return m_spTexture;
}

inline VTextureObjectPtr VSkyLayer::GetCubemapFace(unsigned int nFace) const
{
  VASSERT(nFace < 6);
  return m_spCubemapFace[nFace];
}


inline float VSkyLayer::GetUMovement() const
{
  return m_vScrollSpeed.x;
}
  
inline float VSkyLayer::GetVMovement() const
{
  return m_vScrollSpeed.y;
}
  
inline float VSkyLayer::GetTextureUScale() const
{
  return m_vTextureTransform.x;
}

inline float VSkyLayer::GetTextureVScale() const
{
  return m_vTextureTransform.y;
}


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
