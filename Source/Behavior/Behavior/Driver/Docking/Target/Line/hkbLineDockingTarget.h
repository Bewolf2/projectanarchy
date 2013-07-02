/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_LINE_DOCKING_TARGET_H
#define HKB_LINE_DOCKING_TARGET_H

#include <Behavior/Behavior/Driver/Docking/Target/hkbDockingTarget.h>


	/// implements the docking target interface to dock a character to a line
class hkbLineDockingTarget : public hkbDockingTarget
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );

		hkbLineDockingTarget();

		///////////////////////////////
		/// hkbDockingTarget Interface
		///////////////////////////////

		virtual void getDockedTransform(const hkQsTransform& undockedTransform, hkReal blendParam, hkReal timeOffset, hkQsTransform& dockedTransform) HK_OVERRIDE;

	public:

		///////////////
		/// Properties
		///////////////

		/// Determines if the line docking target is active.
		hkBool m_active;

		/// Determines if rotation is constrained or not.  This is true by default.
		hkBool m_constrainRotation;

		/// When projecting the undocked position onto the line, this is the line parameter (0,1) which defines where
		/// the docked position is in line space.  This parameter can be used to handle boundary cases of docked motion
		/// when the character gets to the end of the line.  This parameter is also used to interpolate the rotations
		/// that are defined for the two endpoints.
		hkReal m_lineParam;

		/// The endpoint that corresponds to line parameter = 0.
		hkQsTransform m_endPointA;

		/// The endpoint that corresponds to line parameter = 1.
		hkQsTransform m_endPointB;
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
