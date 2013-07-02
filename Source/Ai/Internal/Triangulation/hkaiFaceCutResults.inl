/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifdef HK_PLATFORM_PS3_SPU
#	include <Common/Base/Memory/PlatformUtils/Spu/hkMemoryRouterSpu.h>	// For max sizes for cut results allocation on PPU
#else
#	include <Common/Base/Memory/Allocator/Thread/hkThreadMemory.h>	// For max sizes for cut results allocation on PPU
#endif

inline int hkaiFaceCutResults::getNumPolys() const
{ 
	return m_numPolys;
}

inline hkUint32 hkaiFaceCutResults::getSizeOfPoly(PolyPointer h) const
{
	hkUint32 polySize = *reinterpret_cast<const hkUint32*>(h);
	return polySize;
}

inline int hkaiFaceCutResults::getPolyMaterial(PolyPointer h) const
{
	const int byteOffset = 4;
	int polyMaterial = *reinterpret_cast<const int*>( hkAddByteOffsetConst(h, byteOffset) );
	return polyMaterial;
}

inline int hkaiFaceCutResults::getNumEdges() const
{
	return m_numEdges;
}

inline hkUint32 hkaiFaceCutResults::getPolyIdx(int eRel, PolyPointer h) const 
{
	const int byteOffset = 8 + eRel*sizeof(hkUint32);
	hkUint32 polyIdx = *reinterpret_cast<const hkUint32*>( hkAddByteOffsetConst(h, byteOffset) );
 
	return polyIdx;
}

inline hkaiFaceCutResults::ConnectivityFlags hkaiFaceCutResults::getEdgeFlag(int eRel, PolyPointer h) const
{
	const int byteOffset = 8 + eRel*sizeof(hkaiFaceCutResults::ConnectivityFlags) + 2*getSizeOfPoly(h)*sizeof(hkUint32);
	ConnectivityFlags edgeFlag = *reinterpret_cast<const ConnectivityFlags*>( hkAddByteOffsetConst(h, byteOffset) );
	return edgeFlag;
}

inline hkUint32 hkaiFaceCutResults::getEdgeConnectivity(int eRel, PolyPointer h) const
{
	const int byteOffset = 8 + eRel*sizeof(hkUint32) + getSizeOfPoly(h)*sizeof(hkUint32);
	hkUint32 edgeCon = *reinterpret_cast<const hkUint32*>( hkAddByteOffsetConst(h, byteOffset) );
	return edgeCon;
}

inline int hkaiFaceCutResults::getPolyDataSize(int numPolys, int numEdges)
{
	int polyDataSize =
		sizeof(hkUint32) * numPolys // poly sizes
		+ sizeof(hkUint32) * numPolys // materials
		+ sizeof(hkUint32) * numEdges // vertex index for each edge
		+ sizeof(hkUint32) * numEdges // connectivity for each edge
		+ sizeof(ConnectivityFlags) * numEdges // flag for each edge

		// worst case padding would happen if each poly have (size%4) == 1 and we had to pad 3 bytes
		+ 3*sizeof(ConnectivityFlags) * numPolys; 

	return polyDataSize;
}

inline int hkaiFaceCutResults::getRequiredSize(int numPolys, int numEdges)
{
	int polyDataSize = getPolyDataSize(numPolys, numEdges);

	// This might reserve more vertex data than it needs
	
	// Pad up so that the vectors are aligned
	const int reserveSize = HK_NEXT_MULTIPLE_OF(sizeof(hkVector4), polyDataSize) + numEdges * sizeof(hkVector4);
#ifdef HK_ARCH_PS3SPU
	return (reserveSize > hkThreadMemorySpu::MEMORY_MAX_SIZE_LARGE_BLOCK) ? HK_INT32_MAX :  hkThreadMemorySpu::getInstance().getAllocatedSize(HK_NULL, reserveSize);
#else
	return reserveSize;
#endif
}

#ifndef HK_PLATFORM_SPU
inline void hkaiFaceCutResults::deallocateHeap()
{
	if(m_data)
	{
		hkDeallocateChunk( m_data, m_allocatedSize,	HK_MEMORY_CLASS_ARRAY );
	}
}
#endif


inline void hkaiFaceCutResults::writeUint32( PolyOutput& dataPtr, const hkUint32 x ) const
{
	*dataPtr = x;
	dataPtr++;
}

