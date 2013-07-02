/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file vPlatformHandlerShaderInfo.hpp

#ifndef VPLATFORMHANDLER_SHADERINFO_HPP
#define VPLATFORMHANDLER_SHADERINFO_HPP

#include <Vision/Runtime/Base/Graphics/Shader/vShaderConstants.hpp>
#include <Vision/Runtime/Base/Graphics/Shader/vShaderDeclarations.hpp>

// only defined in platform handler
#define V_PLATFORM_SAFE_RELEASE(x) { if (x) { x->Release();       x=NULL; } }

#ifndef _VISION_DOC

// Please note that if this struct is changed the platform handlers may have to be adjusted
// For example the shader preprocessor for the PSP2!
struct VShaderConstant
{
  VShaderConstant () : m_iRegisterIndex (0), m_eValueType (VALUETYPE_UNKNOWN), m_iArrayElements (1)
  {
    m_szName[0] = '\0';
  }

  char m_szName[128];
  int m_iRegisterIndex;
  VValueType_e m_eValueType;
  int m_iArrayElements;
};

struct VShaderSourceInfo
{
  VShaderSourceInfo()
  {
    m_ShaderType = VSS_Invalid;
    m_iUsedInputStreamMask = -1;
    m_iUsedOutputStreamMask = -1;
    m_iTrackingMask = -1;

    m_iCompilerFlags = VSF_NONE;
    m_iNumHSControlPoints = 3;
    m_szCompilerOptions = "";
    m_bSkinningShader = false;

    m_szShaderSource = "";
    m_iShaderSourceLength = 0;
    m_iNumDataPaths = 0;
    m_szEntryFunctionName = "";
    m_szShaderProfileName = "";

    m_ppByteCode = NULL;
    m_pByteCodeLength = NULL;
    m_pConstantTables = NULL;
    m_iNumConstantTables = 0;
    m_szErrorMsgBuffer = NULL;
    m_szShaderName = "";
  }


  inline void AddTrackingRegister (int iRegister)
  {
    if (m_ShaderType == VSS_VertexShader)
    {
      if (iRegister == VSHADER_VS_REGISTER_MODELVIEWMATRIX)
        m_iTrackingMask |= VSHADER_TRACKING_MODELVIEWMATRIX;
      if (iRegister == VSHADER_VS_REGISTER_MODELVIEWPROJECTIONMATRIX)
        m_iTrackingMask |= VSHADER_TRACKING_MODELVIEWPROJECTIONMATRIX;
      if (iRegister == VSHADER_VS_REGISTER_MODELTOWORLDSPACEMATRIX)
        m_iTrackingMask |= VSHADER_TRACKING_MODELTOWORLDSPACEMATRIX;
      if (iRegister >= VSHADER_VS_REGISTER_LIGHTGRID && iRegister <= VSHADER_VS_REGISTER_LIGHTGRID_LAST)
        m_iTrackingMask |= VSHADER_TRACKING_LIGHTGRID_VS;
      if (iRegister == VSHADER_VS_REGISTER_LIGHTMAPTOLIGHTMASK)
        m_iTrackingMask |= VSHADER_TRACKING_LIGHTMASK_MATRIX;
    }
    if (m_ShaderType == VSS_PixelShader)
    {
      if (iRegister == VSHADER_PS_REGISTER_MODELVIEWMATRIX)
        m_iTrackingMask |= VSHADER_TRACKING_MODELVIEWMATRIX_PS;
      if (iRegister >= VSHADER_PS_REGISTER_LIGHTGRID && iRegister <= VSHADER_PS_REGISTER_LIGHTGRID_LAST)
        m_iTrackingMask |= VSHADER_TRACKING_LIGHTGRID_PS;
    }
  }

  static const char* GetShaderTypeDefine (VShaderStage_e Stage)
  {
    switch (Stage)
    {
    case VSS_VertexShader:
      return ("_IS_VERTEX_SHADER");
    case VSS_PixelShader:
      return ("_IS_PIXEL_SHADER");
    case VSS_SkinningShader:
      return ("_IS_SKINNING_SHADER");
    case VSS_GeometryShader:
      return ("_IS_GEOMETRY_SHADER");
    case VSS_HullShader:
      return ("_IS_HULL_SHADER");
    case VSS_DomainShader:
      return ("_IS_DOMAIN_SHADER");
    case VSS_ComputeShader:
      return ("_IS_COMPUTE_SHADER");
    default:
      return (":.$ WHEN YOU ADD A SHADER-TYPE, PLEASE ADD ITS DEFINE TO VShaderSourceInfo::GetShaderTypeDefine");
    }
  }

  const char* m_szShaderName;

  VShaderStage_e m_ShaderType;
  const char* m_szShaderSource;
  int m_iShaderSourceLength;
  int m_iNumDataPaths;
  const char *m_szDataPaths[16];
  const char *m_szEntryFunctionName;
  const char *m_szShaderProfileName;
  int m_iCompilerFlags; ///< only compile relevant flags from VsfLoadSaveFlags_e enum
  const char *m_szCompilerOptions;

  // return values:
  int **m_ppByteCode;
  int *m_pByteCodeLength;
  VShaderConstant *m_pConstantTables;
  int m_iNumConstantTables;
  char *m_szErrorMsgBuffer;
  int m_iUsedInputStreamMask; ///< if -1, then required streams cannot gathered from shader (currently DX10/DX11 support it). Otherwise it uses VERTEX_STREAM_POSITION,... bitflags
  int m_iUsedOutputStreamMask; ///< if -1, then required streams cannot gathered from shader (currently DX10/DX11 support it). Otherwise it uses VERTEX_STREAM_POSITION,... bitflags
  int m_iTrackingMask; ///< required tracking mask; if -1, then the compiler cannot evaluate this mask 
  int m_iNumHSControlPoints; ///< DX11 specific: Number of tesselation control points
  bool m_bSkinningShader;
};


#endif // _VISION_DOC

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
