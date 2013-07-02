/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiLightSource.hpp

#ifndef DEFINE_VISAPILIGHTSOURCE
#define DEFINE_VISAPILIGHTSOURCE
#include "string.h"

#include <Vision/Runtime/Engine/SceneElements/VisApiConvexVolume.hpp>
#include <Vision/Runtime/Engine/System/ModuleSystem/VisApiModuleSystemChild.hpp>
#include <Vision/Runtime/Engine/System/ElementManager/VisApiElementManager.hpp>
#include <Vision/Runtime/Engine/Visibility/VisApiElementVisData.hpp>
#include <Vision/Runtime/Engine/SceneElements/VisApiObject3D.hpp>
#include <Vision/Runtime/Engine/System/VisApiDefs.hpp>
#include <Vision/Runtime/Engine/Renderer/Texture/VisApiBitmap.hpp>
#include <Vision/Runtime/Engine/Network/IVisApiNetworkManager.hpp>

class VisLightSrc_AnimIntensity_cl;
class VisLightSrc_AnimColor_cl;
class VisConvexVolume_cl;
class VisRenderContext_cl;
class VisStaticGeometryInstance_cl;
class VisLightSrcCollection_cl;

/////////////////////////////////////////////////////////////////////////////////////////////////////
// Known trigger actions for MessageFunction
/////////////////////////////////////////////////////////////////////////////////////////////////////
#define VISLIGHTSOURCETRIGGER_TOGGLEON  "ToggleOn"
#define VISLIGHTSOURCETRIGGER_TOGGLEOFF "ToggleOff"

/// \brief
///   Structure which contains information about a light source
/// 
/// It is not necessarily corresponding to any instance of a VisLightSource_cl class. This
/// structure is used for methods in the RenderLoopHelper_cl class, in places where it is possible
/// and useful to dynamically define custom lights without having to create a new
/// VisLightSource_cl instance.
struct LightSrcInfo_t 
{
  LightSrcInfo_t() : m_ColorVal(V_RGBA_WHITE)
  {
    m_Type = VIS_LIGHT_POINT;
    m_fIntensity = 1000.f;
    m_fRange = 200.f;
    m_fAngle = 0.f;
    m_uiLightPercentage = 100;
  }

  hkvVec3 m_vPosition;
  hkvVec3 m_vDirection;
  float m_fIntensity;
  float m_fRange;
  float m_fAngle;
  unsigned char m_uiLightPercentage;
  VColorRef m_ColorVal;
  
  VisLightSourceType_e m_Type;
};

/// \brief
///   Class for static and dynamic light sources
/// 
/// Each instance of the VisLightSource class is a light source with a set of properties, like
/// position, orientation, intensity, color, radius, and the like.
/// 
/// The Vision Engine supports two light source types: Point lights and spot lights. Since spot
/// lights allow specifying a custom projection texture, they also serve as projected lights.
/// 
/// The VisLightSource_cl class provides a lot of convenient features to handle lighting in a
/// real-time rendering environment. The actual rendering is performed in the VisionRenderLoop
/// class, which accesses many of the light properties defined in the VisLightSource_cl class.
/// 
/// For a better understanding of the lower-level implications of lighting in the Vision engine,
/// please refer to the API documentation and to the VisionRenderLoop class.
/// 
/// Lights can either be dynamic (meaning that they can be moved and altered in runtime, and all
/// the illumination is computed in runtime), static (meaning that they can not be altered
/// dynamically; such lights are typically defined in the .V3D file, and their lighting
/// contribution is computed in the vLux preprocessing tool), or static with modifiable properties
/// (only the color and intensity can be modified in runtime; the lights are defined in the .V3D
/// file, and the maximum values of their lighting contributions are precomputed in vLux).
/// 
/// Additional classes (VisLightSrc_AnimColor_cl and VisLightSrc_AnimIntensity_cl) are provided as
/// convenience features for color and intensity animations of light sources.
class VisLightSource_cl : public VisObject3D_cl, public VisElementManager_cl<class VisLightSource_cl *>
{
  V_DECLARE_SERIAL_DLLEXP( VisLightSource_cl, VISION_APIDATA )

public:
  
  ///
  /// @name Constructors / Destructor
  /// @{
  ///

  /// \brief
  ///   Constructor of the light source class
  /// 
  /// The constructor of the light source class creates a new light source and initialises it
  /// according to the provided properties (type and intensity)
  /// 
  /// The attenuation mode is VIS_LIGHT_ATTEN_CUSTOM.
  /// 
  /// \param lType
  ///   Type of the light source, available values are:
  ///   \li VIS_LIGHT_POINT: standard point light source
  /// 
  ///   \li VIS_LIGHT_SPOTLIGHT:    spotlight with standard apex angle of 45 degrees
  /// 
  /// \param intensity
  ///   Intensity of the light source, will be mapped to radius for VIS_LIGHT_ATTEN_CUSTOM.
  VISION_APIFUNC VisLightSource_cl(VisLightSourceType_e lType, float intensity);
  
  
  /// \brief
  ///   Destructor of the light source class
  /// 
  /// Use the DisposeObject function instead of directly deleting the light source.
  VISION_APIFUNC virtual ~VisLightSource_cl();
  

  /// \brief
  ///   DEPRECATED; Use DisposeObject() instead
  inline void Remove()
  {
    DisposeObject();
  }
 
  /// \brief
  ///   Overridden VisTypedEngineObject_cl function to remove this instance from the scene.
  VISION_APIFUNC VOVERRIDE void DisposeObject();

  ///
  /// @}
  ///

  ///
  /// @name Lightsource Properties
  /// @{
  ///

  /// \brief
  ///   Sets the color of the light source.
  /// 
  /// This function will only affect dynamic lights or modifiable static lights.
  /// 
  /// The color of static lights cannot be changed.
  /// 
  /// \param iColor
  ///   Color value that defines the new light color (RGB). Use the V_RGB macro to create a color.
  VISION_APIFUNC void SetColor(VColorRef iColor);

  /// \brief
  ///   Returns the color of the light source.
  /// 
  /// \return
  ///   VColorRef iColor : The current color value (RGB)
  VISION_APIFUNC VColorRef GetColor() const;

