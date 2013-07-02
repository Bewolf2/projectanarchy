/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Profiling/VGraphManager.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Profiling/VGraphObject.hpp>

#define VGRAPH_LEGEND_SIZE 10 // size of legend in pixels

extern VModule g_VisionEngineModule;

VGraphObject::VGraphObject(VGraphProps &props)
{
  Vision::Callbacks.OnUpdateSceneBegin += this;
  Vision::Callbacks.OnVideoChanged += this;
  m_iManagerIndex = VGraphManager::GlobalManager().AddInstance(this); 
  VASSERT(props.iResolution > 0 && props.fMaxValue > props.fMinValue);
  m_props = props;
  m_bVisible = true;
  m_bInitialized = false;
  m_bDirty = false;
  m_fLastMinValue = HKVMATH_FLOAT_MAX_POS;
  m_fLastMaxValue = HKVMATH_FLOAT_MAX_NEG;
}

VGraphObject::~VGraphObject()
{ 
  m_spBackgroundMask = NULL;
  m_spMeshBufferObject = NULL;   
  m_spMeshBuffer = NULL;
  Vision::Callbacks.OnUpdateSceneBegin -= this;
  Vision::Callbacks.OnVideoChanged -= this;
  VGraphManager::GlobalManager().RemoveInstance(m_iManagerIndex);
}

int VGraphObject::AddCurve(const char *szName, VColorRef iColor)
{
  if (m_bInitialized)
  {
    Vision::Error.Warning("VGraphObject: Curve must be added before object is initialized!");
    return -1;
  }

  unsigned int iVertexOffset = m_vertexArray.GetValidSize();
  unsigned int iIndexOffset = m_indexArray.GetValidSize();

  // initialize vertex array
  unsigned int iVertexCount = m_props.iResolution+1;
  float fStepSize = (m_props.fWidth*2.0f)/m_props.iResolution;
  for (unsigned int i=0;i<iVertexCount;i++)
  {
    VGraphVertex vertex;
    vertex.vPos.x = m_props.vPosition.x + i*fStepSize;
    vertex.vPos.y = m_props.vPosition.y - m_props.fHeight;
    vertex.vPos.z = 0.0f;
    vertex.iColor = iColor;
    m_vertexArray[iVertexOffset+i] = vertex;
  }  

  // initialize index array
  unsigned int iIndexCount = iVertexCount*2 - 2;   
  unsigned int idx = iIndexOffset;
  for (unsigned int i=0;i<iIndexCount/2;i++)
  {
    m_indexArray[idx++] = iVertexOffset+i;
    m_indexArray[idx++] = iVertexOffset+i+1;
  }

  // add curve
  VCurveObject curve;
  VASSERT(strlen(szName) < VCURVE_MAX_NAME);
  strcpy(curve.m_szName,szName);
  curve.m_iColor = iColor;
  curve.m_iVertexOffset = iVertexOffset;  
  unsigned int iCurveIndex = m_curves.GetValidSize();
  m_curves[iCurveIndex] = curve;
  
  // create screen-mask for legend of curve  
  m_curves[iCurveIndex].m_spLegendMask = new VisScreenMask_cl("plainwhite.dds");
  m_curves[iCurveIndex].m_spLegendMask->SetColor(iColor);
  m_curves[iCurveIndex].m_spLegendMask->SetDepthWrite(FALSE);
  m_curves[iCurveIndex].m_spLegendMask->SetVisible(TRUE);
  m_curves[iCurveIndex].m_spLegendMask->SetTargetSize(VGRAPH_LEGEND_SIZE, VGRAPH_LEGEND_SIZE);

  return iCurveIndex;
}

VCompiledTechnique* VGraphObject::CreateEffect()
{
  VShaderEffectLib *pLib = Vision::Shaders.LoadShaderLibrary("Shaders\\FixedFunctionShaders.ShaderLib", SHADERLIBFLAG_HIDDEN);
  VASSERT(pLib!=NULL);
  VCompiledTechnique *pTech = Vision::Shaders.CreateTechnique("PositionColor", NULL);
  VASSERT(pTech!=NULL);
  return pTech;
}

hkvVec2 VGraphObject::GetScreenPos(float fRelativePosX, float fRelativePosY)
{
  float fScreenWidth = (float)Vision::Video.GetXRes();
  float fScreenHeight = (float)Vision::Video.GetYRes();
  hkvVec2 vScreenPos = m_props.vPosition*0.5f + hkvVec2(0.5,0.5f);
  vScreenPos.x = (vScreenPos.x+fRelativePosX)*fScreenWidth;
  vScreenPos.y = (1.0f-vScreenPos.y+fRelativePosY)*fScreenHeight;
  return vScreenPos;
}

