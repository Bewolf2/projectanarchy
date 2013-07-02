/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VFmodCollisionMeshInstance.hpp

#ifndef VFMODCOLLISIONMESHINSTANCE_HPP_INCLUDED
#define VFMODCOLLISIONMESHINSTANCE_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/ThirdParty/FmodEnginePlugin/VFmodIncludes.hpp>

/// Serialization versions
#define V_FMODCOLLISIONMESHINSTANCE_VERSION_0         0x00000000                             // Initial version
#define V_FMODCOLLISIONMESHINSTANCE_VERSION_CURRENT   V_FMODCOLLISIONMESHINSTANCE_VERSION_0

/// \brief
///   A single sound blocker geometry instance. Supports positioning etc.
///
/// This is the native counterpart of the vForge sound collision shape.
class VFmodCollisionMeshInstance : public VisObject3D_cl
{
public:
  
  ///
  /// @name Constructor / Destructor
  /// @{
  ///

  FMOD_IMPEXP VFmodCollisionMeshInstance(VFmodManager *pManager=NULL);

  FMOD_IMPEXP virtual ~VFmodCollisionMeshInstance();

  FMOD_IMPEXP VOVERRIDE void DisposeObject();

  ///
  /// @}
  ///


  ///
  /// @name Overridden functions
  /// @{
  ///
 
  FMOD_IMPEXP VOVERRIDE void OnObject3DChanged(int iO3DFlags);

#ifdef SUPPORTS_SNAPSHOT_CREATION
  FMOD_IMPEXP VOVERRIDE void GetDependencies(VResourceSnapshot &snapshot);
#endif

  ///
  /// @}
  ///


  ///
  /// @name Serialization
  /// @{
  ///

  V_DECLARE_SERIAL_DLLEXP( VFmodCollisionMeshInstance, FMOD_IMPEXP );

  FMOD_IMPEXP VOVERRIDE void Serialize( VArchive &ar );

  ///
  /// @}
  ///


  ///
  /// @name Mesh definition
  /// @{
  ///

  /// \brief
  ///   Sets a VSimpleCollisionMesh as collision mesh
  FMOD_IMPEXP bool SetCollisionMesh(VSimpleCollisionMesh *pMesh, float fDirectOcclusion=1.0f, float fReverbOcclusion=1.0f, int iMeshFlags=TRACEFLAG_NONE);  

  /// \brief
  ///   Sets a VBaseMesh as collision mesh
  FMOD_IMPEXP bool SetCollisionMesh(VBaseMesh *pMesh, float fDirectOcclusion=1.0f, float fReverbOcclusion=1.0f);

  /// \brief
  ///   Sets an arbitrary mesh as collision mesh
  FMOD_IMPEXP bool SetCollisionMesh(int iVertexCount, int iTriangleCount, hkvVec3 *pVertices, void *pIndices, float fDirectOcclusion=1.0f, float fReverbOcclusion=1.0f, int iMeshFlags=TRACEFLAG_NONE, int iIndexType = VIS_INDEXFORMAT_16);

  /// \brief
  ///   Removes current collision mesh
  inline void RemoveCollisionMesh() {SetCollisionMesh(0, 0, NULL, NULL, 1.0f, 1.0f); }

  ///
  /// @}
  ///


  ///
  /// @name CollisionMeshInstance property functions
  /// @{
  ///

  /// \brief
  ///   Returns, whether collision mesh is active
  inline bool IsActive() const { return m_bActive; }
  
  /// \brief
  ///  De-/ Activates collision mesh
  FMOD_IMPEXP void SetActive(bool bStatus);
  
  /// \brief
  ///   Gets scale of collision mesh
  inline const hkvVec3& GetScale() const  { return m_vScale; }
  
  /// \brief
  ///   Sets scale of collision mesh
  FMOD_IMPEXP void SetScaling(const hkvVec3 &vScale);

  /// \brief
  ///   Gets direct occlusion of collision mesh
  inline float GetDirectOcclusion() const  { return m_fDirectOcclusion; }
  
  /// \brief
  ///   Gets reverb occlusion of collision mesh
  inline float GetReverbOcclusion() const  { return m_fReverbOcclusion; }

  /// \brief
  ///   Sets direct and reverb occlusion of collision mesh
  FMOD_IMPEXP void SetOcclusion(float fDirectOcclusion, float fReverbOcclusion);

  /// \brief
  ///   Returns whether the collision mesh has a valid Fmod geometry
  inline bool IsValid() const { return m_pGeometry!=NULL; }

  ///
  /// @}
  ///

private:
  VFmodManager *m_pManager;
  hkvVec3 m_vScale;
  bool m_bActive;
  bool m_bDoubleSided;
  VSmartPtr<VBaseMesh> m_spMesh;
  float m_fDirectOcclusion, m_fReverbOcclusion;
  FMOD::Geometry* m_pGeometry;
};


/// \brief
///   Collection of geometry instances
class VFmodCollisionMeshInstanceCollection : public VRefCountedCollection<VFmodCollisionMeshInstance>
{
public:
  /// \brief
  ///   Returns the first occurrence (or NULL) of a collision mesh instance in this collection with specified object key. The object key can be set in vForge.
  FMOD_IMPEXP VFmodCollisionMeshInstance* SearchCollisionMesh(const char* szName) const;
};

#endif // VFMODCOLLISIONMESHINSTANCE_HPP_INCLUDED

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
