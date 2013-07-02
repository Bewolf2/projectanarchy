/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKAI_STRING_PULLING_H
#define HKAI_STRING_PULLING_H

#include <Ai/Pathfinding/Utilities/PathWriter/hkaiPathWriter.h>
#include <Common/Base/Container/LocalArray/hkLocalArray.h>

class hkaiGeneralAccessor;

// Undefine this to debug and *also* in #include <Ai/Pathfinding/NavMesh/Smoothing/hkaiNavMeshPathSmoothingUtil.h>
//#define HKAI_DEBUG_SMOOTHING

	/// Nav mesh path smoothing utility.
class hkaiStringPulling
#ifndef HK_PLATFORM_SPU
	: public hkReferencedObject
#endif
{
public:

#ifndef HK_PLATFORM_SPU
	HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE);
#endif

	friend class hkaiNavMeshPathSmoothingUtil;

	enum Side
	{
		RIGHT = 0,
		LEFT = 1,

		UNDEFINED = -1
	};

	enum Type
	{
		NONE = 0,
		START_POINT,
		END_POINT
	};

private:

		/// Intermediate data for string pulling.
	struct Vertex
	{
		Vertex()
		{
			m_index = -2;
			m_side = UNDEFINED;
			m_type = NONE;
			m_clearanceIsNegative = false;
			m_userEdge = false;
			m_userEdgeSwapped = false;
		}

		HK_FORCE_INLINE ~Vertex()
		{

		}

		HK_FORCE_INLINE const hkVector4& getCenter() const { return m_center; }
		HK_FORCE_INLINE void setCenter( hkVector4Parameter c ) { m_center = c; m_clearanceIsNegative = (c(3) < 0.0f);}
		HK_FORCE_INLINE void setCenterXYZ( hkVector4Parameter c ) { m_center.setXYZ(c); }

		int m_index;
		hkEnum<Side, hkUint8> m_side;
		hkEnum<Type, hkUint8> m_type;
		hkBool m_clearanceIsNegative;
		hkBool m_userEdge;
		hkBool m_userEdgeSwapped;
		hkVector4 m_pos;

		inline int getSide() const;

	private:
		hkVector4 m_center;

	};

		/// Internal state of the string pulling system.
	class Funnel
	{
	public:
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE,hkaiStringPulling::Funnel);
		enum
		{
			MAX_CAPACITY_SPU = 128,
		};

		Funnel();

		void clear();

		bool add(const Vertex& vertex, Side side);

		void pop(Side side, Vertex& vertexOut );
		hkBool32 apexPopped() const;
		Vertex& getApex();
		const Vertex& getApex() const;
		Vertex& operator[](int index);
		const Vertex& operator[](int index) const;

	public:

			/// Internal accessor.
		class Iterator
		{
		public:
			Iterator(Funnel& funnel, Side side, int index = -1);
			Vertex& operator*() { return m_funnel[m_funnelIndex]; }
			Vertex* operator->() { return &m_funnel[m_funnelIndex]; }
			void operator++(int);
			hkBool32 isValid() const;
			Iterator& operator=(const Iterator& other);
			hkBool32 lessOrEqualApex() const;

			int getIndex() const { return m_funnelIndex; }
			void setIndex(int index) { m_funnelIndex = index; }

		private:
			HK_PAD_ON_SPU(int) m_side;
			HK_PAD_ON_SPU(int) m_funnelIndex;
			Funnel& m_funnel;
		};

		HK_PAD_ON_SPU(int) m_start;
		HK_PAD_ON_SPU(int) m_apex;
#ifdef HK_PLATFORM_SPU
		hkLocalArray<Vertex> m_data;
#else
		hkArray<Vertex>::Temp m_data;
#endif
	};

		/// Helper class to prefetch nav mesh data access.
	class PathEdgeCache
	{
	public:
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE,hkaiStringPulling::PathEdgeCache);

		PathEdgeCache( const hkArrayBase<hkaiPackedKey>& visitedEdgeKeys );

		void cacheMeshData( hkaiGeneralAccessor& accessor, hkaiPackedKey startFaceKey, hkVector4Parameter up, hkBool32 computeNormals  );

		HK_FORCE_INLINE hkaiPackedKey getPathIncomingEdgeKey( int pathEdgeIndex ) const;

		HK_FORCE_INLINE const hkaiNavMesh::Edge& getPathIncomingEdge( int pathEdgeIndex ) const;
		HK_FORCE_INLINE const hkaiNavMesh::Edge& getPathOppositeEdge( int pathEdgeIndex ) const;
		HK_FORCE_INLINE hkaiNavMesh::EdgeData getPathIncomingEdgeData( int pathEdgeIndex ) const;
		HK_FORCE_INLINE hkaiNavMesh::EdgeData getPathOppositeEdgeData( int pathEdgeIndex ) const;

		// Get the vertex on one side of a path edge, including global clearance in the W component.
		HK_FORCE_INLINE void getPathIncomingEdgePoint( int pathEdgeIndex, Side side, hkVector4& edgePoint ) const;

		// Get both vertices of a path edge, including global clearance in the W component.
		HK_FORCE_INLINE void getPathIncomingEdgePoints( int pathEdgeIndex, hkVector4& edgePointA, hkVector4& edgePointB ) const;

		// Get both vertices of a path opposite edge. Does not include clearance information.
		HK_FORCE_INLINE void getPathOppositeEdgePoints( int pathEdgeIndex, hkVector4& edgePointA, hkVector4& edgePointB ) const;

		HK_FORCE_INLINE const hkVector4& getPathNormal( int pathEdgeIndex  ) const;

		HK_FORCE_INLINE const hkVector4& getStartFaceNormal() const { return m_startFaceNormal; }

	private:
			/// Saved edge data.
		struct PathEdge
		{
			hkaiNavMesh::Edge m_incomingEdge;
			hkaiNavMesh::Edge m_oppositeEdge;
			hkaiNavMesh::EdgeData m_incomingEdgeData;
			hkaiNavMesh::EdgeData m_oppositeEdgeData;

			// Incoming edge vertex positions, with global clearance stored in W component
			hkVector4 m_incomingEdgePointA_GC;
			hkVector4 m_incomingEdgePointB_GC;

			// Opposite edge vertex positions
			hkVector4 m_oppositeEdgePointA;
			hkVector4 m_oppositeEdgePointB;

			// Opposite face normal
			hkVector4 m_faceNormal;
		};

		hkVector4 m_startFaceNormal;
		hkLocalArray< PathEdge > m_pathEdges;
		const hkArrayBase<hkaiPackedKey>& m_pathEdgeKeys;
	};

