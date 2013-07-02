/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VLensFlareComponent.hpp

#ifndef VLENSFLARECOMPONENT_HPP_INCLUDED
#define VLENSFLARECOMPONENT_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/EffectsModule.hpp>
#include <Vision/Runtime/Engine/Renderer/OcclusionQuery/VisApiOcclusionQuery.hpp>
#include <Vision/Runtime/Engine/Renderer/OcclusionQuery/VisApiOcclusionQueryObjectPixelCounter.hpp>

class VLensFlareManager;
class VLensFlareComponent;
class VOcclusionQueryObjectPixelCounterLensFlare;
typedef VSmartPtr<VLensFlareComponent> VLensFlareComponentPtr;

#define MAX_NUM_LENS_FLARES 16


/// \brief
///   Describes a single lens flare within a VLensFlareComponent.
struct VLensFlareDescriptor
{
  VLensFlareDescriptor ()
    : m_sFlareTexture(), m_fScale(1.0f), m_fPositionOnRay(0.0f) {}

  VString m_sFlareTexture;
  float m_fScale;
  float m_fPositionOnRay;
};


/// \brief
///   Derived occlusion query pixel counter object for lens flare visibility determination.
class VOcclusionQueryObjectPixelCounterLensFlare : public VOcclusionQueryObjectPixelCounter
{
public:
  VOcclusionQueryObjectPixelCounterLensFlare();
  ~VOcclusionQueryObjectPixelCounterLensFlare();

  EFFECTS_IMPEXP virtual bool Render(VOcclusionQuery &query, const hkvAlignedBBox &safeBox) HKV_OVERRIDE;

private:
  VLensFlareComponent *m_pLensFlare;

  friend class VLensFlareComponent;
};


/// \brief
///   An instance of this class defines a lens flare effect instance.
///   This component can be attached to any VisLightSource_cl.
///   It only works on platforms that support occlusion queries.
class VLensFlareComponent : public IVObjectComponent
{
  friend class VLensFlareManager;
public: 
  ///
  /// @name Constructor
  /// @{
  ///

  /// \brief
  ///   Component constructor
  EFFECTS_IMPEXP VLensFlareComponent(int iComponentFlags=VIS_OBJECTCOMPONENTFLAG_NONE);

  /// \brief
  ///   Component destructor
  EFFECTS_IMPEXP virtual ~VLensFlareComponent();

  ///
  /// @}
  ///

  ///
  /// @name Lens Flare Properties
  /// @{
  ///

  /// \brief
  ///   Enables or disables the component
  inline void SetEnabled(bool bStatus=true) {Enabled=bStatus;}

  /// \brief
  ///   Indicates whether the component is currently enabled
  inline bool IsEnabled() const {return Enabled==TRUE;}

  /// \brief
  ///   Sets the VLensFlareDescriptor at the given index.
  ///
  /// If iIndex is out of bounds (0..15) the functions returns false.
  ///
  /// \param iIndex
  ///   Index of the VLensFlareDescriptor to be changed.
  ///
  /// \param desc
  ///   Reference to the new VLensFlareDescriptor defining the lens flare.
  ///
  /// \return
  ///   Returns if the operation was successful.
  ///
  /// \sa VLensFlareComponent::GetLensFlareDescriptor
  EFFECTS_IMPEXP bool SetLensFlareDescriptor (BYTE iIndex, const VLensFlareDescriptor& desc);

  /// \brief
  ///   Retrieves the VLensFlareDescriptor at the given index.
  ///
  /// If iIndex is out of bounds (0..15) the functions returns false.
  ///
  /// \param iIndex
  ///   Index of the VLensFlareDescriptor to be retrieved.
  ///
  /// \param desc
  ///   Reference to a VLensFlareDescriptor in which the to be retrieved flare should be stored.
  ///
  /// \return
  ///   Returns if the operation was successful.
  ///
  /// \sa VLensFlareComponent::SetLensFlareDescriptor
  EFFECTS_IMPEXP bool GetLensFlareDescriptor (BYTE iIndex, VLensFlareDescriptor& desc) const;

