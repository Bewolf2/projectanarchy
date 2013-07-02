/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiLightSourceAnimColor.hpp

#ifndef DEFINE_VISAPILIGHTSOURCEANIMCOLOR
#define DEFINE_VISAPILIGHTSOURCEANIMCOLOR

#include <Vision/Runtime/Engine/Renderer/Texture/VisApiBitmap.hpp>


/// \brief
///   Color animation class for light sources
/// 
/// This class provides convenience functionality for color animations for light sources. 
/// 
/// Color animation instances can be accessed through the VisLightSource_cl class using the
/// VisLightSource_cl::ColorAnimation method.
class VisLightSrc_AnimColor_cl
{
  V_DECLARE_SERIALX( VisLightSrc_AnimColor_cl, VISION_APIFUNC )
  friend class SerializationTest;
public:
  
  
  /// \brief
  ///   Light source animation type
  enum Type_e
  {
    NONE = 0,             ///< no animation
    FLICKER = 2,          ///< flicker animation
    PULSATE = 3,          ///< pulsate animation
    BITMAP_LOOKUP = 4     ///< animation curve defined by texture lookup
  };

    
  /// \brief
  ///   Stop the color animation
  /// 
  /// This function stops the color animation, the color remains at the last value.
  VISION_APIFUNC void Stop();
  

  /// \brief
  ///   Pause the color animation
  /// 
  /// This function pauses the animation, this means that the animation can be continued later on
  /// with the Continue method.
  /// 
  /// \sa Continue
  VISION_APIFUNC void Pause();
  

  /// \brief
  ///   Continue the paused color animation
  /// 
  /// This function continues an animation which has been halted before with the Pause function.
  /// 
  /// \sa Pause
  VISION_APIFUNC void Continue();


  /// \brief
  ///   Get the color animation type, that can be one of the follwing types:
  ///
  /// \li NONE :            No color animation
  ///
  /// \li FLICKER :         Flicker animation
  ///
  /// \li PULSATE :         Pulsate animation
  ///
  /// \li BITMAP_LOOKUP :   Animation curve defined by texture lookup
  /// 
  /// \return
  ///   Type_e: Type of color animation (see the Type_e enum).
  VISION_APIFUNC Type_e GetType() const;


  /// \brief
  ///   Serializes the object data
  /// 
  /// This function stores all important data into an archive or reads from it to setup an object.
  /// 
  /// For more information about serialization, see the Vision API Documentation.
  /// 
  /// \param ar
  ///   The archive object to read from or write to.
  VISION_APIFUNC void SerializeX( VArchive &ar );


  ///
  /// @name Custom Animation Curve
  /// @{
  ///

  /// \brief
  ///   Set the color (and intensity) animation curve via a lookup texture
  /// 
  /// A lookup texture will be used as a color curve in the following manner: The engine will run
  /// through the first texel line in a specified time. The (interpolated) texel color value at the
  /// horizontal position will be used to modulate with the original light color (SetAnimColor).
  /// The alpha channel will be used as a relative intensity to the initial intensity
  /// (SetAnimMaxIntensity): An alpha value of 0 will turn off the light whereas an alpha value of
  /// 255 will set the maximum intensity.
  /// 
  /// \param szTexFilename
  ///   Texture filename for the lookup. The filename must specify a valid texture filename. Using
  ///   compressed dds files is strictly not recommended since the texture data remains in system
  ///   memory and would have to be unpacked.
  /// 
  /// \param fPhaseTime
  ///   Time period in seconds for a single phase cycle. If set to 0.0f, the phase has to be set
  ///   manually via SetAnimPhaseShift.
  /// 
  /// \param fPhaseShift
  ///   The initial phase shift [0..1]
  /// 
  /// \return
  ///   BOOL bResult: TRUE if the curve could be set successfully, i.e if the texture could be
  ///   loaded .
  VISION_APIFUNC BOOL SetAnimCurve(const char *szTexFilename, float fPhaseTime, float fPhaseShift=0.f);

  
  /// \brief
  ///   Set the color (and intensity) animation curve via a lookup texture
  /// 
  /// This function works exactly as its overloaded version except for taking a lookup bitmap
  /// directly as input.
  /// 
  /// \param pLookup
  ///   Lookup bitmap for the texture lookup.
  /// 
  /// \param fPhaseTime
  ///   Time period in seconds for a single phase cycle. If set to 0.0f, the phase has to be set
  ///   manually via SetAnimPhaseShift. A negative value will play the aniamtion backwards.
  /// 
  /// \param fPhaseShift
  ///   The initial phase shift [0..1]
  /// 
  /// \return
  ///   BOOL bResult: TRUE, if the curve could be set successfully, i.e if the texture is loaded. 
  VISION_APIFUNC BOOL SetAnimCurve(VisBitmap_cl *pLookup, float fPhaseTime, float fPhaseShift=0.f);


