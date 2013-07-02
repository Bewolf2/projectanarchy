/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file BlobShadowManager.hpp

#ifndef BLOBSHADOWMANAGER_HPP_INCLUDED
#define BLOBSHADOWMANAGER_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/BlobShadow.hpp>

/// \brief
///   Manager for blob shadows. The global instance is accessible via VBlobShadowManager::GlobalManager()
///
/// This class manages a collection of blob shadows and hooks into an existing renderloop to render the shadows.
class VBlobShadowManager :  public IVisCallbackHandler_cl
{
public:
  EFFECTS_IMPEXP VBlobShadowManager();
  EFFECTS_IMPEXP virtual ~VBlobShadowManager();

  /// \brief
  ///   Called once by the plugin for initialization
  EFFECTS_IMPEXP void OneTimeInit();
  /// \brief
  ///   Called once by the plugin for de-initialization
  EFFECTS_IMPEXP void OneTimeDeInit();


  /// \brief
  ///   Remove a shadow instance (will be called automatically by d'tor)
  EFFECTS_IMPEXP void RemoveShadow(VBlobShadow *pShadow);

  /// \brief
  ///   Clear the collection of instances (frees instances that do not have external references)
  EFFECTS_IMPEXP void PurgeShadows();

  /// \brief
  ///   Render bounding boxes of all shadow instances. Must be called every frame
  EFFECTS_IMPEXP void DebugRenderShadowBoxes();

  /// \brief
  ///   Function to render all visible shadows into the scene. This function is automatically called by a renderloop hook
  EFFECTS_IMPEXP void RenderAllShadows();

  /// \brief
  ///   Implements the listener function
  EFFECTS_IMPEXP virtual void OnHandleCallback(IVisCallbackDataObject_cl *pData);

  /// \brief
  ///   Access the collection of all instances
  inline VBlobShadowCollection &Instances() {return m_Instances;}

  /// \brief
  ///   Returns the default shadow texture. Can be overloaded on a per-instance basis
  EFFECTS_IMPEXP VTextureObject* GetDefaultShadowTexture();

  /// \brief
  ///   Returns the default shader technique for shadow rendering
  EFFECTS_IMPEXP VCompiledTechnique* GetDefaultTechnique(VisStaticGeometryType_e eGeomType);

  /// \brief
  ///   Defines the entry point inside renderloops for rendering the blob shadows. Default is VRH_DECALS
  EFFECTS_IMPEXP void SetRenderHookConstant(int iConst=VRH_DECALS);

  /// \brief
  ///   Access one global instance of a shadow manager
  EFFECTS_IMPEXP static VBlobShadowManager& GlobalManager();

private:

  static VBlobShadowManager g_GlobalManager;

  // shadow instances
  VBlobShadowCollection m_Instances;

  VCompiledEffectPtr m_spDefaultFX;
  VCompiledTechniquePtr m_spDefaultTech[2]; // 0=meshes, 1=terrain

  VTextureObjectPtr m_spDefaultShadowTex;
  VisStaticGeometryInstanceCollection_cl shadowGeom; ///< temp list (avoid allocation)
  VisStaticGeometryInstanceCollection_cl shadowGeomOfType; ///< temp list (avoid allocation)
  bool m_bFailedLoading;
  int m_iRenderHookConst;
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
