/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file TerrainDecorationGroup.hpp

#ifndef VTERRAINDECORATIONGROUP_HPP_INCLUDED
#define VTERRAINDECORATIONGROUP_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/TerrainModule.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Geometry/TerrainDecorationModel.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Geometry/TerrainDecorationInstance.hpp>

#define DEFAULT_NUM_VISIBILITY_COLLECTORS   16

class VShadowMapGenerator;

///\brief
///Native representation of a decoration group shape. The group consists of an array of instances all of the same model
class VTerrainDecorationGroup : public VisVisibilityObjectAABox_cl
{
public:
  ///
  /// @name Constructor / Destructor
  /// @{
  ///

  ///\brief
	///Constructor
	TERRAIN_IMPEXP VTerrainDecorationGroup(bool bRegister=true);

  ///\brief
	///Destructor
  TERRAIN_IMPEXP virtual ~VTerrainDecorationGroup();

  ///
  /// @}
  ///

  ///
  /// @name Group Properties
  /// @{
  ///

  /// \brief
  ///   Gets the decoration model that is associated with this group
  inline IVTerrainDecorationModel* GetModel() const
  {
    return m_spModel;
  }

  /// \brief
  ///   Use this non-virtual override to set the far clip distance. This function actually updates both the per instance clipping and the group clipping. Pass <=0.0 for no clipping.
  TERRAIN_IMPEXP void SetFarClipDistance(float fFar);

  ///
  /// @}
  ///

  ///
  /// @name Important overrides
  /// @{
  ///

  ///\brief
  ///Overridden visibility object function to perform internal computation / tagging
  TERRAIN_IMPEXP virtual BOOL OnTestVisible(IVisVisibilityCollector_cl *pCollector, const VisFrustum_cl *pFrustum);

  ///\brief
  ///Disposes this group
  TERRAIN_IMPEXP virtual void DisposeObject();

  ///\brief
  ///Perform a traceline on all objects in this group
  TERRAIN_IMPEXP virtual int TraceTest(const hkvVec3 &startOfLine, const hkvVec3 &endOfLine, int iStoreResultCount, VisTraceLineInfo_t *pFirstTraceInfo)
  {
    return 0;
  }

  ///
  /// @}
  ///

  ///
  /// @name Rendering
  /// @{
  ///

  ///\brief
  ///Internal function to render a batch of instances associated with the passed collector
  TERRAIN_IMPEXP void RenderVisibleInstances(IVisVisibilityCollector_cl *pCollector);

  ///\brief
  ///Internal function to render a batch of instances associated with the passed collector into a shadow map (performs additional visibility/relevance tests)
  TERRAIN_IMPEXP void RenderVisibleInstancesToShadowMap(IVisVisibilityCollector_cl *pCollector, VShadowMapGenerator *pShadowMapGenerator, float *pfLightFrustumDistances);


  ///
  /// @}
  ///

  ///
  /// @name Lighting
  /// @{
  ///

  /// \brief
  ///   Used by the editor to update the cached lightgrid colors
  TERRAIN_IMPEXP void UpdateLightgridColors();

  /// \brief
  ///   Internal function
  TERRAIN_IMPEXP void TrackLightgridColors();

  /// \brief
  ///   Sets the absolute lightgrid sample position. Does not yet update the colors.
  inline void SetLightgridSamplePosition(const hkvVec3& vPos) {m_vLightgridSamplePosition=vPos;}

  /// \brief
  ///   Returns the absolute lightgrid sample position.
  inline const hkvVec3& GetLightgridSamplePosition() const {return m_vLightgridSamplePosition;}

  /// \brief
  ///   Defines whether lightgrid colors are used for lighting this group. Does not yet update the colors.
  inline void SetUseLightgrid(bool bStatus) 
  {
    m_bUseLightgrid = bStatus;
  }

  /// \brief
  ///   Returns the state previously set with SetUseLightgrid
  inline bool GetUseLightgrid() const 
  {
    return m_bUseLightgrid;
  }

  /// \brief
  ///   Sets the ambient color that is added to the light grid color. UpdateLightgridColors must be called after changing this value
  inline void SetAmbientColor(VColorRef iColor)
  {
    m_AmbientColor = iColor;
  }

  /// \brief
  ///   Returns the color that has been set via SetAmbientColor. Default is black
  inline VColorRef GetAmbientColor() const
  {
    return m_AmbientColor;
  }

  ///
  /// @}
  ///

  ///
  /// @name Editing
  /// @{
  ///

  ///\brief
  ///Initiates an update of instances in this group. Allocation/instance update can be performed on the returned object 
  TERRAIN_IMPEXP virtual VTerrainDecorationInstanceCollection& BeginUpdateInstances();

  ///\brief
  ///Finishes the update (closes the update block initiated with BeginUpdateInstances)
  TERRAIN_IMPEXP virtual void EndUpdateInstances();

  ///\brief
  ///Removes all instances. Internally uses BeginUpdateInstances/EndUpdateInstances
  TERRAIN_IMPEXP void ClearInstances();

  ///
  /// @}
  ///

  ///
  /// @name Serialization
  /// @{
  ///

  V_DECLARE_SERIAL_DLLEXP( VTerrainDecorationGroup,  TERRAIN_IMPEXP );

