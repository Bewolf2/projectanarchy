/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/****************************************************************************
 *
 * Copyright (c) 2008 CRI Middleware Co, Inc.
 *
 ****************************************************************************/

/***************************************************************************
 *      Include file
 ***************************************************************************/
#include <Vision/Runtime/Engine/System/Vision.hpp>

#include <cri_xpt.h>
#include <Vision/Samples/Engine/Common/CriMovie/CriSmpMvYuvTextures_Vision.h>

/***************************************************************************
 *      MACRO CONSTANT
 ***************************************************************************/
#define SHADER_FILE ("CriMovie\\ColorConversion.ShaderLib")

/***************************************************************************
 *      Process MACRO
 ***************************************************************************/
#define CRISMPMV_NOP(arg)	{ if((arg) == (arg)){}}	

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }
#endif

#ifdef WIN32
  #define VIDEO_TEXTURE_FORMAT VTextureLoader::A8
#elif defined(_VISION_XENON)
  #define VIDEO_TEXTURE_FORMAT VTextureLoader::LIN_A8
#elif defined(_VISION_PS3)
  #define VIDEO_TEXTURE_FORMAT VTextureLoader::A8
#else
  #define VIDEO_TEXTURE_FORMAT VTextureLoader::A8
#endif

/***************************************************************************
 *      Enum declaration
 ***************************************************************************/

/***************************************************************************
 *      Data type declaration
 ***************************************************************************/

/***************************************************************************
 *      Function Declaration
 ***************************************************************************/

/***************************************************************************
 *      Variable Definition
 ***************************************************************************/

/***************************************************************************
 *      CLASS
 ***************************************************************************/

/***************************************************************************
 *      Function Definition
 ***************************************************************************/
CriSmpMvYuvTextures::CriSmpMvYuvTextures(void)
: m_pTexBuf_y(NULL), m_pTexBuf_u(NULL), m_pTexBuf_v(NULL)
, m_pEffect(NULL), m_pTech(NULL), m_pPass(NULL)
, m_cur_tb_idx(0)
, m_uiTexBufSize_y(0), m_uiTexBufSize_u(0), m_uiTexBufSize_v(0)
, m_uiMvWidth(0), m_uiMvHeight(0)
, m_bUseAlphaCh(CRI_FALSE)
{
	/* Initialzie members */
	for (int i=0;i<NUM_TEXBUF;i++) {
		m_spVideoTex_Y[i] = NULL;
		m_spVideoTex_U[i] = NULL;
		m_spVideoTex_V[i] = NULL;
		m_spVideoTex_A[i] = NULL;
	}
}
 
