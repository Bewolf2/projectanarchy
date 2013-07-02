/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_HANDLE_H
#define HKB_HANDLE_H

extern const class hkClass hkbHandleClass;

class hkbCharacter;
class hkLocalFrame;
class hkbGeneratorOutput;

#include <Behavior/Utilities/Physics/hkbPhysicsBaseTypes.h>

	/// A reference frame on a rigid body or character.  
class hkbHandle : public hkReferencedObject
{
	//+vtable(1)
	//+version(2)

	public:

		HK_DECLARE_REFLECTION();
		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );

		hkbHandle() {}

			/// Set all members to zero.
		void clear();

			/// Whether this reference refers to a coordinate frame.
		bool isValid() const;

			/// Get the transform from the coordinates of m_frame to the
			/// world coordinates.  If m_frame is HK_NULL, the transform from
			/// local space of the rigid body or animation bone to world coordinates
			/// is returned.  If the handle refers to an animation bone, a pose is required.
			/// You can either provide one, or the pose will be taken from m_character.
			/// If the handle refers to a rigid body, a physicsInterface must be provided.
		void getWorldFromHandleTransform(
			hkbPhysicsInterface* physicsInterface,
			hkTransform& worldFromHandle,
			const hkbGeneratorOutput* output = HK_NULL ) const;

			/// Get the transform from the coordinates of m_frame to the
			/// world coordinates.  If m_frame is HK_NULL, the transform from
			/// local space of the rigid body to world coordinates is returned.
			/// The assumption is that the handle is on the rigid body. 
		void getExtrapolatedWorldFromHandle(
			hkbPhysicsInterface* physicsInterface,
			hkTransform& worldFromHandle,
			hkReal timeStep ) const;

			/// Copy the contents of a handle.
		void operator = ( const hkbHandle& handle );

	public:

			/// A local frame attached to m_rigidBody or the animation skeleton of m_character.  This can be null if there is no local frame.
		const hkLocalFrame* m_frame;	//+default(HK_NULL)
										//+hk.Description("A local frame attached to m_rigidBody or the animation skeleton of m_character. This can be null if there is no local frame.")

			/// The rigid body that m_frame is attached to, or that is itself the handle.  This can be null if there is no rigid body.
		hkbRigidBodyHandleRef m_rigidBody;	//+default(HK_NULL)
											//+hk.Description("The rigid body that m_frame is attached to, or that is itself the handle.  This can be null if there is no rigid body.")

			/// The character that the local frame is attached to, or that is itself the handle.  This can be null if there is no character.
		hkbCharacter* m_character;	//+default(HK_NULL)
									//+hk.Description("The character that the local frame is attached to, or that is itself the handle.  This can be null if there is no character.")

			/// The animation bone that the local frame is attached to (-1 if none).
		hkInt16 m_animationBoneIndex;	//+default(-1)
										//+hk.Description("The animation bone that the local frame is attached to, if there is one.")

	public:

			/// Check if the two input handles are the same.
		static bool HK_CALL areHandlesSame( const hkbHandle& a, const hkbHandle& b );

	public:

		hkbHandle( hkFinishLoadedObjectFlag flag ) : hkReferencedObject( flag ), m_rigidBody(flag) {}
};

#include <Behavior/Behavior/Utils/hkbHandle.inl>

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
