/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Animation/Transition/IVTransitionEventTrigger.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Animation/Transition/VTransitionStateMachine.hpp>
#include <Vision/Runtime/Base/System/Memory/VMemDbg.hpp>

/// =========================================================================== ///
/// IVTransitionEventTrigger Component Methods                                   ///
/// =========================================================================== ///

// Register the class in the engine module so it is available for RTTI
V_IMPLEMENT_SERIAL(IVTransitionEventTrigger, IVAnimationEventTrigger, 0, &g_VisionEngineModule);

IVTransitionEventTrigger::IVTransitionEventTrigger(int iComponentFlags) : IVAnimationEventTrigger(iComponentFlags)
{
}

/// =========================================================================== ///
/// IVTransitionEventTrigger Overridable                                         ///
/// =========================================================================== ///

bool IVTransitionEventTrigger::CommonInit()
{
  // Initialize base component
  if (!IVAnimationEventTrigger::CommonInit())
    return false;

  // Get owner entity
  VisBaseEntity_cl *pEntity = (VisBaseEntity_cl *)m_pOwner;
  if (pEntity == NULL)
    return false;

  // Check if there is a transition state machine component and register this instance as an
  // event listener. If no TSM is present do not return false as the base class might still 
  // have been initialized successfully listening to animation events triggered by the Vision
  // animation system.
  VTransitionStateMachine* tsm = static_cast<VTransitionStateMachine*> (pEntity->Components().GetComponentOfType(V_RUNTIME_CLASS(VTransitionStateMachine)));
  if (tsm != NULL)
  {
    tsm->AddEventListener(this);
    tsm->SetForwardingAnimEvents(true);
  }
  
  return true;
}

/// =========================================================================== ///
/// VTypedObject Overrides                                                      ///
/// =========================================================================== ///

void IVTransitionEventTrigger::MessageFunction( int iID, INT_PTR iParamA, INT_PTR iParamB )
{  
  IVAnimationEventTrigger::MessageFunction(iID, iParamA, iParamB);

#ifdef WIN32
  if (iID == VIS_MSG_EDITOR_GETSTANDARDVALUES)
  {
    // Get bone names
    const char *szKey = (const char *)iParamA;
    if (!strcmp(szKey,"Event"))
    {
      // Check for model and skeleton
      VisBaseEntity_cl* pEntity = (VisBaseEntity_cl *)m_pOwner;
      if (pEntity == NULL)
        return;

      // Check if there is a transition state machine component
      VTransitionStateMachine* tsm = static_cast<VTransitionStateMachine*> (pEntity->Components().GetComponentOfType(V_RUNTIME_CLASS(VTransitionStateMachine)));
      if (tsm == NULL)
        return;
            
      // Fill list with event names (first entry is empty)
      VStrList *pDestList = (VStrList*) iParamB;
      int count = tsm->GetTransitionTable()->GetNumSequenceDefs();
      for (int i = 0; i < count; i++)
      {
        int events = tsm->GetTransitionTable()->GetSequenceDefByIndex(i)->GetNumAnimationEvents();
        for (int e = 0; e < events; e++)
        {
          pDestList->AddUniqueString(tsm->GetTransitionTable()->GetSequenceDefByIndex(i)->GetAnimationEventByIndex(e)->GetEventString());
        }
      }
    }
  }
#endif
}

/// =========================================================================== ///
/// Serialization                                                               ///
/// =========================================================================== ///

void IVTransitionEventTrigger::Serialize( VArchive &ar )
{
  // Common Init is called in the base class
  IVAnimationEventTrigger::Serialize(ar); 
}

/// =========================================================================== ///
/// IVTransitionEventTrigger Variable Table                                 ///
/// =========================================================================== ///

START_VAR_TABLE(IVTransitionEventTrigger, IVAnimationEventTrigger, "Transition Event Trigger Component.", VFORGE_HIDECLASS , "Transition Event Trigger")
END_VAR_TABLE

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