void VGraphObject::Resize()
{
  float fScreenWidth = (float)Vision::Video.GetXRes();
  float fScreenHeight = (float)Vision::Video.GetYRes();

  // resize graph background
  hkvVec2 vScreenPos = GetScreenPos(0.0f, 0.0f);
  if (m_spBackgroundMask)
  {
    m_spBackgroundMask->SetPos(vScreenPos.x, vScreenPos.y);
    m_spBackgroundMask->SetTargetSize(fScreenWidth*m_props.fWidth, fScreenHeight*m_props.fHeight);
  }
 
  // resize legends
  for (unsigned int i=0;i<m_curves.GetValidSize();i++)
  {
    if(m_curves[i].m_spLegendMask)
    {
      vScreenPos = GetScreenPos(m_props.fWidth, 0.0f);
      m_curves[i].m_spLegendMask->SetPos(vScreenPos.x+VGRAPH_LEGEND_SIZE,vScreenPos.y+i*VGRAPH_LEGEND_SIZE*2.5f);
    }
  }
} 

void VGraphObject::Init(const char *szMaskTexture)
{
  if (m_bInitialized)
    return;

  // create background screen-mask
  const char *szTexName = szMaskTexture ? szMaskTexture : "grid2D.dds";
  m_spBackgroundMask = new VisScreenMask_cl(szTexName);
  m_spBackgroundMask->SetDepthWrite(FALSE);
  m_spBackgroundMask->SetTransparency(VIS_TRANSP_ADDITIVE);
  m_spBackgroundMask->SetVisible(TRUE);
  
  // describe the compact graph vertex format
  VisMBVertexDescriptor_t compactVertexDescr;
  compactVertexDescr.m_iStride = sizeof(VGraphVertex);
  compactVertexDescr.m_iPosOfs = offsetof(VGraphVertex,vPos); 
  compactVertexDescr.m_iColorOfs = offsetof(VGraphVertex,iColor);

  // create mesh buffer
  m_spMeshBuffer = new VisMeshBuffer_cl;

  // vertices
  m_spMeshBuffer->AllocateVertices(compactVertexDescr, m_vertexArray.GetValidSize());   
  VGraphVertex *pVertex = (VGraphVertex*)m_spMeshBuffer->LockVertices(VIS_LOCKFLAG_DISCARDABLE);
  memcpy(pVertex,m_vertexArray.GetDataPtr(),sizeof(VGraphVertex)*m_vertexArray.GetValidSize());
  m_spMeshBuffer->UnLockVertices();

  // indices
  m_spMeshBuffer->AllocateIndexList(m_indexArray.GetValidSize());
  unsigned short *pIndex = (unsigned short*)m_spMeshBuffer->LockIndices(VIS_LOCKFLAG_DISCARDABLE);
  memcpy(pIndex,m_indexArray.GetDataPtr(),sizeof(short)*m_indexArray.GetValidSize());
  m_spMeshBuffer->UnLockIndices();

  // configure mesh buffer 
  m_spMeshBuffer->SetPrimitiveType(VisMeshBuffer_cl::MB_PRIMTYPE_INDEXED_LINELIST);
  m_spMeshBuffer->SetUseProjectionMatrix(false); 
  m_spMeshBuffer->EnableDefaultZBufferWriting(FALSE);
  m_spMeshBuffer->SetDefaultCullMode(CULL_NONE);
  m_spMeshBuffer->SetDefaultTechnique(CreateEffect());

  // create mesh instance
  m_spMeshBufferObject = new VisMeshBufferObject_cl(m_spMeshBuffer);
  m_spMeshBufferObject->SetObjectFlag(VObjectFlag_AutoDispose);  
  m_spMeshBufferObject->SetVisibleBitmask(VIS_ENTITY_VISIBLE_IN_WORLD);
  m_spMeshBufferObject->SetOrder(VRH_AFTER_RENDERING, 0);

  // resize all screen masks
  Resize();

  m_bInitialized = true;
}