  /// \brief
  ///   Sets the intensity of the light source.
  /// 
  /// Lights with VIS_LIGHT_ATTEN_CUSTOM:
  /// \li Same as calling SetRadius
  /// 
  /// \li The intensity is the same as the radius of the light in world units
  /// 
  /// \li Overbright effects can be achieved with SetMultiplier
  /// 
  /// Lights with VIS_LIGHT_ATTEN_1OVERX :
  /// \li Not supported anymore
  /// 
  /// Relevant for dynamic lights and modifiable static lights.
  /// 
  /// \param fNewIntensity
  ///   The new intensity value. 0.f to turn off the light.
  /// 
  /// \note
  ///   SetIntensity can also be used for switchable/animated static light sources. Be aware of the
  ///   fact that switchable light sources are turned off by default and have to be turned on with
  ///   the VisLightSource_cl::SetStatus method at first. Setting the intensity on a turned off
  ///   static light source will not have any effect.
  /// 
  /// \note
  ///   It is not allowed to call SetIntensity on light sources which are currently performing
  ///   intensity animations. The result of such an operation is undefined. Stop the intensity
  ///   animation before manually modifying the intensity of animated light sources.
  /// 
  /// \note
  ///   For switchable/animated static light sources, the valid intensity range is between 0 and
  ///   the original light source intensity specified in the editor.
  /// 
  /// \sa VisLightSource_cl::GetIntensity()
  inline void SetIntensity(float fNewIntensity)
  {  
    SetRadius(fNewIntensity);
  }

  /// \brief
  ///   Gets the current intensity of the light source.
  /// 
  /// \return
  ///   float fIntensity: The current light source intensity.
  /// 
  /// \sa VisLightSource_cl::SetIntensity()
  VISION_APIFUNC float GetIntensity() const;

  /// \brief
  ///   Sets the multiplication factor that is applied to the intensity of the light.
  /// 
  /// Only relevant for dynamic lights with VIS_LIGHT_ATTEN_CUSTOM. Used to scale the overall
  /// brightness of a light source. The factor may be negative, resulting in a darkening of
  /// the environment.
  /// 
  /// \param fMultiplier
  ///   The new multiplication factor.
  /// 
  /// \sa VisLightSource_cl::GetMultiplier()
  inline void SetMultiplier(float fMultiplier)
  {
    m_fMultiplier = fMultiplier;
  }
  
  /// \brief
  ///   Gets the multiplication factor that is applied to the intensity of the light.
  /// 
  /// Only relevant for dynamic lights with VIS_LIGHT_ATTEN_CUSTOM
  /// 
  /// Only relevant for dynamic lights with VIS_LIGHT_ATTEN_CUSTOM. Used to scale the overall
  /// brightness of a light source. The factor may be negative, resulting in a darkening of
  /// the environment.
  /// 
  /// \return
  ///   The currently set multiplication factor.
  /// 
  /// \sa VisLightSource_cl::SetMultiplier()
  inline float GetMultiplier() const
  {
    return m_fMultiplier;
  }

  /// \brief
  ///   (De-)activates distance based fading. It is activated for fFadeEnd > 0.f and fFadeStart<=fFadeEnd; Distances are passed in world coordinates
  inline void SetFadeDistances(float fFadeStart, float fFadeEnd)
  {
    VASSERT(fFadeEnd>=0.f && fFadeStart<=fFadeEnd);
    m_fFadeOutStart = fFadeStart;
    m_fFadeOutEnd = fFadeEnd;
  }

  /// \brief
  ///   Retrieves the distance fade start and fade end values.
  inline void GetFadeDistances(float &fFadeStart, float &fFadeEnd)
  {
    fFadeStart = m_fFadeOutStart;
    fFadeEnd = m_fFadeOutEnd;
  }

  /// \brief
  ///   Returns a fade weight value [0..1] that represents the fading multiplier for the passed camera position if fading is defined.
  inline float GetFadeWeight(const hkvVec3& vCameraPos) const
  {
    if (m_fFadeOutEnd<=0.f) // fading is disabled
      return 1.f;
    hkvVec3 vDiff = GetPosition()-vCameraPos;
    const float fCamDist = vDiff.getLength();
    if (fCamDist >= m_fFadeOutEnd)
      return 0.f;
    if (m_fFadeOutEnd<=m_fFadeOutStart) // immediate switch for m_fFadeOutStart==m_fFadeOutEnd
      return 1.f;
    float fWeight = 1.f - (fCamDist-m_fFadeOutStart) / (m_fFadeOutEnd - m_fFadeOutStart);
    return hkvMath::Min(fWeight,1.f);
  }

  /// \brief
  ///   Indicates whether this light should be far clipped for the passed camera position
  inline bool IsFarClipped(const hkvVec3& vCameraPos) const
  {
    if (m_fFadeOutEnd<=0.f)
      return false;
    hkvVec3 vDiff = GetPosition()-vCameraPos;
    const float fCamDistSqr = vDiff.getLengthSquared ();
    return (fCamDistSqr >= m_fFadeOutEnd*m_fFadeOutEnd);
  }


  /// \brief
  ///   Sets the type of a dynamic light source
  /// 
  /// Relevant for dynamic lights only.
  /// 
  /// \param lType
  ///   The new light source type (VIS_LIGHT_POINT or VIS_LIGHT_SPOTLIGHT)
  VISION_APIFUNC void SetType(VisLightSourceType_e lType);
 
  /// \brief
  ///   Sets the apex angle of a spotlight or projected light, respectively.
  /// 
  /// If the light source is a point light, this method will have no effect.
  /// 
  /// By default, the apex angle is set to 45 degrees. An angle of 180 corresponds to a
  /// hemispherical illumination shape.
  /// 
  /// Relevant for dynamic lights only.
  /// 
  /// \param angle
  ///   Apex angle in degrees. Must be 0 < angle < 180.
  /// 
  /// \sa VisLightSource_cl::GetProjectionAngle
  VISION_APIFUNC void SetProjectionAngle(float angle);
  
  /// \brief
  ///   Gets the apex angle of a light source.
  /// 
  /// This function returns the apex angle of spotlights or projected lights, respectively.
  /// 
  /// \return
  ///   float fProjectionAngle: Current apex angle.
  /// 
  /// \sa VisLightSource_cl::SetProjectionAngle
  VISION_APIFUNC float GetProjectionAngle() const;

  /// \brief
  ///   Triggers the light source's intensity and/or color animations.
  /// 
  /// Running intensity and/or color animations will pause/continue.
  /// 
  /// Relevant for dynamic lights and static modifiable lights.
  VISION_APIFUNC void Trigger();

  /// \brief
  ///   Enables/Disables a light source dynamically
  ///
  /// Note: This function is deprecated. Light source can be toggled on/off with
  /// SetVisibleBitmask() instead. Current light status can be queried with GetVisibleBitmask().
  /// VisibleMask = 0xFFFFFFFF (-1) enables a light source.
  /// VisibleMask = 0x00000000 (0) disables a light source.
  /// 
  /// A light source gets disabled by setting the intensity to zero and memorizing the old intensity
  /// in case the light source will be enabled again.
  /// 
  /// Relevant for dynamic lights and static modifiable lights.
  /// 
  /// \param bStatus
  ///   TRUE = on, FALSE = off
  VISION_APIFUNC void SetStatus ( BOOL bStatus );

