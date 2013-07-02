/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file TimeOfDay.hpp

#ifndef TIMEOFDAY_HPP_INCLUDED
#define TIMEOFDAY_HPP_INCLUDED

class VColorCurve;
class VCurve2D;

typedef VSmartPtr<VCurve2D> VCurve2DPtr;
typedef VSmartPtr<VColorCurve> VColorCurvePtr;

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Particles/Curve.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/EffectsModule.hpp>
#if !defined( HK_ANARCHY )
  #include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/DeferredShading/DeferredShadingModule.hpp>
#endif


// Versions
#define TIMEOFDAY_VERSION_0          0                        // Initial version
#define TIMEOFDAY_VERSION_1          1                        // Added additonal sky layer for dusk
#define TIMEOFDAY_VERSION_2          2                        // fog range
#define TIMEOFDAY_VERSION_3          3                        // control sky flag, m_spDuskWeight  serialization
#define TIMEOFDAY_VERSION_4          4                        // fog start distance
#define TIMEOFDAY_VERSION_5          5                        // sun orientation vector
#define TIMEOFDAY_VERSION_6          6                        // height fog data
#define TIMEOFDAY_VERSION_7          7                        // removed weather enum
#define TIMEOFDAY_VERSION_CURRENT    TIMEOFDAY_VERSION_7      // Current version


/// \brief
///   Low-level default time of day functionality
///
/// This class contains all low-level time of day functionality, including the evaluation of
/// sun direction, light and fog setting depending on the color curves specified in vForge. It implements
/// all virtual functions of the IVTimeOfDay interface and adds custom features for the time of day
/// system, such as color curves for ambient light, sun light, and depth fog.
class VTimeOfDay : public IVTimeOfDay
{
public:

  /// \brief
  /// Constructor of the time of day class.
  /// 
  /// \param fDayTime
  /// Current time of day (0..1, mapped to 0..24h).
  /// 
  /// \param fSunRiseTime
  /// Time at which the sun rises (0..1, mapped to 0..24h).
  /// 
  /// \param fSunSetTime
  /// Time at which the sun sets (0..1, mapped to 0..24h).
  /// 
  EFFECTS_IMPEXP VTimeOfDay(float fDayTime = 0.f, float fSunRiseTime = 0.25f, float fSunSetTime = 0.75f);

  /// \brief
  /// Destructor of the time of day class.
  /// 
  EFFECTS_IMPEXP virtual ~VTimeOfDay();

  /// \brief
  /// Sets the orientation of the direction in which the sun is passing the sky
  /// 
  EFFECTS_IMPEXP void SetSunPathOrientation(float fYaw, float fPitch, float fRoll);

  /// \brief
  /// Sets the flags whether the old or the new sun orientation style should be used
  /// 
  EFFECTS_IMPEXP void SetOldOrientationStyle(bool bEnabled) { m_bOldOrientationStyle = bEnabled; }

  /// \brief
  /// Evaluates the sun direction at the current time of day.
  /// 
  /// \param dir
  /// Out: Sun direction.
  /// 
  EFFECTS_IMPEXP virtual void EvaluateSunDir(hkvVec3& dir);

  /// \brief
  /// Evaluates the sun direction at a given time of day.
  /// 
  /// \param dir
  /// Out: Sun direction.
  /// 
  /// \param fTime
  /// In: Time of day.
  /// 
  EFFECTS_IMPEXP virtual void EvaluateSunDirAtDaytime(hkvVec3& dir, float fTime);

  /// \brief
  /// Evaluates the sun color and sky intensities at the current time of day.
  /// 
  /// \param falloff
  /// Falloff, between 0 and 1.
  /// 
  /// \param iColor
  /// Out: Sun Color
  /// 
  /// \param fDawnWeight
  /// Out: Weight of the dawn sky texture (0..1).
  /// 
  /// \param fDuskWeight
  /// Out: Weight of the dusk sky texture (0..1).
  /// 
  /// \param fNightWeight
  /// Out: Weight of the night sky texture (0..1).
  /// 
  EFFECTS_IMPEXP virtual void EvaluateColorValue(float falloff, VColorRef &iColor, float &fDawnWeight, float &fDuskWeight, float &fNightWeight);

  /// \brief
  /// Evaluates the fog parameters at the current time of day.
  /// 
  /// \param iFogColor
  /// Out: Fog Color.
  /// 
  /// \param fogStart
  /// Out: Fog Start distance.
  /// 
  /// \param fogEnd
  /// Out: Fog End distance.
  /// 
  EFFECTS_IMPEXP virtual void EvaluateDepthFog(VColorRef &iFogColor, float &fogStart, float &fogEnd);

