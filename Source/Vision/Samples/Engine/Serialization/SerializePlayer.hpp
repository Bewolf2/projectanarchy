/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

//***********************************************************************
// SerializePlayer_cl Class
//***********************************************************************

// you must first include the vision.hpp file to use this header
#ifndef _SerializePlayer_cl_HEADER_
#define _SerializePlayer_cl_HEADER_

class vHavokCharacterController;
typedef vHavokCharacterController CharacterController;
class VVirtualThumbStick;

extern VisSampleAppPtr spApp;

enum SERIALIZE_CONTROL {
  SERIALIZE_SLOT_1 = VISION_INPUT_CONTROL_LAST_ELEMENT+1,
  SERIALIZE_SLOT_2,
  SERIALIZE_SLOT_3,
  SERIALIZE_SLOT_4,
  SERIALIZE_SAVE_MODIFIER,
  SERIALIZE_NEXT_MAP,

  SERIALIZE_MOVE_FORWARD,
  SERIALIZE_MOVE_BACKWARD,
  SERIALIZE_MOVE_LEFT,
  SERIALIZE_MOVE_RIGHT,

  SERIALIZE_ACTION_OCCURED,

  SERIALIZE_LOOK_CHANGED,
  SERIALIZE_HORIZONTAL_LOOK,
  SERIALIZE_VERTICAL_LOOK,

  SERIALIZE_RUN,
  SERIALIZE_RUN_FASTER
};

//
// *** SerializePlayer_cl  ***
//
// Synopsis:
//    - A mouse-controlled player entity that supports serialization
//
// Description:
//    - This class represents a mouse-controlled player entity. Most of its
//      functionality is derived from its base class, VisMouseCamera_cl. The only
//      addition is the GetSerializeType function (from ISerializeable interface)
//      that is queried by
//
// Notes:
//    - 
//
class SerializePlayer_cl : public SerializeBaseEntity_cl
{
public:
  typedef SerializeBaseEntity_cl Inherited;

  IMPLEMENT_OBJ_CLASS(SerializePlayer_cl);
  V_DECLARE_SERIAL(SerializePlayer_cl, DECLSPEC_DLLEXPORT)

  // virtual functions inherited from VisBaseEntity_cl
  virtual void InitFunction() HKV_OVERRIDE;    // entity initialization
  virtual void DeInitFunction() HKV_OVERRIDE;  // entity deinitialization
  virtual void ThinkFunction() HKV_OVERRIDE;   // entity ThinkFunction, called once per frame
  
  DECLSPEC_DLLEXPORT virtual void Serialize(VArchive& ar) HKV_OVERRIDE; // entity serialization

protected:
  // private function
  void BaseInit();                                              // base initialization

  float m_fHalfScreenSizeX;
  float m_fHalfScreenSizeY;

  // private variables
  int m_SpeedMode;                                              // how fast the player is currently moving
  CharacterController *m_pPhys;                                 // our physics object

private:
#if defined(SUPPORTS_MULTITOUCH)
  VVirtualThumbStick* m_pVirtualThumbStick;
#endif

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