  /// \brief
  ///   Resets the modifiable light source to its original status.
  /// 
  /// This function resets the status (on/off) and the intensity animation of a static
  /// modifiable light source to the original state. 
  /// 
  /// \return
  ///   BOOL bResult: FALSE if the light source is a dynamic light.
  VISION_APIFUNC BOOL ResetModifiableLight();


  ///
  /// @}
  ///

  ///
  /// @name Attenuation and Fall-off
  /// @{
  ///

 
  /// \brief
  ///   Gets the attenuation type of the light source.
  /// 
  /// The attenuation type can be set with the VisLightSource_cl::SetAttenuation method.
  /// 
  /// \return
  ///   VisLightAttenuationType_e attType: Attenuation type of the light source.
  VISION_APIFUNC VisLightAttenuationType_e GetAttenuation() const { return m_eAttenuation;}
  
  /// \brief
  ///   Sets the attenuation type of the light source.
  /// 
  /// Supported attenuation types are VIS_LIGHT_ATTEN_CUSTOM (a "strong" attenuation  method for
  /// bright local lights with higher performance due to shorter light range).
  /// VIS_LIGHT_ATTEN_1OVERX is not supported anymore.
  /// 
  /// Lights by default use attenuation mode VIS_LIGHT_ATTEN_CUSTOM.
  /// 
  /// When using VIS_LIGHT_ATTEN_CUSTOM, radius and intensity are separate parameters both
  /// influencing the visual appearance of the light. This also means that the radius is not
  /// computed automatically, but has to be set with the VisLightSource_cl::SetRadius method.
  /// 
  /// The attenuation type can be queried with the VisLightSource_cl::GetAttenuation method.
  /// 
  /// \param eAttenuation
  ///   The attenuation type to use.
  VISION_APIFUNC void SetAttenuation(VisLightAttenuationType_e eAttenuation);
  
  /// \brief
  ///   Gets the attenuation texture.
  /// 
  /// This function returns the attenuation texture that controls how the light falls-off 
  /// with distance.
  /// 
  /// If no attentuation texture is set, the renderloop will use the smooth attentuation texture.
  /// 
  /// Only valid if VIS_LIGHT_ATTEN_CUSTOM is used.
  /// 
  /// \return
  ///   VTextureObject *pTex: Attenuation texture.
  inline VTextureObject *GetAttenuationTexture() const
  {
    return m_spAttenuationTex;
  }

  /// \brief
  ///   Sets the attenuation texture and the attenuation mode to VIS_LIGHT_ATTEN_CUSTOM.
  /// 
  /// \param pTex
  ///   Attenuation texture.
  VISION_APIFUNC void SetAttenuationTexture(VTextureObject *pTex);


  /// \brief
  ///   Sets the attenuation texture and the attenuation mode to VIS_LIGHT_ATTEN_CUSTOM.
  /// 
  /// \param pszFilename
  ///   Attenuation texture filename.
  /// 
  /// \return
  ///   Attenuation texture object.
  VISION_APIFUNC VTextureObject* SetAttenuationTexture(const char *pszFilename);


  
  ///
  /// @}
  ///

  ///
  /// @name Information Queries
  /// @{
  ///

  /// \brief
  ///   Returns true if the light source is a dynamic light source.
  /// 
  /// \return
  ///   BOOL result: TRUE if it is a dynamic light source, otherwise FALSE.
  /// 
  /// \sa VisLightSource_cl::IsModifiable
  inline BOOL IsDynamic() const
  {
    return !m_bIsStatic;
  }

  /// \brief
  ///   Returns whether this light's color and intensity properties can be modified.
  /// 
  /// A light is modifiable if ANY of the following properties apply to the light: 
  /// \li 1) The light is dynamic.
  ///
  /// \li 2) The light is triggerable.
  ///
  /// \li 3) The light has color- or intensity animation.
  /// 
  /// \return
  ///   BOOL result: TRUE, if the light is modifiable.
  /// 
  /// \sa VisLightSource_cl::IsDynamic
  VISION_APIFUNC BOOL IsModifiable() const;

  /// \brief
  ///   Sets the key of the light source. Wraps around SetObjectKey.
  /// 
  /// \param szNewKey
  ///   The new key. Can be a NULL pointer to remove the key.
  inline void SetKey(const char *szNewKey)
  {
    SetObjectKey(szNewKey);
  }

  /// \brief
  ///   Gets the key of the light source.
  /// 
  /// This function wraps around GetObjectKey of the base class.
  /// 
  /// The key can be set in vForge or via SetObjectKey. It can be used to seach for lights in the
  /// scene via key.
  /// 
  /// See Vision::Game.SearchLight.
  /// 
  /// \return
  ///   const char *key: The key of the light source.
  inline const char *GetKey() const
  {
    return GetObjectKey();
  }
    
  /// \brief
  ///   Gets the trigger status of the light source.
  /// 
  /// This function returns TRUE if the light source if currently triggered. Triggering can be used
  /// to pause and continue color and intensity animations.
  /// 
  /// Only relevant for dynamic and static modifiable lights.
  /// 
  /// \return
  ///   BOOL triggered: The trigger status of the light source.
  VISION_APIFUNC BOOL GetTriggered() const;
  
  /// \brief
  ///   Gets the type of the light source.
  /// 
  /// This function returns the current type of the light source (either VIS_LIGHT_POINT or
  /// VIS_LIGHT_SPOTLIGHT).
  /// 
  /// \return
  ///   VisLightSourceType_e lType: Type of the light source, available types are:
  ///   \li VIS_LIGHT_POINT:            Standard point light source
  /// 
  ///   \li VIS_LIGHT_SPOTLIGHT:        Spotlight or projected light
  VISION_APIFUNC VisLightSourceType_e GetType() const;

  /// \brief
  ///   Returns if specular lighting is enabled for this light source.
  /// 
  /// This function returns the flag that indicates if specular lighting is enabled for
  /// this light source.
  /// 
  /// \return
  ///   bool bUseSpecular: flags if specular calculations are performed for that lightsource
  inline bool GetUseSpecular() const
  {
    return m_bUseSpecular;
  }

  /// \brief
  ///   Sets the flag that defines if specular highlights are calculated for that light source.
  /// 
  /// \param bSpecular
  ///   Flag which enables/disables specular
  inline void SetUseSpecular(bool bSpecular)
  {
    m_bUseSpecular = bSpecular;
  }
  
  /// \brief
  ///   Returns the radius (range) of a light source.
  /// 
  /// For lights with attenuation type VIS_LIGHT_ATTEN_CUSTOM this method will return the value
  /// previously set with VisLightSource_cl::SetRadius.
  /// 
  /// \return
  ///   float fRadius: Radius (range) of the light source.
  inline float GetRadius() const
  { 
    return m_fRadius;
  }

