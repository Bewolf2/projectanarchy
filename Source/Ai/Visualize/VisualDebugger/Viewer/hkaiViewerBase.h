/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKAI_VIEWER_BASE_H
#define HKAI_VIEWER_BASE_H

#include <Common/Visualize/hkProcess.h>
#include <Ai/Visualize/VisualDebugger/hkaiViewerContext.h>

class hkProcessContext;

	/// AI Viewer base class. Takes care of registering itself with the hkaiViewerContext.
class hkaiViewerBase :	public hkReferencedObject, 
						public hkProcess,
						protected hkaiViewerContextWorldListener
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_VDB);

		// Ctor will register as all the listeners
		hkaiViewerBase( const hkArray<hkProcessContext*>& contexts );

		// Dtor will unregister as all the listeners, from the context if any
		virtual ~hkaiViewerBase();

		//
		// hkaiViewerContextWorldListener interface
		//
		virtual void worldAddedCallback( hkaiWorld* newWorld ) HK_OVERRIDE {}
		virtual void worldRemovedCallback( hkaiWorld* oldWorld ) HK_OVERRIDE {}

	protected:

		hkRefPtr<hkaiViewerContext> m_context;

};

#endif  // HKAI_VIEWER_BASE_H

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
