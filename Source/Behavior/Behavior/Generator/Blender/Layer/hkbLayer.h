/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_LAYER_H
#define HKB_LAYER_H

#include <Behavior/Behavior/Character/hkbBoneWeightArray.h>
#include <Behavior/Behavior/Node/hkbBindable.h>
#include <Behavior/Behavior/Event/hkbEvent.h>

class hkbGenerator;

class hkbLayer : public hkbBindable
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

		hkbLayer();

	private:

		hkbLayer( const hkbLayer& layer );

	public:

		~hkbLayer();

	public:

			/// Get a bone weight, with bounds checking.
		hkReal getBoneWeight( int boneIndex ) const;

		///////////////
		// properties
		///////////////

			/// The generator associated with this child.
		HK_ALIGN16( hkbGenerator* m_generator );

			/// The blend weight for this layer.
		hkReal m_weight;	//+default(1.0f)
							//+hkb.RoleAttribute("ROLE_DEFAULT")
							//+hk.RangeReal(absmin=0,absmax=1)
							//+hk.Description("The blend weight for this layer.")

			/// A weight for each bone.
		hkRefPtr<hkbBoneWeightArray> m_boneWeights; //+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_NOT_VARIABLE")

			/// Set the fadeInDuration if the layer needs to be gradually introduced when switching on the layer.
		hkReal m_fadeInDuration;	//+default(0.0f)
									//+hkb.RoleAttribute("ROLE_TIME")
									//+hk.RangeReal(absmin=0,absmax=1000,softmax=10)
									//+hk.Description("Set the fadeInDuration if the layer needs to be gradually introduced when switching on the layer.")

			/// Set the fadeOutDuration if the layer needs to be gradually removed when switching off the layer.
		hkReal m_fadeOutDuration;	//+default(0.0f)
									//+hkb.RoleAttribute("ROLE_TIME")
									//+hk.RangeReal(absmin=0,absmax=1000,softmax=10)
									//+hk.Description("Set the fadeOutDuration if the layer needs to be gradually removed when switching off the layer..")

			/// Event used to switch on the layer
		hkInt32 m_onEventId;	//+default(hkbEvent::EVENT_ID_NULL)
								//+hkb.RoleAttribute("ROLE_EVENT_ID")
								//+hk.Description("Event used to switch on the layer.")

			/// Event used to switch off the layer
		hkInt32 m_offEventId;	//+default(hkbEvent::EVENT_ID_NULL)
								//+hkb.RoleAttribute("ROLE_EVENT_ID")
								//+hk.Description("Event used to switch off the layer.")

			/// If the layer needs to be on when the hkbLayerGenerator is activated set this to true.
		hkBool m_onByDefault;	//+default(false)
								//+hk.Description("If the layer needs to be on when the hkbLayerGenerator is activated set this to true.")

			/// Set this to true if you want to include the world from model from this layer, false otherwise.
		hkBool m_useMotion;	//+default(true)
							//+hk.Description("Set this to true if you want to include the world from model from this layer, false otherwise.")

	public:

		hkbLayer( hkFinishLoadedObjectFlag flag );

		friend class hkbLayerGenerator;
};

#include <Behavior/Behavior/Generator/Blender/Layer/hkbLayer.inl>

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
