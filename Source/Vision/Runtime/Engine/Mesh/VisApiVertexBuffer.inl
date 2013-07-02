/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiVertexBuffer.inl


//#include "Vision.hpp"

#include <Vision/Runtime/Engine/Visibility/VisApiVisibilityObject.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////
// Mesh buffer inline functions
/////////////////////////////////////////////////////////////////////////////////////////////////

inline int VisMeshBuffer_cl::GetVertexCount() const
{
  return m_iAllocVertexCount;
}

inline int VisMeshBuffer_cl::GetIndexCount() const
{
  return m_iAllocIndexCount;
}

inline bool VisMeshBuffer_cl::CanBeLockedForReading() const
{
  return m_spVertexBuffer->CanBeLockedForReading() && m_spIndexBuffer->CanBeLockedForReading();
}

/////////////////////////////////

inline void VisMeshBuffer_cl::SetPrimitiveType(MB_PrimitiveType_e ePrimType)
{
  m_ePrimType = ePrimType;
}


inline void VisMeshBuffer_cl::GetVertexDescriptor(VisMBVertexDescriptor_t &descr) const
{
  descr = m_VertexDescr;
}


inline VisMeshBuffer_cl::MB_PrimitiveType_e VisMeshBuffer_cl::GetPrimitiveType() const
{
  return (MB_PrimitiveType_e)m_ePrimType;
}

inline void VisMeshBuffer_cl::SetChannelTexture(VTextureObject *pTextureObject, int iChannel)
{
  VASSERT(iChannel>=0 && iChannel<MAX_MESHBUFFER_TEXCOORDS);
  m_pChannelTex[iChannel] = pTextureObject;
}

inline void VisMeshBuffer_cl::SetBaseTexture(VTextureObject *pTextureObject)
{
  int i;
  for (i=0;i<MAX_MESHBUFFER_TEXCOORDS;i++)
    SetChannelTexture(pTextureObject,i);
}

inline VTextureObject* VisMeshBuffer_cl::GetBaseTexture() const
{
  return m_pChannelTex[0];
}


inline VTextureObject* VisMeshBuffer_cl::GetChannelTexture(int iChannel) const
{
  VASSERT(iChannel>=0 && iChannel<MAX_MESHBUFFER_TEXCOORDS);
  return m_pChannelTex[iChannel];
}


inline void VisMeshBuffer_cl::SetDefaultTransparency(VIS_TransparencyType eTransp)
{
  m_iRenderState.SetTransparency(eTransp);
  if (eTransp==VIS_TRANSP_COLORKEY)
    m_iRenderState.SetFlag(RENDERSTATEFLAG_ALPHATEST);
  else
    m_iRenderState.RemoveFlag(RENDERSTATEFLAG_ALPHATEST);
}



inline void VisMeshBuffer_cl::EnableDefaultZBufferWriting(BOOL bStatus)
{
  if (bStatus)
    m_iRenderState.SetFlag(RENDERSTATEFLAG_WRITETOZBUFFER);
  else
    m_iRenderState.RemoveFlag(RENDERSTATEFLAG_WRITETOZBUFFER);
}


inline void VisMeshBuffer_cl::SetDefaultCullMode(VCullMode eCullMode)
{
  m_iRenderState.SetFaceCullMode(eCullMode);
}




inline VIS_TransparencyType VisMeshBuffer_cl::GetDefaultTransparency() const
{
  return (VIS_TransparencyType)m_iRenderState.GetTransparency();
}


inline BOOL VisMeshBuffer_cl::IsDefaultZBufferWritingEnabled() const
{
  return m_iRenderState.IsFlagSet(RENDERSTATEFLAG_WRITETOZBUFFER);
}

inline VCullMode VisMeshBuffer_cl::GetDefaultCullMode() const
{
  return (VCullMode)m_iRenderState.GetFaceCullMode();
}


inline BOOL VisMeshBuffer_cl::GetUseProjectionMatrix() const
{
  return m_bUseProjection;
}

inline void VisMeshBuffer_cl::SetUseProjectionMatrix(BOOL bStatus)
{
  m_bUseProjection = (bStatus==TRUE);
}


/////////////////////////////////////////////////////////////////////////////////////////////////
// Mesh buffer object inline functions
/////////////////////////////////////////////////////////////////////////////////////////////////

inline void VisMeshBufferObject_cl::SetVisible(BOOL bVisible)
{
  SetVisibleBitmask(bVisible ? 0xffffffff : 0);
}


inline BOOL VisMeshBufferObject_cl::IsVisible() const
{
  return GetVisibleBitmask()!=0;
}


inline void VisMeshBufferObject_cl::SetMeshBuffer(VisMeshBuffer_cl *pBuffer)
{
  RemoveAllMeshBuffer();
  if (pBuffer)
    AddMeshBuffer(pBuffer); 
}


inline VisMeshBuffer_cl *VisMeshBufferObject_cl::GetCurrentMeshBuffer() const
{
  if (m_iMeshBufferCount>0)
    return m_pMeshBuffer.GetDataPtr()[0];
  return NULL;
}

inline void VisMeshBufferObject_cl::GetRenderRange(int &iFirstPrim, int &iPrimCount) const
{
  iFirstPrim = m_iFirstPrimitive;
  iPrimCount = m_iPrimitiveCount;
}

inline unsigned int VisMeshBufferObject_cl::GetVisibleBitmask() const
{
  return m_iRenderFlags;
}

inline void VisMeshBufferObject_cl::SetVisibleBitmask(unsigned int iFlags)
{
  m_iRenderFlags = iFlags; 
}


inline int VisMeshBufferObject_cl::GetOrder() const 
{
  return m_iOrderBits;
}


inline int VisMeshBufferObject_cl::GetSubOrder() const
{
  return m_iSubOrder;
}


inline const hkvAlignedBBox* VisMeshBufferObject_cl::GetVisibilityBoundingBox() const
{
  if (!m_spVisObj)
    return NULL;
  return m_spVisObj->GetWorldSpaceBoundingBoxPtr();
}


inline VisVisibilityObject_cl* VisMeshBufferObject_cl::GetVisibilityObject() const 
{
  return m_spVisObj;
}


inline void VisMeshBufferObject_cl::SetVisibilityObject(VisVisibilityObject_cl* pNewVisObject)
{
  if (pNewVisObject == m_spVisObj)
    return;
  if (m_spVisObj != NULL)
    m_spVisObj->RemoveObject3D(this);
  m_spVisObj=pNewVisObject;
  if (pNewVisObject != NULL)
    m_spVisObj->AddObject3D(this);
}


inline VCompiledTechnique *VisMeshBufferObject_cl::GetActiveTechnique() const
{
  if (m_spTechnique)
    return m_spTechnique;
  if (m_iMeshBufferCount>0)
    return m_pMeshBuffer.GetDataPtr()[0]->GetDefaultTechnique();
  return NULL;
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
