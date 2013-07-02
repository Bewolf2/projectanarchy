/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VDlgControlBase.hpp

#ifndef VDLGCONTROLBASE_HPP_INCLUDED
#define VDLGCONTROLBASE_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/GUI/VWindowBase.hpp>

class VListControlItem;
class VDialog;
class VCursor;
class VDlgControlBase;
class VGUIManager;
typedef VSmartPtr<VCursor> VCursorPtr;
typedef VSmartPtr<VDlgControlBase> VDlgControlBasePtr;


/// \brief
///   Base class for all dialog controls, derived from VWindowBase.
class VDlgControlBase : public VWindowBase
{
public:

  /// \brief
  ///   Empty constructor
  GUI_IMPEXP VDlgControlBase();

  /// \brief
  ///   Gets the owner dialog.
  GUI_IMPEXP VDialog *GetParentDialog() const;

  GUI_IMPEXP virtual void OnStatusFlagsChanged(int iOldFlags, int iNewFlags);

  /// \brief
  ///   Overridden build function, parses the control ID
  GUI_IMPEXP virtual bool Build(TiXmlElement *pNode, const char *szPath, bool bWrite);

protected:
  friend class VDialog;
  friend class VMenuItemCollection;
// serialization
  V_DECLARE_SERIAL_DLLEXP( VDlgControlBase, GUI_IMPEXP_DATA );
  GUI_IMPEXP virtual void Serialize( VArchive &ar );
};




/// \brief
///   Collection of control items, used by dialogs to store the controls
class VMenuItemCollection : public VRefCountedCollection<VDlgControlBase>
{
public:

  /// \brief
  ///   Renders all controls in this collection.
  GUI_IMPEXP void RenderAll(VGraphicsInfo &Graphics, const VItemRenderInfo &parentState);

  /// \brief
  ///   Calls the OnTick function on all controls.
  GUI_IMPEXP void OnTickFunction(float fTimeDelta);

  /// \brief
  ///   Calls the OnActivate/OnDeactivate function on all controls
  GUI_IMPEXP void OnActivate(bool bStatus);

  /// \brief
  ///   Calls the Refresh function on all controls.
  GUI_IMPEXP void Refresh();

  /// \brief
  ///   Calls the OnParentChanged function on all controls.
  GUI_IMPEXP void OnParentChanged(int iFlags);

  /// \brief
  ///   Finds the control that is under the absolute mouse cursor position of the passed user. Can be a sub-control.
  GUI_IMPEXP VWindowBase *GetMouseOverItem(VGUIUserInfo_t &user) const;

  /// \brief
  ///   Finds a control by its ID.
  GUI_IMPEXP VDlgControlBase *FindItem(int iID);


  ///
  /// @name Order
  /// @{
  ///


  /// \brief
  ///   Sorts the collection by control's sorting order.
  GUI_IMPEXP void SortByPriority();


  /// \brief
  ///   Returns the index (or -1) of the item in this collection that has tab order iOrder.
  GUI_IMPEXP int GetItemIndexWithTabOrder(int iOrder);

  /// \brief
  ///   Returns the index of the item that has the next higher tab order than the item with
  ///   specified index.
  GUI_IMPEXP int GetNextItemIndex(int iCurrentIndex);

  /// \brief
  ///   Returns the index of the item that has the next lower tab order than the item with
  ///   specified index.
  GUI_IMPEXP int GetPrevItemIndex(int iCurrentIndex);


  ///
  /// @}
  ///

  ///
  /// @name Build
  /// @{
  ///

  /// \brief
  ///   Fills this collection with item instances created from the parent XML node ("control"
  ///   subnodes).
  GUI_IMPEXP bool Build(VWindowBase *pOwner, TiXmlElement *pNode, const char *szPath, bool bWrite);

protected:
  static int CompareItemPriority( const void *arg1, const void *arg2);

  ///
  /// @}
  ///

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
