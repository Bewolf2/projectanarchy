/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_COMPUTE_DIRECTION_MODIFIER_H
#define HKB_COMPUTE_DIRECTION_MODIFIER_H

#include <Behavior/Behavior/Modifier/hkbModifier.h>

extern const class hkClass hkbComputeDirectionModifierClass;

class hkbCharacterData;

	/// Deprecated.  Please use the hkbScriptGenerator instead.
	/// 
	/// Given an input point, this modifier outputs the point in the character's coordinate frame, and also a 
	/// vertical angle and ground angle.
class hkbComputeDirectionModifier : public hkbModifier
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

		hkbComputeDirectionModifier();
		hkbComputeDirectionModifier( const hkbComputeDirectionModifier& );

		//////////////////////
		// hkbNode interface 
		//////////////////////

			// hkbNode interface implementation.
		virtual void activate( const hkbContext& context ) HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual hkbNode* cloneNode( hkbBehaviorGraph& rootBehavior ) const HK_OVERRIDE;

		///////////////////////////
		// hkbModifier interface
		///////////////////////////

			// hkbModifier interface implementation.
		virtual void modify( const hkbContext& context, hkbGeneratorOutput& inOut ) HK_OVERRIDE;

	public:

		//////////////
		// properties
		//////////////

			/// The point in world coordinates that we want the direction to.
		hkVector4 m_pointIn;	//+hk.Description("The point in world coordinates that we want the direction to.")

			/// The input point transformed to the character's coordinates.
		hkVector4 m_pointOut;	//+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_OUTPUT")
								//+hk.Description("The input point transformed to the character's coordinates.")

			/// The angle in the ground plane to the point in the character's coordinates.  This is in the range from -180 to 180.  Zero means directly in front of the character.  Positive angles are to the right of the character.
		hkReal m_groundAngleOut;	//+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_OUTPUT")
									//+hk.Description("The angle in the ground plane to the point in the character's coordinates.  This is in the range from -180 to 180.  Zero means directly in front of the character.  Positive angles are to the right of the character.  Directly behind the character is PI.")

			/// The vertical angle to the target.
		hkReal m_upAngleOut;	//+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_OUTPUT")
								//+hk.Description("The vertical angle to the target.")

			/// This is added to m_pointOut in the up direction before computing the output angles.
		hkReal m_verticalOffset;	//+hk.Description("This is added to m_pointOut in the up direction before computing the output angles.")

			/// Flip the sign of m_groundAngleOut.
		hkBool m_reverseGroundAngle;	//+hk.Description("Flip the sign of m_groundAngleOut.")

			/// Flip the sign of m_upAngleOut.
		hkBool m_reverseUpAngle;	//+hk.Description("Flip the sign of m_upAngleOut.")

			/// Whether to remove the character's up component from m_pointOut, projecting it onto the ground plane.
		hkBool m_projectPoint;	//+hk.Description("Whether to remove the character's up component from m_pointOut, projecting it onto the ground plane.")

			/// Whether to normalize m_pointOut to be of length 1.
		hkBool m_normalizePoint; //+hk.Description("Whether to normalize m_pointOut to be of length 1.")

			/// Whether to compute the output only once.
		hkBool m_computeOnlyOnce;	//+hk.Description("Whether to compute the output only once.")

	private:

		HKB_BEGIN_INTERNAL_STATE(0);

		// these are commented out on purpose to force the output properties to be part of the internal state
		//hkVector4 m_pointOut;
		//hkReal m_groundAngleOut;
		//hkReal m_upAngleOut;

		hkBool m_computedOutput;

		HKB_END_INTERNAL_STATE();

	public:

		hkbComputeDirectionModifier( hkFinishLoadedObjectFlag flag );

		friend class hkbComputeDirectionModifierUtils;
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
