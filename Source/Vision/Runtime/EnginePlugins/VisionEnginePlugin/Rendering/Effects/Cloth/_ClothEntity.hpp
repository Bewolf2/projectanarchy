/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file _ClothEntity.hpp

#ifndef H_CLOTHENTITY_CL_INCLUDED
#define H_CLOTHENTITY_CL_INCLUDED


#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/Cloth/ClothMesh.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/Cloth/ClothDeformer.hpp>

extern VModule g_VisionEngineModule;

class VisClothDeformer_cl;
typedef VSmartPtr<VisClothDeformer_cl> VisClothDeformerPtr;

/// \brief
///   Entity class that adds cloth simulation capabilites to the model mesh.
///
/// This class is the native counterpart for the Cloth shape in vForge.
/// The simulation is based on proprietary simulation code and bypasses the pyhsics engine.
class ClothEntity_cl : public VisBaseEntity_cl 
{
public:
// standard entity functions:
  ClothEntity_cl();
  void InitFunction();
  void ThinkFunction();
  void DeInitFunction ();

  /// \brief
  ///   Loads a model mesh that is simulated as cloth. Should be a simple planar model
  EFFECTS_IMPEXP bool SetMeshModel(const char *szModelfile, const hkvVec3& vScaling, VString *pszError=NULL);

  /// \brief
  ///   Moves the whole cloth
  EFFECTS_IMPEXP void SetClothPosition(const hkvVec3& vNewPos);

  inline const hkvVec3& GetClothPosition() {return m_vCurrentPos;}
  EFFECTS_IMPEXP void SetClothOrientation(const hkvVec3& vNewOri);
  EFFECTS_IMPEXP void UpdateClothStatus();

  /// \brief
  ///   Sets the number of physics ticks used for simulation. 0 can be passed to simulate every frame (not recommended)
  inline void SetPhysicsTicks(int iTicks) {m_fPhysicsTicks=(float)iTicks;}

  /// \brief
  ///   Sets the amount of gravity
  inline void SetGravity(float fGravity) {m_fGravity=fGravity;}

  /// \brief
  ///   Flag that determines whether physics is only performed when the mesh is visible (performance optimization)
  inline void SetSimulateWhenVisible(bool bStatus) {m_bSimulateWhenVisible=bStatus;}

  /// \brief
  ///   Runs iTicks physics ticks directly after creation
  inline void SetInitialTickCount(int iTicks) {m_iInitialTickCount=m_iRemainingInitialTicks=iTicks;}
  EFFECTS_IMPEXP void RunInitialTicks();

  /// \brief
  ///   Access the underlying cloth simulation mesh
  inline VClothMesh* GetClothMesh() const {return m_spMesh;}

  /// \brief
  ///   Relaxes all forces
  inline void ResetForces() 
  {
    if (m_spMesh!=NULL) 
      m_spMesh->ResetForces();
  }

protected:
  void BaseInit();
  VClothMeshPtr m_spMesh;
  VisClothDeformerPtr m_spClothDeformer;
  float m_fTickPos;
  hkvVec3 m_vCurrentPos;
  hkvVec3 m_vCurrentOri;
  hkvVec3 m_vCurrentScaling;
  float m_fPhysicsTicks;
  float m_fGravity;
  BOOL m_bSimulateWhenVisible;
  int m_iInitialTickCount, m_iRemainingInitialTicks;

public:
  //serialization
  V_DECLARE_SERIAL_DLLEXP( ClothEntity_cl,  EFFECTS_IMPEXP );
  EFFECTS_IMPEXP virtual void Serialize( VArchive &ar ) HKV_OVERRIDE;

  // entity
  IMPLEMENT_OBJ_CLASS(ClothEntity_cl);
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
