/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef HKAI_NAV_VOLUME_INSTANCE_H
#define HKAI_NAV_VOLUME_INSTANCE_H

#include <Common/Base/Types/Geometry/Aabb/hkAabb.h>
#include <Ai/Pathfinding/NavVolume/hkaiNavVolume.h>

extern const class hkClass hkaiNavVolumeInstanceClass;

	/// A runtime instance of an hkaiNavVolume
class hkaiNavVolumeInstance : public hkReferencedObject
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_AI_NAVMESH );
		HK_DECLARE_REFLECTION();

		friend class hkaiNavVolumeUtils;
		HK_ON_SPU(friend class hkaiSpuNavVolumeAccessor);

		typedef hkInt32 CellIndex;
		typedef hkInt32 EdgeIndex;
		typedef hkInt32 CellData;

			/// Additional owned edges for a cell
		struct CellInstance
		{
			HK_DECLARE_REFLECTION();

				/// The start edge index for this cell
			int m_startEdgeIndex;

				/// The number of owned edges for the cell	
			int m_numEdges;
		};

		/// Constructor : Sets as empty volume
		hkaiNavVolumeInstance();
		hkaiNavVolumeInstance( hkFinishLoadedObjectFlag f );

		void init(const hkaiNavVolume* vol, hkaiSectionUid uid);
		void tempInit( const hkaiNavVolume* volume);
		HK_FORCE_INLINE const hkaiNavVolume* getOriginalVolume() const { return m_originalVolume; }


		//
		// Accessor methods
		//
		HK_FORCE_INLINE	int getNumOriginalEdges() const  { return m_numOriginalEdges; }
		HK_FORCE_INLINE	int getNumOwnedEdges() const  { return m_ownedEdges.getSize(); }
		HK_FORCE_INLINE	int getNumEdges() const  { return getNumOriginalEdges() + getNumOwnedEdges(); }
		HK_FORCE_INLINE	const hkaiNavVolume::Edge& getEdge( EdgeIndex e ) const;

		HK_FORCE_INLINE const hkaiNavVolume::Cell& getCell(CellIndex cellIndex) const;
		HK_FORCE_INLINE void getInstancedCell( CellIndex n, CellInstance& cellOut ) const;
		HK_FORCE_INLINE int getNumCells() const { return m_numOriginalCells; }

		
#ifndef HK_PLATFORM_SPU
		HK_FORCE_INLINE hkaiPackedKey getOppositeCellKeyForEdge( const hkaiNavVolume::Edge& edge ) const;
#endif

		HK_FORCE_INLINE	void getOffsetAndScale(hkVector4& offsetOut, hkVector4& scaleOut ) const;
		HK_FORCE_INLINE void getAabb( hkAabb& aabbOut ) const;

		HK_FORCE_INLINE void setTranslation( hkVector4Parameter t ) { m_translation = t; }
		HK_FORCE_INLINE const hkVector4& getTranslation() const { return m_translation; }

		//
		// Utility functions for adding cells/edges
		//
		HK_FORCE_INLINE CellInstance& getWritableCellInstance( CellIndex n );
		hkaiNavVolume::Edge* addEdgeForCell( CellIndex n );
		void removeOwnedEdgeForCell( CellIndex n, EdgeIndex e );
		HK_FORCE_INLINE hkaiNavVolume::Edge& getWritableEdge( EdgeIndex e );
		hkaiNavVolume::Edge* expandEdgesBy( int n );
		

		//
		// Streaming information
		//

#ifndef HK_PLATFORM_SPU
		/// Get the unique section ID for this volume.
		HK_FORCE_INLINE hkaiSectionUid getSectionUid() const { return m_sectionUid; }

		/// Get the runtime ID for this volume.
		HK_FORCE_INLINE hkaiRuntimeIndex getRuntimeId() const  { return m_runtimeId;}
#endif

		/// Assign the unique section ID for this volume.
		void setSectionUid( hkaiSectionUid uid );

private:
		inline void setOriginalPointers( const hkaiNavVolume* volume);

			/// Assign the runtime ID for this volume, when the volume is loaded.
		void setRuntimeId( hkaiRuntimeIndex id );

		friend class hkaiStreamingManager;
		friend class hkaiStreamingCollection;
	
protected:

	//
	// Pointers to the original volume data, and their respective sizes.
	//

		const hkaiNavVolume::Cell*		m_originalCells;		//+nosave
		int								m_numOriginalCells;		//+nosave

		const hkaiNavVolume::Edge*		m_originalEdges;		//+nosave
		int								m_numOriginalEdges;		//+nosave

		hkRefPtr<const hkaiNavVolume>	m_originalVolume;

		hkArray<int>					m_cellMap;

			/// Instanced cells. These contain information about additional edges from an original cell, added due to streaming.
		hkArray<CellInstance>			m_instancedCells;
		
			/// Owned edges, referenced by instanced cells.
		hkArray<hkaiNavVolume::Edge>	m_ownedEdges;

		/// Unique section ID of this volume.
		hkaiSectionUid					m_sectionUid; //+default(0)

		/// Runtime ID of this volume.
		/// Only assigned when the volume is loaded.
		hkaiRuntimeIndex				m_runtimeId; //+default(0)

			/// Instance-specific translation
		hkVector4						m_translation;
};

#ifndef HK_PLATFORM_SPU
	typedef hkaiNavVolumeInstance hkaiNavVolumeAccessor;
#else
	typedef class hkaiSpuNavVolumeAccessor hkaiNavVolumeAccessor;
#endif

#include <Ai/Pathfinding/NavVolume/hkaiNavVolumeInstance.inl>

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