inline void hkaiFaceCutResults::writeUint32s( PolyOutput& dataPtr, const hkUint32* x, int numWords ) const
{
	// We increment the pointer after each write so that the code works with an SPU iterator too.
	for (int i=0; i<numWords; i++)
	{
		*dataPtr = *x;
		dataPtr++;
		x++;
	}
}


inline void hkaiFaceCutResults::writeFlags(  PolyOutput& dataPtr, const ConnectivityFlags* f, int numFlags ) const
{
	int numWords = numFlags / 4;
	int numLeftovers = numFlags & 3;

	writeUint32s( dataPtr, (const hkUint32*) f, numWords );
	f += numWords*4;

	// Pad the ends so that we always write a multiple of 4 bytes.
	if (numLeftovers > 0)
	{
		union
		{
			hkUint8 m_byte[4];
			hkUint32 m_word;
		} u;

		u.m_byte[0] = (numLeftovers > 0) ? f[0].get() : hkUint8(-1);
		u.m_byte[1] = (numLeftovers > 1) ? f[1].get() : hkUint8(-1);
		u.m_byte[2] = (numLeftovers > 2) ? f[2].get() : hkUint8(-1);
		u.m_byte[3] = (numLeftovers > 3) ? f[3].get() : hkUint8(-1);

		writeUint32( dataPtr, u.m_word);
	}
}


inline void hkaiFaceCutResults::writePolyInfo(PolyOutput& dataPtr, int material, int numEdges, const hkArrayBase<hkUint32>& indices, const hkArrayBase<hkUint32>& connect, const hkArrayBase<hkaiFaceCutResults::ConnectivityFlags>& flags )
{
	writeUint32(dataPtr, numEdges);
	writeUint32(dataPtr, material);
	writeUint32s(dataPtr, indices.begin(), numEdges); 
	writeUint32s(dataPtr, connect.begin(), numEdges); 
	writeFlags(dataPtr, flags.begin(), numEdges); 


	HK_ASSERT(0x173e7c95, (hkUlong((const hkUint32*)dataPtr)%4) == 0);
}

inline hkUint32* hkaiFaceCutResults::init(int numPolys, int numEdges)
{
#ifdef HK_PLATFORM_SPU
#	ifdef HK_PLATFORM_PS3_SPU
		const int maxAllocationAllowed = hkThreadMemorySpu::MEMORY_MAX_SIZE_LARGE_BLOCK;
#	else
		const int maxAllocationAllowed = hkThreadMemory::MEMORY_MAX_SIZE_LARGE_BLOCK;	
#	endif
#else 
	const int maxAllocationAllowed = HK_INT32_MAX;
#endif

	m_numPolys = (hkInt16) numPolys;
	m_numEdges = (hkInt16) numEdges;
	m_numVertices = 0;
	
	int reserveSize = getRequiredSize(numPolys, numEdges);
	if( reserveSize > maxAllocationAllowed )
	{
		return HK_NULL;
	}

	if(reserveSize > 0)
	{
		m_data = hkAllocateChunk<hkUint8>( reserveSize, HK_MEMORY_CLASS_ARRAY );
	}
	m_allocatedSize = reserveSize;

	return (hkUint32*) m_data;
}

inline hkaiFaceCutResults::PolyPointer hkaiFaceCutResults::getFirstPolyPointer() const
{
	return m_data; 
}

inline hkaiFaceCutResults::PolyPointer hkaiFaceCutResults::getNextPolyPointer( PolyPointer h) const
{
	hkUint32 numEdges = *reinterpret_cast<const hkUint32*>(h);
	int offset =  4 // poly size
				+ 4 // material
				+ sizeof(hkUint32) * numEdges // vertex index for each edge
				+ sizeof(hkUint32) * numEdges // connectivity for each edge
				+ sizeof(ConnectivityFlags) * HK_NEXT_MULTIPLE_OF(4, numEdges); // flag for each edge + padding
	
	return hkAddByteOffsetConst(h, offset); 
}

inline hkVector4* hkaiFaceCutResults::getVertexStart()
{
	int polyDataSize = getPolyDataSize(m_numPolys, m_numEdges);

	return reinterpret_cast<hkVector4*> ( hkAddByteOffset(m_data, HK_NEXT_MULTIPLE_OF(sizeof(hkVector4), polyDataSize)) );
}

inline const hkVector4* hkaiFaceCutResults::getVertexStart() const
{
	return const_cast<hkaiFaceCutResults*>(this)->getVertexStart();
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
