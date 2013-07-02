/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/RenderingHelpers/RenderingOptimizationHelpers.hpp>
#include <Vision/Runtime/Base/System/Memory/VMemDbg.hpp>




#if defined WIN32  || defined(_VISION_PSP2) || defined(_VR_GLES2)

void RenderingOptimizationHelpers_cl::SetShaderPreference(int iNumPixelGPR)
{
}

VTextureObject *RenderingOptimizationHelpers_cl::CreateAndPatchDummyTexture(VTextureObject *pRefTexture)
{
  return pRefTexture;
}

void RenderingOptimizationHelpers_cl::SetHiStencilParameters(bool bEnabled, bool bForWriting, int iRef)
{
}

#endif

const VStaticString<7> &RenderingOptimizationHelpers_cl::GetMSAAString(VVIDEO_Multisample level)
{
  static const VStaticString<7> tags[] = { "", "MSAAx2", "MSAAx4", "MSAAx8", "MSAAx16", "ERROR" };

  // perform explicit case analysis just to be safe
  switch (level)
  {
  case VVIDEO_MULTISAMPLE_OFF:
    return tags[0];
  case VVIDEO_MULTISAMPLE_2TIMES:
    return tags[1];
  case VVIDEO_MULTISAMPLE_4TIMES:
    return tags[2];
  case VVIDEO_MULTISAMPLE_8TIMES:
    return tags[3];
  case VVIDEO_MULTISAMPLE_16TIMES:
    return tags[4];
  default:
    VASSERT(0);
  }
  return tags[5];
}


const VStaticString<32> &RenderingOptimizationHelpers_cl::GetMSAAString_Perm (VVIDEO_Multisample level)
{
  static const VStaticString<32> tags[] = { "", "MSAA_SAMPLES=2", "MSAA_SAMPLES=4", "MSAA_SAMPLES=8", "MSAA_SAMPLES=16", "ERROR" };

  // perform explicit case analysis just to be safe
  switch (level)
  {
  case VVIDEO_MULTISAMPLE_OFF:
    return tags[0];
  case VVIDEO_MULTISAMPLE_2TIMES:
    return tags[1];
  case VVIDEO_MULTISAMPLE_4TIMES:
    return tags[2];
  case VVIDEO_MULTISAMPLE_8TIMES:
    return tags[3];
  case VVIDEO_MULTISAMPLE_16TIMES:
    return tags[4];
  default:
    VASSERT(0);
  }
  return tags[5];
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