  /// \brief
  /// Evaluates the ground fog parameters at the current time of day.
  /// 
  /// \param iFogColor
  /// Out: Fog Color.
  /// 
  /// \param fogStart
  /// Out: Fog Start distance.
  /// 
  /// \param fogEnd
  /// Out: Fog End distance.
  /// 
  /// \param fogDensity
  /// Out: Fog density.
  /// 
  /// \param halfDensityHeight
  /// Out: The height at which the density falls off to half of its maximum value.
  EFFECTS_IMPEXP virtual void EvaluateHeightFog(VColorRef &iFogColor, float &fogStart, float &fogEnd, float &fogDensity, float &halfDensityHeight);

  /// \brief
  /// Updates fog parameters
  EFFECTS_IMPEXP virtual void UpdateFogParameters();

  /// \brief
  /// Sets the times at which the sun rises/sets.
  /// 
  /// \param fSunRiseTime
  /// Time at which the sun rises (0..1, mapped to 0..24h).
  /// 
  /// \param fSunSetTime
  /// Time at which the sun sets (0..1, mapped to 0..24h).
  /// 
  inline void SetSunTime(float fSunRiseTime, float fSunSetTime) { m_fSunRiseTime = fSunRiseTime; m_fSunSetTime = fSunSetTime; }

  /// \brief
  /// Sets the current time of day.
  /// 
  /// \param fTime
  /// Current time of day to set (0..1, mapped to 0..24h).
  /// 
  EFFECTS_IMPEXP virtual void SetDayTime(float fTime);

  /// \brief
  /// Modifies the current time of day.
  /// 
  /// \param fTimeDiff
  /// Time difference to apply (-1..+1, mapped to -24h..+24h).
  /// 
  EFFECTS_IMPEXP virtual void IncDayTime(float fTimeDiff) { bool bIsDay = IsDay(); SetDayTime(GetDayTime() + fTimeDiff); m_bDayNightSwitch = (bIsDay != IsDay()); }

  /// \brief
  /// Returns the current time of day.
  /// 
  /// \returns
  /// float fTime: Current time of day (0..1, mapped to 0..24h).
  /// 
  EFFECTS_IMPEXP virtual float GetDayTime();

  /// \brief
  /// Returns the time at which the sun rises.
  /// 
  /// \returns
  /// float fTime: The time at which the sun rises (0..1, mapped to 0..24h).
  /// 
  inline float GetSunRiseTime() const { return m_fSunRiseTime; }

  /// \brief
  /// Returns the time at which the sun sets.
  /// 
  /// \returns
  /// float fTime: The time at which the sun sets (0..1, mapped to 0..24h).
  /// 
  inline float GetSunSetTime() const { return m_fSunSetTime; } 

  /// \brief
  /// Returns whether it is currently after sunrise and before sunset.
  /// 
  /// \returns
  /// bool bIsDay: true if it is currently after sunrise and before sunset.
  /// 
  inline bool IsDay() const { return ((m_fDayTime - m_fSunRiseTime > 0.f) && (m_fDayTime - m_fSunSetTime < 0.f)); }

  /// \brief
  /// Returns whether the last call to IncDayTime resulted in a switch between day and night time.
  /// 
  /// \returns
  /// bool bSwitched: true if a switch occurred.
  /// 
  inline bool DayNightSwitched() const { return m_bDayNightSwitch; }

  /// \brief
  /// Sets the color curve for ambient light.
  /// 
  /// \param ambientColor
  /// Ambient Light Color Curve.
  /// 
  inline void SetAmbientColorCurve(VColorCurve* ambientColor) { m_spAmbientColor = ambientColor; }

  /// \brief
  /// Returns the color curve for ambient light.
  /// 
  /// \returns
  /// Color curve for ambient light.
  /// 
  inline VColorCurve* GetAmbientColorCurve() const { return m_spAmbientColor; }

  /// \brief
  /// Sets the color curve for full-bright sun light.
  /// 
  /// \param fullbrightColor
  /// Full-bright sun light Color Curve.
  /// 
  inline void SetFullbrightColorCurve(VColorCurve* fullbrightColor) { m_spFullbrightColor = fullbrightColor; }

  /// \brief
  /// Returns the color curve for full-bright sun light.
  /// 
  /// \returns
  /// Full-bright sun light Color Curve.
  /// 
  inline VColorCurve* GetFullbrightColorCurve() const { return m_spFullbrightColor; }

