/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#pragma once


/// \brief
///   Collision mesh generator
///
/// Generates a convex hull or a triangle mesh used as collision mesh. This processor uses qhull for
/// convex hull computation, thus introducing an external dependency on qhull.
/// This processor only takes geometry tagged as 'collider' (triangle flags) into account.
class VGEOM2_IMPEXP_CLASS VGProcessor_CollisionMeshGenerator : public VGDynamicProcessor<VGProcessor_CollisionMeshGenerator>
{
public:
  enum CollisionMeshFormat
  {
    VGCF_CONVEX_HULL  // At the moment, the generator supports convex hulls, only.
  };


public:
  VGProcessor_CollisionMeshGenerator();
  virtual ~VGProcessor_CollisionMeshGenerator();



  /// \brief
  ///   Processes a scene - generates a collision mesh for the meshes in a scene
  ///
  /// \param scene
  ///   scene containing meshes to generate collision meshes for
  ///
  /// \return
  ///   False if generation failed
  virtual bool               Process(VGScene& scene) const;



  /// \brief
  ///   Sets the desired collision mesh format
  ///
  /// \param e
  ///   Collision mesh format the processor should generate
  inline void                SetCollisionMeshFormat(CollisionMeshFormat e) throw() { m_collisionMeshFormat = e; }

  /// \brief
  ///   Gets the currently set collision mesh format
  ///
  /// \return
  ///   Currently set collision mesh format
  inline CollisionMeshFormat GetCollisionMeshFormat() const throw()                { return m_collisionMeshFormat; }



  /// \brief
  ///   Sets index into scene of physics info to use for newly generated collision mesh
  ///
  /// \param i
  ///   Index (-1 means "use default")
  inline void                SetPhysicsInfoIndex(int i) throw()                    { m_physicsInfoIndex = i; }

  /// \brief
  ///   Gets index into scene of physics info to use for newly generated collision mesh
  ///
  /// \return
  ///   Index (-1 means "use default")
  inline int                 GetPhysicsInfoIndex() const throw()                   { return m_physicsInfoIndex; }



private:
  CollisionMeshFormat m_collisionMeshFormat;
  int                 m_physicsInfoIndex;
};

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