void VGraphObject::Update(unsigned int iCurveIndex, float fValue)
{
  VASSERT(iCurveIndex < m_curves.GetValidSize());

  // shift vertices from right to left
  int iVertexOffset = m_curves[iCurveIndex].m_iVertexOffset;
  for(unsigned int i=0;i<m_props.iResolution;i++)
    m_vertexArray[iVertexOffset+i].vPos[1] = m_vertexArray[iVertexOffset+i+1].vPos[1];

  // determine min-/ max-value
  if (m_props.bRangeAdaptation) 
  {
    if (fValue > m_fLastMaxValue)
      m_fLastMaxValue = fValue;
    if (fValue < m_fLastMinValue)
      m_fLastMinValue = fValue;
  }

  // update last vertex of corresponding curve
  fValue = hkvMath::clamp(fValue, m_props.fMinValue, m_props.fMaxValue);
  float fFactor = (fValue-m_props.fMinValue)/(m_props.fMaxValue-m_props.fMinValue);
  m_vertexArray[iVertexOffset+m_props.iResolution].vPos[1] = m_props.vPosition.y + (fFactor-1.0f)*m_props.fHeight*2.0f;

  m_bDirty = true;
}

void VGraphObject::SetVisible(bool bVisible)
{
  if (!m_bInitialized)
  {
    Vision::Error.Warning("VGraphObject: Object is not initialized!");
    return;
  }

  BOOL bVis = bVisible ? TRUE : FALSE; 
  if (m_spMeshBufferObject)
    m_spMeshBufferObject->SetVisibleBitmask(bVis ? VIS_ENTITY_VISIBLE_IN_WORLD : VIS_ENTITY_INVISIBLE);
  if (m_spBackgroundMask)
    m_spBackgroundMask->SetVisible(bVis);
  for (unsigned int i=0;i<m_curves.GetValidSize();i++)
  {
    if(m_curves[i].m_spLegendMask)
      m_curves[i].m_spLegendMask->SetVisible(bVis);
  }

  m_bVisible = bVisible;
}

void VGraphObject::OnHandleCallback(IVisCallbackDataObject_cl *pData)
{
  if (pData->m_pSender == &Vision::Callbacks.OnUpdateSceneBegin)
  {
    // print text for legend and min-/ max-values
    if (m_bVisible)
    {
      for (unsigned int i=0;i<m_curves.GetValidSize();i++)
      {
        if(m_curves[i].m_spLegendMask)
        {
          float fPosX, fPosY;
          m_curves[i].m_spLegendMask->GetPos(fPosX, fPosY);
          Vision::Message.Print(1, (int)fPosX+VGRAPH_LEGEND_SIZE*2, (int)fPosY, m_curves[i].m_szName);
        }
      }
      hkvVec2 vScreenPos = GetScreenPos(0.0f, m_props.fHeight);
      Vision::Message.Print(1, (int)vScreenPos.x-50, (int)vScreenPos.y, "%.5f", m_props.fMinValue);
      vScreenPos = GetScreenPos(0.0f, 0.0f);
      Vision::Message.Print(1, (int)vScreenPos.x-50, (int)vScreenPos.y, "%.5f", m_props.fMaxValue);
    }

    if (m_bDirty)
    {
      // update vertex buffer
      VGraphVertex *pVertex = (VGraphVertex*)m_spMeshBuffer->LockVertices(VIS_LOCKFLAG_DISCARDABLE);
      memcpy(pVertex,m_vertexArray.GetDataPtr(),sizeof(VGraphVertex)*m_vertexArray.GetValidSize());
      m_spMeshBuffer->UnLockVertices();

      // dynamically update value range
      if (m_props.bRangeAdaptation)
      {
        // scale last range values into visually proper range  
        m_fLastMinValue -= m_fLastMinValue*0.4f;
        m_fLastMaxValue += m_fLastMaxValue*0.4f;

        // gradually update value range
        const float fLerpFactor = 0.5f;
        m_props.fMinValue = m_props.fMinValue*(1.0f-fLerpFactor)+m_fLastMinValue*fLerpFactor;
        m_props.fMaxValue = m_props.fMaxValue*(1.0f-fLerpFactor)+m_fLastMaxValue*fLerpFactor;

        // reset last range values
        m_fLastMinValue = HKVMATH_FLOAT_MAX_POS;
        m_fLastMaxValue = HKVMATH_FLOAT_MAX_NEG;
      }

      m_bDirty = false;
    }

    return;
  }

  if (pData->m_pSender == &Vision::Callbacks.OnVideoChanged)
  {
    Resize();
    return;
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
