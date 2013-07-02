/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_WORLD_CINFO_H
#define HKB_WORLD_CINFO_H

#include <Behavior/Utilities/Physics/hkbPhysicsBaseTypes.h>

	/// Construction information used to initialize an hkbWorld
class hkbWorldCinfo
{
	public:

		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BEHAVIOR, hkbWorldCinfo);

			/// Initializes the cinfo to reasonable defaults
		hkbWorldCinfo()
			:m_up(0.0f, 1.0f, 0.0f),
			 m_physicsInterface(HK_NULL),
			 m_useDefaultSceneModifiers(true),
			 m_scriptDebuggerPort(27726)
		{
#if HK_CONFIG_THREAD==HK_CONFIG_SINGLE_THREADED || defined(HK_PLATFORM_PS3_PPU)
			m_enableMultithreadedUpdate = false;
#else
			m_enableMultithreadedUpdate = true;
#endif
			// Assigning "this" pointer is fine as we don't expect this to be a derived class
			// and we don't use virtual functions anyway.
			m_physicsWorld.m_cinfo = this;
			m_physicsWorld.m_assigned = false;
		}

			/// Dtor. Cleans up created Physics Interface (if applicable).
		~hkbWorldCinfo();
	
			/// World space up vector
		hkVector4 m_up;

			/// Provided for easy upgrading, may be removed in a future release.
			/// Need to link against hkbPhysics2012Bridge or hkbPhysicsBridge.
		struct DefaultPhysicsInterfaceCreator
		{
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BEHAVIOR, DefaultPhysicsInterfaceCreator);
			void operator=(hkbDeprecatedPhysicsWorldPtr world);
			hkbWorldCinfo* m_cinfo;
			bool m_assigned;
		} m_physicsWorld;
		
			/// Optional physical interface
		hkbPhysicsInterface* m_physicsInterface;

			/// Whether to set up the default scene modifiers (otherwise, no scene modifiers are set up).
			/// The default scene modifiers are (in order): 
			///    hkbAttachmentSceneModifier,
			///    hkbCharacterControllerSceneModifier,
			///    hkbFootIkSceneModifier,
			///    hkbHandIkSceneModifier,
			///    hkbRagdollSceneModifier,
			///    hkbAttachmentFixupSceneModifier,
			///    hkbHandIkFixupSceneModifier,
			///    hkbAiControlSceneModifier.
			/// Please see the Scene Modifiers section of the Behavior manual for more details.
		hkBool m_useDefaultSceneModifiers;

			/// Turn on multithreaded update.  If you set this to false only the generate() call is done
			/// multithreaded (if you use the multithreaded interface).  If you set this to true then the 
			/// update() and handleEvents() calls will also be multithreaded.  But note that these do not have an SPU
			/// implementation, only generate() does.
		hkBool m_enableMultithreadedUpdate;

			/// Port for Havok Script
		hkInt32 m_scriptDebuggerPort;
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
