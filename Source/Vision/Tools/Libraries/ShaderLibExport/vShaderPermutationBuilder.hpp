/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VShaderPermutationBuilder.hpp

#ifndef VSHADERPERMUTATIONBUILDER_HPP_INCLUDED
#define VSHADERPERMUTATIONBUILDER_HPP_INCLUDED

#include <Vision/Runtime/Base/VBase.hpp>
#include <Vision/Runtime/Base/System/vLog.hpp>
#include <Vision/Tools/Libraries/ShaderLibExport/vShaderPermutationConfig.hpp>

struct lua_State;

/// \brief A class to generate shader permutations.
class VShaderPermutationBuilder
{
public:
  /// \brief Static function to replace all #include-directives in the shader code with the proper content.
  ///
  /// \param szSource
  /// The source-code of the shader.
  ///
  /// \param szShaderFilename
  /// The path to the file that contains the shader. Needed to resolve #include-directives inside the shader.
  ///
  /// \param out_bError
  /// Flag that says whether there was any error while processing the shader.
  /// If there was an error, the function will always return an empty string.
  ///
  /// \param pLog
  /// Logging structure where lots of information is sent to. Mostly interesting if an error occurred.
  ///
  /// \return
  /// Returns the complete shader code, after all #include-directives have been resolved.
  VSHADERLIBEXPORT_IMPEXP static VString ResolveAllIncludes (const char* szSource, const char* szShaderFilename, const char* szShaderName, bool& out_bError, IVLog* pLog);

  /// \brief Static helper function to quickly find out whether a shader uses permutation variables, at all.
  ///
  /// \param szSource
  /// The source-code of the shader.
  ///
  /// \param szShaderFilename
  /// The path to the file that contains the shader. Needed to resolve #include-directives inside the shader.
  ///
  /// \param out_bError
  /// Flag that says whether there was any error while processing the shader.
  /// If there was an error, the function will always return "false" (ie. saying it does NOT contain permutation variables).
  ///
  /// \param pLog
  /// Logging structure where lots of information is sent to. Mostly interesting if an error occurred.
  ///
  /// \return
  /// Returns true if the shader uses permutation variables, false if not or if there was an error.
  VSHADERLIBEXPORT_IMPEXP static bool DoesShaderUsePermutationVariables (const char* szSource, const char* szShaderFilename, bool& out_bError, IVLog* pLog);

  /// \brief Sets the base source-code from which to generate the permutations.
  /// 
  /// The function parses the source-code, extracts the used permutation variables and prepares everything to generate permutations.
  ///
  /// \param szSource
  /// The shader source-code.
  ///
  /// \param szShaderFilename
  /// The path to the file that contains the shader. Needed to resolve #include-directives inside the shader.
  ///
  /// \param pLog
  /// Logging structure where lots of information is sent to.
  VSHADERLIBEXPORT_IMPEXP bool SetShaderSource (const char* szSource, const char* szShaderFilename, IVLog* pLog);

  /// \brief Returns whether the previously specified shader code contained any permutation variables.
  //VSHADERLIBEXPORT_IMPEXP bool HasShaderPermutationVariables (void) const {return (m_UsedVariables.GetLength () > 0);}

  /// \brief Returns a list of all the names of the permutation variables that are used in the shader.
  VSHADERLIBEXPORT_IMPEXP const VStrList& GetUsedShaderPermutationVariables (void) const {return (m_UsedVariables);}

  /// \brief Generates all permutations under the given config and determines which are unique.
  ///
  /// \param cfg
  /// Contains information how to permute the variables.
  ///
  /// \param out_Permutations
  /// For each "allowed/filtered" permutation this will contain the exact variable configuration.
  ///
  /// \param out_sUniqueShaders
  /// The list of all the unique shader-strings that were generated.
  ///
  /// \param out_PermutationShaderID
  /// For each allowed permutation this list will contain the index which of the "unique" shaders it uses.
  ///
  /// \param pLog
  /// Logging structure.
  VSHADERLIBEXPORT_IMPEXP bool GenerateUniqueShaderPermutations (const VShaderPermutationConfig& cfg, VArray<VVariablePermutation>& out_Permutations, VStrList& out_sUniqueShaders, VArray<int>& out_PermutationShaderID, IVLog* pLog) const;

private:
  /// \brief Generates a shader permutation under a given configuration of permutation variables.
  ///
  /// After the shader code was specified with SetShaderSource, this function should be called for each
  /// permutation variable configuration of interest, to retrieve the resulting shader code.
  ///
  /// \param out_ShaderSource
  /// The resulting shader code is stored here.
  ///
  /// \param VarCfg
  /// A specific configuration of permutation variables.
  ///
  /// \param pLog
  /// Logging structure where lots of status / error messages are sent.
  ///
  /// \return
  /// True if code generation succeeded, false otherwise.
  bool GenerateShaderCode (VString& out_ShaderSource, const VVariablePermutation& VarCfg, VString& out_sPrologue, IVLog* pLog) const;


  /// \brief Finds all permutation variables that are used inside $ifdef / $if / $elif
  bool FindUsedVariables (const char* szString, IVLog* pLog);

  /// \brief Finds all permutation variables that are used as replacements, such as "int iMSAA = $MSAA_SAMPLES;"
  void FindRemainingVariables (const char* szString, IVLog* pLog);

  /// \brief Replaces all $if / $ifdef / $elif with proper Lua code for later execution.
  bool ReplaceIfDefsWithLua (VStringBuffer& out_Result, const char* szSource, IVLog* pLog);


  VString m_sShaderCodeAsLua;
  VStrList m_UsedVariables;
  mutable VStringBuffer m_sGeneratedShaderCode;
  mutable IVLog* m_pLog;

  static int LUAFUNC_ReadCVAR (lua_State* state);
  static int LUAFUNC_WriteCVAR (lua_State* state);
  /// \brief Lua Callback, appends shader source to the output.
  static int LUAFUNC_AddSourceCode (lua_State* state);
  /// \brief Lua Callback, called whenever an unknown variable is found, to add it to the set of permutation variables.
  static int LUAFUNC_AddVar (lua_State* state);
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
