/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKAI_PATHREQUEST_VIEWER_H
#define HKAI_PATHREQUEST_VIEWER_H

#include <Common/Visualize/hkProcess.h>
#include <Ai/Visualize/VisualDebugger/Viewer/hkaiViewerBase.h>
#include <Ai/Pathfinding/World/hkaiWorld.h>

class hkProcessContext;

	/// A viewer that serializes path requests to the Visual Debugger.
class hkaiPathRequestViewer :	public hkaiViewerBase,
								public hkaiWorld::Listener
{
	public:
	HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE);
			/// Registers hkaiPathRequestViewer with the hkViewerFactory.
		static void HK_CALL registerViewer();

			/// Returns the viewer name. This text appears in the VDB client viewer menu.
		static inline const char* HK_CALL getName() { return "AI - Path Requests"; }

			/// Creates a hkaiPathRequestViewer.
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

			/// Raised for each nav mesh path request that is processed
		virtual void pathRequestProcessedCallback( const hkaiNavMeshPathRequestInfo& request ) HK_OVERRIDE;

			/// Raised for each nav volume path request that is processed
		virtual void pathRequestProcessedCallback( const hkaiNavVolumePathRequestInfo& request ) HK_OVERRIDE;

		//
		// hkaiViewerContextWorldListener interface
		//
		virtual void worldAddedCallback( hkaiWorld* newWorld ) HK_OVERRIDE;
		virtual void worldRemovedCallback( hkaiWorld* oldWorld ) HK_OVERRIDE;


	protected:

		hkaiPathRequestViewer(const hkArray<hkProcessContext*>& contexts);
		virtual ~hkaiPathRequestViewer();

	protected:

		static int m_tag;
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
