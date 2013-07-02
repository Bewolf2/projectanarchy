/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VLink.hpp

#ifndef DC_VISION_VLINK_HPP
#define DC_VISION_VLINK_HPP

#ifndef _VISION_DOC

//This structure is used to create a linklist with variable sized links

struct VLink 
{
  VLink* pNext;
  // we will allocate sizeof(VLink) + (count * size) bytes of memory for an instance
  // data() is the start address of the extra memory
  inline void* data() { return this+1; } 
  
  VBASE_IMPEXP static VLink* Create(VLink *&head, int count, int size);
  VBASE_IMPEXP void FreeChain();
};


typedef void* VPOSITION;
#define VBEFORE_START_POSITION ((VPOSITION)-1L)

#endif // _VISION_DOC

#endif // DC_VISION_VLINK_HPP

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
