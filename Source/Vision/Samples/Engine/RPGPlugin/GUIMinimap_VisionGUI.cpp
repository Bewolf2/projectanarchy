/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#include <Vision/Samples/Engine/RPGPlugin/Precompiled.h>

#include <Vision/Samples/Engine/RPGPlugin/GUIMinimap_VisionGUI.h>

void RPG_GuiMinimap_VisionGUI::OnActivate()
{
  VImageControl::OnActivate();

  InitShaders();

  // Reload shaders after backgrounding
  Vision::Callbacks.OnReassignShaders += this;
}

void RPG_GuiMinimap_VisionGUI::OnDeactivate()
{
  VImageControl::OnDeactivate();
  m_maskShader = NULL;
  m_maskTex = NULL;

  Vision::Callbacks.OnReassignShaders -= this;
}  

void RPG_GuiMinimap_VisionGUI::OnPaint(VGraphicsInfo &Graphics, const VItemRenderInfo &parentState)
{ 
  VASSERT(m_maskShader);
  VImageState &img = m_Image.m_States[GetCurrentState()];
  VColorRef iColor = parentState.iFadeColor;
  VRectanglef rect = GetBoundingBox();
  VRectanglef texCoord(0.f, 0.f, 1.f, 1.f);
  VTextureObject *pTex = img.GetCurrentTexture();

  // Update shader registers in the const tables of the shader pass:
  m_regTransform.SetRegisterValueF(m_maskShader, m_transform.data);
  m_regFadeColor.SetRegisterValueF(m_maskShader, m_fadeColor.data);

  // Render the quad with our mask shader
  Overlay2DVertex_t v[6];
  IVRender2DInterface::CreateQuadVertices(rect.m_vMin.x,rect.m_vMin.y,rect.m_vMax.x,rect.m_vMax.y,texCoord.m_vMin.x,texCoord.m_vMin.y,texCoord.m_vMax.x,texCoord.m_vMax.y,iColor,v);
  VSimpleRenderState_t state = VGUIManager::DefaultGUIRenderState();
  Graphics.Renderer.Draw2DBufferWithShader(6, v, pTex, *m_maskShader);
}

void RPG_GuiMinimap_VisionGUI::InitShaders()
{
  // Load shader library
  Vision::Shaders.LoadShaderLibrary("GUI\\Shaders\\GUIAlphaMask.Shaderlib");
  VCompiledTechnique *pTechnique = Vision::Shaders.CreateTechnique("GUIAlphaMask",NULL);
  VASSERT(pTechnique);
  m_maskShader = pTechnique->GetShader(0);

  // Apply a custom texture here
  int iMaskSampler = m_maskShader->GetSamplerIndexByName(VSS_PixelShader, "MaskTexture");
  VASSERT(iMaskSampler>=0);
  m_maskShader->GetStateGroupTexture(VSS_PixelShader, iMaskSampler)->m_spCustomTex = m_maskTex;

  // Cache the shader registers:
  m_regTransform.Init(m_maskShader,"BaseTransform");
  m_regFadeColor.Init(m_maskShader,"FadeColor");
  VASSERT(m_regTransform.IsValid());
  VASSERT(m_regFadeColor.IsValid());
}

void RPG_GuiMinimap_VisionGUI::OnHandleCallback(IVisCallbackDataObject_cl *pData)
{
  if (pData->m_pSender == &Vision::Callbacks.OnReassignShaders)
  {
    InitShaders();
  }
}

V_IMPLEMENT_SERIAL( RPG_GuiMinimap_VisionGUI, VImageControl, 0, &g_RPGPluginModule );

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