  /// \brief
  ///   Sets the radius (range) of a light source. 
  /// 
  /// This has a direct effect for lights using attenuation type VIS_LIGHT_ATTEN_CUSTOM.
  /// 
  /// \param fRadius
  ///   Radius (range) of the light source.
  VISION_APIFUNC void SetRadius(float fRadius);
  
  ///
  /// @}
  ///

  ///
  /// @name Animation Functions
  /// @{
  ///

  /// \brief
  ///   Gets the intensity animation instance.
  /// 
  /// This function returns the reference to the intensity animation instance. Call this
  /// function to make use of the intensity animation class. All the available methods for the
  /// intensity animations can be found in the documentation of VisLightSrc_AnimIntensity_cl.
  /// 
  /// \return
  ///   VisLightSrc_AnimIntensity_cl &intensityAnimation: Reference to the intensity animation instance.
  /// 
  /// \example
  ///   \code
  ///   lightSource->IntensityAnimation().Pulsate(200, 0, 1.2f, 1.2f);
  ///   \endcode
  VISION_APIFUNC VisLightSrc_AnimIntensity_cl &IntensityAnimation();
  
  
  /// \brief
  ///   Gets the color animation instance.
  /// 
  /// ColorAnimation returns the reference to the color animation instance. Call this function to
  /// make use of the color animation class. All the available methods for the color animations can
  /// be found in the documentation of  VisLightSrc_AnimColor_cl.
  /// 
  /// \return
  ///   VisLightSrc_AnimColor_cl &colorAnimation: Reference to the color animation instance.
  /// 
  /// \example
  ///   \code
  ///   lightSource->ColorAnimation().Flicker(0, 255, 0, 0, 0, 255, 1.0f);
  ///   \endcode
  VISION_APIFUNC VisLightSrc_AnimColor_cl &ColorAnimation();

  /// \brief
  ///   Returns the pointer to the color animation instance. May be NULL.
  inline VisLightSrc_AnimColor_cl *GetColorAnimation() { return animColor; }

  /// \brief
  ///   Returns the pointer to the intensity animation instance. May be NULL.
  inline VisLightSrc_AnimIntensity_cl *GetIntensityAnimation() { return animIntensity; }


  /// \brief
  ///   Creates an instance of the intensity animation class.
  VISION_APIFUNC void EnableIntensityAnimations();


  /// \brief
  ///   Creates an instance of the color animation class.
  VISION_APIFUNC void EnableColorAnimations();


  /// \brief
  ///   Deletes the intensity animation and releases all the allocated memory.
  VISION_APIFUNC void DisableIntensityAnimations();

  
  /// \brief
  ///   Deletes the color animation and releases all the allocated memory.
  VISION_APIFUNC void DisableColorAnimations();


  ///
  /// @}
  ///

  ///
  /// @name Overrideable Module System Functions
  /// @{
  ///
    
  
  /// \brief
  ///   Uses the virtual function of the child system for visibility re-computations.
  VISION_APIFUNC void ModSysNotifyFunctionCommand(int command);
  
  /// \brief
  ///   Gets the filename of the projection texture.
  /// 
  /// If this is a light source that uses a projection texture (spot light), this function returns
  /// the file name of the texture being used.
  /// 
  /// \return
  ///   const char *: The projected texture file name, or NULL if no projection texture is used.
  VISION_APIFUNC const char *GetProjectedTextureFileName() const;

   
  ///
  /// @}
  ///


  ///
  /// @name Misc
  /// @{
  ///
  
  /// \brief
  ///   Sets bitmasks that define whether the light affects static geometry and/or entities.
  /// 
  /// With this feature you can e.g. create dynamic lights that only illuminate selected scene
  /// elements (e.g. only the main character, only entities, only static geometry, ... ).
  /// 
  /// For entities, the light's influence bitmask is AND-ed with the entity's influence bitmask
  /// (set via the VisBaseEntity_cl::SetLightInfluenceBitMask method). If the result is 0, the
  /// light will not affect the entity.
  /// 
  /// For static geometry, there is currently no option to set the bitmask on a per-primitive
  /// basis. Instead, the light's bit mask will always be compared to 1, so if the lowest bit of
  /// the light influence bitmask is set, static geometry will be affected, otherwise it won't.
  /// 
  /// Naturally, even if the bitmask test for a specific scene element/light source combination
  /// succeeds, the light will only illuminate the geometry if the respective scene element is
  /// actually within the light's range.
  /// 
  /// Only relevant for dynamic lights.
  /// 
  /// \param iWorldMask
  ///   Bitmask for static geometry (currently only 0 and 1 should be passed).
  /// 
  /// \param iObjMask
  ///   Bitmask for entities.
  /// 
  /// \note
  ///   The default values are 1 for both bitmasks.
  /// 
  /// \sa VisLightSource_cl::GetLightInfluenceBitMasks
  /// \sa VisBaseEntity_cl::SetLightInfluenceBitMask
  /// \sa VisBaseEntity_cl::GetLightInfluenceBitMask
  VISION_APIFUNC void SetLightInfluenceBitMasks(unsigned int iWorldMask, unsigned int iObjMask);

  /// \brief
  ///   Returns the influence bit masks previously set with SetLightInfluenceBitMasks.
  /// 
  /// \param iWorldMask
  ///   Bitmask for static geometry.
  /// 
  /// \param iObjMask
  ///   Bitmask for entities.
  /// 
  /// \sa VisLightSource_cl::SetLightInfluenceBitMasks
  /// \sa VisBaseEntity_cl::SetLightInfluenceBitMask
  /// \sa VisBaseEntity_cl::GetLightInfluenceBitMask
  VISION_APIFUNC void GetLightInfluenceBitMasks(unsigned int &iWorldMask, unsigned int &iObjMask) const;

  /// \brief
  ///   Returns the bitmask used to determine whether this light source affects static geometry.
  /// 
  /// \sa VisLightSource_cl::SetLightInfluenceBitMasks
  /// \sa VisBaseEntity_cl::SetLightInfluenceBitMask
  /// \sa VisBaseEntity_cl::GetLightInfluenceBitMask
  VISION_APIFUNC unsigned int GetLightInfluenceBitMaskWorld() const;

  /// \brief
  ///   Returns the bitmask used to determine whether this light source affects entities.
  /// 
  /// \sa VisLightSource_cl::SetLightInfluenceBitMasks
  /// \sa VisBaseEntity_cl::SetLightInfluenceBitMask
  /// \sa VisBaseEntity_cl::GetLightInfluenceBitMask
  VISION_APIFUNC unsigned int GetLightInfluenceBitMaskEntity() const;

