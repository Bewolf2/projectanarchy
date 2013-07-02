/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

HK_FORCE_INLINE void hkbGeneratorOutputUtils::replicateTrackStructure( const hkbGeneratorOutput& from, hkbGeneratorOutput& to )
{
	const int numTracks = from.getNumTracks();

	// copy the headers
	const int numBytesToCopy = sizeof(hkbGeneratorOutput::TrackMasterHeader) + numTracks * sizeof(hkbGeneratorOutput::TrackHeader);

	HK_CHECK_ALIGN16( numBytesToCopy );
	HK_ASSERT( 0x59a91289, numBytesToCopy >= 16 );

	// for safety, you need to set the number of bytes beforehand in "to"
	HK_ASSERT2( 0x61627ab3, numBytesToCopy <= to.getTracks()->m_masterHeader.m_numBytes, "track count mismatch (did you set numBytes in 'to'?)" );

	// save and restore the number of bytes in "to" since we are copying over it
	hkInt32 toNumBytes = to.getTracks()->m_masterHeader.m_numBytes;
	hkString::memCpy16NonEmpty( to.accessTracks(), from.getTracks(), numBytesToCopy >> 4 );
	to.accessTracks()->m_masterHeader.m_numBytes = toNumBytes;

	
	// turn off all of the tracks
	for( int i = 0; i < numTracks; i++ )
	{
		to.accessTrackHeader(i).m_onFraction = 0.0f;
	}
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
