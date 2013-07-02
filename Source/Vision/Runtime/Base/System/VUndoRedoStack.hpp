/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VUndoRedoStack.hpp

#ifndef VISION_VUNDOREDOSTACK_HPP
#define VISION_VUNDOREDOSTACK_HPP

#include <Vision/Runtime/Base/Action/VAction.hpp>

class VActionGroup;

/// \brief
///   Class managing an undo/redo stack of VAction instances.
/// 
class VUndoRedoStack
{
public:

  /// \brief
  ///   Constructor setting the maximal stack size.
  ///
  /// \param size the maximal size of the stack. The stack size will never exceed this
  ///   value.
  VBASE_IMPEXP VUndoRedoStack(int size);
  /// \brief
  ///   Destructor
  VBASE_IMPEXP ~VUndoRedoStack();
  
  /// \brief
  ///   Performs a redo of the next item in the history
  VBASE_IMPEXP VBool Redo();
  /// \brief
  ///   Performs an undo of the previous item in the history
  VBASE_IMPEXP VBool Undo();

  /// \brief
  ///   Inserts a new action into the current position of the stack (but does not execute it)
  VBASE_IMPEXP void PushAction(VAction *newAction);

  /// \brief
  ///   Sets the maximal stack size.
  ///
  /// PushAction ensures the stacksize won't exceed this value by calling DeleteOldestUndoAction when
  /// the maximum number is overrun.
  ///
  /// \param newSize
  ///   The new maximal stack size. Values smaller than the current stack size will cause the according
  ///   number of oldest actions to be deleted. Greater values will allow the stack to conain more actions.
  VBASE_IMPEXP void SetSize(int newSize);

  /// \brief
  ///   Deletes all items on undo- and redo-stack
  VBASE_IMPEXP void Reset();

  /// \brief
  ///   Returns the number of possible Undo() function calls
  VBASE_IMPEXP int GetUndoCount() const;

  /// \brief
  ///   Returns the number of possible Redo() function calls
  VBASE_IMPEXP int GetRedoCount() const;

  /// \brief
  ///   Prints stack content (debug only)
  VBASE_IMPEXP void DumpUndoStack();

  /// \brief
  ///   Prints stack content (debug only)
  VBASE_IMPEXP void DumpRedoStack();
  
protected:
  friend class VAction;

  VBASE_IMPEXP void DeleteOldestUndoAction();    ///< Deletes the oldest action.
  VBASE_IMPEXP void EmptyRedoList();

  VActionList undoList; ///< List that holds the undo elements
  VActionList redoList; ///< List that holds the redo elements
  int stackSize;  ///< Maximum wanted size
  VActionGroup *activeGroup;  ///< internal
  int pos;                    ///< index of the action which would be "done" at the next Do() call (usually lastIndex + 1)
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
