/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_JIGGLER_GROUP
#define HKB_JIGGLER_GROUP

#include <Behavior/Behavior/Node/hkbBindable.h>
#include <Behavior/Behavior/Character/hkbBoneIndexArray.h>

extern const class hkClass hkbJigglerGroupClass;

/// This is a helper class for hkbJigglerModifier. The hkbJigglerModifier has an array of pointers
/// to this. This allows you to attach a single spring-mass system to an arbitrary number of bones.
class hkbJigglerGroup : public hkbBindable
{
	public:
		// +version(1)
		//+vtable(1)
		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

		hkbJigglerGroup();

		hkbJigglerGroup( const hkbJigglerGroup& group );

		~hkbJigglerGroup();

		//////////////////////////
		// hkbBindable interface
		//////////////////////////

			// hkbBindable interface implementation.
		virtual void collectBindables( hkbBindableCollector& collector ) HK_OVERRIDE;

	public:

		hkRefPtr<hkbBoneIndexArray> m_boneIndices;	//+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_NOT_VARIABLE")
													//+hk.Description("The bones that will be jiggled in this group.")

			/// The mass.
		hkReal m_mass; //+default(1.0f)+hk.RangeReal(absmin=0.001,absmax=10000.0,softmin=0.001,softmax=100.0)+hk.Description("The mass of each jiggled bone point.")

			/// How stiff the spring is.
		hkReal m_stiffness; //+default(100.0f)+hk.RangeReal(absmin=0.001,absmax=100000.0,softmin=0.001,softmax=1000)+hk.Description("The stiffness of the spring attaching each jiggled bone point to the animating skeleton.  Larger values will cause faster jiggles and more conformance to the incoming animation.")

			/// How much the spring should be damped.
		hkReal m_damping; //+default(1.0f)+hk.RangeReal(absmin=0.0,absmax=1000.0,softmin=0.0,softmax=10.0)+hk.Description("The damping of each spring.  Larger values will create slower, smoother motion.")

			/// By what fraction of its original length the bone can stretch.  A value of 0 means
			/// that the bone cannot stretch at all.  A value of 1 means that it can stretch to twice its original
			/// length.
		hkReal m_maxElongation; //+default(1.0f)+hk.RangeReal(absmin=0.0,absmax=100.0,softmin=0.0,softmax=2.0)+hk.Description("By what fraction of its original length the bone can stretch.  A value of 0 means that the bone cannot stretch at all.  A value of 1 means that it can stretch to twice its original length.")

			/// By what fraction of its original length the bone can compress.  A value of 0 means
			/// that it cannot shrink at all.  A value of 1 means that it can fully shrink to a length of
			/// zero.
		hkReal m_maxCompression; //+default(0.5f)+hk.RangeReal(absmin=0.0,absmax=1.0)+hk.Description("By what fraction of its original length the bone can compress. A value of 0 means that it cannot shrink at all.  A value of 1 means that it can fully shrink to zero length.")

			/// The jiggling of the bone affects all its children when this is set to true.
		hkBool m_propagateToChildren; //+default(true)+hk.Description("The jiggling of the bone affects all its children when this is set to true.")

			/// The bone is jiggled by rotating its parent bone. The parent bone can have other
			/// children (jiggled bone's siblings). You may not want the siblings to move around.
			/// If this is set to true then the rotation of the parent bone affects all its children.
			/// Otherwise it just affects the jiggled bone.
		hkBool m_affectSiblings; //+default(false)+hk.Description("The bone is jiggled by rotating its parent bone. The parent bone can have other children, siblings of the jiggled bone. If this is set to true then the rotation of the parent bone affects all its children. Otherwise it just affects the jiggled bone.")

			/// If jiggling breaks skinning then this should be set to true.
		hkBool m_rotateBonesForSkinning; //+default(false)+hk.Description("If jiggling breaks skinning then this should be set to true.")

		hkBool m_pad[5]; //+nosave

	public:

		hkbJigglerGroup( hkFinishLoadedObjectFlag flag )
			:	hkbBindable(flag),
				m_boneIndices(flag) {}
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
