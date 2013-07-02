/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKAI_TRAVERSAL_ANNOTATION_VIEWER_H
#define HKAI_TRAVERSAL_ANNOTATION_VIEWER_H

#include <Common/Visualize/hkProcess.h>
#include <Ai/Visualize/VisualDebugger/Viewer/hkaiViewerBase.h>
#include <Common/Visualize/Shape/hkDisplayAABB.h>

class hkProcessContext;

/// Viewer to display traversal annotations.
class hkaiTraversalAnnotationViewer : public hkaiViewerBase, public hkaiWorld::Listener
{
public:
	HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_VDB);

	static void HK_CALL registerViewer();

	/// Returns the viewer name. This text appears in the VDB client viewer menu.
	static inline const char* HK_CALL getName() { return "AI - Traversal Annotations"; }

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
	virtual void postStepCallback(class hkaiWorld* world, const hkArrayBase<class hkaiBehavior*>& behaviors) HK_OVERRIDE;


	//
	// hkaiViewerContextWorldListener interface
	//
	virtual void worldAddedCallback( hkaiWorld* newWorld ) HK_OVERRIDE;
	virtual void worldRemovedCallback( hkaiWorld* oldWorld ) HK_OVERRIDE;
	virtual void traversalAnnotationLibraryAddedCallback( hkaiTraversalAnnotationLibrary const* library ) HK_OVERRIDE;
	virtual void traversalAnnotationLibraryRemovedCallback( hkaiTraversalAnnotationLibrary const* library ) HK_OVERRIDE;

protected:

	hkaiTraversalAnnotationViewer(const hkArray<hkProcessContext*>& contexts);
	virtual ~hkaiTraversalAnnotationViewer();
protected:

	static int m_tag;

private:
	void addLibraryGeometry(hkaiTraversalAnnotationLibrary const* library);
	void removeLibraryGeometry(hkaiTraversalAnnotationLibrary const* library);

	void addAnnotationGeometry(hkaiTraversalAnnotationLibrary const* library, hkaiTraversalAnnotationLibrary::Index index) const;
	void addJumpGeometry(hkaiTraversalAnnotationLibrary const* library, hkaiTraversalAnnotationLibrary::Index index) const;
	void addClimbUpGeometry(hkaiTraversalAnnotationLibrary const* library, hkaiTraversalAnnotationLibrary::Index index) const;
	void addVaultGeometry(hkaiTraversalAnnotationLibrary const* library, hkaiTraversalAnnotationLibrary::Index index) const;
	void addDuckUnderGeometry(hkaiTraversalAnnotationLibrary const* library, hkaiTraversalAnnotationLibrary::Index index) const;
	void addDropDownGeometry(hkaiTraversalAnnotationLibrary const* library, hkaiTraversalAnnotationLibrary::Index index) const;
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
