/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file vShaderParam.hpp

#ifndef VSHADERPARAM_HPP_INCLUDED
#define VSHADERPARAM_HPP_INCLUDED

#include <Vision/Runtime/Base/Graphics/Shader/vShaderIncludes.hpp>


/// \brief
///   Class that represents a single shader effect parameter.
/// 
/// This parameter is effect-specific so it does not contain constant table register information, as different shader passes
/// may map the same parameter to different registers.
/// 
/// In addition, this parameter class contains a description of the parameter and a display hint flags that is used by vForge.
/// 
/// Thus a list of these parameter objects represents the information exposed to artists to tweak a shader effect.
class VShaderParam : public VRefCounter
{
public:

  ///
  /// @name Constructors
  /// @{
  ///

  /// \brief
  ///   Constructor, sets defaults.
  VShaderParam() {m_eValueType=VALUETYPE_FLOAT;m_eDisplayHints=DISPLAY_HINT_NONE;m_iNameID=-1;}

  /// \brief
  ///   Constructor, sets all values.
  VShaderParam(const char *szName, const char *szDescr, const char *szVal, const char *szAdditionalInfo
    , VValueType_e valType, VDisplayHintFlags_e hints)
  {
    m_iNameID = RegisterParamName(szName);
    m_sDescription = szDescr;
    m_sValue = szVal;
    m_eValueType = valType;
    m_eDisplayHints = hints;
    m_sAdditionalInfo = szAdditionalInfo;
  }


  ///
  /// @}
  ///

  ///
  /// @name Access Functions
  /// @{
  ///

  /// \brief
  ///   Returns the parameter name.
  inline const char *GetName() const {return GetParamNameString(m_iNameID);}

  /// \brief
  ///   Parses the parameter value string and returns it as an int.
  inline int    ValueToInt() const {return ValueToInt(m_sValue);}

  /// \brief
  ///   Parses the parameter value string and returnss it as a float.
  inline float  ValueToFloat() const {return ValueToFloat(m_sValue);}

  /// \brief
  ///   Parses the parameter value string and returns it as a bool.
  inline bool   ValueToBool() const {return ValueToBool(m_sValue);}

  /// \brief
  ///   Parses the parameter value string and returns it as a 2-component vector.
  inline hkvVec2 ValueToFloat2() const {return ValueToFloat2(m_sValue);}

  /// \brief
  ///   Parses the parameter value string and returns it as a 3-component vector.
  inline hkvVec3 ValueToFloat3() const {return ValueToFloat3(m_sValue);}

  /// \brief
  ///   Parses the parameter value string and returns it as a 4-component vector.
  inline hkvVec4 ValueToFloat4() const {return ValueToFloat4(m_sValue);}

  /// \brief
  ///   Returns the value string.
  inline const char *Value() const {return m_sValue;}

  /// \brief
  ///   Binary file serialization.
  VBASE_IMPEXP void ChunkFileExchange(VChunkFile &file);

  /// \brief
  /// checks if this variable needs to be sent to (i.e., updated in) the shader
  VBASE_IMPEXP bool NeedsToBeUpdated() const;

  ///
  /// @}
  ///

public:
  // static helpers:
  VBASE_IMPEXP static int RegisterParamName(const char *szName);
  VBASE_IMPEXP static int FindParamName(const char *szName);
  VBASE_IMPEXP static const char *GetParamNameString(int iID);  

#pragma warning(push)
#pragma warning(disable:4996)
  static inline int    ValueToInt(const char *szValue) {int iVal=0;sscanf(szValue,"%i",&iVal); return iVal;}
  static inline float  ValueToFloat(const char *szValue) {float fVal=0.f;sscanf(szValue,"%f",&fVal); return fVal;}
  static inline bool   ValueToBool(const char *szValue) {return (_stricmp(szValue,"true")==0);}
  static inline hkvVec2 ValueToFloat2(const char *szValue) {hkvVec2 vVal;sscanf(szValue,"%f,%f",&vVal.x,&vVal.y); return vVal;}
  static inline hkvVec3 ValueToFloat3(const char *szValue) {hkvVec3 vVal;sscanf(szValue,"%f,%f,%f",&vVal.x,&vVal.y,&vVal.z); return vVal;}
  static inline hkvVec4 ValueToFloat4(const char *szValue) {hkvVec4 vVal;sscanf(szValue,"%f,%f,%f,%f",&vVal.r,&vVal.g,&vVal.b,&vVal.a); return vVal;}
#pragma warning(pop)

  VBASE_IMPEXP static void ClearNameList();
  static VStrList g_NameList;

public:
  int m_iNameID; ///< Use GetName() to get the actual name string.
  VString m_sDescription;
  VString m_sValue;
  VString m_sAdditionalInfo; //used to save the name of the inclusion tag on the VALUETYPE_INCLUSION_TAG, if you use it for something else make sure to change ChunkFileExchange()
  VValueType_e m_eValueType;
  VDisplayHintFlags_e m_eDisplayHints;
};


/// \brief
///   Collection class for shader effect parameters, each element of type VShaderParam
class VShaderParamCollection : public VRefCountedCollection<VShaderParam>
{
public:

  /// \brief
  ///   Constructor
  VBASE_IMPEXP VShaderParamCollection();

  /// \brief
  ///   Helper function to lookup a parameter by name.
  VBASE_IMPEXP VShaderParam *FindParamByName(const char *szName) const;
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
