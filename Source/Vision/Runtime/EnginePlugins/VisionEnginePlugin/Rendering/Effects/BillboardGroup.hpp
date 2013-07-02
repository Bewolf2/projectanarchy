/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file BillboardGroup.hpp

#ifndef BILLBOARDGROUP_HPP_INCLUDED
#define BILLBOARDGROUP_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/EffectsModule.hpp>

// assume we have cross-style billboards with 8 vertices per billboard
#define MAX_BILLBOARDS_PER_GROUP    8192

/*
Other defines:

//target platforms that do not support spanning with shaders can use this define to use traditional cross-style billboards
#define BILLBOARDS_AS_CROSSES

// if additionally this is defined then each cross has a pseudo-random rotation
#define BILLBOARDS_CROSS_RANDOM_ROTATION

*/



#ifdef BILLBOARDS_AS_CROSSES
  #define BILLBOARD_TARGET_VERTEX VBillboardStaticMesh::VBillboardCrossVertex_t
#else
  #define BILLBOARD_TARGET_VERTEX VBillboardStaticMesh::VBillboardVertex_t
#endif

// custom material flags
#define MATERIALFLAG_NODEFERREDLIGHTING   V_BIT(0)

class VBillboardGroupInstance;

/// \brief
///   Special type of VisStaticMesh_cl class that holds a mesh of a billboard group. Usually VBillboardGroupInstance is used for instantiation
class VBillboardStaticMesh : public VisStaticMesh_cl
{
public:

  /// \brief
  ///  Vertex structure for a single billboard corner vertex 
  struct VBillboardVertex_t
  {
    hkvVec3 vPivot;
    VColorRef iColor;
    hkvVec2 vTexCoord;
    hkvVec2 vCorner;

    // uninitialized constructor
    inline VBillboardVertex_t() : vPivot(hkvNoInitialization),iColor(false),vTexCoord(hkvNoInitialization),vCorner(hkvNoInitialization)
    {
    }

    // pivot constructor
    inline VBillboardVertex_t(const hkvVec3& vPos, VColorRef color)
    {
      vPivot = vPos;
      iColor = color;
    }

    inline void GetPivotBoundingBox(hkvAlignedBBox &bbox, float fMaxRadX, float fMaxRadY) const
    {
      hkvVec3 r(fMaxRadX,fMaxRadX,fMaxRadY);
      bbox.m_vMin = vPivot-r;
      bbox.m_vMax = vPivot+r;
    }

    inline void SetPivot(const VBillboardVertex_t& pivot, float cornerX, float cornerY, float u, float v)
    {
      *this = pivot;
      vCorner.set(cornerX,cornerY);
      vTexCoord.set(u,v);
    }

    static inline void GetDesc(VisMBVertexDescriptor_t &desc)
    {
      desc.m_iStride = sizeof(VBillboardVertex_t);
      desc.m_iPosOfs = offsetof(VBillboardVertex_t,vPivot);
      desc.m_iColorOfs = offsetof(VBillboardVertex_t,iColor);
      desc.m_iTexCoordOfs[0] = offsetof(VBillboardVertex_t,vTexCoord);
      desc.m_iTexCoordOfs[1] = offsetof(VBillboardVertex_t,vCorner);
    }
    
    inline static const char *GetFormatString() {return "ffffffff";}

  };

#ifdef BILLBOARDS_AS_CROSSES

