/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKAI_INTERVAL_PARTITION_LIBRARY_H
#define HKAI_INTERVAL_PARTITION_LIBRARY_H

#include <Ai/Internal/SegmentCasting/hkaiIntervalPartition.h>

/// A collection of immutable interval partitions, stored in a RAM-friendly manner.
/// Interval partitions cannot be removed or modified after adding.
class hkaiIntervalPartitionLibrary
{
public:
	//+version(0)
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI_SPATIAL, hkaiIntervalPartitionLibrary);
	HK_DECLARE_REFLECTION();

	hkaiIntervalPartitionLibrary() { }
	hkaiIntervalPartitionLibrary(hkFinishLoadedObjectFlag f)
		: m_data(f)
		, m_partitionRecords(f)
	{ }

	typedef hkUint32 Index;
	static const Index INVALID_PARTITION_INDEX = 0xFFFFFFFFu;

	Index addIntervalPartition(hkaiIntervalPartition const& intervalPartition, bool storeYData, bool storeIntData);

	hkBool32 isDefinedAt(Index idx, hkReal x) const;
	hkBool32 tryEvaluateYAt(Index idx, hkReal x, hkReal& y) const;
	hkBool32 tryGetIntDataAt(Index idx, hkReal x, hkUint32& intData) const;

	hkBool32 getClosestDefined(Index idx, hkReal & xInOut) const;

	hkUint16 getNumIntervals(Index idx) const;
	void getInterval(Index idx, hkUint16 intervalIdx, hkaiIntervalPartition::Interval & intervalOut) const;

private:
	hkArray<hkReal> m_data;

public:
	struct PartitionRecord
	{
		//+version(0)
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI_SPATIAL, PartitionRecord);
		HK_DECLARE_REFLECTION();
		HK_DECLARE_POD_TYPE();

		hkUint32 m_intervalDataOffset;
		hkUint16 m_numIntervals;
		hkBool m_hasYData;
		hkBool m_hasIntData;
	};
private:

	hkArray<PartitionRecord> m_partitionRecords;
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
