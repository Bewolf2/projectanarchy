/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/HavokPhysicsEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokDisplayHandler.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokConversionUtils.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokPhysicsModule.hpp>

#include <Physics2012/Utilities/VisualDebugger/Viewer/Dynamics/hkpPhantomDisplayViewer.h>
#include <Physics2012/Utilities/VisualDebugger/Viewer/Dynamics/hkpConstraintViewer.h>


// -------------------------------------------------------------------------- //
// HAVOK DISPLAY GEOMETRY                                                     //
// -------------------------------------------------------------------------- //

vHavokDisplayGeometry::vHavokDisplayGeometry(hkDisplayGeometry* geometry, const hkTransform &transform, hkUlong id, int tag)
{
  // Set id
  m_ID = id;

  // Create mesh instance
  if (geometry->getGeometry() == NULL)
    geometry->buildGeometry();

  // Create mesh buffer
  VisMBVertexDescriptor_t desc;
  desc.m_iStride = sizeof(SimpleMesh_t);
  desc.m_iPosOfs = offsetof(SimpleMesh_t,pos);

  UINT iVertices = geometry->getGeometry()->m_vertices.getSize();
  UINT iTriangles = geometry->getGeometry()->m_triangles.getSize();
  VisMeshBuffer_cl *pMesh = new VisMeshBuffer_cl();

  // Copy vertices
  pMesh->AllocateVertices(desc, iVertices);
  SimpleMesh_t *pVertex = (SimpleMesh_t *)pMesh->LockVertices(VIS_LOCKFLAG_DISCARDABLE);
  for (UINT i = 0; i < iVertices; i++, pVertex++)
  {
	  hkVector4 vPos; 
	  vPos.setMul(geometry->getGeometry()->m_vertices[i], vHavokConversionUtils::GetHavok2VisionScaleSIMD());
	  vPos.store<3,HK_IO_NATIVE_ALIGNED>(&pVertex->pos[0]);
  }
  pMesh->UnLockVertices();

  // Copy indices
  pMesh->AllocateIndexList(iTriangles*3);
  unsigned short *pIndex = (unsigned short *)pMesh->LockIndices(VIS_LOCKFLAG_DISCARDABLE);

  for (UINT i = 0; i < iTriangles; i++, pIndex+=3)
  {
    pIndex[0] = geometry->getGeometry()->m_triangles[i].m_a;
    pIndex[1] = geometry->getGeometry()->m_triangles[i].m_b;
    pIndex[2] = geometry->getGeometry()->m_triangles[i].m_c;
  }
  pMesh->UnLockIndices();
  pMesh->SetPrimitiveType(VisMeshBuffer_cl::MB_PRIMTYPE_INDEXED_TRILIST );
  
  // Create mesh instance
  m_spMeshInstance = new VisMeshBufferObject_cl(pMesh);

  // Mesh instance created
  if (m_spMeshInstance == NULL)
    return;
 
  // Apply highlighting shader
  m_spMeshInstance->SetTechnique(CreateHighlightEffect(VColorRef(55, 255, 55)));

  // Set transform. Add the transform of the actual shape to the transform of the Vision mesh. 
  // This is needed for shapes that come from HKT files where they can have custom pivots.
  hkTransform finalTransform = transform;
  finalTransform.setMulEq(geometry->getTransform());
  vHavokConversionUtils::PhysTransformToVisMatVecWorld(finalTransform, m_vRot, m_vPos);
  m_spMeshInstance->SetUseEulerAngles(FALSE);
  m_spMeshInstance->SetPosition(m_vPos);
  m_spMeshInstance->SetRotationMatrix(m_vRot);
  m_spMeshInstance->SetVisible(false);
  m_spMeshInstance->SetObjectFlag(VObjectFlag_AutoDispose);
}


vHavokDisplayGeometry::~vHavokDisplayGeometry()
{
}

