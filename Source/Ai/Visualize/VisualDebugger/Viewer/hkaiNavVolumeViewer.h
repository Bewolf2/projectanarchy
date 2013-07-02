/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKAI_NAV_VOLUME_VIEWER_H
#define HKAI_NAV_VOLUME_VIEWER_H

#include <Common/Visualize/hkProcess.h>
#include <Ai/Pathfinding/World/hkaiWorld.h>
#include <Ai/Visualize/VisualDebugger/Viewer/hkaiViewerBase.h>

class hkProcessContext;
class hkaiViewerContext;
class hkaiNavVolume;

/// Nav volume geomety viewer
class hkaiNavVolumeViewer : 	public hkaiViewerBase,
								public hkaiWorld::Listener
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_VDB);

		/// Registers hkaiNavVolumeViewer with the hkViewerFactory.
		static void HK_CALL registerViewer();

		/// Returns the viewer name. This text appears in the VDB client viewer menu.
		static inline const char* HK_CALL getName() { return "AI - Nav Volume Geometry"; }

		/// Creates a hkaiNavVolumeViewer.
		static hkProcess* HK_CALL create(const hkArray<hkProcessContext*>& contexts);

		//
		// hkProcess interface
		//
		void init();

			/// Returns the tag associated with this viewer type.
			/// This is passed to the drawing calls so that the VDB knows from which viewer those calls came.
		virtual int getProcessTag() HK_OVERRIDE { return m_tag; }

		//
		// hkaiWorld::Listener interface
		//

			/// Update function that updates the volume geometries.
		virtual void postStepCallback(class hkaiWorld* world, const hkArrayBase<class hkaiBehavior*>& behaviors) HK_OVERRIDE;
		virtual void navVolumeAdded( const class hkaiWorld* world, const class hkaiNavVolumeInstance* volume, const class hkaiNavVolumeMediator* mediator ) HK_OVERRIDE;
		virtual void navVolumeRemoved( const class hkaiWorld* world, const class hkaiNavVolumeInstance* volume ) HK_OVERRIDE;

		//
		// hkaiViewerContextWorldListener interface
		//
		virtual void worldAddedCallback( hkaiWorld* newWorld ) HK_OVERRIDE;
		virtual void worldRemovedCallback( hkaiWorld* oldWorld ) HK_OVERRIDE;


protected:

	hkaiNavVolumeViewer(const hkArray<hkProcessContext*>& contexts);
	virtual ~hkaiNavVolumeViewer();

	/// Builds and adds a display geometry for the provided nav volume.
	void addNavVolume(const hkaiNavVolumeInstance* navVol, const hkaiWorld* world);

	/// Removes the display geometry associated with a particular nav volume.
	void removeNavVolume(const hkaiNavVolumeInstance* navVol);

protected:

	/// Internal bookkeeping information
	struct NavVolumeInfo
	{
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE,hkaiNavVolumeViewer::NavVolumeInfo);
		const hkaiNavVolumeInstance*	m_navVolume;
		hkUlong							m_id;
		const hkaiWorld*				m_world;
	};

	/// Information pertaining to nav volumes for which display geometries have been added.
	hkArray<NavVolumeInfo> m_navVolumeInfos;

	// Counter used to generate unique ids for each added display geometry. This is required because
	// removing a geometry and then adding a new geometry with the same id doesn't seem to work.
	hkUint32 m_counter;

	static int m_tag;

};


#endif  // HKAI_NAV_VOLUME_VIEWER_H

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
