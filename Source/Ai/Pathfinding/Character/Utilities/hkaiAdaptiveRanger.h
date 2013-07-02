/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HK_AI_ADAPTIVE_RANGER_H
#define HK_AI_ADAPTIVE_RANGER_H

/// Utility to dynamically adjust sensor range to keep the number of avoided neighbors in a reasonable range.
///
/// The range is always in the range (0,1] and is intended to be used as a multiplicative factor with a
/// "maximum range". The range expands and contracts multiplicatively, with a step size given in the source
/// file, when the observed target count differs from the ideal target count by more than 25% of the ideal
/// target count. So, for an ideal target count of 12, the adaptive ranger will expand its range when 
/// fewer than 9 targets are encountered, and will contract if more than 15 targets are encountered.
class hkaiAdaptiveRanger
{
public:
	HK_DECLARE_REFLECTION();
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI_STEERING, hkaiAdaptiveRanger);

	hkaiAdaptiveRanger() : m_curRange(1.0f) { }

	hkaiAdaptiveRanger(hkFinishLoadedObjectFlag f) { }

	hkReal getCurRange() const { return m_curRange; }
	void updateRange(hkUint32 idealTargetCount, hkUint32 actualTargetCount);

private:
	hkReal m_curRange; // +default(1.0f)
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
