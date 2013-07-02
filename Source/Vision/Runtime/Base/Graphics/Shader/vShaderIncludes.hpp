/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file vShaderIncludes.hpp

#ifndef VSHADERINCLUDES_HPP_INCLUDED
#define VSHADERINCLUDES_HPP_INCLUDED

#include <Vision/Runtime/Base/System/Resource/VResourceManager.hpp>
#include <Vision/Runtime/Base/System/Resource/VResource.hpp>
#include <Vision/Runtime/Base/Container/VBitField.hpp>

#include <Vision/Runtime/Base/Graphics/Shader/vShaderConstants.hpp>


// forward declarations
class VShaderEffectResource;
class VShaderPassResource;
class VShaderParam;
class VShaderParamCollection;
class VCompiledShaderPass;
class VFloatConstantBuffer;
class VShaderEffectLib;
class VShaderEffectTechnique;
class VShaderEffectResource;
class VShaderProgramResource;
class VCompiledTechnique;
class VCompiledEffect;
class VTechniqueConfig;
class VCompiledShaderManager;
class IVTextureLoader;
class VLoadedCGProgram;
class VLoadedGLES2Program;
class VLoadedGX2Program;

#ifdef _VR_DX9
struct IDirect3DVertexShader9;
struct IDirect3DPixelShader9;
#endif



// smart pointer defs
typedef VSmartPtr<VShaderEffectResource>  VShaderEffectResourcePtr;
typedef VSmartPtr<VShaderPassResource>    VShaderPassResourcePtr;
typedef VSmartPtr<VCompiledShaderPass>    VCompiledShaderPassPtr;
typedef VSmartPtr<VShaderEffectLib>       VShaderEffectLibPtr;
typedef VSmartPtr<VShaderEffectTechnique> VShaderEffectTechniquePtr;
typedef VSmartPtr<VShaderEffectResource>  VShaderEffectResourcePtr;
typedef VSmartPtr<VShaderProgramResource> VShaderProgramResourcePtr;
typedef VSmartPtr<VCompiledTechnique>     VCompiledTechniquePtr;
typedef VSmartPtr<VCompiledEffect>        VCompiledEffectPtr;

#if defined(_VR_DX9)
  typedef IDirect3DVertexShader9 VVertexShaderObject;
  typedef IDirect3DPixelShader9 VPixelShaderObject;
#elif defined(_VR_DX11)
  typedef ID3D11VertexShader VVertexShaderObject;
  typedef ID3D11PixelShader VPixelShaderObject;
  typedef ID3D11GeometryShader VGeometryShaderObject;
  typedef ID3D11HullShader VHullShaderObject;
  typedef ID3D11DomainShader VDomainShaderObject;
  typedef ID3D11ComputeShader VComputeShaderObject;
#elif defined(_VISION_PS3)
  typedef VLoadedCGProgram VVertexShaderObject;
  typedef VLoadedCGProgram VPixelShaderObject;
#elif defined(_VISION_LINUX) //@@@L might be merged with ps3 section
  typedef VLoadedCGProgram VVertexShaderObject;
  typedef VLoadedCGProgram VPixelShaderObject;
#elif defined(_VISION_IOS)
  typedef VLoadedGLES2Program VVertexShaderObject;
  typedef VLoadedGLES2Program VPixelShaderObject;
#elif defined(_VISION_ANDROID)
  typedef VLoadedGLES2Program VVertexShaderObject;
  typedef VLoadedGLES2Program VPixelShaderObject;
#elif defined(_VISION_WIIU)

  template<typename ShaderType> class VGX2ShaderObject : public VRefCounter
  {
    public:
      
      VGX2ShaderObject(ShaderType* pShaderPointer)
        : m_pShaderPointer(pShaderPointer)
      {
        this->AddRef();
      }

      ShaderType* GetShader() const
      {
        return m_pShaderPointer;
      }

    private:

      mutable ShaderType* m_pShaderPointer;

  };

  typedef VGX2ShaderObject<GX2VertexShader> VVertexShaderObject;
  typedef VGX2ShaderObject<GX2PixelShader> VPixelShaderObject;
  typedef VGX2ShaderObject<GX2GeometryShader> VGeometryShaderObject;
#elif defined(_VISION_PSP2)
  typedef VLoadedCGProgram VVertexShaderObject;
  typedef VLoadedCGProgram VPixelShaderObject;
#endif

typedef VSmartPtr<VVertexShaderObject> VVertexShaderObjectPtr;
typedef VSmartPtr<VPixelShaderObject> VPixelShaderObjectPtr;
#if defined(SUPPORTS_GEOMETRY_SHADER)
  typedef VSmartPtr<VGeometryShaderObject> VGeometryShaderObjectPtr;
#endif
#if defined(SUPPORTS_TESSELATION_SHADER)
  typedef VSmartPtr<VHullShaderObject> VHullShaderObjectPtr;
  typedef VSmartPtr<VDomainShaderObject> VDomainShaderObjectPtr;
#endif
#if defined(SUPPORTS_COMPUTE_SHADER)
  typedef VSmartPtr<VComputeShaderObject> VComputeShaderObjectPtr;
#endif

class IVLog;
class VShaderEffectResource;
class VShaderPassResource;


// global function
VBASE_IMPEXP unsigned int ComputeHash(const void *pData, unsigned int iSize);
VBASE_IMPEXP void InvalidateStates(void *pData, unsigned int iSize);

VBASE_IMPEXP VulpType_e ValueTypeToVulpType(VValueType_e eType);


#define SHADERLIB_BINFILE_VERSION_0   0
// 7.5.5: technique names
#define SHADERLIB_BINFILE_VERSION_1   1
// 8.1.2: added support for loading shader-libs from shader-bins for the Wii
#define SHADERLIB_BINFILE_VERSION_2   2
// 8.1.6: render target wirte mask rather than writemask
#define SHADERLIB_BINFILE_VERSION_3   3
// 8.2.0: Shader Permutation Techniques
#define SHADERLIB_BINFILE_VERSION_4   4
// 2012.1: blendFactor for StateGroupBlend
#define SHADERLIB_BINFILE_VERSION_5   5
// 2013.1: new pass types
#define SHADERLIB_BINFILE_VERSION_6   6
// 2013.1: texture size constant
#define SHADERLIB_BINFILE_VERSION_7   7
// 2013.1: texture size constant
#define SHADERLIB_BINFILE_VERSION_8   8

#define SHADERLIB_BINFILE_CURRENTVERSION   SHADERLIB_BINFILE_VERSION_8

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
