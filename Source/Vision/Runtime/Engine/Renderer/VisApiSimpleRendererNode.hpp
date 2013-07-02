/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiSimpleRendererNode.hpp

#ifndef VISAPISIMPLERENDERERNODE_HPP_INCLUDED
#define VISAPISIMPLERENDERERNODE_HPP_INCLUDED

#include <Vision/Runtime/Engine/Renderer/VisApiRendererNode.hpp>
#include <Vision/Runtime/Engine/Renderer/Context/VisApiRenderContext.hpp>

/// \brief
///  A simple renderer node to wrap around a single render context.
///
/// This class is intended to replace the legacy behavior of registering the main context globally with a
/// single-pass render loop. Post processing effects, shadows, or multiple registered contexts are not supported.
class VSimpleRendererNode : public IVRendererNode, public IVisCallbackHandler_cl
{
public:
  /// \brief Constructor.
  VISION_APIFUNC VSimpleRendererNode();

  /// \brief Constructor.
  VISION_APIFUNC VSimpleRendererNode(VisRenderContext_cl* pTargetContext);

  /// \brief Destructor.
  VISION_APIFUNC virtual ~VSimpleRendererNode();

  /// \brief Sets and registers the final target context.
  VISION_APIFUNC virtual void SetFinalTargetContext(VisRenderContext_cl* pTargetContext) HKV_OVERRIDE;

  /// \brief Overridden method. See base class.
  VISION_APIFUNC virtual VisRenderContext_cl* GetReferenceContext() HKV_OVERRIDE
  {
    return GetFinalTargetContext();
  }

  VISION_APIFUNC virtual void InitializeRenderer() HKV_OVERRIDE;

  VISION_APIFUNC virtual void DeInitializeRenderer() HKV_OVERRIDE;

#if !defined(_VISION_DOC)
  virtual void OnHandleCallback(IVisCallbackDataObject_cl *pData) HKV_OVERRIDE;
  V_DECLARE_SERIAL_DLLEXP(VSimpleRendererNode, VISION_APIFUNC);
#endif

private:
  VSimpleRendererNode(const VSimpleRendererNode&);
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
