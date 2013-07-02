/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file vHavokStaticMesh.hpp

// ***********************************************************************************************
// vHavok binding for Vision that uses Havok for physics
// Copyright (C) Trinigy GmbH. All rights reserved.
// ***********************************************************************************************
#ifndef VHAVOKSTATICMESH_HPP_INCLUDED
#define VHAVOKSTATICMESH_HPP_INCLUDED

#include <Common/Base/hkBase.h>
#include <Physics2012/Dynamics/Entity/hkpRigidBody.h>

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokPhysicsIncludes.hpp>

class vHavokPhysicsModule;
class VisStaticMeshInstance_cl;


///
/// \brief 
///   Representation of static mesh instances in the Havok binding
///
/// The vHavokStaticMesh class takes one or multiple Vision static mesh instances as input,
/// creates a Havok rigid body for these meshes and throws them into the Havok simulation.
///
/// Creation and deletion of vHavokStaticMesh instances is in the responsibility of the
/// Havok physics module. It is not recommended to create vHavokStaticMesh instances in
/// game code, since the module will take care of that.
/// 
/// Please note that the constructor does not yet add the object to the simulation. One
/// has to call the ::Init functions for that purpose.
///
class vHavokStaticMesh : public VRefCounter
{
public:

  ///
  /// @name Construction / Destruction
  /// @{
  ///


  ///
  /// \brief
  ///   Constructor. Takes the Havok physics module instance it belongs to as parameter
  ///
  /// The constructor does not add the object to the Havok simulation. Use one of the 
  /// ::Init functions to put the object into simulation.
  ///
  /// \param module
  ///   Reference of the Havok Physics Module.
  /// 
  VHAVOK_IMPEXP vHavokStaticMesh(vHavokPhysicsModule &module);


  ///
  /// \brief
  ///   Destructor. Deletes the Havok rigid body if still alive.
  ///
  VHAVOK_IMPEXP ~vHavokStaticMesh();


  ///
  /// \brief
  ///   Removes the static mesh from the simulation without necessarily deleting this instance.
  ///
  VHAVOK_IMPEXP VOVERRIDE void DisposeObject();


  ///
  /// \brief
  ///   Initializes the object with one Vision static mesh instance and adds it to the simulation.
  ///
  /// \param meshInstance
  ///   Vision mesh instance that shall be part of the Havok simulation.
  ///
  VHAVOK_IMPEXP VOVERRIDE void Init(VisStaticMeshInstance_cl &meshInstance);


  ///
  /// @}
  ///


  ///
  /// @name Initialization
  /// @{
  ///


  ///
  /// \brief
  ///   Initializes the object with one Vision static mesh instance collection and adds it to the simulation.
  ///
  /// \param meshInstances
  ///   Vision mesh instance collection that shall be part of the Havok simulation.
  ///
  VHAVOK_IMPEXP VOVERRIDE void Init(VisStaticMeshInstCollection &meshInstances);


  ///
  /// @}
  ///


  ///
  /// @name Access to Havok Internals
  /// @{
  ///

  ///
  /// \brief
  ///   Gets the Havok internal rigid body instance (can be NULL if not yet initialized).
  ///
  /// \returns
  ///   Pointer to the internal Havok Rigid Body (can be NULL).
  /// 
  VHAVOK_IMPEXP hkpRigidBody *GetHkRigidBody() { return m_pRigidBody; }


  ///
  /// \brief
  ///   Gets the Havok shape that belongs to the rigid body instance (can be NULL if not yet initialized).
  /// 
  /// \returns
  ///   Pointer to the internal Havok Rigid Shape (can be NULL).
  /// 
  VHAVOK_IMPEXP const hkpShape *GetHkShape() const; 


  ///
  /// @}
  ///


  ///
  /// @name Synchronization
  /// @{
  ///


  ///
  /// \brief
  ///   Gets the transformation of the static mesh(es) from Havok and sets them on the Vision instances.
  ///
  /// The physics module takes care of updating the transformation during the physics simulation. You
  /// will typically not have to call this function manually.
  ///
  VHAVOK_IMPEXP void UpdateHavok2Vision();


  ///
  /// \brief
  ///   Gets the transformation of the static mesh(es) from Vision and sets them on the Havok instance
  ///
  /// The physics module takes care of updating the transformation during the physics simulation. You
  /// will typically not have to call this function manually.
  ///
  VHAVOK_IMPEXP void UpdateVision2Havok();


  ///
  /// @}
  ///


  ///
  /// @name Access to Vision Internals
  /// @{
  ///


  ///
  /// \brief
  ///   Gets the number of the static mesh instances represented by this class.
  /// 
  /// \returns
  ///   Number of static meshes.
  ///
  VHAVOK_IMPEXP int GetNumWrappedStaticMeshes();

  /// \brief
  ///   Gets the number of valid static meshes wrapped by this instance.
  ///
  /// The number of valid static meshes may be lower than the number of wrapped
  /// static meshes, if this instance was initialized with a group of meshes, and
  /// some meshes have been removed in the meantime.
  ///
  /// \return
  ///   the number of valid static meshes
  VHAVOK_IMPEXP int GetNumValidStaticMeshes() const;


  /// \brief
  ///   Gets a static mesh instance at the given index in the list.
  /// \param index
  ///   Index of the static mesh instance in the list.
  /// \returns
  ///   Pointer to the static mesh instance at the given index in the list.
  /// \note
  ///   It is possible to get back a \c null pointer, in case a static mesh 
  ///   managed by this object has been removed in the meantime.
  VHAVOK_IMPEXP VisStaticMeshInstance_cl* GetWrappedStaticMesh(int index);


  /// \brief
  ///   Removes a static mesh instance from this object.
  /// \param pMesh
  ///   the static mesh instance to remove
  VHAVOK_IMPEXP void RemoveStaticMesh(VisStaticMeshInstance_cl *pMesh);

  ///
  /// @}
  ///


  ///
  /// @name Debug Functionality
  /// @{
  ///

  /// \brief
  ///   Enables or disabled debug rendering of the havok collision mesh.
  VHAVOK_IMPEXP void SetDebugRendering (bool bEnable);

  ///
  /// @}
  ///

protected:

  ///
  /// \brief
  ///   Creates the Havok shape and rigid body, and adds the object to the simulation.
  ///
  void CreateHkRigidBody();


  ///
  /// \brief
  ///   Removes object from the simulation and releases the rigid body reference.
  ///
  void RemoveHkRigidBody();


  ///
  /// \brief
  ///   Common deinitialisation code that is used both on DisposeObject and on destruction
  ///
  void CommonDeinit();

  
  hkpRigidBody *m_pRigidBody;                   ///< Pointer to the Havok internal rigid body instance
  bool m_bInitialized;                          ///< Indicates whether object has been initialized with one of the ::Init functions

  VisStaticMeshInstCollection m_staticMeshes;   ///< List of Vision static mesh instances represented by this class. Not reference counted to avoid cyclic dependencies and resource cleanup warnings.
  vHavokPhysicsModule &m_module;                ///< Reference to the physics module this object belongs to
  int m_iNumValidStaticMeshes;                  ///< Number of valid static meshes managed by this instance
  hkvVec3 m_vScale;								              ///< the scale this static mesh was created with
  const char *m_szShapeCacheId;                 ///< ID of shape of the rigid body in runtime cache table (points to memory in cache table). 
};




#endif // VHAVOKSTATICMESH_HPP_INCLUDED

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
