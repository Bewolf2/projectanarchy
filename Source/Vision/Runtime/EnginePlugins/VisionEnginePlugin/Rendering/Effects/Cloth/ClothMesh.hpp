/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file ClothMesh.hpp

#ifndef CLOTHMESH_HPP_INCLUDED
#define CLOTHMESH_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Particles/ParticleConstraint.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Particles/ParticleGroupBase.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/EffectsModule.hpp>


#ifndef _VISION_DOC

class VClothMesh;
typedef VSmartPtr<VClothMesh> VClothMeshPtr;

// the internal spring structure
struct Spring_t
{
  void Set(int iV1, int iV2, float fLength)
  {
    VASSERT(iV1!=iV2);
    fDefaultLength = fLength;
    iVertexIndex[0] = iV1;
    iVertexIndex[1] = iV2;
  }

  float fDefaultLength;
  unsigned short iVertexIndex[2];
};

#define INDEX_UNINITIALIZED 0xffff

struct ClothParticle_t : public Particle_t
{
  ClothParticle_t()
  {
    memset(this,0,sizeof(ClothParticle_t));
    valid = 1;
    size = 1.f;
    SetNormalIndices(INDEX_UNINITIALIZED,INDEX_UNINITIALIZED);
    velocity[0] = velocity[1] = velocity[2] = 0.f;
  }
  inline void SetNormalIndices(unsigned short v1,unsigned short v2)
  {
    unsigned short *pInd = (unsigned short *)&color; ///< write into this int32
    pInd[0] = v1; pInd[1] = v2;
  }
  inline void GetNormalIndices(unsigned short &v1,unsigned short &v2) const
  {
    unsigned short *pInd = (unsigned short *)&color; ///< write into this int32
    v1 = pInd[0]; v2 = pInd[1];
  }
  inline unsigned short GetNormalIndex(int iIndex) const
  {
    VASSERT(iIndex==0 || iIndex==1);
    unsigned short *pInd = (unsigned short *)&color; ///< write into this int32
    return pInd[iIndex];
  }
  inline void SetNormalIndex(int iIndex, unsigned short v)
  {
    VASSERT(iIndex==0 || iIndex==1);
    unsigned short *pInd = (unsigned short *)&color; ///< write into this int32
    pInd[iIndex] = v;
  }
};

/// \brief
///    Constraint collection class that holds point constraints. Used for cloth simulation.
class VisPointConstraintList_cl : public VisParticleConstraintList_cl
{
public:
  int AddPointConstraint(VisParticleConstraintPoint_cl *pPoint, int iVertex)
  {
    VASSERT(iVertex>=0);
    int iIndex = AddConstraint(pPoint);
    m_iVertex[iIndex] = iVertex;
    return iIndex;
  }
  DynArray_cl<int>m_iVertex; ///< every point constraint in this list has a vertex reference

  V_DECLARE_SERIALX( VisParticleConstraint_cl, EFFECTS_IMPEXP );
  EFFECTS_IMPEXP void SerializeX( VArchive &ar );
};


/// \brief
///    Task class to asynchronously update cloth
class ClothMeshPhysicsTask_cl : public VThreadedTask
{
public:
  ClothMeshPhysicsTask_cl(VClothMesh *pMesh);
  virtual void Run(VManagedThread *pThread);
  VClothMesh *m_pMesh;
  float m_fDeltaTime;
  float m_fGravity;
  int m_iTickCount;
public:
  //type management
  inline ClothMeshPhysicsTask_cl() {}
  V_DECLARE_DYNCREATE_DLLEXP( ClothMeshPhysicsTask_cl,  EFFECTS_IMPEXP );
};


/// \brief
///    Cloth mesh simulation class. Usually associated with class ClothEntity_cl
class VClothMesh : public VRefCounter, public IVPhysicsParticleCollection_cl
{
public:
  EFFECTS_IMPEXP VClothMesh();
  EFFECTS_IMPEXP virtual ~VClothMesh();