  /// \brief
  ///   De-serializes this group
  TERRAIN_IMPEXP virtual void Serialize( VArchive &ar );
  /// \brief
  ///   Gets resource dependencies for this group
  TERRAIN_IMPEXP virtual void GetDependencies(VResourceSnapshot &snapshot);

  // internal function
  inline void BackupPosition()
  {
    m_vPositionBackup = GetPosition();
  }

  ///
  /// @}
  ///

  
  IVTerrainDecorationModelPtr m_spModel; ///< Pointer to model resource
  VTerrainDecorationInstanceCollection m_Instances; ///< List of instances in this mesh (all of same model)
  
  hkvVec3 m_LightGridColors[6]; ///< 6 lightgrid colors-cached
  hkvVec3 m_vLightgridSamplePosition; ///< absolute sample position
  VColorRef m_AmbientColor;
  float m_fMax3DDistance, m_fUnscaledMaxDist;
  VTerrain *m_pDecoOfTerrain;
  bool m_bUseCollisions, m_bCastLightmapShadows;
protected:
  friend class VTerrainDecorationGroupManager;

  ////////////////////////////////////////////////////////////
  // Internal visibility related functions
  ////////////////////////////////////////////////////////////
#ifndef _VISION_DOC
  struct VCollectorVisibleState
  {
    inline bool IsAnyInstanceVisible() const 
    { 
      return m_iLastRelevantBit >= m_iFirstRelevantBit; 
    }

    IVisVisibilityCollector_cl *m_pCollector;
    bool m_bVerticalBBVisible;

    // first 8 bits of frame number of last time the visibility was checked 
    // (as returned by VisRenderContext_cl::GetGlobalTickCount)
    unsigned char m_iLastFrameChecked; 

    int m_iFirstRelevantBit, m_iLastRelevantBit;
    unsigned int m_iFirstMask;
  };
#endif

  inline void FreeAllVisStates()
  {
    VCollectorVisibleState** ppVisStates = static_cast<VCollectorVisibleState**>(m_VisStates.GetBuffer());
    for (int i=0;i<m_iVisStateCount;i++)
      V_SAFE_FREE(ppVisStates[i]);
    m_iVisStateCount = 0;
  }

  inline void FreeCollectorVisStates(IVisVisibilityCollector_cl *pCollector)
  {
    VCollectorVisibleState** ppVisStates = static_cast<VCollectorVisibleState**>(m_VisStates.GetBuffer());
    int iCount = m_iVisStateCount;
    m_iVisStateCount = 0;
    for (int i=0;i<iCount;i++)
    {
      if (ppVisStates[i]->m_pCollector==pCollector)
      {
        V_SAFE_FREE(ppVisStates[i]);
        continue;
      }
      ppVisStates[m_iVisStateCount++] = ppVisStates[i];
    }
    for (int i=m_iVisStateCount;i<iCount;i++)
      ppVisStates[i] = NULL;
  }


  static inline void SetGlobalRenderOffset(const hkvVec3& vOfs)
  {
    g_vGlobalRenderOffset = vOfs; // for repositioning
  }
   

  VCollectorVisibleState* FindVisStateForCollector(IVisVisibilityCollector_cl* pColl);
  VCollectorVisibleState* CreateVisStateForCollector(IVisVisibilityCollector_cl* pColl);
  void CalcMax3DDistance();

  VMutex m_VisibilityMutex;
  int m_iVisStateCount;
  VMemoryTempBuffer<DEFAULT_NUM_VISIBILITY_COLLECTORS * sizeof(VCollectorVisibleState*)> m_VisStates;
  bool m_bUseLightgrid, m_bIsRegistered;
  hkvVec3 m_vPositionBackup; // position at export time (for repositioning)
  static hkvVec3 g_vGlobalRenderOffset; // for repositioning
  static DynArray_cl<VTerrainDecorationInstance *>g_VisibleInstances; ///< only needed while rendering
};



///\brief
///Refcounted collection of decoration groups
class VTerrainDecorationGroupCollection : public VRefCountedCollection<VTerrainDecorationGroup>
{
public:
  ///\brief
  ///  Helper function to dispose all objects in this collection
  inline void DisposeObjects()
  {
    for (int i=Count()-1;i>=0;i--)
    {
      VTerrainDecorationGroup *pGroup = GetAt(i);
      V_SAFE_DISPOSEOBJECT(pGroup);
    }
    Clear();
  }

};


///\brief
///Manager instance that handles all visible instances of VTerrainDecorationGroup
class VTerrainDecorationGroupManager : public VRefCountedCollection<VTerrainDecorationGroup>, public IVisCallbackHandler_cl
{
public:

  VTerrainDecorationGroupManager() : 
      m_VisibleGroups(0,NULL)
  {
    m_iVisibleGroupCount = 0;
  }

  virtual void OnHandleCallback(IVisCallbackDataObject_cl *pData);

  void AddGroup(VTerrainDecorationGroup *pGroup);
  void RemoveGroup(VTerrainDecorationGroup *pGroup);
  void RemoveAllGroups();
  void RegisterCallbacks();
  void UnRegisterCallbacks();

  TERRAIN_IMPEXP static VTerrainDecorationGroupManager& GlobalManager();

  static VTerrainDecorationGroupManager g_GlobalManager;

  int m_iVisibleGroupCount;
  DynArray_cl<VTerrainDecorationGroup *>m_VisibleGroups; ///< Temp array
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
