/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file TerrainDecorationModel.hpp

#ifndef TERRAINDECORATIONMODEL_HPP_INCLUDED
#define TERRAINDECORATIONMODEL_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/TerrainModule.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Application/TerrainConfig.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/BillboardGroup.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Math/LargePosition.hpp>

class IVTerrainDecorationModel;
class VTerrainDecorationInstance;
class VTerrain;
class VTerrainVisibilityCollectorComponent;

typedef VSmartPtr<IVTerrainDecorationModel> IVTerrainDecorationModelPtr;

/// \brief
///   Data object class for decoration model resources. Used by the editor
class VTerrainDecorationModelDataObject : public IVisCallbackDataObject_cl
{
public:
  VTerrainDecorationModelDataObject(VCallback *pSender,IVTerrainDecorationModel *pModel) : IVisCallbackDataObject_cl(pSender)
  {
    m_pModel = pModel; 
  }
  IVTerrainDecorationModel *m_pModel;
};

/// \brief
///   Helper data structure to provide physics primitive info for decoration objects (capsule)
class VDecorationCollisionPrimitive
{
public:
  ///\brief
	///Create a capsule from 2 positions and a radius
	///
	///\param vStart
  ///Center-position of start
	///
	///\param vEnd
  ///Center-position of end
	///
	///\param fRadius
	///Radius of the capsule
	///
	inline void Set(const hkvVec3& vStart, const hkvVec3& vEnd, float fRadius)
  {
    m_vStartPosition = vStart;
    m_vDirection = vEnd-vStart;
    m_fCapsuleRadius = fRadius;
    
    m_fDirectionLength = m_vDirection.getLength();
    m_vDirection.normalizeIfNotZero ();
  }

  ///\brief
  ///Returns the radius
  inline float GetRadius() const {return m_fCapsuleRadius;}

  /// \brief
  ///   Gets the start position of the capsule
  inline hkvVec3 GetStart() const { return m_vStartPosition; }

  /// \brief
  ///   Gets the direction vector of the capsule
  inline hkvVec3 GetDir() const { return m_vDirection; }

  /// \brief
  ///   Gets the end position of the vector of the capsule
  inline hkvVec3 GetEnd() const { return m_vStartPosition + m_vDirection * m_fDirectionLength; }

  ///\brief
  ///Returns the length of the capsule (distance between start and end position)
  inline float GetLength() const {return m_fDirectionLength;}

  ///\brief
  ///Evaluates the bounding box of the capsule
  inline void GetBoundingBox(hkvAlignedBBox &dest)
  {
    dest.m_vMin = m_vStartPosition;
    dest.m_vMax = m_vStartPosition;
    dest.expandToInclude(m_vStartPosition+m_vDirection*GetLength());
    dest.addBoundary (hkvVec3 (GetRadius()));
  }
  
  ///\brief
  ///Helper trace function
  TERRAIN_IMPEXP bool TraceDistance(const hkvVec3& startOfLine, const hkvVec3& vRayDir, float &fDist) const;

  ///\brief
  ///Render the capsule at specified position, orientation and scaling
  TERRAIN_IMPEXP void DebugRender(IVRenderInterface* pRI, const hkvMat4 &transform, float fScaling);
protected:
  hkvVec3 m_vStartPosition;
  float m_fCapsuleRadius;
  hkvVec3 m_vDirection;
  float m_fDirectionLength;

};

#if defined(_VR_DX11)
/// \brief
///   DX10 specific shader registers
struct VTerrainConstantBuffer_PerModel
{
  hkvVec4  VegetationParams; ///< x:fade start y:1/fade interval z=constraint w=wind
};

typedef VTypedConstantBuffer<VTerrainConstantBuffer_PerModel> VTerrainConstantBufferPerModel;

#endif


/// \brief
///   Decoration resource class, typically has a VDynamicMesh reference
class IVTerrainDecorationModel : public VManagedResource
{
public:
  /// \brief
  ///   Bitflags for the model resource
  enum ModelFlags_e
  {
    None            = 0x00000000, ///< no flags
    Smooth          = 0x00000000, ///< terrain alignment mode is: aligned
    AlwaysVertical  = 0x00000001, ///< terrain alignment mode is: always vertical
    SmoothSheared   = 0x00000002, ///< terrain alignment mode is: sheared aligned
    CastLightmapShadows = 0x00000100, ///< flag that determines whether this geometry type supports casting lightmap shadows
    CastDynamicShadows  = 0x00000200, ///< flag that determines whether this geometry type is rendered into the depth buffer
    AlignmentModes = Smooth|AlwaysVertical|SmoothSheared, ///< bit mask
  };

