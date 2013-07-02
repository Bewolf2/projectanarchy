/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VFileData.hpp

#ifndef _VISION_VFILEDATA_HPP
#define _VISION_VFILEDATA_HPP

#include <Vision/Runtime/Base/System/IO/System/VFileTime.hpp>


/// \brief
///   class which hold information about one file
class VBASE_IMPEXP_CLASS VFileData
{
public:
  /// \brief
  ///   Combinable flags for the file attributes. The values are the same as the standard C attrib
  ///   values, although not all C attrib values are supported.
  enum Attributes
  {
    normal   = 0x0000,
    readOnly = 0x0001,
    hidden   = 0x0002,
    system   = 0x0004,
    subDir   = 0x0010,
    archive  = 0x0020
  };
  
  virtual ~VFileData() {}

  int size;   ///< size of the file in bytes
  char name[FS_MAX_FILE];   ///< filename without any path
  unsigned int attrib;      ///< attributes of the file (see Attributes enum)
  VFileTime time;   ///< date and time of last write access
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
