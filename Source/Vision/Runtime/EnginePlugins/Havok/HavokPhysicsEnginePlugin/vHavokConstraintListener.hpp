/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file vHavokConstraintListener.hpp

// ***********************************************************************************************
// vHavok binding for Vision that uses Havok for physics
// Copyright (C) Trinigy GmbH. All rights reserved.
// ***********************************************************************************************

#ifndef VHAVOKCONSTRAINTLISTENER_HPP_INCLUDED
#define VHAVOKCONSTRAINTLISTENER_HPP_INCLUDED

#pragma managed(push, off)
#include <Physics2012/Dynamics/Constraint/hkpConstraintListener.h>
#pragma managed(pop)

class vHavokConstraint;
class vHavokConstraintListener;

///
/// \brief
///   Structure that holds information about a constraint break.
///
/// When a constraint is broken, the corresponding constraint object is notified.
///
struct vHavokConstraintBreakInfo_t
{
  vHavokConstraint *m_pConstraint; ///< The constraint that has been broken
  hkReal m_fImpulse;                ///< Actual impulse that broke the constraint
};

/// \brief
///   Helper class for vHavok event handling - internal use only!
///
class vHavokConstraintBreakListenerProxy : public hkpConstraintListener
{
public:
	vHavokConstraintBreakListenerProxy() { }
	void setOwner(vHavokConstraintListener* o ) { m_owner = o; }
	virtual void constraintBreakingCallback(const hkpConstraintBrokenEvent &event);
	vHavokConstraintListener* m_owner;
};


/// 
/// \brief
///    Class that implements the hkpConstraintListener Interface, which notifies
///    about constraint events.
///
/// The Havok constraint listener is added to the Havok world to provide notification
/// to other parts of the Havok plugin when a constraint event occurs.
///
class vHavokConstraintListener : 
	public VRefCounter
{
public:

  /// @name Constructor / Destructor
  // @{
  /// \brief
  ///   Constructor of the Havok Contact Listener.
  /// \param pPhysicsWorld
  ///   Pointer to the Havok physics world this listener is added to.
  vHavokConstraintListener(hkpWorld* pPhysicsWorld);  

  /// \brief
  ///   Destructor.
  ~vHavokConstraintListener();
  //@}


  /// @name hkpConstraintListener Virtual Overrides
  //@{
  /// \brief
  ///   Called by Havok when a constraint gets broken.
  VOVERRIDE void constraintBreakingCallback(const hkpConstraintBrokenEvent &event);
  //@}

  vHavokConstraintBreakListenerProxy m_listener;
};

#endif // VHAVOKCONSTRAINTLISTENER_HPP_INCLUDED

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