VCompiledTechnique *vHavokDisplayGeometry::CreateHighlightEffect(VColorRef iColor)
{
  VShaderEffectLib *pLib = Vision::Shaders.LoadShaderLibrary("Shaders\\WireframeColor.ShaderLib",SHADERLIBFLAG_HIDDEN);
  VASSERT(pLib!=NULL);

  // Create a separate technique for each mesh instance
  char szParamStr[1024];
  hkvVec3 colF = iColor.ToFloat();
  sprintf(szParamStr,"DebugColor=%.3f,%.3f,%.3f,1.0;",colF.x,colF.y,colF.z);
  VCompiledTechnique *pTech = Vision::Shaders.CreateTechnique("WireframeColor", szParamStr);
  VASSERT(pTech!=NULL);
  return pTech;
}

void vHavokDisplayGeometry::UpdateTransform(const hkTransform &transform)
{
  if (!m_spMeshInstance)
    return;

  vHavokConversionUtils::PhysTransformToVisMatVecWorld(transform, m_vRot, m_vPos);
  m_spMeshInstance->SetPosition(m_vPos);
  m_spMeshInstance->SetRotationMatrix(m_vRot);
}

void vHavokDisplayGeometry::SetVisible(BOOL bVisible)
{
  if (!m_spMeshInstance)
    return;
  
  m_spMeshInstance->SetVisible(bVisible);
}

BOOL vHavokDisplayGeometry::IsVisible() const
{
  if (!m_spMeshInstance)
    return FALSE;

  return  m_spMeshInstance->IsVisible();
}

void vHavokDisplayGeometry::SetColor(VColorRef iColor)
{
  if (!m_spMeshInstance)
    return;

  m_spMeshInstance->SetTechnique(CreateHighlightEffect(iColor));
}

void vHavokDisplayGeometry::SetOwner(VisObject3D_cl *pOwner)
{
  if (!m_spMeshInstance)
    return;

  m_spMeshInstance->AttachToParent(pOwner);
}

// -------------------------------------------------------------------------- //
// HAVOK DISPLAY GEOMETRY LIST                                                //
// -------------------------------------------------------------------------- //
vHavokDisplayGeometryList::vHavokDisplayGeometryList()
{
  m_iSize = 0;
  m_List.SetDefaultValue(NULL);
}

vHavokDisplayGeometryList::~vHavokDisplayGeometryList()
{
  for (unsigned int i = 0; i < m_iSize; ++i)
    m_List[i]->Release();
}

void vHavokDisplayGeometryList::Add(vHavokDisplayGeometry *pGeometry)
{
  VVERIFY_OR_RET(pGeometry);
  pGeometry->AddRef();
  m_List[m_iSize] = pGeometry;
  m_iSize++;
}

void vHavokDisplayGeometryList::EnsureCapacity(unsigned int iCapacity)
{
  m_List.EnsureSize(iCapacity);
}

unsigned int vHavokDisplayGeometryList::GetSize() const
{
  return m_iSize;
}

BOOL vHavokDisplayGeometryList::IsVisible(unsigned int iID) const
{
  for (unsigned int i = 0; i < m_iSize; ++i)
  {
    vHavokDisplayGeometry *pGeom = m_List.Get(i);
    if ((pGeom->m_ID == iID) && pGeom->IsVisible())
      return TRUE;
  }

  return FALSE;
}

void vHavokDisplayGeometryList::Remove(unsigned int iID)
{
  bool bDeleted = false;
  for (unsigned int i = 0; i < m_iSize; ++i)
  {
    if (m_List[i]->m_ID == iID)
    {
      m_List[i]->Release();
      m_List[i] = NULL;
      bDeleted = true;
    }
  }

  if (bDeleted)
    m_iSize = m_List.Pack();
}

void vHavokDisplayGeometryList::SetColor(unsigned int iID, VColorRef iColor)
{
  for (unsigned int i = 0; i < m_iSize; ++i)
  {
    if (m_List[i]->m_ID == iID)
      m_List[i]->SetColor(iColor);
  }
}

