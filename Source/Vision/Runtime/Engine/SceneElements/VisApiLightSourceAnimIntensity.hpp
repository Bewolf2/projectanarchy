/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiLightSourceAnimIntensity.hpp

#ifndef DEFINE_VISAPILIGHTSOURCEANIMINTENSITY
#define DEFINE_VISAPILIGHTSOURCEANIMINTENSITY


#include <Vision/Runtime/Engine/SceneElements/VisApiLightSource.hpp>

/// \brief
///   Intensity animation class for light sources
/// 
/// This class provides convenience functionality for intensity (i.e. brightness) animations for
/// light sources. 
/// 
/// Intensity animation instances can be accessed through the VisLightSource_cl class using the
/// VisLightSource_cl::IntensityAnimation method.
class VisLightSrc_AnimIntensity_cl
{
  V_DECLARE_SERIALX( VisLightSrc_AnimIntensity_cl, VISION_APIFUNC )
  friend class SerializationTest;
public:
  

  /// \brief
  ///   Start a flicker intensity animation
  /// 
  /// This function starts an animation which causes the light source to flicker between two
  /// intensity values.
  /// 
  /// \param minInt
  ///   Minimum intensity of the animation between 0 and 16384 (phase #1)
  /// 
  /// \param maxInt
  ///   Maximum intensity of the animation between 0 and 16384 (phase #2)
  /// 
  /// \param singleLoopTime
  ///   Duration of one flicker phase in seconds.
  VISION_APIFUNC void Flicker(int minInt, int maxInt, float singleLoopTime);
  

  /// \brief
  ///   Start a pulsate intensity animation
  /// 
  /// This function starts an animation which causes the light source to pulsate between two
  /// intensity values.
  /// 
  /// \param startInt
  ///   Intensity start value between 0 and 16384.
  /// 
  /// \param endInt
  ///   Intensity end value between 0 and 16384.
  /// 
  /// \param singleLoopTime
  ///   Duration of one phase in seconds.
  /// 
  /// \param animDuration
  ///   Time after which animation will be stopped automatically (if set to -1 then the animation
  ///   will run forever).
  VISION_APIFUNC void Pulsate(int startInt, int endInt, float singleLoopTime, float animDuration = - 1);
  

  /// \brief
  ///   Start a candle intensity animation
  /// 
  /// This function starts an animation which causes the light source to flicker irregularely
  /// between two intensity values, similar to a candle.
  /// 
  /// \param minInt
  ///   Minimum intensity of the animation between 0 and 16384.
  /// 
  /// \param maxInt
  ///   Maximum intensity of the animation between 0 and 16384.
  /// 
  /// \param singleLoopTime
  ///   Duration of one candle phase in seconds.
  VISION_APIFUNC void Candle(int minInt, int maxInt, float singleLoopTime);
  

  /// \brief
  ///   Stop the intensity animation
  /// 
  /// This function stops the intensity animation, the intenstiy keeps the last value.
  VISION_APIFUNC void Stop();
  

  /// \brief
  ///   Pause the intensity animation
  /// 
  /// This function pauses the intensity animation, this means that the animation be 
  /// continued later on with the Continue function.
  /// 
  /// \sa Continue
  VISION_APIFUNC void Pause();
  

  /// \brief
  ///   Continue the paused intensity animation
  /// 
  /// This function continues an animation which has been halted before with the Pause function.
  /// 
  /// \sa Pause
  VISION_APIFUNC void Continue();


  /// \brief
  ///   Get type of light source
  ///
  /// \li NONE :     No intensity animation
  /// 
  /// \li CANDLE :   Candle animation
  ///
  /// \li FLICKER :  Flicker animation
  ///
  /// \li PULSATE :  Pulsate animation
  /// 
  /// \return
  ///   int: type of light source, see Type_e
  VISION_APIFUNC int GetType() const;


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

private:
  friend class VisLightSource_cl;
  

  /// \brief
  ///   Constructor of the intensity animation class, can only be created by the light source
  VisLightSrc_AnimIntensity_cl(int lightSrcNr);

  
  /// \brief
  ///   This function will be called by the light source once per second in order to calculate the
  ///   necessary data for the animation
  void Handle();


  /// \brief
  ///   The intensity animation type
  enum Type_e
  {
    NONE = 0, 
    CANDLE = 1, 
    FLICKER = 2, 
    PULSATE = 3
  };

  // Animation direction
  enum Dir_e
  {
    INC = 1, DEC = 2
  };

  int lNr;              ///< Light source number
  Type_e type;          ///< Current animation type
  Type_e oldType;       ///< Old "stored" animation type for paused animations
  Dir_e dir;            ///< Animation direction
  int minI;             ///< Minimum intensity in the animation
  int maxI;             ///< Maximum intensity in the animation
  int curI;             ///< Current intensity in the animation
  int diffI;            ///< Difference between maximum intensity and minimum intensity
  int speed;            ///< Animation speed of the animation
  int time;             ///< Current time in the animation
  float duration;       ///< Duration of the animation
  
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
