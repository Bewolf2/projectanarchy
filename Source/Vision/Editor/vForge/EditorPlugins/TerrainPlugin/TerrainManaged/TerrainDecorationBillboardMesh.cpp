/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Editor/vForge/EditorPlugins/TerrainPlugin/TerrainManaged/TerrainManagedPCH.h>
#include <Vision/Editor/vForge/EditorPlugins/TerrainPlugin/TerrainManaged/TerrainDecorationBillboardMesh.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Geometry/TerrainDecorationInstance.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Application/Terrain.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Geometry/TerrainSector.hpp>
#include <Vision/Runtime/Base/System/Memory/VMemDbg.hpp>

VTerrainDecorationBillboardMeshFactory VTerrainDecorationBillboardMesh::g_BillboardMeshFactory;

/*
  // billboard model factory class (fake model to generate static billboard meshes per sector)
  VTerrainDecorationModelManager::GlobalManager().RegisterFactory(&VTerrainDecorationBillboardMesh::g_BillboardMeshFactory);
*/

IVTerrainDecorationModel* VTerrainDecorationBillboardMeshFactory::CreateModelWithFilename(const char *szFilename, const IVTerrainDecorationModel::ModelProperties_t *pProperties)
{
  if (!VFileHelper::HasExtension(szFilename,"atlas"))
    return NULL;

  VTerrainDecorationBillboardMesh *pRes = new VTerrainDecorationBillboardMesh(szFilename,pProperties);
  pRes->EnsureLoaded();
  return pRes;
}

bool VTerrainDecorationBillboardMeshFactory::DisposePerSectorObject(VisTypedEngineObject_cl *pPerSectorObj)
{
  if (pPerSectorObj->IsOfType(V_RUNTIME_CLASS(VBillboardDecorationSectorInstance)))
  {
    pPerSectorObj->DisposeObject();
    return true;
  }
  return false;
}

BOOL VTerrainDecorationBillboardMesh::Reload()
{
  m_pAtlas = TextureAtlas::CreateFromFile(ConversionUtils::VStringToString(GetFilename()));

  if (static_cast<TextureAtlas ^>(m_pAtlas) == nullptr)
  {
    m_bValidState = false;
    return TRUE;
  }

  VString sTexture;
  ConversionUtils::StringToVString(m_pAtlas->TextureFile,sTexture);

  m_spTexture = Vision::TextureManager.GetPlainWhiteTexture();
  if (sTexture.IsEmpty())
  {
    m_pAtlas->TextureSizeX = 128;
    m_pAtlas->TextureSizeY = 128;
  }
  else
  {
    m_spTexture = Vision::TextureManager.Load2DTexture(sTexture);
    m_pAtlas->TextureSizeX = m_spTexture->GetTextureWidth();
    m_pAtlas->TextureSizeY = m_spTexture->GetTextureHeight();
  }

  m_bValidState = true;
  float bx = m_pAtlas->BaseSizeX * 0.25f; // tweaked
  float by = m_pAtlas->BaseSizeY;
  m_LocalBBox.set(hkvVec3 (-bx,-bx,0), hkvVec3 (bx,bx,by));
  return TRUE;
}

BOOL VTerrainDecorationBillboardMesh::Unload()
{
  m_pAtlas = NULL;
  m_spTexture = NULL;
  return TRUE;
}

void VTerrainDecorationBillboardMesh::UpdateParameter()
{
  IVTerrainDecorationModel::UpdateParameter();
  m_iSupportedContextTypes &= ~(1<<VIS_CONTEXTUSAGE_DEPTHSHADOW); // shadows not yet supported for .model
}

void VTerrainDecorationBillboardMesh::RemoveSectorDecoration(VEditableTerrainSector *pSector)
{
  VASSERT(pSector);
  // first, see whether there is a group in the sector objects
  int iCount = pSector->m_iPerSectorObjectCount;
  pSector->m_iPerSectorObjectCount = 0;
  int iThisID = GetID();
  for (int i=0;i<iCount;i++)
  {
    VisTypedEngineObject_cl *pObj = pSector->m_PerSectorObjects.GetDataPtr()[i];
    if (pObj==NULL) continue;
    if (pObj->IsOfType(V_RUNTIME_CLASS(VBillboardDecorationSectorInstance)))
    {
      VBillboardDecorationSectorInstance *pGroup = (VBillboardDecorationSectorInstance *)pObj;
      if (pGroup->m_iModelID==iThisID)
      {
        pGroup->DisposeObject();
        continue;
      }
    }

    // leave in list
    pSector->m_PerSectorObjects.GetDataPtr()[pSector->m_iPerSectorObjectCount++] = pObj;
  }
}

