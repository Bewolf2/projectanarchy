/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

HK_FORCE_INLINE hkbGeneratorOutput::ConstTrack::ConstTrack( TrackHeader* header, hkReal* data ) 
:	m_header(header), 
	m_data(data) 
{
}

HK_FORCE_INLINE const hkbGeneratorOutput::TrackHeader* hkbGeneratorOutput::ConstTrack::getTrackHeader() const
{
	return m_header;
}

HK_FORCE_INLINE const hkReal* hkbGeneratorOutput::ConstTrack::getDataReal() const 
{ 
	return m_data; 
}

HK_FORCE_INLINE const hkQsTransform* hkbGeneratorOutput::ConstTrack::getDataQsTransform() const 
{ 
	return reinterpret_cast<hkQsTransform*>( m_data ); 
}

HK_FORCE_INLINE const hkInt8* hkbGeneratorOutput::ConstTrack::getIndices() const
{
	HK_ASSERT( 0xd7910a01, m_header->m_flags.anyIsSet( TRACK_FLAG_SPARSE | TRACK_FLAG_PALETTE ) );
		
	int numDataBytes = HK_NEXT_MULTIPLE_OF( 16, m_header->m_elementSizeBytes * m_header->m_capacity );

	return reinterpret_cast<const hkInt8*>( m_data ) + numDataBytes;
}

HK_FORCE_INLINE bool hkbGeneratorOutput::ConstTrack::isValid() const 
{ 
	return ( m_header->m_onFraction > 0.0f ); 
}

HK_FORCE_INLINE int hkbGeneratorOutput::ConstTrack::getNumData() const 
{ 
	return m_header->m_numData; 
}

HK_FORCE_INLINE int hkbGeneratorOutput::ConstTrack::getCapacity() const
{
	return m_header->m_capacity;
}

HK_FORCE_INLINE int hkbGeneratorOutput::ConstTrack::getElementSizeBytes() const
{
	return m_header->m_elementSizeBytes;
}

HK_FORCE_INLINE hkbGeneratorOutput::TrackTypes hkbGeneratorOutput::ConstTrack::getType() const 
{ 
	return m_header->m_type; 
}

HK_FORCE_INLINE hkReal hkbGeneratorOutput::ConstTrack::getOnFraction() const 
{ 
	return m_header->m_onFraction; 
}

HK_FORCE_INLINE bool hkbGeneratorOutput::ConstTrack::isPalette() const
{
	return m_header->m_flags.anyIsSet( TRACK_FLAG_PALETTE );
}

HK_FORCE_INLINE bool hkbGeneratorOutput::ConstTrack::isSparse() const
{
	return m_header->m_flags.anyIsSet( TRACK_FLAG_SPARSE );
}

HK_FORCE_INLINE hkbGeneratorOutput::Track::Track( TrackHeader* header, hkReal* data ) 
: ConstTrack( header, data )
{
}

HK_FORCE_INLINE hkReal* hkbGeneratorOutput::Track::accessDataReal() 
{ 
	return m_data; 
}

HK_FORCE_INLINE hkQsTransform* hkbGeneratorOutput::Track::accessDataQsTransform() 
{ 
	return reinterpret_cast<hkQsTransform*>( m_data ); 
}

HK_FORCE_INLINE hkInt8* hkbGeneratorOutput::Track::accessIndices() const
{
	HK_ASSERT( 0xaf3b81bb, m_header->m_flags.anyIsSet( TRACK_FLAG_SPARSE | TRACK_FLAG_PALETTE ) );

	int numDataBytes = HK_NEXT_MULTIPLE_OF( 16, m_header->m_elementSizeBytes * m_header->m_capacity );

	return reinterpret_cast<hkInt8*>( m_data ) + numDataBytes;
}

HK_FORCE_INLINE void hkbGeneratorOutput::Track::setDataReal( const hkReal* data, int count )
{
	HK_ASSERT2( 0x46912bc4, m_header->m_numData >= count, "track buffer is not big enough" );

	hkString::memCpy4( m_data, data, count );

	m_header->m_onFraction = 1.0f;
}