  /// \brief
  ///   Returns a pointer to the VTextureObject of the flare at the given index.
  ///
  /// If iIndex is out of bounds (0..15) or no texture is set for the given flare,
  /// the function returns NULL.
  ///
  /// \param iIndex
  ///   Index of the flare for which the texture should be retrieved.
  ///
  /// \return
  ///   Returns a pointer to the VTextureObject of the flare.
  EFFECTS_IMPEXP VTextureObject* GetLensFlareTexture (BYTE iIndex);

  /// \brief
  ///   Set the size of the check block for the lens flare occlusion query.
  /// 
  /// The size is specified as the radius in pixels. A larger radius allows more fine-grained
  /// fading of the lens flare, but it also means that irrelevant geometry may affect the
  /// lens flare visibility more easily. 
  /// 
  /// \param iRadiusPixels
  ///   Radius in Pixels.
  ///
  /// \sa VLensFlareComponent::GetCheckBlockSize
  inline void SetCheckBlockSize(BYTE iRadiusPixels) {QueryRadius = hkvMath::Min( hkvMath::Max(iRadiusPixels, 1), 255);}

  /// \brief
  ///   Get the size of the check block for the occlusion query of this lens flare.
  /// 
  /// The value can be set using the VLensFlareComponent::SetCheckBlockSize method.
  /// 
  /// \return
  ///   Radius in Pixels.
  ///
  /// \sa VLensFlareComponent::SetCheckBlockSize
  inline BYTE GetCheckBlockSize() const {return QueryRadius;}

  /// \brief
  ///   Sets the time in ms that is needed for the lens flare to become fully visible.
  inline void SetPreGlowMS(unsigned int iPreGlowMS) {PreGlowMS = iPreGlowMS;}

  /// \brief
  ///   Returns the time in ms that is needed for the lens flare to become fully visible.
  inline unsigned int GetPreGlowMS() const {return PreGlowMS;}

  /// \brief
  ///   Sets the time in ms that is needed for the lens flare to vanish completely.
  inline void SetAfterGlowMS(unsigned int iAfterGlowMS) {AfterGlowMS = iAfterGlowMS;}

  /// \brief
  ///   Returns the time in ms that is needed for the lens flare to vanish completely.
  inline unsigned int GetAfterGlowMS() const {return AfterGlowMS;}

  /// \brief
  ///   Sets the depth bias of the occlusion query. Negative values will move the query closer to the camera.
  inline void SetDepthBias(float fDepthBias) {DepthBias = fDepthBias;}

  /// \brief
  ///   Returns the depth bias of the occlusion query. 
  inline float GetDepthBias() const {return DepthBias;}

  /// \brief
  ///   Sets the OnlyVisibleInSpotLight flag. This flag only influences the LensFlare, when it is attached to a spotlight. If it is true, the Corona is only visible if the camera is inside the cone defined by the spotlight. Please also note that the projected texture is not used to calculate the intensity of the corona. Instead a simple falloff function is used.
  inline void SetOnlyVisibleInSpotLight(bool bOnlyVisibleInSpotLight) {OnlyVisibleInSpotLight = bOnlyVisibleInSpotLight;}

  /// \brief
  ///   Gets the OnlyVisibleInSpotLight flag. This flag only influences the LensFlare, when it is attached to a spotlight. If it is true, the Corona is only visible if the camera is inside the cone defined by the spotlight. Please also note that the projected texture is not used to calculate the intensity of the corona. Instead a simple falloff function is used.
  inline bool GetOnlyVisibleInSpotLight() const {return OnlyVisibleInSpotLight==TRUE;}

  /// \brief
  ///   Sets the query radius of the occlusion query in pixels.
  inline void SetQueryRadius(BYTE iQueryRadius) { QueryRadius = hkvMath::Min( hkvMath::Max(iQueryRadius, 1), 255); }

