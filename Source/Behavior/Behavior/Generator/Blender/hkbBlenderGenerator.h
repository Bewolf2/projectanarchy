/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_BLENDER_GENERATOR_H
#define HKB_BLENDER_GENERATOR_H

#include <Behavior/Behavior/Generator/hkbGenerator.h>
#include <Behavior/Behavior/Generator/hkbGeneratorSyncInfo.h>
#include <Behavior/Behavior/Generator/Blender/hkbBlenderGeneratorChild.h>

extern const class hkClass hkbBlenderGeneratorChildInternalStateClass;

extern const class hkClass hkbBlenderGeneratorClass;

struct hkbBlenderGeneratorGenerateSetup;

/// A generator which performs a blend between an arbitrary number of children.  
/// 
/// Each child has a blend weight between 0 and 1.   Each child can also have a blend weight for each bone.
/// The two weights are multiplied to get the per-child per-bone weight.  For each bone, the weights
/// across all of the children are normalized to sum to 1.  If the sum of the weights falls below 
/// the threshold specified in m_referencePoseWeightThreshold, the reference pose is
/// blended in.
///
/// When the FLAG_PARAMETRIC_BLEND flag is set the blender generator works as a parametric blender. The parametric 
/// blend can be cyclic or acyclic. Cyclic and acyclic differs in the way the boundary conditions are handled. 
///
/// During a parametric blend the child generator weights represents the sample points on a line. Based on the value of 
/// m_blendParameter an interval is computed such that the parametric blend lies within this interval and the animations 
/// that bound the interval are blended. There can be multiple animations with the same weight. If that happens then 
/// we have a convention the interval is chosen such that the highest child generator index bounds it on the left side. 
///
/// In case of a acyclic parametric blend whenever the value of m_blendParameter is less than the value of first child
/// then the interval between the first and second child is chosen and when it is greater then the value of its last child
/// then the interval between the second last child and the last child is chosen. The same thing happens in cyclic mode
/// when the first child weight is equal to the m_minBlendParameter and the last child weight is equal to m_maxBlendParameter.
/// If that is not the case then when the m_blendParameter goes beyond the first child and m_minBlendParameter is less than
/// the first child weight or m_blendParameter goes beyond the last child weight and m_maxBlendParameter is greater than
/// the last child weight the interval is between the first and the last children.

class hkbBlenderGenerator : public hkbGenerator
{
	public:
		//+version(1)

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

			/// Construct a blender with no children, initially.
		hkbBlenderGenerator();

	protected:

		hkbBlenderGenerator( const hkbBlenderGenerator& blender );

	public:

		~hkbBlenderGenerator();

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
		virtual void deactivate( const hkbContext& context ) HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual int getMaxNumChildren( GetChildrenFlags flags ) HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual void getChildren( GetChildrenFlags flags, ChildrenInfo& childrenInfo ) HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual bool isValid( const hkbCharacter* character, hkStringPtr& errorString ) const HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual hkbNode* cloneNode( hkbBehaviorGraph& rootBehavior ) const HK_OVERRIDE;

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

			/// Get the weight associated with the worldFromModel of a child generator.
		hkReal getWorldFromModelWeight( int generatorIndex ) const;

			/// Set the weight associated with the worldFromModel of a child generator.
			/// 
			/// The weight must be between 0 and 1 inclusive.  The default is 1.
		void setWorldFromModelWeight( int generatorIndex, hkReal weight );

			/// Set the weight associated with a bone and a generator.
			/// 
			/// The weight must be between 0 and 1 inclusive.  The default is 1.
		void setBoneWeight( int generatorIndex, int boneIndex, hkReal boneWeight );

			/// Get the weight associated with a bone and a generator.
		hkReal getBoneWeight( int generatorIndex, int boneIndex ) const;

		///////////////
		// properties
		///////////////

		enum BlenderFlags
		{
				/// Adjusts the speed of the children so that their cycles align (default: false).
			FLAG_SYNC = 0x1,

				/// Filter the weights using a cubic curve.
				/// 
				/// If true, the generator weights are passed through a cubic filter.
				/// The new weight w is computed as w = w^2 * (-2w + 3).
				/// This makes it so that you can move the weights linearly as
				/// a function of time and get a smooth result.
			FLAG_SMOOTH_GENERATOR_WEIGHTS = 0x4,

				/// If true, a child will not be deactivated when its weight is zero.
			FLAG_DONT_DEACTIVATE_CHILDREN_WITH_ZERO_WEIGHTS = 0x8,