CriSmpMvYuvTextures* CriSmpMvYuvTextures::Create(CriUint32 uiWidth, CriUint32 uiHeight, CriBool bUseAlphaCh)
{
	CriSmpMvYuvTextures *yuv_texs;

	yuv_texs = new CriSmpMvYuvTextures;

	yuv_texs->m_uiMvWidth    = uiWidth;
	yuv_texs->m_uiMvHeight   = uiHeight;
	yuv_texs->m_bUseAlphaCh = bUseAlphaCh;

	yuv_texs->CreateTexture(uiWidth, uiHeight);

	// Keep the pointer to prevent the library from being purged when a new world is loaded
	yuv_texs->m_spExLib = Vision::Shaders.LoadShaderLibrary(SHADER_FILE);
	yuv_texs->m_pEffect = Vision::Shaders.CreateEffect("ColorConversionEffect",NULL);

	// Create technique (use a different technique depending on whether it is alpha movie or not)
	VTechniqueConfig *pGlobalConfig = Vision::Shaders.GetGlobalTechniqueConfig();
    VTechniqueConfig usageConfig;
	if (bUseAlphaCh == CRI_TRUE) {
		usageConfig.SetInclusionTags("CCIR601wAlpha");
	} else {
		usageConfig.SetInclusionTags("CCIR601");
	}
	yuv_texs->m_pTech = yuv_texs->m_pEffect->FindCompatibleTechnique(&usageConfig, pGlobalConfig);
	yuv_texs->m_pPass = yuv_texs->m_pTech->GetShader(0);

	// Y-Texture
	int iSamplerY = yuv_texs->m_pPass->GetSamplerIndexByName(VSS_PixelShader, "Tex0"); // use same sampler name as in the shader source code
	VASSERT(iSamplerY>=0 && "Sampler not found");
  VStateGroupTexture *pStateGroupTexture = yuv_texs->m_pPass->GetStateGroupTexture(VSS_PixelShader, iSamplerY);
  if (pStateGroupTexture != NULL)
    pStateGroupTexture->m_spCustomTex = yuv_texs->m_spVideoTex_Y[0]; // assign the texture to that sampler
  VStateGroupSampler *pStateGroupSampler = yuv_texs->m_pPass->GetStateGroupSampler(VSS_PixelShader, iSamplerY);
  if (pStateGroupSampler != NULL)
  {
    pStateGroupSampler->m_cTextureAddressMode[0] = TEXTURE_ADDRESS_CLAMP;	// u
	  pStateGroupSampler->m_cTextureAddressMode[1] = TEXTURE_ADDRESS_CLAMP;	// v
	  pStateGroupSampler->m_cTextureAddressMode[2] = TEXTURE_ADDRESS_CLAMP;	// w
    pStateGroupSampler->ComputeHash();
  }

	// U-Texture
	int iSamplerU = yuv_texs->m_pPass->GetSamplerIndexByName(VSS_PixelShader, "Tex1"); // use same sampler name as in the shader source code
	VASSERT(iSamplerU>=0 && "Sampler not found");
  pStateGroupTexture = yuv_texs->m_pPass->GetStateGroupTexture(VSS_PixelShader, iSamplerU);
  if (pStateGroupTexture != NULL)
    pStateGroupTexture->m_spCustomTex = yuv_texs->m_spVideoTex_U[0]; // assign the texture to that sampler
  pStateGroupSampler = yuv_texs->m_pPass->GetStateGroupSampler(VSS_PixelShader, iSamplerU);
  if (pStateGroupSampler != NULL)
  {
	  pStateGroupSampler->m_cTextureAddressMode[0] = TEXTURE_ADDRESS_CLAMP;	// u
	  pStateGroupSampler->m_cTextureAddressMode[1] = TEXTURE_ADDRESS_CLAMP;	// v
	  pStateGroupSampler->m_cTextureAddressMode[2] = TEXTURE_ADDRESS_CLAMP;	// w
    pStateGroupSampler->ComputeHash();
  }

	// V-Texture
	int iSamplerV = yuv_texs->m_pPass->GetSamplerIndexByName(VSS_PixelShader, "Tex2"); // use same sampler name as in the shader source code
	VASSERT(iSamplerV>=0 && "Sampler not found");
  pStateGroupTexture = yuv_texs->m_pPass->GetStateGroupTexture(VSS_PixelShader, iSamplerV);
  if (pStateGroupTexture != NULL)
    pStateGroupTexture->m_spCustomTex = yuv_texs->m_spVideoTex_V[0]; // assign the texture to that sampler
  pStateGroupSampler = yuv_texs->m_pPass->GetStateGroupSampler(VSS_PixelShader, iSamplerV);
  if (pStateGroupSampler != NULL)
  {
	  pStateGroupSampler->m_cTextureAddressMode[0] = TEXTURE_ADDRESS_CLAMP;	// u
	  pStateGroupSampler->m_cTextureAddressMode[1] = TEXTURE_ADDRESS_CLAMP;	// v
	  pStateGroupSampler->m_cTextureAddressMode[2] = TEXTURE_ADDRESS_CLAMP;	// w
    pStateGroupSampler->ComputeHash();
  }

	// Alpha-Texture
	if (yuv_texs->m_bUseAlphaCh == CRI_TRUE) 
  {
		int iSamplerA = yuv_texs->m_pPass->GetSamplerIndexByName(VSS_PixelShader, "Tex3"); // use same sampler name as in the shader source code
		VASSERT(iSamplerA>=0 && "Sampler not found");

    pStateGroupTexture = yuv_texs->m_pPass->GetStateGroupTexture(VSS_PixelShader, iSamplerA);
    if (pStateGroupTexture != NULL)
      pStateGroupTexture->m_spCustomTex = yuv_texs->m_spVideoTex_A[0]; // assign the texture to that sampler
    pStateGroupSampler = yuv_texs->m_pPass->GetStateGroupSampler(VSS_PixelShader, iSamplerA);
    if (pStateGroupSampler != NULL)
    {
		  pStateGroupSampler->m_cTextureAddressMode[0] = TEXTURE_ADDRESS_CLAMP;	// u
		  pStateGroupSampler->m_cTextureAddressMode[1] = TEXTURE_ADDRESS_CLAMP;	// v
		  pStateGroupSampler->m_cTextureAddressMode[2] = TEXTURE_ADDRESS_CLAMP;	// w
      pStateGroupSampler->ComputeHash();
    }
	}

	return yuv_texs;
}

