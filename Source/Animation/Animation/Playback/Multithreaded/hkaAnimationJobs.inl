/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#if !defined(HK_PLATFORM_SPU)
static hkBool testValidForDMA( const char* name, const void* srcInMainMemory, int size)
{
	if(size == 0)
	{
		return true;
	}
#ifdef HK_PLATFORM_HAS_SPU
	hkBool sizeValid = (size >= 0) && !(size             & 0xf);
	HK_ASSERT3(0xad97d84f, sizeValid, name << " size must be >= 0 and a multiple of 16.");
	if(!sizeValid) return false;

	hkBool sizeLess16 = size <= 0x4000;
	HK_ASSERT3(0xad97d84f, sizeLess16, name <<  " size must be less than 16k.");
	if(!sizeLess16) return false;

	hkBool aligned16 = !(hkUlong(srcInMainMemory) & 0xf);
	HK_ASSERT3(0xad97d8de, aligned16, name << " address must be 16-byte aligned.");
	if(!aligned16) return false;

	return true;
#else
	return true;
#endif
}
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
