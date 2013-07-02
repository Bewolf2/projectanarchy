/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

// ***********************************************************************************************
// Tutorial 05 : Serialization
// Copyright (C) Havok.com Inc. All rights reserved.
// ***********************************************************************************************
// How to create a custom entity class with serialization
// ***********************************************************************************************
#ifndef MY_ENTITY_HPP
#define MY_ENTITY_HPP

//A custom entity class that rotates the entity
class MyEntity_cl : public VisBaseEntity_cl
{
public:

  MyEntity_cl();
  ~MyEntity_cl();
  
  VOVERRIDE void InitFunction();
  VOVERRIDE void ThinkFunction();
  VOVERRIDE void Serialize( VArchive &ar );
  
  V_DECLARE_SERIAL_DLLEXP( MyEntity_cl, DECLSPEC_DLLEXPORT );  
  IMPLEMENT_OBJ_CLASS(MyEntity_cl)
  
  float RotateSpeed;  //rotation speed (degrees per second)
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