void CriSmpMvYuvTextures::Destroy(void)
{
	// Release the pointer of shader library
	m_spExLib = NULL;

	DestroyTexture();
	delete this;
	return;
}

void CriSmpMvYuvTextures::CreateTexture(CriUint32 uiWidth, CriUint32 uiHeight)
{
	m_uiTexBufSize_y = uiWidth   * uiHeight   * 1;
	m_uiTexBufSize_u = uiWidth/2 * uiHeight/2 * 1;
	m_uiTexBufSize_v = uiWidth/2 * uiHeight/2 * 1;

	m_pTexBuf_y = (CriUint8*)Vision::System.Malloc( m_uiTexBufSize_y );	// 8-bit Y-frame
	m_pTexBuf_u = (CriUint8*)Vision::System.Malloc( m_uiTexBufSize_u );	// 8-bit U-frame
	m_pTexBuf_v = (CriUint8*)Vision::System.Malloc( m_uiTexBufSize_v );	// 8-bit V-frame

	for (int i=0; i<NUM_TEXBUF;i++) {
		m_spVideoTex_Y[i] = Vision::TextureManager.Create2DTextureObject("Ytex", uiWidth, uiHeight, 1, VIDEO_TEXTURE_FORMAT, VTM_FLAG_DEFAULT_NON_MIPMAPPED | VTM_FLAG_FORCELINEAR);
		VASSERT(m_spVideoTex_Y[i]);
		m_spVideoTex_U[i] = Vision::TextureManager.Create2DTextureObject("Utex", uiWidth/2, uiHeight/2, 1, VIDEO_TEXTURE_FORMAT, VTM_FLAG_DEFAULT_NON_MIPMAPPED | VTM_FLAG_FORCELINEAR);
		VASSERT(m_spVideoTex_U[i]);
		m_spVideoTex_V[i] = Vision::TextureManager.Create2DTextureObject("Vtex", uiWidth/2, uiHeight/2, 1, VIDEO_TEXTURE_FORMAT, VTM_FLAG_DEFAULT_NON_MIPMAPPED | VTM_FLAG_FORCELINEAR);
		VASSERT(m_spVideoTex_V[i]);
	}

	/* Alpha-channel frame buffer */
	if (m_bUseAlphaCh == CRI_TRUE) {
		m_uiTexBufSize_a = uiWidth   * uiHeight   * 1;
		m_pTexBuf_a = (CriUint8*)Vision::System.Malloc( m_uiTexBufSize_a );	// 8-bit Alpha-frame	

		for (int i=0; i<NUM_TEXBUF;i++) {
			m_spVideoTex_A[i] = Vision::TextureManager.Create2DTextureObject("Atex", uiWidth, uiHeight, 1, VIDEO_TEXTURE_FORMAT, VTM_FLAG_DEFAULT_NON_MIPMAPPED | VTM_FLAG_FORCELINEAR);
			VASSERT(m_spVideoTex_A[i]);
		}
	}

	/* Clear texture buffers as black color */
	ClearTextures();

	return;
}