  /// \brief
  ///   Decoration Type
  enum ModelType_e
  {
    AnyType = -1,
    Unknown = 0,
    Model,
    Speedtree5,
    Billboard,
    Speedtree6,
    FirstUserModelType = 20
  };

  /// \brief
  ///   Event enum that is sent by vForge to the model's OnEditorEvent function
  enum EditorEvent_e
  {
    GatherLightmapBegin,    ///< lightmap calculation has begun
    GatherLightmapFinished, ///< lightmap calculation has finished

    SectorDecorationUpdateBegin,    ///< decoration of a sector has begun. Called for all old models in the sector. Sector pointer is in pDataA
    SectorDecorationUpdateFinished, ///< decoration of a sector has been updated. Called for all new models in the sector. Sector pointer is in pDataA
    SectorDecorationDeleted,        ///< decoration of a sector has been deleted. Sector pointer is in pDataA

    SurfaceShaderChanged,           ///< A material shader has been changed in the material layout. pDataA can be casted to VisSurface_cl*
    RendererNodeChanged,            ///< The global renderer node has changed
  };

  /// \brief
  ///   Enum that is used for the RenderBatch functions to differ between OTW and IR rendering
  enum RenderMode_e
  {
    RENDER_MODE_OTW = 0,
    RENDER_MODE_IR_PREPASS,
    RENDER_MODE_IR_MAINPASS
  };

  /// \brief
  ///   Contains serializable data only
  struct ModelProperties_t
  {
    TERRAIN_IMPEXP ModelProperties_t();
    TERRAIN_IMPEXP void SerializeX(VArchive &ar, int iVersion);
    TERRAIN_IMPEXP void ChunkFileExchange(VChunkFile &file, int iVersion);
    TERRAIN_IMPEXP bool operator == (const ModelProperties_t &other) const;
    TERRAIN_IMPEXP bool operator != (const ModelProperties_t &other) const;
    inline void operator = (const ModelProperties_t &other)
    {
      memcpy(this,&other,sizeof(ModelProperties_t));
    }

    // required properties for runtime:
    int m_iModelID;
    float m_fNearClip, m_fFarClip, m_fRelFadeStart;

    // properties only relevant inside the editor:
    // these parameters aren't saved, instead vForge saves them
    //int m_iDensityMapSectorRes;
    ModelFlags_e m_eModelFlags;
    float m_fRelHeight; ///< usually m_LocalBBox.m_vMin.z. Generated from PivotHeight
    float m_fAverageScale, m_fScaleVariation;
    float m_fRelativeDensity, m_fRandomness;
    float m_fApplyWind, m_fApplyConstraint; // runtime relevant
    float m_fCollisionCapsuleRadius; ///> if >0 it defines a capsule radius for decoration collisions
    unsigned int m_iVisibleBitmask;
  };

  TERRAIN_IMPEXP IVTerrainDecorationModel(const char *szFilename, const IVTerrainDecorationModel::ModelProperties_t *pProperties=NULL);
  TERRAIN_IMPEXP virtual IVSerializationProxy *CreateProxy();

  inline int GetID() const {return m_Properties.m_iModelID;}
  inline bool IsValid() const {return m_bValidState;}
  TERRAIN_IMPEXP void GetDensitySampleIndicesAtPos(const VTerrainConfig &cfg, const VLargePosition &vPos, int &x, int &y) const;

#ifdef WIN32
  TERRAIN_IMPEXP virtual int GetAdditionalOutputString(char *szDestBuffer, int iMaxChars) 
  {
    return sprintf(szDestBuffer,"ID:%i%s",m_Properties.m_iModelID, m_bIgnore ? " (ignored)":"");
  }
#endif

  ///\brief
	///Overridable that is called to update model properties
	TERRAIN_IMPEXP virtual void UpdateParameter();
  ///\brief
  ///Overridable that is called to collect lightmap information for the passed instance (if lightmapping is enabled)
  TERRAIN_IMPEXP virtual void GatherLightmapInfo(VLightmapSceneInfo &info, VTerrainDecorationInstance **pInst, int iCount);
  ///\brief
  ///Static helper function
  TERRAIN_IMPEXP static void GatherLightmapInfo(VBaseMesh *pMesh, VLightmapSceneInfo &info, VTerrainDecorationInstance **pInst, int iCount);

