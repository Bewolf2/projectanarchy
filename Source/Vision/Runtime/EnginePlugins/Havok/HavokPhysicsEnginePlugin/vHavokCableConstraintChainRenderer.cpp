/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/HavokPhysicsEnginePluginPCH.h>

#include <Vision/Runtime/Engine/Mesh/VisApiVertexBuffer.hpp>

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokConstraintChain.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokCableConstraintChainRenderer.hpp>

// ----------------------------------------------------------------------------
// vHavokCableConstraintChainRenderer
// ----------------------------------------------------------------------------
V_IMPLEMENT_SERIAL(vHavokCableConstraintChainRenderer, vHavokConstraintChainRendererBase, 0, &g_vHavokModule);

// ----------------------------------------------------------------------------
const unsigned int vHavokCableConstraintChainRenderer::s_iSerialVersion = 2;

// ----------------------------------------------------------------------------
vHavokCableConstraintChainRenderer::vHavokCableConstraintChainRenderer(
  int iComponentFlags)
: vHavokConstraintChainRendererBase(iComponentFlags)
{
  VerticesPerRing = 8;
  RingsPerLink = 2;
}

// ----------------------------------------------------------------------------
void vHavokCableConstraintChainRenderer::CommonDeinit()
{
  if (m_spChainEntity)
    m_spChainEntity->Remove();
  m_spChainEntity = NULL;
  m_spChainMesh = NULL;

  m_iLastKnownNumLinks = 0;
  
  vHavokConstraintChainRendererBase::CommonDeinit();
}

// ----------------------------------------------------------------------------
bool vHavokCableConstraintChainRenderer::DoInit()
{
  if (!vHavokConstraintChainRendererBase::DoInit())
    return false;
  
  return RebuildModel();
}

// ----------------------------------------------------------------------------
void vHavokCableConstraintChainRenderer::OnUpdate()
{
  if (m_pConstraintChain && (m_pConstraintChain->GetNumLinks() != m_iLastKnownNumLinks))
    RebuildModel();
}

// ----------------------------------------------------------------------------
void vHavokCableConstraintChainRenderer::OnVariableValueChanged(
  VisVariable_cl *pVar, const char * value)
{
  CommonInit();
}

