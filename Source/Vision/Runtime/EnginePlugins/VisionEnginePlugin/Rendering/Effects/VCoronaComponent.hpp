/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VCoronaComponent.hpp

#ifndef VCORONACOMPONENT_HPP_INCLUDED
#define VCORONACOMPONENT_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/EffectsModule.hpp>
#include <Vision/Runtime/Engine/Renderer/OcclusionQuery/VisApiOcclusionQuery.hpp>
#include <Vision/Runtime/Engine/Renderer/OcclusionQuery/VisApiOcclusionQueryObjectPixelCounter.hpp>

class VCoronaManager;
class VCoronaComponent;
class VOcclusionQueryObjectPixelCounterCorona;
typedef VSmartPtr<VCoronaComponent> VCoronaComponentPtr;


/// \brief
///   Derived occlusion query pixel counter object for corona visibility determination.
class VOcclusionQueryObjectPixelCounterCorona : public VOcclusionQueryObjectPixelCounter
{
public:
  VOcclusionQueryObjectPixelCounterCorona();
  ~VOcclusionQueryObjectPixelCounterCorona();

  EFFECTS_IMPEXP virtual bool Render(VOcclusionQuery &query, const hkvAlignedBBox &safeBox);

private:
  VCoronaComponent *m_pCorona;

  friend class VCoronaComponent;
};


/// \brief
///   An instance of this class defines a corona effect instance.
///   This component can be attached to any VisLightSource_cl.
///   It only works on platforms that support occlusion queries.
class VCoronaComponent : public IVObjectComponent
{
  friend class VCoronaManager;
public: 
  ///
  /// @name Constructor
  /// @{
  ///

  /// \brief
  ///   Component constructor
  EFFECTS_IMPEXP VCoronaComponent(int iComponentFlags=VIS_OBJECTCOMPONENTFLAG_NONE);

  /// \brief
  ///   Component destructor
  EFFECTS_IMPEXP virtual ~VCoronaComponent();

  ///
  /// @}
  ///

  ///
  /// @name Corona Properties
  /// @{
  ///

  /// \brief
  ///   Enables or disables the component
  inline void SetEnabled(bool bStatus=true) {Enabled=bStatus;}

  /// \brief
  ///   Indicates whether the component is currently enabled
  inline bool IsEnabled() const {return Enabled==TRUE;}

  /// \brief
  ///   Set the filter bitmask for rendering only in some contexts.
  /// 
  /// This corona will only be rendered in render contexts, if the logical AND result of this
  /// bitmask and the context's bitmask (see VisRenderContext_cl::SetRenderFilterMask) is not zero.
  /// By default, all bits are set.
  /// 
  /// \param iMask
  ///   The new filter bitmask
  /// 
  /// \sa VisRenderContext_cl::SetRenderFilterMask
  inline void SetVisibleBitmask(unsigned int iMask) { VisibleBitmask = iMask; }

  /// \brief
  ///   Get the filter bitmask that has been set via SetVisibleBitmask.
  inline unsigned int GetVisibleBitmask() const { return VisibleBitmask; }

  /// \brief
  ///   Set a custom corona texture for this lightsource
  /// 
  /// \param textureFile
  ///   Path to a corona texture file.
  EFFECTS_IMPEXP void SetCoronaTexture(const char *textureFile);

  /// \brief
  ///   Get corona texture file name
  /// 
  /// If this light source uses a corona, this function returns the file name of the corona
  /// texture.
  /// 
  /// \return
  ///   const char *: The corona texture file name, otherwise NULL.
  EFFECTS_IMPEXP const char *GetCoronaTextureFileName() const;

  /// \brief
  ///   Returns a pointer to the VTextureObject of the corona.
  ///
  /// If no texture is set, this will return NULL;
  ///
  /// \return
  ///   Returns a pointer to the VTextureObject of the corona.
  EFFECTS_IMPEXP VTextureObject* GetCoronaTexture ();

  /// \brief
  ///   Set the size of the check block for the corona occlusion query.
  /// 
  /// The size is specified as the radius in pixels. A larger radius allows more fine-grained
  /// fading of the corona, but it also means that irrelevant geometry may affect the
  /// corona visibility more easily. 
  /// 
  /// Typically, larger coronas (see VCoronaComponent::SetCoronaScale) should have larger check
  /// blocks than small coronas.
  /// 
  /// \param iRadiusPixels
  ///   Radius in Pixels.
  ///
  /// \sa VCoronaComponent::GetCheckBlockSize
  /// \sa VCoronaComponent::SetCoronaScale
  inline void SetCheckBlockSize(BYTE iRadiusPixels) {QueryRadius = hkvMath::Min( hkvMath::Max(iRadiusPixels, 1), 255); }

