/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#pragma once


/// \brief
///   Texture preparation tool, copying to specified directory and rewriting paths in scene
///
/// Runs over the scene and rewrites every texture file path and tries to locate and copy referenced
/// textures to new location (new pathes might be absolute, use VGProcessor_PathRewriter to adapt them)
class VGEOM2_IMPEXP_CLASS VGProcessor_TextureCopier : public VGDynamicProcessor<VGProcessor_TextureCopier>//@@@ we can extend this class to a metadata copier... e.g. other referenced files, etc.
{
public:
  VGProcessor_TextureCopier();
  virtual ~VGProcessor_TextureCopier();



  /// \brief
  ///   Processes a scene - copies textures and rewrites pathes
  ///
  /// \param scene
  ///   Scene containing data to process
  ///
  /// \return
  ///   False if path copying and rewriting failed
  virtual bool            Process(VGScene& scene) const;



  /// \brief
  ///   Sets path processor use as target directory for textures (and to rewrite paths)
  ///
  /// \param p
  ///   Texture path
  inline void             SetTexturePath(const VString& p)           { m_texturePath = p; }//@@@ test

  /// \brief
  ///   Gets path processor use as target directory for textures (and to rewrite paths)
  ///
  /// \return
  ///   Texture path
  inline const VString&   GetTexturePath() const throw()             { return m_texturePath; }//@@@ test



private:
  VString m_texturePath;
};

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
