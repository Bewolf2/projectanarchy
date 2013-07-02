/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKAI_PATH_WRITER_H
#define HKAI_PATH_WRITER_H

#include <Ai/Pathfinding/NavMesh/Streaming/hkaiStreamingCollection.h>
#include <Ai/Pathfinding/Character/hkaiPath.h>
#ifdef HK_PLATFORM_SPU
#	include <Common/Base/Container/LocalArray/hkLocalArray.h>
#	include <Common/Base/Spu/Dma/Iterator/hkSpuWriteOnlyIterator.h>
#endif



/// hkaiPathWriter is used to by the hkaiNavMeshPathSmoothingUtil to write to hkArrays and general pointers, from both the CPU and SPU
class hkaiPathWriter
{
public:
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_AI_ASTAR, hkaiPathWriter );

#ifndef HK_PLATFORM_SPU
	typedef hkaiPath::PathPoint* hkaiPathOutputIterator;
#else
	typedef hkSpuWriteOnlyIterator<hkaiPath::PathPoint, 8*sizeof(hkaiPath::PathPoint)> hkaiPathOutputIterator;
#endif
	
	hkaiPathWriter();
	void init(hkArray<hkaiPath::PathPoint>* pathArray, hkaiPath::PathPoint* pathPtr, int maxLength);

	void addPointToPath(hkVector4Parameter p, hkVector4Parameter normal, hkaiRuntimeIndex sectionId, hkUint32 userEdgeData, hkUint8 flags);

	void addTerminatorToPathPointer();
	
	inline bool isFull() const
	{
		return !m_pathArrayOut && (m_totalNumOutput >= m_maxNumOutput);
	}

protected:
	// Writes only to the array (if non-NULL)
	void addPointToPathArray(hkaiPath::PathPoint& point);

	// Writes only to the pointer (if non-NULL)
	void addPointToPathPointer(hkaiPath::PathPoint& point);


public:
	HK_PAD_ON_SPU( hkArray<hkaiPath::PathPoint>* ) m_pathArrayOut;
	hkaiPathOutputIterator m_pathPtrOut;
	HK_PAD_ON_SPU( int ) m_maxNumOutput;
	HK_PAD_ON_SPU( int ) m_totalNumOutput;

	HK_PAD_ON_SPU( int ) m_numWritten;
	hkVector4 m_previousPoint;
	hkSimdReal m_totalDistance;
};


#endif // HKAI_PATH_WRITER_H

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
