/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_SYMBOL_ID_MAP_H
#define HKB_SYMBOL_ID_MAP_H

/// A map of symbol IDs.  
/// 
/// Some objects like hkbBehaviorGraph and hkbSequence can contain their own indexed lists 
/// of symbols such as event names, variable names, etc.  An hkbSymbolIdMap maintains
/// a map from the local IDs of such an object, and the IDs in a global list or in 
/// another object.
class hkbSymbolIdMap : public hkReferencedObject
{
	public:
	
		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BEHAVIOR);

			/// Map an internal ID to an external ID.
		int getExternal( int internal ) const;

			/// Map an external ID to an internal ID.
		int getInternal( int external ) const;

			/// The map from internal IDs to external IDs.
			/// 
			/// The internal symbols are typically indexed sequentially starting
			/// at 0, so we use a dense array to store the map.
		hkArray<int> m_internalToExternalMap;

			/// The map from external IDs to internal UDs.
			/// 
			/// Typically, only a subset of the external symbol IDs are covered
			/// by the internal IDs, so we use a sparse map representation.
		hkPointerMap<int,int> m_externalToInternalMap;
};

#include <Behavior/Behavior/Linker/hkbSymbolIdMap.inl>

#endif

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
