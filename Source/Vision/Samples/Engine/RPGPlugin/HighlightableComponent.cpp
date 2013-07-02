/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#include <Vision/Samples/Engine/RPGPlugin/Precompiled.h>

#include <Vision/Samples/Engine/RPGPlugin/HighlightableComponent.h>
#include <Vision/Samples/Engine/RPGPlugin/HighlightableComponentManager.h>

RPG_HighlightableComponent::RPG_HighlightableComponent()
: RPG_BaseComponent(),
m_color(255, 0, 255),
m_flashColor(0, 0, 0),
m_flashStartTime(-1.f),
m_flashDuration(1.0f),
m_flashing(false)
{
}

void RPG_HighlightableComponent::SetColor(VColorRef const& color)
{
  m_color = color;
}

VColorRef const& RPG_HighlightableComponent::GetColor() const
{
  if (m_flashing)
    return m_flashColor;

  return m_color;
}

void RPG_HighlightableComponent::Flash(VColorRef const& color, float const duration /*= 1.0f*/)
{
  m_flashStartTime = Vision::GetTimer()->GetTime();
  m_flashDuration = duration;
  m_flashColor = color;
}

void RPG_HighlightableComponent::Update(float time)
{
  float timediff = time - m_flashStartTime;
  m_flashing = timediff < m_flashDuration;
  if (m_flashing)
  {
    float intensity = (timediff / m_flashDuration);
    m_flashColor.Lerp(m_flashColor, m_color, intensity);
  }
}

BOOL RPG_HighlightableComponent::CanAttachToObject(VisTypedEngineObject_cl *object, VString& errorOut)
{
  if(!RPG_BaseComponent::CanAttachToObject(object, errorOut))
  {
    return FALSE;
  }

  bool const isBaseEntity = (object->IsOfType(V_RUNTIME_CLASS(VisBaseEntity_cl)) == TRUE);
  if(!isBaseEntity)
  {
    errorOut = "VisBaseEntity_cl (or derived) required";
    return FALSE;
  }

  bool const hasHighlightable = (object->Components().GetComponentOfBaseType(V_RUNTIME_CLASS(RPG_HighlightableComponent)) != NULL);
  if(hasHighlightable)
  {
    errorOut = "RPG_HighlightableComponent (or derived) already attached";
    return FALSE;
  }

  return TRUE;
}

void RPG_HighlightableComponent::SetOwner(VisTypedEngineObject_cl *owner)
{
  RPG_BaseComponent::SetOwner(owner);

  if(owner)
  {
    RPG_HighlightableComponentManager::s_instance.GetHighlightables().AddUnique(this);

    static_cast<VisBaseEntity_cl *>(owner)->SetTraceAccuracy(VIS_TRACEACC_AABOX);
    static_cast<VisBaseEntity_cl *>(owner)->SetTracelineBitmask(RPG_HighlightableComponentManager::s_instance.GetTraceBitmask());
  }
  else
  {
    RPG_HighlightableComponentManager::s_instance.GetHighlightables().Remove(this);
  }
}

V_IMPLEMENT_SERIAL(RPG_HighlightableComponent, RPG_BaseComponent, 0, &g_RPGPluginModule);

START_VAR_TABLE(RPG_HighlightableComponent, RPG_BaseComponent, "(RPG) Highlightable", VFORGE_HIDECLASS, "(RPG) Highlightable")  
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
