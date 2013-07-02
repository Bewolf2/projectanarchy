/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file vShaderDeclarations.hpp

#ifndef VSHADERDECLARATIONS_HPP_INCLUDED
#define VSHADERDECLARATIONS_HPP_INCLUDED


/// \brief
///   Enum representing the various shader stages (i.e., pixel shader, geometry shader, vertex shader, etc.)
enum VShaderStage_e
{
  VSS_Invalid             =-1,
  VSS_VertexShader        = 0,
  VSS_PixelShader         = 1,
  VSS_SkinningShader      = 2,
  VSS_GeometryShader      = 3,
  VSS_DomainShader        = 4,
  VSS_HullShader          = 5,
  VSS_ComputeShader       = 6,

#ifdef _VR_DX11
  VSS_PlatformStageCount  = 7,
#elif defined(_VR_GX2)
  VSS_PlatformStageCount  = 4,
#else
  VSS_PlatformStageCount  = 3,
#endif

  VSS_MaxStageCount       = 7 
};

inline const char* GetShaderStageName (VShaderStage_e stage)
{
  switch (stage)
  {
  case VSS_VertexShader:
    return ("Vertex Shader");
  case VSS_PixelShader:
    return ("Pixel Shader");
  case VSS_SkinningShader:
    return ("Skinning Shader");
  case VSS_GeometryShader:
    return ("Geoemtry Shader");
  case VSS_DomainShader:
    return ("Domain Shader");
  case VSS_HullShader:
    return ("Hull Shader");
  case  VSS_ComputeShader:
    return ("Compute Shader");
  }

  //VASSERT_MSG (false, "GetShaderStageName: Unknown Shader Stage.");
  return (NULL);
}


/// \brief
///   Enum that represents a shader target profile.
enum VShaderModel_e
{
  VSM_INVALID       = -2,   ///< Reserved. Represents an invalid shader model.
  VSM_BEST          = -1,   ///< Indicates that the "best fitting" shader model should be used. Only supported as input by a few API functions.
  VSM_SM11          =  0,   ///< Unsupported. Vision now requires at least SM2.0.
  //VSM_SM12        =  1,   ///< ps only
  //VSM_SM13        =  2,   ///< ps only
  //VSM_SM14        =  3,   ///< ps only
  VSM_SM20          =  4,   ///< vs and ps
  VSM_SM2a          =  5,   ///< vs and ps
  VSM_SM2b          =  6,   ///< vs and ps
  VSM_SM30          =  7,   ///< vs and ps
  VSM_SM40          =  8,   ///< vs, ps and gs (DX10, DX11)
  VSM_SM41          =  9,   ///< vs, ps and gs (DX10, DX11)
  VSM_SM40_LEVEL_91 = 10,   ///< vs and ps (DX11 Level 9)
  VSM_SM40_LEVEL_93 = 11,   ///< vs and ps (DX11 Level 9)
  VSM_SM50          = 12,   ///< vs, ps, gs, hs, ds, cs (DX11)

  VSM_COUNT         = 13
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
