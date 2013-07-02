/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file vHavokVisualDebugger.hpp

// ***********************************************************************************************
// vHavok binding for Vision that uses Havok for physics
// Copyright (C) Trinigy GmbH. All rights reserved.
// ***********************************************************************************************
#ifndef VHAVOKVISUALDEBUGGER_HPP_INCLUDED
#define VHAVOKVISUALDEBUGGER_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokPhysicsIncludes.hpp>
#include <Common/Visualize/hkProcess.h>
#include <Common/Visualize/hkProcessFactory.h>

#include <Vision/Runtime/Base/Action/VAction.hpp>

class hkVisualDebugger;
class hkpPhysicsContext;


/// 
/// \brief
///   Console Action for toggling the Havok Visual Debugger.
/// 
class vHavokVisualDebuggerAction : public VAction
{
  V_DECLARE_ACTION(vHavokVisualDebuggerAction);

public:
  VOVERRIDE VBool Do(const class VArgList &argList);
};


/// 
/// \brief
///   Class that holds VDB callback data
/// 
class vHavokVisualDebuggerCallbackData_cl : public IVisCallbackDataObject_cl
{
	public:
		VHAVOK_IMPEXP vHavokVisualDebuggerCallbackData_cl(VisCallback_cl* pSender, hkVisualDebugger* vdb, hkArray<hkProcessContext*>* contexts );

	public:
		hkVisualDebugger* m_pVisualDebugger;
		hkArray<hkProcessContext*>* m_contexts;
};

/// 
/// \brief
///   Class that represents the Havok Visual Debugger
/// 
/// Initializes & Deinitializes the visual debugger so we can connect remotely to the simulation.
/// 
class vHavokVisualDebugger: public VRefCounter 
{
public:
    
  ///
  /// @name Constructor / Destructor
  /// @{
  ///
  

  /// 
  /// \brief
  ///   Constructor of the Havok Visual Debugger
  /// 
  /// Initializes the visual debugger so we can connect remotely to the simulation.
  ///
  /// \param pPhysicsWorld
  ///   Pointer to the Havok Physics World
  /// 
  vHavokVisualDebugger(hkpWorld* pPhysicsWorld);


  /// 
  /// \brief
  ///   Destructor of the Havok Visual Debugger
  /// 
  /// Deinitializes the visual debugger.
  ///
  ~vHavokVisualDebugger();


  ///
  /// @}
  ///


  ///
  /// @name Access to Havok Internals
  /// @{
  ///


  /// 
  /// \brief
  ///   Get internal Havok Visual Debugger.
  /// 
  /// \returns
  ///   Pointer to the visual debugger (or NULL if visual debugger is not initialized).
  /// 
  inline hkVisualDebugger* GetVisualDebugger() { return m_pVisualDebugger; }


  ///
  /// @}
  ///


  ///
  /// @name Stepping
  /// @{
  ///


  /// 
  /// \brief
  ///   Steps the visual debugger. Called once per frame.
  /// 
  VHAVOK_IMPEXP void Step();


  ///
  /// @}
  ///

  /// Static callbacks
  VHAVOK_IMPEXP static VisCallback_cl OnCreatingContexts;
  VHAVOK_IMPEXP static VisCallback_cl OnAddingDefaultViewers;

public:

  hkpPhysicsContext* m_physicsContext;          ///< Physics Context that holds all physics viewer
  hkVisualDebugger* m_pVisualDebugger;    ///< Internal Havok Visual Debugger

};

#endif // VHAVOKVISUALDEBUGGER_HPP_INCLUDED

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
