/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VMapLookupControl.hpp

#ifndef VMAPLOOKUPCONTROL_HPP_INCLUDED
#define VMAPLOOKUPCONTROL_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/GUI/Controls/VListControls.hpp>

class VLookupItem;
typedef VSmartPtr<VLookupItem> VLookupItemPtr;

/// \brief
///   A single item that identifies a context sensitive region on a bitmap by a color
class VLookupItem : public VListControlItem
{
public:
  GUI_IMPEXP VLookupItem() {m_iMatchColor=V_RGBA_WHITE;}
  GUI_IMPEXP virtual bool Build(TiXmlElement *pNode, const char *szPath, bool bWrite) HKV_OVERRIDE;

  // serialization
  V_DECLARE_SERIAL_DLLEXP( VLookupItem, GUI_IMPEXP_DATA );
  GUI_IMPEXP virtual void Serialize( VArchive &ar ) HKV_OVERRIDE;

  VColorRef m_iMatchColor;
protected:
};


/// \brief
///   A control that has context sensitive regions identified by color keys on a bitmap.
class VMapLookupControl : public VDlgControlBase
{
public:
  GUI_IMPEXP VMapLookupControl();
  GUI_IMPEXP virtual void OnPaint(VGraphicsInfo &Graphics, const VItemRenderInfo &parentState) HKV_OVERRIDE;
  GUI_IMPEXP virtual bool Build(TiXmlElement *pNode, const char *szPath, bool bWrite) HKV_OVERRIDE;
  GUI_IMPEXP virtual VCursor *GetMouseOverCursor(VGUIUserInfo_t &user) HKV_OVERRIDE;
  GUI_IMPEXP virtual VWindowBase* TestMouseOver(VGUIUserInfo_t &user, const hkvVec2 &vAbsMouse) HKV_OVERRIDE;

protected:
// serialization
  V_DECLARE_SERIAL_DLLEXP( VMapLookupControl, GUI_IMPEXP_DATA );
  GUI_IMPEXP virtual void Serialize( VArchive &ar );

  VImageStates m_Image;  ///< 4 states
  VisBitmapPtr m_spLookup;

  VListControlItemCollection m_MatchItems;
  VLookupItemPtr m_spMouseOverItem;
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
