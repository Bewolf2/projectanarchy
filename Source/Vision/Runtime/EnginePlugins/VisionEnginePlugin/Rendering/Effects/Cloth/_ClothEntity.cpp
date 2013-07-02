/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/Cloth/_ClothEntity.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/Cloth/ClothDeformer.hpp>
#include <Vision/Runtime/Base/System/Memory/VMemDbg.hpp>


#pragma warning(push)  
#pragma warning(disable:4996)   //Disable deprecation warnings


ClothEntity_cl::ClothEntity_cl() : m_vCurrentScaling(1.f,1.f,1.f)
{
  m_bSimulateWhenVisible = TRUE;
  m_fGravity = 100.f;
  m_fPhysicsTicks = 50.f;
  m_fTickPos = Vision::Game.GetFloatRand();
  m_iInitialTickCount = m_iRemainingInitialTicks = 0;
}


void ClothEntity_cl::BaseInit()
{
  SetUseEulerAngles(FALSE);
}


void ClothEntity_cl::InitFunction()
{
  m_vCurrentPos = GetPosition();
  m_vCurrentOri = GetOrientation();
  BaseInit();
  if (!GetMesh())
    return;
  const char *szModel = GetMesh()->GetFilename();
  SetMeshModel(szModel,m_vCurrentScaling);
}


  
bool ClothEntity_cl::SetMeshModel(const char *szModelfile, const hkvVec3& vScaling, VString *pszError)
{
  VThreadedTask *pTask = GetPreparationTask();
  WAIT_UNTIL_FINISHED(pTask);

  m_vCurrentScaling = vScaling;
  bool bStatus = true;
  if (!szModelfile || !szModelfile[0])
  {
    m_spMesh = NULL;
    SetPreparationTask(NULL);
    V_SAFE_DELETE(pTask);
    return true;
  }
  else
  {
    const char *szCurrentModel = "";
    if (GetMesh())
      szCurrentModel = GetMesh()->GetFilename();

    // set new API model
    if (_stricmp(szCurrentModel,szModelfile)!=0)
      SetMesh(szModelfile);

    m_spMesh = new VClothMesh();
    if (!m_spMesh->CreateFromEntityModel(this,m_vCurrentScaling))
    {
      m_spMesh = NULL;
      bStatus = false;
    }

  }

  //// model changed so we need to create a new anim config
  VisAnimConfig_cl *pAnimConfig = new VisAnimConfig_cl(GetMesh());
  
  // add modifier to vertex deformer stacks
  VisVertexDeformerStack_cl *pVertexDeformerStack = new VisVertexDeformerStack_cl();
  pAnimConfig->SetVertexDeformerStack(pVertexDeformerStack);
  VisClothDeformer_cl* pClothDeformer = new VisClothDeformer_cl(pVertexDeformerStack, m_spMesh,this);
    pVertexDeformerStack->AddDeformer(pClothDeformer);

  SetAnimConfig(pAnimConfig);

#if defined(_VISION_PSP2)
  pAnimConfig->SetSkinningMode(VIS_SKINNINGMODE_SOFTWARE);
#endif

  if (m_spMesh)
    SetCurrentVisBoundingBox(m_spMesh->GetBoundingBox());

  if (!bStatus && pszError)
    *pszError = VClothMesh::GetLastError();

  if (pTask!=NULL)
  {
    WAIT_UNTIL_FINISHED(pTask);
    SetPreparationTask(NULL);
    V_SAFE_DELETE(pTask);
  }

  SetPreparationTask(new ClothMeshPhysicsTask_cl(m_spMesh));

  return bStatus;
}


void ClothEntity_cl::SetClothPosition(const hkvVec3& vNewPos)
{
  VThreadedTask *pTask = GetPreparationTask();
  WAIT_UNTIL_FINISHED(pTask);

  SetPosition(vNewPos);
  
  if (m_spMesh)
  {
    m_spMesh->Translate(vNewPos-m_vCurrentPos);
    SetCurrentVisBoundingBox(m_spMesh->GetBoundingBox());
  }

  //update the lightgrid offset, because cloth entity is located at (0,0,0)
  //SetRelativeLightGridSamplePos(vNewPos-GetPosition());

  m_vCurrentPos = vNewPos;
  HandleAnimations(0.f); // force a vertex buffer update
}


void ClothEntity_cl::SetClothOrientation(const hkvVec3& vNewOri)
{
  VThreadedTask *pTask = GetPreparationTask();
  WAIT_UNTIL_FINISHED(pTask);

  m_vCurrentOri = vNewOri;

  hkvMat3 rotMat (hkvNoInitialization);
  rotMat.setFromEulerAngles (vNewOri.z, vNewOri.y, vNewOri.x);

  if (m_spMesh)
  {
    m_spMesh->Rotate(rotMat,m_vCurrentPos);
    SetCurrentVisBoundingBox(m_spMesh->GetBoundingBox());
  }
  HandleAnimations(0.f); // force a vertex buffer update
}


void ClothEntity_cl::RunInitialTicks()
{
  ClothMeshPhysicsTask_cl *pTask = (ClothMeshPhysicsTask_cl *)GetPreparationTask();
  if (!pTask || m_iRemainingInitialTicks<1)
    return;
  WAIT_UNTIL_FINISHED(pTask);

  pTask->m_fDeltaTime = (m_fPhysicsTicks>0.f) ? (1.f/m_fPhysicsTicks) : 0.02f;
  pTask->m_fGravity = m_fGravity;
  pTask->m_iTickCount = m_iRemainingInitialTicks;
  Vision::GetThreadManager()->ScheduleTask(pTask, 3);
  m_iRemainingInitialTicks = 0; // otherwise applied in ThinkFunction again
}


