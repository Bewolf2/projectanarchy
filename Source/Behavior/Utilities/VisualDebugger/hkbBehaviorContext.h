/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_BEHAVIOR_CONTEXT_H
#define HKB_BEHAVIOR_CONTEXT_H

#include <Common/Visualize/hkProcessContext.h>
#include <Common/Base/Types/Color/hkColor.h>
#include <Behavior/Behavior/World/hkbWorldListener.h>
#include <Behavior/Utilities/Utils/hkbAssetLoader.h>

#define HK_BEHAVIOR_CONTEXT_TYPE_STRING "Behavior"

class hkbWorld;
class hkProcess;
class hkbSkinLoader;
class hkbCharacterDebugData;

	/// Callback interface for hkbBehaviorContext.
class hkbBehaviorContextWorldListener
{
	public:

		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_TOOLS, hkbBehaviorContextWorldListener);
			
			/// Empty destructor
		virtual ~hkbBehaviorContextWorldListener() {}
			
			/// Called when a world is added to the context
		virtual void worldAddedCallback( hkbWorld* world ) {}
			/// Called when a world is removed from the context
		virtual void worldRemovedCallback( hkbWorld* world ) {}

			/// Called when a character is added to the context.
		virtual void characterAddedToContextCallback( hkbCharacter* character ) {}
			/// Called when a character is removed from the context.
		virtual void characterRemovedFromContextCallback( hkbCharacter* character ) {}
};

	/// Context that controls what behavior viewers are registered and how they access hkbWorld information. 
class hkbBehaviorContext: public hkReferencedObject,						
						  public hkbWorldListener,
						  public hkProcessContext						  
{
	public:						

		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_TOOLS);
	
			/// Max number of bones per skin supported by Havok vertex shaders.
		enum { HK_DEFAULT_MAX_BONES_PER_SKIN = 48 };

			/// Initializes a context with an asset loader.  If skinLoader is set to HK_NULL then one will be
			/// created internally.
		hkbBehaviorContext( hkbAssetLoader* assetLoader = HK_NULL, hkbSkinLoader* skinLoader = HK_NULL );

			/// Destroys the context and removes all worlds.
		virtual ~hkbBehaviorContext();

			/// Register all behavior viewers
		static void HK_CALL registerAllBehaviorViewers();
	
			/// Add required behavior viewers to the default process list for the visual debugger server
		static void HK_CALL addDefaultViewers(hkVisualDebugger* visualDebugger);

			/// hkProcessContext interface
		virtual const char* getType() HK_OVERRIDE; 

			/// Adds a world to the context.  The world will be removed automatically when it is deleted or when the
			/// context is deleted.
		void addWorld( hkbWorld* world );
			/// Removes a world from the context.
		void removeWorld( hkbWorld* world );
			/// Gets a list of worlds in the context.
		const hkArray<hkbWorld*>& getWorlds() const;

			/// Gets debug data for a hkbCharacter.  The debug data is created when either the world that contains
			/// the character is added to the context or when the character is added to the world.
		hkbCharacterDebugData* getDebugData( hkbCharacter* character ) const;
			/// Throw away the debug data for a character.  Ideally, this would only be done
			/// internally, but the hkbCharacterViewer needs to discard meshes before the debugData
			/// is thrown out so it needs to call this (after discarding its meshes).
		void discardDebugData( hkbCharacter* character );

			/// Accesses the asset loader associate with the context.
		hkbAssetLoader* getAssetLoader();
			/// Gets the skin loader associated with the context.
		hkbSkinLoader* getSkinLoader() { return m_skinLoader; }
			
			/// hkbWorldListener interface
		virtual void characterAddedCallback( hkbCharacter* character ) HK_OVERRIDE;
			/// hkbWorldListener interface
		virtual void characterRemovedCallback( hkbCharacter* character ) HK_OVERRIDE;
			/// hkbWorldListener interface
		virtual void worldDeletedCallback( hkbWorld* world ) HK_OVERRIDE;

			/// Adds a listener to the context
		void addWorldListener( hkbBehaviorContextWorldListener* listener );
			/// Remove a listener from the context
		void removeWorldListener( hkbBehaviorContextWorldListener* listener );

			/// Finds a character by address by searching all hkbWorlds.
			/// Returns HK_NULL if the character cannot be found.
		hkbCharacter* findCharacter(hkUint64 id);
		
	public:
			//
			// Shared colors that viewers can use to render data.  Use these colors so that
			// behavior viewers look consistent.
			//

		static const hkColor::Argb Purple = 0xFF6649B8;
		static const hkColor::Argb Red = 0xFFB84949;			
		static const hkColor::Argb Blue = 0xFF4982B8;
		static const hkColor::Argb Brown = 0xFFCC6600;
		static const hkColor::Argb Orange = 0xFFB88249;
		static const hkColor::Argb Green = 0xFF49B84B;
		static const hkColor::Argb Black = 0xFF000000;
		static const hkColor::Argb Gray = 0xFF7F7F7F;

	protected:

			/// Worlds in this context 
		hkArray<hkbWorld*> m_worlds;
						
			/// Characters registered for debugging in this context.
		hkPointerMap<hkbCharacter*, hkbCharacterDebugData*> m_characterDebugData;
			
			/// Listeners in this context
		hkArray<hkbBehaviorContextWorldListener*> m_listeners;

			/// Used for loading assets on the heap.
		hkRefPtr<hkbAssetLoader> m_assetLoader;

			/// Loader that stores skins referenced by characters being viewed
		hkRefPtr<hkbSkinLoader> m_skinLoader;		
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
