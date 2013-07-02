/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef RPG_PLUGIN_GUI_MINIMAP_VISIONGUI_H__
#define RPG_PLUGIN_GUI_MINIMAP_VISIONGUI_H__

class RPG_GuiMinimap_VisionGUI : public VImageControl, public IVisCallbackHandler_cl
{
public:

  virtual void OnActivate() HKV_OVERRIDE;
  virtual void OnDeactivate() HKV_OVERRIDE;
  virtual void OnPaint(VGraphicsInfo &Graphics, const VItemRenderInfo &parentState) HKV_OVERRIDE;

  void SetTransform(const hkvVec4& transform) { m_transform = transform; }
  void SetFadeColor(const hkvVec4& fadeColor) { m_fadeColor = fadeColor; }
  void SetMaskTexture(const VTextureObjectPtr maskTexture) { m_maskTex = maskTexture; }

  virtual void OnHandleCallback(IVisCallbackDataObject_cl *pData) HKV_OVERRIDE;

private:
  void InitShaders();

  VCompiledShaderPassPtr m_maskShader;                    // Mask Shader
  VTextureObjectPtr m_maskTex;                            // Mask Texture
  hkvVec4 m_transform;                                    // xy: scaling of the image under the mask, zw: offset
  hkvVec4 m_fadeColor;                                    // RGBA color modulation
  VConstantBufferRegister m_regTransform, m_regFadeColor; // store shader registers

  V_DECLARE_SERIAL_DLLEXP( RPG_GuiMinimap_VisionGUI, RPG_PLUGIN_IMPEXP )
  virtual void Serialize( VArchive &ar ) HKV_OVERRIDE {}

};

#endif  //RPG_PLUGIN_GUI_MINIMAP_VISIONGUI_H__

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