void vHavokDisplayGeometryList::SetOwner(unsigned int iID, VisObject3D_cl *pOwner)
{
  for (unsigned int i = 0; i < m_iSize; ++i)
  {
    if (m_List[i]->m_ID == iID)
      m_List[i]->SetOwner(pOwner);
  }
}

void vHavokDisplayGeometryList::SetVisible(unsigned int iID, BOOL bVisible)
{
  for (unsigned int i = 0; i < m_iSize; ++i)
  {
    if (m_List[i]->m_ID == iID)
      m_List[i]->SetVisible(bVisible);
  }
}

void vHavokDisplayGeometryList::UpdateTransform(unsigned int iID, const hkTransform &transform)
{
  for (unsigned int i = 0; i < m_iSize; ++i)
  {
    if (m_List[i]->m_ID == iID)
    {
      // do not update objects that are attached to a parent, the parent will already transform them properly
      if ((m_List[i]->GetMeshInstance ()) && (m_List[i]->GetMeshInstance ()->GetParent ()))
        break;

      m_List[i]->UpdateTransform(transform);
      break;
    }
  }
}


// -------------------------------------------------------------------------- //
// HAVOK DISPLAY HANDLER                                                      //
// -------------------------------------------------------------------------- //

vHavokDisplayHandler::vHavokDisplayHandler (hkpWorld* pPhysicsWorld)
{
  m_pContext = new hkpPhysicsContext;
  hkpPhysicsContext::registerAllPhysicsProcesses();   // all the physics viewers
  m_pContext->addWorld(pPhysicsWorld);                // add the physics world so the viewers can see it

  // Setup the visual debugger
  m_contexts.pushBack(m_pContext);

  // Setup Havok Process for Havok Debug Display Handler
  m_pLocalProcesses.pushBack( hkProcessFactory::getInstance().createProcess(hkpShapeDisplayViewer::getName(), m_contexts ) );
  hkpShapeDisplayViewer* pDisplayViewer = (hkpShapeDisplayViewer*)( m_pLocalProcesses.back() );
  pDisplayViewer->setAutoColorMode(true); 
  pDisplayViewer->setAutoColorModeOverride(true);
  pDisplayViewer->setFixedObjectColor( 0xffff0000 );
  pDisplayViewer->setMovableObjectColor( 0xff00ff00 );

  m_pLocalProcesses.pushBack( hkProcessFactory::getInstance().createProcess("Trigger Volumes", m_contexts ) );
  hkpShapeDisplayViewer* pDisplayViewerTrigger = (hkpShapeDisplayViewer*)( m_pLocalProcesses.back() );
  pDisplayViewerTrigger->setAutoColorMode(true); 
  pDisplayViewerTrigger->setAutoColorModeOverride(true);
  pDisplayViewerTrigger->setFixedObjectColor( 0xffff0000 );
  pDisplayViewerTrigger->setMovableObjectColor( 0xff00ff00 );

  m_pLocalProcesses.pushBack( hkProcessFactory::getInstance().createProcess(hkpPhantomDisplayViewer::getName(), m_contexts ) );
  m_pLocalProcesses.pushBack( hkProcessFactory::getInstance().createProcess(hkpConstraintViewer::getName(), m_contexts ) );

  

  for (int lvi=0; lvi < m_pLocalProcesses.getSize(); ++lvi)
  {
    hkProcess* p = m_pLocalProcesses[lvi];
    p->m_inStream = HK_NULL; // no streams (these are the network streams for the VDB)
    p->m_outStream = HK_NULL;           
    p->m_displayHandler = this;
    p->m_processHandler = HK_NULL; // no process handler
    p->init();
  }

  vHavokPhysicsModule::GetInstance()->AddStepper(this); // some Viewers need to be stepped explicitly to display (not callback driven)
}