void CriSmpMvYuvTextures::DestroyTexture(void)
{
	for (int i=0;i<NUM_TEXBUF;i++) {
		m_spVideoTex_Y[i] = NULL;
		m_spVideoTex_U[i] = NULL;
		m_spVideoTex_V[i] = NULL;
	}

	Vision::System.Free(m_pTexBuf_y);
	Vision::System.Free(m_pTexBuf_u);
	Vision::System.Free(m_pTexBuf_v);

	if (m_bUseAlphaCh == CRI_TRUE) {
		for (int i=0;i<NUM_TEXBUF;i++) {
			m_spVideoTex_A[i] = NULL;
		}
		Vision::System.Free(m_pTexBuf_a);
	}

	return;
}


void CriSmpMvYuvTextures::GetTextureBuffer(CriMvYuvBuffers &yuvbuffers)
{
	yuvbuffers.y_bufsize  = m_uiTexBufSize_y;
	yuvbuffers.y_imagebuf = m_pTexBuf_y;
	yuvbuffers.y_pitch    = m_uiMvWidth;
	yuvbuffers.u_bufsize  = m_uiTexBufSize_u;
	yuvbuffers.u_imagebuf = m_pTexBuf_u;
	yuvbuffers.u_pitch    = m_uiMvWidth/2;
	yuvbuffers.v_bufsize  = m_uiTexBufSize_v;
	yuvbuffers.v_imagebuf = m_pTexBuf_v;
	yuvbuffers.v_pitch    = m_uiMvWidth/2;

	/* Use alpha channel */
	if (m_bUseAlphaCh == CRI_TRUE) {
		yuvbuffers.a_bufsize  = m_uiTexBufSize_a;
		yuvbuffers.a_imagebuf = m_pTexBuf_a;
		yuvbuffers.a_pitch    = m_uiMvWidth;		

	/* Not using alpha channel */
	} else {
		yuvbuffers.a_imagebuf = NULL;
		yuvbuffers.a_bufsize  = 0;
		yuvbuffers.a_pitch    = 0;
	}

	return;
}


