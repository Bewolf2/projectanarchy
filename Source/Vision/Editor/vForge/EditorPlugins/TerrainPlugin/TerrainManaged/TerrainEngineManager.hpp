/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#pragma once

using namespace CSharpFramework;
using namespace CSharpFramework::BaseTypes;
using namespace ManagedFramework;
using namespace Sharp3D::Math::Core;
using namespace CSharpFramework::Math;
using namespace System::Diagnostics;
using namespace CSharpFramework::Scene;
using namespace CSharpFramework::Helper;

#include ".\DataRectangle.hpp"
#include ".\heightvaluebrush.hpp"
#include ".\modifyablebitmap.hpp"
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Editing/ITerrainFilter.hpp>
#include ".\SimpleTerrainFilter.hpp"
#include ".\ColormapFilter.hpp"

namespace TerrainManaged
{
  class VForgeHeightfieldInfo;


  ////////////////////////////////////////////////////////////////////////////
  // TerrainEngineManager : This class is responsible for managing the 
  //  terrain painting on the engine side. All functions are static so they
  //  are easier accessible in the C# code
  ////////////////////////////////////////////////////////////////////////////
  public __gc class TerrainEngineManager
  {
  public:
    __value enum PaintMode
    {
      None,     // no terrain painting
      Elevate,  // elevate terrain mode
      EditColorMap, // editing the colormap
    };

    ////////////////////////////////////////////////////////////////////////////
    // Init/Deinit the manager once
    ////////////////////////////////////////////////////////////////////////////
    static void InitOneTime();
    static void DeInitOneTime();

    ////////////////////////////////////////////////////////////////////////////
    // Some engine callbacks
    ////////////////////////////////////////////////////////////////////////////
    static void NewWorldLoaded();
    static void UnloadWorld();
    static void OnRenderHook(int iOrder);

    static void SetActivate(bool bStatus);
    static void GenerateSceneInfo();

    ////////////////////////////////////////////////////////////////////////////
    // view related:
    ////////////////////////////////////////////////////////////////////////////
    static void SetPaintMode(PaintMode mode);
    static void UpdateTracePosition(int iMouseX, int iMouseY);
    static void SetUseDirectionalLight(bool bStatus);
    static void SetTesselationScheme(TerrainTesselationMode_e eMode);

    ////////////////////////////////////////////////////////////////////////////
    // saving
    ////////////////////////////////////////////////////////////////////////////
    static bool SaveAll();

    ////////////////////////////////////////////////////////////////////////////
    // editing height values
    ////////////////////////////////////////////////////////////////////////////
    static void SetHeightValueBrush(HeightValueBrush* pBrush);
    static int GetCurrentHeightmapIndex()
    {
      if (g_iPaintingOnIndex>=0)
        return g_iPaintingOnIndex; // while painting, return this one
      if (!g_pTraceInfo->detected) 
        return g_iHeightmapIndex;
      return g_iMainHeightmapIndex;
    }
    static VForgeHeightfieldInfo* GetCurrentHeightmap();

    static void BackupHeightValues(int iHMIndex);
    static bool ApplyHeightmapChanges(DataRectangle **pOldData, DataRectangle **pNewData);
    static void BlitHeightValues(HeightValueBrush::BlitMode mode); // while painting
    static void BlitHeightValues(int iHMIndex, DataRectangle *pSrcRect); // for do/undo
    static void SetElevationMode(HeightValueBrush::ElevationMode mode) {g_ElevationMode=mode;}
    static void SetLevel(int level) {g_Level=level;}

    ////////////////////////////////////////////////////////////////////////////
    // editing color map
    ////////////////////////////////////////////////////////////////////////////

    static void SetColormapBrush(String *Filename);
    static bool BackupColormap(int iHMIndex);
    static void BlitColorBrush(BlitMode mode); // while painting
    static void BlitColormap(DataRectangle *pSrcRect); // for do/undo
    static bool ApplyColormapChanges(DataRectangle **pOldData, DataRectangle **pNewData);
    static void SetRadius(float fRad) {g_fCursorRadius=fRad;}
    static void SetIntensity(float fIntensity) {g_fIntensity=fIntensity;}
    //static bool ClearColormap(TerrainEditColorTarget *target, DataRectangle **pOldData, DataRectangle **pNewData);


    ////////////////////////////////////////////////////////////////////////////
    // editing target
    ////////////////////////////////////////////////////////////////////////////
    
    static void SetColorEditingTarget(TerrainEditColorTarget *target) {g_pColorEditTarget = target;}

    // pass a null string to edit the surface's base texture
    static void SetColormapTargetFile(String *filename);
    static void SetColormapTarget(ModifyableBitmap *target) {g_pCurrentBitmap = target;}

    ////////////////////////////////////////////////////////////////////////////
    // assign terrain shader
    ////////////////////////////////////////////////////////////////////////////

    static void AssignShaderFX(int iHMIndex, ShaderEffectConfig *pShaderCfg);
    static void FillShaderFXParams(int iHMIndex, ShaderEffectConfig *pShaderCfg);
    static String* GetHeightfieldMaterialName(int iHMIndex);

    ////////////////////////////////////////////////////////////////////////////
    // run Filter
    ////////////////////////////////////////////////////////////////////////////
    static bool ExecuteHeightmapFilter(ITerrainFilter *filter, bool bWithDialog, DataRectangle **pOldData, DataRectangle **pNewData);
    static bool ExecuteColormapFilter(TerrainEditColorTarget *target, ITerrainFilter *filter, bool bWithDialog, DataRectangle **pOldData, DataRectangle **pNewData);

    ////////////////////////////////////////////////////////////////////////////
    // height Info
    ////////////////////////////////////////////////////////////////////////////
    static int GetHeight();
    static void GetCurrentCursorPos(hkvVec3 &vPos3D);

    ////////////////////////////////////////////////////////////////////////////
    // export height field generator shape to v3d
    ////////////////////////////////////////////////////////////////////////////
    static void BeginHeightFieldExport(String *v3dFilename);
    static void CancelHeightFieldExport();
    static void EndHeightFieldExport();
    static bool ExportHeightFieldShape(HeightFieldGeneratorShape *generator);


    ////////////////////////////////////////////////////////////////////////////
    // members:
    ////////////////////////////////////////////////////////////////////////////
    static bool g_bHeightInfo;
    static bool g_bActivated;
    static bool g_bHasV3DInfo;
    static PaintMode g_PaintMode;
    static VisTraceLineInfo_t *g_pTraceInfo;
    static int g_iMainHeightmapIndex;
    static int g_iHeightmapIndex;
    static int g_iHeightmapCount;
    static VForgeHeightfieldInfo *g_pHeightMap;
    
    static VCompiledTechnique *g_pCursorFX, *g_pDirectionalLight;
    
    static DataRectangle *g_pBackupHeightmap;
    static int g_iPaintingOnIndex;
    static int g_iModifiedX1,g_iModifiedY1,g_iModifiedX2,g_iModifiedY2;
    static bool g_bDirectionalLight;
    static HeightValueBrush* g_pCurrentHeightBrush;
    static float g_fCursorRadius; // for colormap
    static float g_fIntensity; // for colormaps and heightmaps
    static HeightValueBrush::ElevationMode g_ElevationMode; // for heightmaps
    static int g_Level; // for heightmaps
    static VisBitmap_cl *m_pColormapBrush;

    static TerrainEditColorTarget *g_pColorEditTarget;
    static ModifyableBitmap *g_pCurrentBitmap; // the bitmap that is currently painted on

    static bool g_bOverwriteHeightmap;
    static String *g_szExportErrorMsg;
  private:
    static void UpdateCursorShader(VCompiledShaderPass *shader, const hkvAlignedBBox &cursorBox, const hkvVec3 &vPlaneNrml, VTextureObject *pDisplayTexture, VColorRef iColor);
    static void RenderAllHeightfieldsWithShader(VCompiledShaderPass *pShader);
    static bool DoTrace(const hkvVec3 &vStart, const hkvVec3 &vEnd);
    static VWorld *g_pWorld;
    static String *g_szV3DFilename;
  };


