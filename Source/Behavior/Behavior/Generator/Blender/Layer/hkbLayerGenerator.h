/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_LAYER_GENERATOR_H
#define HKB_LAYER_GENERATOR_H

#include <Behavior/Behavior/Generator/hkbGenerator.h>
#include <Behavior/Behavior/Generator/Blender/Layer/hkbLayer.h>

class hkbLayer;

/// This generator is a Beta feature
class hkbLayerGenerator : public hkbGenerator
{
	public:
		
		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

			/// Construct a layer generator with no layers, initially.
		hkbLayerGenerator();

	protected:

		hkbLayerGenerator( const hkbLayerGenerator& layerGenerator );

	public:

		~hkbLayerGenerator();

		//////////////////////////
		// hkbBindable interface
		//////////////////////////

			// hkbBindable interface implementation.
		virtual void collectBindables( hkbBindableCollector& collector ) HK_OVERRIDE;

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
		virtual void handleEvent(const hkbContext& context, const hkbEvent e ) HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual bool isValid( const hkbCharacter* character, hkStringPtr& errorString ) const HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual hkbNode* cloneNode( hkbBehaviorGraph& rootBehavior ) const HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual void getActiveEvents( hkPointerMap<int, bool>& activeEvents ) const HK_OVERRIDE;

		///////////////////////////
		// hkbGenerator interface
		///////////////////////////

			// hkbGenerator interface implementation.
		virtual void generate(	const hkbContext& context, 
								const hkbGeneratorOutput** activeChildrenOutput,
								hkbGeneratorOutput& output,
								hkReal timeOffset = 0.0f ) const HK_OVERRIDE;

			// hkbGenerator interface implementation.
		virtual bool canRecycleOutput() const HK_OVERRIDE;

			// hkbGenerator interface implementation.
		virtual void updateSync( const hkbContext& context ) HK_OVERRIDE;

			// hkbGenerator interface implementation.
		virtual void setLocalTime( const hkbContext& context, hkReal seconds ) HK_OVERRIDE;

			// hkbGenerator interface implementation.
		virtual void startEcho( hkReal echoDuration ) HK_OVERRIDE;

	public:

			/// Get the number of child generators.
		int getNumGenerators() const;

			/// Get a child generator.
		hkbGenerator* getGenerator( int generatorIndex ) const;

			/// Set a child generator.  
			/// 
			/// Decrements the reference count on the discarded generator and
			/// increments the reference count on the passed in generator.
		void setGenerator( int generatorIndex, hkbGenerator* generator );

			/// Set the weight associated with a child generator (must be between 0 and 1 inclusive).
			/// 
			/// The weight must be between 0 and 1 inclusive.  The default is 1.
			/// When doing the parametric blend the weights can be any number.
		void setGeneratorWeight( int generatorIndex, hkReal weight );

			/// Set the weight associated with a bone and a generator.
			/// 
			/// The weight must be between 0 and 1 inclusive.  The default is 1.
		void setBoneWeight( int generatorIndex, int boneIndex, hkReal boneWeight );

			/// Get the weight associated with a bone and a generator.
			/// 
			/// The weight must be between 0 and 1 inclusive.  Returns 1 if there are no bone weights
		hkReal getBoneWeight( int generatorIndex, int boneIndex ) const;

		///////////////
		// properties
		///////////////

		enum LayerFlagBits
		{
			/// Adjusts the speed of the children so that their cycles align (default: false).
			FLAG_SYNC = 0x1,
		};

		typedef hkFlags<LayerFlagBits, hkUint16> LayerFlags;

			/// The layers and their info.  This is exposed publically for HAT.  You are
			/// better off calling setGenerator() to add layer.
		hkArray<hkbLayer*> m_layers; //+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_NOT_VARIABLE")

		/// If you want a particular child's duration to be used to sync all of the other children,
		/// set this to the index of the child.  Otherwise, set it to -1.
		hkInt16 m_indexOfSyncMasterChild; //+default(-1)+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_HIDDEN")

		/// The flags affecting specialized behavior.
		LayerFlags m_flags; //+default(0)+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_HIDDEN")

		struct LayerInternalState
		{
			HK_DECLARE_REFLECTION();
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbLayerGenerator::LayerInternalState );
			HK_DECLARE_POD_TYPE();

			LayerInternalState() {}
			LayerInternalState( hkFinishLoadedObjectFlag flag ) {}

			/// The blend weight of this layer
			hkReal m_weight;

			/// Time elapsed since the layer was switched on/off.
			hkReal m_timeElapsed;

			/// The on fraction for the layer. If this is 0, then the layer has been "faded out", 1 then it has been "faded in"
			/// The weight overall layer weight (hkbLayer::m_weight) can be one, but if this is 0, the layer will be off.
			hkReal m_onFraction;

			enum FadingState
			{
				/// The layer is neither fading in nor fading out.	
				FADING_STATE_NONE,

				/// The layer is fading in.
				FADING_STATE_IN,

				/// The layer is fading out.
				FADING_STATE_OUT,
			};

			/// Whether the layer is fading in or out.
			hkEnum<FadingState,hkInt8> m_fadingState;

			
			hkBool m_useMotion;

			/// whether to do an initial sync on this child next frame
			hkBool m_syncNextFrame;

			/// Whether the state is active. A state can be active with 0 weight, but no state with weight > 0 can be inactive.
			hkBool m_isActive;
		};

	private:

		HKB_BEGIN_INTERNAL_STATE(0);

			// The number of active layers.
		int m_numActiveLayers; //+nosave

		hkArray<hkbLayerGenerator::LayerInternalState> m_layerInternalStates; //+nosave

		// Whether to start synching on the next updateSync().
		hkBool m_initSync; //+nosave

		HKB_END_INTERNAL_STATE();
	
	public:

			/// Internal implementation.
		hkbLayer* getLayer( int index ) const { return m_layers[index]; }

	public:

		hkbLayerGenerator( hkFinishLoadedObjectFlag flag );

		friend class hkbLayerGeneratorUtils;
		friend class hkbSpuBehaviorUtils;
};

#include <Behavior/Behavior/Generator/Blender/Layer/hkbLayerGenerator.inl>

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
