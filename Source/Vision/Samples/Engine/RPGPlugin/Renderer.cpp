/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#include <Vision/Samples/Engine/RPGPlugin/Precompiled.h>

#include <Vision/Samples/Engine/RPGPlugin/Renderer.h>

namespace
{
  void StoreViewParamsFromContext(RPG_ViewParams& viewParams, VisRenderContext_cl const *context)
  {
    VASSERT(context);
    if(!context)
      return;

    // Projection
    viewParams.m_projectionType = context->GetProjectionType();
    switch(viewParams.m_projectionType)
    {
    case VIS_PROJECTIONTYPE_PERSPECTIVE:
      context->GetFOV(viewParams.m_fovX, viewParams.m_fovY);
      break;

    case VIS_PROJECTIONTYPE_ORTHOGRAPHIC:
      context->GetOrthographicSize(viewParams.m_orthoSizeX, viewParams.m_orthoSizeY);
      break;

    case VIS_PROJECTIONTYPE_CUSTOM:
    case VIS_PROJECTIONTYPE_PERSPECTIVE_AUTOFRUSTUM:
      viewParams.m_projectionMatrix = const_cast<VisRenderContext_cl *>(context)->getProjectionMatrix();
      break;
    }

    context->GetClipPlanes(viewParams.m_nearClip, viewParams.m_farClip);

    // Viewport
    context->GetViewport(viewParams.m_vpX, viewParams.m_vpY, viewParams.m_vpW, viewParams.m_vpH, viewParams.m_vpNear, viewParams.m_vpFar);
  }

  void StoreViewParamsFromNode(RPG_ViewParams& viewParams, IVRendererNode /*const*/*node)
  {
    VASSERT(node && node->GetReferenceContext());
    if(!(node && node->GetReferenceContext()))
      return;

    StoreViewParamsFromContext(viewParams, node->GetReferenceContext());
  }

  void LoadViewParamsToContext(VisRenderContext_cl *context, RPG_ViewParams const& viewParams)
  {
    VASSERT(context);
    if(!context)
      return;

    // Viewport
    context->SetViewport(viewParams.m_vpX, viewParams.m_vpY, viewParams.m_vpW, viewParams.m_vpH, viewParams.m_vpNear, viewParams.m_vpFar);

    // Projection
    context->SetProjectionType(viewParams.m_projectionType);
    context->SetClipPlanes(viewParams.m_nearClip, viewParams.m_farClip);
      
    switch(viewParams.m_projectionType)
    {
    case VIS_PROJECTIONTYPE_PERSPECTIVE:
      context->SetFOV(viewParams.m_fovX, viewParams.m_fovY);
      break;

    case VIS_PROJECTIONTYPE_ORTHOGRAPHIC:
      context->SetOrthographicSize(viewParams.m_orthoSizeX, viewParams.m_orthoSizeY);
      break;

    case VIS_PROJECTIONTYPE_CUSTOM:
      context->SetCustomProjectionMatrix(viewParams.m_projectionMatrix.getPointer());
      break;

    case VIS_PROJECTIONTYPE_PERSPECTIVE_AUTOFRUSTUM:
      context->SetCustomProjectionMatrix(viewParams.m_projectionMatrix.getPointer(), true);
      break;
    }
  }

  void LoadViewParamsToNode(IVRendererNode *node, RPG_ViewParams const& viewParams)
  {
    VASSERT(node && node->GetReferenceContext());
    if(!(node && node->GetReferenceContext()))
      return;

    VisRenderContext_cl *const context = node->GetReferenceContext();
    {
      VisProjectionType_e const projType = context->GetProjectionType();
      bool const customProjection = (projType == VIS_PROJECTIONTYPE_CUSTOM || projType == VIS_PROJECTIONTYPE_PERSPECTIVE_AUTOFRUSTUM);
      if(!customProjection)
      {
        if(projType == VIS_PROJECTIONTYPE_PERSPECTIVE)
        {
          context->SetFOV(viewParams.m_fovX, viewParams.m_fovY);
        }
        context->SetClipPlanes(viewParams.m_nearClip, viewParams.m_farClip);
      }
      else
      {
        context->SetCustomProjectionMatrix(viewParams.m_projectionMatrix.getPointer());
      }
    }
  }
}

void RPG_RendererUtil::StoreViewParams(RPG_ViewParams& viewParams)
{
  IVRendererNode *const rendererNode0 = Vision::Renderer.GetRendererNode(0);
  if(rendererNode0)
  {
    StoreViewParamsFromNode(viewParams, rendererNode0);
  }
  else
  {
    StoreViewParamsFromContext(viewParams, Vision::Contexts.GetMainRenderContext());
  }
}

void RPG_RendererUtil::LoadViewParams(RPG_ViewParams const& viewParams)
{
  IVRendererNode *const rendererNode0 = Vision::Renderer.GetRendererNode(0);
  if(rendererNode0)
  {
    LoadViewParamsToNode(rendererNode0, viewParams);
  }
  else
  {
    LoadViewParamsToContext(Vision::Contexts.GetMainRenderContext(), viewParams);
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
