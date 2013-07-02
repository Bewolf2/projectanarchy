/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#pragma once
using namespace System;
using namespace TerrainBase::Config;
using namespace TerrainBase::Editing;


class VTerrainConfig;

namespace TerrainManaged
{
  public ref class IHeightmapImporterFromFile abstract : public IHeightmapImporter
  {
  public:
    IHeightmapImporterFromFile();

    virtual bool OnStart(IValueProvider ^originalValues, int x1, int y1, int x2, int y2) override;
    virtual void OnFinish() override;

    VOVERRIDE float GetHeightAtInternal(int x, int y) override;

    VOVERRIDE property int      SizeX     {  int     get() override { return m_iSizeX;               } }
    VOVERRIDE property int      SizeY     {  int     get() override { return m_iSizeY;               } }
    VOVERRIDE property String ^ LastError {  String ^get() override { return m_pLastError;           } }
    VOVERRIDE property bool     Valid     {  bool    get() override { return m_pHeightData!=NULL;    } }

  protected:
    int m_iSizeX,m_iSizeY;
    String ^m_pLastError;
    float *m_pHeightData;
  };


  // still abstract, but can load .tex textures
  public ref class IHeightmapImporterFromTextureFile : public IHeightmapImporterFromFile
  {
  public:
    void LoadFromFile(String ^value);

    VOVERRIDE property String ^ Name      {  String ^get() override { return "Import from Texture"; } }
  };


  public ref class IHeightmapImporterFromRawFile : public IHeightmapImporterFromFile
  {
  public:
    IHeightmapImporterFromRawFile()
    {
      m_iComponentSize = 16; // 16bpp
      m_iHeaderSize = m_iOutroSize = 0;
      m_iRowStride = -1;
    }

      void LoadFromFile(String ^value);

      VOVERRIDE property String ^ Name      {  String ^get() override { return "Import from raw file"; } }

  protected:
    int m_iComponentSize;
    int m_iHeaderSize, m_iOutroSize, m_iRowStride;
  };


  public ref class IHeightmapImporterFromDDSFile : public IHeightmapImporterFromFile
  {
  public:
    void LoadFromFile(String ^value);

    VOVERRIDE property String ^ Name      {  String ^get() override { return "Import from Texture";  } }
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
