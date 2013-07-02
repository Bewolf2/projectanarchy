/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_BLENDER_GENERATOR_CHILD_H
#define HKB_BLENDER_GENERATOR_CHILD_H

#include <Behavior/Behavior/Character/hkbBoneWeightArray.h>

extern const class hkClass hkbBlenderGeneratorChildClass;

class hkbGenerator;

	/// An internal class representing the child of an hkbBlenderGenerator.
class hkbBlenderGeneratorChild : public hkbBindable
{
	public:

		//+version(2)
		//+vtable(1)

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

		hkbBlenderGeneratorChild();

	private:

		hkbBlenderGeneratorChild( const hkbBlenderGeneratorChild& child );

	public:

		~hkbBlenderGeneratorChild();

		//////////////////////////
		// hkbBindable interface
		//////////////////////////

			// hkbBindable interface implementation.
		virtual void collectBindables( hkbBindableCollector& collector ) HK_OVERRIDE;

	public:

			/// Get a bone weight, with bounds checking.
		hkReal getBoneWeight( int boneIndex ) const;

			/// Get the world-from-model weight, with bounds checking.
		hkReal getWorldFromModelWeight() const;

		///////////////
		// properties
		///////////////

			/// The generator associated with this child.
		HK_ALIGN16( hkbGenerator* m_generator );

			/// A weight for each bone.
		hkRefPtr<hkbBoneWeightArray> m_boneWeights; //+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_NOT_VARIABLE")

			/// The blend weight for this child.
		hkReal m_weight; //+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_HIDDEN")

			/// The weights used to determine the contribution of this child's world-from-model
			/// transform to the final world-from-model.
		hkReal m_worldFromModelWeight; //+default(1.0)+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_HIDDEN")

	public:

		hkbBlenderGeneratorChild( hkFinishLoadedObjectFlag flag );

		friend class hkbBlenderGeneratorUtils;
		friend class hkbBlenderGenerator;
};

#include <Behavior/Behavior/Generator/Blender/hkbBlenderGeneratorChild.inl>

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
