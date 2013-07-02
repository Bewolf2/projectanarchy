/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

using System;
using System.Collections.Generic;
using System.Text;
using TerrainBase.Editing;
using TerrainManaged;
using System.ComponentModel;
using System.Drawing.Design;
using CSharpFramework.PropertyEditors;
using CSharpFramework;
using CSharpFramework.Helper;

namespace TerrainEditorPlugin.Filter
{
  #region ImportHeightmapTEX16bpp

  public class ImportHeightmapTEX16bpp : IHeightmapImporterFromTextureFile
  {
    public override string Name
    {
      get
      {
        return "Import from 16bpp TEX file";
      }
    }

    public override string[] SupportedFileExtensions
    {
      get
      {
        return new string[] { ".TEX" };
      }
    }

    private void LoadFromCurrentFile()
    {
      // always load with absolute filename
      if (string.IsNullOrEmpty(FileName) || FileHelper.IsAbsolute(FileName))
        LoadFromFile(FileName);
      else
        LoadFromFile(EditorManager.Project.MakeAbsolute(FileName));
      ShowLastError();
    }

    [EditorAttribute(typeof(FilenameEditor), typeof(UITypeEditor)), FileDialogFilter(new string[] { ".tex" })]
    public override string FileName
    {
      get
      {
        return base.FileName;
      }
      set
      {
        base.FileName = value;
        LoadFromCurrentFile();
      }
    }

    public override bool Active
    {
      get
      {
        return base.Active;
      }
      set
      {
        base.Active = value;
        if (Active)
          LoadFromCurrentFile();
      }
    }

    [Description("Defines the vertical scaling factor to map 16 bit source values (0..65535) to world units")]
    public override float HeightScaling
    {
      get
      {
        return base.HeightScaling;
      }
      set
      {
        base.HeightScaling = value;
      }
    }

  }

  #endregion

  #region ImportHeightmapRAW

  public class ImportHeightmapRAW : IHeightmapImporterFromRawFile
  {
    public enum ComponentType_e
    {
      Unsigned8Bit  = 8,
      Unsigned16Bit  = 16,
      Unsigned32Bit  = 32,
    }

    public override string Name
    {
      get
      {
        return "Import from Raw file";
      }
    }

    public override string[] SupportedFileExtensions
    {
      get
      {
        return new string[] { ".raw" };
      }
    }

    private void LoadFromCurrentFile()
    {
      // always load with absolute filename
      if (string.IsNullOrEmpty(FileName) || FileHelper.IsAbsolute(FileName))
        LoadFromFile(FileName);
      else
        LoadFromFile(EditorManager.Project.MakeAbsolute(FileName));
      ShowLastError();
    }

    [EditorAttribute(typeof(FilenameEditor), typeof(UITypeEditor)), FileDialogFilter(new string[] { ".raw" })]
    public override string FileName
    {
      get
      {
        return base.FileName;
      }
      set
      {
        base.FileName = value;
        LoadFromCurrentFile();
      }
    }

    [SortedCategory("File", 1), PropertyOrder(20)]
    [Description("Component type in the raw file")]
    public ComponentType_e RawComponents
    {
      get
      {
        return (ComponentType_e)m_iComponentSize;
      }
      set
      {
        if (m_iComponentSize == (int)value)
          return;
        m_iComponentSize = (int)value;
        LoadFromCurrentFile();
      }
    }

    [SortedCategory("File", 1), PropertyOrder(21)]
    [Description("Size of the header in bytes until raw heightmap data")]
    public int HeaderSize
    {
      get { return m_iHeaderSize; }
      set 
      {
        if (m_iHeaderSize == value)
          return;
        m_iHeaderSize = value;
        LoadFromCurrentFile();
      }
    }

    [SortedCategory("File", 1), PropertyOrder(22)]
    [Description("Number of bytes after the terrain raw data. Used to calculate terrain dimension properly")]
    public int OutroSize
    {
      get { return m_iOutroSize; }
      set
      {
        if (m_iOutroSize == value)
          return;
        m_iOutroSize = value;
        LoadFromCurrentFile();
      }
    }

    [SortedCategory("File", 1), PropertyOrder(23)]
    [Description("Stride in bytes for a single row of terrain data. Can be left to -1 to use the width*component size")]
    public int RowStride
    {
      get { return m_iRowStride; }
      set
      {
        if (m_iRowStride == value)
          return;
        m_iRowStride = value;
        LoadFromCurrentFile();
      }
    }

    public override bool Active
    {
      get
      {
        return base.Active;
      }
      set
      {
        base.Active = value;
        if (Active)
          LoadFromCurrentFile();
      }
    }

    [Description("Defines the vertical scaling factor to map source values to world units")]
    public override float HeightScaling
    {
      get
      {
        return base.HeightScaling;
      }
      set
      {
        base.HeightScaling = value;
      }
    }

    public override void ResetParameter()
    {
      base.ResetParameter();
      m_iComponentSize = (int)ComponentType_e.Unsigned16Bit;
      m_iHeaderSize = m_iOutroSize = 0;
      m_iRowStride = -1;
      m_iSizeX = m_iSizeY = 0;
      FileName = null;
    }
  }

  #endregion

  #region ImportHeightmapDDS

  public class ImportHeightmapDDS : IHeightmapImporterFromDDSFile
  {
    public ImportHeightmapDDS()
      : base()
    {
      this._fScaling = 1.0f; // assumes most imports provide absolute values
    }

    public override string Name
    {
      get
      {
        return "Import from DDS file";
      }
    }

    public override string[] SupportedFileExtensions
    {
      get
      {
        return new string[] { ".dds" };
      }
    }

    private void LoadFromCurrentFile()
    {
      // always load with absolute filename
      if (string.IsNullOrEmpty(FileName) || FileHelper.IsAbsolute(FileName))
        LoadFromFile(FileName);
      else
        LoadFromFile(EditorManager.Project.MakeAbsolute(FileName));
      ShowLastError();
    }

    [EditorAttribute(typeof(FilenameEditor), typeof(UITypeEditor)), FileDialogFilter(new string[] { ".dds" })]
    public override string FileName
    {
      get
      {
        return base.FileName;
      }
      set
      {
        base.FileName = value;
        LoadFromCurrentFile();
      }
    }

    public override bool Active
    {
      get
      {
        return base.Active;
      }
      set
      {
        base.Active = value;
        if (Active)
          LoadFromCurrentFile();
      }
    }

    [Description("Defines the vertical scaling factor to map source values to world units. If the source file represents correct ablsolute heights (e.g. R32F dds), this value should be set to 1.0")]
    public override float HeightScaling
    {
      get
      {
        return base.HeightScaling;
      }
      set
      {
        base.HeightScaling = value;
      }
    }

  }

  #endregion
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