  /// \brief
  /// Sets the weight curve for the contribution of the dawn sky texture.
  /// 
  /// \param dawnWeight
  /// Weight curve for the dawn sky texture.
  /// 
  inline void SetDawnWeightCurve(VCurve2D *dawnWeight) { m_spDawnWeight = dawnWeight; }

  /// \brief
  /// Returns the weight curve for the contribution of the dawn sky texture.
  /// 
  /// \returns
  /// Weight curve for the dawn sky texture.
  /// 
  inline VCurve2D* GetDawnWeightCurve() const { return m_spDawnWeight; }

  /// \brief
  /// Sets the weight curve for the contribution of the dusk sky texture.
  /// 
  /// \param duskWeight
  /// Weight curve for the dusk sky texture.
  /// 
  inline void SetDuskWeightCurve(VCurve2D *duskWeight) { m_spDuskWeight = duskWeight; }

  /// \brief
  /// Returns the weight curve for the contribution of the dusk sky texture.
  /// 
  /// \returns
  /// Weight curve for the dusk sky texture.
  /// 
  inline VCurve2D* GetDuskWeightCurve() const { return m_spDuskWeight; }

  /// \brief
  /// Sets the weight curve for the contribution of the night sky texture.
  /// 
  /// \param nightWeight
  /// Weight curve for the night sky texture.
  /// 
  inline void SetNightWeightCurve(VCurve2D *nightWeight) { m_spNightWeight = nightWeight; }

  /// \brief
  /// Returns the weight curve for the contribution of the night sky texture.
  /// 
  /// \returns
  /// Weight curve for the night sky texture.
  /// 
  inline VCurve2D* GetNightWeightCurve() const { return m_spNightWeight; }

  /// \brief
  /// Returns the status whether this time of day instance should take control over the dynamic sky layer weighting. If disabled, the user is responsible for changing the sky dynamically
  inline bool GetControlSky() const 
  {
    return m_bControlSky;
  }

  /// \brief
  /// Sets the status whether this time of day instance should take control over the dynamic sky layer weighting. If disabled, the user is responsible for changing the sky dynamically
  inline void SetControlSky(bool bStatus) 
  {
    m_bControlSky = bStatus;
  }

  /// \brief
  /// Sets the curve for the fog start distance.
  /// 
  /// \param fogStart
  /// The fog start distance curve.
  /// 
  inline void SetFogStartCurve(VCurve2D *fogStart) { m_spFogStart = fogStart; }

  /// \brief
  /// Returns the curve for the fog start distance. Curve values are in [0..1] range and are multiplied with the fog range (SetFogRange)
  /// 
  /// \returns
  /// The fog start distance curve.
  /// 
  inline VCurve2D* GetFogStartCurve() const { return m_spFogStart; }

  /// \brief
  /// Sets the curve for the fog end distance. Curve values are in [0..1] range and are multiplied with the fog range (SetFogRange)
  /// 
  /// \param fogEnd
  /// The fog end distance curve.
  /// 
  inline void SetFogEndCurve(VCurve2D* fogEnd) { m_spFogEnd = fogEnd; }

  /// \brief
  /// Returns the curve for the fog end distance.
  /// 
  /// \returns
  /// The fog end distance curve.
  /// 
  inline VCurve2D* GetFogEndCurve() const { return m_spFogEnd; }

  /// \brief
  /// Sets the fog color curve.
  /// 
  /// \param fogColor
  /// The fog color curve.
  /// 
  inline void SetFogColorCurve(VColorCurve* fogColor) { m_spFogColor = fogColor; }

  /// \brief
  /// Returns the fog color curve.
  /// 
  /// \returns
  /// The fog color curve.
  /// 
  inline VColorCurve* GetFogColorCurve() const { return m_spFogColor; }


  /// \brief
  /// Sets the fog range (in world units) that is used to scale the relative fog curve values
  inline void SetFogRange(float fRange)
  {
    m_fFogRange = fRange;
  }

  /// \brief
  /// Returns the fog range (in world units) that is used to scale the relative fog curve values
  inline float GetFogRange() const
  {
    return m_fFogRange;
  }

  /// \brief
  /// Sets the ground fog range (in world units) that is used to scale the relative fog curve values
  inline void SetHeightFogRange(float fRange)
  {
    m_fHeightFogRange = fRange;
  }

  /// \brief
  /// Returns the ground fog range (in world units) that is used to scale the relative fog curve values
  inline float GetHeightFogRange() const
  {
    return m_fHeightFogRange;
  }


