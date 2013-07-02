/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/HavokPhysicsEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokVisualDebugger.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokPhysicsIncludes.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokConversionUtils.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokPhysicsModule.hpp>
#include <Common/Base/Thread/Job/ThreadPool/hkJobThreadPool.h>

#pragma managed(push, off)

#include <Common/Visualize/hkProcessRegisterUtil.h>	
#include <Physics2012/Dynamics/World/hkpWorld.h>
	
#pragma managed(pop)


// --------------------------------------------------------------------------
// HAVOK VISUAL DEBUGGER ACTION
// --------------------------------------------------------------------------
V_IMPLEMENT_ACTION("HavokVisualDebugger",vHavokVisualDebuggerAction,VAction,&g_vHavokModule,"[<on/ off]")

VBool vHavokVisualDebuggerAction::Do(const class VArgList &argList)
{
  if (argList.GetArgCount()==1 && argList.IsString(1)) 
  {
    vHavokPhysicsModule *pMod = vHavokPhysicsModule::GetInstance();
    if (!pMod)
    {
      PrintWarning("Could not retrieve an instance of Havok Physics module.");
      return FALSE;
    }

    const char *szArg = argList.AsString(1);
    if (_stricmp(szArg, "on")==0)
    {
      pMod->SetEnabledVisualDebugger(TRUE);
      Print("Enabled Havok Visual Debugger.");
      return TRUE;
    }
    else if (_stricmp(szArg, "off")==0)
    {
      pMod->SetEnabledVisualDebugger(FALSE);
      Print("Disabled Havok Visual Debugger.");
      return TRUE;
    }
    else
      PrintWarning("Invalid argument. Valid arguments are: [on] for enabling and [off] for disabling.");

    return FALSE;
  }
   
  PrintWarning("Invalid argument. Valid arguments are: [on] for enabling and [off] for disabling.");
  return FALSE;
}


// Static callbacks
VisCallback_cl vHavokVisualDebugger::OnCreatingContexts;
VisCallback_cl vHavokVisualDebugger::OnAddingDefaultViewers;


vHavokVisualDebuggerCallbackData_cl::vHavokVisualDebuggerCallbackData_cl(VisCallback_cl* pSender, hkVisualDebugger* vdb, hkArray<hkProcessContext*>* contexts )
{
	m_pVisualDebugger = vdb;
	m_contexts = contexts;
}

// --------------------------------------------------------------------------
// HAVOK VISUAL DEBUGGER
// --------------------------------------------------------------------------

vHavokVisualDebugger::vHavokVisualDebugger(hkpWorld* pPhysicsWorld)
{
  hkArray<hkProcessContext*> contexts;
  hkProcessRegisterUtil::registerAllCommonProcesses();

  // Initialize the visual debugger so we can connect remotely to the simulation
  // The context must exist beyond the use of the VDB instance, and you can make
  // whatever contexts you like for your own viewer types.
  m_physicsContext = new hkpPhysicsContext;
  hkpPhysicsContext::registerAllPhysicsProcesses();			// all the physics viewers
  m_physicsContext->addWorld(pPhysicsWorld);                // add the physics world so the viewers can see it

  // Allow others to add contexts to the list before we initialize the visual debugger
  vHavokVisualDebuggerCallbackData_cl vdbContextCreateCallbackData(&vHavokVisualDebugger::OnCreatingContexts, m_pVisualDebugger, &contexts);
  vHavokVisualDebugger::OnCreatingContexts.TriggerCallbacks(&vdbContextCreateCallbackData);

  // Setup the visual debugger
  contexts.pushBack(m_physicsContext);
  m_pVisualDebugger = new hkVisualDebugger(contexts);

  // Allow others to register default viewers
  vHavokVisualDebuggerCallbackData_cl vdbAddDefaultViewerCallbackData(&vHavokVisualDebugger::OnAddingDefaultViewers, m_pVisualDebugger, &contexts);
  vHavokVisualDebugger::OnAddingDefaultViewers.TriggerCallbacks(&vdbAddDefaultViewerCallbackData);

  m_pVisualDebugger->serve();

  // Allocate memory for internal profiling information
  // You can discard this if you do not want Havok profiling information
  hkMonitorStream& stream = hkMonitorStream::getInstance();
  stream.resize( HKVIS_MONITOR_STREAM_BYTES_PER_THREAD );  
  stream.reset();
}

vHavokVisualDebugger::~vHavokVisualDebugger()
{
    // Delete the context after you have finished using the VDB
  hkpWorld* world = (m_physicsContext->getNumWorlds() > 0) ? m_physicsContext->getWorld(0) : HK_NULL;
  if (world) world->markForWrite();
  
  // Clean up Havok Visual Debugger
  m_pVisualDebugger->shutdown();
  m_pVisualDebugger->removeReference();

  m_physicsContext->removeReference();

  if (world) world->unmarkForWrite();

}

void vHavokVisualDebugger::Step()
{
  // Get the local timers to feed to the stats viewer
  // reset our VDB stats list
  if (m_physicsContext)
  {
    m_physicsContext->syncTimers(vHavokPhysicsModule::GetInstance()->GetThreadPool());
  }

  hkReal dt = Vision::GetTimer()->GetTimeDifference();

  // Step the debugger
  m_pVisualDebugger->step(dt*hkReal(1000));

  // Update camera
  hkVector4 pos; 
  vHavokConversionUtils::VisVecToPhysVecWorld(Vision::Camera.GetMainCamera()->GetPosition(),pos);
  hkVector4 dir; 
  vHavokConversionUtils::VisVecToPhysVec_noscale(Vision::Camera.GetMainCamera()->GetDirection(),dir);
  hkVector4 up;
  vHavokConversionUtils::VisVecToPhysVec_noscale(Vision::Camera.GetMainCamera()->GetObjDir_Up(), up);

  hkVector4 lookat;
  lookat.setAdd(pos,dir);

  // Havok visual debugger defaults
  float nearPlane = 0.1f;
  float farPlane = 500.f;
  float fovX = 0.f;
  float fovY = 45.f;


  VisRenderContext_cl::GetMainRenderContext()->GetClipPlanes(nearPlane, farPlane);
  VisRenderContext_cl::GetMainRenderContext()->GetFinalFOV(fovX, fovY);

  // Scale them to Havok space
  nearPlane = float(VIS2HK_FLOAT_SCALED(nearPlane));
  farPlane = float(VIS2HK_FLOAT_SCALED(farPlane));

  HK_UPDATE_CAMERA(pos, lookat, up, nearPlane, farPlane, fovY, "Vision");

  // Reset internal profiling info for next frame
  hkMonitorStream::getInstance().reset();
}

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
