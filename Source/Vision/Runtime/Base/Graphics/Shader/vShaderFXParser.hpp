/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file vShaderFXParser.hpp

#ifndef _VSHADERFXPARSER_HPP_INCLUDED_
#define _VSHADERFXPARSER_HPP_INCLUDED_

#include <Vision/Runtime/Base/Graphics/Shader/vShaderIncludes.hpp>


#ifdef WIN32
  #if defined (_VR_DX9)
    //TODO: Specific PC DX9 platform
    #define SHADERLIB_REQUIRED_PLATFORM       SHADERLIB_PLATFORM_DX9  
  #elif defined (_VR_DX11)
    #define SHADERLIB_REQUIRED_PLATFORM       SHADERLIB_PLATFORM_DX11
  #endif



#elif defined (_VISION_XENON)
  #define SHADERLIB_REQUIRED_PLATFORM       SHADERLIB_PLATFORM_XENONDX



#elif defined (_VISION_PS3)



#elif defined (_VISION_LINUX)


#elif defined(_VISION_IOS) || defined(_VISION_ANDROID)
#elif defined(_VISION_PSP2)
#elif defined(_VISION_WIIU)

#else
 #error Undefined platform!
#endif

class VShaderEffectLib;
enum VShaderCompileFlags_e;

void RegisterAllShaderEnums();
#ifdef WIN32
bool ParseFXLib (char *text, int numChars, VShaderCompileFlags_e flags, VShaderEffectLib &fxlib);

class IVFileOutStream;
VBASE_IMPEXP VBool WriteFXLib(VShaderEffectLib &fxlib, IVFileOutStream* pOut, VShaderCompileFlags_e flags);
#endif

#ifndef _VISION_DOC

class VBASE_IMPEXP_CLASS VShaderEnum
{
public:
  VShaderEnum() {m_numEntries=0;m_EntryOfs=-1;}

  inline int GetNumEntries() const {return m_numEntries;}
  char **GetEntryList() const;

  void RegisterEnum(const char *szName, unsigned int enumval);
  const char *GetNameFromIndex(int index) const;// {if (index<0 || index>=m_numEntries) return NULL; return}
  const char *GetNameFromEnum(unsigned int enumval) const;
  int GetIndexFromName(const char *szName) const;
  int GetIndexFromEnum(unsigned int enumval) const;
  unsigned int GetEnumFromName(const char *szName) const;
  unsigned int GetEnumFromIndex(int index) const;
  VBool GetEnumFromName(const char *szName, unsigned int &dest) const;
  VBool GetCharEnumFromName(const char *szName, char &dest) const;
private:

  short m_numEntries;
  short m_EntryOfs;

public:
// enums:
  static void OneTimeInit();
  static void OneTimeDeInit();

  static int Helper_SplitVal(char *str, char **pSplits, bool removequotes, bool removeblanks=false);
  static int Helper_GetSplitString(char *command, const char *breaks, char *splitstr, char **splitList);
  static char *g_EnumText;
  static char **g_EnumEntry;
  static unsigned int *g_EnumValue;
  static int g_NumEntries;
  static int g_TextMemSize;

  // standard
  static VShaderEnum g_platform;
  static VShaderEnum g_devices; ///< wraps around VTargetDevice_e
  static VShaderEnum g_shadermodel; ///< wraps around VShaderModel_e
  static VShaderEnum g_bool;
  static VShaderEnum g_valuetype;
  static VShaderEnum g_displayhints;
  static VShaderEnum g_blendOP; ///< wraps around VBlendOp
  static VShaderEnum g_blendmode; ///< wraps around VBlendMode
  static VShaderEnum g_filtermode; ///< wraps around VFilterMode
  static VShaderEnum g_textureaddressmode; ///< wraps around VTextureAddressMode
  static VShaderEnum g_comparisionfunction; ///< wraps around VComparisonFunc
  static VShaderEnum g_stencilOP; ///< wraps around VStencilOp

  static VShaderEnum g_texturetype;
  static VShaderEnum g_passtype;
  static VShaderEnum g_cullmode; ///< wraps around VCullMode
  static VShaderEnum g_fillmode; ///< wraps around VFillmode
  static VShaderEnum g_tesselationmode; ///< wraps around VTesselationMode


};


#endif // _VISION_DOC

#endif //#define _VSHADERFXPARSER_HPP_INCLUDED_

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
