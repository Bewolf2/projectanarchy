/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VGUICommandBuffer.hpp

#ifndef VGUICOMMANDBUFFER_HPP_INCLUDED
#define VGUICOMMANDBUFFER_HPP_INCLUDED

struct VGraphicsInfo;
struct VItemRenderInfo;
class IVRenderableItem;
class VGUICommandBuffer;
typedef VSmartPtr<VGUICommandBuffer> VGUICommandBufferPtr;


///\brief
///Internal class for GUI rendering callback pre-recording and faster playback
///
class VGUICommandBuffer : public IVRender2DInterface, public VRefCounter
{
public:
  GUI_IMPEXP VGUICommandBuffer();

  /// \brief The cache is updated and then processed. 
  GUI_IMPEXP bool UpdateCache(IVRenderableItem *pItem, VGraphicsInfo &Graphics, const VItemRenderInfo &parentState);

  /// \brief Flags the cache as invalid.
  inline void Invalidate()
  {
    m_eCacheState = Cache_Dirty;
  }

  // IVRender2DInterface overrides
  virtual void Draw2DBuffer(int iVertexCount, Overlay2DVertex_t *pVertices, VTextureObject *pTexture, const VSimpleRenderState_t &iProperties) HKV_OVERRIDE;
  virtual void Draw2DBufferWithShader(int iVertexCount, Overlay2DVertex_t *pVertices, VTextureObject *pTexture, VCompiledShaderPass &shader) HKV_OVERRIDE;
  virtual void SetScissorRect(const VRectanglef *pScreenRect) HKV_OVERRIDE;
  virtual void SetDepth(float fZCoord) HKV_OVERRIDE;
  virtual float GetDepth() const HKV_OVERRIDE { return m_fLastDepth; }
  virtual void SetTransformation(const hkvVec4 *pScaleAndOfs) HKV_OVERRIDE {VASSERT_MSG(false, "Not implemented");}

protected:
  enum CacheState_e
  {
    Cache_Valid = 0,
    Cache_Dirty,
    Cache_DryRun,
    Cache_FinalRun
  };

  void Reset();
  void Append(const void *pData, int iByteCount);
  void AppendCommand(char cmd, const void *pData=NULL, int iByteCount=0);
  void ProcessCache(VGraphicsInfo &Graphics, const VItemRenderInfo &parentState);

  CacheState_e m_eCacheState;
  int m_iCommandCount;
  DynArray_cl<char> m_Commands;

  // cache:
  char m_iLastCommand;
  int m_iLastCommandBlockPos;
  VTextureObject *m_pLastTex;
  VSimpleRenderState_t m_iLastState;
  VCompiledShaderPass *m_pLastShader;
  float m_fLastDepth;
};


#endif // VGUICOMMANDBUFFER_HPP_INCLUDED

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