  /// \brief
  ///   Sets a custom technique to be used when rendering geometry is affected by this light source.
  /// 
  /// Typically, this has to be queried in render loop implementations to determine which technique
  /// to use for dynamic illumination of scene geometry. It is recommended to take a look at the
  /// default VisionRenderLoop implementation as a reference.
  /// 
  /// Only relevant for dynamic lights.
  /// 
  /// \param pTechnique
  ///   Custom light technique to be used. Pass NULL to use the default technique again.
  VISION_APIFUNC void SetCustomTechnique(VCompiledTechnique *pTechnique);

  /// \brief
  ///   Returns the technique previously set with VisLightSource_cl::SetCustomTechnique.
  /// 
  /// Only relevant for dynamic lights.
  /// 
  /// \return
  ///   VCompiledTechnique *pTechnique: Custom light technique to be used.
  VISION_APIFUNC VCompiledTechnique *GetCustomTechnique() const;

  /// \brief
  ///   Fills the passed LightSrcInfo_t structure with the data from this light source.
  /// 
  /// LightSrcInfo_t structures are required as input parameter by various methods of the
  /// VisRenderLoopHelper_cl class.
  /// 
  /// \param lightInfo
  ///   Reference to a LightSrcInfo_t structure that is to be filled.

  VISION_APIFUNC void GetLightSrcInfo(LightSrcInfo_t &lightInfo) const;

  /// \brief
  ///   Returns a bounding box around the sphere of influence of this light source.
  /// 
  /// Returns a bounding box that completely encompasses the sphere of influence of the light
  /// source.
  /// 
  /// \param box
  ///   Reference to a hkvAlignedBBox object that will be filled with the light's bounding box.
  VISION_APIFUNC void GetBoundingBox(hkvAlignedBBox& box) const;

  /// \brief
  ///   Sets the projection texture to be used for a spotlight.
  /// 
  /// This overrides the default spotlight projection texture.
  /// 
  /// \param pTex
  ///   Projection texture object.
  VISION_APIFUNC void SetProjectionTexture(VTextureObject *pTex);

  /// \brief
  ///   Sets the projection texture to be used for a spotlight.
  /// 
  /// This overrides the default spotlight projection texture.
  /// 
  /// \param szTexName
  ///   File name of the projection texture, including path.
  VISION_APIFUNC void SetProjectionTexture(const char *szTexName);

  /// \brief
  ///   Gets the projection texture of a spotlight.
  /// 
  /// Returns the projection texture of a spot light source as previously set by using
  /// one of the SetProjectionTexture overloads.
  /// 
  /// \return
  ///   VTextureObject *pTex: Pointer to the projection texture object.
  VISION_APIFUNC VTextureObject *GetProjectionTexture() const;

  /// \brief
  ///   Gets the projection planes of a spot light source.
  /// 
  /// Projection planes can be used e.g. in vertex/pixel shaders to compute texture coordinates for
  /// projective texture mapping. A good example for this are the dynamic spot light shaders provided 
  /// with the Vision SDK.
  /// 
  /// \param RefPlaneX
  ///   Reference to hkvPlane object which will be filled with the texture plane information in the light's local-space x direction.
  /// 
  /// \param RefPlaneY
  ///   Reference to hkvPlane object which will be filled with the texture plane information in the light's local-space y direction.
  /// 
  /// \param RefPlaneZ
  ///   Reference to hkvPlane object which will be filled with the texture plane information in the light's local-space z direction.
  ///   The RefPlaneZ also encodes the opening angle of the cone.
  VISION_APIFUNC void GetProjectionPlanes(hkvPlane& RefPlaneX, hkvPlane& RefPlaneY, hkvPlane& RefPlaneZ) const;

  /// \brief
  ///   Evaluates the lightgrid contribution of this light source at a specific world position.
  /// 
  /// This is useful to generate light grid replacement colors for a scene object that is
  /// influenced by dynamic lights.
  /// 
  /// \param vPos
  ///   The world position to generate the lightgrid colors for.
  /// 
  /// \param pDestColors
  ///   Pointer to 6 colors that receive the color values. The colors are added to this array.
  VISION_APIFUNC void GetLightGridColorsAtPosition(const hkvVec3& vPos, hkvVec3* pDestColors);

  /// \brief
  ///   Gets the visibility status of this light source in the last rendered frame.
  /// 
  /// If the object was classified as visible in the last frame, the function returns TRUE.
  /// 
  /// \param pContext
  ///   Render Context for which the visibility status is to be queried. If the context is NULL,
  ///   the currently active render context is used.
  /// 
  /// \return
  ///   BOOL status : The visibility status of the light in the last frame.
  /// 
  /// \note
  ///   Calling this function is rather cheap, since the visibility status has already been
  ///   calculated.
  VISION_APIFUNC BOOL WasVisibleLastFrame(VisRenderContext_cl *pContext) const;

  /// \brief
  ///   Gets the visibility status of this light source in the last rendered frames of all active
  ///   render contexts.
  /// 
  /// If the light has been visible in the last rendered frames in any of the active render contexts,
  /// the function returns TRUE. 
  /// 
  /// \return
  ///   BOOL status : The visibility status of the light.
  /// 
  /// \note
  ///   Calling this function is rather cheap, since the visibility status has already been
  ///   calculated.
  VISION_APIFUNC BOOL WasVisibleInAnyLastFrame() const;

  /// \brief
  ///   Sets the filter bitmask for rendering this light only in some contexts.
  /// 
  /// This light will only be rendered in render contexts, if the result of the logical AND
  /// operation of this bitmask and the context's bitmask (see VisRenderContext_cl::SetRenderFilterMask) 
  /// is not zero.
  /// 
  /// For compatibility reasons, there are some predefined bitmask constants, which can also be
  /// used for lights:
  ///
  /// \li VIS_ENTITY_INVISIBLE : Defined as 0, so the light will not be visible anywhere. 
  /// 
  /// \li VIS_ENTITY_VISIBLE_IN_WORLD : The engine will compare against this bit when rendering in the main scene.
  ///
  /// \li VIS_ENTITY_VISIBLE_IN_TEXTURE : The engine will compare against this bit when rendering to a texture.
  ///
  /// \li VIS_ENTITY_VISIBLE_IN_CUBEMAP : The engine will compare against this bit when rendering to a cubemap. 
  ///
  /// \li VIS_ENTITY_VISIBLE : Defined as a bit combination of all bits to flag a light as relevant for all contexts.
  /// 
  /// By default, the VIS_ENTITY_VISIBLE bitmask is set.
  /// 
  /// \param iMask
  ///   The new filter bitmask.
  /// 
  /// \sa VisRenderContext_cl::SetRenderFilterMask
  inline void SetVisibleBitmask(unsigned int iMask) { m_iVisibleMask = iMask; }