  ///\brief
  ///Overridable that is called by the editor.
  TERRAIN_IMPEXP virtual void OnEditorEvent(EditorEvent_e action, VTerrain *pTerrain, void *pDataA, void *pDataB) {}

  ///\brief
	///Renders a batch of vegetation objects
	///
	///\param pInfoComp
	///The visibility component
	///
	///\param pInstList
	///The list of instances
	///
	///\param iCount
	///The number of instances in the pInstList array
  ///
  ///\param eRenderMode
  ///The render mode to use
	///
	///This is the key function of this class as it is responsible for actually rendering a batch of instances
  TERRAIN_IMPEXP virtual void RenderBatch(VTerrainVisibilityCollectorComponent *pInfoComp, VTerrainDecorationInstance **pInstList, int iCount,
                                          RenderMode_e eRenderMode = RENDER_MODE_OTW) = 0;

  ///\brief
	///This function is used internally to determine whether this decoration type should be rendered into a specific context
  inline bool SupportsContextType(VisContextUsageHint_e eType) const
  {
    return ((1<<eType)&m_iSupportedContextTypes)!=0;
  }

  ///\brief
	///  Overridable that returns the number of collision primitives that can be obtained via GetCollisionSubObject
  TERRAIN_IMPEXP virtual int GetCollisionSubObjectCount() {return 0;}

  ///\brief
  ///  Overridable that returns a valid pointer to a collision object (iIndex must be in valid range [0..GetCollisionSubObjectCount()-1])
  TERRAIN_IMPEXP virtual VDecorationCollisionPrimitive* GetCollisionSubObject(int iIndex) {return NULL;}

  ///\brief
  ///  If the model definition itself has a far clip distance, return it here, otherwise return -1
  TERRAIN_IMPEXP virtual float GetNativeFarClipDistance() {return -1.f;}

  ///\brief
  ///  Indicates whether this model type generates real instances. (For instance, billboards do not)
  TERRAIN_IMPEXP virtual bool GenerateInstances() {return true;}

  ///\brief
  ///  Returns the number of LOD levels in the model chain. 1 for no LOD
  inline int GetLODCount() const
  {
    VASSERT(m_pNextLOD!=this);
    if (m_pNextLOD!=NULL)
      return m_pNextLOD->GetLODCount()+1;
    return 1;
  }

  ///\brief
  ///  Returns an enum value that represents the decoration type of this model
  virtual ModelType_e GetDecorationType() const = 0;

  bool m_bValidState;
  bool m_bNeedsLightmap;
  bool m_bFullMatrixTransformationType; ///< determines the way the per-instance transformation data is interpreted
  bool m_bTempFlag;
  bool m_bIgnore;
  int m_iLightmapSampler;
  ModelProperties_t m_Properties;
  int m_iSupportedContextTypes; // bitmask where every bit stands for an enum value from VisContextUsageHint_e. See IsVisibleInContextType

  // properties:
  hkvAlignedBBox m_LocalBBox;

#if defined(_VR_DX11)
    VTerrainConstantBufferPerModel m_PerModelConstantBuffer;
#else
  hkvVec4 m_vDecorationParams; ///< vertex shader reg 48
#endif

  IVTerrainDecorationModel *m_pNextLOD; ///< Only relevant/set inside the editor: Poiner to the next LOD model in the chain
  VSmartPtr<VBaseMesh> m_spCustomLightmapMesh; ///< Optional mesh. If defines, this one is used as lightmap casters
};




/// \brief
///   Factory class for specific decoration model types. The overridable CreateModelWithFilename
///   function must return NULL if the file extension cannot be handled by this type
class IVTerrainDecorationModelFactory
{
public:
  IVTerrainDecorationModelFactory(IVTerrainDecorationModel::ModelType_e eType) : m_eType(eType)
  {
  }

  virtual ~IVTerrainDecorationModelFactory(){}

  /// \brief
	///  Custom decoration type factories must implement this and return a model instance for the passed filename.
	///
	/// If the factory class does not recognize a supported file extension it should return NULL.
	///
	/// \param szFilename
	///  Filename of the decoration model file
	///
	/// \param pProperties
	///  Additional decoration model properties
	///
	/// \returns
	///  A new model instance or NULL if this type does not support the file extension.
	///
	TERRAIN_IMPEXP virtual IVTerrainDecorationModel* CreateModelWithFilename(const char *szFilename, const IVTerrainDecorationModel::ModelProperties_t *pProperties) = 0;