  /// \brief
  ///   Get the size of the check block for the occlusion query of this corona.
  /// 
  /// The value can be set using the VCoronaComponent::SetCheckBlockSize method.
  /// 
  /// \return
  ///   Radius in Pixels.
  ///
  /// \sa VCoronaComponent::SetCheckBlockSize
  inline BYTE GetCheckBlockSize() const {return QueryRadius;}

  /// \brief
  ///   Set the scale and rotation mode for this light source's corona
  /// 
  /// The passed parameter are a combination of flags defined in VisCoronaScaleMode_e:
  ///
  /// \li VIS_CORONASCALE_DISTANCE: Scale the corona's size according to distance, similar to regular 
  ///   scene geometry.
  ///
  /// \li VIS_CORONASCALE_VISIBLEAREA : Scale the corona's size according to the percentage of its check
  ///   block size that is currently visible. 
  ///
  /// \li VIS_CORONASCALE_USEFADEOUT : Fade out the corona according to the parameters globally set with 
  ///   the static VCoronaComponent::SetGlobalCoronaFadeOutDistance method. 
  ///
  /// \li VIS_CORONASCALE_ROTATING : If this flag is set, the corona will rotate as the camera moves 
  ///   relative to the light source.
  /// 
  /// VIS_CORONASCALE_NONE is passed, all the aforementioned features will be disabled.
  /// 
  /// \param iScaleMode
  ///   Corona Scale mode.
  ///
  /// \sa VisCoronaScaleMode_e
  inline void SetCoronaScaleMode(unsigned char iScaleMode) {CoronaFlags = iScaleMode;}

  /// \brief
  ///   Get the scale and rotation mode for this light source's corona
  /// 
  /// The value can be set using the VisLightSource_cl::SetCoronaScaleMode method.
  /// 
  /// \return
  ///   VisCoronaScaleMode_e eScaleMode: Corona Scale mode.
  inline unsigned char GetCoronaScaleMode() const {return CoronaFlags;}

  /// \brief
  ///   Set the scaling factor for this light source's corona
  /// 
  /// Greater values than 1.0f result in larger coronas. Smaller values than 1.0f result in smaller coronas.
  /// 
  /// The default value is 1.0f.
  /// 
  /// \param fScale
  ///   Corona size scaling factor.
  inline void SetCoronaScale(float fScale) {CoronaScaling = fScale;}

  /// \brief
  ///   Get the scaling factor for this light source's corona
  /// 
  /// The scaling factor can be set using the VisLightSource_cl::SetCoronaScale method.
  /// 
  /// \return
  ///   float fScale: Corona size scaling factor.
  inline float GetCoronaScale() const {return CoronaScaling;}

  /// \brief
  ///   Sets the time in ms that is needed for the corona to become fully visible.
  inline void SetPreGlowMS(unsigned int iPreGlowMS) {PreGlowMS = iPreGlowMS;}

  /// \brief
  ///   Returns the time in ms that is needed for the corona to become fully visible.
  inline unsigned int GetPreGlowMS() const {return PreGlowMS;}

  /// \brief
  ///   Sets the time in ms that is needed for the corona to vanish completely.
  inline void SetAfterGlowMS(unsigned int iAfterGlowMS) {AfterGlowMS = iAfterGlowMS;}

  /// \brief
  ///   Returns the time in ms that is needed for the corona to vanish completely.
  inline unsigned int GetAfterGlowMS() const {return AfterGlowMS;}

  /// \brief
  ///   Sets the depth bias of the occlusion query. Negative values will move the query closer to the camera.
  inline void SetDepthBias(float fDepthBias) {DepthBias = fDepthBias;}

  /// \brief
  ///   Returns the depth bias of the occlusion query. 
  inline float GetDepthBias() const {return DepthBias;}

  /// \brief
  ///   Sets the OnlyVisibleInSpotLight flag. This flag only influences the Corona, when it is attached to a spotlight. If it is true, the Corona is only visible if the camera is inside the cone defined by the spotlight. Please also note that the projected texture is not used to calculate the intensity of the corona. Instead a simple falloff function is used.
  inline void SetOnlyVisibleInSpotLight(bool bOnlyVisibleInSpotLight) {OnlyVisibleInSpotLight = bOnlyVisibleInSpotLight;}

  /// \brief
  ///   Gets the OnlyVisibleInSpotLight flag. This flag only influences the Corona, when it is attached to a spotlight. If it is true, the Corona is only visible if the camera is inside the cone defined by the spotlight. Please also note that the projected texture is not used to calculate the intensity of the corona. Instead a simple falloff function is used.
  inline bool GetOnlyVisibleInSpotLight() const {return OnlyVisibleInSpotLight==TRUE;}