  /// \brief
  ///  Vertex structure for a single billboard corner vertex when using billboard crosses
  struct VBillboardCrossVertex_t
  {
    hkvVec3 vPos;
    hkvVec2 vTexCoord;
    VColorRef iColor;
    static inline void GetDesc(VisMBVertexDescriptor_t &desc)
    {
      desc.m_iStride = sizeof(VBillboardCrossVertex_t);
      desc.m_iPosOfs = offsetof(VBillboardCrossVertex_t,vPos);
      desc.m_iColorOfs = offsetof(VBillboardCrossVertex_t,iColor);
      desc.m_iTexCoordOfs[0] = offsetof(VBillboardCrossVertex_t,vTexCoord);
    }
    static inline void Create2CrossingVertices(VBillboardCrossVertex_t *pDest, const VBillboardVertex_t &src, float fSeed)
    {
      pDest[0].iColor = pDest[1].iColor = src.iColor;
      pDest[0].vTexCoord = pDest[1].vTexCoord = src.vTexCoord;

    #ifdef BILLBOARDS_CROSS_RANDOM_ROTATION
      float fAngle = fSeed*fSeed;
      float sf = hkvMath::sinRad (fAngle)*src.vCorner.x;
      float cf = hkvMath::cosRad (fAngle)*src.vCorner.x;
      pDest[0].vPos.Set(src.vPivot.x + cf + sf,src.vPivot.y - sf + cf, src.vPivot.z + src.vCorner.y);
      pDest[1].vPos.Set(src.vPivot.x - sf + cf,src.vPivot.y - cf - sf, src.vPivot.z + src.vCorner.y);
    #else
      pDest[0].vPos.Set(src.vPivot.x + src.vCorner.x,src.vPivot.y, src.vPivot.z + src.vCorner.y);
      pDest[1].vPos.Set(src.vPivot.x,src.vPivot.y + src.vCorner.x, src.vPivot.z + src.vCorner.y);
    #endif
    }
  };

#endif

  /// \brief
  ///   Empty constructor
  VBillboardStaticMesh();

  /// \brief
  ///   Initializes the mesh - allocates a certain amount of billboards
  void InitMesh(int iBillboardCount, VBillboardGroupInstance *pInstanceCreateDefaultFX);

  EFFECTS_IMPEXP VCompiledEffect *CreateBillboardEffect(VBillboardGroupInstance *pInstance);

  inline void ReassignShader(VBillboardGroupInstance *pInstance)
  {
    Surface().SetEffect(CreateBillboardEffect(pInstance));
  }

  // members:
protected:
  friend class VBillboardGroupInstance;
  VisSurface_cl &Surface()
  {
    return *GetSurface(0);
  }
};



/// \brief
///   Special type of VisStaticMeshInstance_cl class that renders a billboard group
///
/// Instances of billboard groups can be created and edited in vForge. This class is the native counterpart for
//  the vForge shape.
class VBillboardGroupInstance : public VisStaticMeshInstance_cl
{
public:

  ///
  /// @name Constructor
  /// @{
  ///

  /// \brief
  ///   Constructor that allocates a certain amount of billboards in the mesh
  EFFECTS_IMPEXP VBillboardGroupInstance(int iBillboardCount=0, bool bCreateDefaultFX=false);

  ///
  /// @}
  ///

  ///
  /// @name Billboard Group Properties
  /// @{
  ///

  /// \brief
  ///   Sets the texture object that should be used for the billboards in this group
  inline void SetTexture(VTextureObject *pTex); 

  /// \brief
  ///   Sets the blend mode (e.g. VIS_TRANSP_ALPHA)
  EFFECTS_IMPEXP void SetTransparency(VIS_TransparencyType transp, bool bDeferredUseLighting=true); 

  /// \brief
  ///   Re-apply the shader. Call this if for instance after changing to deferred mode
  inline void ReassignShader();

  /// \brief
  ///   Use this function to set near and far clip distance. This function actually updates both the mesh clipping and the shader parameter for individual clipping. Pass <=0.0 for no clipping.
  EFFECTS_IMPEXP void SetClipDistances(float fNear, float fFar);

  /// \brief
  ///   Assign a custom shader effect to this instance
  inline void SetCustomEffect(VCompiledEffect *pBillboardFX);

  ///
  /// @}
  ///

  ///
  /// @name Custom Mesh Locking
  /// @{
  ///

  /// \brief
  ///   Locks the vertices of the underlying billboard mesh
  inline BILLBOARD_TARGET_VERTEX *LockVertices();

