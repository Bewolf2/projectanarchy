/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_PROJECT_DATA_H
#define HKB_PROJECT_DATA_H

#include <Behavior/Behavior/TransitionEffect/hkbTransitionEffect.h>
#include <Behavior/Behavior/Project/hkbProjectStringData.h>

extern const class hkClass hkbProjectDataClass;

/// Constant data associated with a behavior project.
class hkbProjectData : public hkReferencedObject
{
	//+vtable(1)
	//+version(2)
	public:

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

		hkbProjectData();

			/// Get the string data for the project.
		const hkbProjectStringData* getStringData() const { return m_stringData; }

			/// Set the string data and update the reference counts on the old and new string data.
		void setStringData( hkbProjectStringData* stringData );

	public:

			/// The world up vector.
		hkVector4 m_worldUpWS;

			/// String data such as names and filenames associated with the project.
		hkRefPtr<hkbProjectStringData> m_stringData;

			/// This is used to initialize hkbTransitionEffect::m_defaultEventMode.
		hkEnum< hkbTransitionEffect::EventMode, hkInt8 > m_defaultEventMode;

	public:

		hkbProjectData( hkFinishLoadedObjectFlag flag );
};

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