  TERRAIN_IMPEXP virtual bool DisposePerSectorObject(VisTypedEngineObject_cl *pPerSectorObj)
  {
    return false;
  }

  IVTerrainDecorationModel::ModelType_e m_eType;
};

#ifndef _VISION_DOC

#define DECORATION_INSTANCE_FIRST_STREAM  5
#define DECORATION_MAX_INSTANCE_COUNT     256

struct VModelInstanceData_t
{
  hkvVec4 row0, row1, row2; // 4x3 matrix
  DWORD iColorVal;

  static inline void GetDesc(VisMBVertexDescriptor_t &desc)
  {
    desc.m_iStride = sizeof(VModelInstanceData_t);
    desc.m_iSecondaryColorOfs = offsetof(VModelInstanceData_t,iColorVal);
    desc.m_iTexCoordOfs[DECORATION_INSTANCE_FIRST_STREAM+0] = offsetof(VModelInstanceData_t,row0) | VERTEXDESC_FORMAT_FLOAT4;
    desc.m_iTexCoordOfs[DECORATION_INSTANCE_FIRST_STREAM+1] = offsetof(VModelInstanceData_t,row1) | VERTEXDESC_FORMAT_FLOAT4;
    desc.m_iTexCoordOfs[DECORATION_INSTANCE_FIRST_STREAM+2] = offsetof(VModelInstanceData_t,row2) | VERTEXDESC_FORMAT_FLOAT4;
  }

  inline void Set(const hkvMat3& m, const hkvVec3& vPos, VColorRef iColor=V_RGBA_WHITE)
  {
    row0 = m.getRow (0).getAsVec4 (vPos.x);
    row1 = m.getRow (1).getAsVec4 (vPos.y);
    row2 = m.getRow (2).getAsVec4 (vPos.z);
    iColorVal = iColor.GetNative();
  }

  inline void Set(const VTerrainDecorationInstance &srcinst);
};

#endif // _VISION_DOC

/// \brief
///   Resource manager class for resources of type IVTerrainDecorationModel. There is one global
///   instance of this manager (VTerrainDecorationModelManager::GlobalManager())
class VTerrainDecorationModelManager : public VisResourceManager_cl, public IVisCallbackHandler_cl
{
public:
  VTerrainDecorationModelManager() : VisResourceManager_cl("Decoration Models",VRESOURCEMANAGERFLAG_SHOW_IN_VIEWER, 0)
  {
    m_iModelFactoryCount = 0;
    m_iInstancingBatchCount = DECORATION_MAX_INSTANCE_COUNT;
  }

  ///\brief
	///Creates a new model resource or returns an existing with matching filename and properties
  TERRAIN_IMPEXP IVTerrainDecorationModel *CreateModel(const char *szFilename, const IVTerrainDecorationModel::ModelProperties_t &props, bool bApplyProperties, IVTerrainDecorationModel::ModelType_e eForceType);
  ///\brief
  ///Looks up an existing model by its ID and returns it (or NULL)
  TERRAIN_IMPEXP IVTerrainDecorationModel *FindModelByID(int iID) const;

  ///\brief
  ///Accesses one global instance of the terrain decoration model manager
  TERRAIN_IMPEXP static VTerrainDecorationModelManager &GlobalManager();

  ///\brief
  ///Registers a factory for custom decoration types. Used by the speedtree implementation.
  TERRAIN_IMPEXP bool RegisterFactory(IVTerrainDecorationModelFactory *pFactory, bool bPrioritize=false);
  ///\brief
  ///Unregisters a factory
  TERRAIN_IMPEXP bool UnregisterFactory(IVTerrainDecorationModelFactory *pFactory);
  
  inline void FlagValidState(bool bState)
  {
    for (int i=0;i<GetResourceCount();i++)
    {
      IVTerrainDecorationModel *pModel = (IVTerrainDecorationModel *)GetResourceByIndex(i);
      if (pModel)
        pModel->m_bValidState = bState;
    }
  }
  inline void SetTempFlag(bool bState)
  {
    for (int i=0;i<GetResourceCount();i++)
    {
      IVTerrainDecorationModel *pModel = (IVTerrainDecorationModel *)GetResourceByIndex(i);
      if (pModel)
        pModel->m_bTempFlag = bState;
    }
  }

