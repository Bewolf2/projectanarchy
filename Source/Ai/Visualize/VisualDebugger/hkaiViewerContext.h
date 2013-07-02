/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKAI_VIEWER_CONTEXT_H
#define HKAI_VIEWER_CONTEXT_H

#include <Common/Visualize/hkProcessContext.h>
#include <Ai/Pathfinding/World/hkaiWorld.h>
#include <Ai/SpatialAnalysis/Traversal/hkaiTraversalAnnotationLibrary.h>

/// A simple interface that viewers that want to know
/// when hkWorlds are added and removed from the physics
/// context can implement.
class hkaiViewerContextWorldListener
{
public:
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_VDB,hkaiViewerContextWorldListener);
	virtual ~hkaiViewerContextWorldListener() { }
	virtual void worldAddedCallback( hkaiWorld* newWorld ) = 0;
	virtual void worldRemovedCallback( hkaiWorld* oldWorld ) = 0;
	virtual void traversalAnnotationLibraryAddedCallback(hkaiTraversalAnnotationLibrary const* newLibrary) { }
	virtual void traversalAnnotationLibraryRemovedCallback(hkaiTraversalAnnotationLibrary const* oldLibrary) { }

};

/// This is the context that stores a list of hkaiWorld pointers for use by (Visual Debugger) AI viewers.
class hkaiViewerContext :	public hkReferencedObject, 
							public hkProcessContext,
							public hkaiWorld::Listener
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_VDB);

		hkaiViewerContext();
		virtual ~hkaiViewerContext();

			/// Registers all AI viewers. This must be called for the AI viewers to appear in the VDB client.
		static void HK_CALL registerAllAiProcesses();

			/// Returns a string to identify this type of context.
		virtual const char* getType() HK_OVERRIDE { return "ai"; }

		//
		// AI worlds
		//
			/// Registers a world with the context.
		void addWorld(hkaiWorld* world);

			/// Removes a world from the context.
		void removeWorld(hkaiWorld* world);

			/// Returns the worlds registered with the context.
		const hkArray<hkaiWorld*>& getWorlds() const { return m_worlds; }

		int getNumWorlds() const { return m_worlds.getSize(); }

		hkaiWorld* getWorld(int i) { return m_worlds[i]; }
		const hkaiWorld* getWorld(int i) const { return m_worlds[i]; }

		//
		// Display offset
		//

			/// Sets a displacement vector that viewers may use when drawing lines, triangles, etc.
		void setDisplayOffset( hkVector4Parameter displayOffset) { m_displayOffset = displayOffset; }

			/// Returns the displacement vector used when drawing.
		const hkVector4& getDisplayOffset() const { return m_displayOffset; }

		void addTraversalAnnotationLibrary(hkaiTraversalAnnotationLibrary const* library);

		void removeTraversalAnnotationLibrary(hkaiTraversalAnnotationLibrary const* library);

		int getNumTraversalAnnotationLibraries() const { return m_traversalAnnotationLibraries.getSize(); }

		hkaiTraversalAnnotationLibrary const* getTraversalAnnotationLibrary(int libraryIdx) const
		{
			return m_traversalAnnotationLibraries[libraryIdx];
		}

		//
		// Viewer registration
		//
			/// Add a viewer that implements the hkaiViewerContextWorldListener interface
		void addWorldAddedListener( hkaiViewerContextWorldListener* cb );

			/// Remove a viewer that implements the hkaiViewerContextWorldListener interface
		void removeWorldAddedListener( hkaiViewerContextWorldListener* cb );

		//
		// hkaiWorld::Listener interface
		//
		void worldDeletedCallback( hkaiWorld* world );

	protected:

		hkArray<hkaiWorld*> m_worlds;

		hkArray<hkRefPtr<const hkaiTraversalAnnotationLibrary> > m_traversalAnnotationLibraries;

		hkArray<hkaiViewerContextWorldListener*> m_listeners;

		hkVector4 m_displayOffset;
};

#endif  // HKAI_VIEWER_CONTEXT_H

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