public:

	static hkReal TOLERANCE;

	hkaiStringPulling(hkaiGeneralAccessor& accessor, const hkArray<hkaiPackedKey>& visitedEdgeKeys, hkaiPackedKey startFaceKey,
		hkVector4Parameter startPoint, hkVector4Parameter endPoint, hkBool32 project, hkBool32 computeNormals, hkVector4Parameter up, 
		hkReal radius, hkaiPathWriter& writer);

	Side getSide(hkVector4Parameter a, hkVector4Parameter b, hkVector4Parameter p) const;

	hkBool32 undefinedTangent(const Vertex& a, const Vertex& b) const;
	void convertToTangent( hkVector4Parameter start, hkVector4& endInOut, Side side) const;
	
		/// Static tangent computation, called by the non-static one
	static void HK_CALL convertToTangent( hkVector4Parameter start, hkVector4Parameter up, hkVector4& endInOut, hkSimdRealParameter radius, Side side);

	void convertToTangentNorm(hkVector4& startInOut, hkVector4& endInOut, Side side) const;
	void convertToTangentCrossed(hkVector4& startInOut, hkVector4& endInOut, Side side) const;
	hkBool32 isUserEdge(int i) const;

	hkBool32 getVertexPosition(int edgeIndex, Side side, hkVector4& vertexOut, bool& userEdgeSwapped) const;
	bool getUserEdgeVertexPosition(int edgeIndex, Side side, hkVector4& vertexOut) const;
	int getIndex(int edgeIndex, Side side) const;

	void getVertex(Vertex& inOut) const;

	void genericTangent(hkVector4& start, Side startSide, hkVector4& end, Side endSide) const;
	void genericTangent(Vertex& start, Vertex& end) const;

	bool checkTermination(const Vertex& vertex, Side side, int index);

	bool isShared(int i, Side side);

	bool stringPull();

	hkBool32 handleUserEdge(const Vertex& vertex, int index);
	hkBool32 isCloser(const Vertex& s, const Vertex& e, const Vertex& p, int lastEdgeIndex ) const;
	hkBool32 isUndefinedTangentVertexAddable(const Vertex& s0, const Vertex& e0, const Vertex& p) const;
	bool doWedgeCheck(const Vertex& wedgeStart, const Vertex& wedgeEnd, Vertex& start, Vertex& end, const Vertex& originalEnd, int lastEdgeIndex );

	void addVertexToPath(const Vertex& a, hkUint8 pointType);
	void addSegmentToPath(const Vertex& a, const Vertex& b);
	bool terminateOutput();
	void projectSegmentEndpoint(const Vertex& a, Vertex& projOut);

	HK_FORCE_INLINE static Side oppositeSide(Side side)
	{
		switch (side)
		{
		case LEFT:
			return RIGHT;
		case RIGHT:
			return LEFT;
		default:
			return UNDEFINED;
		}
	}

private:
	hkSimdReal m_radius;
	const HK_PAD_ON_SPU(hkBool32) m_radiusIsZero;
	const HK_PAD_ON_SPU(int) m_pathLength;

#ifdef HKAI_DEBUG_SMOOTHING
	// Used to display the navmesh and debug data.
	HK_PAD_ON_SPU(class hkaiNavMeshPathSmoothingUtil*) m_debugUtil;
#endif

	hkaiGeneralAccessor& m_accessor;

	// Cached mesh data, gathered in a single pass at initialization.
	PathEdgeCache m_pathEdgeCache;

	Funnel m_funnel;
	const hkVector4 m_up;
	const hkVector4 m_startPoint;
	const hkVector4 m_endPoint;
	hkaiPathWriter& m_writer;
	Vertex m_lastVertexInPath;
	const hkBool32 m_project;
	const hkaiPackedKey m_startFaceKey;
};

#endif // HKAI_STRING_PULLING_H

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
