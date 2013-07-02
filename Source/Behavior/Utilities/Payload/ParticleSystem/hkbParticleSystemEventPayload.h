/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_PARTICLE_SYSTEM_PAYLOAD_H
#define HKB_PARTICLE_SYSTEM_PAYLOAD_H

#include <Behavior/Behavior/Event/hkbEventPayload.h>

extern const hkClass hkbParticleSystemEventPayloadClass;

	/// An example event payload that can be used to drive a particle system.  This payload class
	/// is not intended to be used in a production environment.  It is used only in Havok demos.
class hkbParticleSystemEventPayload: public hkbEventPayload
{
		//+vtable(1)
	public:

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

		hkbParticleSystemEventPayload() : m_type(DEBRIS), m_emitBoneIndex(-1), m_numParticles(100), m_speed(1.0f) { m_offset.set(0.0f, 0.0f, 0.0f); m_direction.set(0.0f, 0.0f, 1.0f); }

		enum SystemType
		{
			DEBRIS,
			DUST,
			EXPLOSION,
			SMOKE,
			SPARKS
		};

	public:
			/// Type of particle to emit.
		hkEnum<SystemType, hkUint8> m_type; //+default(hkbParticleSystemEventPayload::DUST) //+hk.Description("What type of particle to spawn.")
			/// Index of the bone the system should emit from.
		hkInt16 m_emitBoneIndex; //+default(-1)+hkb.RoleAttribute("ROLE_BONE_INDEX","FLAG_NONE")+hk.Description("The bone the particle system should emit from")
			/// Offset, in local space of the emit bone, to emit the particles from.
		hkVector4 m_offset; //+default(0,0,0)+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_NONE")+hk.Description("Offset, in local space of the emitBone, to emit the particles from.")
			/// Direction, in world space, where particles should emit from
		hkVector4 m_direction; //+default(0,0,1)+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_NORMALIZED")+hk.Description("Direction, in world space, where particles should emit from.")
			/// Color of the particles.
		hkInt32 m_numParticles;  //+default(100) //+hk.Description("Number of particles to emit.")
			/// Speed of the particles
		hkReal m_speed; //+default(1.0f) //+hk.Description("Speed of the particles")
		
	public:

		hkbParticleSystemEventPayload( hkFinishLoadedObjectFlag flag ) : hkbEventPayload(flag) {}
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