void CriSmpMvYuvTextures::UploadTextures()
{
  VISION_PROFILE_FUNCTION(VIS_PROFILE_TEMP_01);
	// Update texture data.

  m_spVideoTex_Y[m_cur_tb_idx]->UpdateRect(0, 0,0, 0,0, -1, m_pTexBuf_y, V_TEXTURE_LOCKFLAG_DISCARDABLE);
  m_spVideoTex_U[m_cur_tb_idx]->UpdateRect(0, 0,0, 0,0, -1, m_pTexBuf_u, V_TEXTURE_LOCKFLAG_DISCARDABLE);
  m_spVideoTex_V[m_cur_tb_idx]->UpdateRect(0, 0,0, 0,0, -1, m_pTexBuf_v, V_TEXTURE_LOCKFLAG_DISCARDABLE);

	int iSamplerY = m_pPass->GetSamplerIndexByName(VSS_PixelShader, "Tex0"); // use same sampler name as in the shader source code
	VASSERT(iSamplerY>=0 && "Sampler not found");
  VStateGroupTexture *pStateGroupTexture = m_pPass->GetStateGroupTexture(VSS_PixelShader, iSamplerY);
  if (pStateGroupTexture != NULL)
    pStateGroupTexture->m_spCustomTex = m_spVideoTex_Y[m_cur_tb_idx]; // assign the texture to that sampler
	int iSamplerU = m_pPass->GetSamplerIndexByName(VSS_PixelShader, "Tex1"); // use same sampler name as in the shader source code
	VASSERT(iSamplerU>=0 && "Sampler not found");
  pStateGroupTexture = m_pPass->GetStateGroupTexture(VSS_PixelShader, iSamplerU);
  if (pStateGroupTexture != NULL)
    pStateGroupTexture->m_spCustomTex = m_spVideoTex_U[m_cur_tb_idx]; // assign the texture to that sampler
	int iSamplerV = m_pPass->GetSamplerIndexByName(VSS_PixelShader, "Tex2"); // use same sampler name as in the shader source code
	VASSERT(iSamplerV>=0 && "Sampler not found");
  pStateGroupTexture = m_pPass->GetStateGroupTexture(VSS_PixelShader, iSamplerV);
  if (pStateGroupTexture != NULL)
    pStateGroupTexture->m_spCustomTex = m_spVideoTex_V[m_cur_tb_idx]; // assign the texture to that sampler
	
	// Update alpha texture buffer
	if (m_bUseAlphaCh == CRI_TRUE) 
  {
    m_spVideoTex_A[m_cur_tb_idx]->UpdateRect(0, 0,0, 0,0, -1, m_pTexBuf_a, V_TEXTURE_LOCKFLAG_DISCARDABLE);
		//Vision::TextureManager.UploadTexture2D(m_spVideoTex_A[m_cur_tb_idx], m_pTexBuf_a, VIDEO_TEXTURE_FORMAT);

		int iSamplerA = m_pPass->GetSamplerIndexByName(VSS_PixelShader, "Tex3"); // use same sampler name as in the shader source code
		VASSERT(iSamplerA>=0 && "Sampler not found");
    pStateGroupTexture = m_pPass->GetStateGroupTexture(VSS_PixelShader, iSamplerA);
    if (pStateGroupTexture != NULL)
      pStateGroupTexture->m_spCustomTex = m_spVideoTex_A[m_cur_tb_idx]; // assign the texture to that sampler
	}

	m_cur_tb_idx = (m_cur_tb_idx + 1) % NUM_TEXBUF;
	
	return;
}

void CriSmpMvYuvTextures::ClearTextures()
{
	// Fill data for Black Y=0,U=128,V=128
	memset(m_pTexBuf_y, 0, m_uiTexBufSize_y);
	memset(m_pTexBuf_u, 0x80, m_uiTexBufSize_u);
	memset(m_pTexBuf_v, 0x80, m_uiTexBufSize_v);

	// Clear the textures with the cleared buffers
  m_spVideoTex_Y[0]->UpdateRect(0, 0,0, 0,0, -1, m_pTexBuf_y, V_TEXTURE_LOCKFLAG_DISCARDABLE);
  m_spVideoTex_U[0]->UpdateRect(0, 0,0, 0,0, -1, m_pTexBuf_u, V_TEXTURE_LOCKFLAG_DISCARDABLE);
  m_spVideoTex_V[0]->UpdateRect(0, 0,0, 0,0, -1, m_pTexBuf_v, V_TEXTURE_LOCKFLAG_DISCARDABLE);

  /*
	Vision::TextureManager.UploadTexture2D(m_spVideoTex_Y[0], m_pTexBuf_y, VIDEO_TEXTURE_FORMAT);
	Vision::TextureManager.UploadTexture2D(m_spVideoTex_U[0], m_pTexBuf_u, VIDEO_TEXTURE_FORMAT);
	Vision::TextureManager.UploadTexture2D(m_spVideoTex_V[0], m_pTexBuf_v, VIDEO_TEXTURE_FORMAT);
  */

	if (m_bUseAlphaCh == CRI_TRUE) 
  {
		memset(m_pTexBuf_a, 0, m_uiTexBufSize_a);
    m_spVideoTex_A[0]->UpdateRect(0, 0,0, 0,0, -1, m_pTexBuf_a, V_TEXTURE_LOCKFLAG_DISCARDABLE);
		//Vision::TextureManager.UploadTexture2D(m_spVideoTex_A[0], m_pTexBuf_a, VIDEO_TEXTURE_FORMAT);	
	}

}

/* end of file */

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
