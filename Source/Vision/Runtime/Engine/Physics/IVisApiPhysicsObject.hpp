/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file IVisApiPhysicsObject.hpp

#ifndef FR_DEFINE_IVISAPIPHYSICSOBJECT
#define FR_DEFINE_IVISAPIPHYSICSOBJECT



/// \brief
///   Interface for physics objects
/// 
/// \note
///   Several parts of the Vision Physics API are obsolete without explicitly being marked as such
///   and should not be used directly. When in doubt, use the functionality provided by the
///   vHavokPhysicsPlugin directly instead of relying on the generic interface.
/// 
/// The IVisPhysicsObject_cl class is the interface for physics objects. It doesn't specify any
/// interface functions but rather serves the purpose to have a typed pointer instead of a void
/// pointer for accessing physics objects.
/// 
/// Physics modules have to implement their own physics object class based on this interface.
/// 
/// \sa IVisPhysicsModule_cl::CreatePhysicsObject
/// \sa VisBaseEntity_cl::CreatePhysicsObject
/// \sa VisBaseEntity_cl::GetPhysicsObject
/// \sa VisBaseEntity_cl::SetPhysicsObject
/// \sa IVisPhysicsModule_cl
class IVisPhysicsObject_cl : public IVObjectComponent
{
  public:
    /// \brief
    ///   Empty Constructor. Physics modules have to implement their own physics object class based
    ///   on this interface
    IVisPhysicsObject_cl() {}
};


typedef VSmartPtr<IVisPhysicsObject_cl> IVisPhysicsObjectPtr;

#endif  // FR_DEFINE_VISAPIPHYSICSOBJECT

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