  /// \brief
  /// Sets the fog start distance in world units. This distance corresponds to the bottom of the fog distance curve
  inline void SetFogStartDistance(float fDist)
  {
    m_fFogStartDistance = fDist;
  }

  /// \brief
  /// Returns the fog start distance previously set with SetFogStartDistance
  inline float GetFogStartDistance() const
  {
    return m_fFogStartDistance;
  }

  /// \brief
  /// Sets the ground fog start distance in world units. This distance corresponds to the bottom of the ground fog distance curve
  inline void SetHeightFogStartDistance(float fDist)
  {
    m_fHeightFogStartDistance = fDist;
  }

  /// \brief
  /// Returns the fog start distance previously set with SetHeightFogStartDistance
  inline float GetHeightFogStartDistance() const
  {
    return m_fHeightFogStartDistance;
  }

  /// \brief
  /// Sets the ground fog density scale.
  inline void SetHeightFogDensityScale(float fDensityScale, float fHalfDensityHeightScale)
  {
    m_fHeightFogDensityScale = fDensityScale;
    m_fHeightFogHalfDensityHeightScale = fHalfDensityHeightScale;
  }

  /// \brief
  /// Returns the height at which the ground fog density falls off to half of its maximum value.
  inline float GetHeightFogDensityHeightScale() const
  {
    return m_fHeightFogHalfDensityHeightScale;
  }

  /// \brief
  /// Returns the ground fog falloff density scale.
  inline float GetHeightFogHalfDensityHeightScale() const
  {
    return m_fHeightFogHalfDensityHeightScale;
  }

  /// \brief
  /// Overridden virtual function.
  EFFECTS_IMPEXP virtual void GetSunDirection(hkvVec3& vDirection);

  /// \brief
  /// Overridden virtual function.
  EFFECTS_IMPEXP virtual void GetSunDirectionAtDayTime(hkvVec3& vDirection, float fTime);

  /// \brief
  /// Overridden virtual function.
  EFFECTS_IMPEXP virtual VColorRef GetSunColor();

  /// \brief
  /// Overridden virtual function.
  EFFECTS_IMPEXP virtual VColorRef GetAmbientColor();

  #ifndef _VISION_DOC
  V_DECLARE_SERIAL_DLLEXP( VTimeOfDay,  EFFECTS_IMPEXP );
  EFFECTS_IMPEXP virtual void Serialize( VArchive &ar );

  EFFECTS_IMPEXP void SwitchSceneLightSources();
  void DeInit();

private:
  void SetDefaultCurves();

public:

  float m_fDayTime;
  float m_fSunRiseTime;
  float m_fSunSetTime;  
  bool m_bDayNightSwitch;
  bool m_bControlSky;
  bool m_bOldOrientationStyle;

  //color curves
  VColorCurvePtr m_spAmbientColor;
  VColorCurvePtr m_spFullbrightColor;
  VCurve2DPtr m_spDawnWeight;
  VCurve2DPtr m_spDuskWeight;
  VCurve2DPtr m_spNightWeight;

  //fog curves
  float m_fFogRange, m_fFogStartDistance;
  VCurve2DPtr m_spFogStart;
  VCurve2DPtr m_spFogEnd;
  VColorCurvePtr m_spFogColor;

  // ground fog curves
  float m_fHeightFogRange, m_fHeightFogStartDistance, m_fHeightFogDensityScale, m_fHeightFogHalfDensityHeightScale;
  VCurve2DPtr m_spHeightFogStart;
  VCurve2DPtr m_spHeightFogEnd;
  VCurve2DPtr m_spHeightFogDensity;
  VCurve2DPtr m_spHeightFogHalfDensityHeight;
  VColorCurvePtr m_spHeightFogColor;

  hkvVec3 m_vSunPathEuler;
  hkvMat3 m_SunPathOrientation;

  #endif  // _VISION_DOC

};


/// \brief
///   An instance of this class is globally installed via Vision::Renderer.SetTimeOfDayFactory to supply the editor with default instances of class VTimeOfDay
class VTimeOfDayFactory : public IVObjectInstanceFactory
{
public:
  VTimeOfDayFactory() {}
  virtual ~VTimeOfDayFactory() {}

  /// \brief
  ///   Implements the interface function
  EFFECTS_IMPEXP virtual VTypedObject *CreateObjectInstance(VType *pExpectedBaseType)
  {
    return new VTimeOfDay;
  }

  static VTimeOfDayFactory g_Instance;

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
