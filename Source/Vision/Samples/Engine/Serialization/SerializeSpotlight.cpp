/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

//***********************************************************************
// SerializeSpotlight_cl Source
//***********************************************************************

#include <Vision/Samples/Engine/Serialization/SerializationPCH.h>
#include <Vision/Samples/Engine/Serialization/SerializeSpotlight.hpp>


V_IMPLEMENT_SERIAL( SerializeSpotlight_cl, SerializeBaseEntity_cl, 0, &g_serializeSampleModule );


// ---------------------------------------------------------------------------------
// Method: InitFunction
// Author: Trinigy
// Notes: 
// ---------------------------------------------------------------------------------
void SerializeSpotlight_cl::InitFunction()
{
  Inherited::InitFunction();

  m_pLight = new VisLightSource_cl(VIS_LIGHT_SPOTLIGHT, 2000);
  m_pLight->AttachToParent(this);
  m_pLight->SetLocalPosition(hkvVec3(0,0,0));
  m_pLight->SetKey("disco_spotlight");
  m_pLight->SetMultiplier(3.f);

  // carry out shared part of the initialization
  BaseInit();

  // set up spot light parameters
  SetupSpotlight();
}


// ---------------------------------------------------------------------------------
// Method: DeInitFunction
// Author: Trinigy
// Notes: 
// ---------------------------------------------------------------------------------
void SerializeSpotlight_cl::DeInitFunction()
{
  // destroy light source
  V_SAFE_DISPOSEOBJECT(m_pLight);

  Inherited::DeInitFunction();
}


// ---------------------------------------------------------------------------------
// Method: BaseInit
// Author: Trinigy
// Notes: Initialization code that is shared between InitFunction and Serialize
// ---------------------------------------------------------------------------------
void SerializeSpotlight_cl::BaseInit()
{
}


// ---------------------------------------------------------------------------------
// Method: SetupSpotlight
// Author: Trinigy
// Notes: Set up spot light parameters. This function is only called when a new
//        entity is created, but not on serialization.
// ---------------------------------------------------------------------------------
void SerializeSpotlight_cl::SetupSpotlight()
{
  // set a random light source colour
  VColorRef iColor;
  iColor.SetRandomColor();
  m_pLight->SetColor( iColor );

  // activate an intensity animation that pulsates between 5000 and 6000 units
  // and that has a randomized period time of 1-3 seconds
  float randf = Vision::Game.GetFloatRand();
  m_pLight->IntensityAnimation().Pulsate( 5000, 6000, ( randf * 2 ) + 1 );

  // set the spotlight's cone angle to 45-60 degrees
  UBYTE ang = ( Vision::Game.GetRand() % 15 ) + 45;
  m_pLight->SetProjectionAngle(ang);
}


// ---------------------------------------------------------------------------------
// Method: ThinkFunction
// Author: Trinigy
// Notes: 
// ---------------------------------------------------------------------------------
void SerializeSpotlight_cl::ThinkFunction()
{
  float fDiff = Vision::GetTimer()->GetTimeDifference();
  // rotate spot light
  IncOrientation(SPOTLIGHT_ROT_SPEED * fDiff, 0, 0);
}


// ---------------------------------------------------------------------------------
// Method: Serialize
// Author: Trinigy
// Notes: In addition to the base class' serialization, we only need to serialize
//        our light source. We can safely serialize a pointer to a serializable
//        class, the code in VArchive takes care of serializing the object itself
//        and restoring the pointer hierarchy on loading.
// ---------------------------------------------------------------------------------
void SerializeSpotlight_cl::Serialize( VArchive& ar )
{
  // call base class Serialize
  Inherited::Serialize( ar );

  // serialize additional data
  if ( ar.IsLoading() )
  {
    ar >> m_pLight;

    BaseInit();
  }
  else
  {
    ar << m_pLight;
  }
}


START_VAR_TABLE(SerializeSpotlight_cl, SerializeBaseEntity_cl, "SerializeSpotlight_cl", 0, "")
END_VAR_TABLE

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