// ----------------------------------------------------------------------------
bool vHavokCableConstraintChainRenderer::RebuildModel()
{
  m_spChainMesh = NULL;

  if (!m_pConstraintChain || !m_pConstraintChain->IsValid())
    return false;
  
  int iNumLinks = m_pConstraintChain->GetNumLinks();
  m_iLastKnownNumLinks = iNumLinks;

  if (iNumLinks < 1)
  {
    Vision::Error.Warning("vHavok: Can't create cable rendering - constraint chain has no links.");
    return false;
  }

  int iVerticesPerRing = hkvMath::Max(6, VerticesPerRing);
  int iRingsPerLink = hkvMath::Max(1, RingsPerLink);

//#define ENDS_ONLY

  // Vertices for each ring (final vertex is at the position of the first
  // vertex), times the number of rings
#ifdef ENDS_ONLY
  int iNumVertices = 0;
#else
  int iNumVertices = ((iNumLinks * iRingsPerLink) + 1) * (iVerticesPerRing + 1);
#endif
  // Extra vertices for the end surfaces
  iNumVertices += (iVerticesPerRing + 1) * 2;
  if (iNumVertices > 65535)
  {
    Vision::Error.Warning("vHavok: Can't create cable rendering - too many vertexes.");
    return false;
  }

#ifdef ENDS_ONLY
  int iNumTriangles = 0;
#else
  int iNumTriangles = iRingsPerLink * iNumLinks * iVerticesPerRing * 2;
#endif
  // Extra triangles for the end surfaces
  iNumTriangles += iVerticesPerRing * 2;

  float fRadius = m_pConstraintChain->GetDiameter() / 2.f;
  float fCircumference = fRadius * 2.0f * hkvMath::pi ();
  
  float fRingHeight = m_pConstraintChain->GetLinkLength() / iRingsPerLink;
  float fTextureVPerRing = fRingHeight / fCircumference;

  // Load the model that provides the surface material of the cable
  VDynamicMeshPtr spTemplateMesh;
  if (!ModelFile.IsEmpty())
    spTemplateMesh = VDynamicMesh::LoadDynamicMesh(ModelFile);

  // Pre-calculate the coordinates, normals and texture offsets of the ring
  // and end cap vertices
  hkvVec3* rgvVertexTemplates = new hkvVec3[iVerticesPerRing + 1];
  hkvVec3* rgvNormalTemplates = new hkvVec3[iVerticesPerRing + 1];
  float *rgfTextureU = new float[iVerticesPerRing + 1];
  hkvVec2* rgvEndTexCoords = new hkvVec2[iVerticesPerRing];
  {
    float fMaxEndTexOffset = 1.f / (2.0f * hkvMath::pi ());
    for (int i = 0; i < iVerticesPerRing; ++i)
    {
      float fRatio = (float)i / (float)iVerticesPerRing;
      float fAngle = fRatio * 2.0f * hkvMath::pi ();
      rgvNormalTemplates[i].set(0.f, hkvMath::cosRad (fAngle), hkvMath::sinRad (fAngle));
      rgvVertexTemplates[i] = rgvNormalTemplates[i] * fRadius;
      rgfTextureU[i] = fRatio;
      rgvEndTexCoords[i].set(
        0.5f + (hkvMath::cosRad (fAngle) * fMaxEndTexOffset),
        0.5f + (hkvMath::sinRad (fAngle) * fMaxEndTexOffset));
    }
    rgvNormalTemplates[iVerticesPerRing] = rgvNormalTemplates[0];
    rgvVertexTemplates[iVerticesPerRing] = rgvVertexTemplates[0];
    rgfTextureU[iVerticesPerRing] = 1.f;
  }


  VColorRef cableColor(V_RGBA_YELLOW);
  hkvVec3 vTangent(1.f, 0.f, 0.f);

  int usageFlags = VIS_MEMUSAGE_STREAM;
  int bindFlags = VIS_BIND_SHADER_RESOURCE;
  
#ifdef _VR_DX11
  // Only specify this flag if this is a true DX11 card, since the engine currently
  // just passes it along to the device. This fails if the feature is not supported.
  // It is needed for Compute Shader Skinning
  if (Vision::Video.GetDXFeatureLevel() >= D3D_FEATURE_LEVEL_11_0)
  {
    usageFlags |= VIS_MEMUSAGE_UAV_BYTEADDRESS;
    bindFlags |= VIS_BIND_UNORDERED_ACCESS;
  }
#endif

  VDynamicMeshBuilder meshBuilder(iNumVertices, iNumTriangles, iNumLinks + 2, 1, usageFlags, bindFlags);

  if (spTemplateMesh && (spTemplateMesh->GetSurfaceCount() > 0))
    meshBuilder.CopySurfaceFrom(0, *spTemplateMesh->GetSurface(0));

#ifndef ENDS_ONLY
  for (int iCurrentLink = 0; iCurrentLink < iNumLinks; ++iCurrentLink)
  {
    /// The vertices of a chain link are influenced by three bones.
    int iPrevBone = iCurrentLink;
    int iThisBone = iCurrentLink + 1;
    int iNextBone = iCurrentLink + 2;

    /// The last link has an additional ring of vertices at its end
    int iLocalNumRings = iCurrentLink == (iNumLinks - 1)
      ? iRingsPerLink + 1 : iRingsPerLink;

    /// Compute the vertices for each ring of the current chain link:
    for (int iCurrentRing = 0; iCurrentRing < iLocalNumRings; ++iCurrentRing)
    {
      /// Normalized offset of this ring from the start of the chain link (range 0..1)
      float fNormOffset = (float)iCurrentRing / (float)iRingsPerLink;

      /// Calculate the influence factors of the three bones that might affect
      /// the current ring
      float fPrevBoneWeight = hkvMath::Max(0.f, 0.5f - fNormOffset);
      float fThisBoneWeight = 1.f - hkvMath::Abs (fNormOffset - 0.5f);
      float fNextBoneWeight = hkvMath::Max(0.f, fNormOffset - 0.5f);

      /// Compute the vertices of one ring. The start/end vertices are at the same
      /// position, since two different texture coordinates are needed here.
      int iStartVertex = meshBuilder.GetNextVertexIndex();
      for (int iLocalVertex = 0; iLocalVertex <= iVerticesPerRing; ++iLocalVertex)
      {
        hkvVec3 vPos(rgvVertexTemplates[iLocalVertex]);

        // Texture v coordinate increases throughout the chain
        float fTextureV = ((iCurrentLink * iRingsPerLink) + iCurrentRing) * fTextureVPerRing;
        hkvVec2 vTexCoords(rgfTextureU[iLocalVertex], fTextureV);

        // Add vertex data...
        int iCurrentVertex = meshBuilder.AddVertex(vPos, 
          rgvNormalTemplates[iLocalVertex], vTangent, vTexCoords, cableColor);
        VASSERT(iCurrentVertex >= 0);

        // ...and bone weights.
        if (fPrevBoneWeight > 0.f)
          meshBuilder.AddBoneWeight(iPrevBone, fPrevBoneWeight);
        meshBuilder.AddBoneWeight(iThisBone, fThisBoneWeight);
        if (fNextBoneWeight > 0.f)
          meshBuilder.AddBoneWeight(iNextBone, fNextBoneWeight);

        // Unless we are at the last vertex in a ring or at the last ring of the
        // last link, compute the vertex indices that make up the triangles for 
        // the cable.
        if ((iCurrentRing < iRingsPerLink) && (iLocalVertex < iVerticesPerRing))
        {
          unsigned short i1, i2, i3;
          int iNextRing = iStartVertex + iVerticesPerRing + 1;

          i1 = iStartVertex + iLocalVertex;
          i2 = iStartVertex + iLocalVertex + 1;
          i3 = iNextRing + iLocalVertex;
          meshBuilder.AddTriangle(i1, i2, i3);

          i1 = iStartVertex + iLocalVertex + 1;
          i2 = iNextRing + iLocalVertex + 1;
          i3 = iNextRing + iLocalVertex;
          meshBuilder.AddTriangle(i1, i2, i3);
        }

        //iCurrentVertex++;
      } // End of vertex loop
    } // End of rings per chain link loop
  } // End of chain link loop
#endif

  // Add the end surfaces
  {
    hkvVec3 vEndNormal = hkvVec3(-1.f, 0.f, 0.f);
    hkvVec3 vEndTangent = hkvVec3(0.f, 1.f, 0.f);

    // Center vertex (Top)
    int iCenterVertexIndex = meshBuilder.GetNextVertexIndex();
    meshBuilder.AddVertex(hkvVec3::ZeroVector (), vEndNormal, 
      vEndTangent, hkvVec2 (0.5f, 0.5f), cableColor);
    meshBuilder.AddBoneWeight(0, 0.5f);
    meshBuilder.AddBoneWeight(1, 0.5f);

    // Outer vertices (Top)
    int iStartVertexIndex = meshBuilder.GetNextVertexIndex();
    for (int iLocalVertex = 0; iLocalVertex < iVerticesPerRing; ++iLocalVertex)
    {
      meshBuilder.AddVertex(rgvVertexTemplates[iLocalVertex],
        vEndNormal, vEndTangent, rgvEndTexCoords[iLocalVertex], cableColor);
      meshBuilder.AddBoneWeight(0, 0.5f);
      meshBuilder.AddBoneWeight(1, 0.5f);
      int iNextLocalVertex = iLocalVertex + 1;
      if (iNextLocalVertex == iVerticesPerRing)
        iNextLocalVertex = 0;

      meshBuilder.AddTriangle(iCenterVertexIndex, 
        iStartVertexIndex + iLocalVertex, iStartVertexIndex + iNextLocalVertex);
    }


    // Bottom cap: normal points the other way (tangent remains)
    vEndNormal.set(1.f, 0.f, 0.f);

    // Center vertex (Bottom)
    iCenterVertexIndex = meshBuilder.GetNextVertexIndex();
    meshBuilder.AddVertex(hkvVec3::ZeroVector (), vEndNormal, 
      vEndTangent, hkvVec2 (0.5f, 0.5f), cableColor);
    meshBuilder.AddBoneWeight(iNumLinks, 0.5f);
    meshBuilder.AddBoneWeight(iNumLinks + 1, 0.5f);

    // Outer vertices (Top)
    iStartVertexIndex = meshBuilder.GetNextVertexIndex();
    for (int iLocalVertex = 0; iLocalVertex < iVerticesPerRing; ++iLocalVertex)
    {
      meshBuilder.AddVertex(rgvVertexTemplates[iLocalVertex],
        vEndNormal, vEndTangent, rgvEndTexCoords[iLocalVertex], cableColor);
      meshBuilder.AddBoneWeight(iNumLinks, 0.5f);
      meshBuilder.AddBoneWeight(iNumLinks + 1, 0.5f);
      int iNextLocalVertex = iLocalVertex + 1;
      if (iNextLocalVertex == iVerticesPerRing)
        iNextLocalVertex = 0;

      meshBuilder.AddTriangle(iCenterVertexIndex, 
        iStartVertexIndex + iLocalVertex, iStartVertexIndex + iNextLocalVertex);
    }
  }

  V_SAFE_DELETE_ARRAY(rgvVertexTemplates);
  V_SAFE_DELETE_ARRAY(rgvNormalTemplates);
  V_SAFE_DELETE_ARRAY(rgfTextureU);
  V_SAFE_DELETE_ARRAY(rgvEndTexCoords);

  VASSERT(meshBuilder.GetNextVertexIndex() == iNumVertices);
  VASSERT(meshBuilder.GetNextIndexIndex() == (iNumTriangles * 3));

  m_spChainMesh = meshBuilder.Finalize();
  VASSERT(m_spChainMesh);

  // Create the entity and the animation state
  if (!m_spChainEntity)
  {
    m_spChainEntity = static_cast<vHavokCableConstraintChainEntity*>(
      Vision::Game.CreateEntity("vHavokCableConstraintChainEntity", hkvVec3::ZeroVector ()));
  }
  m_spChainEntity->SetConstraintChain(m_pConstraintChain);
  m_spChainEntity->SetMesh(m_spChainMesh);
  m_spChainEntity->SetCastShadows(CastDynamicShadows);

  VisAnimFinalSkeletalResult_cl* pFinalSkeletalResult;
  VisAnimConfig_cl* pAnimConfig = VisAnimConfig_cl::CreateSkeletalConfig(m_spChainMesh, &pFinalSkeletalResult);
  m_spChainEntity->SetAnimConfig(pAnimConfig);

  return true;
}

