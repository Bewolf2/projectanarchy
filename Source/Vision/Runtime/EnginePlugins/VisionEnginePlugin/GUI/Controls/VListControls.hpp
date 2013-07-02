/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VListControls.hpp

#ifndef VLISTCONTROLS_HPP_INCLUDED
#define VLISTCONTROLS_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/GUI/Controls/VSliderControl.hpp>

class VListControlItemCollection;
class VListControlItem;
class VSliderControl;
typedef VSmartPtr<VListControlItem> VListControlItemPtr;
typedef VSmartPtr<VSliderControl> VSliderControlPtr;


/// \brief
///   Window that represents a list control item
class VListControlItem : public VWindowBase
{
public:
  GUI_IMPEXP VListControlItem();
  GUI_IMPEXP virtual ~VListControlItem();
  GUI_IMPEXP VListControlItem(const char *szText);

  /// \brief set the text to be displayed
  GUI_IMPEXP virtual void SetText(const char *szText) HKV_OVERRIDE {m_Text.SetText(szText);m_bCachedTextRectValid=false;}

  /// \brief get the text to be displayed
  GUI_IMPEXP virtual const char* GetText() const HKV_OVERRIDE {return m_Text.GetText();}

  /// \brief This function can be overridden to get informed when the parent size is changed.
  GUI_IMPEXP virtual void OnParentSizeChanged() {m_bCachedTextRectValid=false;}

  /// \brief Returns the icon's VImageStates object.
  inline VImageStates& Icon() {return m_Icon;}

  /// \brief in addition to user data, a list item has a data member (e.g. used for enum index). Do not use for pointers
  inline void SetData(int iVal) {m_iData=iVal;}

  /// \brief in addition to user data, a list item has a data member (e.g. used for enum index). Do not use for pointers
  inline int GetData() const {return m_iData;}

  /// \brief determines whether item is visible (inside the control box). Gets calculated in OnMeasureItem
  inline bool IsClipped() const {return m_bClipped;}

  /// \brief determines whether item is visible (inside the control box). Gets calculated in OnMeasureItem
  inline bool IsFullyVisible() const {return m_bFullyVisible;}

  /// \brief Returns this item's index.
  inline int GetIndex() const {return m_iIndex;}

  // VWindowBase overrides
  GUI_IMPEXP virtual void OnPaint(VGraphicsInfo &Graphics, const VItemRenderInfo &parentState) HKV_OVERRIDE;
  GUI_IMPEXP virtual bool Build(TiXmlElement *pNode, const char *szPath, bool bWrite) HKV_OVERRIDE;
  GUI_IMPEXP virtual VCursor *GetMouseOverCursor(VGUIUserInfo_t &user) HKV_OVERRIDE {return m_Text.m_States[GetCurrentState()].GetCursor();}

  /// \brief Updates the item's position. Makes sure the item is positioned on pixel boundaries, or the text would be sampled incorrectly.
  GUI_IMPEXP virtual void OnMeasureItem(const VRectanglef &area, hkvVec2 &vCurrentPos);

  // serialization
  V_DECLARE_SERIAL_DLLEXP( VListControlItem, GUI_IMPEXP_DATA );
  GUI_IMPEXP virtual void Serialize( VArchive &ar );

  // member vars
  int m_iData;
  VTextStates m_Text;
  VImageStates m_Icon;

protected:
  GUI_IMPEXP void CommonInit();
  friend class VListControlItemCollection;

  // temp data:
  bool m_bClipped,m_bFullyVisible,m_bCachedTextRectValid;
  int m_iIndex; ///< enumerated by collection
  VTextStates *m_pDefaultStates;
};


/// \brief
///   Collection of list item in a list control
class VListControlItemCollection : public VRefCountedCollection<VListControlItem>
{
public:
  GUI_IMPEXP VListControlItemCollection() {m_pOwner=NULL;}

  // collection functions
  GUI_IMPEXP void Add(VListControlItem *pItem, int iInsertBefore, bool bSetDefaultStates);
  inline VListControlItem* GetAtSafe(int iIndex) const {if (iIndex<0 || iIndex>=Count()) return NULL;return GetAt(iIndex);}
  GUI_IMPEXP VListControlItem *FindItemByDataValue(int iValue) const;
  GUI_IMPEXP VListControlItem *FindItemByUserData(const void *pData) const;

  // internal functions
  GUI_IMPEXP void MeasureItems(const VRectanglef &area, hkvVec2 &vStartPos);
  GUI_IMPEXP void PaintAll(VGraphicsInfo &Graphics, const VItemRenderInfo &parentState);
  GUI_IMPEXP bool Build(TiXmlElement *pNode, const char *szPath, bool bWrite); ///< non-virtual
  GUI_IMPEXP VListControlItem *GetItemAt(VGUIUserInfo_t &user, const hkvVec2 &vAbsPos) const;
  V_DECLARE_SERIALX( VListControlItemCollection, GUI_IMPEXP );
  GUI_IMPEXP void SerializeX( VArchive &ar );

