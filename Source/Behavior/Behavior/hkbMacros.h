/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_MACROS_H
#define HKB_MACROS_H

#define HK_PTR_TO_hkUint64( ptr ) (hkUint64)( (hkUlong)( ptr ) )

#define HKB_COPY_MEMBERS( dest, source, TYPE, m_firstMember, m_lastMember ) \
{ \
	int startOffset = HK_OFFSET_OF( TYPE, m_firstMember ); \
	int endOffset = HK_OFFSET_OF( TYPE, m_lastMember ) + sizeof(m_lastMember); \
	int numBytes = endOffset - startOffset; \
	hkString::memCpy( &(dest).m_firstMember, &(source).m_firstMember, numBytes ); \
}

// internalClassVersion should be the version of the InternalClass that will be generated
// This should be the latest version as defined by the patches
#define HKB_BEGIN_INTERNAL_STATE( internalClassVersion ) \
	public: \
	virtual hkReferencedObject* createInternalState() HK_OVERRIDE; \
	virtual void getInternalState( hkReferencedObject& internalState ) const HK_OVERRIDE; \
	virtual void setInternalState( const hkReferencedObject& internalState ) HK_OVERRIDE; \
	protected:

#define HKB_END_INTERNAL_STATE()

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
