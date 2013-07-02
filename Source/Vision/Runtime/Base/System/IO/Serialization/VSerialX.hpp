/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VSerialX.hpp
/// \brief
///   Definition file for the serialization macros used for serialization
//    of the non VBase derived objects.

#ifndef _VISION_VSERIALX_H_
#define _VISION_VSERIALX_H_

class VArchive;

#define V_DECLARE_SERIALX( class_name, imp_exp_spec )                        \
  friend imp_exp_spec VArchive& operator <<( VArchive &ar, class_name& v );  \
  friend imp_exp_spec VArchive& operator <<( VArchive &ar, class_name* v );  \
  friend imp_exp_spec VArchive& operator >>( VArchive &ar, class_name& v );  \
  friend imp_exp_spec VArchive& operator >>( VArchive &ar, class_name* v );  \

//#define V_SERIALIZE_DEBUG

#ifdef V_SERIALIZE_DEBUG
#define V_SERIALIZE_DEBUG_OUT(x) OutputDebugStringA(x);
#else
#define V_SERIALIZE_DEBUG_OUT(x)
#endif
  
#define V_IMPLEMENT_SERIALX( class_name )                \
  VArchive& operator <<( VArchive &ar, class_name& obj ) \
  {                                                      \
    V_SERIALIZE_DEBUG_OUT(V_CAT(V_CAT("start serializing ",V_STRINGIZE(class_name)),"\n"))\
    obj.SerializeX( ar );                                \
    V_SERIALIZE_DEBUG_OUT(V_CAT(V_CAT("done serializing ",V_STRINGIZE(class_name)),"\n"))            \
    return ar;                                           \
  }                                                      \
  VArchive& operator >>( VArchive &ar, class_name& obj ) \
  {                                                      \
    V_SERIALIZE_DEBUG_OUT(V_CAT(V_CAT("start serializing ",V_STRINGIZE(class_name)),"\n"))           \
    obj.SerializeX( ar );                                \
    V_SERIALIZE_DEBUG_OUT(V_CAT(V_CAT("done serializing ",V_STRINGIZE(class_name)),"\n"))            \
    return ar;                                           \
  }                                                      \
  VArchive& operator <<( VArchive &ar, class_name* obj ) \
  {                                                      \
    V_SERIALIZE_DEBUG_OUT(V_CAT(V_CAT("start serializing ",V_STRINGIZE(class_name)),"\n"))           \
    obj->SerializeX( ar );                               \
    V_SERIALIZE_DEBUG_OUT(V_CAT(V_CAT("done serializing ",V_STRINGIZE(class_name)),"\n"))            \
    return ar;                                           \
  }                                                      \
  VArchive& operator >>( VArchive &ar, class_name* obj ) \
  {                                                      \
    V_SERIALIZE_DEBUG_OUT(V_CAT(V_CAT("start serializing ",V_STRINGIZE(class_name)),"\n"))           \
    obj->SerializeX( ar );                               \
    V_SERIALIZE_DEBUG_OUT(V_CAT(V_CAT("done serializing ",V_STRINGIZE(class_name)),"\n"))            \
    return ar;                                           \
  }

#define V_DECLARE_SERIALX_NONINTRUSIVE(class_name, imp_exp_spec)\
\
  imp_exp_spec void SerializeX (VArchive &ar, class_name& obj); \
  \
  inline VArchive& operator<< (VArchive &ar, class_name& obj) \
  {                                                      \
    V_SERIALIZE_DEBUG_OUT(V_CAT(V_CAT("start serializing ",V_STRINGIZE(class_name)),"\n"))          \
    SerializeX (ar, obj);                                \
    V_SERIALIZE_DEBUG_OUT(V_CAT(V_CAT("done serializing ",V_STRINGIZE(class_name)),"\n"))            \
    return ar;                                           \
  }                                                      \
  inline VArchive& operator>> (VArchive &ar, class_name& obj) \
  {                                                      \
    V_SERIALIZE_DEBUG_OUT(V_CAT(V_CAT("start serializing ",V_STRINGIZE(class_name)),"\n"))          \
    SerializeX (ar, obj);                                \
    V_SERIALIZE_DEBUG_OUT(V_CAT(V_CAT("done serializing ",V_STRINGIZE(class_name)),"\n"))            \
    return ar;                                           \
  }                                                      \
  inline VArchive& operator<< (VArchive &ar, class_name* obj) \
  {                                                      \
    V_SERIALIZE_DEBUG_OUT(V_CAT(V_CAT("start serializing ",V_STRINGIZE(class_name)),"\n"))           \
    SerializeX (ar, *obj);                               \
    V_SERIALIZE_DEBUG_OUT(V_CAT(V_CAT("done serializing ",V_STRINGIZE(class_name)),"\n"))            \
    return ar;                                           \
  }                                                      \
  inline VArchive& operator>> (VArchive &ar, class_name* obj) \
  {                                                      \
    V_SERIALIZE_DEBUG_OUT(V_CAT(V_CAT("start serializing ",V_STRINGIZE(class_name)),"\n"))           \
    SerializeX (ar, *obj);                               \
    V_SERIALIZE_DEBUG_OUT(V_CAT(V_CAT("done serializing ",V_STRINGIZE(class_name)),"\n"))            \
    return ar;                                           \
  }                                                      \

#endif // _VISION_VSERIALX_H_

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
