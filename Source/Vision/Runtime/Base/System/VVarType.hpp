/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VVarType.hpp

#ifndef VVARTYPE_HPP
#define VVARTYPE_HPP

/// \brief
///   enum types for Vision variable types (e.g. entities and effect parameters)
enum VulpType_e
{
  VULPTYPE_UNKNOWN=-1,
  
  //Entity variable types
  VULPTYPE_INT=0,
  VULPTYPE_UINT,
  VULPTYPE_FLOAT,
  VULPTYPE_DOUBLE,
  VULPTYPE_ENUM,
  VULPTYPE_BOOL,
  VULPTYPE_VECTOR_INT,
  VULPTYPE_VECTOR_FLOAT,
  VULPTYPE_VECTOR_DOUBLE,
  VULPTYPE_STRING,
  VULPTYPE_PSTRING,
  VULPTYPE_MODEL,
  VULPTYPE_VSTRING,
  VULPTYPE_SHADEREFFECT,
  VULPTYPE_BITMASK,

  //0x40 - Keys : Remember to update IS_KEY_TYPE below if you add more keys
  VULPTYPE_ENTITY_KEY = 0x40,
  VULPTYPE_PRIMITIVE_KEY,
  VULPTYPE_EVENTAREA_KEY,
  VULPTYPE_VERTEX_KEY,
  VULPTYPE_LIGHTSOURCE_KEY,
  VULPTYPE_WORLDANIMATION_KEY,
  VULPTYPE_PATH_KEY,
  VULPTYPE_REFERENCED_OBJECT,

  //0x60 - Special internal variables for the editor

  //0x80 - Special internal variables for the engine (e.g. shaders)
  VULPTYPE_FLOAT_VECTOR4 = 0x80,
  VULPTYPE_FLOAT_VECTOR2,
  VULPTYPE_FLOAT_MATRIX16,
  VULPTYPE_FLOAT_COLOR4,
  VULPTYPE_BYTE_COLOR4,
  VULPTYPE_CUBEMAP_ENTITY_KEY,
  VULPTYPE_TEXTURE_FORMAT_STRING,
  VULPTYPE_TEXTURE_FILE,
  VULPTYPE_SWITCH,

  VULPTYPE_INCONSISTENT_TYPE = 0x200  ///< one parameter is used several times with different and incompatible vartypes
};

//TODO: Rename to VVarType_e ?

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
