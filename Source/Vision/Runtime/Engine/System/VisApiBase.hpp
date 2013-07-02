/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiBase.hpp

#ifndef FR_DEFINE_VISAPIBASE
#define FR_DEFINE_VISAPIBASE

#include <Vision/Runtime/Engine/System/VisApiTypedEngineObject.hpp>


/// \brief
///   Class that describes a link source/target provided for entities inside vForge
/// 
/// This class is only relevant when using as a plugin for vForge. It is never used in final game
/// applications.
/// 
/// Each instance specifies a link source or link target. User data can be supplied to identify the
/// link type later.
/// 
/// For more information about providing custom links, please refer to the documentation.
class VShapeLinkInfo : public VUserDataObj
{
public:
  /// \brief
  ///   Link endpoint types.
  enum VLinkType_e
  {
    LINK_SOURCE = 0, ///< Endpoint can be the source of a link
    LINK_TARGET = 1, ///< Endpoint can be the target of a link
    LINK_BIDIRECTIONAL = 2 ///< Endpoint can be the source or the target of a link
  };

  inline VShapeLinkInfo() 
  {
    m_eType = LINK_SOURCE;
    m_iCustomID = 0;
  }

  /// \brief
  ///   Assignment operator
  inline VShapeLinkInfo& operator = (const VShapeLinkInfo& other)
  {
    m_eType = other.m_eType;
    m_iCustomID = other.m_iCustomID;
    m_sDisplayName = other.m_sDisplayName;
    m_sUniqueStringID = other.m_sUniqueStringID;
    m_sIconFilename = other.m_sIconFilename;
    return *this;
  }

  /// \brief
  ///   Link type of this link: LINK_SOURCE (default), LINK_TARGET or LINK_BIDIRECTIONAL
  VLinkType_e m_eType;

  /// \brief
  ///   Custom integer ID for arbitrary use (e.g. to identify this link)
  int m_iCustomID;

  /// \brief
  ///   Name string of this link type that is displayed inside the engine view as tool tips
  VString m_sDisplayName;
  
  /// \brief
  ///   This must be a unique name string to identify the link. The name specified here must at
  ///   least be unique for the link collections of the owner shape
  VString m_sUniqueStringID;

  /// \brief
  ///   Optional project-relative texture filename of the icon to show in the view
  VString m_sIconFilename;
};


/// \brief
///   Defines a collection of link types (collection of VShapeLinkInfo instances).
/// 
/// A pointer to this class is passed as iParamA to the MessageFunction of entities using the
/// VIS_MSG_EDITOR_GETLINKS message ID. Entity classes can then fill this object with their custom
/// link type information.
/// 
/// For more information about providing custom links, please refer to the documentation.
class VShapeLinkConfig
{
public:
  inline VShapeLinkConfig()
  {
    m_iLinkCount = 0;
    m_pLinks = NULL;
  }

  inline ~VShapeLinkConfig()
  {
    FreeLinks();
  }

  /// \brief
  ///   Must be called by the entity to allocate iCount link types. The entity code can then fill
  ///   the link infos (GetLink function)
  /// 
  /// This function allocates additional links and preserves the existing ones.
  /// 
  /// The return value is the index of the first new link (i.e. same as GetLinkCount before
  /// allocation).
  ///
  /// \param iCount
  ///   Number of links to allocate.
  VISION_APIFUNC int AllocateLinks(int iCount);

  /// \brief
  ///   Releases all link information.
  inline void FreeLinks()
  {
    if (m_pLinks!=m_Buffer) V_SAFE_DELETE_ARRAY(m_pLinks);
    m_iLinkCount = 0;
  }

  /// \brief
  ///   Returns the number of link types allocated by AllocateLinks. See also GetLinkInfo().
  inline int GetLinkCount() const
  {
    return m_iLinkCount;
  }

  /// \brief
  ///   Returns the link info object (class VShapeLinkInfo) with specified index
  ///   [0..GetLinkCount()-1]
  inline VShapeLinkInfo &GetLinkInfo(int iIndex) const
  {
    VASSERT(iIndex>=0 && iIndex<m_iLinkCount);
    return m_pLinks[iIndex];
  }
private:
  int m_iLinkCount;
  VShapeLinkInfo *m_pLinks;

  VShapeLinkInfo m_Buffer[4];
};


/// \brief
///   Provides information about a link between two entities.
class VShapeLink
{
public:
  VShapeLink(VShapeLinkInfo &info, VisTypedEngineObject_cl *pObj) : m_LinkInfo(info)
  {
    m_pOtherObject = pObj;
    m_bResult = true;
  }


  /// \brief
  ///   Reference to the link info object
  VShapeLinkInfo &m_LinkInfo;

  /// \brief
  ///   Pointer to the other object that should be linked to
  VisTypedEngineObject_cl *m_pOtherObject;

  /// \brief
  ///   Name of the link source/target counterpart that holds m_pOtherObject
  VString m_sOtherUniqueStringID;

  /// \brief
  ///   For the VIS_MSG_EDITOR_CANLINK message, this member must be set by the entity code
  bool m_bResult;
};


#endif  // FR_DEFINE_VISAPIBASE

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
