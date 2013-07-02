/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/HavokPhysicsEnginePluginPCH.h>

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokConstraintChain.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokDebugConstraintChainRenderer.hpp>


// ----------------------------------------------------------------------------
// vHavokDebugConstraintChainRenderer
// ----------------------------------------------------------------------------
V_IMPLEMENT_SERIAL(vHavokDebugConstraintChainRenderer, vHavokConstraintChainRendererBase, 0, &g_vHavokModule);

// ----------------------------------------------------------------------------
const unsigned int vHavokDebugConstraintChainRenderer::s_iSerialVersion = 1;

// ----------------------------------------------------------------------------
vHavokDebugConstraintChainRenderer::vHavokDebugConstraintChainRenderer(
  int iComponentFlags)
: vHavokConstraintChainRendererBase(iComponentFlags)
, Render_Color(V_RGBA_YELLOW)
{
}

// ----------------------------------------------------------------------------
void vHavokDebugConstraintChainRenderer::OnRender()
{
  VVERIFY_OR_RET(m_pConstraintChain);

  float fLinkExtent = (m_pConstraintChain->GetLinkLength() - m_pConstraintChain->GetLinkGap()) / 2.f;
  for (unsigned int i = 0; i < m_pConstraintChain->GetNumLinks(); ++i)
  {
    hkvVec3 vTranslation;
    hkvMat3 mRotation;
    if (!m_pConstraintChain->GetLinkTransform(i, mRotation, vTranslation))
      continue;

    hkvVec3 v1 = hkvVec3(-fLinkExtent, 0.f, 0.f);
    hkvVec3 v2 = -v1;

    v1 = (mRotation * v1) + vTranslation;
    v2 = (mRotation * v2) + vTranslation;

    Vision::Game.DrawSingleLine(v1, v2, Render_Color);
  }
}

// ----------------------------------------------------------------------------
void vHavokDebugConstraintChainRenderer::Serialize(VArchive &ar)
{
  vHavokConstraintChainRendererBase::Serialize(ar);

  if (ar.IsLoading())
  {
    unsigned int iVersion = 0;
    ar >> iVersion;
    VASSERT_MSG((iVersion > 0) && (iVersion <= s_iSerialVersion), "Invalid version of serialized data!");

    ar >> Render_Color;
  }
  else
  {
    ar << s_iSerialVersion;
    ar << Render_Color;
  }
}

// ----------------------------------------------------------------------------
START_VAR_TABLE(vHavokDebugConstraintChainRenderer, vHavokConstraintChainRendererBase, "vHavok constraint chain renderer (renders each link as a simple line)", VFORGE_HIDECLASS, "Havok Constraint Chain Renderer (Debug)" )
  DEFINE_VAR_COLORREF(vHavokDebugConstraintChainRenderer, Render_Color, "Color for rendering the constraint chain.", "255,255,0,255", 0, NULL);
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
