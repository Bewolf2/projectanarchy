/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_PLANE_DOCKING_TARGET_H
#define HKB_PLANE_DOCKING_TARGET_H

#include <Behavior/Behavior/Driver/Docking/Target/hkbDockingTarget.h>


	/// Implements the docking target interface to dock a character to a 3d plane
class hkbPlaneDockingTarget : public hkbDockingTarget
{
	public:
		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );

			/// Initialize the target with the plane provided
		hkbPlaneDockingTarget(const hkVector4& plane);

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

		/// Contains the coefficients of the plane that will be docked to.
		hkVector4 m_plane;
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
