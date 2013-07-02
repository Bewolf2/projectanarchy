/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_CUSTOM_TEST_GENERATOR_H
#define HKB_CUSTOM_TEST_GENERATOR_H

#include <Behavior/Behavior/Generator/ReferencePose/hkbReferencePoseGenerator.h>
#include <Behavior/Behavior/Modifier/hkbModifier.h>
#include <Behavior/Utilities/Physics/hkbPhysicsBaseTypes.h>

extern const class hkClass hkbCustomTestGeneratorStruckClass;

extern const class hkClass hkbCustomTestGeneratorClass;

	/// This custom generator demonstrates all of the public properties that HAT is capable of exposing.
class hkbCustomTestGenerator : public hkbReferencePoseGenerator
{
	//+version(1)
	//+vtable(1)

	public:

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

			// default constructor
		hkbCustomTestGenerator();

	protected:

		hkbCustomTestGenerator( const hkbCustomTestGenerator& gen );

	public:

			// packfile constructor
		hkbCustomTestGenerator( hkFinishLoadedObjectFlag flag );

			// hkbNode interface implementation.
		virtual hkbNode* cloneNode( hkbBehaviorGraph& rootBehavior ) const HK_OVERRIDE;

		///////////////
		// properties
		///////////////

		// basic types
		hkVector4 m_hkVector4;
		hkBool m_hkBool;
		hkStringPtr m_string; //+default(HK_NULL)+hkb.RoleAttribute("ROLE_FILE_NAME")
		int m_int; //+hk.Ui(label="number of items")+hk.Description("The number of items in your item pool.  Must be an odd number or lucky 13")
		hkInt8 m_hkInt8; //+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_NOT_VARIABLE")
		hkInt16 m_hkInt16;
		hkInt32 m_hkInt32; //+hk.RangeInt32(absmax=50,softmax=1,softmin=-100,absmin=-1000000000)
		hkUint8 m_hkUint8;
		hkUint16 m_hkUint16;
		hkUint32 m_hkUint32;
		hkReal m_hkReal; //+hk.RangeReal(absmin=-500.0,absmax=-400.0,softmin=-499.0,softmax=-401.0) 
		hkQuaternion m_hkQuaternion;
		hkbRigidBodyHandleRef m_hkRigidBody; //+default(HK_NULL)

		enum Modes
		{
			MODE_ALA = 0,
			MODE_DEPECHE = 1,
			MODE_FURIOUS = 5,
		};

		// modes should come up as a drop-down list or radio buttons
		hkEnum<Modes, hkInt8> m_mode_hkInt8;
		hkEnum<Modes, hkInt16> m_mode_hkInt16;
		hkEnum<Modes, hkInt32> m_mode_hkInt32;
		hkEnum<Modes, hkUint8> m_mode_hkUint8;
		hkEnum<Modes, hkUint16> m_mode_hkUint16;
		hkEnum<Modes, hkUint32> m_mode_hkUint32;

		enum StrangeFlags
		{
			//+defineflags(true)
			FLAG_UNO = 0x1,
			FLAG_ZWEI = 0x2,
			FLAG_SHI_OR_YON = 0x4,
			FLAG_LOTS_O_BITS = 0xf0,
		};

		// flags should come up as check boxes in the UI
		hkFlags<StrangeFlags, hkInt8> m_flags_hkInt8;
		hkFlags<StrangeFlags, hkInt16> m_flags_hkInt16;
		hkFlags<StrangeFlags, hkInt32> m_flags_hkInt32;
		hkFlags<StrangeFlags, hkUint8> m_flags_hkUint8;
		hkFlags<StrangeFlags, hkUint16> m_flags_hkUint16;
		hkFlags<StrangeFlags, hkUint32> m_flags_hkUint32;

		// try a typedef
		typedef hkInt32 MyInt;
		MyInt m_myInt;

		// you should be able to hook up a child here
		hkbGenerator* m_generator1;
		hkbGenerator* m_generator2;

		// and you should be able to hook up a child modifier
		hkbModifier* m_modifier1;
		hkbModifier* m_modifier2;

		// arrays should be supported
		hkArray<hkBool> m_array_hkBool;
		hkArray<int> m_array_int;
		hkArray<hkInt8> m_array_hkInt8;
		hkArray<hkInt16> m_array_hkInt16;
		hkArray<hkInt32> m_array_hkInt32;
		hkArray<hkInt8> m_array_hkUint8;
		hkArray<hkInt16> m_array_hkUint16;
		hkArray<hkInt32> m_array_hkUint32;
		hkArray<hkReal> m_array_hkReal;
		hkArray<hkbGenerator*> m_array_hkbGenerator;
		hkArray<hkbModifier*> m_array_hkbModifier;

		struct Struck
		{
			HK_DECLARE_REFLECTION();
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbCustomTestGenerator::Struck );

			Struck();
			Struck(hkFinishLoadedObjectFlag flag): m_string(flag) {}

			hkBool m_hkBool;
			hkStringPtr m_string;
			int m_int;
			hkInt8 m_hkInt8;
			hkInt16 m_hkInt16;
			hkInt32 m_hkInt32;
			hkUint8 m_hkUint8;
			hkUint16 m_hkUint16;
			hkUint32 m_hkUint32;
			hkReal m_hkReal;

			hkEnum<Modes, hkInt8> m_mode_hkInt8;
			hkEnum<Modes, hkInt16> m_mode_hkInt16;
			hkEnum<Modes, hkInt32> m_mode_hkInt32;
			hkEnum<Modes, hkUint8> m_mode_hkUint8;
			hkEnum<Modes, hkUint16> m_mode_hkUint16;
			hkEnum<Modes, hkUint32> m_mode_hkUint32;

			hkFlags<StrangeFlags, hkInt8> m_flags_hkInt8;
			hkFlags<StrangeFlags, hkInt16> m_flags_hkInt16;
			hkFlags<StrangeFlags, hkInt32> m_flags_hkInt32;
			hkFlags<StrangeFlags, hkUint8> m_flags_hkUint8;
			hkFlags<StrangeFlags, hkUint16> m_flags_hkUint16;
			hkFlags<StrangeFlags, hkUint32> m_flags_hkUint32;

			hkbGenerator* m_generator1;
			hkbGenerator* m_generator2;
			hkbModifier* m_modifier1;
			hkbModifier* m_modifier2;

			// note: we won't allow arrays in structs
		};

		// am embedded struct
		struct Struck m_Struck;

		// an array of structs
		hkArray<struct Struck> m_array_Struck;
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
