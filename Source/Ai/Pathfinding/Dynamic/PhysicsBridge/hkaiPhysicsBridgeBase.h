/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKAI_PHYSICS_BRIDGE_H
#define HKAI_PHYSICS_BRIDGE_H

#include <Ai/Pathfinding/World/hkaiWorld.h>
#include <Ai/Pathfinding/Dynamic/Silhouette/SilhouetteGenerator/PointCloud/hkaiPointCloudSilhouetteGenerator.h>

class hkaiSilhouetteGenerator;
class hkaiObstacleGenerator;

	/// Common interface for hkaiPhysicsWorldListener and hkaiPhysics2012WorldListener.
class hkaiPhysicsBridgeBase : public hkReferencedObject, public hkaiWorld::Listener
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI);

		hkaiPhysicsBridgeBase(hkaiWorld* aiWorld);
		virtual ~hkaiPhysicsBridgeBase();

			/// This will register callback functions with the physics world and add silhouette/obstacle generators
			/// for existing bodies.
		virtual void connectToPhysicsWorld() = 0;

			/// Disconnect any connected callbacks from the physics world.
		virtual void disconnectFromPhysicsWorld() = 0;

		enum BodyTypeFlagBits
		{
			BODY_INVALID = 0,

				/// Fully dynamic (i.e. not keyframed) body
			BODY_DYNAMIC = 1 << 0,
			
				/// Keyframed body
			BODY_KEYFRAMED = 1 << 1,
			
				/// Static/fixed body
			BODY_STATIC = 1 << 2,
			
				/// Character body (usually dynamic but not necessarily)
			BODY_CHARACTER = 1 << 3,

				/// Default ignored body types flags
			DEFAULT_IGNORE = BODY_STATIC | BODY_CHARACTER,
		};

		typedef hkFlags<BodyTypeFlagBits, hkUint8> BodyTypeFlags;

	
			/// Set the shift that has been applied to the physics world.
		inline void setWorldShift( hkVector4Parameter shift);

			/// Get the shift that has been applied to the physics world.
		inline const hkVector4& getWorldShift() const;

			/// Set the flags used to determine which bodies are ignored.
		inline void setIgnoreBodyFlags( BodyTypeFlags flags);

			/// Get the flags used to determine which bodies are ignored.
		inline BodyTypeFlags getIgnoreBodyFlags() const;

		inline hkReal getDefaultSilhouetteDistanceThreshold() const;
		inline void setDefaultSilhouetteDistanceThreshold(hkReal val);
		inline hkReal getDefaultSilhouetteVelocityThreshold() const;
		inline void setDefaultSilhouetteVelocityThreshold(hkReal val);

		inline hkaiPointCloudSilhouetteGenerator::DetailLevel getDefaultDetailLevel() const;
		inline void setDefaultDetailLevel( hkaiPointCloudSilhouetteGenerator::DetailLevel val);


	protected:

			/// If this value is non-negative, new silhouettes will use this as their distance threshold.
		hkReal	m_defaultSilhouetteDistanceThreshold;

			/// If this value is non-negative, new silhouettes will use this as their velocity threshold.
		hkReal	m_defaultSilhouetteVelocityThreshold;

		hkRefPtr<hkaiWorld>	m_aiWorld;

			/// Bodies that match any of these flags are ignored for silhouette and obstacle generators.
		BodyTypeFlags m_ignoreBodyFlags;

		hkEnum< hkaiPointCloudSilhouetteGenerator::DetailLevel, hkUint8 > m_defaultDetailLevel;
			/// Shift that has been applied to the physics world.
			/// This is sometimes done to avoid numerical precision issues in large open-world games.
			/// Note that this must be set manually, it is not tracked by the hkpWorld or hknpWorld
			/// Accessed by getWorldShift() and setWorldShift()
		hkVector4 m_worldShift;

};

#include <Ai/Pathfinding/Dynamic/PhysicsBridge/hkaiPhysicsBridgeBase.inl>

#endif	// HKAI_PHYSICS_BRIDGE_H

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
