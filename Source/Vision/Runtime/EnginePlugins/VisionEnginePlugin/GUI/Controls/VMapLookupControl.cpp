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

VMapLookupControl::VMapLookupControl()
{
  m_MatchItems.m_pOwner = this;
}


bool VLookupItem::Build(TiXmlElement *pNode, const char *szPath, bool bWrite)
{
  if (!VListControlItem::Build(pNode,szPath,bWrite))
    return false;
  XMLHelper::Exchange_Color(pNode,"matchcolor", m_iMatchColor,bWrite);
  return true;
}


V_IMPLEMENT_SERIAL( VLookupItem, VListControlItem, 0, &g_VisionEngineModule );
void VLookupItem::Serialize( VArchive &ar )
{
  char iLocalVersion = 0;
  VListControlItem::Serialize(ar);
  if (ar.IsLoading())
  {
    ar >> iLocalVersion; VASSERT_MSG(iLocalVersion==0,"Invalid version");
    ar >> m_iMatchColor;
  } else
  {
    ar << iLocalVersion;
    ar << m_iMatchColor;
  }
}


V_IMPLEMENT_SERIAL( VMapLookupControl, VDlgControlBase, 0, &g_VisionEngineModule );
void VMapLookupControl::Serialize( VArchive &ar )
{
  char iLocalVersion = 0;
  VDlgControlBase::Serialize(ar);
  if (ar.IsLoading())
  {
    ar >> iLocalVersion; VASSERT_MSG(iLocalVersion==0,"Invalid version");
    ar >> m_Image;
    m_spLookup = (VisBitmap_cl *)ar.ReadProxyObject();
    m_MatchItems.SerializeX(ar);
  } else
  {
    ar << iLocalVersion;
    ar << m_Image;
    ar.WriteProxyObject(m_spLookup);
    m_MatchItems.SerializeX(ar);
  }
}


void VMapLookupControl::OnPaint(VGraphicsInfo &Graphics, const VItemRenderInfo &parentState)
{
  VItemRenderInfo thisState(parentState,this,1.f);
  m_Image.OnPaint(Graphics,thisState);
}

bool VMapLookupControl::Build(TiXmlElement *pNode, const char *szPath, bool bWrite)
{
  if (!VDlgControlBase::Build(pNode,szPath,bWrite))
    return false;

  // display image
  m_Image.Build(this,XMLHelper::SubNode(pNode,"image",bWrite),szPath,bWrite);

  // size from texture
  if (m_vSize.x<=0.f)
    m_vSize = m_Image.m_States[VDlgControlBase::NORMAL].GetSize();


  // table of matching sub items : iterate through all "item" elements
  TiXmlElement *pTableNode = XMLHelper::SubNode(pNode,"matchtable",bWrite);
  if (pTableNode)
  {
    // lookup texture
    const char *szLookup = XMLHelper::Exchange_String(pTableNode,"map",NULL,bWrite);
    if (szLookup)
    {
      char szPathname[FS_MAX_PATH];
      VFileHelper::CombineDirAndFile(szPathname,szPath,szLookup);
      m_spLookup = VisBitmap_cl::LoadBitmapFromFile(szPathname);
    }

    // matching items
    for (TiXmlElement *pItemNode=pTableNode->FirstChildElement("item"); pItemNode; pItemNode=pItemNode->NextSiblingElement("item") )
    {
      VLookupItem *pItem = new VLookupItem();
      pItem->SetParent(m_pOwner);
      pItem->Build(pItemNode,szPath,bWrite);
      pItem->OnBuildFinished();
      m_MatchItems.Add(pItem,-1,false);
    }
  }  

  return true;
}



VCursor *VMapLookupControl::GetMouseOverCursor(VGUIUserInfo_t &user) 
{
  if (m_spMouseOverItem)
    return m_spMouseOverItem->GetMouseOverCursor(user);
  return m_Image.m_States[GetCurrentState()].GetCursor();
}



VWindowBase* VMapLookupControl::TestMouseOver(VGUIUserInfo_t &user, const hkvVec2 &vAbsMouse)
{
  if (!VDlgControlBase::TestMouseOver(user, vAbsMouse)) // outside bounding box
    return NULL;

  if (!m_spLookup)
    return this;

  // map coordinates to image size range
  hkvVec2 vRelPos = vAbsMouse-GetAbsPosition();
  int x = (int)((float)m_spLookup->GetWidth()*vRelPos.x/m_vSize.x);
  int y = (int)((float)m_spLookup->GetHeight()*vRelPos.y/m_vSize.y);
  VColorRef iLookupColor = m_spLookup->LookupTexelColor(x,y);

  // find the lookup item
  VWindowBase* pReturnItem = this;
  m_spMouseOverItem = NULL;
  for (int i=0;i<m_MatchItems.Count();i++)
  {
    VLookupItem *pItem = (VLookupItem *)m_MatchItems.GetAt(i);
    if (pItem->m_iMatchColor==iLookupColor)
    {
      m_spMouseOverItem = pItem;
      pReturnItem = pItem;
      break;
    }
  }
  // send item changed message?

  return pReturnItem;
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