// ----------------------------------------------------------------------------
void vHavokCableConstraintChainRenderer::Serialize(VArchive &ar)
{
  vHavokConstraintChainRendererBase::Serialize(ar);

  if (ar.IsLoading())
  {
    unsigned int iVersion = 0;
    ar >> iVersion;
    VASSERT_MSG((iVersion > 0) && (iVersion <= s_iSerialVersion), "Invalid version of serialized data!");

    ar >> VerticesPerRing;
    ar >> RingsPerLink;
    ar >> ModelFile;
    if (iVersion >= 2)
    {
      ar >> CastDynamicShadows;
    }
  }
  else
  {
    ar << s_iSerialVersion;
    ar << VerticesPerRing;
    ar << RingsPerLink;
    ar << ModelFile;
    ar << CastDynamicShadows;
  }
}

// ----------------------------------------------------------------------------
START_VAR_TABLE(vHavokCableConstraintChainRenderer, vHavokConstraintChainRendererBase, "vHavok constraint chain renderer (renders the whole chain as a cable)", VFORGE_HIDECLASS, "Havok Constraint Chain Renderer (Cable)")
  DEFINE_VAR_INT(vHavokCableConstraintChainRenderer, VerticesPerRing, "Number of vertices in each of the rings that form the cable's outline.", "8", 0, NULL);
  DEFINE_VAR_INT(vHavokCableConstraintChainRenderer, RingsPerLink, "Number of vertex rings defining the cable's outline for each chain link.", "2", 0, NULL);
  DEFINE_VAR_VSTRING(vHavokCableConstraintChainRenderer, ModelFile, "Model file from which the material definition for the cable's surface is taken", "", 0, 0, "assetpicker(Model)");
  DEFINE_VAR_BOOL(vHavokCableConstraintChainRenderer, CastDynamicShadows, "Determine if a dynamic shadow should be rendered for the cable", "FALSE", 0, NULL);