  /// \brief
  ///   Gets the filter bitmask that has been set via SetVisibleBitmask.
  inline unsigned int GetVisibleBitmask() const { return m_iVisibleMask; }

  /// \brief
  ///   Static function that returns the global list of all light sources of type VIS_LIGHT_DIRECTED.
  VISION_APIFUNC static const VisLightSrcCollection_cl& GetGlobalDirectionalLightList();

  /// \brief
  ///   Static function that returns the global list of all light sources of that are animated.
  VISION_APIFUNC static const VisLightSrcCollection_cl& GetGlobalAnimatedLightList();

  /// \brief
  ///   Static function that updates all types of light source animations. See VisionApp_cl::OnUpdateScene().
  VISION_APIFUNC static void HandleAllLightSources(float dtime);

  /// \brief
  ///   Helper function that returns the virtual position of the light in the passed context that is used for coronas.
  // For non-directional lights this is the same as GetPosition.
  VISION_APIFUNC void GetVirtualPosition(hkvVec3& vPosition, const VisRenderContext_cl *pContext) const;

  /// \brief
  /// Returns true, if this light-source should always be exported, even if it is a static light.
  VISION_APIFUNC bool IsRelevantForExport() const;

  /// \brief
  /// Returns true, if this light-source has a Clip Volume Component.
  VISION_APIFUNC bool HasClipVolumeComponent() const;

  /// \brief
  /// Returns true, if this light-source has a Shadow Map Component.
  VISION_APIFUNC bool HasShadowMapComponent() const;

  ///
  /// @name Deprecated
  /// @{
  ///

  /// \brief
  ///   DEPRECATED: This function now maps to a VCoronaComponent which is created if none exists.
  ///   Sets a custom corona texture for this light source.
  /// 
  /// \param textureFile
  ///   Path to a corona texture file.
  HKV_DEPRECATED_2012_1 VISION_APIFUNC void SetCoronaTexture(const char *textureFile);

  /// \brief
  ///   DEPRECATED: This function now maps to a VCoronaComponent which is created if none exists.
  ///   Enables/Disables corona for this light source.
  /// 
  /// By default coronas are enabled.
  /// 
  /// \param status
  ///   If TRUE, this light source will have a corona.
  HKV_DEPRECATED_2012_1 VISION_APIFUNC void SetFlag_Corona(BOOL status);

  /// \brief
  ///   DEPRECATED: This function now maps to a VLensFlareComponent which is created if none exists.
  ///   Sets the flag which defines whether the light source has lens flares.
  /// 
  /// By default, lens flares are disabled.
  /// 
  /// \param status
  ///   If TRUE, this light source will produce a lens flare.
  HKV_DEPRECATED_2012_1 VISION_APIFUNC void SetFlag_LensFlares(BOOL status);  

  /// \brief
  ///   DEPRECATED: This function now maps to a VCoronaComponent which is created if none exists.
  ///   Indicates whether this light source has a corona.
  HKV_DEPRECATED_2012_1 VISION_APIFUNC BOOL HasCorona();

  /// \brief
  ///   DEPRECATED: This function now maps to a VLensFlareComponent which is created if none exists.
  ///   Indicates whether this light source uses lens flares.
  HKV_DEPRECATED_2012_1 VISION_APIFUNC BOOL HasLensFlare();

  /// \brief
  ///   DEPRECATED: This function now maps to a VCoronaComponent which is created if none exists.
  ///   Sets the size of the check block for coronas and lens flares for this light source.
  /// 
  /// The size is specified as the radius in pixels. A larger radius allows more fine-grained
  /// fading of coronas and lens flares, but it also means that irrelevant geometry may affect the
  /// corona/lens flare visibility more easily. 
  /// 
  /// Typically, larger coronas (see VisLightSource_cl::SetCoronaScale) should have larger check
  /// blocks than small coronas.
  /// 
  /// As a special case, radius 0 can be passed to bypass the visibility test and always render the
  /// corona.
  /// 
  /// \param iRadiusPixels
  ///   Radius in Pixels.
  HKV_DEPRECATED_2012_1 VISION_APIFUNC void SetCheckBlockSize(unsigned short iRadiusPixels);

  /// \brief
  ///   DEPRECATED: This function now maps to a VCoronaComponent which is created if none exists.
  ///   Gets the size of the check block for coronas and lens flares for this light source.
  /// 
  /// The value can be set using the VisLightSource_cl::SetCheckBlockSize method.
  /// 
  /// \return
  ///   unsigned short iRadiusPixels: Radius in Pixels.
  HKV_DEPRECATED_2012_1 VISION_APIFUNC unsigned short GetCheckBlockSize();
  
  /// \brief
  ///   DEPRECATED: This function now maps to a VCoronaComponent which is created if none exists.
  ///   Sets the scale and rotation mode for this light source's corona.
  /// 
  /// The passed parameter is an enum providing the following flags:
  ///
  /// \li VIS_CORONASCALE_DISTANCE: Scale the corona's size according to distance, similar to regular 
  ///   scene geometry.
  ///
  /// \li VIS_CORONASCALE_VISIBLEAREA : Scale the corona's size according to the percentage of its check
  ///   block size that is currently visible. 
  ///
  /// \li VIS_CORONASCALE_USEFADEOUT : Fade out the corona according to the parameters globally set with 
  ///   the VCoronaComponent::SetGlobalCoronaFadeOutDistance method. 
  ///
  /// \li VIS_CORONASCALE_ROTATING : If this flag is set, the corona will rotate as the camera moves 
  ///   relative to the light source.
  /// 
  /// VIS_CORONASCALE_NONE is passed, all the aforementioned features will be disabled.
  /// 
  /// \param eScaleMode
  ///   Corona Scale mode.
  HKV_DEPRECATED_2012_1 VISION_APIFUNC void SetCoronaScaleMode(VisCoronaScaleMode_e eScaleMode);

  /// \brief
  ///   DEPRECATED: This function now maps to a VCoronaComponent which is created if none exists.
  ///   Gets the scale and rotation mode for this light source's corona.
  /// 
  /// The value can be set using the VisLightSource_cl::SetCoronaScaleMode method.
  /// 
  /// \return
  ///   VisCoronaScaleMode_e eScaleMode: Corona Scale mode.
  HKV_DEPRECATED_2012_1 VISION_APIFUNC VisCoronaScaleMode_e GetCoronaScaleMode();

  /// \brief
  ///   DEPRECATED: This function now maps to a VCoronaComponent which is created if none exists.
  ///   Sets the scaling factor for this light source's corona.
  /// 
  /// Greater values than 1.0f result in larger coronas. Smaller values than 1.0f result in smaller coronas.
  /// 
  /// The default value is 1.0f.
  /// 
  /// \param fScale
  ///   Corona size scaling factor.
  HKV_DEPRECATED_2012_1 VISION_APIFUNC void SetCoronaScale(float fScale);

