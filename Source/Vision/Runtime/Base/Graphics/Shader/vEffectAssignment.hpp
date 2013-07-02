/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file vEffectAssignment.hpp

//vEffectAssignment.hpp
#ifndef _VEFFECTASSIGNMENT_HPP_INCLUDED
#define _VEFFECTASSIGNMENT_HPP_INCLUDED


struct SurfaceFXAssignment_t;
class IVFileInStream;
class IVFileOutStream;
class IVFileStreamManager;
class VCompiledEffect;
class VModelInfoXMLDocument;

DECLARE_LIST(VSurfaceAssignmentList,SurfaceFXAssignment_t,VBASE_IMPEXP)


/// \brief
///   Helper structure to store surface/shader effect name pairs. The VSurfaceFXConfig class keeps
///   a list of these entries.
struct SurfaceFXAssignment_t
{

  /// \brief
  ///   Constructor
  inline SurfaceFXAssignment_t() {surfaceNumber=-1;}

  /// \brief
  ///   Returns the target surface name string
  inline const char *GetSurfaceName() const {return surfacename.AsChar();}

  /// \brief
  ///   Returns the shader effect name
  inline const char *GetEffectName() const {return effectname.AsChar();}

  /// \brief
  ///   Returns the shader effect parameter string
  inline const char *GetEffectParams() {return paramstr.GetChar();}

  /// \brief
  ///   Returns the target model filename (NULL for a static mesh or custom geometry types)
  inline const char *GetModelFile() {return modelFile.GetChar();}

  /// \brief
  ///   Returns the surface (material) index (can be -1 when GetSurfaceName is used)
  inline int GetSurfaceNumber() const {return surfaceNumber;}

  int surfaceNumber;
  VString surfacename;
  VString effectlib;
  VString effectname;
  VString paramstr;
  VString modelFile;
};


/// \brief
///   Stores a list of surface shader assignments (SurfaceFXAssignment_t) and involved shader
///   library files
/// 
/// This is a legacy class and should not be used outside of the Vision codebase.
class VSurfaceFXConfig
{
public:

  /// \brief
  ///   Constructor
  VBASE_IMPEXP VSurfaceFXConfig();

  /// \brief
  ///   Destructor
  VBASE_IMPEXP virtual ~VSurfaceFXConfig();

  /// \brief
  ///   Resets the lists
  VBASE_IMPEXP void ReleaseAll();

  /// \brief
  ///   Returns the number of surface shader assignments.
  inline int GetNumAssignments() const {return m_assignment.GetLength();}

  /// \brief
  ///   Accesses a single surface shader assignment by index. Index must be in valid range.
  inline SurfaceFXAssignment_t *GetAssignment(int index) const {VASSERT (index>=0 && index<GetNumAssignments());return m_assignment.Get(index);}

  /// \brief
  ///   Returns the number of shader libararies
  inline int GetNumLibFiles() const {return m_fxlibfile.GetLength();}

  /// \brief
  ///   Accesses the filename of the shader library with specified index, where index must be
  ///   [0..GetNumLibFiles()-1]
  inline const char *GetLibFilename(int index) const {VASSERT (index>=0 && index<GetNumLibFiles());return m_fxlibfile.Get(index);}

  /// \brief
  ///   Returns the index (or -1) of the shader library with the specified filename.
  VBASE_IMPEXP int FindLibFilename(const char *szFilename) const;

  /// \brief
  ///   Adds a library filename
  VBASE_IMPEXP void AddLibFilename(const char *szFilename);

  /// \brief
  ///   Adds a library filename only if it does not exist in the list
  VBASE_IMPEXP void AddLibFilenameUnique(const char *szFilename);

  /// \brief
  ///   Adds a new surface effect (surface name, effect name and effect parameter string)
  VBASE_IMPEXP void AddSurfaceEffect(const char *szSrfName, const char *szLibName, const char *szFXName, const char *szParam);

  /// \brief
  ///   Adds a new surface effect (surface index, effect name and effect parameter string)
  VBASE_IMPEXP void AddSurfaceEffect(int srfNumber, const char *szLibName, const char *szFXName, const char *szParam);

  /// \brief
  ///   Adds a new model surface effect (model filename, surface name, effect name and effect
  ///   parameter string)
  VBASE_IMPEXP void AddModelFileEffect(const char *szModelFile, const char *szSrfName, const char *szFXName, const char *szParam);

  /// \brief
  ///   Adds a surface effect, where effect name and parameter string are extracted from pFX
  VBASE_IMPEXP void AddSurfaceEffect(const char *szSrfName, VCompiledEffect *pFX, const char *szModelPath);

  /// \brief
  ///   Sets version number (internal function for IO)
  inline void SetVersion(short vmaj, short vmin) {m_VersMaj=vmaj;m_VersMin=vmin;}

  /// \brief
  ///   Gets version number (internal function for IO)
  inline void GetVersion(short &vmaj, short &vmin) const {vmaj=m_VersMaj;vmin=m_VersMin;}

  /// \brief
  ///   Loads a config from a text file
  VBASE_IMPEXP VBool LoadFromFile(const char *szFilename, IVFileStreamManager* pMan = NULL);

  /// \brief
  ///   Loads a config from a text file
  VBASE_IMPEXP VBool LoadFromFile(IVFileInStream* pStream);

#ifdef WIN32

  /// \brief
  ///   Saves a config to text file
  VBASE_IMPEXP VBool SaveToFile(const char *szFilename, IVFileStreamManager* pMan = NULL);

  /// \brief
  ///   Saves a config to text file
  VBASE_IMPEXP VBool SaveToFile(IVFileOutStream* file);
  
  VBASE_IMPEXP void UpdateMaterialXML(VModelInfoXMLDocument *pInfo);
#endif

  VBASE_IMPEXP static void SplitEffectAndParam(char *effectString, char **szEffectName, char **szParam);
private:
  VBool ParseFile(char *text, int len);
  short m_VersMaj,m_VersMin;
  VSurfaceAssignmentList m_assignment;
  VStrList m_fxlibfile;
};


#endif // _VEFFECTASSIGNMENT_HPP_INCLUDED

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