void VTerrainDecorationBillboardMesh::OnEditorEvent(EditorEvent_e action, VTerrain *pTerrain, void *pDataA, void *pDataB) 
{
  IVTerrainDecorationModel::OnEditorEvent(action, pTerrain, pDataA, pDataB);

#ifdef WIN32
  // When vegetation is updated, refresh the billboard group
  if (action == SectorDecorationUpdateBegin || action == SectorDecorationDeleted)
  {
    // This code path is however never executed as there are no instances in a sector
    VEditableTerrainSector *pSector = (VEditableTerrainSector *)pDataA;
    RemoveSectorDecoration(pSector);
  }
  else if (action == SectorDecorationUpdateFinished)
  {
    VEditableTerrainSector *pSector = (VEditableTerrainSector *)pDataA;
    VASSERT(pSector);
    RemoveSectorDecoration(pSector); // Make sure the current mesh is removed because SectorDecorationUpdateBegin is never called

    // Create a new billboard mesh
    int iDecoCount = 0;
    VTerrainDecorationInstance *pDecoInst = pSector->GetAdditionalDecoration(iDecoCount);

    // First count
    int iRelevantCount = 0;
    for (int i=0; i<iDecoCount; i++)
    {
      if (pDecoInst[i].m_spModel == this)
        iRelevantCount++;
    }

    const float relCenterX = 0.5f;
    const float relCenterY = 0.9f;
    const float cx = -1.f + relCenterX;
    const float cy = -1.f + relCenterY;
    const float mx = hkvMath::Max(relCenterX, -cx);
    const float my = hkvMath::Max(relCenterY, -cy);

    // Then fill buffer
    int iStartIndex = 0;
    while (iRelevantCount>0 && static_cast<TextureAtlas ^>(m_pAtlas) != nullptr)
    {
      // Split into smaller groups (8k per group)
      int iThisGroupCount = hkvMath::Min(iRelevantCount, MAX_BILLBOARDS_PER_GROUP);
      iRelevantCount -= iThisGroupCount;
      VBillboardDecorationSectorInstance *pGroup = new VBillboardDecorationSectorInstance(this, iThisGroupCount, pSector);
      BILLBOARD_TARGET_VERTEX *pV = pGroup->LockVertices();

      hkvAlignedBBox groupBox;
      int iAdded = 0;
      for (int i=iStartIndex; i<iDecoCount ;i++,iStartIndex++)
      {
        if (pDecoInst[i].m_spModel == this)
        {
          const VTerrainDecorationInstance& decoObj(pDecoInst[i]);

          // Generate a deterministic random number to select an atlas entry
          const float fScale = decoObj.GetScaling();
          double dAtlasValue = decoObj.m_vPosition.x * 1000.0 * fScale;
          dAtlasValue = fabs(dAtlasValue);
          double integralPart;
          double decimalPart = modf(dAtlasValue, &integralPart);
          VASSERT(decimalPart<1.0 && decimalPart>=0);
          TextureAtlas::AtlasEntry ^pEntry = m_pAtlas->GetRandomEntry((float)decimalPart);

          const float sx = m_pAtlas->BaseSizeX * fScale * pEntry->ResultingScaleX;
          const float sy = m_pAtlas->BaseSizeY * fScale * pEntry->ResultingScaleY;
          
          hkvVec3 vObjPivot(hkvNoInitialization);
          decoObj.GetPosition(vObjPivot);
          VBillboardStaticMesh::VBillboardVertex_t pivot(vObjPivot, V_RGBA_WHITE);

          pV[0].SetPivot(pivot, sx * relCenterX, sy * relCenterY, pEntry->U1, pEntry->V1);
          pV[1].SetPivot(pivot, sx * cx,         sy * relCenterY, pEntry->U2, pEntry->V1);
          pV[2].SetPivot(pivot, sx * relCenterX, sy * cy,         pEntry->U1, pEntry->V2);
          pV[3].SetPivot(pivot, sx * cx,         sy * cy,         pEntry->U2, pEntry->V2);

          hkvAlignedBBox bbox;
          pivot.GetPivotBoundingBox(bbox, mx * sx, my * sy);
          groupBox.expandToInclude(bbox);
          pV+=4;
          iAdded++;
          if (iAdded == iThisGroupCount)
            break;
        }
      }

      pGroup->UnLockVertices();
      pGroup->SetUsedBillboardCount(iThisGroupCount,groupBox);
      pGroup->SetTexture(m_spTexture);
      pGroup->SetTransparency((VIS_TransparencyType)m_pAtlas->Transparency,true);
      pGroup->SetClipDistances(m_Properties.m_fNearClip, m_Properties.m_fFarClip);
      VCompiledEffect *pFX = ConversionUtils::GetShaderEffect(m_pAtlas->ShaderEffect, EFFECTFLAGS_NONE, NULL);
      pGroup->SetCustomEffect(pFX);
      pSector->AddPerSectorObject(pGroup);
    }
  }
#endif
}

void VTerrainDecorationBillboardMesh::RenderBatch(VTerrainVisibilityCollectorComponent *pInfoComp, VTerrainDecorationInstance **pInstList, int iCount, IVTerrainDecorationModel::RenderMode_e eRenderMode)
{
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
