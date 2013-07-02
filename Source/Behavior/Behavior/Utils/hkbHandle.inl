/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

HK_FORCE_INLINE void hkbHandle::clear()
{
	m_frame = HK_NULL;
	m_rigidBody = HKB_INVALID_PHYSICS_HANDLE;
	m_character = HK_NULL;
	m_animationBoneIndex = -1;
}

HK_FORCE_INLINE bool hkbHandle::isValid() const
{
	// The m_frame itself can be null, which just means that we will use the coordinate frame
	// of the rigid body or character.

	return ( ( m_rigidBody != HKB_INVALID_PHYSICS_HANDLE ) || ( m_character != HK_NULL ) );
}

HK_FORCE_INLINE void hkbHandle::operator = ( const hkbHandle& handle )
{
	m_frame = handle.m_frame;
	m_rigidBody = handle.m_rigidBody;
	m_character = handle.m_character;
	m_animationBoneIndex = handle.m_animationBoneIndex;
}

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
