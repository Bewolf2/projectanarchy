/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file vTechniqueConfig.hpp

#ifndef VTECHNIQUECONFIG_HPP_INCLUDED
#define VTECHNIQUECONFIG_HPP_INCLUDED

#include <Vision/Runtime/Base/Graphics/Shader/vShaderIncludes.hpp>

/// \brief
///   Defines inclusion and exclusion conditions for accessing specific compiled techniques in
///   effects.
/// 
/// A compiled shader effect can have a collection of compiled techniques, each of them a variant of the 
/// effect that is used under different conditions. A typical use case for this is a material that
/// may have to be rendered in different ways depending on whether static or dynamic lighting is applied
/// to it. The Vision Shader Provider makes extensive use of this mechanism for identifying specific shaders
/// for different rendering conditions.
/// 
/// The technique config is used to identify a matching technique inside an effect. For that it
/// provides inclusion tags and exclusion tags.
/// 
/// If an inclusion tag is defined in the passed config but not in the technique, the technique will
/// not be used.
/// 
/// If an exclusion tag is defined in the passed config and also in the technique, the technique
/// will not be used.
/// 
/// The effect will return the first technique in its hierarchy that has a matching config.
/// 
/// On the effect resource side, the inclusion and exclusion tags can be defined in the shader
/// editor per technique.
class VTechniqueConfig : public VBaseObject
{
public:

  ///
  /// @name Constructors
  /// @{
  ///


  /// \brief
  ///   Empty constructor
  inline VTechniqueConfig() {}

  /// \brief
  ///   Copy Constructor
  inline VTechniqueConfig(const VTechniqueConfig& other)
  {
    m_InclusionTags = other.m_InclusionTags;
    m_ExclusionTags = other.m_ExclusionTags;
  }

  /// \brief
  ///   Constructor that takes inclusion and exclusion tag strings
  /// 
  /// Each string defines one or more tags  separated by semicolon, e.g. "tag1;tag2;tag3"
  /// 
  /// The tag strings are parsed and stored as bitflags for faster runtime comparisons.
  /// Thus, a technique config should be created once and passed to technique creation functions,
  /// rather than setting up a new VTechniqueConfig object every time.
  /// 
  /// \param szInclusionTags
  ///   Tag string that defines inclusion tags
  /// 
  /// \param szExclusionTags
  ///   Tag string that defines exclusion tags
  inline VTechniqueConfig(const char *szInclusionTags,const char *szExclusionTags) {Set(szInclusionTags,szExclusionTags);}
  
  ///
  /// @}
  ///

  ///
  /// @name Parse Tag Strings
  /// @{
  ///


  /// \brief
  ///   Parses the exclusion tag string and sets the internal bitflags accordingly
  VBASE_IMPEXP void SetExclusionTags(const char *szExclusionTags);

  /// \brief
  ///   Parses the inclusion tag string and sets the internal bitflags accordingly
  VBASE_IMPEXP void SetInclusionTags(const char *szInclusionTags);

  /// \brief
  ///   Removes all exclusion tags.
  VBASE_IMPEXP void ClearExclusionTags(void);

  /// \brief
  ///   Removes all inclusion tags.
  VBASE_IMPEXP void ClearInclusionTags(void);

  /// \brief
  ///   Adds the given inclusion tag to the list of inclusion tags.
  VBASE_IMPEXP void AddInclusionTag(const char *szInclusionTag);

  /// \brief
  ///   Adds the given integer inclusion tag to the list of inclusion tags (ie. "IncTag=iValue").
  VBASE_IMPEXP void AddInclusionTagInt(const char *szInclusionTagName, int iValue);

  /// \brief
  ///   Removes the given inclusion tag from the list of inclusion tags.
  VBASE_IMPEXP void RemoveInclusionTag(const char* szInclusionTag);

  /// \brief
  ///   Adds the given exclusion tag to the list of exclusion tags.
  VBASE_IMPEXP void AddExclusionTag(const char *szExclusionTag);

  /// \brief
  ///   Adds the given integer exclusion tag to the list of exclusion tags (ie. "ExcTag=iValue").
  VBASE_IMPEXP void AddExclusionTagInt(const char *szExclusionTagName, int iValue);

  /// \brief
  ///   Removes the given exclusion tag from the list of exclusion tags.
  VBASE_IMPEXP void RemoveExclusionTag(const char* szExclusionTag);

  /// \brief
  ///   Parses both the inclusion and exclusion tag strings
  void Set(const char *szInclusionTags,const char *szExclusionTags)
  {
    SetExclusionTags(szExclusionTags);
    SetInclusionTags(szInclusionTags);
  }
  
  /// \brief Returns all the inclusion tags that are set, combined into one string.
  VBASE_IMPEXP VString GetInclusionTagsString (void) const;

  /// \brief Returns all the exclusion tags that are set, combined into one string.
  VBASE_IMPEXP VString GetExclusionTagsString (void) const;

  ///
  /// @}
  ///

  ///
  /// @name Operators
  /// @{
  ///


  /// \brief
  ///   Assignment operator; copies the config
  VTechniqueConfig &operator = (const VTechniqueConfig &other)
  {
    m_InclusionTags = other.m_InclusionTags;
    m_ExclusionTags = other.m_ExclusionTags;
    return *this;
  }


  /// \brief
  ///   Checks two configs for compatibility. This function is used when finding a compatible
  ///   technique in the effect hierarchy.
  VBASE_IMPEXP bool IsCompatibleWith(const VTechniqueConfig &other) const;

  /// \brief
  ///   Function for binary serialization. Used for loading and writing vShaderBin files.
  VBASE_IMPEXP void ChunkFileExchange(VChunkFile &file);

  ///
  /// @}
  ///

  ///
  /// @name Members
  /// @{
  ///

public:
  /// \brief Returns the Inclusion Tag VBitfield.
  const VBitfield& GetInclusionTagBitfield (void) const { return (m_InclusionTags); }

  /// \brief Returns the Exclusion Tag VBitfield.
  const VBitfield& GetExclusionTagBitfield (void) const { return (m_ExclusionTags); }

private:
  VBitfield m_InclusionTags;
  VBitfield m_ExclusionTags;

  ///
  /// @}
  ///

public:
  // static helpers:
  VBASE_IMPEXP static int RegisterTag(const char *szTag);
  VBASE_IMPEXP static const char *GetTagString(int iID);  
  VBASE_IMPEXP static void WriteBitfieldToText(IVFileOutStream *pOut, const VBitfield &bits, char separator=';');
  VBASE_IMPEXP static void ReadBitfieldFromBinary(VChunkFile &file, VBitfield &bits);
  VBASE_IMPEXP static void ClearTagList();
  VBASE_IMPEXP static const VStrList& GetTagList (void) { return (g_TagList); }

private:
  static VStrList g_TagList;
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
