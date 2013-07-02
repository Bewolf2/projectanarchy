/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_WORLD_VIEWER_BASE_H
#define HKB_WORLD_VIEWER_BASE_H

#include <Common/Visualize/hkProcess.h>
#include <Behavior/Behavior/World/hkbWorldListener.h>
#include <Behavior/Utilities/VisualDebugger/hkbBehaviorContext.h>
#include <Common/Visualize/Process/hkRemoteObjectProcess.h>

class hkLocalFrame;
class hkbCharacterControlCommand;

	/// Base class of all behavior world viewers.  
	/// 
	/// Contains the following common functionality:
	/// 
	///    (1) - Connects the viewer to the hkRemoteObjectProcess so that remote objects can
	///          be received from the server and processed.  Override receiveObjectCallback to
	///          process these objects.
	///
	///    (2) - Character visibility.  The visual debugger protocol does not support viewing a
	///          single object at a time.  It instead operates on a class of objects (e.g. all rigid 
	///          bodies or all constraints).  If a viewer wants to support per-object visibility 
	///          it must implement the functionality itself.   The hkbWorldViewerBase implements
	///          per-character visibility for behavior.
	///
	///          The implementation works with the following rules:
	///
    ///             (1) The client is responsible for telling the server which characters are 
	///                 visible.
	///             (2) Viewer instances store which characters are visible when the server
	///                 tells them.
	///             (3) If a viewer needs to show / hide information it should override the
    ///                 showCharacter and hideCharacter methods.
	///             (4) If a viewer is a local viewer (e.g. in the demo framework) all characters
	///                 are visible.
    ///
	///          Note to those implementing a client:  If implementing a client, like HAT, adhere to
	///          rule 1.  This is especially true while recording movies.  Since viewers 
	///          are created and destroyed in the process you will need to make sure that the character
    ///          visibility state is persistent across disconnect / reconnect process.
class hkbWorldViewerBase: public hkReferencedObject,
						  public hkProcess,
						  protected hkbBehaviorContextWorldListener,
						  public hkbWorldListener,
						  public hkRemoteObjectServerSideListener
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_TOOLS);

			/// Initializes the viewer
		hkbWorldViewerBase( const hkArray<hkProcessContext*>& contexts );
			/// Destroys the viewer
		virtual ~hkbWorldViewerBase();
			
			/// Returns the type of the viewer as a behavior viewer type
		virtual ProcessType getType() const HK_OVERRIDE { return hkProcess::HK_PROCESS_TYPE_BEHAVIOR; }

			/// Connects the viewer to world.  Also raises hkbWorldListener::characterAddedCallback
			/// for each character in the world.
		virtual void worldAddedCallback( hkbWorld* world ) HK_OVERRIDE;
			/// Disconnects the viewer from world.  Also raises hkbWorldListener::characterRemovedCallback
			/// for each character in the world.
		virtual void worldRemovedCallback( hkbWorld* world ) HK_OVERRIDE;		

			/// hkbBehaviorContextWorldListener interface.
		virtual void characterAddedToContextCallback( hkbCharacter* character ) HK_OVERRIDE;
			/// hkbBehaviorContextWorldListener interface.
		virtual void characterRemovedFromContextCallback( hkbCharacter* character ) HK_OVERRIDE;			

			/// Raised after the viewer has been initialized.  Also calls worldAddedCallback for each
			/// world in the context.
		virtual void init() HK_OVERRIDE;

			/// hkRemoteObjectServerSideListener.  This callback is raised when the server receives
			/// and object from a client.  Objects are typically commands and are dispatched to one
			/// of the "handle" methods below.
		virtual void receiveObjectCallback( hkReferencedObject* object, const hkClass* klass ) HK_OVERRIDE;
		
			/// Called when a character should be shown.
		virtual void showCharacter( hkbCharacter* character ) {}
			/// Called when a character should be hidden.
		virtual void hideCharacter( hkbCharacter* character ) {}
			
			/// Returns true if a character is visible, false otherwise.  If the viewer is a local viewer
			/// true is always returned.
		virtual bool isVisible( hkbCharacter* character );

	protected:

			/// Processes the character control command.
		void handleCharacterControlCommand( hkbCharacterControlCommand& command );			
			
	protected:

			/// Context which owns the viewer.
		hkbBehaviorContext* m_context;
			/// Which characters are visible to the viewer
		hkPointerMap<hkbCharacter*, int> m_isVisible;			
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
