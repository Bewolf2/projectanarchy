/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_CAMERA_SHAKE_PAYLOAD_H
#define HKB_CAMERA_SHAKE_PAYLOAD_H

#include <Behavior/Behavior/Event/hkbEventPayload.h>

extern const hkClass hkbCameraShakeEventPayloadClass;

	/// An example event payload that can be used to drive camera shake.  This payload class
	/// is not intended to be used in a production environment.  It is used only in Havok demos.
class hkbCameraShakeEventPayload: public hkbEventPayload
{
		//+vtable(1)
	public:

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

		hkbCameraShakeEventPayload() : m_amplitude( 1.0f ), m_halfLife( 1.0f ) {}

	public:

		hkReal m_amplitude; //+default(1.0f) //+hk.Description("Amplitude of the camera shake")
		hkReal m_halfLife;  //+default(1.0f) //+hk.Description("Half life of the camera shake in seconds")
		
	public:

		hkbCameraShakeEventPayload( hkFinishLoadedObjectFlag flag ) : hkbEventPayload(flag) {}
};

#endif // HKB_CAMERA_SHAKE_PAYLOAD_H

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
