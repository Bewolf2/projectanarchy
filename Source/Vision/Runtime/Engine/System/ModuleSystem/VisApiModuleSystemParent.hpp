/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiModuleSystemParent.hpp

#ifndef DEFINE_VISAPIMODULESYSTEMPARENT
#define DEFINE_VISAPIMODULESYSTEMPARENT


#include <Vision/Runtime/Engine/System/ModuleSystem/VisApiModuleSystemFlags.hpp>


/// \brief
///   Module System Parent class
/// 
/// This class is the module system class for the parent side. If you derive your class from this
/// VisModuleSystemParent_cl, then your class will automatically behave as a parent. The
/// registration functions for adding and removing children will automatically be called from the
/// children side,  but some notify functions may be called from the parent side in order to send
/// notifies to all registered children.
class VisModuleSystemParent_cl
{
public:

  ///
  /// @name Constructors / Destructor
  /// @{
  ///

  /// \brief
  ///   Constructor of the module system parent class
  /// 
  /// Initialises the childrenTable.
  VISION_APIFUNC VisModuleSystemParent_cl();
  

  /// \brief
  ///   Destructor of the module system parent class which deletes all the registered children
  /// 
  /// Calls the ModSysNotifyFunctionOnDisconnect(TRUE) for each registered child.
  VISION_APIFUNC virtual ~VisModuleSystemParent_cl();
  

  ///
  /// @}
  ///

  ///
  /// @name Setup Functions
  /// @{
  ///

  /// \brief
  ///   Adds a child to the childrenTable
  /// 
  /// The ModSysAddChild function is automatically called by the module system child class.
  /// 
  /// \param child
  ///   pointer to the child to be registered
  VISION_APIFUNC virtual void ModSysAddChild(class VisModuleSystemChild_cl *child);


  /// \brief
  ///   Removes a child from the childrenTable and optionally deletes it physically
  /// 
  /// The ModSysRemoveChild function removes a child from the childrenTable and optionally deletes
  /// the child physically. This function is automatically called from the module system child
  /// class to remove the child from the childrenTable, but in that case the child is not going to
  /// be deleted physically.
  /// 
  /// \param child
  ///   pointer to the child to be removed from the childrenTable
  /// 
  /// \param deleteChild
  ///   if TRUE the child will additionally be deleted physically.
  VISION_APIFUNC virtual void ModSysRemoveChild(class VisModuleSystemChild_cl *child, BOOL deleteChild = TRUE);


  ///
  /// @}
  ///

  ///
  /// @name Notify Functions
  /// @{
  ///

  /// \brief
  ///   Notifies all the children that parent data has been changed
  /// 
  /// This notify function has to be called whenever certain data of the parent changes, the notify
  /// functions of the children will then be called automatically
  /// 
  /// The flag parameter defines which data has been changed.
  /// 
  /// \param flags
  ///   Available flags are:
  ///   \li VIS_MODSYSFLAG_POSITION: position has changed, might be called multiple times per frame
  /// 
  ///   \li VIS_MODSYSFLAG_ORIENTATION: orientation has changed, might be called multiple times per
  ///     frame
  /// 
  ///   \li VIS_MODSYSFLAG_FRAMEPOSITION: position has changed, called only once per frame
  /// 
  ///   \li VIS_MODSYSFLAG_FRAMEORIENTATION: orientation has changed, called only once per frame
  /// 
  /// \note
  ///   The available flags are defined in the file "VisApiModuleSystemFlags.hpp"
  VISION_APIFUNC void ModSysNotifyFunctionParentAltered(int flags);


  /// \brief
  ///   Calls the handle notify function for all the children
  /// 
  /// This notify function has to be called whenever the handle functions of the children shall be
  /// called, the handle notify functions of the children will then be called automatically.
  VISION_APIFUNC void ModSysNotifyFunctionHandle();

  /// \brief
  ///   Notifies all the children that the parent has been destroyed
  /// 
  /// This notify function will automatically be called when the parent is going to be destroyed.
  /// It calls all the ParentDestroyed notify functions of the children.
  VISION_APIFUNC void ModSysNotifyFunctionParentDestroyed();


  /// \brief
  ///   Sends a command to all the children
  /// 
  /// \param command
  ///   Available commands are:
  ///   \li VIS_MODSYSCMD_RECOMPUTEVISIBILITY: Child has to recompute the visibility
  ///     (ReComputeVisibility function)
  VISION_APIFUNC void ModSysNotifyFunctionCommand(int command);


  /// \brief
  ///   Returns the number of children. Might be overestimated due to NULL entries in child list
  /// 
  /// \return
  ///   iChildren : number of children
  inline int GetNumChildren() const {return childrenTable.GetSize();}

  /// \brief
  ///   Accesses the n-th child in the list. Return value might be NULL.
  /// 
  /// \param iIndex
  ///   The index of the child to return
  ///
  /// \return
  ///   VisModuleSystemChild_cl *pChild : Pointer to child. Might be NULL even for valid range.
  inline VisModuleSystemChild_cl *GetChild(int iIndex) const {return childrenTable.GetDataPtr()[iIndex];}

  ///
  /// @}
  ///

private:

  DynArray_cl<class VisModuleSystemChild_cl*> childrenTable;           ///< array with all registered children
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
