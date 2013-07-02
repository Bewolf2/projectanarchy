/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Samples/Engine/RPGPlugin/Precompiled.h>
#include <Vision/Samples/Engine/RPGPlugin/VisionHavokBehaviorWorldListener.h>
#include <Vision/Samples/Engine/RPGPlugin/Character.h>

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/VScriptIncludes.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokBehaviorEnginePlugin/vHavokBehaviorComponent.hpp>

#include <Common/Base/Reflection/Registry/hkVtableClassRegistry.h>

RPG_HavokBehaviorWorldListener RPG_HavokBehaviorWorldListener::s_instance;

RPG_HavokBehaviorWorldListener::RPG_HavokBehaviorWorldListener() :
  m_BehaviorEventNotificationQueueSize(0)
{
}

void RPG_HavokBehaviorWorldListener::SendEnqueuedNotifications()
{
  for (unsigned int i = 0; i < m_BehaviorEventNotificationQueueSize; i++)
  {
    RPG_HavokBehaviorEventInfo const& info = m_BehaviorEventNotificationQueue[i];
    {
      // Get entity from character behavior user data
      vHavokBehaviorComponent *behaviorComponent = (vHavokBehaviorComponent *)info.m_character->m_userData;
      VASSERT(behaviorComponent);
      if (behaviorComponent)
      {
        VisTypedEngineObject_cl *pCompOwner = behaviorComponent->GetOwner();
        if (pCompOwner)
        {
          // TODO: Differentiate event payloads (sounds, particles, etc. though, the particle one does not have name property)
          const hkClass* payloadClass = hkBuiltinTypeRegistry::getInstance().getVtableClassRegistry()->getClassFromVirtualInstance( info.m_event.getPayload() );
          if (payloadClass && payloadClass->equals(&hkbNamedEventPayloadClass))
          {
            const hkbNamedEventPayload* payload = static_cast< const hkbNamedEventPayload* >( info.m_event.getPayload() );
            if (payload->m_name)
            {
              pCompOwner->TriggerScriptEvent("OnHavokBehaviorEvent", "*s", payload->m_name.cString()); 

              if (pCompOwner->IsOfType(V_RUNTIME_CLASS(RPG_Character)))
              {
                RPG_Character* pCharacter = static_cast<RPG_Character*>(pCompOwner);
                VString payloadName(payload->m_name.cString());
                pCharacter->OnHavokBehaviorEvent(payloadName);
              }
            }
          } 
        }
      }
    }
  }

  m_BehaviorEventNotificationQueue.Resize(0);
  m_BehaviorEventNotificationQueueSize = 0;
}

void RPG_HavokBehaviorWorldListener::eventRaisedCallback( hkbCharacter* character, const hkbEvent& behaviorEvent, bool raisedBySdk )
{
  if (behaviorEvent.getPayload() == HK_NULL)
    return;

  RPG_HavokBehaviorEventInfo& info = m_BehaviorEventNotificationQueue[m_BehaviorEventNotificationQueueSize++];
  {
    info.m_character = character;
    info.m_event = behaviorEvent;
  }
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
