/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

//***********************************************************************
// SerializeSpotlight_cl Class
//***********************************************************************

// you must first include the vision.hpp file to use this header
#ifndef _SerializeSpotlight_cl_HEADER_
#define _SerializeSpotlight_cl_HEADER_

// include base class declaration
#include <Vision/Samples/Engine/Serialization/SerializeBaseEntity.hpp>


////////////////////////////////////////////////////////////////////////////////////
//
// defines
//
// spot light rotation speed (in degrees per second)
#define SPOTLIGHT_ROT_SPEED 20.0f


//
// *** SerializeSpotlight_cl  ***
//
// Synopsis:
//    - A serializable spot light entity class
//
// Description:
//    - This class represents the rotating and animated spot light ("disco light")
//      entity.
//    - The serialization code in this class is quite simple, as only the light
//      source is serialized. The VisLightSource_cl serialization code takes care
//      of loading and storing all importatnt light source data (including animation
//      and position/orientation).
//
class SerializeSpotlight_cl : public SerializeBaseEntity_cl
{
  V_DECLARE_SERIAL(SerializeSpotlight_cl, DECLSPEC_DLLEXPORT)
  typedef SerializeBaseEntity_cl Inherited;
private:
  // private variables
  VisLightSource_cl *m_pLight;            // our spot light (serialized)
  hkvMat3 m_matRot;               // rotation matrix
  void BaseInit();
  void SetupSpotlight();
public:
  // virtual functions inherited from SerializeBaseEntity_cl
  virtual void InitFunction();            // entity initialization
  virtual void DeInitFunction();          // entity de-initialization
  virtual void ThinkFunction();           // entity ThinkFunction, called once per frame
  virtual void Serialize( VArchive& ar ); // entity serialization

IMPLEMENT_OBJ_CLASS(SerializeSpotlight_cl);
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