void ClothEntity_cl::ThinkFunction()
{
  float dtime = Vision::GetTimer()->GetTimeDifference();
  if (!m_spMesh || (m_bSimulateWhenVisible && !WasVisibleInAnyLastFrame() && m_iRemainingInitialTicks==0))
    return;

  ClothMeshPhysicsTask_cl *pTask = (ClothMeshPhysicsTask_cl *)GetPreparationTask();
  WAIT_UNTIL_FINISHED(pTask);

  int iTickCount;
  float fTimeDelta;
  if (m_fPhysicsTicks>0.f)
  {
    m_fTickPos += dtime*m_fPhysicsTicks;
    iTickCount = (int)m_fTickPos;
    m_fTickPos -= (float)iTickCount;
    fTimeDelta = 1.f/m_fPhysicsTicks;
  } 
  else // every frame
  {
    fTimeDelta = dtime;
    iTickCount = 1;
  }

  iTickCount += m_iRemainingInitialTicks;
  m_iRemainingInitialTicks = 0;

  // Note: this order results in the visibility bounding box's status being one frame delayed, but it is
  // necessary in order to achieve an advantage when multithreading. 
  SetCurrentVisBoundingBox(m_spMesh->GetBoundingBox());

  if (pTask!=NULL && iTickCount > 0)
  {
    pTask->m_fDeltaTime = fTimeDelta;
    pTask->m_fGravity = m_fGravity;
    pTask->m_iTickCount = iTickCount;
    Vision::GetThreadManager()->ScheduleTask(pTask, 3);
  }
}

void ClothEntity_cl::DeInitFunction()
{
  VThreadedTask *pTask = GetPreparationTask();
  if (pTask!=NULL)
  {
    WAIT_UNTIL_FINISHED(pTask);
    SetPreparationTask(NULL);
    V_SAFE_DELETE(pTask);
  }

  m_spMesh = NULL;
}


V_IMPLEMENT_SERIAL( ClothEntity_cl, VisBaseEntity_cl, 0, &g_VisionEngineModule );
void ClothEntity_cl::Serialize( VArchive &ar )
{
  if (ar.IsLoading())
  {
    VisBaseEntity_cl::Serialize(ar);
    char iVersion,iModelType;
    char szModelFile[FS_MAX_PATH+1];
    int iReserved;
    bool bTemp;
    hkvVec4 vPos,vOri;
    ar >> iVersion;VASSERT(iVersion==0 || iVersion==1);
    ar >> iModelType;VASSERT(iModelType==0); // currently from file only. Later maybe planar subdiv.
    ar.ReadStringBinary(szModelFile,FS_MAX_PATH);
    ar >> vPos;
    ar >> vOri;
    if (iVersion>=1)
      m_vCurrentScaling.SerializeAsVisVector (ar);

    ar >> m_fPhysicsTicks;
    ar >> bTemp;m_bSimulateWhenVisible=bTemp;
    ar >> m_fGravity;

    BaseInit();
    
    m_vCurrentPos = vPos.getAsVec3 (); // do not apply delta in SetClothPosition
    SetMeshModel(szModelFile, m_vCurrentScaling);

    ar >> bTemp; // has mesh?
    if (bTemp)
    {
      VASSERT(m_spMesh!=NULL);
      m_spMesh->SerializeX(ar);;
    }
    ar >> m_iInitialTickCount;
    ar >> iReserved; VASSERT(iReserved==0);

    //SetClothPosition(vPos); 
    SetClothOrientation(vOri.getAsVec3 ());
    ResetForces();
    //SetRelativeLightGridSamplePos(vLGRelPos);
    m_iRemainingInitialTicks = m_iInitialTickCount;
    RunInitialTicks();
  } else
  {
    VisAnimConfigPtr spStoreConfig = GetAnimConfig();
    SetAnimConfig(NULL); // don't store the anim config
    VisBaseEntity_cl::Serialize(ar);

    ar << (char)1; // version for cloth entity loading
    ar << (char)0; // model type
    if (GetMesh()!=NULL)
      ar.WriteStringBinary( GetMesh()->GetFilename());
    else
      ar.WriteStringBinary( NULL);

    m_vCurrentPos.SerializeAsVisVector (ar);
    m_vCurrentOri.SerializeAsVisVector (ar);
    m_vCurrentScaling.SerializeAsVisVector (ar); // version 1
    ar << m_fPhysicsTicks;
    ar << (bool)(m_bSimulateWhenVisible==TRUE);
    ar << m_fGravity;
    ar << (bool)(m_spMesh!=NULL);
    if (m_spMesh)
      m_spMesh->SerializeX(ar);

    ar << m_iInitialTickCount;
    ar << (int)0; // reserved
    SetAnimConfig(spStoreConfig);
  }
}


// hide this entity class in vForge, because there is a dedicated cloth shape
START_VAR_TABLE(ClothEntity_cl, VisBaseEntity_cl, "ClothEntity_cl", VFORGE_HIDECLASS, "")  
  DEFINE_VAR_FLOAT(ClothEntity_cl, m_fPhysicsTicks, "framerate for physics calculation", "50", 0, 0);
  DEFINE_VAR_FLOAT(ClothEntity_cl, m_fGravity, "gravity applied to the mesh", "100", 0, 0);
  DEFINE_VAR_BOOL(ClothEntity_cl, m_bSimulateWhenVisible, "if enabled, simulation is only performed when mesh is visible", "TRUE", 0, 0);
END_VAR_TABLE


#pragma warning(pop)

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
