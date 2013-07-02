/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file vHavokTerrain.hpp

// ***********************************************************************************************
// vHavok binding for Vision that uses Havok for physics
// Copyright (C) Trinigy GmbH. All rights reserved.
// ***********************************************************************************************
#ifndef VHAVOKTERRAIN_HPP_INCLUDED
#define VHAVOKTERRAIN_HPP_INCLUDED

#include <Common/Base/hkBase.h>
#include <Physics2012/Dynamics/Entity/hkpRigidBody.h>

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokPhysicsIncludes.hpp>

class vHavokPhysicsModule;


///
/// \brief 
///   Representation of terrains (heightfields) instances in the Havok binding.
///
class vHavokTerrain : public VRefCounter
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
  VHAVOK_IMPEXP vHavokTerrain(vHavokPhysicsModule &module);


  ///
  /// \brief
  ///   Destructor. Deletes the Havok rigid body if still alive.
  ///
  VHAVOK_IMPEXP ~vHavokTerrain();


  ///
  /// \brief
  ///   Removes the static mesh from the simulation without necessarily deleting this instance.
  ///
  VHAVOK_IMPEXP VOVERRIDE void DisposeObject();


  ///
  /// \brief
  ///   Initializes the object with one Vision Terrain sector and adds it to the simulation.
  ///
  /// \param sector
  ///   Vision terrain sector that shall be part of the Havok simulation.
  VHAVOK_IMPEXP VOVERRIDE void Init(const VTerrainSector &sector);


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
  inline hkpRigidBody *GetHkRigidBody() { return m_pRigidBody; }


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
  /// @name Access to Vision Internals
  /// @{
  ///

  
  ///
  /// \brief
  ///   Gets the terrain sector of this Havok Terrain instance.
  /// 
  /// \returns
  ///   Pointer to the terrain sector.
  ///
  VHAVOK_IMPEXP const VTerrainSector* GetWrappedTerrainSector();


  ///
  /// @}
  ///


  ///
  /// @name Helper methods
  /// @{
  ///

  ///
  /// \brief
  ///   Caches the internally used physics shape to a HKT file.
  ///
  VHAVOK_IMPEXP void SaveShapeToFile();

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

  bool m_bInitialized;                          ///< Indicates whether object has been initialized with one of the ::Init functions
  hkpRigidBody *m_pRigidBody;                   ///< Pointer to the Havok internal rigid body instance

  const VTerrainSector* m_terrainSector;        ///< Terrain sector of this Havok Terrain instance.
  vHavokPhysicsModule &m_module;                ///< Reference to the physics module this object belongs to
};



///
/// \brief 
///   Representation of a IVisDecorationGroup_cl, created in vHavokPhysicsModule::OnDecorationCreated and attached to the group
class VHavokDecorationGroup : public VRefCounter
{
public:
  VHavokDecorationGroup(IVisDecorationGroup_cl &group);
  virtual ~VHavokDecorationGroup();
  VHAVOK_IMPEXP VOVERRIDE void DisposeObject();
  void CommonDeinit();
  VHAVOK_IMPEXP hkpRigidBody* GetHkRigidBody();
private:
  hkpEntity *m_pCompoundRigidBody;
};


#endif // VHAVOKTERRAIN_HPP_INCLUDED

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