  /// \brief
  ///   Unlocks the vertices of the underlying billboard mesh
  inline void UnLockVertices();

  /// \brief
  ///   Set the actual number of billboard instances to be rendered
  EFFECTS_IMPEXP void SetUsedBillboardCount(int iBillboardCount, const hkvAlignedBBox &groupbox);

  ///
  /// @}
  ///



#ifdef SUPPORTS_LIT_FILE_LOADING
  EFFECTS_IMPEXP virtual bool GatherLightmapInfo(VLightmapSceneInfo &sceneInfo);
#endif
  void UpdateMeshClipDistances();

  //serialization
  V_DECLARE_SERIAL_DLLEXP( VBillboardGroupInstance,  EFFECTS_IMPEXP )
  EFFECTS_IMPEXP virtual void Serialize( VArchive &ar );
  hkvVec2 m_vClipDist; // near, far
};


/////////////////////////////////////////////////////////////////////
// INLINES:
/////////////////////////////////////////////////////////////////////


inline BILLBOARD_TARGET_VERTEX *VBillboardGroupInstance::LockVertices()
{
  VBillboardStaticMesh *pSM = (VBillboardStaticMesh *)GetMesh();
  VASSERT(pSM != NULL && pSM->GetMeshBuffer());
  return (BILLBOARD_TARGET_VERTEX *)pSM->GetMeshBuffer()->LockVertices(VIS_LOCKFLAG_DISCARDABLE);
}

inline void VBillboardGroupInstance::UnLockVertices()
{
  VBillboardStaticMesh *pSM = (VBillboardStaticMesh *)GetMesh();
  VASSERT(pSM != NULL && pSM->GetMeshBuffer());
  pSM->GetMeshBuffer()->UnLockVertices();
}

inline void VBillboardGroupInstance::SetTexture(VTextureObject *pTex)
{
  VBillboardStaticMesh *pSM = (VBillboardStaticMesh *)GetMesh();
  pSM->Surface().m_spDiffuseTexture = pTex;
}



void VBillboardGroupInstance::ReassignShader()
{
  ((VBillboardStaticMesh *)GetMesh())->ReassignShader(this);
}

inline void VBillboardGroupInstance::SetCustomEffect(VCompiledEffect *pBillboardFX)
{
  VBillboardStaticMesh *pMesh = (VBillboardStaticMesh *)GetMesh();
  VASSERT(pMesh);
  if (pBillboardFX==NULL)
    pBillboardFX = pMesh->CreateBillboardEffect(this);
  pMesh->Surface().SetEffect(pBillboardFX);
}


/* 
/////////////////////////////////////////////////////////////////////
// SAMPLE CODE:
/////////////////////////////////////////////////////////////////////
  int iBillboardCount = 1000;
  VBillboardGroupInstance *bb = new VBillboardGroupInstance(iBillboardCount, true);
  hkvAlignedBBox groupbox;
  VBillboardStaticMesh::VBillboardVertex_t *pV = bb->LockVertices();
  for (int i=0;i<iBillboardCount;i++,pV+=4)
  {
    hkvAlignedBBox bbox;
    VBillboardStaticMesh::VBillboardVertex_t pivot(hkvVec3(Vision::Game.GetFloatRandNeg()*2000.f,Vision::Game.GetFloatRandNeg()*2000.f,0.f),V_RGBA_WHITE);
    pV[0].SetPivot(pivot, -10.f,-20.f, 0.f,0.f);
    pV[1].SetPivot(pivot,  10.f,-20.f, 1.f,0.f);
    pV[2].SetPivot(pivot, -10.f, 20.f, 0.f,1.f);
    pV[3].SetPivot(pivot,  10.f, 20.f, 1.f,1.f);
    pivot.GetPivotBoundingBox(bbox,10.f,20.f);
    groupbox.Inflate(bbox);
  }
  bb->UnLockVertices();
  bb->SetUsedBillboardCount(iBillboardCount,groupbox);
*/

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
