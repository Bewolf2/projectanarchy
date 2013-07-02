/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file vShaderPermutationConfig.hpp

#ifndef VPERMUTATIONCONFIG_HPP_INCLUDED
#define VPERMUTATIONCONFIG_HPP_INCLUDED

#include <Vision/Tools/Libraries/ShaderLibExport/vImpExp.hpp>
#include <Vision/Tools/Libraries/ShaderLibExport/vStringBuffer.hpp>

#include <Vision/Tools/Libraries/ShaderLibExport/LuaWrapper/vLuaWrapper.hpp>
using namespace V_NS_LUAWRAPPER;


/// \brief Type of a shader permutation variable
enum VShaderVariableType_e
{
  VSVT_None,    ///< No type (initial value)
  VSVT_Bool,    ///< Bool variable (value will be 0 or 1)
  VSVT_Int,     ///< Signed int variable
};

/// \brief Permutation Variable Flags
enum VPermutationVariableFlags_e
{
  VPVF_None               = 0,
  VPVF_Const              = 1 << 0,   ///< Variable was added as being "const" (no permutation, just one value)
  VPVF_ConstWithIncTag    = 1 << 1,   ///< Variable was added as being "const" (no permutation, just one value), but is added as an inclusion tag
};

/// \brief State of a shader permutation variable
struct VPermutationVariableInstance
{
  VStaticString<64> m_sName;        ///< The variables name, should be all upper-case (enforced in the code)
  VShaderVariableType_e m_Type;     ///< The variables type
  int m_iValue;                     ///< The variables value. For bools: 0 or 1
  unsigned char m_uiFlags;          ///< VPermutationVariableFlags_e
};

/// \brief A set of permutation variables with defined values.
struct VVariablePermutation
{
  int m_iIndexInArray; // needed after sorting to know its original position
  int m_iPermutationIndex;
  VArray<VPermutationVariableInstance> m_Instances;
};

/// \brief A class that parses a shader permutation config file and generates the set of possible permutation configurations.
class VShaderPermutationConfig
{
public:
  VSHADERLIBEXPORT_IMPEXP VShaderPermutationConfig ();
  VSHADERLIBEXPORT_IMPEXP ~VShaderPermutationConfig ();

  /// \brief Reads a permutation configuration Lua-file.
  ///
  /// \param szFile
  /// The Lua file to read.
  ///
  /// \param out_sError
  /// If the config file could not be read, this string will contain an error message.
  ///
  /// \return
  /// True if all went successful.
  VSHADERLIBEXPORT_IMPEXP bool ReadConfigFromFile (const char* szFile, VString& out_sError, IVLog *pLog, const char* szEffectName, const char* szTechniqueName);

  /// \brief Returns the number of maximum possible permutations (before filtering anything out).
  VSHADERLIBEXPORT_IMPEXP int GetMaxPermutations (void) const { return (m_iMaxPermutations); }

  /// \brief Generates all possible permutations and uses the Lua filter-function to remove unwanted permutations.
  ///
  /// \param out_FilteredPermutations
  /// Array of permutation configurations that were the result.
  VSHADERLIBEXPORT_IMPEXP void GenerateFilteredPermutations (VArray<VVariablePermutation>& out_FilteredPermutations) const;

  /// \brief Returns whether some configuration should be supported on the given platform or excluded.
  VSHADERLIBEXPORT_IMPEXP bool IsPlatformSupported (VTargetPlatform_e Platform, const VArray<VPermutationVariableInstance>& Permutation) const;

  /// \brief Returns the shader class the permutation should use
  VSHADERLIBEXPORT_IMPEXP const char* GetShaderClass(const VArray<VPermutationVariableInstance>& Permutation) const;

private:
  /// \brief Generates the n-th permutation. Returns false if the permutation is supposed to be filtered out (ie. not used).
  bool GeneratePermutation (int iPermutation, VVariablePermutation& out_Permutation) const;
  /// \brief Checks for a permutation that exactly one boolean permutation variable from each bool-group is set.
  bool IsBoolGroupConstraintMet (const VVariablePermutation& CurValues) const;

  /// \brief Lua Callback, to add an array of bools to the set of permutation variables.
  static int LUAFUNC_AddPermutationBoolArray (lua_State* state);
  /// \brief Lua Callback, to add a bool to the set of permutation variables.
  static int LUAFUNC_AddPermutationBool (lua_State* state);
  /// \brief Lua Callback, to add an int to the set of permutation variables.
  static int LUAFUNC_AddPermutationInt (lua_State* state);
  /// \brief Lua Callback, to add a constant (bool or int) to the set of permutation variables.
  static int LUAFUNC_AddPermutationConst (lua_State* state);
  /// \brief Lua Callback, to add a constant (bool or int) to the set of permutation variables, BUT will also add those as inclusion tags.
  static int LUAFUNC_AddPermutationConstAndIncTag (lua_State* state);

  static int AddPermutationConstHelper (lua_State* state, bool bAddIncTag);

  /// Defines a set of boolean variables from which only one is supposed to be true at a time.
  struct VBoolGroup
  {
    VArray<int> m_BoolVars;         ///< Array of indices to permutation variables, that make up this group.
  };

  /// The definition of one permutation variable.
  struct VPermutationVariable
  {
    VStaticString<64> m_sName;      ///< Variable Name
    VShaderVariableType_e m_Type;   ///< Variable Type
    char m_iUsedValues;             ///< How many values of m_iValues are used
    unsigned char m_uiFlags;        ///< Additional Flags
    int m_iValues[32];            ///< The different values this variable can take
    int m_iSwitchValueDivider;      ///< Used to iterate over all permutations and select to correct value.
  };


  /// All boolean groups that were added to the config.
  VArray<VBoolGroup> m_BoolGroups;

  /// All permutation variables that were added to the config.
  VArray<VPermutationVariable> m_PermutationVariables;

  /// The maximum number of permutations that could be built.
  int m_iMaxPermutations;

  /// The Lua-Script that contains the variable definitions and the filter function.
  VLuaWrapper m_ConfigScript;
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
