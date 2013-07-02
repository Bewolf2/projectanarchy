/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_EVALUATE_HANDLE_MODIFIER_H
#define HKB_EVALUATE_HANDLE_MODIFIER_H

#include <Behavior/Behavior/Modifier/hkbModifier.h>
#include <Behavior/Behavior/Utils/hkbHandle.h>

extern const class hkClass hkbEvaluateHandleModifierClass;

class hkbHandle;

	/// This modifier evaluates a handle on a rigid body during modify().  The result is a position and
	/// rotation in world coordinates.
class hkbEvaluateHandleModifier : public hkbModifier
{
	public:
		// +version(2)

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

			/// Construct with defaults.
		hkbEvaluateHandleModifier();

	protected:

		hkbEvaluateHandleModifier( const hkbEvaluateHandleModifier& mod );

	public:

		~hkbEvaluateHandleModifier();

		//////////////////////
		// hkbNode interface 
		//////////////////////

			// hkbNode interface implementation.
		virtual void activate( const hkbContext& context ) HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual void update( const hkbContext& context, hkReal timestep ) HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual void deactivate( const hkbContext& context ) HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual hkbNode* cloneNode( hkbBehaviorGraph& rootBehavior ) const HK_OVERRIDE;

		//////////////////////////
		// hkbModifier interface
		//////////////////////////

			// hkbModifier interface implementation
		virtual void modify( const hkbContext& context, hkbGeneratorOutput& inOut ) HK_OVERRIDE;
			
		///////////////
		// properties
		///////////////

			/// The handle that is to be evaluated.
		hkRefPtr<hkbHandle> m_handle;	//+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_NOT_CHARACTER_PROPERTY")
										//+hk.Description("The handle that is to be evaluated.")

			/// The position in the world of the handle.
		hkVector4 m_handlePositionOut;	//+default(0,0,0)
										//+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_OUTPUT")
										//+hk.Description("The position in the world of the handle.")
			
			/// The orientation in the world of the handle.
		hkQuaternion m_handleRotationOut;	//+default(0,0,0,1)
											//+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_OUTPUT")
											//+hk.Description("The orientation in the world of the handle.")

			/// Whether or not the handle was evaluated.  If a handle is provided, it will be evaluated.
		hkBool m_isValidOut;	//+default(false)
								//+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_OUTPUT")
								//+hk.Description("Whether or not the handle was evaluated.  If a handle is provided, it will be evaluated.")
			
			/// If the handle is on a rigid body then while evaluating the handle we can extrapolate it
			/// to get its future position.
		hkReal m_extrapolationTimeStep; //+default(0.0f)
										//+hk.Description("If the handle is on a rigid body then while evaluating the handle we can extrapolate it to get its future position.")

			/// If the handle has changed then how fast would you like to go from the old handle's position
			/// and rotation towards the new handle.
		hkReal m_handleChangeSpeed; //+default(1.0f)
									//+hk.Description("If the handles have changed then how fast would you like to go from the old handles position and rotation towards the new handle.")

		enum HandleChangeMode
		{
				/// Switch to the new handle immediately 
			HANDLE_CHANGE_MODE_ABRUPT = 0,

				/// Move to the new handle with constant velocity
			HANDLE_CHANGE_MODE_CONSTANT_VELOCITY,
		};

			/// What do you want to do when the handle changes.
		hkEnum<HandleChangeMode, hkInt8> m_handleChangeMode;	//+default(hkbEvaluateHandleModifier::HANDLE_CHANGE_MODE_ABRUPT)
																//+hk.Description("What do you want to do when the handle changes.")

	private:
		
			/// The handle that was evaluated the last frame
		class hkbHandle m_oldHandle; //+nosave

			/// The position in the world of the old handle
		hkVector4 m_oldHandlePosition; //+nosave

			/// The orientation in the world of the old handle.
		hkQuaternion m_oldHandleRotation; //+nosave

			/// time elapsed since modify was called
		hkReal m_timeSinceLastModify; //+nosave

			/// The status of the handle. Are we still smoothly moving between two handles.
		hkBool m_smoothlyChangingHandles; //+nosave
 
	public:

		hkbEvaluateHandleModifier( hkFinishLoadedObjectFlag flag );

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
