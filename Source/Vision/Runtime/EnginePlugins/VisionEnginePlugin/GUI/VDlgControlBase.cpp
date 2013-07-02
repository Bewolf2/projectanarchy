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


VItemValueChangedEvent::VItemValueChangedEvent(VDlgControlBase *pItem, ValueType_e eType, void *pNewVal, bool bChanging)
: IVisCallbackDataObject_cl(&pItem->GetContext()->OnItemValueChanged)
{
  m_pItem = pItem;
  m_eValueType = eType;
  m_pNewValue = pNewVal;
  m_bChanging = bChanging;
}



VDlgControlBase::VDlgControlBase()
{
}




V_IMPLEMENT_SERIAL( VDlgControlBase, VWindowBase, 0, &g_VisionEngineModule );
void VDlgControlBase::Serialize( VArchive &ar )
{
  VWindowBase::Serialize(ar);
  if (ar.IsLoading())
  {
  } else
  {
  }
}


bool VDlgControlBase::Build(TiXmlElement *pNode, const char *szPath, bool bWrite)
{
  if (!VWindowBase::Build(pNode,szPath,bWrite))
    return false;

  // register the dialog result ID
  const char *szID = XMLHelper::Exchange_String(pNode,"dialogResult",NULL,bWrite);
  if (szID) m_iDialogResult = VGUIManager::GetID(szID);
  
  return true;
}


VDialog * VDlgControlBase::GetParentDialog() const
{
  // find the first owner of type dialog
  VWindowBase *pParent = GetParent();
  VType *pDlgType = V_RUNTIME_CLASS(VDialog);
  while (pParent)
  {
    if (pParent->IsOfType(pDlgType))
      return (VDialog *)pParent;
    pParent = pParent->GetParent();
  }
  return NULL;
}


void VDlgControlBase::OnStatusFlagsChanged(int iOldFlags, int iNewFlags)
{
  VWindowBase::OnStatusFlagsChanged(iOldFlags, iNewFlags);

  // enabled flag switched from 'on' to 'off'?
  if ((iOldFlags&ITEMSTATUS_ENABLED)>0 && (iNewFlags&ITEMSTATUS_ENABLED)==0 && HasFocus())
  {
    // remove focus
    VDialog *pDlg = GetParentDialog();
    if (pDlg!=NULL)
      pDlg->SetFocusItem(NULL);
  }
}



/////////////////////////////////////////////////////////////////////////////////////////////
// VMenuItemCollection class
/////////////////////////////////////////////////////////////////////////////////////////////

void VMenuItemCollection::RenderAll(VGraphicsInfo &Graphics, const VItemRenderInfo &parentState)
{
  // render back to front
  for (int i=0;i<Count();i++)
    if (GetAt(i)->IsVisible())
      GetAt(i)->OnPaint(Graphics,parentState);
}


void VMenuItemCollection::OnTickFunction(float fTimeDelta)
{
  for (int i=0;i<Count();i++)
    GetAt(i)->OnTick(fTimeDelta);
}

  
void VMenuItemCollection::OnActivate(bool bStatus)
{
  for (int i=0;i<Count();i++)
    bStatus ? GetAt(i)->OnActivate() : GetAt(i)->OnDeactivate();
}

void VMenuItemCollection::Refresh()
{
  for (int i=0;i<Count();i++)
    GetAt(i)->Refresh();
}

void VMenuItemCollection::OnParentChanged(int iFlags)
{
  for (int i=0;i<Count();i++)
    GetAt(i)->OnParentChanged(iFlags);
}


VDlgControlBase *VMenuItemCollection::FindItem(int iID)
{
  for (int i=0;i<Count();i++)
    if (GetAt(i)->GetID()==iID)
      return GetAt(i);
  return NULL;
}

