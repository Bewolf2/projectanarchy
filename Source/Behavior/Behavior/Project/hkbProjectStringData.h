/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_PROJECT_STRING_DATA_H
#define HKB_PROJECT_STRING_DATA_H


extern const class hkClass hkbProjectStringDataClass;

/// Constant string data associated with a behavior project.
class hkbProjectStringData : public hkReferencedObject
{
	//+vtable(1)
	//+version(2)
	public:

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

		hkbProjectStringData() {}

			/// A list of referenced animation filenames.
			/// This is not necessarily equivalent to the files that were exported.
			/// The list of exported files is dependent on several settings in HAT
			/// such as "Only Export Referenced Animations" and "Bundle Referenced Animations"
		hkArray<hkStringPtr> m_animationFilenames;

			/// A list of referenced behavior filenames.
		hkArray<hkStringPtr> m_behaviorFilenames;

			/// A list of referenced character filenames.
		hkArray<hkStringPtr> m_characterFilenames;

			/// The names of the events in the project.  These will not be present if your 
			/// project has events defined per-behavior.
		hkArray<hkStringPtr> m_eventNames;

			/// The path to the folder containing animations.  This can be either absolute or relative to the project folder.
		hkStringPtr m_animationPath;

			/// The path to the folder containing behaviors.  This can be either absolute or relative to the project folder.
		hkStringPtr m_behaviorPath;

			/// The path to the folder containing characters.  This can be either absolute or relative to the project folder.
		hkStringPtr m_characterPath;

			/// The path to the folder containing scripts.  This can be either absolute or relative to the project folder.
		hkStringPtr m_scriptsPath;

			/// The full path to the source HKP file.
		hkStringPtr m_fullPathToSource;

			/// The path from which this project data was loaded from.  This is not saved.  It is set by the hkbProjectAssetManager and used
			/// when loading assets.
		hkStringPtr m_rootPath;	//+nosave

	public:

		hkbProjectStringData( hkFinishLoadedObjectFlag flag )
			:	hkReferencedObject(flag),
				m_animationFilenames(flag),
				m_behaviorFilenames(flag),
				m_characterFilenames(flag),
				m_eventNames(flag),
				m_animationPath(flag),
				m_behaviorPath(flag),
				m_characterPath(flag),
				m_scriptsPath(flag),
				m_fullPathToSource(flag)
		{}
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
