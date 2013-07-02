/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file TerrainDecorationEntityModel.hpp

#ifndef TERRAINDECORATIONENTITYMODEL_HPP_INCLUDED
#define TERRAINDECORATIONENTITYMODEL_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/TerrainModule.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Geometry/TerrainDecorationModel.hpp>

/// \brief
///  Interface to add support for custom decoration model types
///
/// This interface can be used for creation of custom model types for decoration editing.
/// Instances of this factory can be registered via VTerrainDecorationModelManager::GlobalManager().RegisterFactory
///
/// \see
///  VTerrainDecorationModelManager::RegisterFactory
class VTerrainDecorationEntityModelFactory : public IVTerrainDecorationModelFactory
{
public:
  VTerrainDecorationEntityModelFactory() : IVTerrainDecorationModelFactory(IVTerrainDecorationModel::Model)
  {
  }

  /// \brief
	///  This pure virtual function has to be implemented to return model resources of specified file extension
	///
	/// \param szFilename
	///  Filename of the model
	///
	/// \param pProperties
	///  Additional properties
	///
	/// \returns
	///  A new model resource instance. Or NULL if this factory does not support the file extension in the filename.
	///
	/// \remarks
	///  The speedtree binding uses this to incorporate it into the vegetation painting.
	///
	TERRAIN_IMPEXP virtual IVTerrainDecorationModel* CreateModelWithFilename(const char *szFilename, const IVTerrainDecorationModel::ModelProperties_t *pProperties);
};


/// \brief
///  Specific vegetation resource class for rendering .model instances
///
/// \see
///  VTerrainDecorationEntityModelFactory
class VTerrainDecorationEntityModel : public IVTerrainDecorationModel
{
public:
  /// \brief
	///  Constructor
	///
	/// \param szFilename
	///  Filename of the model
	///
	/// \param pProperties
	///  Additional model properties
	///
	TERRAIN_IMPEXP VTerrainDecorationEntityModel(const char *szFilename, const ModelProperties_t *pProperties=NULL) :
    IVTerrainDecorationModel(szFilename, pProperties)
  {
  }

  /// \brief
	///  Overridden reload function
	///
	/// \returns
	///  Success of the reload function
	///
	TERRAIN_IMPEXP virtual BOOL Reload();

  /// \brief
	///  Overridden unload function
	///
	/// \returns
	///  Success of the unload function
	///
  TERRAIN_IMPEXP virtual BOOL Unload();

  /// \brief
	///  Overridden function to update shader constants
	///
	TERRAIN_IMPEXP virtual void UpdateParameter();

  /// \brief
	///  Overridden function to collect an instance's static lighting mesh
	TERRAIN_IMPEXP virtual void GatherLightmapInfo(VLightmapSceneInfo &info, VTerrainDecorationInstance **pInst, int iCount);


  /// \brief
	///  Overridable to respond to editing events
	///
	/// \param action
	///  sub-action enum
	///
	/// \param pTerrain
	///  terrain instance
	///
	/// \param pDataA
	///  data pointer, depends on action
	///
	/// \param pDataB
	///  data pointer, depends on action
	///
	TERRAIN_IMPEXP virtual void OnEditorEvent(EditorEvent_e action, VTerrain *pTerrain, void *pDataA, void *pDataB);

  /// \brief
	///  Renders actual instances
	///
	/// \param pInfoComp
	///  source visibility
	///
	/// \param pInstList
	///  List of visible instances
	///
	/// \param iCount
	///  Number of list entries
	///
  /// \param eRenderMode
  ///  The render mode to use
  ///
	TERRAIN_IMPEXP virtual void RenderBatch(VTerrainVisibilityCollectorComponent *pInfoComp, VTerrainDecorationInstance **pInstList, int iCount,
                                          RenderMode_e eRenderMode = RENDER_MODE_OTW) HKV_OVERRIDE;

  /// \brief
  ///   Internal functions that is used for depth fill rendering
  void RenderBatchDepthFill(VTerrainVisibilityCollectorComponent *pInfoComp, VTerrainDecorationInstance **pInstList, int iCount);

  /// \brief
  ///   Internal functions that is used for depth shadow rendering
  void RenderBatchDepthShadow(VTerrainVisibilityCollectorComponent *pInfoComp, VTerrainDecorationInstance **pInstList, int iCount);

  /// \brief
  ///   Internal functions that is used for OTW rendering
  void RenderBatchOTW(VTerrainVisibilityCollectorComponent *pInfoComp, VTerrainDecorationInstance **pInstList, int iCount);

  /// \brief
  ///   Internal functions that is used for IR rendering
  void RenderBatchIR(VTerrainVisibilityCollectorComponent *pInfoComp, VTerrainDecorationInstance **pInstList, int iCount, RenderMode_e eRenderMode);

  /// \brief
  ///   Recreates the IR Shaders for pre and main pass
  void RecreateIRShaders(VisSurface_cl* pSurface, VShaderEffectLib* pEffectLib);

  /// \brief
  ///   Retrieves the param string used to setup the IR shader
  char* GetParamStringForIRSurface(VisSurface_cl* pSurface, char* pszParam);

  /// \brief
	///  Return the decoration type
  TERRAIN_IMPEXP virtual ModelType_e GetDecorationType() const {return Model;}

  ///\brief
	///  Overridden function that returns 0 or 1 depending on the capsule radius in the model properties
  TERRAIN_IMPEXP virtual int GetCollisionSubObjectCount() HKV_OVERRIDE
  {
    return (IsValid() && m_Properties.m_fCollisionCapsuleRadius > 0.f) ? 1 : 0;
  }

  ///\brief
  ///  Overridden function that returns a capsule in case a capsule radius has been defined
  TERRAIN_IMPEXP virtual VDecorationCollisionPrimitive* GetCollisionSubObject(int iIndex) HKV_OVERRIDE;

#ifdef SUPPORTS_SNAPSHOT_CREATION
  /// \brief
	///  Overridden function to gather resource dependencies
	///
	/// \param snapshot
	///  snapshot info
	///
	TERRAIN_IMPEXP virtual void GetDependencies(VResourceSnapshot &snapshot)
  {
    if (m_spMesh!=NULL)
      m_spMesh->GetDependencies(snapshot);
  }
#endif

  void ReapplyShaders();

  /// \brief 
  ///   Data per decoration instance
  struct VPerInstanceData_t
  {
    hkvVec4 vPerInstanceColor;
  };

  VTypedConstantBuffer<VPerInstanceData_t> m_PerInstanceData;

  //VisModelPtr m_spCoreModel;  ///< pointer to engine model to render
  VDynamicMeshPtr m_spMesh;

  // instancing:
  VisMeshBufferPtr m_spModelMesh;
  VCompiledTechniquePtr m_spInstancingTech;
  VCompiledTechniquePtr m_spInstancingTechShadow;
  VCompiledTechniquePtr m_spInstancingTechIRPrePass;
  VCompiledTechniquePtr m_spInstancingTechIRMainPass;

  int m_iModelStreams;

  VisShaderSetPtr m_spVegetationShaders;
  VDecorationCollisionPrimitive m_CollisionCapsule;
  static VTerrainDecorationEntityModelFactory g_EntityModelFactory; ///< one static instance that is globally registered
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