  ////////////////////////////////////////////////////////////////////////////////////////////
  // VForgeHeightfieldInfo : An instance of this class keeps some per-heightfield 
  //  information. We need some more information than provided through VisHeightmapInfo_cl
  ////////////////////////////////////////////////////////////////////////////////////////////
  class VForgeHeightfieldInfo : public VisHeightmapInfo_cl
  {
  public:
    /*VForgeHeightfieldInfo()
    {
      m_pColorMap = NULL;
    }*/

    void GetInfoFromHeightfield(int iIndex);

    void ReloadColormap();
  public:
    VTextureObjectPtr m_spOldBaseTexture; // store the original base texture of the surface
    ModifyableBitmap *m_pColorMap; // a system memory version of the height field's colormap, loaded through ModifyableBitmapManager
  };


  ////////////////////////////////////////////////////////////////////////////////////////////
  // ModifyableBitmapManager : Manages all loaded bitmaps that have a system memory representation
  ////////////////////////////////////////////////////////////////////////////////////////////
  class ModifyableBitmapManager
  {
  public:
    static ModifyableBitmap* GetModifyableBitmap(String *filename);
    static void UnloadAll();
    static bool SaveAll();

  private:
    static DynArray_cl<ModifyableBitmap *>m_Bitmap;
  };
}

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