END_VAR_TABLE

// ----------------------------------------------------------------------------
// vHavokCableConstraintChainEntity
// ----------------------------------------------------------------------------
V_IMPLEMENT_SERIAL(vHavokCableConstraintChainEntity, VisBaseEntity_cl, 0, &g_vHavokModule);

// ----------------------------------------------------------------------------
vHavokCableConstraintChainEntity::vHavokCableConstraintChainEntity()
{
  m_pConstraintChain = NULL;
  m_iLastKnownNumLinks = 0;
}

// ----------------------------------------------------------------------------
void vHavokCableConstraintChainEntity::SetConstraintChain(vHavokConstraintChain *pConstraintChain)
{
  m_pConstraintChain = pConstraintChain;
  m_iLastKnownNumLinks = m_pConstraintChain ? m_pConstraintChain->GetNumLinks() : 0;
}

// ----------------------------------------------------------------------------
void vHavokCableConstraintChainEntity::ThinkFunction()
{
  // Check prerequisites
  if (!m_pConstraintChain)
    return;
  if (m_pConstraintChain->GetNumLinks() != m_iLastKnownNumLinks)
    return;
  unsigned int iNumLinks = m_pConstraintChain->GetNumLinks();
  if (iNumLinks < 1)
    return;

  VisAnimConfig_cl *pAnimConfig = GetAnimConfig();
  if (!pAnimConfig)
    return;
  VisAnimFinalSkeletalResult_cl *pAnimResult = pAnimConfig->GetFinalResult();
  if (!pAnimResult)
    return;

  hkvVec3* pLinkPositions = new hkvVec3[iNumLinks];
  hkvMat3 *pLinkRotations = new hkvMat3[iNumLinks];
  m_pConstraintChain->GetLinkTransforms(pLinkRotations, pLinkPositions, true, false);

  hkvVec3 vTrans;
  hkvMat3 mRot;
  hkvQuat qRot;

  // Extrapolate a virtual link before the first one
  {
    mRot = pLinkRotations[0];
    vTrans = pLinkPositions[0];
    hkvVec3 vExtraTrans(-m_pConstraintChain->GetLinkLength(), 0, 0);
    vTrans += mRot * vExtraTrans;
    pAnimResult->SetCustomBoneTranslation(0, vTrans);
    qRot.setFromMat3(mRot);
    pAnimResult->SetCustomBoneRotation(0, qRot);
  }

  // When iterating the actual links, build the visibility bounding box
  // of the cable to be rendered
  hkvAlignedBBox newVisBBox;

  // Transfer the transform of each chain link to the corresponding bone in the
  // model's skeleton, and update the bounding box.
  for (unsigned int i = 0; i < iNumLinks; ++i)
  {
    mRot = pLinkRotations[i];
    vTrans = pLinkPositions[i];

    pAnimResult->SetCustomBoneTranslation(i+1, vTrans);

    qRot.setFromMat3(mRot);
    pAnimResult->SetCustomBoneRotation(i+1, qRot);

    if (i == 0)
    {
      newVisBBox.m_vMin = vTrans;
      newVisBBox.m_vMax = vTrans;
    }
    else
    {
      newVisBBox.expandToInclude(vTrans);
    }

#if 0
    float fLength = 10.f;
    hkvVec3 v0 = (hkvVec3::ZeroVector () * mRot) + vTrans;
    hkvVec3 v1 = (hkvVec3(fLength, 0, 0) * mRot) + vTrans;
    hkvVec3 v1a = (hkvVec3(-fLength, 0, 0) * mRot) + vTrans;
    hkvVec3 v2 = (hkvVec3(0, fLength, 0) * mRot) + vTrans;

    Vision::Game.DrawSingleLine(v1a, v1, V_RGBA_YELLOW);
    Vision::Game.DrawSingleLine(v0, v2, V_RGBA_BLUE);
#endif
  }

  // Extrapolate another virtual link behind the last one
  {
    mRot = pLinkRotations[iNumLinks - 1];
    vTrans = pLinkPositions[iNumLinks - 1];
   
    hkvVec3 vExtraTrans(m_pConstraintChain->GetLinkLength(), 0, 0);
    vTrans += mRot * vExtraTrans;
    pAnimResult->SetCustomBoneTranslation(m_pConstraintChain->GetNumLinks() + 1, vTrans);
    qRot.setFromMat3(mRot);
    pAnimResult->SetCustomBoneRotation(m_pConstraintChain->GetNumLinks() + 1, qRot);
  }

  V_SAFE_DELETE_ARRAY(pLinkPositions);
  V_SAFE_DELETE_ARRAY(pLinkRotations);

  // Inflate the bounding box by the length of half a chain link and the
  // cable radius; then set it.
  float fInflateAmount = 
    (m_pConstraintChain->GetLinkLength() + m_pConstraintChain->GetDiameter()) / 2.f;
  newVisBBox.addBoundary(hkvVec3 (fInflateAmount));
  SetCurrentVisBoundingBox(newVisBBox);
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
