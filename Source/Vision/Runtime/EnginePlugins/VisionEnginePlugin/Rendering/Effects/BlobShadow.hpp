/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file BlobShadow.hpp

#ifndef BLOBSHADOW_HPP_INCLUDED
#define BLOBSHADOW_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/EffectsModule.hpp>

class VBlobShadowManager;
class VBlobShadow;
typedef VSmartPtr<VBlobShadow> BlobShadowPtr;


/// \brief
///   Shader class with shadow update capabilities
class VBlobShadowShader : public VCompiledShaderPass
{
public:
  EFFECTS_IMPEXP VBlobShadowShader();
  EFFECTS_IMPEXP virtual void PostCompileFunction(VShaderEffectResource *pSourceFX,VShaderPassResource *pSourceShader);
  void UpdateShadow(const VBlobShadow *pShadow);

  int m_iShadowTexSampler, m_iGradientTexSampler;
  int m_iRegPlaneU, m_iRegPlaneV, m_iRegColor;

  /// \brief
  ///   RTTI macro
  V_DECLARE_DYNCREATE_DLLEXP( VBlobShadowShader, EFFECTS_IMPEXP)

};


/// \brief
///   An instance of this class defines a single blob shadow instance. It is a component that can be attached to
///   objects of class VisObject3D_cl
class VBlobShadow : public IVObjectComponent
{
public: 
  ///
  /// @name Constructor
  /// @{
  ///

  /// \brief
  ///   Component constructor
  EFFECTS_IMPEXP VBlobShadow(int iComponentFlags=VIS_OBJECTCOMPONENTFLAG_NONE);
  EFFECTS_IMPEXP virtual ~VBlobShadow();

  ///
  /// @}
  ///

  ///
  /// @name Shadow Properties
  /// @{
  ///

  /// \brief
  ///   Enables or disables the blob shadow
  inline void SetEnabled(bool bStatus=true) {Enabled=bStatus;}

  /// \brief
  ///   Indicates whether the blob shadow is currently enabled
  inline bool IsEnabled() const {return Enabled==TRUE;}

  /// \brief
  ///   Sets the world space bounding box for the shadow. The bounding box is used for determining relevant primitives
  inline void SetShadowBox(const hkvAlignedBBox &bbox) {m_ShadowBox=bbox;}

  /// \brief
  ///   Retrieves the current world space bounding box
  inline void GetShadowBox(hkvAlignedBBox &bbox) const {bbox=m_ShadowBox;}

  /// \brief
  ///   Defines radius and height of the shadow box if the center is provided through the owner object
  inline void SetSize(float fRadius, float fHeight) {Height=fHeight;Radius=fRadius;}

  /// \brief
  ///   Defines radius and height of the shadow box if the center is provided through the owner object
  void SetBoundingBoxFromOwnerProperties();

  /// \brief
  ///   Sets the shadow color. The default color is gray
  inline void SetColor(VColorRef iColor) {ShadowColor=iColor;m_vBlendColor = hkvVec4(1.f-(float)iColor.r/255.f,1.f-(float)iColor.g/255.f,1.f-(float)iColor.b/255.f, 1.f);}

  /// \brief
  ///   Returns the current shadow color. The default color is gray
  inline VColorRef GetColor() const {return ShadowColor;}

  /// \brief
  ///   Sets a custom shadow texture. Can be set to NULL to use the default texture
  inline void SetShadowTexture(VTextureObject *pTex) {m_spShadowTex=pTex;}

  /// \brief
  ///   Returns the custom shadow texture. Returns NULL if no custom texture is set
  inline VTextureObject* GetShadowTexture() const {return m_spShadowTex;}

  /// \brief
  ///   Loads a custom texture by filename and sets it
  inline void SetShadowTexture(const char *szFilename) 
  {
    SetShadowTexture(Vision::TextureManager.Load2DTexture(szFilename));
  }

  ///
  /// @}
  ///

  ///
  /// @name IVObjectComponent Overrides
  /// @{
  ///

  /// \brief
  ///   Overridden function to respond to owner changes
  EFFECTS_IMPEXP virtual void SetOwner(VisTypedEngineObject_cl *pOwner);

  /// \brief
  ///   Overridden function. Blob shadows can be attached to VisObject3D_cl instances
  EFFECTS_IMPEXP virtual BOOL CanAttachToObject(VisTypedEngineObject_cl *pObject, VString &sErrorMsgOut);

  /// \brief
  ///   Overridden function to respond to variable changes
  EFFECTS_IMPEXP virtual void OnVariableValueChanged(VisVariable_cl *pVar, const char * value);

  ///
  /// @}
  ///

  ///
  /// @name Serialization
  /// @{
  ///

  /// \brief
  ///   RTTI macro
  V_DECLARE_SERIAL_DLLEXP( VBlobShadow, EFFECTS_IMPEXP)

  /// \brief
  ///   RTTI macro to add a variable table
  V_DECLARE_VARTABLE(VBlobShadow, EFFECTS_IMPEXP)

  /// \brief
  ///   Serialization function
  ///
  /// \param ar
  ///   binary archive
  EFFECTS_IMPEXP virtual void Serialize( VArchive &ar );

  ///
  /// @}
  ///

public:  
  // internal values/functions
  bool PrepareForRendering(VCompiledShaderPass *pShader);
  // shadow flags
  BOOL Enabled;
  bool m_bNormalFalloff; ///</< not used

  // shadow properties
  hkvAlignedBBox m_ShadowBox;
  VColorRef ShadowColor;
  hkvVec4 m_vBlendColor;
  VTextureObjectPtr m_spShadowTex;
  float Radius, Height;
};


/// \brief
///   Collection for handling shadow instances
class VBlobShadowCollection : public VRefCountedCollection<VBlobShadow>
{
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