vHavokDisplayHandler::~vHavokDisplayHandler ()
{  
  hkArray<hkpWorld*> worlds;
  for (int wi=0;wi< m_pContext->getNumWorlds(); ++wi)
  {
    worlds.pushBack(m_pContext->getWorld(wi));
    worlds.back()->markForWrite();
  }
  
  vHavokPhysicsModule::GetInstance()->RemoveStepper(this);

  for (int pi=0; pi < m_pLocalProcesses.getSize(); ++pi)
  {
	  delete m_pLocalProcesses[pi];
  }

  // Delete the context after you have finished using the Havok Display Handler
  m_pContext->removeReference();

  for (int wii=0;wii< worlds.getSize(); ++wii)
  {
    worlds[wii]->unmarkForWrite();
  }
  
}

hkResult vHavokDisplayHandler::addGeometry(const hkArrayBase<hkDisplayGeometry *> &geometries, const hkTransform &transform, hkUlong id, int tag, hkUlong shapeIdHint, hkGeometry::GeometryType geomType)
{
  m_Geometries.EnsureCapacity(m_Geometries.GetSize() + geometries.getSize());
  for (int i = 0; i < geometries.getSize(); i++)
    m_Geometries.Add(new vHavokDisplayGeometry(geometries[i], transform, id, tag));

  return HK_SUCCESS;
}

hkResult vHavokDisplayHandler::addGeometry(hkDisplayGeometry *geometry, hkUlong id, int tag, hkUlong shapeIdHint)
{
  m_Geometries.Add(new vHavokDisplayGeometry(geometry, geometry->getTransform(), id, tag));

  return HK_SUCCESS;
}

hkResult vHavokDisplayHandler::addGeometryInstance(hkUlong originalInstanceId, const hkTransform& transform, hkUlong id, int tag, hkUlong shapeIdHint)
{

  return HK_SUCCESS;
}

hkResult vHavokDisplayHandler::updateGeometry(const hkTransform &transform, hkUlong id, int tag)
{
  m_Geometries.UpdateTransform( (unsigned int)id, transform);
  return HK_SUCCESS;
}

#if (HAVOK_SDK_VERSION_MAJOR >= 2010)
hkResult vHavokDisplayHandler::updateGeometry(const hkMatrix4& transform, hkUlong id, int tag)
{
  hkTransform tempTransform;
  transform.get(tempTransform);
  return updateGeometry(tempTransform, id, tag); 
}
#endif

hkResult vHavokDisplayHandler::removeGeometry(hkUlong id, int tag, hkUlong shapeIdHint)
{
  m_Geometries.Remove((unsigned int)id);
  return HK_SUCCESS;
}

hkResult vHavokDisplayHandler::setGeometryVisibility(int geometryIndex, bool isEnabled, hkUlong id, int tag)
{
  return HK_SUCCESS;
}


hkResult vHavokDisplayHandler::setGeometryPickable( hkBool isPickable, hkUlong id, int tag )
{
  return HK_SUCCESS;
}

hkResult vHavokDisplayHandler::setGeometryColor( hkColor::Argb color, hkUlong id, int tag)
{
  return HK_SUCCESS;
}

hkResult vHavokDisplayHandler::setGeometryTransparency(float alpha, hkUlong id, int tag)
{
  return HK_SUCCESS;
}

hkResult vHavokDisplayHandler::updateCamera(const hkVector4& from, const hkVector4& to, const hkVector4& up, hkReal nearPlane, hkReal farPlane, hkReal fov, const char* name)
{
  return HK_SUCCESS;
}

hkResult vHavokDisplayHandler::updateBehavior (const hkArrayBase< int > &wordVarIdx, const hkArrayBase< int > &wordStack, const hkArrayBase< int > &quadVarIdx, const hkArrayBase< hkVector4 > &quadStack, const hkArrayBase< char * > &activeNodes, const hkArrayBase< int > &activeStateIds, const hkArrayBase< int > &activeTransitions, const hkArrayBase< hkQsTransform > &transforms)
{
  return HK_SUCCESS;
}

