/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

HK_FORCE_INLINE bool hkbGeneratorPartitionInfo::hasPartitions() const
{
	return m_numMaxPartitions > 0;
}

HK_FORCE_INLINE bool hkbGeneratorPartitionInfo::hasPartitionIndex(const hkInt16 partitionIndex) const
{
	const hkBitField partitionMaskBitfield(const_cast<hkUint32*>(m_partitionMask), MAX_NUM_PARTITIONS);
	return (!hasPartitions()) || (partitionMaskBitfield.get(partitionIndex) & 0x1);
}

HK_FORCE_INLINE bool hkbGeneratorPartitionInfo::hasBoneIndex(const hkInt16 boneIndex) const
{
	const hkBitField boneMaskBitfield(const_cast<hkUint32*>(m_boneMask), MAX_NUM_BONES);
	return (!hasPartitions()) || (boneMaskBitfield.get(boneIndex) & 0x1);
}

HK_FORCE_INLINE bool hkbGeneratorPartitionInfo::hasRootBone() const
{
	return hasBoneIndex(0);
}

HK_FORCE_INLINE hkInt16 hkbGeneratorPartitionInfo::getNumBones() const
{
	return m_numBones;
}

/// Returns the number of total possible partitions
HK_FORCE_INLINE hkInt16 hkbGeneratorPartitionInfo::getMaxNumPartitions() const
{
	return m_numMaxPartitions;
}

HK_FORCE_INLINE void hkbGeneratorPartitionInfo::setNumBones(const hkaSkeleton& skeleton, hkInt16 numBones)
{
	m_numBones = numBones;

	// After we set a new amount of bones, make sure the masks reflect that change
	truncateMasksToLOD( skeleton );
}

HK_FORCE_INLINE const hkUint32* hkbGeneratorPartitionInfo::getUsedPartitionMask() const
{
	return m_partitionMask;
}

HK_FORCE_INLINE const hkUint32* hkbGeneratorPartitionInfo::getUsedBoneMask() const
{
	return m_boneMask;
}

HK_FORCE_INLINE hkInt16 hkbGeneratorPartitionInfo::getTrackIndexFromBoneIndex(hkInt16 boneIndex) const
{
	if(boneIndex == 0)
	{
		return 0;
	}

	return !hasPartitions() ? boneIndex : (hkInt16)hkBitField::countOnes(m_boneMask, boneIndex);
}

HK_FORCE_INLINE bool hkbGeneratorPartitionInfo::isAllPartitions() const
{
	return !hasPartitions() || ((hkInt16)hkBitField::countOnes(m_partitionMask, MAX_NUM_PARTITIONS) == m_numMaxPartitions);
}

HK_FORCE_INLINE bool hkbGeneratorPartitionInfo::hasSameNumBones(const hkbGeneratorPartitionInfo& info, hkInt16 numBonesInOutput ) const
{
	return (isAllPartitions() && info.isAllPartitions()) || m_numBones == numBonesInOutput;
}

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
