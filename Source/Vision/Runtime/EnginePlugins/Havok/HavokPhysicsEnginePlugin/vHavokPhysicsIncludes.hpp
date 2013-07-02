/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file vHavokPhysicsIncludes.hpp

// ***********************************************************************************************
// vHavok binding for Vision that uses Havok for physics
// Copyright (C) Havok. All rights reserved.
// ***********************************************************************************************
#ifndef VISION_HAVOK_INCLUDED_HPP
#define VISION_HAVOK_INCLUDED_HPP

#include <Common/Base/KeyCode.h>
#ifndef HAVOK_PHYSICS_2012_KEYCODE
#error "A valid Havok Physics 2012 keycode is required to build vHavokEnginePlugin. Please enter your keycode(s) in <Common/Base/KeyCode.h>"
#endif

extern VModule g_vHavokModule;

#ifdef WIN32
  #ifdef VHAVOKMODULE_EXPORTS
    #define VHAVOK_IMPEXP __declspec(dllexport)
  #elif defined VHAVOKMODULE_IMPORTS
    #define VHAVOK_IMPEXP __declspec(dllimport)
  #else
    #define VHAVOK_IMPEXP
  #endif
  
#elif defined (_VISION_XENON)
    #define VHAVOK_IMPEXP 
    


#elif defined (_VISION_PS3)
  #define VHAVOK_IMPEXP
  

  
#elif defined (_VISION_IOS) || defined(_VISION_ANDROID)
  #define VHAVOK_IMPEXP

#elif defined(_VISION_PSP2)
    #define VHAVOK_IMPEXP 

#elif defined(_VISION_WIIU)
	#define VHAVOK_IMPEXP 

#else
 #error Undefined platform!
#endif

// Common typedefs
typedef VPListT<VisStaticMeshInstance_cl> VisStaticMeshInstCollection;


#if defined(SN_TARGET_PS3)

// Undefining _M_PPCBE in PS3 because Havok then thinks it is a pure PPC architecture.
// Search for "_M_PPCBE" in hkBaseTypes.h. It sets HK_ARCH_PPC instead of HK_ARCH_PS3/PS3SPU
#ifdef _M_PPCBE
#undef _M_PPCBE
#define RESTORE_M_PPCBE
#endif

#endif


#ifdef _VISION_ANDROID
#undef printf
#endif

// Always include the Havok headers as unmanaged code, since there are constructs
// contained in there which don't compile to managed targets.
#pragma managed(push, off)

// Havok - Math and base include
#include <Common/Base/KeyCode.h>
#include <Common/Base/hkBase.h>
#include <Common/Base/Ext/hkBaseExt.h>
#include <Common/Base/System/hkBaseSystem.h>
#include <Common/Base/Memory/System/Util/hkMemoryInitUtil.h>
#include <Common/Base/Memory/Allocator/Thread/hkThreadMemory.h>

// Havok - Base Types
#include <Common/Base/Types/Geometry/hkStridedVertices.h>
#include <Common/Base/Types/Geometry/hkGeometry.h>

// Havok - Dynamics includes
#include <Physics2012/Collide/hkpCollide.h>
#include <Physics2012/Collide/Filter/Group/hkpGroupFilter.h>
#include <Physics2012/Dynamics/World/hkpWorld.h>
#include <Physics2012/Dynamics/Entity/hkpRigidBody.h>
#include <Physics2012/Dynamics/Collide/ContactListener/hkpContactListener.h>

#include <Physics2012/Collide/Shape/Convex/Box/hkpBoxShape.h>
#include <Physics2012/Collide/Shape/Convex/Sphere/hkpSphereShape.h>
#include <Physics2012/Collide/Shape/Convex/Capsule/hkpCapsuleShape.h>
#include <Physics2012/Collide/Shape/Convex/Cylinder/hkpCylinderShape.h>
#include <Physics2012/Collide/Shape/Convex/hkpConvexShape.h>
#include <Physics2012/Collide/Shape/Convex/ConvexVertices/hkpConvexVerticesShape.h>
#include <Physics2012/Collide/Shape/Convex/ConvexVertices/hkpConvexVerticesConnectivityUtil.h>

#include <Physics2012/Collide/Query/CastUtil/hkpWorldRayCastInput.h>
#include <Physics2012/Collide/Query/Collector/RayCollector/hkpClosestRayHitCollector.h>

#include <Physics2012/Collide/Util/Welding/hkpMeshWeldingUtility.h>
#include <Physics2012/Utilities/Collide/ShapeUtils/ShapeScaling/hkpShapeScalingUtility.h>
#include <Physics2012/Utilities/Dynamics/Inertia/hkpInertiaTensorComputer.h>
#include <Physics2012/Collide/Dispatch/hkpAgentRegisterUtil.h>

// Havok - Visual Debugger includes
#include <Common/Visualize/hkVisualDebugger.h>
#include <Physics2012/Utilities/VisualDebugger/hkpPhysicsContext.h>

// Vision - custom shapes
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokVisionShapes.h>

#pragma managed(pop)


#if defined(RESTORE_M_PPCBE) && !defined(_M_PPCBE)
#define _M_PPCBE
#undef RESTORE_M_PPCBE
#endif

///
/// \brief
///   Havok Stepper Interface.
/// 
class IHavokStepper
{
public:
  
  virtual ~IHavokStepper()
  {}

  VOVERRIDE void Step( float dt ) = 0;

	/// Called by the Havok physics module on de-initialization. It is called before anything
	/// was actually deleted, with the hkpWorld marked for write.
  VOVERRIDE void OnDeInitPhysicsModule() {}
};


#endif //VISION_HAVOK_INCLUDED_HPP

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