hkResult vHavokDisplayHandler::displayPoint(const hkVector4& position, hkColor::Argb color, int id, int tag)
{
  return HK_SUCCESS;
}

hkResult vHavokDisplayHandler::displayLine(const hkVector4& start, const hkVector4& end, hkColor::Argb color, int id, int tag)
{
  return HK_SUCCESS;
}

hkResult vHavokDisplayHandler::displayTriangle(const hkVector4& a, const hkVector4& b, const hkVector4& c, hkColor::Argb color, int id, int tag)
{

  return HK_SUCCESS;
}

hkResult vHavokDisplayHandler::displayText(const char* text, hkColor::Argb color, int id, int tag)
{

  return HK_SUCCESS;
}

hkResult vHavokDisplayHandler::display3dText(const char* text, const hkVector4& pos, hkColor::Argb color, int id, int tag)
{

  return HK_SUCCESS;
}

// compatibility with VisionIntegration branch
#if defined(HAVOK_SDK_VERSION_MAJOR) && (HAVOK_SDK_VERSION_MAJOR >= 2012)
hkResult vHavokDisplayHandler::displayPoint2d(const hkVector4& position, hkColor::Argb color, int id, int tag)
{

  return HK_SUCCESS;
}

hkResult vHavokDisplayHandler::displayLine2d(const hkVector4& start, const hkVector4& end, hkColor::Argb color, int id, int tag)
{

  return HK_SUCCESS;
}

hkResult vHavokDisplayHandler::displayTriangle2d(const hkVector4& a, const hkVector4& b, const hkVector4& c, hkColor::Argb color, int id, int tag)
{

  return HK_SUCCESS;
}

 hkResult vHavokDisplayHandler::displayText2d(const char* text, const hkVector4& pos, hkReal sizeScale, hkColor::Argb color, int id, int tag)
{

  return HK_SUCCESS;
}
#endif

hkResult vHavokDisplayHandler::displayAnnotation(const char* text, int id, int tag)
{

  return HK_SUCCESS;
}

hkResult vHavokDisplayHandler::displayGeometry(const hkArrayBase<hkDisplayGeometry*>& geometries, const hkTransform& transform, hkColor::Argb color, int id, int tag)
{

  return HK_SUCCESS;
}

hkResult vHavokDisplayHandler::displayGeometry(const hkArrayBase<hkDisplayGeometry*>& geometries, hkColor::Argb color, int id, int tag)
{

  return HK_SUCCESS;
}

hkResult vHavokDisplayHandler::sendMemStatsDump(const char* data, int length)
{

  return HK_SUCCESS;
}

hkResult vHavokDisplayHandler::holdImmediate()
{

  return HK_SUCCESS;
}

void vHavokDisplayHandler::lockForUpdate()
{
}

void vHavokDisplayHandler::unlockForUpdate()
{
}

void vHavokDisplayHandler::Step(float dt)
{
  for (int pi=0; pi < m_pLocalProcesses.getSize(); ++pi)
  {
    m_pLocalProcesses[pi]->step(dt * 1000.0f);
  }
}

void vHavokDisplayHandler::SetColor(hkUlong id, VColorRef iColor)
{
  m_Geometries.SetColor((unsigned int)id, iColor);
}

void vHavokDisplayHandler::SetVisible(hkUlong id, BOOL bVisible)
{
  m_Geometries.SetVisible((unsigned int)id, bVisible);
}

BOOL vHavokDisplayHandler::IsVisible(hkUlong id) const
{
  return m_Geometries.IsVisible((unsigned int)id);
}

void vHavokDisplayHandler::SetOwner(hkUlong id, VisObject3D_cl *pOwner)
{
  m_Geometries.SetOwner((unsigned int)id, pOwner);
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
