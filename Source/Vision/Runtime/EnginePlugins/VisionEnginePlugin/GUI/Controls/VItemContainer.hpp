/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VItemContainer.hpp

#ifndef VITEMCONTAINER_HPP_INCLUDED
#define VITEMCONTAINER_HPP_INCLUDED



/// \brief
///   Contains a collection of items. Can be used to group items for moving, hiding etc.
class VItemContainer : public VDlgControlBase
{
public:
  GUI_IMPEXP VItemContainer();

  /// \brief
  ///   Access the collection of sub-items. Use SubItems().FindItem(ID) to search for specific
  ///   sub-items
  VMenuItemCollection& SubItems() {return m_SubItems;}


  /// \brief
  ///   Optional frame to display
  VDialogFrame m_Frame;

protected:
// serialization
  V_DECLARE_SERIAL_DLLEXP( VItemContainer, GUI_IMPEXP_DATA );
  GUI_IMPEXP virtual void Serialize( VArchive &ar );
// renderable
  GUI_IMPEXP virtual void OnPaint(VGraphicsInfo &Graphics, const VItemRenderInfo &parentState);
  GUI_IMPEXP virtual bool Build(TiXmlElement *pNode, const char *szPath, bool bWrite);
  GUI_IMPEXP virtual void OnTick(float dtime);

  // override some important functions which are propagated to the subitems
  GUI_IMPEXP virtual VWindowBase* TestMouseOver(VGUIUserInfo_t &user, const hkvVec2 &vAbsMouse);
  GUI_IMPEXP virtual VCursor *GetMouseOverCursor(VGUIUserInfo_t &user);
  GUI_IMPEXP virtual VTooltip* GetTooltip(VGUIUserInfo_t &user);

  VMenuItemCollection m_SubItems; 
  VWindowBasePtr m_spMouseOverItem[VGUIUserInfo_t::GUIMaxUser];
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
