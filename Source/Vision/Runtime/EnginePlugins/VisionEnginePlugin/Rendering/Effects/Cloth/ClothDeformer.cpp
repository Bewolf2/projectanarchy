/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/Cloth/ClothDeformer.hpp>
#include <Vision/Runtime/Base/System/Memory/VMemDbg.hpp>


class VClothMesh_cl;

VisClothDeformer_cl::VisClothDeformer_cl(VisVertexDeformerStack_cl* pMeshDeformerStack, VClothMesh *pMesh, VisObject3D_cl *pParentObject)
:IVisVertexDeformer_cl(pMeshDeformerStack)
{
  m_pParentObject = pParentObject;
  m_spMesh = pMesh;
}


bool VisClothDeformer_cl::UpdateDeformerState(float fTimeDelta, VisAnimState_cl* pLocalAnimState)
{ 

  return true;
}


#define ADVANCE_VERTEXPOINTERS  pDestVertexPosition = (float*)(((char*)pDestVertexPosition) + sizeof(SkinningVertex_t)),\
                                pDestVertexNormal = (float*)(((char*)pDestVertexNormal) + sizeof(SkinningVertex_t))

bool VisClothDeformer_cl::UpdateDeformerResult(VisVertexAnimResult_cl* pVertexAnimResult)
{
  VISION_PROFILE_FUNCTION(VIS_PROFILE_ANIMSYS_RESULT_VERTEX_ANIM);
   
  if(m_spMesh == NULL)
    return false;

  //// fill vertexanimresult with the mesh data
  // destination buffer
  float *pDestVertexPosition;
  const int  iDestVertexPositionStride = pVertexAnimResult->GetDestVertexPosition(pDestVertexPosition);
  float *pDestVertexNormal;
  const int  iDestVertexNormalStride = pVertexAnimResult->GetDestVertexNormal(pDestVertexNormal);

  int iVertexCount = m_spMesh->GetVertexCount();
  VDynamicMesh *pMesh = pVertexAnimResult->GetMesh();
  VASSERT(pMesh->GetNumOfVertices() == m_spMesh->GetVertexCount());
  VisObjectVertexDelta_t *pVertexDelta = m_spMesh->GetVertexDeltaList();

  // copy mesh vertices into vertexanim result
  hkvVec3 tempNormal(hkvNoInitialization);
  hkvVec3 vTranslate;
  if (m_pParentObject)
    vTranslate = m_pParentObject->GetPosition();

  for(int i=0; i<iVertexCount; i++, pVertexDelta++, ADVANCE_VERTEXPOINTERS)
  {
    pDestVertexPosition[0] = pVertexDelta->delta[0] - vTranslate.x;
    pDestVertexPosition[1] = pVertexDelta->delta[1] - vTranslate.y;
    pDestVertexPosition[2] = pVertexDelta->delta[2] - vTranslate.z;
    tempNormal.set(pVertexDelta->normal[0], pVertexDelta->normal[1], pVertexDelta->normal[2]);
    tempNormal.normalizeIfNotZero();
    pDestVertexNormal[0] = tempNormal.x;
    pDestVertexNormal[1] = tempNormal.y;
    pDestVertexNormal[2] = tempNormal.z;
  }

  return true;
}



V_IMPLEMENT_SERIAL( VisClothDeformer_cl, VisTypedEngineObject_cl, 0, &g_VisionEngineModule );
void VisClothDeformer_cl::Serialize( VArchive &ar )
{
  VisTypedEngineObject_cl::Serialize(ar);
  if (ar.IsLoading())
  {
  }
  else
  {
  }
}

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