HK_FORCE_INLINE void hkbGeneratorOutput::Track::setDataQsTransform( const hkQsTransform* data, int count )
{
	HK_ASSERT2( 0x5601b6f4, m_header->m_numData >= count, "track buffer is not big enough" );

	hkString::memCpy16( m_data, data, count * (sizeof(hkQsTransform)/16) );

	m_header->m_onFraction = 1.0f;
}

HK_FORCE_INLINE void hkbGeneratorOutput::Track::setDataReal( int index, const hkReal& data )
{
	HK_ASSERT2( 0x1d8a0b50, m_header->m_numData > index, "track buffer is not big enough" );

	accessDataReal()[index] = data;
}

HK_FORCE_INLINE void hkbGeneratorOutput::Track::setDataQsTransform( int index, const hkQsTransform& data )
{
	HK_ASSERT2( 0x5b3298ab, m_header->m_numData > index, "track buffer is not big enough" );

	accessDataQsTransform()[index] = data;
}

HK_FORCE_INLINE void hkbGeneratorOutput::Track::setValid() 
{ 
	m_header->m_onFraction = 1.0f; 
}

HK_FORCE_INLINE void hkbGeneratorOutput::Track::setOnFraction( hkReal onFraction ) 
{
	m_header->m_onFraction = onFraction; 
}

HK_FORCE_INLINE void hkbGeneratorOutput::Track::setNumData( hkInt16 numData )
{
	m_header->m_numData = numData;
}

HK_FORCE_INLINE hkbGeneratorOutput::hkbGeneratorOutput( Tracks* tracks )
:	m_tracks(tracks),
	m_deleteTracks(false)
{
}

HK_FORCE_INLINE hkbGeneratorOutput::~hkbGeneratorOutput()
{
#if !defined(HK_PLATFORM_SPU)
	if ( m_deleteTracks )
	{
		hkDeallocate<hkVector4>( reinterpret_cast<hkVector4*>(m_tracks) );
	}
#endif
}

HK_FORCE_INLINE void hkbGeneratorOutput::clearTracks()
{
	for( int i = 0; i < m_tracks->m_masterHeader.m_numTracks; i++ )
	{
		m_tracks->m_trackHeaders[i].m_onFraction = 0.0f;
		m_tracks->m_trackHeaders[i].m_flags.andWith( ~TRACK_FLAG_ADDITIVE_POSE );
		
		// clear the sparse and palette tracks
		if ( m_tracks->m_trackHeaders[i].m_flags.anyIsSet( hkbGeneratorOutput::TRACK_FLAG_PALETTE | hkbGeneratorOutput::TRACK_FLAG_SPARSE ) )
		{
			m_tracks->m_trackHeaders[i].m_numData = 0;
		}
	}
}

HK_FORCE_INLINE bool hkbGeneratorOutput::trackExists( int trackId ) const 
{
	return m_tracks->m_masterHeader.m_numTracks > trackId;
}

HK_FORCE_INLINE void hkbGeneratorOutput::setValid( int trackId ) 
{
	HK_ASSERT( 0x61734a0f, trackExists( trackId ) );

	m_tracks->m_trackHeaders[trackId].m_onFraction = 1.0f; 
}

HK_FORCE_INLINE bool hkbGeneratorOutput::isValid( int trackId ) const 
{
	if( trackExists( trackId ) )
	{
		return ( m_tracks->m_trackHeaders[trackId].m_onFraction > 0.0f ); 
	}

	return false;
}

HK_FORCE_INLINE hkbGeneratorOutput::ConstTrack hkbGeneratorOutput::getTrack( int trackId ) const
{
	HK_ASSERT( 0x1563bd55, trackExists( trackId ) );

	TrackHeader* header = &(m_tracks->m_trackHeaders[trackId]);

	hkReal* data = reinterpret_cast<hkReal*>( reinterpret_cast<char*>( m_tracks ) + m_tracks->m_trackHeaders[trackId].m_dataOffset );

	return ConstTrack( header, data );
}

HK_FORCE_INLINE hkbGeneratorOutput::Track hkbGeneratorOutput::accessTrack( int trackId )
{
	HK_ASSERT( 0x03b6a77d, trackExists( trackId ) );

	TrackHeader* header = &(m_tracks->m_trackHeaders[trackId]);

	hkReal* data = reinterpret_cast<hkReal*>( reinterpret_cast<char*>( m_tracks ) + m_tracks->m_trackHeaders[trackId].m_dataOffset );

	return Track( header, data );
}

