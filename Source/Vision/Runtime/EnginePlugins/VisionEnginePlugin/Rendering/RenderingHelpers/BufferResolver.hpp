/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file BufferResolver.hpp

#ifndef BUFFERRESOLVER_HPP_INCLUDED
#define BUFFERRESOLVER_HPP_INCLUDED

#include <Vision/Runtime/Engine/Renderer/Context/VisApiRenderContext.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/EffectsModule.hpp>

/// \brief
///   Helper class to resolve a render context to a readable texture
class VBufferResolver : public IVisCallbackHandler_cl
{
public:
  /// \brief Initialization mode for the buffer resolver
  enum VInitMode_e {
    VIM_Invalid, ///< Invalid init mode, for internal use only.
    VIM_CreateNewResolveBuffer, ///< Creates a new resolve buffer and manages it internally.
    VIM_UseGivenResolveBuffer ///< Uses a given resolve buffer. Uf you use this option pay attention that the resolution of the resolve buffer might need to be changed when your screen resolution changes.
  };

  /// \brief
  ///   Constructor
  /// 
  /// \param pRenderNode
  ///   The render node this buffer resolver should be used for.
  ///
  /// \param pRenderContextToResolve
  ///   The render context which should be resolved.
  ///
  /// \param uiRenderHook
  ///   Specifies the RenderHook at which the context should be resolved.
  EFFECTS_IMPEXP VBufferResolver(IVRendererNode* pRenderNode, VisRenderContext_cl* pRenderContextToResolve, unsigned int uiRenderHook);

  /// \brief
  ///   Destructor
  EFFECTS_IMPEXP ~VBufferResolver();

  /// \brief
  ///   Initialization 
  ///
  /// \param eInitMode
  ///   Which mode should be used for initialization.
  ///
  /// \param pResolvedBuffer
  ///   Texture to be used for resolving. Only needed for VIM_UseGivenResolveBuffer initialization mode.
  ///
  /// \param pOverrideTextureConfig
  ///   Specifies a possible override texture config for the target texture (default = NULL, no override config).
  ///
  /// \return
  ///   true if successful, false otherwise
  EFFECTS_IMPEXP bool Initialize(VInitMode_e eInitMode, VTextureObject* pResolvedBuffer = NULL, VisRenderableTextureConfig_t* pOverrideTextureConfig = NULL);

  /// \brief
  ///   Returns the resolved buffer.
  inline VTextureObject* GetResolvedBuffer() const
  {
    return m_spResolvedBuffer;
  }

  /// \brief
  ///   Sets the RenderHook at which the context should be resolved.
  EFFECTS_IMPEXP void SetRenderHook(unsigned int uiRenderHook);

  /// \brief
  ///   Resolve the context manually.
  EFFECTS_IMPEXP void ResolveBuffer();

  /// \brief
  ///   Overridden virtual function.
  EFFECTS_IMPEXP virtual void OnHandleCallback(IVisCallbackDataObject_cl *pData) HKV_OVERRIDE;

private:
  void CreateResolveBuffer(VisRenderableTextureConfig_t* pOverrideTextureConfig);

  VisRenderContextPtr m_spRenderContextToResolve;

  VTextureObjectPtr m_spResolvedBuffer;
  IVRendererNode* m_pRenderNode;

  unsigned int m_uiRenderHook;
  VInitMode_e m_eInitMode;
  int m_iBufferWidth, m_iBufferHeight;
  VTextureLoader::VTextureFormat_e m_eBufferFormat;
};

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
