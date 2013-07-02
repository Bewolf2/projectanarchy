/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKAI_LOCAL_STEERING_OUTPUT_H
#define HKAI_LOCAL_STEERING_OUTPUT_H

struct hkaiLocalSteeringOutput
{
	hkVector4 m_forwardAndResult; // m_result encoded into W as int
	hkVector4 m_velocity;

	void setForwardAndResult(hkVector4Parameter forward, hkResult result)
	{
		m_forwardAndResult = forward;
		m_forwardAndResult.setInt24W(result.m_enum);
	}

	hkResult getResult() const
	{
		return hkResult((hkResultEnum)m_forwardAndResult.getInt24W());
	}
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
