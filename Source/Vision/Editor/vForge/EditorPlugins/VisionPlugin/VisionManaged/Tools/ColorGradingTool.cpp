/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Editor/vForge/EditorPlugins/VisionPlugin/VisionManaged/VisionManagedPCH.h>
#include <Vision/Editor/vForge/EditorPlugins/VisionPlugin/VisionManaged/Tools/ColorGradingTool.hpp>
#include <Vision/Runtime/Engine/System/Vision.hpp>
#using <mscorlib.dll>
using namespace System;
using namespace CSharpFramework;
using namespace ManagedFramework;

struct LUT_t
{
  LUT_t()
  {
    memset(mapping,0,sizeof(mapping)); // alpha==0!
    m_pDownSampled = nullptr;
  }
  ~LUT_t()
  {
    V_SAFE_DELETE_ARRAY(m_pDownSampled);
  }

  VColorRef* DownSample(int iSize, bool &bSuccess)
  {
    bSuccess = true;
    V_SAFE_DELETE_ARRAY(m_pDownSampled);
    m_pDownSampled = new VColorRef[iSize*iSize*iSize];
    VColorRef *pDst = m_pDownSampled;
    int iBlock = 256/iSize;
    for (int z=0;z<iSize;z++)
      for (int y=0;y<iSize;y++)
        for (int x=0;x<iSize;x++,pDst++)
        {
          // average block color:
          int r = 0, g = 0, b = 0;
          int iValid = 0;
          for (int zz=0;zz<iBlock;zz++)
            for (int yy=0;yy<iBlock;yy++)
              for (int xx=0;xx<iBlock;xx++)
              {
                VColorRef c = mapping[x*iBlock+xx][y*iBlock+yy][z*iBlock+zz];
                if (c.a==0) continue;
                iValid++;
                r += (int)c.r;
                g += (int)c.g;
                b += (int)c.b;
              }
          if (iValid>0)
            pDst->SetRGBA(r/iValid,g/iValid,b/iValid,255);
          else
            bSuccess = false;

        }
    return m_pDownSampled;
  }

  VColorRef mapping[256][256][256];
  VColorRef *m_pDownSampled;
};


String ^ ColorGradingTool::CreateGradingLUT(String ^origFile, String ^targetFile, String ^tragetLUTFile, int iEdgeSize)
{
  VString s1,s2,sDest,sLUTProjRel;
  ConversionUtils::StringToVString(origFile,s1);
  ConversionUtils::StringToVString(targetFile,s2);
  ConversionUtils::StringToVString(tragetLUTFile,sDest);
  if (s1.IsEmpty() || s2.IsEmpty() || sDest.IsEmpty())
    return "valid file names must be passed";

  if (EditorManager::Project != nullptr)
  {
    String ^tragetLUTFileRel = EditorManager::Project->MakeRelative(tragetLUTFile);
    ConversionUtils::StringToVString(tragetLUTFileRel,sLUTProjRel);
  }

  VisBitmap_cl *pBmp1 = VisBitmap_cl::LoadBitmapFromFile(s1);
  VisBitmap_cl *pBmp2 = VisBitmap_cl::LoadBitmapFromFile(s2);
  if (pBmp1==nullptr || pBmp1->IsMissing())
    return "cannot load original bitmap file";
  if (pBmp2==nullptr || pBmp2->IsMissing())
    return "cannot load target bitmap file";
  if (pBmp1->GetWidth()!=pBmp2->GetWidth() || pBmp1->GetHeight()!=pBmp2->GetHeight())
    return "Original and target image must have same resolution";


  // fill the table
  LUT_t *pLUT = new LUT_t();
  int iCount = pBmp1->GetWidth() * pBmp1->GetHeight();
  int iCoverage = 0;
  for (int i=0;i<iCount;i++)
  {
    VColorRef c1 = pBmp1->GetDataPtr()[i];
    VColorRef c2 = pBmp2->GetDataPtr()[i];
    c2.a = 255; // important to identify
    VColorRef &dst(pLUT->mapping[c1.r][c1.g][c1.b]);

    if (dst.a==0)
    {
      dst = c2;
      iCoverage++;
    } else
    {
      if (hkvMath::Abs ((int)dst.r-(int)c2.r)>1 || hkvMath::Abs ((int)dst.g-(int)c2.g)>1 || hkvMath::Abs ((int)dst.b-(int)c2.b)>1)
      {
        delete pLUT;
        return "Inconsistent image values: The same source color in the original image maps to different target colors";
      }
    }
  }

  float fPercentage = (float)iCoverage*100.f/(float)iCount; // just for debugging

  // upload and save 3D texture
  VTexture3DObject *pTex = Vision::TextureManager.Create3DTextureObject("<TempLUT>",iEdgeSize,iEdgeSize,iEdgeSize,1,VTextureLoader::B8G8R8A8);
  if (pTex==nullptr)
  {
    delete pLUT;
    return "Failed to create texture";
  }

  bool bSuccess = true;
  VColorRef* pFinal = pLUT->DownSample(iEdgeSize,bSuccess);
  if (!bSuccess)
  {
    delete pLUT;
    return "The lookup table is incomplete. Please add more color samples to the reference screenshot, e.g. merge the VColors.dds texture from base data folder into the screenshot.";
  }
  pTex->UpdateBox(0, 0,0,0, iEdgeSize,iEdgeSize,iEdgeSize, iEdgeSize*sizeof(VColorRef),iEdgeSize*iEdgeSize*sizeof(VColorRef),pFinal, V_TEXTURE_LOCKFLAG_DISCARDABLE);

#if defined(_VR_DX11) && defined(SUPPORTS_D3DX)
    bool bRes = D3DX11SaveTextureToFile(VVideo::GetD3DDeviceContext(), pTex->GetD3DResource(),D3DX11_IFF_DDS,sDest.AsChar())==S_OK;
#elif !defined(_VR_DX11)
    bool bRes = D3DXSaveTextureToFile(sDest.AsChar(),D3DXIFF_DDS,pTex->GetD3DInterface(),NULL)==S_OK;
#endif

  delete pLUT;

  pTex->Purge();

  // force reloading the 3D texture on the engine side, if overwritten:
  if (!sLUTProjRel.IsEmpty())
  {
    VTextureObject *pExisting = Vision::TextureManager.GetTextureObjectByName(sLUTProjRel);
    if (pExisting)
      pExisting->CheckFileModified(Vision::File.GetManager(),TRUE,FALSE);
  }
  return nullptr;
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