  EFFECTS_IMPEXP void HandleMeshPhysics(float dtime, float fGravity);
  EFFECTS_IMPEXP void HandleSpringPhysics(float dtime, float fGravity);
  EFFECTS_IMPEXP bool CreateFromEntityModel(VisBaseEntity_cl *pEntity, const hkvVec3& vScaling);
  EFFECTS_IMPEXP void ComputeNormals();
  EFFECTS_IMPEXP static const char *GetLastError() {return g_sLastError;} ///< if CreateFromEntityModel failed

  // repositioning
  EFFECTS_IMPEXP void Translate(const hkvVec3& vMoveDelta, bool bHandleConstraints=true);
  EFFECTS_IMPEXP void Rotate(const hkvMat3 &rotMatrix, const hkvVec3& vCenter, bool bHandleConstraints=true);
  EFFECTS_IMPEXP void ResetForces();

  // access mesh properties:
  inline VisObjectVertexDelta_t *GetVertexDeltaList() const {return m_pVertexDelta;}
  inline int GetVertexCount() const {return m_iVertexCount;}
  inline const hkvAlignedBBox& GetBoundingBox() {if (!m_bBoxValid) CalcBoundingBox();return m_BoundingBox;}

  // constraints:
  EFFECTS_IMPEXP bool AddConstraint(VisParticleConstraint_cl *pConstraint, bool bCheckInfluence);
  EFFECTS_IMPEXP bool AddPointConstraint(VisParticleConstraintPoint_cl *pPoint, int iVertex=-1);
  EFFECTS_IMPEXP bool RemoveConstraint(VisParticleConstraint_cl *pConstraint);

  // debugging:
  EFFECTS_IMPEXP void RenderVertices();

  // serialize
  V_DECLARE_SERIALX( VClothMesh, EFFECTS_IMPEXP );
  EFFECTS_IMPEXP virtual void SerializeX( VArchive &ar );

  static VString g_sLastError;
protected:
  // overridden IVPhysicsParticleCollection_cl functions
  virtual int GetPhysicsParticleCount() const {return m_iVertexCount;}
  virtual int GetPhysicsParticleStride() const {return sizeof(ClothParticle_t);}
  virtual Particle_t *GetPhysicsParticleArray() const {return &m_pParticle[0];}
  virtual void DestroyParticle(Particle_t *pParticle,float fTimeDelta) {}

private:

  void FreeMesh();
  void FreeSprings()
  {
    V_SAFE_DELETE_ARRAY(m_pSpring);
    m_iSpringCount=0;
  }
  void AllocateSprings(int iCount) 
  {
    FreeSprings();
    m_iSpringCount=iCount;
    if (iCount>0) m_pSpring = new Spring_t[iCount];
  }
  void CalcBoundingBox()
  {
    m_BoundingBox.setInvalid();
    VisObjectVertexDelta_t *pDelta = m_pVertexDelta;
    for (int i=0;i<m_iVertexCount;i++,pDelta++)
      m_BoundingBox.expandToInclude((hkvVec3&) pDelta->delta);
    m_bBoxValid = true;
  }

  bool BuildVertexNormalReferences(unsigned short *pTriangleIndices, int iTriCount);
  bool GenerateSprings(unsigned short *pTempTriangleIndices, int iTriCount);

  // mesh data:
  int m_iVertexCount;
  VisObjectVertexDelta_t  *m_pVertexDelta;  ///< the vertex delta list
  ClothParticle_t         *m_pParticle;     ///< additional vertex information
  hkvVec3               *m_pLocalSpacePos;// model mesh in local space, used for rotations (not used in exported version)
  hkvAlignedBBox        m_BoundingBox;
  bool m_bBoxValid;

  // spring constraints
  int m_iSpringCount;
  Spring_t *m_pSpring;    ///< pointer to the spring structure

  // other physics constraints:

  // a list of constraints
  VisParticleConstraintList_cl m_Constraints;
  // a list of point constraints; they are a bit different because they need a per-assignment vertex index
  VisPointConstraintList_cl m_PointConstraints;

  friend class ClothMeshPhysicsTask_cl;
};

#endif // _VISION_DOC


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
