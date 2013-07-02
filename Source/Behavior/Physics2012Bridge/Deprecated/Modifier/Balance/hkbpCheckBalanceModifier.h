/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKBP_CHECK_BALANCE_MODIFIER_H
#define HKBP_CHECK_BALANCE_MODIFIER_H

#include <Behavior/Behavior/Modifier/hkbModifier.h>

extern const class hkClass hkbpCheckBalanceModifierClass;

/// An example custom modifier for checking the balance of the ragdoll.
class hkbpCheckBalanceModifier : public hkbModifier
{
	//+version(1)

	public:
		
		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

		hkbpCheckBalanceModifier();

	public:

		//////////////////////
		// hkbNode interface 
		//////////////////////

			// hkbNode interface implementation.
		virtual void activate( const hkbContext& context );

			// hkbNode interface implementation.
		virtual void update( const hkbContext& context, hkReal timestep );

		//////////////////////////
		// hkbModifier interface
		//////////////////////////

			// hkbModifier interface implementation
		virtual void modify( const hkbContext& context, hkbGeneratorOutput& inOut );

		///////////////
		// properties
		///////////////
		
		/// the left foot bone (used to determine the ideal center-of-mass)
		hkInt16 m_ragdollLeftFootBoneIndex;

		/// the right foot bone (used to determine the ideal center-of-mass)
		hkInt16 m_ragdollRightFootBoneIndex;

		/// to balance over the ankle midpoint instead of the foot centroid midpoint, set this > 0 and <= 1
		/// the rational for this is that the ragdoll feet are usually exaggerated in size
		hkReal m_balanceOnAnklesFraction;

		/// when the character is deemed to be off balance, this event is sent
		class hkbEvent m_eventToSendWhenOffBalance;

		/// if the distance between the com and ideal com is beyond this threshold, the event is sent
		hkReal m_offBalanceEventThreshold;

		/// the world up axis 
		int m_worldUpAxisIndex;

		/// you can bias the COM in the X direction
		hkReal m_comBiasX;

		/// if true, extract the pose from the ragdoll instead of using the input pose
		hkBool m_extractRagdollPose;

		///////////
		// output
		///////////

		/// the mass of the character
		hkReal m_mass; //+nosave

		/// the center of mass of the character
		hkVector4 m_comWS; //+nosave

		/// the desired center of mass of the character (between the feet)
		hkVector4 m_desiredComWS; //+nosave

		/// distance (ignoring Y) between m_com and m_desiredCom
		hkReal m_offBalanceDistance; //+nosave

		/// the direction of off-balancedness, in model space
		hkVector4 m_errorMS; //+nosave

	public:

		hkbpCheckBalanceModifier( hkFinishLoadedObjectFlag flag );

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
