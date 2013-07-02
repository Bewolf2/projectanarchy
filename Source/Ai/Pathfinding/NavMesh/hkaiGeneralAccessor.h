/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef HKAI_GENERAL_ACCESSOR_H
#define HKAI_GENERAL_ACCESSOR_H

#include <Ai/Pathfinding/NavMesh/hkaiNavMesh.h>
#include <Ai/Pathfinding/NavMesh/Streaming/hkaiStreamingCollection.h>
#ifdef HK_PLATFORM_SPU
#include <Ai/Pathfinding/NavMesh/Spu/hkaiSpuNavMeshAccessor.h>
#endif

	/// A wrapper class that provides a uniform API for a streaming collection across CPU and SPU.
class hkaiGeneralAccessor
{
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI_ASTAR, hkaiGeneralAccessor);
#ifdef HK_PLATFORM_SPU
		typedef hkaiSpuNavMeshAccessor Accessor;
		typedef Accessor* AccessorPointer;
		typedef const Accessor* ConstAccessorPointer;
		typedef const Accessor& AccessorReference;
#else
		typedef const hkaiNavMeshInstance* Accessor;
		typedef Accessor AccessorPointer;
		typedef Accessor ConstAccessorPointer;
		typedef const hkaiNavMeshInstance& AccessorReference;
#endif

	public:
		hkaiGeneralAccessor(const hkaiStreamingCollection::InstanceInfo* sectionInfo)
			: m_currentSection(HKAI_INVALID_RUNTIME_INDEX)
			, m_sectionInfo(sectionInfo)
		{
			HK_ON_CPU(m_accessor = HK_NULL);
		}

		HK_FORCE_INLINE AccessorPointer operator->()
		{
			return getAccessor();
		}

		HK_FORCE_INLINE ConstAccessorPointer operator->() const
		{
			return getAccessor();
		}

		HK_FORCE_INLINE AccessorReference operator*()
		{
			return *getAccessor();
		}

		HK_FORCE_INLINE AccessorPointer getAccessor()
		{
#ifdef HK_PLATFORM_SPU
			return &m_accessor;
#else
			return m_accessor;
#endif
		}

		HK_FORCE_INLINE ConstAccessorPointer getAccessor() const
		{
#ifdef HK_PLATFORM_SPU
			return &m_accessor;
#else
			return m_accessor;
#endif
		}

		HK_FORCE_INLINE void setSectionOf(hkaiPackedKey key) const
		{
			hkaiRuntimeIndex section = hkaiGetRuntimeIdFromPacked(key);
			setSection(section);
		}

		HK_FORCE_INLINE hkaiRuntimeIndex getSection() const
		{
			return m_currentSection;
		}

		void setSection(hkaiRuntimeIndex section) const;

		HK_FORCE_INLINE void setStreamingInfo(const hkaiStreamingCollection::InstanceInfo* info)
		{
			m_sectionInfo = info;
		}

		const hkaiNavMesh::Edge& getEdgeFromPacked(hkaiPackedKey edgeKey) const;
		const hkaiNavMesh::Edge& getEdgeAndDataPtrFromPacked(hkaiPackedKey edgeKey, const hkaiNavMesh::EdgeData*& edgeDataOut) const;
		const hkaiNavMesh::Edge& getEdgeAndVertsFromPacked(hkaiPackedKey edgeKey, hkVector4& edgeAOut, hkVector4& edgeBOut, const hkaiNavMesh::EdgeData*& edgeDataOut) const;
		const hkaiNavMesh::Face& getFaceFromPacked(hkaiPackedKey faceKey) const;

	private:

		Accessor m_accessor;
		mutable HK_PAD_ON_SPU(hkaiRuntimeIndex) m_currentSection;
		HK_PAD_ON_SPU(const hkaiStreamingCollection::InstanceInfo*) m_sectionInfo;
};


#endif //HKAI_GENERAL_ACCESSOR_H

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