HK_FORCE_INLINE hkbGeneratorOutput::TrackHeader& hkbGeneratorOutput::accessTrackHeader( int trackId ) 
{ 
	return m_tracks->m_trackHeaders[trackId]; 
}

HK_FORCE_INLINE hkbGeneratorOutput::Tracks* hkbGeneratorOutput::accessTracks() 
{ 
	return m_tracks; 
}

HK_FORCE_INLINE const hkbGeneratorOutput::Tracks* hkbGeneratorOutput::getTracks() const 
{ 
	return m_tracks; 
}

HK_FORCE_INLINE const hkReal* hkbGeneratorOutput::getTrackDataReal( int trackId ) const
{
	HK_ASSERT2( 0x459ab350, trackExists( trackId ), "track not found" );
	HK_ASSERT2( 0x5123be6b, m_tracks->m_trackHeaders[trackId].m_type == TRACK_TYPE_REAL, "wrong track type" );

	const hkReal* p = reinterpret_cast<const hkReal*>( reinterpret_cast<char*>( m_tracks ) + m_tracks->m_trackHeaders[trackId].m_dataOffset );

	HK_CHECK_ALIGN16( p );
	return p;
}

HK_FORCE_INLINE const hkQsTransform* hkbGeneratorOutput::getTrackDataQsTransform( int trackId ) const
{
	HK_ASSERT2( 0x459ab350, trackExists( trackId ), "track not found" );
	HK_ASSERT2( 0x9123b46a, m_tracks->m_trackHeaders[trackId].m_type == TRACK_TYPE_QSTRANSFORM, "wrong track type" );

	const hkQsTransform* p = reinterpret_cast<const hkQsTransform*>( reinterpret_cast<char*>( m_tracks ) + m_tracks->m_trackHeaders[trackId].m_dataOffset );

	HK_CHECK_ALIGN16( p );
	return p;
}

HK_FORCE_INLINE const hkQsTransform& hkbGeneratorOutput::getWorldFromModel() const
{
	return *( getTrackDataQsTransform( TRACK_WORLD_FROM_MODEL ) );
}

HK_FORCE_INLINE const hkQsTransform& hkbGeneratorOutput::getExtractedMotion() const
{
	return *( getTrackDataQsTransform( TRACK_EXTRACTED_MOTION ) );
}

HK_FORCE_INLINE const hkQsTransform* hkbGeneratorOutput::getPoseLocal() const
{
	return getTrackDataQsTransform( TRACK_POSE );
}

HK_FORCE_INLINE const hkReal* hkbGeneratorOutput::getBoneWeights() const
{
	const hkbGeneratorOutput::ConstTrack trackInfo = getTrack(TRACK_POSE);
	return reinterpret_cast<const hkReal*>( trackInfo.getDataQsTransform() + trackInfo.getNumData() );
}

HK_FORCE_INLINE const hkReal* hkbGeneratorOutput::getFloats() const
{
	return getTrackDataReal( TRACK_FLOAT_SLOTS );
}

HK_FORCE_INLINE const hkReal* hkbGeneratorOutput::getAttributes() const
{
	return getTrackDataReal( TRACK_ATTRIBUTES );
}

HK_FORCE_INLINE int hkbGeneratorOutput::getNumTracks() const
{
	return m_tracks->m_masterHeader.m_numTracks;
}

HK_FORCE_INLINE int hkbGeneratorOutput::getNumPoseLocal() const
{
	if ( trackExists( TRACK_POSE ) )
	{
		return m_tracks->m_trackHeaders[TRACK_POSE].m_numData;
	}

	return 0;
}

HK_FORCE_INLINE int hkbGeneratorOutput::getNumBoneWeights() const
{
	if ( trackExists( TRACK_POSE ) )
	{
		return m_tracks->m_trackHeaders[TRACK_POSE].m_numData + 1;
	}

	return 0;
}

HK_FORCE_INLINE const hkbGeneratorPartitionInfo& hkbGeneratorOutput::getPartitionInfo() const
{
	return *reinterpret_cast<const hkbGeneratorPartitionInfo*>( (getBoneWeights() + HK_NEXT_MULTIPLE_OF(4, getNumBoneWeights())) );
}

