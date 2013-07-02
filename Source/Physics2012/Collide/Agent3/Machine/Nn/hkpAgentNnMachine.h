/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HK_COLLIDE2_AGENT3_COLLECTION_MACHINE_H
#define HK_COLLIDE2_AGENT3_COLLECTION_MACHINE_H

#include <Physics2012/Collide/Agent3/hkpAgent3.h>
#include <Physics2012/Collide/Dispatch/Agent3Bridge/hkpAgent3Bridge.h>
#include <Physics2012/Collide/Agent3/Machine/Nn/hkpAgentNnTrack.h>

class hkpLinkedCollidable;
class hkpShapeKeyTrackWriter;
class hkpShapeKeyTrackConsumer;

#if defined(HK_PLATFORM_HAS_SPU)

// this struct holds essential data for the collision detector, but in a padded form
struct hkpSpuAgentSectorJobCollisionObjects
{
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_CDINFO, hkpSpuAgentSectorJobCollisionObjects );

	hkPadSpu<hkpCollidable*> m_collidableA;
	hkPadSpu<hkpCollidable*> m_collidableB;
	hkPadSpu<hkpContactMgr*> m_contactMgr;
};

// a small cache to be used within the hkSpuAgentSectorJob; note that this pointer will be invalid outside of the hkSpuAgentSectorJob!
extern class hkSpu4WayCache* g_SpuCollideUntypedCache;

#endif

struct hkpAgentNnMachinePaddedEntry: public hkpAgentNnEntry
{
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_CDINFO, hkpAgentNnMachinePaddedEntry );

#if HK_POINTER_SIZE == 4
	hkUint32 m_padOutTo16BytesAlignment[2];
#else // HK_POINTER_SIZE == 8
	hkUint32 m_padOutTo16BytesAlignment[2];
#endif
};

