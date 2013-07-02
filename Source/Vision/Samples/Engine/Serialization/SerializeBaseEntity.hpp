/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

//***********************************************************************
// SerializeBaseEntity_cl Class
//***********************************************************************

// you must first include the vision.hpp file to use this header
#ifndef _SerializeBaseEntity_cl_HEADER_
#define _SerializeBaseEntity_cl_HEADER_


#define MSG_SER_CLEANUP VIS_MSG_USER + 1


extern VModule g_serializeSampleModule;

#define SERIALIZEDEMO_DLLEXP _declspec( dllexport )

enum SerializeType_e
{
  SERIALIZE_FULL,       // full object serialization
  SERIALIZE_RECREATE    // just store entity params and re-create entity on load
};


//
// *** SerializeBaseEntity_cl  ***
//
// Synopsis:
//    - 
//
// Description:
//    - 
//
class SerializeBaseEntity_cl : public VisBaseEntity_cl
{
  typedef VisBaseEntity_cl Inherited;
  V_DECLARE_SERIAL( SerializeBaseEntity_cl, DECLSPEC_DLLEXPORT )
private:
  // private variables
public:
  // serialization support functions
  virtual SerializeType_e GetSerializeType() { return SERIALIZE_FULL; }
  DECLSPEC_DLLEXPORT virtual void Serialize( VArchive& ar );
  
IMPLEMENT_OBJ_CLASS(SerializeBaseEntity_cl);
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