  inline void EditorEventOnTempFlagStatus(bool bStatus, IVTerrainDecorationModel::EditorEvent_e action, VTerrain *pTerrain, void *pDataA, void *pDataB)
  {
    for (int i=0;i<GetResourceCount();i++)
    {
      IVTerrainDecorationModel *pModel = (IVTerrainDecorationModel *)GetResourceByIndex(i);
      if (pModel != NULL && pModel->m_bTempFlag==bStatus)
        pModel->OnEditorEvent(action,pTerrain,pDataA,pDataB);
    }
  }

  inline void TriggerOnEditorEvent(IVTerrainDecorationModel::EditorEvent_e action, VTerrain *pTerrain, void *pDataA, void *pDataB)
  {
    const int iCount = GetResourceCount();
    for (int i=0;i<iCount;i++)
    {
      IVTerrainDecorationModel *pRes = (IVTerrainDecorationModel *)GetResourceByIndex(i);
      if (pRes!=NULL)
        pRes->OnEditorEvent(action,pTerrain,pDataA,pDataB);
    }
  }

  inline bool DisposePerSectorObject(VisTypedEngineObject_cl *pPerSectorObj)
  {
    for (int i=0;i<m_iModelFactoryCount;i++)
      if (m_pModelFactory[i]!=NULL && m_pModelFactory[i]->DisposePerSectorObject(pPerSectorObj))
        return true;
    return false;
  }

  static VTerrainDecorationModelManager g_GlobalManager;

  VisCallback_cl OnGatherModelProperties; ///< used by the editor

  int m_iModelFactoryCount;
  IVTerrainDecorationModelFactory* m_pModelFactory[16];

  // instancing:
  VisMeshBuffer_cl *GetInstanceBuffer(int &iInstanceCount, int &iStreamMask);
  inline void ReleaseInstanceBuffer()
  {
    m_spInstanceBuffer = NULL;
  } 

  ///\brief
  ///Sets the number of decoration instances that are rendered by an instancing batch. Modify this value only for debugging purposes. It can be set to 0 to disable instancing.
  inline void SetInstancingBatchCount(int iCount)
  {
    m_iInstancingBatchCount = hkvMath::Min(DECORATION_MAX_INSTANCE_COUNT, iCount);
  }

  ///\brief
  ///Returns the number of decoration instances that are rendered by an instancing batch. This value is always in valid range [0..DECORATION_MAX_INSTANCE_COUNT]
  inline int GetInstancingBatchCount() const
  {
    return m_iInstancingBatchCount;
  }

  /// \brief
  ///   Returns the IR shader library (in case the view context supports IR rendering - only possible when Simulation package is installed)
  inline VShaderEffectLib* GetInfraredShaderLib()
  {
    if (m_spInfraredLib == NULL)
    {
      if (Vision::File.Exists("Shaders\\vInfraredMaterial.Shaderlib"))
        m_spInfraredLib = Vision::Shaders.LoadShaderLibrary("Shaders\\vInfraredMaterial.Shaderlib", SHADERLIBFLAG_HIDDEN);
      else
        Vision::Error.Warning("Unable to open IR Shaderlib for terrain decoration model rendering");
    }
    return m_spInfraredLib;
  }

  TERRAIN_IMPEXP virtual void OnHandleCallback(IVisCallbackDataObject_cl *pData);
  TERRAIN_IMPEXP void OneTimeInit();
  TERRAIN_IMPEXP void OneTimeDeInit();

  int m_iInstancingBatchCount;
  int m_iInstanceStreamMask;
  VisMeshBufferPtr m_spInstanceBuffer;
  VShaderEffectLibPtr m_spInfraredLib;
};



/// \brief
///  Internal helper class
class VBillboardDecorationSectorInstance : public VBillboardGroupInstance
{
public:
  TERRAIN_IMPEXP VBillboardDecorationSectorInstance(IVTerrainDecorationModel *pModel=NULL, int iBillboardCount=0, VTerrainSector *pOwner=NULL);

  //serialization
  V_DECLARE_SERIAL_DLLEXP( VBillboardDecorationSectorInstance,  TERRAIN_IMPEXP )
  TERRAIN_IMPEXP virtual void Serialize( VArchive &ar );
  TERRAIN_IMPEXP virtual void DisposeObject();

public:
  static VRefCountedCollection<VBillboardDecorationSectorInstance> g_Instances;
  int m_iModelID;
  VTerrainSector *m_pSector;
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
