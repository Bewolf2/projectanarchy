/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#pragma once


/// \brief
///   File path rewriter
///
/// Runs over the scene and rewrites every file path, according to some user specified options
class VGEOM2_IMPEXP_CLASS VGProcessor_PathRewriter : public VGDynamicProcessor<VGProcessor_PathRewriter>
{
public:
  VGProcessor_PathRewriter();
  virtual ~VGProcessor_PathRewriter();


  /// \brief
  ///   Mode
  enum PathRewriteMode
  {
    VGRM_MAKE_RELATIVE,
    VGRM_MAKE_ABSOLUTE
  };

  /// \brief
  ///   Scope (combinable)
  enum PathRewriteScope
  {
    VGRS_MATERIALS = V_BIT(0), // Materials and shader assignments
    VGRS_NODES     = V_BIT(1)  // Nodes (and instances)
  };



  /// \brief
  ///   Processes a scene - rewrites pathes respecting scope and mode
  ///
  /// \param scene
  ///   Scene containing data to process
  ///
  /// \return
  ///   False if path rewriting failed
  virtual bool            Process(VGScene& scene) const;



  /// \brief
  ///   Sets the desired rewrite mode
  ///
  /// \param m
  ///   Rewrite mode
  inline void             SetMode(PathRewriteMode m) throw()     { m_mode = m; }

  /// \brief
  ///   Gets the currently set rewrite mode
  ///
  /// \return
  ///   Currently set rewrite mode
  inline PathRewriteMode  GetMode() const throw()                { return m_mode; }

  /// \brief
  ///   Sets the desired rewrite scope
  ///
  /// \param s
  ///   Rewrite scope
  inline void             SetScope(PathRewriteScope s) throw()   { m_scope = s; }

  /// \brief
  ///   Gets the currently set rewrite scope
  ///
  /// \return
  ///   Currently set rewrite scope
  inline PathRewriteScope GetScope() const throw()               { return m_scope; }

  /// \brief
  ///   Sets path processor uses as relative directory
  ///
  /// \param p
  ///   Relative path
  inline void             SetRelPath(const VString& p)           { m_relPath = p; }

  /// \brief
  ///   Gets path processor uses as relative directory
  ///
  /// \return
  ///   Relative path
  inline const VString&   GetRelPath() const throw()             { return m_relPath; }



private:
  PathRewriteMode  m_mode;
  PathRewriteScope m_scope;
  VString          m_relPath;
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
