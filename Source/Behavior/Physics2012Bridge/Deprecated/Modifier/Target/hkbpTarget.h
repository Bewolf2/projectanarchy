/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKBP_TARGET_H
#define HKBP_TARGET_H

extern const class hkClass hkbpTargetClass;

class hkpRigidBody;

	/// A rigid body or point in space that is being targeted.
class hkbpTarget : public hkReferencedObject
{
	public:
		// +version(3)
		// +vtable(1)

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

		hkbpTarget() {}

			/// Set all members to zero.
		void clear();

			/// Copy all members.
		void operator = ( const hkbpTarget& target );
		
		// the current target
		const hkpRigidBody* m_target; //+nosave+default(HK_NULL)

		// the current contact point in the target rigid body space
		hkVector4 m_contactPointTS; //+nosave

		// the current contact normal in the target rigid body space
		hkVector4 m_contactNormalTS; //+nosave

		// the centroid of the shape in the target rigid body space
		hkVector4 m_shapeCentroidTS; //+nosave

		// the target distance
		hkReal m_distance; //+nosave

		// target priority
		hkInt8 m_targetPriority; //+nosave

	public:

		hkbpTarget( hkFinishLoadedObjectFlag flag ) : hkReferencedObject(flag) {}

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
