/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_SCRIPT_GENERATOR_H
#define HKB_SCRIPT_GENERATOR_H

#include <Behavior/Behavior/Generator/hkbGenerator.h>

/// A generator that can be customized using script.
class hkbScriptGenerator : public hkbGenerator
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

		hkbScriptGenerator();
		hkbScriptGenerator( const hkbScriptGenerator& that );
		hkbScriptGenerator( hkFinishLoadedObjectFlag flag );

		//////////////////////
		// hkbNode interface 
		//////////////////////

			// hkbNode interface implementation.
		virtual void activate( const hkbContext& context ) HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual void deactivate( const hkbContext& context ) HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual void handleEvent( const hkbContext& context, const hkbEvent e ) HK_OVERRIDE;

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
		virtual void updateSync( const hkbContext& context ) HK_OVERRIDE;

			// hkbGenerator interface implementation.
		virtual bool canRecycleOutput() const HK_OVERRIDE;

			// hkbGenerator interface implementation.
		virtual void preUpdate( const hkbContext& context, hkReal timestep ) HK_OVERRIDE;

	public:

			/// Returns whether the a generate script is present or not.
		hkReal hasGenerateScript() { return m_onGenerateScript.getLength() != 0; }

	public:

		///////////////
		// Properties
		///////////////

			/// The child node.
		hkRefPtr<hkbGenerator> m_child;	//+hk.Description("The child node.")

			/// The script code to call on activate.
		hkStringPtr m_onActivateScript;		//+hk.Description("The script code to call on activate.")
											//+hkb.RoleAttribute("ROLE_SCRIPT","FLAG_NOT_VARIABLE|FLAG_NOT_CHARACTER_PROPERTY")
		
			/// The script code to call on preUpdate.
		hkStringPtr m_onPreUpdateScript;	//+hk.Description("The script code to call on preUpdate.")
											//+hkb.RoleAttribute("ROLE_SCRIPT","FLAG_NOT_VARIABLE|FLAG_NOT_CHARACTER_PROPERTY")

			/// The script code to call on generate.
		hkStringPtr m_onGenerateScript;		//+hk.Description("The script code to call on generate.")
											//+hkb.RoleAttribute("ROLE_SCRIPT","FLAG_NOT_VARIABLE|FLAG_NOT_CHARACTER_PROPERTY")
		
			/// The script code to call on handle event.
		hkStringPtr m_onHandleEventScript;	//+hk.Description("The script code to call when an event is raised.")
											//+hkb.RoleAttribute("ROLE_SCRIPT","FLAG_NOT_VARIABLE|FLAG_NOT_CHARACTER_PROPERTY")

			/// The script code to call on deactivate.
		hkStringPtr m_onDeactivateScript;	//+hk.Description("The script code to call on deactivate.")
											//+hkb.RoleAttribute("ROLE_SCRIPT","FLAG_NOT_VARIABLE|FLAG_NOT_CHARACTER_PROPERTY")

	private:

			/// Reference Ids to look up precompiled scripts in the lua registry
		int m_refOnActivate;	//+nosave
		int m_refOnPreUpdate;	//+nosave		
		int m_refOnGenerate;	//+nosave
		int m_refOnHandleEvent;	//+nosave
		int m_refOnDeactivate;	//+nosave

			/// The timestep used in the last call to update.  It is provided to the next call to generate.
		hkReal m_timeStep;	//+nosave
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
