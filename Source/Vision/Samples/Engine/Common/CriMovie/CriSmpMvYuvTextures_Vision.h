/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file CriSmpMvYuvTextures_Vision.h

/****************************************************************************
 *
 * CRI Movie sample program wrappering Vision textures
 *
 * Copyright (c) 2008 CRI Middleware, Inc
 *
 * - Synopsis
 *      Vision texture management class. these textures contain the decoded
 *      output frames from CRI Movie. 
 *
 ****************************************************************************/
#ifndef	CRISMPMVYUVTEXTURES_H_INCLUDED		/* Re-definition prevention */
#define	CRISMPMVYUVTEXTURES_H_INCLUDED

/***************************************************************************
 *      Include file
 ***************************************************************************/
#include <cri_movie.h>

// Vision include files
#include <Vision/Runtime/Base/Graphics/Shader/vCompiledEffect.hpp> 
#include <Vision/Runtime/Base/Graphics/Shader/vCompiledShaderPass.hpp> 

/***************************************************************************
 *      Constant Macros
 ***************************************************************************/
#define NUM_TEXBUF (2)	// double texturing
                        // [NOTE] If quitting multi-texturing, movie playback will stutter.

/***************************************************************************
 *      INTERFACE
 ***************************************************************************/
class CriSmpMvYuvTextures
{
public:
	/* Create/Destroy instance */
	static CriSmpMvYuvTextures* Create(CriUint32 uiWidth, CriUint32 uiHeight, CriBool bUseAlphaCh = CRI_FALSE);
	virtual void Destroy(void);

	/* Texture buffers handler */
	virtual void GetTextureBuffer(CriMvYuvBuffers &yuvbuffers);
	virtual void UploadTextures();
	virtual void ClearTextures();

	inline VCompiledTechnique* GetTechnique(void) { return m_pTech; }
	inline VCompiledEffect*    GetEffect(void) { return m_pEffect; }

protected:
 // internal functions
	virtual void CreateTexture(CriUint32 uiWidth, CriUint32 uiHeight);
	virtual void DestroyTexture(void);

protected:
	CriSmpMvYuvTextures(void);
	~CriSmpMvYuvTextures(void) {}

private:

 // Video textures
	CriUint32 m_cur_tb_idx;
	VTextureObjectPtr  m_spVideoTex_Y[NUM_TEXBUF]; ///< Y frame
	VTextureObjectPtr  m_spVideoTex_U[NUM_TEXBUF]; ///< U frame
	VTextureObjectPtr  m_spVideoTex_V[NUM_TEXBUF]; ///< V frame
	VTextureObjectPtr  m_spVideoTex_A[NUM_TEXBUF]; ///< Alpha frame

 // Texture buffers
	CriUint8 *m_pTexBuf_y; CriUint32 m_uiTexBufSize_y;
	CriUint8 *m_pTexBuf_u; CriUint32 m_uiTexBufSize_u;
	CriUint8 *m_pTexBuf_v; CriUint32 m_uiTexBufSize_v;
	CriUint8 *m_pTexBuf_a; CriUint32 m_uiTexBufSize_a;

 // Shader params
	VShaderEffectLibPtr m_spExLib;
	VCompiledEffect     *m_pEffect;
	VCompiledTechnique  *m_pTech;
	VCompiledShaderPass *m_pPass;

	CriUint32 m_uiMvWidth;
	CriUint32 m_uiMvHeight;

 // Alpha param
	CriBool m_bUseAlphaCh;
};


#endif	/* CRISMPMVYUVTEXTURES_H_INCLUDED */

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
