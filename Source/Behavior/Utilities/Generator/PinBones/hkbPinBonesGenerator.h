/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_PIN_BONE_GENERATOR_H
#define HKB_PIN_BONE_GENERATOR_H

#include <Behavior/Behavior/Generator/hkbGenerator.h>
#include <Behavior/Behavior/Character/hkbBoneIndexArray.h>

extern const class hkClass hkbPinBonesGeneratorClass;

/// An example custom generator for pinning a set of bones in world space. 
class hkbPinBonesGenerator : public hkbGenerator
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

		hkbPinBonesGenerator();

	protected:

		hkbPinBonesGenerator( const hkbPinBonesGenerator& gen );

	public:

		~hkbPinBonesGenerator();

		//////////////////////////
		// hkbBindable interface
		//////////////////////////

		// hkbBindable interface implementation.
		virtual void collectBindables( hkbBindableCollector& collector ) HK_OVERRIDE;

		//////////////////////
		// hkbNode interface 
		//////////////////////

			// hkbNode interface implementation.
		virtual void getChildren( GetChildrenFlags flags, ChildrenInfo& childrenInfo ) HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual bool isValid( const hkbCharacter* character, hkStringPtr& errorString ) const HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual hkbNode* cloneNode( hkbBehaviorGraph& rootBehavior ) const HK_OVERRIDE;

		///////////////////////////
		// hkbGenerator interface
		///////////////////////////

			// hkbGenerator interface implementation
		virtual void generate(	const hkbContext& context, 
								const hkbGeneratorOutput** activeChildrenOutput,
								hkbGeneratorOutput& output,
								hkReal timeOffset = 0.0f ) const HK_OVERRIDE;

			// hkbGenerator interface implementation
		virtual void updateSync( const hkbContext& context ) HK_OVERRIDE;

			// hkbGenerator interface implementation
		virtual void computePartitionInfo( const hkArray<hkbNodeChildInfo>& activeChildren, const hkbContext& context ) HK_OVERRIDE;

		///////////////
		// properties
		///////////////

			// The generator from which the reference frame of the pinned bone be computed.
		hkRefPtr<hkbGenerator> m_referenceFrameGenerator; //+hk.Description("The generator from which the reference frame of the pinned bone be computed.")

			// The animation from this generator is pinned.
		hkRefPtr<hkbGenerator> m_pinnedGenerator; //+hk.Description("The animation from this generator is pinned.")

			// bone indices
		hkRefPtr<hkbBoneIndexArray> m_boneIndices;	//+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_NOT_VARIABLE")
													//+hk.Description("The bones that will be pinned.")

			// Fraction to pin the bones. Set this to 1 to completely pin the bones.
		hkReal m_fraction; //+default(0)
										//+hk.RangeReal(absmin=0.0,absmax=1.0)
										//+hk.Description("Fraction to pin the bones. Set this to 1 to completely pin the bones." )

	public:

		hkbPinBonesGenerator( hkFinishLoadedObjectFlag flag );

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