VWindowBase *VMenuItemCollection::GetMouseOverItem(VGUIUserInfo_t &user) const
{
  const int iCount = Count();
  int iMaxPrior=0;
  VWindowBase *pBestItem = NULL;
  for (int i=0;i<iCount;i++)
  {
    VDlgControlBase *pItem = GetAt(i);
    if (!pItem->IsEnabled() || !pItem->IsVisible())
      continue;
    VWindowBase *pHitItem = pItem->TestMouseOver(user, user.m_vMousePos);
    if (!pHitItem)
      continue;
    if (!pBestItem || pHitItem->m_iOrder>=iMaxPrior) // take the last one to match render order
    {
      iMaxPrior = pHitItem->m_iOrder;
      pBestItem = pHitItem;
    }
  }
  return pBestItem;
}


int VMenuItemCollection::CompareItemPriority( const void *arg1, const void *arg2)
{
  VDlgControlBase *pElem1 = *(VDlgControlBase **)arg1;
  VDlgControlBase *pElem2 = *(VDlgControlBase **)arg2;
  if (pElem1->m_iOrder>pElem2->m_iOrder) return 1;
  if (pElem1->m_iOrder<pElem2->m_iOrder) return -1;
  return 0;
}


void VMenuItemCollection::SortByPriority()
{
  qsort(GetPtrs(),Count(),sizeof(VDlgControlBase *),CompareItemPriority);
}



int VMenuItemCollection::GetItemIndexWithTabOrder(int iOrder)
{
  for (int i=0;i<Count();i++)
    if (GetAt(i)->GetTabOrder()==iOrder)
      return i;
  return -1;
}


int VMenuItemCollection::GetNextItemIndex(int iCurrentIndex)
{
  const int iCount = Count();
  if (!iCount)
    return -1;
  if (iCurrentIndex<0)
    return GetItemIndexWithTabOrder(0);

  int iThisTab = GetAt(iCurrentIndex)->GetTabOrder();
  int iIndex;
  iIndex = GetItemIndexWithTabOrder(iThisTab+1);
  if (iIndex>=0) return iIndex;
  return GetItemIndexWithTabOrder(0);
}


int VMenuItemCollection::GetPrevItemIndex(int iCurrentIndex)
{
  const int iCount = Count();
  if (!iCount || iCurrentIndex<0)
    return -1;

  int iThisTab = GetAt(iCurrentIndex)->GetTabOrder();
  int iIndex;
  if (iThisTab<=0)
  {
    int iMax = -1;
    int iMaxIndex = -1;
    for (int i=0;i<iCount;i++)
      if (GetAt(i)->GetTabOrder()>iMax)
      {
        iMax = GetAt(i)->GetTabOrder();
        iMaxIndex = i;
      }
    return iMaxIndex;
  }

  iIndex = GetItemIndexWithTabOrder(iThisTab-1);
  if (iIndex>=0) return iIndex;
  return -1;
}



bool VMenuItemCollection::Build(VWindowBase *pOwner, TiXmlElement *pNode, const char *szPath, bool bWrite)
{
  // first count
  int iCount = 0;
  for (TiXmlElement *pItemNode=pNode->FirstChildElement("control"); pItemNode; pItemNode=pItemNode->NextSiblingElement("control") )
    iCount++;

  EnsureCapacity(iCount);

  // build all items ("control" nodes)
  for (TiXmlElement *pItemNode=pNode->FirstChildElement("control"); pItemNode; pItemNode=pItemNode->NextSiblingElement("control") )
  {
    const char *szClassName = XMLHelper::Exchange_String(pItemNode,"class",NULL,bWrite);
    VASSERT(szClassName != NULL && szClassName[0]); // must be defined!

    VType *pType = Vision::GetTypeManager()->GetType(szClassName);
    VASSERT(pType && "Control class not available");
    if (!pType)
      continue;
    
    VDlgControlBase *pItem = (VDlgControlBase *)pType->CreateInstance();
    // sanity check
    if (!pItem->IsOfType(Vision::GetTypeManager()->GetType("VDlgControlBase")))
      Vision::Error.FatalError("class '%s' is not derived from base class VDlgControlBase",szClassName);

    pItem->SetParent(pOwner);
    pItem->Build(pItemNode,szPath,bWrite);
    pItem->OnBuildFinished();
    this->Add(pItem);
  }
  return true;
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