HK_FORCE_INLINE int hkbGeneratorOutput::getNumFloats() const
{
	if ( trackExists( TRACK_FLOAT_SLOTS ) )
	{
		return m_tracks->m_trackHeaders[TRACK_FLOAT_SLOTS].m_numData;
	}

	return 0;
}

HK_FORCE_INLINE int hkbGeneratorOutput::getNumAttributes() const
{
	if ( trackExists( TRACK_ATTRIBUTES ) )
	{
		return m_tracks->m_trackHeaders[TRACK_ATTRIBUTES].m_numData;
	}

	return 0;
}

HK_FORCE_INLINE hkReal* hkbGeneratorOutput::accessTrackDataReal( int trackId )
{
	HK_ASSERT2( 0x459ab350, trackExists( trackId ), "track not found" );
	HK_ASSERT2( 0x5123be6b, m_tracks->m_trackHeaders[trackId].m_type == TRACK_TYPE_REAL, "wrong track type" );

	hkReal* p = reinterpret_cast<hkReal*>( reinterpret_cast<char*>( m_tracks ) + m_tracks->m_trackHeaders[trackId].m_dataOffset );

	HK_CHECK_ALIGN16( p );
	return p;
}

HK_FORCE_INLINE hkQsTransform* hkbGeneratorOutput::accessTrackDataQsTransform( int trackId )
{
	HK_ASSERT2( 0x459ab350, trackExists( trackId ), "track not found" );
	HK_ASSERT2( 0x9123b46a, m_tracks->m_trackHeaders[trackId].m_type == TRACK_TYPE_QSTRANSFORM, "wrong track type" );

	hkQsTransform* p = reinterpret_cast<hkQsTransform*>( reinterpret_cast<char*>( m_tracks ) + m_tracks->m_trackHeaders[trackId].m_dataOffset );

	HK_CHECK_ALIGN16( p );
	return p;
}

HK_FORCE_INLINE hkQsTransform& hkbGeneratorOutput::accessWorldFromModel()
{
	return *( accessTrackDataQsTransform( TRACK_WORLD_FROM_MODEL ) );
}

HK_FORCE_INLINE hkQsTransform& hkbGeneratorOutput::accessExtractedMotion()
{
	return *( accessTrackDataQsTransform( TRACK_EXTRACTED_MOTION ) );
}

HK_FORCE_INLINE hkQsTransform* hkbGeneratorOutput::accessPoseLocal()
{
	return accessTrackDataQsTransform( TRACK_POSE );
}

HK_FORCE_INLINE hkReal* hkbGeneratorOutput::accessBoneWeights()
{
	Track trackInfo = accessTrack( TRACK_POSE );
	return reinterpret_cast<hkReal*>( trackInfo.accessDataQsTransform() + trackInfo.getNumData() );
}

HK_FORCE_INLINE hkbGeneratorPartitionInfo& hkbGeneratorOutput::accessPartitionInfo()
{
	return *reinterpret_cast<hkbGeneratorPartitionInfo*>( (accessBoneWeights() + HK_NEXT_MULTIPLE_OF(4, getNumBoneWeights())) );
}

HK_FORCE_INLINE void hkbGeneratorOutput::setPoseAdditive()
{
	accessTrackHeader( TRACK_POSE ).m_flags.orWith( TRACK_FLAG_ADDITIVE_POSE );
}

HK_FORCE_INLINE bool hkbGeneratorOutput::isPoseAdditive() const
{
	return ( getTrack( TRACK_POSE ).getTrackHeader()->m_flags.get( TRACK_FLAG_ADDITIVE_POSE ) != 0 );
}

HK_FORCE_INLINE void hkbGeneratorOutput::clearPoseData()
{
	if( trackExists( hkbGeneratorOutput::TRACK_POSE ) )
	{
		const int numPoseLocal = getNumPoseLocal();
		hkQsTransform* poseLocal = accessTrackDataQsTransform( TRACK_POSE );

		hkString::memSet16( poseLocal, &hkVector4::getZero(), (sizeof(hkQsTransform) * numPoseLocal) >> 4 );

		const int numBoneWeights = getNumBoneWeights();
		hkReal* boneWeights = accessBoneWeights();

		hkString::memSet4( boneWeights, 0, (sizeof(hkReal) * numBoneWeights) >> 2 );
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