  /// \brief
  ///   DEPRECATED: This function now maps to a VCoronaComponent which is created if none exists.
  ///   Gets the scaling factor for this light source's corona.
  /// 
  /// The scaling factor can be set using the VisLightSource_cl::SetCoronaScale method.
  /// 
  /// \return
  ///   float fScale: Corona size scaling factor.
  HKV_DEPRECATED_2012_1 VISION_APIFUNC float GetCoronaScale();
  
  /// \brief
  ///   DEPRECATED: This function now maps to a VCoronaComponent which is created if none exists.
  ///   Gets corona texture file name.
  /// 
  /// If this light source uses a corona, this function returns the file name of the corona
  /// texture.
  /// 
  /// \return
  ///   const char *: The corona texture file name, otherwise NULL.
  HKV_DEPRECATED_2012_1 VISION_APIFUNC const char *GetCoronaTextureFileName();

  /// \brief
  ///   Helper function for deprecated corona functions. Will create a corona component if none exists.
  VISION_APIFUNC IVObjectComponent* GetCoronaComponent();

  /// \brief
  ///   Helper function for deprecated lensflare functions. Will create a lensflare component if none exists.
  VISION_APIFUNC IVObjectComponent* GetLensFlareComponent();

  ///
  /// @}
  ///

  ///
  /// @name Network related
  /// @{
  ///

  VISION_APIFUNC int GetSynchronizationGroupList(const VNetworkViewContext &context, VNetworkSynchronizationGroupInstanceInfo_t *pDestList);

  ///
  /// @}
  ///

  // internal functions; do not use!
  inline void FlagAsStatic(bool bStatus) {m_bIsStatic=bStatus;}
  inline void SetStaticGeometryInstanceList(VisStaticGeometryInstance_cl **pList) { m_pStaticGeometryInstanceList = pList; }
  inline VisStaticGeometryInstance_cl **GetStaticGeometryInstanceList() { return m_pStaticGeometryInstanceList; }
  inline int GetStaticGeometryInstanceCount() { return m_iStaticGeometryInstanceCount; }
  inline void SetStaticGeometryInstanceCount(int iCount) { m_iStaticGeometryInstanceCount=iCount; }
  inline BOOL AffectsStaticGeometryInstance(const VisStaticGeometryInstance_cl *pInst) const
  {
    for (int i=0;i<m_iStaticGeometryInstanceCount;i++)
      if (m_pStaticGeometryInstanceList[i]==pInst)
        return TRUE;
    return FALSE;
  }
  inline bool AddAffectedStaticGeometryInstance(VisStaticGeometryInstance_cl *pInst)
  {
    VASSERT(!AffectsStaticGeometryInstance(pInst));
    if (!m_pStaticGeometryInstanceList)
      return false;
    VASSERT(m_pStaticGeometryInstanceList[m_iStaticGeometryInstanceCount]==NULL); 
    m_pStaticGeometryInstanceList[m_iStaticGeometryInstanceCount++] = pInst;
    return true;
  }

  void AddAffectedStaticGeometryInstanceUnique(VisStaticGeometryInstance_cl *pInst);
  inline void ResetStaticGeometryInstanceList()
  {
    m_iStaticGeometryInstanceCount = 0;
    V_SAFE_DELETE_ARRAY(m_pStaticGeometryInstanceList);
  }


  /// \brief
  ///   Static helper function to find a light source by its unique ID
  VISION_APIFUNC static VisLightSource_cl *FindByUID(__int64 iUID);

  ///
  /// @}
  ///

protected:
  VISION_APIFUNC VisLightSource_cl(); // serialization
  friend class VisLightMask_cl;
  friend class VisStaticGeometryInstance_cl;

  void SetInvisible(BOOL status);
  BOOL GetInvisible() const;

  friend VisLightSource_cl* CreateLight(int light, int style, const char *key);

  /// \brief
  ///   Overridden VisObject3D_cl function. Gets called when position or orientation changes.
  VISION_APIFUNC VOVERRIDE void OnObject3DChanged(int iO3DFlags);

  /// \brief
  ///   Overridden VisObject3D_cl function. Used to process trigger messages.
  VISION_APIFUNC VOVERRIDE void MessageFunction(int iID, INT_PTR iParamA, INT_PTR iParamB);

  /// \brief
  ///   Updates the core engine data according to the current Object3D data.
  VISION_APIFUNC void UpdateCoreData();


  /// \brief
  ///   Internal function. Initializes a new light source.
  void Init(const hkvVec3& origin, VisLightSourceType_e lType, float intensity, char *lKey);


  /// \brief
  ///   Internal function. Reinitializes a de-serialized (or newly created) light source.
  void ReInit(float fIntensity);


  /// \brief
  ///   Internal function. This function will be called internally by the engine each frame in order to do update of animation.
  void Handle();

  
  /// \brief
  ///   Loads a corona texture for a light source.
  /// 
  /// Use this function to load a corona texture for a light source.
  /// 
  /// \param szTexName
  ///   The name of the texture file to load
  /// 
  /// \return
  ///   int: The texture number under which it was loaded
  int LoadCoronaTexture( const char *szTexName );

  /// \brief
  ///   Serializes the object.
  /// 
  /// This function stores all important data into an archive or reads from it to create a new
  /// object.
  /// 
  /// For more information about serialization, see the Vision API Documentation.
  /// 
  /// \param ar
  ///   The archive object to read from or write to.
  VISION_APIFUNC VOVERRIDE void Serialize( VArchive& ar );
  
  /// \brief
  ///   Setup the color and intensity animations for static light sources dependent on the specified
  ///   style.
  void SetupStaticLightSourceStyle( int iStyle );


  void UpdateDirectionalLightList(VisLightSourceType_e lType);
  void UpdateAnimatedLightList();

  BOOL LoadProjectedTexture( const char *szTexName );

  VisStaticGeometryInstance_cl **m_pStaticGeometryInstanceList;
  int m_iStaticGeometryInstanceCount;

  unsigned int m_iVisibleMask;

  bool m_bIsStatic;                                 ///< if true light source is static, otherwise it is dynamic (serialized)
  bool m_bTriggered;                                ///< trigger status (serialized)

  float oldI;                                       ///< old "stored" intensity, used for pausing animations (serialized)
  hkvVec3 m_vDirection;                             ///< direction the light source points to (serialized)
  float m_fProjectionFactor;                        ///< ~ tan of the current opening angle (cached when setting the angle)
  //VString m_Key;                                  ///< light source key (serialized)
  int style;                                        ///< animation style for static light sources (serialized)
  
