/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

// WARNING: THIS FILE IS GENERATED. EDITS WILL BE LOST.
// Generated from 'Behavior/Behavior/Generator/Blender/hkbBlenderGenerator.h'

#ifndef HKB_BLENDER_GENERATOR_INTERNAL_STATE_H
#define HKB_BLENDER_GENERATOR_INTERNAL_STATE_H

#include <Behavior/Behavior/Generator/Blender/hkbBlenderGenerator.h>

class hkbBlenderGeneratorInternalState : public hkReferencedObject
{
	//+vtable(1)
	//+version(0)
	public:

		HK_DECLARE_REFLECTION();
		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );

		hkbBlenderGeneratorInternalState() {}
		hkbBlenderGeneratorInternalState( hkFinishLoadedObjectFlag flag )
			: hkReferencedObject(flag)
			, m_childrenInternalStates(flag)
			, m_sortedChildren(flag)
		{
		}

		hkArray<struct hkbBlenderGenerator::ChildInternalState> m_childrenInternalStates;
		hkArray<hkInt16> m_sortedChildren;
		hkReal m_endIntervalWeight;
		int m_numActiveChildren;
		hkInt16 m_beginIntervalIndex;
		hkInt16 m_endIntervalIndex;
		hkBool m_initSync;
		hkBool m_doSubtractiveBlend;
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
