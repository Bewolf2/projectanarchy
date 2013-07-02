/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef HKAI_NAV_VOLUME_H
#define HKAI_NAV_VOLUME_H

#include <Common/Base/Types/Geometry/Aabb/hkAabb.h>
#include <Ai/Pathfinding/hkaiBaseTypes.h>
#include <Ai/Pathfinding/NavMesh/Streaming/hkaiStreamingSet.h>

extern const class hkClass hkaiNavVolumeClass;
extern const class hkClass hkaiNavVolumeCellClass;
extern const class hkClass hkaiNavVolumeEdgeClass;

/// This contains the nav volume data which is used for 3D pathfinding in Havok A.I.
/// 
/// The nav volume represents navigable space as a set of connected axis-aligned boxes.
class hkaiNavVolume : public hkReferencedObject
{
	public:

		//+version(10)

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_AI_NAVMESH );
		HK_DECLARE_REFLECTION();

		friend class hkaiNavVolumeGenerationUtils;
		friend class hkaiNavVolumeUtils;
		
		typedef hkInt32 CellIndex;
		typedef hkInt32 EdgeIndex;
		typedef hkInt32 CellData;

			/// Constant values
		enum Constants
		{
 			INVALID_CELL_INDEX	= -1,
			INVALID_EDGE_INDEX	= -1
		};

			/// Flags
		enum CellEdgeFlagBits
		{
			EDGE_EXTERNAL_OPPOSITE = 64, // same as hkaiNavMesh and hkaiDirectedGraphExplicitCost
		};

			/// Flags typedef
		typedef hkFlags<CellEdgeFlagBits, hkUint8> EdgeFlags;

		/// A cell represents axis-aligned box of navigable space
		struct Cell
		{
			//+version(0)
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_AI_NAVMESH, Cell );
			HK_DECLARE_REFLECTION();

			// Constructor
			Cell()
			{
				m_min[0] = m_min[1] = m_min[2] = 0;
				m_max[0] = m_max[1] = m_max[2] = 0;
				m_numEdges = 0;
				m_startEdgeIndex = 0;
				m_data = 0;
			}

			
			/// Coordinates of AABB minimum in the space of the nav volume
			hkUint16 m_min[3];
			/// The number of edges (adjacent cells)
			hkInt16 m_numEdges;
			/// Coordinates of AABB maximum in the space of the nav volume
			hkUint16 m_max[3];
			
			/// The start edge index for this cell
			int m_startEdgeIndex;
			/// User data associated with this cell
			CellData m_data;
		};

		/// An 'edge' (connecting to an adjacent cell)
		struct Edge
		{
			//+version(1)
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_AI_NAVMESH, Edge );
			HK_DECLARE_REFLECTION();
			
			HK_DECLARE_POD_TYPE();

			/// Initialize to invalid opposite cell
			inline void init();
			/// Get opposite cell key (index + section ID)
			inline hkaiPackedKey getOppositeCellKeyUnchecked() const;
			/// Get opposite cell index
			inline CellIndex getOppositeCellIndex() const;
			
			/// Returns true unless edge points to a cell in an unloaded section
			HK_FORCE_INLINE hkBool32 isTraversable() const;
			HK_FORCE_INLINE hkBool32 isExternal() const;

			HK_FORCE_INLINE void setTargetKey( hkaiPackedKey k);

			/// Flags determine if opposite cell is loaded
			EdgeFlags m_flags;
			/// Opposite cell
			hkaiPackedKey m_oppositeCell;
		};

		/// Constructor : Sets as empty volume
		inline hkaiNavVolume();

		hkaiNavVolume( hkFinishLoadedObjectFlag f );

		//
		// Accessor methods
		//
		HK_FORCE_INLINE const Edge& getEdge( int idx ) const;
		HK_FORCE_INLINE	Edge& getEdge( int idx );
		HK_FORCE_INLINE	int getNumEdges() const;

		HK_FORCE_INLINE	void getOffsetAndScale(hkVector4& offsetOut, hkVector4& scaleOut ) const;

		HK_FORCE_INLINE int getNumCells() const;
		HK_FORCE_INLINE const Cell& getCell(CellIndex cellIndex) const;
		HK_FORCE_INLINE       Cell& getCell(CellIndex cellIndex);

		HK_FORCE_INLINE const hkAabb& getAabb() const;

		/// Convert from nav volume space (quantized integers) to world space
		void convertPointToWorld(hkVector4& pointInOut) const;

		/// Clear all internal arrays
		void reset();

			/// Swap the internal arrays, along with all other members
		void swap ( hkaiNavVolume& other );

			/// Coppy all information from the other volume.
		void copy ( const hkaiNavVolume& other );

		inline const hkVector4& getQuantizationScale() const;
		inline const hkVector4& getQuantizationOffset() const;

		/// Set the current AABB ( can use to reposition a nav volume)
		void setAabb( const hkAabb& aabb, bool updateQuantization = true );
		/// Set the current resolution - internal use only
		void setResolution(int x, int y, int z, bool updateQuantization = true);
		const int* getResolution() const;

	protected:
		void _updateQuantizationValues();

	public:
		
		/// Nav volume cells
		hkArray<Cell>	m_cells;
		/// Nav volume edges
		hkArray<Edge>	m_edges;
		/// Streaming information
		hkArray<hkaiStreamingSet> m_streamingSets;
	protected:
		/// Bounding AABB
		hkAabb m_aabb;
		/// Cached scale (computed from m_aabb and m_res)
		hkVector4 m_quantizationScale;
		/// Cached offset (computed from m_aabb and m_res)
		hkVector4 m_quantizationOffset;
		/// Quantization resolution
		int m_res[3];

			/// User-defined data - Havok never touches this.
		hkUlong						m_userData; //+default(0)

	private:
		hkaiNavVolume( const hkaiNavVolume& other);
		void operator=(const hkaiNavVolume& other);
};

#include <Ai/Pathfinding/NavVolume/hkaiNavVolume.inl>

#endif // HKAI_NAV_VOLUME_H

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
