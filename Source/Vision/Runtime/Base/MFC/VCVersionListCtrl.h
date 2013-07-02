/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#if !defined(AFX_VCVERSIONLISTCTRL_H__E7048C1F_9A0C_4C60_B47C_06211173457F__INCLUDED_)
#define AFX_VCVERSIONLISTCTRL_H__E7048C1F_9A0C_4C60_B47C_06211173457F__INCLUDED_

#if defined( _MSC_VER) && _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef _VISION_DOC

#include <Vision/Runtime/Base/System/ModuleSystem/VModuleInfo.hpp>
#include <Vision/Runtime/Base/System/ModuleSystem/VModuleInfoList.hpp>
#include <Vision/Runtime/Base/System/IO/Clipboard/VClipboardHelper.hpp>


//   This list control displays a list of modules.
// 
// The list controls displays the module name, version number and path of every module used by the
// current process.
// 
// Create an instance in a dialog. Thats all! The control fills itself during OnCreate and it
// supports CTRL+C.
//
// This class is currently still used in the vModelViewer application (2010-04-07).
class VCVersionListCtrl : public CListCtrl
{
// Construction
public:
  VCVersionListCtrl() {}

// Attributes
public:

// Operations
public:

// Overrides

  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(VCVersionListCtrl)
  public:
  virtual BOOL PreTranslateMessage(MSG* pMsg);
  //}}AFX_VIRTUAL

// Implementation
public:
  virtual ~VCVersionListCtrl() {}

  //   Fills the control with the module data
  void Fill();

  // Generated message map functions
protected:
  //{{AFX_MSG(VCVersionListCtrl)
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
  afx_msg void OnColumnClick(NMHDR* pNMHDR, LRESULT* pResult);
  //}}AFX_MSG


  
  int m_iSortOrder; ///< 0 = by modulename, 1 = version, 2 = path
  VModuleInfoList m_ModuleList; ///< module list object from which this objects gets all the information

  
  //   Callbackfunction to sort the items in the list
  static int CALLBACK CompareFunction(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
  
  
  DECLARE_MESSAGE_MAP()
};



BEGIN_MESSAGE_MAP(VCVersionListCtrl, CListCtrl)
  //{{AFX_MSG_MAP(VCVersionListCtrl)
  ON_WM_CREATE()
  ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnClick)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()


// ---------------------------------------------------------------------------------
// Method: OnCreate
// Author: Gerd Zanker
// Notes: 
// ---------------------------------------------------------------------------------
inline int VCVersionListCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
  if (CListCtrl::OnCreate(lpCreateStruct) == -1)
    return -1;
  
  m_iSortOrder = 0;

  // add headers
  InsertColumn(1, "Module", LVCFMT_LEFT, 100, -1);
  InsertColumn(2, "Version", LVCFMT_LEFT, 100, 1);
  InsertColumn(3, "Path", LVCFMT_LEFT, 250, 2);
  
  m_ModuleList.Fill();
  Fill();
  
  SortItems(VCVersionListCtrl::CompareFunction, (DWORD) this);
  
  return 0;
}


// ---------------------------------------------------------------------------------
// Method: Fill
// Author: Gerd Zanker
// Notes: 
// ---------------------------------------------------------------------------------
inline void VCVersionListCtrl::Fill()
{
  DeleteAllItems(); ///< clear old items

  int count = m_ModuleList.GetLength();
  if(count==0)
  {
    InsertItem(0, "<Not possible to fill list>");
    return;
  }

  for(int i=0; i<count; i++)
  {
    const VModuleInfo *pModule = m_ModuleList.GetModule(i);
    InsertItem(i, pModule->m_szModuleName);
    SetItemText(i, 1, pModule->m_szVersion);
    SetItemText(i, 2, pModule->m_szPath);
    SetItemData(i, (DWORD) pModule);
  }
}


// ---------------------------------------------------------------------------------
// Method: OnColumnClick
// Author: Gerd Zanker
// Notes: 
// ---------------------------------------------------------------------------------
inline void VCVersionListCtrl::OnColumnClick(NMHDR* pNMHDR, LRESULT* pResult) 
{
  NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
  
  // use different sorting for different columns
  // The iItem member is -1, and the iSubItem member identifies the column. All other members are zero. 
  if(pNMListView->iSubItem == 0)
  {
    // sort Modules name
    m_iSortOrder = 0;
  }
  else if(pNMListView->iSubItem == 1)
  {
    // sort version
    m_iSortOrder = 1;
  }
  else if(pNMListView->iSubItem == 2)
  {
    // sort path
    m_iSortOrder = 2;
  }
  
  SortItems(VCVersionListCtrl::CompareFunction, (DWORD) this);
  
  *pResult = 0;
}


// ---------------------------------------------------------------------------------
// Method: CompareFunction
// Author: Gerd Zanker
// Notes: 
// ---------------------------------------------------------------------------------
inline int CALLBACK VCVersionListCtrl::CompareFunction(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
  // lParamSort contains a pointer to the list view control.
  // The lParam of an item is a pointer to the VModule object
  VCVersionListCtrl* pListCtrl = (VCVersionListCtrl*) lParamSort;

  VModuleInfo* pMod1 = (VModuleInfo*) lParam1;
  VModuleInfo* pMod2 = (VModuleInfo*) lParam2;
  
  char* strItem1;
  char* strItem2;

  if(pListCtrl->m_iSortOrder == 1)
  {
    strItem1 = pMod1->m_szVersion;
    strItem2 = pMod2->m_szVersion;
  }
  else if(pListCtrl->m_iSortOrder == 2)
  {
    strItem1 = pMod1->m_szPath;
    strItem2 = pMod2->m_szPath;
  }
  else
  {
    strItem1 = pMod1->m_szModuleName;
    strItem2 = pMod2->m_szModuleName;
  }
  
  return strcmp(strItem1, strItem2);
}



// ---------------------------------------------------------------------------------
// Method: PreTranslateMessage
// Author: Gerd Zanker
// Notes: Handels CTRL+C
// ---------------------------------------------------------------------------------
inline BOOL VCVersionListCtrl::PreTranslateMessage(MSG* pMsg) 
{
  if (pMsg->message == WM_CHAR)
  {
    if (pMsg->wParam == 3) ///< this is ctrl-c (ctrl-a..ctrl-z is 1..26)
    {
      VClipBoardHelper::SetText(m_ModuleList.AsString());
      return TRUE;
    }
  }
  
  return CListCtrl::PreTranslateMessage(pMsg);
}

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // _VISION_DOC


#endif // !defined(AFX_VCVERSIONLISTCTRL_H__E7048C1F_9A0C_4C60_B47C_06211173457F__INCLUDED_)

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