				/// This is a parametric blend
			FLAG_PARAMETRIC_BLEND = 0x10,

				/// If the blend is parametric, setting this makes it cyclic parametric blend.
			FLAG_IS_PARAMETRIC_BLEND_CYCLIC = 0x20,

				/// Force the output pose to be dense by filling in any missing bones with the
				/// reference pose.
			FLAG_FORCE_DENSE_POSE = 0x40,

				/// Blend the motion of additive and subtractive animations.
			FLAG_BLEND_MOTION_OF_ADDITIVE_ANIMATIONS = 0x80,

				/// Use the velocity-based synchronization
			FLAG_USE_VELOCITY_SYNCHRONIZATION = 0x100
		};

			/// If the sum of non-additive generator weights falls below this threshold, the reference pose is blended in.
		hkReal m_referencePoseWeightThreshold; //+default(0.0)+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_HIDDEN")

			/// This value controls the parametric blend. 
		hkReal m_blendParameter; //+default(0.0)+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_HIDDEN")

			/// The minimum value the blend parameter can have when doing a cyclic parametric blend.
		hkReal m_minCyclicBlendParameter; //+default(0.0)+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_HIDDEN")

			/// The maximum value the blend parameter can have when doing a cyclic parametric blend.
		hkReal m_maxCyclicBlendParameter; //+default(1.0)+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_HIDDEN")

			/// If you want a particular child's duration to be used to sync all of the other children,
			/// set this to the index of the child.  Otherwise, set it to -1.
		hkInt16 m_indexOfSyncMasterChild; //+default(-1)+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_HIDDEN")

			/// The flags affecting specialized behavior.
		hkInt16 m_flags; //+default(0)+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_HIDDEN")

			/// If this is set to true then the last child will be a subtracted from the blend of the rest
		hkBool m_subtractLastChild; //+default(false)

			/// The children and their info.  This is exposed publically for HAT.  You are
			/// better off calling setGenerator() to add children.
		hkArray<hkbBlenderGeneratorChild*> m_children; //+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_NOT_VARIABLE")

		struct ChildInternalState
		{
			HK_DECLARE_REFLECTION();
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbBlenderGenerator::ChildInternalState );
			HK_DECLARE_POD_TYPE();

			ChildInternalState() {}
			ChildInternalState( hkFinishLoadedObjectFlag flag ) {}

				// whether or not m_weight was non-zero on last update()
			hkBool m_isActive;

				// whether to do an initial sync on this child next frame
			hkBool m_syncNextFrame;
		};

	protected:

		HKB_BEGIN_INTERNAL_STATE(0);

			// Information about the state of each child.
		hkArray<struct hkbBlenderGenerator::ChildInternalState> m_childrenInternalStates;	//+nosave

			// The indices of the children sorted according to weight.
		hkArray<hkInt16> m_sortedChildren; //+nosave

			// During a parametric blend we compute between which two sample points(animations) 
			// does the m_blendParameter lie and store the weight associated with the end sample point.
		hkReal m_endIntervalWeight; //+nosave

			// The number of active children (those with nonzero weight).
		int m_numActiveChildren; //+nosave

			// When parametric blending, this is the index of the child at the beginning of the current interval.
		hkInt16 m_beginIntervalIndex; //+nosave

			// When parametric blending, this is the index of the child at the end of the current interval.
		hkInt16 m_endIntervalIndex; //+nosave

			// Whether to start synching on the next updateSync().
		hkBool m_initSync; //+nosave

			// Whether you want to subtract the last child or not.
		hkBool m_doSubtractiveBlend;  //+nosave

		HKB_END_INTERNAL_STATE();

	public:

			/// Internal implementation.
		hkbBlenderGeneratorChild* getChild( int index ) const { return m_children[index]; }

	private:

			// Set the m_active flag on all of the children.  Returns whether or not any child's active flag changed.
		bool updateChildrenActiveFlags();

			// This computes the end interval index and its weight.
		void computeEndInterval();			

	public:

		hkbBlenderGenerator( hkFinishLoadedObjectFlag flag );

		friend class hkbCpuBehaviorJob;
		friend class hkbCpuUtils;
		friend class hkbBlenderGeneratorUtils;
		friend class hkbSpuBehaviorUtils;
};

#include <Behavior/Behavior/Generator/Blender/hkbBlenderGenerator.inl>

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
