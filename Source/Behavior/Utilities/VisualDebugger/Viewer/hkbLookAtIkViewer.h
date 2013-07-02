/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_LOOK_AT_IK_VIEWER_H
#define HKB_LOOK_AT_IK_VIEWER_H

#include <Behavior/Utilities/VisualDebugger/Viewer/hkbWorldViewerBase.h>
#include <Common/Visualize/Shape/hkDisplayCone.h>

	/// A viewer that displays information about LookAtModifiers
class hkbLookAtIkViewer: public hkbWorldViewerBase
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_TOOLS );

			/// Initializes the viewer
		hkbLookAtIkViewer( const hkArray<hkProcessContext*>& contexts );
		
		~hkbLookAtIkViewer();

			/// Draws LookAtIK-related debug information after the character's pose is generated
		virtual void postGenerateCallback( hkbCharacter* character, hkReal deltaTime ) HK_OVERRIDE;			
	
			/// Create an hkbLookAtIkViewer process
		static hkProcess* HK_CALL create( const hkArray<hkProcessContext*>& contexts );

			/// Registers the hkbLookAtIkViewer with the hkProcessFactory.
		static void HK_CALL registerViewer();

			/// Gets the tag associated with this viewer type
		virtual int getProcessTag() HK_OVERRIDE { return m_tag; }

			/// The name of the viewer shown in the VDB
		static inline const char* HK_CALL getName() { return "Behavior Look At IK"; }
			
	protected:			

			/// Unique ID for the viewer
		static int m_tag;

			/// Cone for display
		hkDisplayCone m_displayCone;
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