  /// \brief
  ///   Returns the query radius of the occlusion query in pixels.
  inline BYTE GetQueryRadius() const {return QueryRadius;}

  /// \brief
  ///   Sets the fade out start distance of the lens flare. 
  ///
  /// If the distance to the light is below 'FadeOutStart' the lens flare is not faded and
  /// if it is over 'FadeOutEnd' the lens flare is not rendered at all.
  /// If 'FadeOutEnd' is set to zero, fading is disabled.
  ///
  /// \param iFadeOutStart
  ///   New start distance of the fade out.
  ///
  /// \sa VLensFlareComponent::SetFadeOutEnd
  inline void SetFadeOutStart(float iFadeOutStart) {FadeOutStart = iFadeOutStart;}

  /// \brief
  ///   Returns the fade out start distance of the lens flare.
  inline float GetFadeOutStart() const {return FadeOutStart;}

  /// \brief
  ///   Sets the fade out end distance of the lens flare. 
  ///
  /// If the distance to the light is below 'FadeOutStart' the lens flare is not faded and
  /// if it is over 'FadeOutEnd' the lens flare is not rendered at all.
  /// If 'FadeOutEnd' is set to zero, fading is disabled.
  ///
  /// \param iFadeOutEnd
  ///   New end distance of the fade out. Zero disables fading.
  ///
  /// \sa VLensFlareComponent::SetFadeOutEnd
  inline void SetFadeOutEnd(float iFadeOutEnd) {FadeOutEnd = iFadeOutEnd;}

  /// \brief
  ///   Returns the fade out end distance of the lens flare.
  inline float GetFadeOutEnd() const {return FadeOutEnd;}

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
  ///   Overridden function. Lens flares can be attached to VisLightSource_cl instances
  EFFECTS_IMPEXP virtual BOOL CanAttachToObject(VisTypedEngineObject_cl *pObject, VString &sErrorMsgOut);

  /// \brief
  ///   Overridden function to respond to variable changes
  EFFECTS_IMPEXP virtual void OnVariableValueChanged(VisVariable_cl *pVar, const char * value);

#if defined(WIN32) || defined(_VISION_DOC)

  /// \brief
  ///   Overridden function that is called by the editor to retrieve per-instance variable display hints. 
  EFFECTS_IMPEXP virtual void GetVariableAttributes(VisVariable_cl *pVariable, VVariableAttributeInfo &destInfo) HKV_OVERRIDE;

#endif

  ///
  /// @}
  ///

  ///
  /// @name Serialization
  /// @{
  ///

  /// \brief
  ///   RTTI macro
  V_DECLARE_SERIAL_DLLEXP( VLensFlareComponent, EFFECTS_IMPEXP)

  /// \brief
  ///   RTTI macro to add a variable table
  V_DECLARE_VARTABLE(VLensFlareComponent, EFFECTS_IMPEXP)

  /// \brief
  ///   Serialization function
  ///
  /// \param ar
  ///   binary archive
  EFFECTS_IMPEXP virtual void Serialize( VArchive &ar );

  ///
  /// @}
  ///

private:
  bool IsValidCandidate(VisRenderContext_cl* pContext) const;
  void UpdateVisibility (float& fLastVisibilityQuery, float& fCurrentVisibility);
  void InitFlares();

  BYTE GetNumberOfUsedFlares() const;

  VLensFlareDescriptor m_Flares[MAX_NUM_LENS_FLARES];
  VTextureObjectPtr m_spTextures[MAX_NUM_LENS_FLARES];

  VOcclusionQueryObjectPixelCounterLensFlare m_LensFlarePixelCounter; 

  int m_iIndex; 

  // Lens flare properties
  unsigned int PreGlowMS;
  unsigned int AfterGlowMS;
  float DepthBias;
  float FadeOutStart;
  float FadeOutEnd;
  BYTE QueryRadius;
  BOOL Enabled;
  BOOL OnlyVisibleInSpotLight;
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
