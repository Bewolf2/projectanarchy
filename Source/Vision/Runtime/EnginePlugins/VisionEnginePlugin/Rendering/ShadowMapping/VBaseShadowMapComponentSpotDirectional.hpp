/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VBaseShadowMapComponentSpotDirectional.hpp

#ifndef VBASESHADOWMAPCOMPONENT_SPOTDIRECTIONAL_HPP_INCLUDED
#define VBASESHADOWMAPCOMPONENT_SPOTDIRECTIONAL_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/ShadowMapping/IVShadowMapComponent.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/EffectsModule.hpp>

/// \brief
///   Base shadow map component for spot and directional lights.
///
/// This base class adds common functionality, used for spot and directional lights, to the IVShadowMapComponent 
/// base class.
/// 
/// \sa IVShadowMapComponent
class VBaseShadowMapComponentSpotDirectional : public IVShadowMapComponent
{
public:

  /// \brief
  ///  Different options for selecting the cascades
  enum VCascadeSelectionMethod_e
  {
    CSM_SELECT_BY_INTERVAL,
    CSM_SELECT_BY_BOUNDINGBOX,
    CSM_SELECT_BY_INTERVAL_DEPTH,
    CSM_NO_SELECTION
  };

  ///
  /// @name Initialization
  /// @{
  ///

  /// \brief
  ///   Constructor taking the associated renderer node index as input.
  EFFECTS_IMPEXP VBaseShadowMapComponentSpotDirectional(int iRendererNodeIndex);

  /// \brief
  ///   Constructor.
  EFFECTS_IMPEXP VBaseShadowMapComponentSpotDirectional();

  /// \brief
  ///   Destructor.
  EFFECTS_IMPEXP virtual ~VBaseShadowMapComponentSpotDirectional();

  ///
  /// @}
  ///

  ///
  /// @name Overridden functions
  /// @{
  ///

  /// \brief
  ///   Overridden from base class.
  EFFECTS_IMPEXP virtual BOOL CanAttachToObject(VisTypedEngineObject_cl *pObject, VString &sErrorMsgOut) HKV_OVERRIDE;

  /// \brief
  ///   Overridden from base class.
  EFFECTS_IMPEXP virtual bool SetVariable(const char *szName, const char *szValue) HKV_OVERRIDE;

#if defined(WIN32) || defined(_VISION_DOC)

  /// \brief
  ///   Overridden from base class.
  EFFECTS_IMPEXP virtual void GetVariableAttributes(VisVariable_cl* pVariable, VVariableAttributeInfo& destInfo) HKV_OVERRIDE;

#endif

  /// \brief
  ///   Overridden from base class.
  EFFECTS_IMPEXP virtual void Serialize(VArchive &ar) HKV_OVERRIDE;

  /// \brief
  ///   Overridden from base class.
  EFFECTS_IMPEXP virtual void GetLightVolumeMeshBufferIndexAndCount(int iCascade, int& iIndex, int& iCount) HKV_OVERRIDE;

  ///
  /// @}
  ///

  ///
  /// @name Shadow Properties
  /// @{
  ///

  /// \brief
  ///   Returns the number of cascades for cascaded shadow mapping (between 1 and 4).
  inline unsigned int GetCascadeCount() const { return CascadeCount; }

  /// \brief
  ///   Sets the number of cascades for cascaded shadow mapping (must be between 1 and 4).
  ///
  /// Note that a higher cascade count generally requires more texture space. Additionally, please take a look
  /// at IVShadowMapComponent::SetShadowMapSize for further factors affecting the required texture space.
  /// It is generally recommended to try various combinations of shadow map size and cascade count to find the
  /// best trade-off between quality and speed. Typically, higher cascade counts increase demands on vertex
  /// throughput since they require additional rendering passes, whereas higher shadow map resolutions may
  /// have an adverse effect on texture cache hit rate and memory bandwidth.
  ///
  /// Only directional lights support multiple cascades.
  ///
  /// \param iCount
  ///   Number of cascades to use.
  EFFECTS_IMPEXP virtual void SetCascadeCount(unsigned int iCount);

  /// \brief
  ///   Returns the cascade selection method for cascaded shadow mapping.
  inline VCascadeSelectionMethod_e GetCascadeSelection() const { return CascadeSelection; }

  /// \brief
  ///   Sets the cascade selection method. 
  ///
  /// Interval based selection will select the cascade by the distance to the viewer. 
  /// Bounding box based selection will use the bounding box of the cascade to select the right one. 
  /// Bounding box selection will result in more effective texture space usage and thus in better 
  /// quality. However this method uses a more complicated shader than the interval based selection,
  /// especially when using more than two cascades.
  ///
  /// \param cascadeSelection
  ///   Cascade selection method to use.
  EFFECTS_IMPEXP virtual void SetCascadeSelection(VCascadeSelectionMethod_e cascadeSelection);

