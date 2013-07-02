/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_DOCKING_GENERATOR_H
#define HKB_DOCKING_GENERATOR_H

#include <Behavior/Behavior/Generator/hkbGenerator.h>

class hkbClipGenerator;

class hkbDockingGenerator : public hkbGenerator
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

		/// Construct with defaults.
		hkbDockingGenerator();

	protected:

		hkbDockingGenerator( const hkbDockingGenerator& dockingGen );

	public:

		//////////////////////
		// hkbNode interface
		//////////////////////

		// hkbNode interface implementation.
		virtual void activate( const hkbContext& context ) HK_OVERRIDE;

		// hkbNode interface implementation.
		virtual void update( const hkbContext& context, hkReal timestep ) HK_OVERRIDE;

		// hkbNode interface implementation.
		virtual int getMaxNumChildren( GetChildrenFlags flags ) HK_OVERRIDE;

		// hkbNode interface implementation.
		virtual void getChildren( GetChildrenFlags flags, ChildrenInfo& childrenInfo ) HK_OVERRIDE;

		// hkbNode interface implementation.
		virtual bool isValid( const hkbCharacter* character, hkStringPtr& errorString ) const HK_OVERRIDE;

		// hkbNode interface implementation.
		virtual hkbNode* cloneNode( hkbBehaviorGraph& rootBehavior ) const HK_OVERRIDE;

	public:

		///////////////////////////
		// hkbGenerator interface
		///////////////////////////

		// hkbGenerator interface implementation.
		virtual void generate(	const hkbContext& context,
								const hkbGeneratorOutput** activeChildrenOutput,
								hkbGeneratorOutput& output,
								hkReal timeOffset = 0.0f ) const HK_OVERRIDE;

		// hkbGenerator interface implementation.
		virtual void updateSync( const hkbContext& context ) HK_OVERRIDE;

		// hkbGenerator interface implementation.
		virtual bool canRecycleOutput() const HK_OVERRIDE;

		///////////////
		// properties
		///////////////

			/// The bone to align while docking.
		hkInt16 m_dockingBone;	//+default(-1)
								//+hkb.RoleAttribute("ROLE_BONE_INDEX","FLAG_NONE")
								//+hk.Description("The bone to align while docking.")

		/// The translational offset applied to the docking target.
		hkVector4 m_translationOffset;	//+hk.Description("The translational offset applied to the docking target.")

		/// The rotational offset applied to the docking target.
		hkQuaternion m_rotationOffset;	//+hk.Description("The rotational offset applied to the docking target.")

		enum BlendType
		{
			// The docking correction is being blended in over the course of the generator.
			BLEND_TYPE_BLEND_IN,

			// The docking correction is fully on during the generator.
			BLEND_TYPE_FULL_ON
		};

		// The blend type determines how the docking correction is blended.
		hkEnum<BlendType, hkInt8>	m_blendType; //+hk.Description("The blend type determines how the docking correction is blended.")

		enum DockingFlagBits
		{
			//+defineflags(true)

			/// No flags.
			FLAG_NONE = 0x0,

			/// Predict the future position to determine docking corrections.
			FLAG_DOCK_TO_FUTURE_POSITION = 0x1,

			/// Allow docking to entirely override the motion in the animation.
			FLAG_OVERRIDE_MOTION = 0x2,
		};

		// Flags that modify the behavior of the docking generator.
		hkFlags<DockingFlagBits, hkUint16 > m_flags;	//+default(hkbDockingGenerator::FLAG_NONE)
														//+hk.Description("Flags that modify the behavior of the docking generator.")

		// The underlying generator that plays while docking happens.
		hkRefPtr<hkbGenerator> m_child;	//+hk.Description("The underlying generator that plays while docking happens.")

		// The event which optionally defines the start of the docking interval.
		hkInt32 m_intervalStart;	//+default(hkbEvent::EVENT_ID_NULL)
									//+hkb.RoleAttribute("ROLE_EVENT_ID")
									//+hk.Description("Event which optionally defines the start of the docking interval.  This must be specified in a child clip generator.")

		// The event which optionally defines the end of the docking interval.
		hkInt32 m_intervalEnd;		//+default(hkbEvent::EVENT_ID_NULL)
									//+hkb.RoleAttribute("ROLE_EVENT_ID")
									//+hk.Description("Event which optionally defines the end of the docking interval.  This must be specified in a child clip generator.")

	protected:

		// If m_intervalStart and m_intervalEnd are specified, initializes the docking interval based
		// on their locations on the timeline of the first leaf ward clip.
		void intializeDockingInterval(const hkbContext& context);
		
		// Find the first leaf ward clip generator.  If child nodes of the docking generator do not contain a clip
		// or contain more than one HK_NULL is returned.
		hkbClipGenerator* findFirstLeafWardClip( const hkbContext& context, hkbNode* node ) const;

	protected:

		HKB_BEGIN_INTERNAL_STATE(0);

		// We keep track of time.
		hkReal m_localTime; //+nosave

		// The previous local time of the generator.
		hkReal m_previousLocalTime; 

		// The local time of the start of the docking interval. 
		hkReal m_intervalStartLocalTime; 

		// The local time of the end of the docking interval.
		hkReal m_intervalEndLocalTime; 

		HKB_END_INTERNAL_STATE();

	public:

		hkbDockingGenerator( hkFinishLoadedObjectFlag flag );

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