  /// \brief
  ///   Set the maximum intensity for the animation curve
  /// 
  /// The lookup alpha will be modulated with this intensity.
  /// 
  /// This value only affects the color animation curve.
  /// 
  /// \param fMaxIntensity
  ///   The maximum intensity.
  VISION_APIFUNC void SetAnimMaxIntensity(float fMaxIntensity);


  /// \brief
  ///   Set the color for the light curve animation
  /// 
  /// This color will be modulated with the lookup color in the curve.
  /// 
  /// This allows using the same lookup curve for lights with different colors.
  /// 
  /// This color only affects the light for custom color animation curve.
  /// 
  /// \param red
  ///   Red portion of the light color.
  /// 
  /// \param green
  ///   Green portion of the light color.
  /// 
  /// \param blue
  ///   Blue portion of the light color.
  VISION_APIFUNC void SetAnimColor(UBYTE red, UBYTE green, UBYTE blue);


  /// \brief
  ///   Set the color for the light curve animation
  /// 
  /// This color will be modulated with the lookup color in the curve. 
  /// 
  /// This allows using the same lookup curve for lights with different colors.
  /// 
  /// This color only affects the light for custom color animation curves.
  /// 
  /// \param iColor
  ///   Light color (uses RGB only)
  inline void SetAnimColor(VColorRef iColor) {SetAnimColor(iColor.r,iColor.g,iColor.b);}


  /// \brief
  ///   Set the current animation curve phase
  /// 
  /// If the animation time has been set to 0.0f, this function allows controlling the animation
  /// phase manually.
  /// 
  /// \param fCurvePos
  ///   The new phase [0..1]
  VISION_APIFUNC void SetAnimPhaseShift(float fCurvePos);


  /// \brief
  ///   Get the current animation curve phase 
  /// 
  /// This value is in the range [-1..1]
  /// 
  /// \return
  ///   float fCurrentPhase: Current phase value [-1..1]
  VISION_APIFUNC float GetCurrentAnimPhase() const;


  ///\brief
  /// When this flag is enabled then the light is deleted as soon as the animation is finished. Default is false
  ///
  ///\param bStatus
  /// New status 
  inline void SetRemoveLightWhenFinished(bool bStatus) {m_bRemoveLightWhenFinished=bStatus;}

  ///\brief
  /// Returns the flag that has been set via SetRemoveLightWhenFinished. Default is false
  inline bool GetRemoveLightWhenFinished() const {return m_bRemoveLightWhenFinished;}



  ///
  /// @}
  ///

private:
  friend class VisLightSource_cl;


  /// \brief
  ///   Constructor of the color animation class, can only be created by the light source
  VisLightSrc_AnimColor_cl(int lightSrcNr);


  /// \brief
  ///   This function will be called by the light source once per second in order to calculate the
  ///   necessary stuff for the animation
  void Handle();  


  // enumeration for accessing the three different color channels
  enum
  {
    RED = 0, GREEN = 1, BLUE = 2
  };


  // animation direction
#if defined INC ///< @@@ put these undefines somewhere else... check where it has been defined
#undef INC
#endif
#if defined DEC
#undef DEC
#endif
  enum Dir_e
  {
    INC = 1, DEC = 2
  };
  
  int lNr;                              ///< Light source number
  Type_e type;                          ///< Current animation type
  Type_e oldType;                       ///< Old "stored" animation type for paused animations
  Dir_e dir[3];                         ///< Animation direction
  BOOL enabled[3];                      ///< If TRUE then this certain color channelis animated
  int minC[3];                          ///< Minimum value for each color channel
  int maxC[3];                          ///< Maximum value for each color channel
  int curC[3];                          ///< Current value for each color channel
  int speed[3];                         ///< Current animation speed for each color channel
  int time[3];                          ///< Current time for each color channel

  // curve related
  VisBitmapPtr m_spLookup;              ///< Smart pointer to lookup texture
  float m_fCurveSpeed, m_fCurvePhase;   ///< Curve speed of the color animation
  float m_fMaxIntensity;                ///< Maximun intensity of the color animation curve
  UBYTE m_ModColor[3];                  ///< Modulate color that can be used to modify the color animation result
  bool m_bRemoveLightWhenFinished;      ///< True if it should remove light when animation is finished otherwise it will loop. Default is false
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
