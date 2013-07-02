/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file StochasticalParam.hpp

#ifndef STOCHASTICALPARAM_HPP_INCLUDED
#define STOCHASTICALPARAM_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Particles/ParticleModule.hpp>
#include <Vision/Runtime/Base/Math/Random/VRandom.hpp>

// forward declarations
class TiXmlElement;


///\brief
/// A single parameter that allows stochastical variation. Basically has an average and a variation value
///
/// The variation parameter defines the interval random values around the average value.
/// This class is extensively used by the particle system.
class VisStochasticalParam_cl
{
public: 
  V_DECLARE_SERIALX( VisStochasticalParam_cl, PARTICLE_IMPEXP )

  ///\brief
	///Constructor that takes average and variation
	///
	///\param fAverage
	///The average value of the parameter
	///
	///\param fVariation
  ///The variation (0.0 = always average, 1.0 = full 0.0 to 2*average range)
	///
	inline VisStochasticalParam_cl(float fAverage=0.f, float fVariation=0.f) {m_fAverage=fAverage;m_fVariation=fVariation;};

  ///\brief
  ///Set average and variation (see constructor)
  inline void Set(float fAverage, float fVariation) {VASSERT(fVariation>=0.f && fVariation<=1.f);m_fAverage=fAverage;m_fVariation=fVariation;}

  ///\brief
  ///Checks whether average is >0
  inline bool IsDefined() const {return m_fAverage>0.f;}

  ///\brief
  ///Returns a new random value (1.0+randneg*variation)*average
  inline float GetRandomValue(const VRandom& randGen) const 
  {
    float fMul = 1.f + m_fVariation*randGen.GetFloatNeg();
    return fMul * m_fAverage;
  }

  ///\brief
  ///  Returns the minimal possible value
  inline float GetMinValue() const
  {
    return m_fAverage * (1.0f - m_fVariation);
  }

  ///\brief
  ///  Returns the maximal possible value
  inline float GetMaxValue() const
  {
    return m_fAverage * (1.0f + m_fVariation);
  }

  ///\brief
  ///XML data serialization helper
  bool DataExchangeXML(const char *szName, TiXmlElement *pParent, bool bWrite);

  ///\brief
  ///Assignent operator
  VisStochasticalParam_cl &operator = (const VisStochasticalParam_cl &other)
  {
    m_fAverage = other.m_fAverage;
    m_fVariation = other.m_fVariation;
    return *this;
  }

  ///\brief
  ///Binary file serialization helper
  void SerializeX( VArchive &ar );
public: 
  float m_fAverage;  ///< average value
  float m_fVariation;///< value variation (0.0 = always average, 1.0 = full 0.0 to 2*average range)
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