struct hkpAgentNnMachineTimEntry: public hkpAgentNnEntry
{
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_CDINFO, hkpAgentNnMachineTimEntry );

	hkTime     m_timeOfSeparatingNormal;		// only used if tims are enabled
	hkVector4  m_separatingNormal;
};
#if !defined(HK_PLATFORM_HAS_SPU)
#	define HK_FOR_ALL_AGENT_ENTRIES_BEGIN( TRACK, ENTRY )											\
{																									\
	for (int HKLOOP_sectorIndex = 0; HKLOOP_sectorIndex < TRACK.m_sectors.getSize(); )				\
    {																								\
	    hkpAgentNnSector* HKLOOP_currentSector = TRACK.m_sectors[HKLOOP_sectorIndex];				\
	    hkpAgentNnEntry* ENTRY = HKLOOP_currentSector->getBegin();									\
	    HKLOOP_sectorIndex++;																		\
	    hkpAgentNnEntry* HKLOOP_endEntry =  (HKLOOP_sectorIndex == TRACK.m_sectors.getSize()) ?		\
		    hkAddByteOffset(ENTRY, TRACK.m_bytesUsedInLastSector) : HKLOOP_currentSector->getEnd();	\
	    for( ; ENTRY < HKLOOP_endEntry; ENTRY = hkAddByteOffset( ENTRY, ENTRY->m_size ) )			\
    	{
#else
#	define HK_FOR_ALL_AGENT_ENTRIES_BEGIN( TRACK, ENTRY )										\
{																								\
	for (int HKLOOP_sectorIndex = 0; HKLOOP_sectorIndex < TRACK.m_sectors.getSize(); )			\
    {																							\
	    hkpAgentNnSector* HKLOOP_currentSector = TRACK.m_sectors[HKLOOP_sectorIndex];			\
	    hkpAgentNnEntry* ENTRY = HKLOOP_currentSector->getBegin();								\
	    HKLOOP_sectorIndex++;																	\
	    hkpAgentNnEntry* HKLOOP_endEntry =  (HKLOOP_sectorIndex == TRACK.m_spuNumSectors) ?		\
		    hkAddByteOffset(ENTRY, TRACK.m_spuBytesUsedInLastSector)							\
		    : (HKLOOP_sectorIndex == TRACK.m_sectors.getSize()) ?								\
			    hkAddByteOffset(ENTRY, TRACK.m_ppuBytesUsedInLastSector) : HKLOOP_currentSector->getEnd();		\
	       for( ; ENTRY < HKLOOP_endEntry; ENTRY = hkAddByteOffset( ENTRY, ENTRY->m_size ) )	\
  		   {
    #endif

#define HK_FOR_ALL_AGENT_ENTRIES_END } } }


extern "C"
{
	void HK_CALL hkAgentNnMachine_DestroyTrack( hkpAgentNnTrack& track, hkpCollisionDispatcher* dispatch );

	void HK_CALL hkAgentNnMachine_GetAgentType( const hkpCdBody* cdBodyA, const hkpCdBody* cdBodyB, const hkpProcessCollisionInput& input, int& agentTypeOut, int& isFlippedOut );

	hkpAgentNnEntry* HK_CALL hkAgentNnMachine_CreateAgent( hkpAgentNnTrack& track, hkpLinkedCollidable* collA, const hkpCdBody* firstNonTransformBodyA, hkpLinkedCollidable* collB, const hkpCdBody* firstNonTransformBodyB, hkUchar cdBodyHasTransformFlag, int agentType, const hkpProcessCollisionInput& input, hkpContactMgr* mgr );

	void HK_CALL hkAgentNnMachine_DestroyAgent( hkpAgentNnTrack& track, hkpAgentNnEntry* entry, hkpCollisionDispatcher* dispatch, hkCollisionConstraintOwner& constraintOwner );


	void HK_CALL hkAgentNnMachine_AppendTrack( hkpAgentNnTrack& track, hkpAgentNnTrack& appendee);


	void HK_CALL hkAgentNnMachine_UpdateShapeCollectionFilter( hkpAgentNnEntry* entry, const hkpCollisionInput& input, hkCollisionConstraintOwner& constraintOwner );

	//
	// processing
	//

	void HK_CALL hkAgentNnMachine_ProcessAgent( hkpAgentNnEntry* entry,	const hkpProcessCollisionInput& input, hkpProcessCollisionOutput& output, hkpContactMgr* mgr );


		// process a track, stops if out of memory
	void HK_CALL hkAgentNnMachine_ProcessTrack( class hkpConstraintOwner* owner, hkpAgentNnTrack& track,	const hkpProcessCollisionInput& input );

	bool HK_CALL hkAgentNnMachine_IsEntryOnTrack(hkpAgentNnTrack& track, hkpAgentNnEntry* entry);

	void HK_CALL hkAgentNnMachine_InternalDeallocateEntry(hkpAgentNnTrack& track, hkpAgentNnEntry* entry);

	// ALSO FOR EXTERNAL USE

		/// Makes a copy of the entry in the destTrack and redirects all pointers to entry to the new copy.
		/// Does not touch the entry. In the end, no pointers will point to the original entry
	hkpAgentNnEntry* HK_CALL hkAgentNnMachine_CopyAndRelinkAgentEntry( hkpAgentNnTrack& destTrack, hkpAgentNnEntry* entry );

#if ! defined (HK_PLATFORM_SPU)
	hkpAgentNnEntry* HK_CALL hkAgentNnMachine_FindAgent( const hkpLinkedCollidable* collA, const hkpLinkedCollidable* collB );
#endif

		// activation/deactiation/setPositionOnEntity
	void HK_CALL hkAgentNnMachine_InvalidateTimInAgent( hkpAgentNnEntry* entry, const hkpCollisionInput& input );

	void HK_CALL hkAgentNnMachine_WarpTimeInAgent( hkpAgentNnEntry* entry, hkTime oldTime, hkTime newTime, const hkpCollisionInput& input );


	//
	// Debugging
	//

#if ! defined (HK_PLATFORM_SPU)
	void HK_CALL hkAgentNnMachine_AssertTrackValidity( hkpAgentNnTrack& track );
#endif

	//
	// UNDONE
	//

	void HK_CALL hkAgentNnMachine_TouchAgent( hkpAgentEntry* entry,	const hkpProcessCollisionInput& input );

	const hkpCdBody* HK_CALL hkAgentMachine_processTransformedShapes(const hkpCdBody* cdBody, hkpCdBody* newCdBodies, class hkMotionState* newMotionStates, int numSlots, hkPadSpu<hkUchar>& cdBodyHasTransformFlag);

}





#endif // HK_COLLIDE2_AGENT3_COLLECTION_MACHINE_H

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