  VisLightSourceType_e type;                        ///< light source type (serialized)
  VisLightAttenuationType_e m_eAttenuation;         ///< attenuation of the light (serialized)
  float m_fRadius;                                  ///< radius for custom technique (serialized as intensity)
  float m_fMultiplier;                              ///< multiplier factor for custom attenuation (serialized)
  VTextureObjectPtr m_spAttenuationTex;             ///< Custom attenuation curve (serialized)
  VisBitmapPtr m_spAttenuationLookup;               ///< cached bitmap for software brightness lookup
  VisLightSrc_AnimIntensity_cl *animIntensity;      ///< pointer to the intensity animation instance (serialized)
  VisLightSrc_AnimColor_cl *animColor;              ///< pointer to the color animation instance (serialized)
  int m_iOldVisibleMask;                            ///< stores the old visible bitmask (serialized)
  float m_fFadeOutStart, m_fFadeOutEnd;             ///< Parameter for distance based fading
  bool m_bUseSpecular;                              ///< flags if specular calculations are performed (serialized)

  static VisLightSrcCollection_cl s_pDirectionalLights; ///< global list of all directional lights
  static VisLightSrcCollection_cl s_pAnimatedLights;    ///< global list of all animated lights

  //VisElementVisData_cl visData;                     ///< visibility data for light source
  //VisElementVisData_cl coronaVisData;               ///< visibility data for corona
  
  // This is used in VisLightSource_cl::GetCoronaTextureFileName to store the string. Will be removed once the deprecated function is gone.
  VString m_sDeprecatedTempString;
  
public:


#ifdef SUPPORTS_SNAPSHOT_CREATION
  VISION_APIFUNC VOVERRIDE void GetDependencies(VResourceSnapshot &snapshot);
#endif

  //Internal use
  //VISION_APIFUNC VisElementVisData_cl* GetVisData() { return &visData; }
  //VISION_APIFUNC VisElementVisData_cl* GetCoronaVisData() { return &coronaVisData; }
};


VISION_ELEMENTMANAGER_TEMPLATE_DECL(VisLightSource_cl)


/// \brief
///   Internal class that defines the link between a light source and a lightmask luminance texture
class VisLightMask_cl
{
public:
  inline VisLightMask_cl()
  {
    m_pLight = NULL;
  }

  V_DECLARE_SERIALX( VisLightMask_cl, VISION_APIFUNC )
  VISION_APIFUNC void SerializeX( VArchive &ar, bool bLightByID=false );

#ifdef SUPPORTS_SNAPSHOT_CREATION
  VISION_APIFUNC void GetDependencies(VResourceSnapshot &snapshot, VManagedResource *pOwnerResource);
#endif
public:
  VTextureObjectPtr m_spLightMask;
  hkvVec4 m_vUVScaleOfs;
  VisLightSource_cl *m_pLight;
};


/// \brief
///   Implements IVNetworkSynchronizationGroup as a synchronization group that synchronizes lights. This version supports interpolation
///
/// There is one global instance: g_InstanceL
class VNetworkLightGroupI : public IVNetworkSynchronizationGroup
{
public:
  /// \brief
  ///   Data history
  class VHistoryDataLight : public IVNetworkSynchronizationGroupInstanceData
  {
  public:
    VHistoryDataLight () : m_iChangedDataFlags(0), m_iDataFlagsToSend(0)
    {
    }

    VisDataHistory_cl<float, 3> m_multiplierHistory;
    VisDataHistory_cl<float, 3> m_radiusHistory;
    VisDataHistory_cl<VColorRef, 3> m_colorHistory;
    BYTE m_iChangedDataFlags;
    BYTE m_iDataFlagsToSend;
  };

  /// \brief
  ///   Flags defining which data is being sent
  enum VChangedDataFlags
  {
    VCD_MULTIPLIER = V_BIT(0),
    VCD_RADIUS = V_BIT(1),
    VCD_COLOR = V_BIT(2),
    VCD_DataFlags = V_BIT(0) | V_BIT(1) | V_BIT(2),
  };

  VISION_APIFUNC VOVERRIDE const char *GetName() const {return "LightSettings";}
  VISION_APIFUNC VOVERRIDE VGroupTypeFlags_e GetUsageTypeFlags() const {return VGroupTypeBit_Light;}
  VISION_APIFUNC VOVERRIDE IVNetworkSynchronizationGroupInstanceData* CreatePerInstanceData() const {return (new VHistoryDataLight);}
  VISION_APIFUNC VOVERRIDE bool QuerySynchronize(const VNetworkViewContext& context, VNetworkSynchronizationGroupInstanceInfo_t &instanceInfo, VMessageSettings& out_paketSettings);
  VISION_APIFUNC VOVERRIDE void Synchronize(const VNetworkViewContext& context, VNetworkSynchronizationGroupInstanceInfo_t &instanceInfo, VArchive &ar);
  VISION_APIFUNC VOVERRIDE void TickFunction(const VNetworkViewContext &context, VNetworkSynchronizationGroupInstanceInfo_t &instanceInfo, float fTimeDelta);

  VISION_APIDATA static VNetworkLightGroupI g_InstanceL; ///< light color, multiplier and radius
};



/// \brief
///   Similar to VNetworkLightGroupI but without interpolation
///
/// There is one global instance: g_InstanceL
class VNetworkLightGroup : public IVNetworkSynchronizationGroup
{
public:

  VISION_APIFUNC VOVERRIDE const char *GetName() const {return "LightSettings";}
  VISION_APIFUNC VOVERRIDE VGroupTypeFlags_e GetUsageTypeFlags() const {return VGroupTypeBit_Light;}
  VISION_APIFUNC VOVERRIDE bool QuerySynchronize(const VNetworkViewContext& context, VNetworkSynchronizationGroupInstanceInfo_t &instanceInfo, VMessageSettings& out_paketSettings);
  VISION_APIFUNC VOVERRIDE void Synchronize(const VNetworkViewContext& context, VNetworkSynchronizationGroupInstanceInfo_t &instanceInfo, VArchive &ar);

  VISION_APIDATA static VNetworkLightGroup g_InstanceL; ///< light color, multiplier and radius
};

#ifndef DEFINE_VISAPILIGHTSOURCEANIMCOLOR
#include <Vision/Runtime/Engine/SceneElements/VisApiLightSourceAnimColor.hpp>
#endif

#ifndef DEFINE_VISAPILIGHTSOURCEANIMINTENSITY
#include <Vision/Runtime/Engine/SceneElements/VisApiLightSourceAnimIntensity.hpp>
#endif

#include <Vision/Runtime/Engine/SceneElements/VisApiLightSource.inl>

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