  /// \brief
  ///   Returns the range (in units) of the specified cascade.
  inline float GetCascadeRange(int iCascade = 0) const { return CascadeRange[iCascade]; }

  /// \brief
  ///   Sets the range (in units) of the specified cascade.
  ///
  /// The cascade range specifies the distance around the camera within which the cascade is in effect.
  /// The higher the cascade index, the larger the range has to be. Typically, each subsequent range should be
  /// significantly larger than the previous range (e.g. if cascade 0 has a range of 1000 units, cascade 1 could have
  /// a range of 3000 or 4000 units).
  ///
  /// \param iCascade
  ///   Cascade index for which to set the range
  ///
  /// \param fRange
  ///   Range to set (in units).
  EFFECTS_IMPEXP virtual void SetCascadeRange(int iCascade, float fRange);

  /// \brief
  ///   Returns whether cascade overestimation is enabled or disabled.
  inline BOOL GetOverestimateCascades() const { return OverestimateCascades; }

  /// \brief
  ///   Enables/disables cascade size overestimation
  ///
  /// If overestimation is disabled, the range of shadow cascades is strictly limited to the slice of the view
  /// frustum affected by the respective cascade. Since this causes the projection of the shadow map to change
  /// whenever the light source of the camera is moved, you may observe flickering shadows.
  ///
  /// By enabling cascade overestimation, you can avoid the flickering artifacts at the cost of a loss of precision
  /// and shadow map space. Instead of generating the shadow map projection as a tight fit around the relevant part
  /// of the view frustum, this will first compute a sphere around the view frustum part and then generate the projection
  /// to match that sphere. This will overestimate the cascade range, but it will avoid the changes in projection
  /// and the associated flickering.
  ///
  /// \param bOverestimate
  ///   TRUE to enable, FALSE (default) to disable overestimation.
  EFFECTS_IMPEXP virtual void SetOverestimateCascades(BOOL bOverestimate);

  /// \brief
  ///   Returns the value previously set with SetShadowMapCameraUpdateInterval.
  inline float GetShadowMapCameraUpdateInterval() const { return CameraUpdateInterval; }

  /// \brief
  ///   If cascade overestimation is enabled, this sets the distance the camera has to move before the shadow map projection is recomputed. Default is 200 units.
  EFFECTS_IMPEXP void SetShadowMapCameraUpdateInterval(float fInterval);

  /// \brief
  ///   Returns the value previously set with SetShadowMapCameraUpdateAngle.
  inline float GetShadowMapCameraUpdateAngle() const { return CameraUpdateAngle; }

  /// \brief
  ///   If cascade overestimation is enabled, this sets the angle the camera has to move before the shadow map projection is recomputed. Default is 5 degrees.
  EFFECTS_IMPEXP void SetShadowMapCameraUpdateAngle(float fAngle);

  /// \brief
  ///   Returns the distance at which shadows start to fade out
  inline float GetShadowFadeOutStart() const { return FadeOutStart; }

  /// \brief
  ///   Sets the distance at which shadows start to fade out. If this value is negative the fade out distance is calculated automatically.
  inline void SetShadowFadeOutStart(float fFadeOutStart) { FadeOutStart = fFadeOutStart; }

  /// \brief
  ///   Returns the distance at which shadows are completely faded out
  inline float GetShadowFadeOutEnd() const { return FadeOutEnd; }

  /// \brief
  ///   Sets the distance at which shadows are completely faded out. If this value is negative the fade out distance is calculated automatically.
  inline void SetShadowFadeOutEnd(float fFadeOutEnd) { FadeOutEnd = fFadeOutEnd; }

#ifndef _VISION_DOC

  EFFECTS_IMPEXP virtual void SetCascadeRangePtr(float* range, int size);

  V_DECLARE_VARTABLE(VBaseShadowMapComponentSpotDirectional, EFFECTS_IMPEXP);
  V_DECLARE_SERIAL_DLLEXP(VBaseShadowMapComponentSpotDirectional, EFFECTS_IMPEXP);

#endif

protected:
  // Vartable Properties
  unsigned int CascadeCount;
  VCascadeSelectionMethod_e CascadeSelection;
  float CascadeRange[MAX_SHADOW_PARTS_COUNT];
  BOOL OverestimateCascades;
  float CameraUpdateInterval;
  float CameraUpdateAngle;
  float FadeOutStart;
  float FadeOutEnd;
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
