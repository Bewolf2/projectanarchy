/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/HavokPhysicsEnginePluginPCH.h>

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokConstraintChain.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokChainConstraintChainRenderer.hpp>

// ----------------------------------------------------------------------------
// vHavokChainConstraintChainRenderer
// ----------------------------------------------------------------------------
V_IMPLEMENT_SERIAL(vHavokChainConstraintChainRenderer, vHavokConstraintChainRendererBase, 0, &g_vHavokModule);

// ----------------------------------------------------------------------------
const unsigned int vHavokChainConstraintChainRenderer::s_iSerialVersion = 1;

// ----------------------------------------------------------------------------
vHavokChainConstraintChainRenderer::vHavokChainConstraintChainRenderer(
  int iComponentFlags)
: vHavokConstraintChainRendererBase(iComponentFlags)
{
  Scaling.set (1.0f);
}

// ----------------------------------------------------------------------------
void vHavokChainConstraintChainRenderer::CommonDeinit()
{
  if (m_spChainEntity)
    m_spChainEntity->Remove();
  m_spChainEntity = NULL;
  m_spChainMesh = NULL;

  vHavokConstraintChainRendererBase::CommonDeinit();
}

// ----------------------------------------------------------------------------
bool vHavokChainConstraintChainRenderer::DoInit()
{
  if (!vHavokConstraintChainRendererBase::DoInit())
    return false;

  return ReloadModel();
}

// ----------------------------------------------------------------------------
void vHavokChainConstraintChainRenderer::DrawEntity(VPassType_e ePassType)
{
  // Code copied from VisionRenderLoop_cl::DrawEntitiesShaders()

  if (!m_spChainEntity->HasShadersForPass(ePassType))
    return;

  VisDrawCallInfo_t SurfaceShaderList[RLP_MAX_ENTITY_SURFACESHADERS];

  // Get a list of the corresponding pass type shader set
  VisShaderSet_cl *pShaderSet = m_spChainEntity->GetActiveShaderSet();
  if (pShaderSet == NULL)
    return;

  int iNumSurfaceShaders = pShaderSet->GetShaderAssignmentList(SurfaceShaderList, ePassType, RLP_MAX_ENTITY_SURFACESHADERS);
  VASSERT(iNumSurfaceShaders < RLP_MAX_ENTITY_SURFACESHADERS);
  if (iNumSurfaceShaders == 0)
    return;

  // If the model has lit surfaces, and if the shaders makes use of the lighting information, we need to set up
  // the global lights.
  if (m_spChainMesh->HasLitSurfaces())
  {
    if(pShaderSet->GetCombinedTrackingMask() & (VSHADER_TRACKING_LIGHTGRID_PS|VSHADER_TRACKING_LIGHTGRID_GS|VSHADER_TRACKING_LIGHTGRID_VS))
    {
      Vision::RenderLoopHelper.TrackLightGridInfo(m_spChainEntity);
    }
  }

  // Render the entity with the surface shader list
  Vision::RenderLoopHelper.RenderEntityWithSurfaceShaderList(m_spChainEntity, iNumSurfaceShaders, SurfaceShaderList);
}

// ----------------------------------------------------------------------------
void vHavokChainConstraintChainRenderer::OnRender()
{
  // Check prerequisites: entity, render context, visibility collector
  if (!m_spChainEntity)
    return;
  VisRenderContextPtr spContext = Vision::Contexts.GetCurrentContext();
  if (!spContext)
    return;
  IVisVisibilityCollectorPtr spVisCollector = spContext->GetVisibilityCollector();
  if (!spVisCollector)
    return;
  VisFrustum_cl const* pFrustum = spVisCollector->GetBaseFrustum();

  // Make entity visible for drawing
  m_spChainEntity->SetVisibleBitmask(VIS_ENTITY_VISIBLE);

  // For each chain link: move the entity bounding box accordingly,
  // check visibility, and then draw the entity there if visible.
  for (unsigned int i = 0; i < m_pConstraintChain->GetNumLinks(); ++i)
  {
    hkvMat3 mRot;
    hkvVec3 vTrans;
    m_pConstraintChain->GetLinkTransform(i, mRot, vTrans);
    m_spChainEntity->SetRotationMatrix(mRot);
    m_spChainEntity->SetPosition(vTrans);
    m_spChainEntity->UpdateVisBoundingBox();

    hkvAlignedBBox const& bBox = *m_spChainEntity->GetCurrentVisBoundingBoxPtr();
    if ((!pFrustum) || pFrustum->Overlaps(bBox))
    {
      DrawEntity(VPT_PrimaryOpaquePass);
      DrawEntity(VPT_SecondaryOpaquePass);
      DrawEntity(VPT_TransparentPass);
    }
  }

  // Hide entity for regular rendering
  m_spChainEntity->SetVisibleBitmask(VIS_ENTITY_INVISIBLE);
}

// ----------------------------------------------------------------------------
void vHavokChainConstraintChainRenderer::OnVariableValueChanged(
  VisVariable_cl *pVar, const char * value)
{
  CommonInit();
}

// ----------------------------------------------------------------------------
bool vHavokChainConstraintChainRenderer::ReloadModel()
{
  m_spChainMesh = NULL;

  if (ModelFile.IsEmpty())
    return true;

  m_spChainMesh = VDynamicMesh::LoadDynamicMesh(ModelFile);
  if (!m_spChainMesh)
    return false;

  m_spChainEntity = Vision::Game.CreateEntity("VisBaseEntity_cl", hkvVec3::ZeroVector ());
  m_spChainEntity->SetMesh(m_spChainMesh);
  m_spChainEntity->SetScaling(Scaling);
  m_spChainEntity->SetVisibleBitmask(VIS_ENTITY_INVISIBLE);
  m_spChainEntity->SetUseEulerAngles(FALSE);

  return true;
}

// ----------------------------------------------------------------------------
void vHavokChainConstraintChainRenderer::Serialize(VArchive &ar)
{
  vHavokConstraintChainRendererBase::Serialize(ar);

  if (ar.IsLoading())
  {
    unsigned int iVersion = 0;
    ar >> iVersion;
    VASSERT_MSG((iVersion > 0) && (iVersion <= s_iSerialVersion), "Invalid version of serialized data!");

    ar >> ModelFile;
    Scaling.SerializeAsVisVector (ar);
  }
  else
  {
    ar << s_iSerialVersion;
    ar << ModelFile;
    Scaling.SerializeAsVisVector (ar);
  }
}

// ----------------------------------------------------------------------------
START_VAR_TABLE(vHavokChainConstraintChainRenderer, vHavokConstraintChainRendererBase, "vHavok constraint chain renderer (renders the chain links as separate entities)", VFORGE_HIDECLASS, "Havok Constraint Chain Renderer (Chain)")
  DEFINE_VAR_VSTRING(vHavokChainConstraintChainRenderer, ModelFile, "Model file used to render each chain link", "", 0, 0, "assetpicker(Model)");
  DEFINE_VAR_VECTOR_FLOAT(vHavokChainConstraintChainRenderer, Scaling, "Scaling of the model used for each chain link.", "1.0, 1.0, 1.0", 0, NULL);
END_VAR_TABLE

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
