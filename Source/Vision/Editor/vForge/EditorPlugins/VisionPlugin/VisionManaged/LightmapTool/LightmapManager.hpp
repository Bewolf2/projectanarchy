/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

using namespace CSharpFramework;
using namespace CSharpFramework::Math;

class LightmapPageTweakInfo
{
public:
  LightmapPageTweakInfo();
  ~LightmapPageTweakInfo();

  void AllocateData();
  void FinishPage();

  bool MixSingleChannel(int iChannel, UBYTE *pDestBuffer, bool bSwapRB);
  void MixAndUploadLightmaps(UBYTE *pDestBuffer);
  void Save(UBYTE *pDestBuffer);

  inline int GetPageSizeX(int iChannel) {if (!m_spLightmapChannel[iChannel]) return 0;return m_spLightmapChannel[iChannel]->GetTextureWidth();}
  inline int GetPageSizeY(int iChannel) {if (!m_spLightmapChannel[iChannel]) return 0;return m_spLightmapChannel[iChannel]->GetTextureHeight();}
public:
  VTextureObjectPtr m_spLightmapChannel[4];
  VColorExpRef *m_pBaseColor[4];
  VColorExpRef *m_pRadiosityColor[4];
  char *m_pRefTag;
  int m_iMaxSizeX, m_iMaxSizeY;
};


class LightGridTweakInfo
{
public:
  LightGridTweakInfo()  
  {
    m_pBaseColors=m_pRadiosityColors=m_pMixResult=NULL;
    m_iFlatColorCount=0;
    m_bHasBase=m_bHasRadiosity=false;
  }
  ~LightGridTweakInfo() {ReleaseColors();}

  void ReleaseColors() 
  {
    V_SAFE_DELETE_ARRAY(m_pBaseColors);
    V_SAFE_DELETE_ARRAY(m_pRadiosityColors);
    V_SAFE_DELETE_ARRAY(m_pMixResult);
    m_iFlatColorCount=0;
  }
  void FromLightGrid(VLightGrid_cl *pGrid);
  //void ToLightGrid(VLightGrid_cl *pGrid);
  void MixAndUpdate(VLightGrid_cl *pGrid);


  hkvVec3* m_pBaseColors;
  hkvVec3* m_pRadiosityColors;
  hkvVec3* m_pMixResult;
  bool m_bHasBase, m_bHasRadiosity;
  int m_iFlatColorCount;
};


class LightmapInfoLoader : public VChunkFile
{
public:
  VOVERRIDE BOOL OnStartChunk(CHUNKIDTYPE chunkID, int iChunkLen) override;
};



// This class manages the engine lightmaps
public ref class LightmapManager
{
public:
  static void OneTimeInit();
  static void OneTimeDeInit();

  static bool GetSceneLightmapInfo(bool bForceReload);
  static bool GetSceneLightGridInfo(bool bForceReload);
  static void UpdateLightmaps(bool bImmediateViewUpdate);
  static void UpdateLightGrid(bool bImmediateViewUpdate);
  static void ReleaseLightmapInfo();

  static void ApplyColorMatrices(Matrix4F baseMat, Matrix4F radioMat);

  static bool LoadLightmapInfo(const char *szV3DName);
  static bool LoadLightGridInfo(const char *szV3DName);

  static void SaveLightGrid();
  static void SaveAllLightmaps();
  static bool MapLoaded();
public:
  LightmapManager(void) {} // prevent creation
  ~LightmapManager(void) {}

  // scene info
  static bool bLightmapInfoValid = false;
  static bool bLightgridInfoValid = false;
  static int iPageCount = 0;
  static LightmapPageTweakInfo* pPageInfo = nullptr;
  static UBYTE *pUploadBuffer = nullptr; // 3byte RGB
  static float fColorMulFactor = 1.f;
  static VIS_CFG_LightingMode eLightEquation = VIS_LIGHTING_MODULATE;

  static LightGridTweakInfo *pLightGridInfo = nullptr;
//  static LightGridTweakInfo *pRadiosityLightGrid = nullptr;
//  static LightGridTweakInfo *pMixingResult = nullptr;

  // what is currently loaded in the v3d?
  static bool bLightmapBase       = false;
  static bool bLightmapRadiosity  = false;
  static bool bLightGridBase      = false;
  static bool bLightGridRadiosity = false;

  // mixer parameter
  static bool bNoBaseLight = false;
  static bool bNoRadiosity = false;
 
  static Matrix4F baseColorMat = Matrix4F::Identity;
  static Matrix4F radiosityColorMat = Matrix4F::Identity;
};


class LightmapSceneListener : public IVisCallbackHandler_cl
{
public:
  VOVERRIDE void OnHandleCallback(IVisCallbackDataObject_cl *pData) override;
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