  /// \brief
  ///   Sets the fade out start distance of the corona. 
  ///
  /// If the distance to the light is below 'FadeOutStart' the corona is not faded and
  /// if it is over 'FadeOutEnd' the corona is not rendered at all.
  /// If 'FadeOutEnd' is set to zero, fading is disabled.
  ///
  /// \param iFadeOutStart
  ///   New start distance of the fade out.
  ///
  /// \sa VCoronaComponent::SetFadeOutEnd
  inline void SetFadeOutStart(float iFadeOutStart) {FadeOutStart = iFadeOutStart;}

  /// \brief
  ///   Returns the fade out start distance of the corona.
  inline float GetFadeOutStart() const {return FadeOutStart;}

  /// \brief
  ///   Sets the fade out end distance of the corona. 
  ///
  /// If the distance to the light is below 'FadeOutStart' the corona is not faded and
  /// if it is over 'FadeOutEnd' the corona is not rendered at all.
  /// If 'FadeOutEnd' is set to zero, fading is disabled.
  ///
  /// \param iFadeOutEnd
  ///   New end distance of the fade out. Zero disables fading.
  ///
  /// \sa VCoronaComponent::SetFadeOutEnd
  inline void SetFadeOutEnd(float iFadeOutEnd) {FadeOutEnd = iFadeOutEnd;}

  /// \brief
  ///   Returns the fade out end distance of the corona.
  inline float GetFadeOutEnd() const {return FadeOutEnd;}

  /// \brief
  ///   Returns the current visibility of the corona.
  ///
  /// \return
  ///   Visibility of the corona between 0.0 and 1.0.
  EFFECTS_IMPEXP float GetCurrentVisibility() const;

  ///
  /// @}
  ///


  ///
  /// @name Static Corona Settings
  /// @{
  ///

  /// \brief
  ///   Sets the current settings for global corona fade-out.
  ///
  /// These settings are used if the flag VIS_CORONASCALE_USEFADEOUT is set via
  /// SetCoronaScaleMode. Otherwise, the local parameters of the component are used.
  /// By default, distance fadeout is disabled, by setting both values to 0.
  ///
  /// \param fStartDistance
  ///   Fadeout start distance in world units.
  /// 
  /// \param fEndDistance
  ///   Fadeout end distance in world units.
  ///
  /// \return
  ///   true if successful, otherwise false (e.g. fEndDistance is smaller than
  ///   fStartDistance)
  ///
  /// \sa VCoronaComponent::GetGlobalCoronaFadeOutDistance
  /// \sa VCoronaComponent::SetCoronaScaleMode
  EFFECTS_IMPEXP static bool SetGlobalCoronaFadeOutDistance(float fStartDistance, float fEndDistance);

  /// \brief
  ///   Gets the current settings for global corona fade-out.
  /// 
  /// These settings are used if the flag VIS_CORONASCALE_USEFADEOUT is set via
  /// SetCoronaScaleMode. Otherwise, the local parameters of the component are used.
  /// By default, distance fadeout is disabled, by setting both values to 0.
  ///
  /// \param out_fStartDistance
  ///   Variable that will receive the fadeout start distance in world units.
  /// 
  /// \param out_fEndDistance
  ///   Variable that will receive the fadeout end distance in world units.
  /// 
  /// \sa VCoronaComponent::SetGlobalCoronaFadeOutDistance
  /// \sa VCoronaComponent::SetCoronaScaleMode
  EFFECTS_IMPEXP static void GetGlobalCoronaFadeOutDistance(float &out_fStartDistance, float &out_fEndDistance);

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
  V_DECLARE_SERIAL_DLLEXP( VCoronaComponent, EFFECTS_IMPEXP)

  /// \brief
  ///   RTTI macro to add a variable table
  V_DECLARE_VARTABLE(VCoronaComponent, EFFECTS_IMPEXP)

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

  VOcclusionQueryObjectPixelCounterCorona m_CoronaPixelCounter; 
  VTextureObjectPtr m_spTexture;

  int m_iIndex; 

  // Corona properties
  VString CoronaTexture;
  float CoronaScaling;
  unsigned int PreGlowMS;
  unsigned int AfterGlowMS;
  float DepthBias;
  float FadeOutStart;
  float FadeOutEnd;
  unsigned int VisibleBitmask;
  BYTE QueryRadius;
  BYTE CoronaFlags;
  BOOL Enabled;
  BOOL OnlyVisibleInSpotLight;

private:
  static float s_fGlobalFadeOutStart;
  static float s_fGlobalFadeOutEnd;
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