  GUI_IMPEXP hkvVec2 GetSize() const;

  VDlgControlBase *m_pOwner;
  VTextStates m_DefaultProperties;
};


/// \brief
///   A GUI control class for a list control
class VListControl : public VDlgControlBase
{
public:
  GUI_IMPEXP VListControl();

  // VWindowBase overrides
  GUI_IMPEXP virtual void OnClick(VMenuEventDataObject *pEvent) HKV_OVERRIDE;
  GUI_IMPEXP virtual void OnDoubleClick(VMenuEventDataObject *pEvent) HKV_OVERRIDE;
  GUI_IMPEXP virtual void OnPointerDown(VMenuEventDataObject *pEvent) HKV_OVERRIDE;
  GUI_IMPEXP virtual void OnPointerUp(VMenuEventDataObject *pEvent) HKV_OVERRIDE;
  GUI_IMPEXP virtual void OnActivate() HKV_OVERRIDE {AdjustScrollBars();}
  GUI_IMPEXP virtual void OnSizeChanged() HKV_OVERRIDE;
  GUI_IMPEXP virtual VWindowBase* TestMouseOver(VGUIUserInfo_t &user, const hkvVec2 &vAbsMouse) HKV_OVERRIDE;
  GUI_IMPEXP virtual VTooltip* GetTooltip(VGUIUserInfo_t &user) HKV_OVERRIDE;

  // items
  GUI_IMPEXP void AddItem(VListControlItem *pItem, int iInsertBefore=-1, bool bSetDefaultStates=true);
  GUI_IMPEXP VListControlItem* AddItem(const char *szName, int iInsertBefore=-1, int iData=0, void *pUserData=NULL);
  GUI_IMPEXP void RemoveItem(VListControlItem *pItem);
  GUI_IMPEXP void RemoveItemAt(int iIndex);
  GUI_IMPEXP void Reset();
  GUI_IMPEXP void EnsureVisible(VListControlItem *pItem);
  GUI_IMPEXP void SetScrollPosition(float fPos);

  /// \brief Grants read access to the sub items.
  GUI_IMPEXP const VListControlItemCollection& Items() const {return m_Items;}
  
  // selection
  inline int GetSelectionIndex(const VGUIUserInfo_t *pUser=NULL) const 
  {
    int i = (pUser!=NULL) ? pUser->m_iID : 0;
    return m_spSelectedItem[i] ? m_spSelectedItem[i]->GetIndex() : -1;
  }
  inline VListControlItem* GetSelectedItem(const VGUIUserInfo_t *pUser=NULL) const 
  {
    int i = (pUser!=NULL) ? pUser->m_iID : 0;
    return m_spSelectedItem[i];
  }

  GUI_IMPEXP void SetSelectionIndex(int iIndex, const VGUIUserInfo_t *pUser=NULL);
  GUI_IMPEXP void SetSelection(VListControlItem *pItem, const VGUIUserInfo_t *pUser=NULL);

  GUI_IMPEXP VListControlItem *GetItemAt(VGUIUserInfo_t &user, const hkvVec2 &vAbsPos) const;

  inline void SetIconSize(float fSizeInPixels, const hkvVec2& vRelIconOfs)
  {
    m_fIconSize = fSizeInPixels;
    m_vIconOfs = vRelIconOfs;
  }

  inline float GetIconSize() const
  {
    return m_fIconSize;
  }

  inline const hkvVec2& GetIconRelOfs() const
  {
    return m_vIconOfs;
  }

protected:
  GUI_IMPEXP void AdjustScrollBars();

// serialization
  V_DECLARE_SERIAL_DLLEXP( VListControl, GUI_IMPEXP_DATA );
  GUI_IMPEXP virtual void Serialize( VArchive &ar );
// renderable
  GUI_IMPEXP virtual void OnPaint(VGraphicsInfo &Graphics, const VItemRenderInfo &parentState);
  GUI_IMPEXP virtual bool Build(TiXmlElement *pNode, const char *szPath, bool bWrite);
  GUI_IMPEXP virtual void OnTick(float dtime);

  GUI_IMPEXP void SetMouseOverItem(VGUIUserInfo_t &user, VListControlItem *pItem);

  // member vars:
  bool m_bAllowSelection;
  VListControlItemCollection m_Items;
  VSliderControlPtr m_spVScrollbar;
  VColorRef m_iBackgroundCol;

  float m_fIconSize;
  hkvVec2 m_vIconOfs;

  // temp data
  float m_fCurrentScrollPos;
  bool m_bCollectionChanged, m_bFromScrollbar;
  VListControlItemPtr m_spMouseOverItem[VGUIUserInfo_t::GUIMaxUser];
  VListControlItemPtr m_spSelectedItem[VGUIUserInfo_t::GUIMaxUser];

  VListControlItemPtr m_spMouseDownItem[VGUIUserInfo_t::GUIMaxUser];
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
