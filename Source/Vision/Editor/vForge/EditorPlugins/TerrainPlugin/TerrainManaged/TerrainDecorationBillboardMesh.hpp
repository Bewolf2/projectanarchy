/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file TerrainDecorationBillboardMesh.hpp

#ifndef TERRAINDECORATIONBILLBOARDMESH_HPP_INCLUDED
#define TERRAINDECORATIONBILLBOARDMESH_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/TerrainModule.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Geometry/TerrainDecorationModel.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Editing/EditableTerrainSector.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/BillboardGroup.hpp>

#using <mscorlib.dll>
#include <vcclr.h>

using namespace CSharpFramework::View;
using namespace ManagedFramework;

/// \brief
///  Implements billboards into terrain painting
///
/// Why is this file in vForge and not VisionEnginePlugin?
/// Because the runtime does not need it. It is just supplier class that generates billboard meshes internally and saves them along
/// with the per-sector objects. Since we need the TextureAtlas class here, this class is in vForge
class VTerrainDecorationBillboardMeshFactory : public IVTerrainDecorationModelFactory
{
public:
  VTerrainDecorationBillboardMeshFactory() : IVTerrainDecorationModelFactory(IVTerrainDecorationModel::Billboard)
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
	VOVERRIDE IVTerrainDecorationModel* CreateModelWithFilename(const char *szFilename, const IVTerrainDecorationModel::ModelProperties_t *pProperties) ;

  VOVERRIDE bool DisposePerSectorObject(VisTypedEngineObject_cl *pPerSectorObj);
};


/// \brief
///  Specific vegetation resource class for rendering .model instances
///
/// \see
///  VTerrainDecorationBillboardMeshFactory
public class VTerrainDecorationBillboardMesh : public IVTerrainDecorationModel
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
	VTerrainDecorationBillboardMesh(const char *szFilename, const ModelProperties_t *pProperties=NULL) :
    IVTerrainDecorationModel(szFilename, pProperties)
  {
    m_bFullMatrixTransformationType = false;
    m_pAtlas = NULL;
  }

  /// \brief
	///  Overridden reload function
	///
	/// \returns
	///  Success of the reload function
	///
	VOVERRIDE BOOL Reload() ;

  /// \brief
	///  Overridden unload function
	///
	/// \returns
	///  Success of the unload function
	///
  VOVERRIDE BOOL Unload() ;

  /// \brief
	///  Overridden function to update shader constants
	///
	VOVERRIDE void UpdateParameter();

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
	VOVERRIDE void OnEditorEvent(EditorEvent_e action, VTerrain *pTerrain, void *pDataA, void *pDataB) ;

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
	virtual void RenderBatch(VTerrainVisibilityCollectorComponent *pInfoComp, VTerrainDecorationInstance **pInstList, int iCount,
                           RenderMode_e eRenderMode = RENDER_MODE_OTW) HKV_OVERRIDE;

    /// \brief
	///  Return the decoration type
  VOVERRIDE ModelType_e GetDecorationType() const {return Billboard;}

#ifdef SUPPORTS_SNAPSHOT_CREATION
  /// \brief
	///  Overridden function to gather resource dependencies
	///
	/// \param snapshot
	///  snapshot info
	///
	VOVERRIDE void GetDependencies(VResourceSnapshot &snapshot) 
  {
  }
#endif

  /// \brief
	///  This type does not generate persistent instances
  VOVERRIDE bool GenerateInstances()  
  {
    return false;
  }
  VOVERRIDE float GetNativeFarClipDistance()  
  {
    return 0.f;
  }

  void RemoveSectorDecoration(VEditableTerrainSector *pSector);

  gcroot<TextureAtlas ^> m_pAtlas;
  float m_fBaseSizeX, m_fBaseSizeY;
  VTextureObjectPtr m_spTexture;

  static VTerrainDecorationBillboardMeshFactory g_BillboardMeshFactory; ///< one static instance that is globally registered
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
