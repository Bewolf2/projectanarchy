/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#pragma once


/// \brief
///   Mesh proxy class (placeholder for unloaded meshes - designed to be used with VGInstance)
class VGEOM2_IMPEXP_CLASS VGMeshProxy : public VGNode, public VGInstantiable // NOTE: Maybe simply rename to "Proxy" ?
{
public:

  enum LODUsage
  {
    DEFINITION_IN_MESH_FILE,
    FARCLIP_ONLY,
    LOD_DEFINITION_FILE
  };

  enum CollisionBehavior
  {
    FROM_FILE,
    FORCE_NO_COLLISION,
    CUSTOM
  };


  VGMeshProxy() : 
    VGNode("")                                                    
    , m_iVisibleBitmask(0xffffffff)
    , m_iCollisionBitmask(0xffff)
    , m_iLightInfluenceBitmask(0xffff)
    , m_iCollisionLayerID(2)
    , m_fSampleOfsX(0)
    , m_fSampleOfsY(0)
    , m_fSampleOfsZ(0)
    , m_fLightmapSizeMultiplier(1)
    , m_fFarClipDistance(0)
    , m_bCastStaticShadows(true)
    , m_bCastDynamicShadows(false)
    , m_bShowLightGridWidget(false)
    , m_eLODUsage(DEFINITION_IN_MESH_FILE)
    , m_eCollisionBehavior(FROM_FILE)
    , m_iSubSystemID(0)
    , m_iSubSystemDontCollideWith(0)
    , m_iSystemGroup(0)
  { }

  VGMeshProxy(const VGMeshProxy& other)                                         
  { operator=(other); }
  
  virtual ~VGMeshProxy()                                                        
  { }

  /// \brief
  ///   Copies essential data to passed-in instance, e.g. initial name and transformation, etc..
  ///
  /// \param inst
  ///   Instance to copy data to
  virtual void        CopyEssentialsToInstance(VGInstance& inst) const          { inst.SetName(GetName()); inst.SetTransformation(GetTransformation()); }

  /// \brief
  ///   Copies essential data from passed-in instance, e.g. initial name and transformation, etc.
  ///
  /// \param inst
  ///   Instance to copy data from
  virtual void        CopyEssentialsFromInstance(const VGInstance& inst)        { SetName(inst.GetName()); SetTransformation(inst.GetTransformation()); }



  /// \brief
  ///   Gets some brief information about the type of this instantiable
  ///
  /// \return
  ///   String with name of instantiable type
  virtual const char* GetTypeString() const throw()                             { return "MeshProxy"; }



  /// \brief
  ///   Assignment operator
  ///
  /// \param rhs
  ///   Proxy to assign
  ///
  /// \return
  ///   Reference to this proxy
  inline VGMeshProxy& operator=(const VGMeshProxy& rhs)                         
  { 
    m_iVisibleBitmask = rhs.m_iVisibleBitmask;
    m_iCollisionBitmask = rhs.m_iCollisionBitmask;
    m_iLightInfluenceBitmask = rhs.m_iLightInfluenceBitmask;
    m_iCollisionLayerID = rhs.m_iCollisionLayerID;

    m_iSubSystemID = rhs.m_iSubSystemID;
    m_iSubSystemDontCollideWith = rhs.m_iSubSystemDontCollideWith;
    m_iSystemGroup = rhs.m_iSystemGroup;

    m_fSampleOfsX = rhs.m_fSampleOfsX;
    m_fSampleOfsY = rhs.m_fSampleOfsY;
    m_fSampleOfsZ = rhs.m_fSampleOfsZ;

    m_fLightmapSizeMultiplier = rhs.m_fLightmapSizeMultiplier;
    m_fFarClipDistance = rhs.m_fFarClipDistance;

    m_bCastStaticShadows = rhs.m_bCastStaticShadows;
    m_bCastDynamicShadows = rhs.m_bCastDynamicShadows;
    m_bShowLightGridWidget = rhs.m_bShowLightGridWidget;

    m_eLODUsage = rhs.m_eLODUsage;
    m_eCollisionBehavior = rhs.m_eCollisionBehavior;

    m_key = rhs.m_key;
    m_lodDefinitionFile = rhs.m_lodDefinitionFile;

    VGNode::operator=(rhs); 
    VGInstantiable::operator=(rhs); 
    return *this; 
  }

public:

  unsigned int m_iVisibleBitmask;
  unsigned int m_iCollisionBitmask;
  unsigned int m_iLightInfluenceBitmask;
  unsigned int m_iCollisionLayerID;

  unsigned int m_iSubSystemID;
  unsigned int m_iSubSystemDontCollideWith;
  unsigned int m_iSystemGroup;

  float m_fSampleOfsX;
  float m_fSampleOfsY;
  float m_fSampleOfsZ;

  float m_fLightmapSizeMultiplier;
  float m_fFarClipDistance;

  bool m_bCastStaticShadows;
  bool m_bCastDynamicShadows;
  bool m_bShowLightGridWidget;

  LODUsage m_eLODUsage;
  CollisionBehavior m_eCollisionBehavior;

  VString m_key;
  VString m_lodDefinitionFile;
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
