/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>         // precompiled header
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/GUI/VMenuIncludes.hpp>
#include <Vision/Runtime/Base/System/Memory/VMemDbg.hpp>


#define FOREACH_ITEM \
  for (int i=0;i<m_SubItems.Count();i++)\
  {\
    VDlgControlBase *pItem = m_SubItems.GetAt(i);


VItemContainer::VItemContainer()
{
  m_Frame.SetOwner(this);
}


V_IMPLEMENT_SERIAL( VItemContainer, VDlgControlBase, 0, &g_VisionEngineModule );
void VItemContainer::Serialize( VArchive &ar )
{
  char iLocalVersion = 0;
  VDlgControlBase::Serialize(ar);
  if (ar.IsLoading())
  {
    ar >> iLocalVersion; VASSERT_MSG(iLocalVersion==0,"Invalid version");
    ar >> m_Frame;
    m_SubItems.SerializeX(ar);
  } else
  {
    ar << iLocalVersion;
    ar << m_Frame;
    m_SubItems.SerializeX(ar);
  }
}



bool VItemContainer::Build(TiXmlElement *pNode, const char *szPath, bool bWrite)
{
  if (!VDlgControlBase::Build(pNode,szPath,bWrite))
    return false;

  // optionally build frame
  m_Frame.Build(XMLHelper::SubNode(pNode,"frame",bWrite), szPath, bWrite);

  // items defined as sub-nodes
  m_SubItems.Build(this,pNode,szPath,bWrite);

  return true;
}


void VItemContainer::OnPaint(VGraphicsInfo &Graphics, const VItemRenderInfo &parentState)
{
  VDlgControlBase::OnPaint(Graphics,parentState);
  VItemRenderInfo thisState(parentState,this,1.f);

  m_Frame.OnPaint(Graphics,thisState);

  FOREACH_ITEM
    if(pItem->IsVisible())
    {
      pItem->OnPaint(Graphics,thisState);
    }
  }
}



void VItemContainer::OnTick(float dtime)
{
  VDlgControlBase::OnTick(dtime);
  for (int i=0;i<VGUIUserInfo_t::GUIMaxUser;i++)
  {
    VGUIUserInfo_t *pUser = GetContext()->GetUser((VGUIUserInfo_t::VGUIUserID_e)i);
    if (!pUser) continue;
    m_spMouseOverItem[i] = m_SubItems.GetMouseOverItem(*pUser);
  }

  FOREACH_ITEM
    pItem->OnTick(dtime);
  }
}


VWindowBase* VItemContainer::TestMouseOver(VGUIUserInfo_t &user, const hkvVec2 &vAbsMouse)
{
  int i = user.m_iID;
  if (m_spMouseOverItem[i]!=NULL) 
    return m_spMouseOverItem[i];
  return VDlgControlBase::TestMouseOver(user, vAbsMouse);
}

VCursor *VItemContainer::GetMouseOverCursor(VGUIUserInfo_t &user)
{
  int i = user.m_iID;
  if (m_spMouseOverItem[i]!=NULL)
    return m_spMouseOverItem[i]->GetMouseOverCursor(user);

  return VDlgControlBase::GetMouseOverCursor(user);
}

VTooltip* VItemContainer::GetTooltip(VGUIUserInfo_t &user)
{
  int i = user.m_iID;
  if (m_spMouseOverItem[i]!=NULL)
    return m_spMouseOverItem[i]->GetTooltip(user);

  return VDlgControlBase::GetTooltip(user);
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
