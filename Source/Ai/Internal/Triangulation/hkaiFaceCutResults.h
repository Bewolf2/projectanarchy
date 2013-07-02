/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef HK_FACE_CUT_RESULTS_H
#define HK_FACE_CUT_RESULTS_H

#include <Ai/Pathfinding/NavMesh/hkaiNavMesh.h>

#ifdef HK_PLATFORM_SPU
#	include <Common/Base/Spu/Dma/Iterator/hkSpuWriteOnlyIterator.h>
#endif

	/// Intermediate results for nav mesh cutting. Contains simple re-triangulation results for one face.
struct hkaiFaceCutResults
{
public:
HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE,hkaiFaceCutResults);


	/// Edge connectivity information
	enum ConnectivityTypes
	{
			/// Was an edge in the original mesh
		ORIGINAL = 0,
			/// Blocked edge from a silhouette
		SILHOUETTE = 1,
			/// Internal edge resulting from triangulation
		INTERNAL = 2,
		
			/// Mask for getting the connectivity from the info byte.
		CONNECTIVITY_MASK = 3,

		//
		// Additional information
		//

			/// Whether or not the corresponding vertex new, e.g., it was added by a silhouette crossing an edge.
			/// Note that this is different from the ORIGINAL flag.
		VERTEX_IS_NEW = 4,
	};

	enum ConnectivitySpecialValues
	{
			/// Corresponds to hkaiFaceCutResults::SILHOUETTE.
		CONNECTIVITY_CONSTRAINED = 0xFFFFFFFF,

			/// Corresponds to hkaiFaceCutResults::INTERNAL.
		CONNECTIVITY_UNCONSTRAINED = 0xFFFFFFFE,
	};

	enum ResultsBits
	{
			/// No cutting was performed (i.e. no overlapping silhouettes)
		RESULT_NO_CUTS = 1
	};

	typedef hkFlags<ConnectivityTypes, hkUint8> ConnectivityFlags;
	typedef hkFlags<ResultsBits, hkUint8> ResultFlags;

#ifdef HK_PLATFORM_SPU
	
	typedef hkSpuWriteOnlyIterator<hkUint32, 16> PolyOutput;
	typedef hkSpuWriteOnlyIterator<hkVector4, 64> VertexOutput;
#else
	typedef hkUint32* PolyOutput;
	typedef hkVector4* VertexOutput;
#endif

	hkaiFaceCutResults()
	:	m_flags(0),
		m_data(HK_NULL)
	{
	}
	
	// Data is laid out as follow:
	/*	polysize 0
			polyIdx0, polyIdx1	...	polyIdxN
			polyCon0, polyCon1	... polyConN
			polyFlg0, polyFlg1	...	polyFlgN
		(padding to multiple of 4)

		polysize 1
			polyIdx1,0	...

		(padding, determined during initialization)
		vertex0
		vertex1
		...
	*/

	typedef const hkUint8* PolyPointer;
	PolyPointer getFirstPolyPointer() const ;
	PolyPointer getNextPolyPointer(PolyPointer h) const;

	HK_ALIGN16(hkUint32)				m_originalFace;
	hkInt16								m_numPolys;
	hkInt16								m_numEdges;
	hkInt16								m_numVertices;
	int 								m_allocatedSize;
	ResultFlags							m_flags;

	inline int getNumPolys() const;
	inline hkUint32 getSizeOfPoly(PolyPointer h) const;
	inline int getPolyMaterial( PolyPointer h ) const;
	inline int getNumEdges() const;

	inline hkUint32 getPolyIdx(int eRel, PolyPointer h) const;
	inline ConnectivityFlags getEdgeFlag(int eRel, PolyPointer h) const;
	inline hkUint32 getEdgeConnectivity(int eRel, PolyPointer h) const;

	inline hkVector4* getVertexStart();
	inline const hkVector4* getVertexStart() const;

#ifndef HK_PLATFORM_SPU
	inline void deallocateHeap();
#endif

		/// Returns the pointer where the data is written to.
	inline hkUint32* init(int numPolys, int numEdges);
	inline void writePolyInfo(PolyOutput& dataPtr, int material, int numEdges, const hkArrayBase<hkUint32>& edgeIndices, const hkArrayBase<hkUint32>& connectivity, const hkArrayBase<hkaiFaceCutResults::ConnectivityFlags>& flags );

	/// Required size for all polygon data (indices, connectivity, and flags)
	static int getPolyDataSize(int numPolys, int numEdges);
	/// Total required size (getPolyDataSize() + vertex data)
	static int getRequiredSize(int numPolys, int numEdges);

protected:

	inline void writeUint32( PolyOutput& dataPtr, const hkUint32 x ) const;
	inline void writeUint32s( PolyOutput& dataPtr, const hkUint32* x, int numWords ) const;
	inline void writeFlags( PolyOutput& dataPtr, const ConnectivityFlags* f, int numFlags ) const;

	hkUint8*					m_data;
};

#include <Ai/Internal/Triangulation/hkaiFaceCutResults.inl>

#endif // HK_FACE_CUT_RESULTS_H

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
