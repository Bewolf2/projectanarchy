/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKAI_TRAVERSAL_TYPE_H
#define HKAI_TRAVERSAL_TYPE_H

/*
HK_DECLARE_REFLECTION();
*/

enum hkaiTraversalType
{
	HKAI_TRAVERSAL_TYPE_JUMP,
	HKAI_TRAVERSAL_TYPE_CLIMB_UP,
	HKAI_TRAVERSAL_TYPE_VAULT,
	HKAI_TRAVERSAL_TYPE_DUCK_UNDER,
	HKAI_TRAVERSAL_TYPE_DROP_DOWN,

	LAST_BUILTIN = 127,
	// Add user-defined traversal types here

};

class hkClassEnum;
extern const hkClassEnum* hkaiTraversalTypeEnum;

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
