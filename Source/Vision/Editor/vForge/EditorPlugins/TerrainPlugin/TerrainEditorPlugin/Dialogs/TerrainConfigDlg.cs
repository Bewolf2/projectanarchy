/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using TerrainBase.Config;
using TerrainBase.Editing;
using CSharpFramework.Math;
using CSharpFramework;

namespace TerrainEditorPlugin.Dialogs
{
  public partial class TerrainConfigDlg : Form
  {
    // Variables
    bool bHighlightTiles = false;
    bool bShowSectorSizeX = false;
    bool bShowSectorSizeY = false;
    bool bShowBaseTexture = false;

    Point pMovedSectorPoint = Point.Empty;

    Point pOldRectPoint = Point.Empty;
    Point pNewRectPoint = Point.Empty;

    Point pOldRectPointIndex = Point.Empty;
    Point pNewRectPointIndex = Point.Empty;

    Point pInitialSectorSize = Point.Empty;

    Rectangle rOldRect;
    Rectangle rNewRect;
    Rectangle rMaxRect;

    float dsx = 0;
    float dsy = 0;

    int iMaxSectorSizeX = 0;
    int iMaxSectorSizeY = 0;

    private bool m_bNewTerrain = false;
    public bool NewTerrain
    {
      get
      {
        return m_bNewTerrain;
      }
      set
      {
        m_bNewTerrain = value;
      }
    }



    public TerrainConfigDlg()
    {
      InitializeComponent();

      FillComboBox();
      TerrainConfig.OnMemberChanged += new EventHandler(TerrainConfig_OnMemberChanged);
    }

    protected override void OnClosed(EventArgs e)
    {
      base.OnClosed(e);
      TerrainConfig.OnMemberChanged -= new EventHandler(TerrainConfig_OnMemberChanged);


      _config._newSectorIndexX = pNewRectPointIndex.X;
      _config._newSectorIndexY = pNewRectPointIndex.Y;

      _config._oldSectorIndexX = pOldRectPointIndex.X;
      _config._oldSectorIndexY = pOldRectPointIndex.Y;
    }

    #region Config

    TerrainConfig _config = new TerrainConfig();

    void UpdatePropertyGridObj()
    {
      if (_config == null)
      {
        //propertyGrid_Config.SelectedObject = null;
        return;
      }
      _config._bModifiableOnly = !_bFullConfig;
      _config._oldSectorCountX = _config._oldSectorCountY = -1;
      if (!_bFullConfig)
      {
        _config._oldSectorCountX = _config.SectorCountX;
        _config._oldSectorCountY = _config.SectorCountY;
      }

      propertyGrid_General.SelectedObject = new GeneralTerrainConfig(_config);
      propertyGrid_Heightmap.SelectedObject = new HeightmapTerrainConfig(_config);
      propertyGrid_Texturing.SelectedObject = new TexturingTerrainConfig(_config);
      propertyGrid_Vegetation.SelectedObject = new VegetationTerrainConfig(_config);
    }

    void RefreshPropertyGridObj()
    {
      propertyGrid_General.Refresh();
      propertyGrid_Heightmap.Refresh();
      propertyGrid_Texturing.Refresh();
      propertyGrid_Vegetation.Refresh();
    }


    /// <summary>
    /// Gets or sets the config. The set function clones it.
    /// </summary>
    public TerrainConfig Config
    {
      get
      {
        return _config;
      }
      set
      {
        _config = (TerrainConfig)value.Clone();
        _config.Modified = false;
        UpdatePropertyGridObj();
        UpdateStatistics();
      }
    }

    bool _bFullConfig = true;

    /// <summary>
    /// If true (default), the full config (creation time) can be edited, otherwise just a subset
    /// </summary>
    public bool FullConfig
    {
      get { return _bFullConfig; }
      set
      {
        if (_bFullConfig == value)
          return;
        _bFullConfig = value;
        UpdatePropertyGridObj();
        this.label_Import.Enabled = _bFullConfig;
        this.comboBox_Import.Enabled = _bFullConfig;
      }
    }

    #endregion

    void TerrainConfig_OnMemberChanged(object sender, EventArgs e)
    {
      UpdateStatistics();
      RefreshPropertyGridObj();
    }

    private void numericUpDown_Units_ValueChanged(object sender, EventArgs e)
    {
      UpdateStatistics();
    }

    private void propertyGrid_Config_SelectedGridItemChanged(object sender, SelectedGridItemChangedEventArgs e)
    {
      bHighlightTiles = false;
      bShowSectorSizeX = bShowSectorSizeY = false;
      bShowBaseTexture = false;

      if (e.NewSelection == null)
      {
        pictureBox1.Invalidate();
        return;
      }

      if (e.NewSelection.Label == "TilesPerSectorX" || e.NewSelection.Label == "TilesPerSectorY")
        bHighlightTiles = true;
      else if (e.NewSelection.Label == "SectorSizeX")
        bShowSectorSizeX = true;
      else if (e.NewSelection.Label == "SectorSizeY")
        bShowSectorSizeY = true;
      else if (e.NewSelection.Label == "BaseTextureFile" || e.NewSelection.Label == "BaseTextureTilingX" || e.NewSelection.Label == "BaseTextureTilingY")
        bShowBaseTexture = true;
      pictureBox1.Invalidate();
    }

    void UpdateStatistics()
    {
      if (_config == null)
        return;
      int iOverallSamples = _config.OverallHeightSampleCount.Width * _config.OverallHeightSampleCount.Height;
      decimal iHMMem = iOverallSamples * 4 / (1024 * 1024);
      label_SampleCount.Text = string.Format("{0} ({1:0.00}MB data)", iOverallSamples, iHMMem);
      decimal fUnits = this.numericUpDown_Units.Value/1000;
      decimal kmx = (decimal)_config.SectorCount.Width*(decimal)_config.SectorSize.X*fUnits;
      decimal kmy = (decimal)_config.SectorCount.Height*(decimal)_config.SectorSize.Y*fUnits;

      label_TerrainSize.Text = string.Format("{0:0.00} km x {1::0.00} km", kmx, kmy);
      label_TerrainArea.Text = string.Format("{0:0.00} km{1}", kmx*kmy, (char)178);

      int iWMx = _config.DefaultWeightmapResolution.Width * _config.SectorCount.Width;
      int iWMy = _config.DefaultWeightmapResolution.Height * _config.SectorCount.Height;
      decimal iSamples = iWMx*iWMy;
//      label_WeightmapRes.Text = string.Format("{0} x {1}", iWMx, iWMy);
//      label_WMUncompressed.Text = string.Format("{0:0.00}", iSamples * 4 / (1024 * 1024));
//      label_WMDXT5.Text = string.Format("{0:0.00}", iSamples / (1024 * 1024));
      pictureBox1.Invalidate();
    }

    private void button_OK_Click(object sender, EventArgs e)
    {
      const int iWarningCount = 64; // warn for more than 64 sectors
      if (this.Config._sectorCount.Width>iWarningCount ||Config._sectorCount.Height>iWarningCount)
      {
        string s = string.Format("The terrain has more than {0} sectors in one dimension (width and/or height).\nTerrain editing or the runtime might become instable with this large amount of sectors.",iWarningCount);
        EditorManager.ShowMessageBox(this, s, "Terrain is very large", MessageBoxButtons.OK, MessageBoxIcon.Information);
    }
    }

    #region Import Combobox

    void FillComboBox()
    {
      comboBox_Import.BeginUpdate();
      comboBox_Import.Items.Clear();

      if (TerrainEditor.HeightmapImporterList.Count > 0)
      {
        comboBox_Import.Enabled = true;
        comboBox_Import.Items.Add("<select source here>");

        foreach (IHeightmapImporter importer in TerrainEditor.HeightmapImporterList)
          comboBox_Import.Items.Add(importer);

        comboBox_Import.SelectedIndex = 0;
      }
      else
      {
        comboBox_Import.Enabled = false;
      }

      comboBox_Import.EndUpdate();
    }

    private void comboBox_Import_SelectedIndexChanged(object sender, EventArgs e)
    {
      IHeightmapImporter importer = comboBox_Import.SelectedItem as IHeightmapImporter;
      if (importer == null)
        return;

      ApplyHeightmapFilterDlg dlg = new ApplyHeightmapFilterDlg();
      dlg.Filter = importer;
      dlg.HasApplyButton = false; // show only
      if (dlg.ShowDialog() == DialogResult.OK)
      {
        Config.SettingsFromImporter(importer);
        UpdatePropertyGridObj();
        UpdateStatistics();
      }
      comboBox_Import.SelectedIndex = 0;
    }


    #endregion


    private void DrawArrow(PaintEventArgs e, Pen pen, float x1, float y1, float x2, float y2, float fPtrSize, bool bStart, bool bEnd)
    {
      e.Graphics.DrawLine(pen, x1, y1, x2, y2);
      Vector2F dir = new Vector2F(x2 - x1, y2 - y1);
      dir.Normalize();
      dir *= fPtrSize;

      if (bStart)
      {
        e.Graphics.DrawLine(pen, x1, y1, x1 + dir.X + dir.Y, y1 + dir.Y - dir.X);
        e.Graphics.DrawLine(pen, x1, y1, x1 + dir.X - dir.Y, y1 + dir.Y + dir.X);
      }
      if (bEnd)
      {
        e.Graphics.DrawLine(pen, x2, y2, x2 - dir.X + dir.Y, y2 - dir.Y - dir.X);
        e.Graphics.DrawLine(pen, x2, y2, x2 - dir.X - dir.Y, y2 - dir.Y + dir.X);
      }
    }

    private Point GetIndexInRect(Point pPoint, Rectangle rect, int dx, int dy, int iTargetRectSizeX, int iTargetRectSizeY, int iSourceRectWidth, int iSourceRectHeight)
    {
      if (rect.Contains(pPoint) == true)
      {
        int iSectorSizeX = rect.Width / dx;
        int iSectorSizeY = rect.Height / dy;

        // half of dx, dy is flexible space

        int iIndexX = (pPoint.X - rect.X + (dx / 2)) / dx;
        int iIndexY = (pPoint.Y - rect.Y + (dy / 2)) / dy;

        Point pReturn = Point.Empty;

        pReturn.X = iIndexX;
        pReturn.Y = iTargetRectSizeY - iSourceRectHeight - iIndexY;

        return pReturn;
      }

      return Point.Empty;
    }


    /// <summary>
    /// Draw Terrain Sector to 2D image
    /// Warning : Engine View`s Y is 2D view`s -Y
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>

    private void pictureBox1_Paint(object sender, PaintEventArgs e)
    {
      /// At first, save initial sector size.
      if (pInitialSectorSize == Point.Empty)
      {
        pInitialSectorSize.X = _config.SectorCount.Width;
        pInitialSectorSize.Y = _config.SectorCount.Height;
      }

      if (NewTerrain == true) // Now Modify Terrain
      {
        pInitialSectorSize.X = _config.SectorCount.Width;
        pInitialSectorSize.Y = _config.SectorCount.Height;
      }

      iMaxSectorSizeX = Math.Max(pInitialSectorSize.X + pOldRectPointIndex.X, _config.SectorCount.Width + pNewRectPointIndex.X);
      iMaxSectorSizeY = Math.Max(pInitialSectorSize.Y + pOldRectPointIndex.Y, _config.SectorCount.Height + pNewRectPointIndex.Y);


      if (_config == null)
        return;

      Rectangle rect = this.pictureBox1.ClientRectangle;
      Rectangle trect = rect; // rectangle relevant for terrain


      trect.Y = rect.Top + 10;
      trect.X = rect.Left + 10;

      trect.Width = rect.Width - 20;
      trect.Height = rect.Height - 20;


      // constraint to view size but keeping ratio intact
      float fSectorRatio = _config.SectorSize.X / _config.SectorSize.Y;
      float fTerrainRatio = fSectorRatio * (float)iMaxSectorSizeX / (float)iMaxSectorSizeY;
      float fViewRatio = (float)rect.Width / (float)rect.Height;


      if (fTerrainRatio > fViewRatio)
      {
        trect.Height = (int)((float)rect.Height * (fViewRatio / fTerrainRatio));
        trect.Y = (rect.Height - trect.Height) / 2; // recenter
      }
      else
      {
        trect.Width = (int)((float)rect.Width * (fTerrainRatio / fViewRatio));
        trect.X = (rect.Width - trect.Width) / 2; // recenter
      }

      rMaxRect = trect;

      dsx = (float)trect.Width / (float)iMaxSectorSizeX;
      dsy = (float)trect.Height / (float)iMaxSectorSizeY;


      rOldRect = trect;
      rOldRect.Width = (int)((float)rOldRect.Width * ((float)pInitialSectorSize.X / (float)iMaxSectorSizeX));
      rOldRect.Height = (int)((float)rOldRect.Height * ((float)pInitialSectorSize.Y / (float)iMaxSectorSizeY));


      rNewRect = trect;
      rNewRect.Width = (int)((float)rNewRect.Width * ((float)_config.SectorCount.Width / (float)iMaxSectorSizeX));
      rNewRect.Height = (int)((float)rNewRect.Height * ((float)_config.SectorCount.Height / (float)iMaxSectorSizeY));

      e.Graphics.FillRectangle(Brushes.Black, rect);

      Point pOffsetOldRect = GetIndexInRect(pOldRectPoint, rOldRect, (int)dsx, (int)dsy, iMaxSectorSizeX, iMaxSectorSizeY, pInitialSectorSize.X, pInitialSectorSize.Y);
      Point pOffsetNewRect = GetIndexInRect(pNewRectPoint, rNewRect, (int)dsx, (int)dsy, iMaxSectorSizeX, iMaxSectorSizeY, _config.SectorCount.Width, _config.SectorCount.Height);

      rOldRect.X += pOldRectPointIndex.X * (int)dsx;
      rOldRect.Y += (trect.Height - rOldRect.Height) - (pOldRectPointIndex.Y * (int)dsy);

      rNewRect.X += pNewRectPointIndex.X * (int)dsx;
      rNewRect.Y += (trect.Height - rNewRect.Height) - (pNewRectPointIndex.Y * (int)dsy);

      // Determine Drawing Order
      if (rNewRect.Contains(rOldRect))
      {
        e.Graphics.FillRectangle(Brushes.Brown, rNewRect);
        e.Graphics.FillRectangle(Brushes.DarkGreen, rOldRect);

        // Moving Sector position supported only Extension
        if (pInitialSectorSize.X == _config.SectorCount.Width && pInitialSectorSize.Y == _config.SectorCount.Height)
          CheckboxEnableMovingSectorPosition.Visible = false;
        else
          CheckboxEnableMovingSectorPosition.Visible = true;
      }
      else if (rOldRect.Contains(rNewRect))
      {
        e.Graphics.FillRectangle(Brushes.DarkGreen, rOldRect);
        e.Graphics.FillRectangle(Brushes.Brown, rNewRect);

        CheckboxEnableMovingSectorPosition.Visible = false;
      }
      else
      {
        e.Graphics.FillRectangle(Brushes.Brown, rNewRect);
        e.Graphics.FillRectangle(Brushes.DarkGreen, rOldRect);

        CheckboxEnableMovingSectorPosition.Visible = false;
      }

      bool bDrawExpensionArrow = true;
      if (bDrawExpensionArrow)
      {
        Pen pen = (Pen)Pens.White.Clone();
        pen.Width = 2.0f;
        float fPtrSize = Math.Min(dsx * 0.25f, 7.0f);

        DrawArrow(e, pen, rOldRect.Left, rOldRect.Top, rNewRect.Left, rNewRect.Top, fPtrSize, false, true);
        DrawArrow(e, pen, rOldRect.Right, rOldRect.Top, rNewRect.Right, rNewRect.Top, fPtrSize, false, true);
        DrawArrow(e, pen, rOldRect.Left, rOldRect.Bottom, rNewRect.Left, rNewRect.Bottom, fPtrSize, false, true);
        DrawArrow(e, pen, rOldRect.Right, rOldRect.Bottom, rNewRect.Right, rNewRect.Bottom, fPtrSize, false, true);
      }



      if (bShowBaseTexture)
      {
        bool flipX = _config.BaseTextureTilingX < 0;
        bool flipY = _config.BaseTextureTilingY > 0;

        float dx = (float)trect.Width / Math.Abs(_config.BaseTextureTilingX);
        float dy = (float)trect.Height / Math.Abs(_config.BaseTextureTilingY);
        int iTexSize = TerrainConfig.GetNextPowerOf2((int)Math.Max(dx, dy), 8, 128);
        Image img = _config.GetBaseTextureImage(iTexSize);
        if (img != null)
        {
          int iX = Math.Min(64, (int)Math.Abs(_config.BaseTextureTilingX));
          int iY = Math.Min(64, (int)Math.Abs(_config.BaseTextureTilingY));
          for (int y = 0; y < iY; y++)
            for (int x = 0; x < iX; x++)
            {
              RectangleF targetRect = new RectangleF(
                (float)trect.X + (float)(x + (flipX ? 1 : 0)) * dx,
                (float)trect.Y + (float)(y + (flipY ? 1 : 0)) * dy,
                (dx + 0.5f) * (flipX ? -1 : 1),
                (dy + 0.5f) * (flipY ? -1 : 1)
              );
              e.Graphics.DrawImage(img, targetRect);
            }
        }
      }

      // subdiv sectors:
      for (int sy = 0; sy < iMaxSectorSizeY; sy++)
        for (int sx = 0; sx < iMaxSectorSizeX; sx++)
        {
          e.Graphics.DrawRectangle(Pens.Green, (float)trect.X + (float)sx * dsx, (float)trect.Y + (float)sy * dsy, dsx, dsy);
        }

      if (bShowSectorSizeX)
      {
        Pen pen = (Pen)Pens.White.Clone();
        pen.Width = 2.0f;
        float fPtrSize = Math.Min(dsx * 0.25f, 7.0f);
        float sx = (float)(_config.SectorCount.Width / 2);
        float sy = (float)(_config.SectorCount.Height / 2);
        float x1 = (float)trect.X + sx * dsx + 1.0f;
        float x2 = (float)trect.X + (sx + 1.0f) * dsx - 1.0f;
        float y = (float)trect.Y + (sy + 0.0f) * dsy + fPtrSize + 2.0f;
        DrawArrow(e, pen, x1, y, x2, y, fPtrSize, true, true);
      }
      if (bShowSectorSizeY)
      {
        Pen pen = (Pen)Pens.White.Clone();
        pen.Width = 2.0f;
        float fPtrSize = Math.Min(dsx * 0.25f, 6.0f);
        float sx = (float)(_config.SectorCount.Width / 2);
        float sy = (float)(_config.SectorCount.Height / 2);
        float y1 = (float)trect.Y + sy * dsy + 1.0f;
        float y2 = (float)trect.Y + (sy + 1.0f) * dsy - 1.0f;
        float x = (float)trect.X + (sx + 0.0f) * dsx + fPtrSize + 2.0f;
        DrawArrow(e, pen, x, y1, x, y2, fPtrSize, true, true);
      }

      float dtx = dsx / (float)_config.TilesPerSector.Width;
      float dty = dsy / (float)_config.TilesPerSector.Height;

      // subdiv tiles in first sector
      Pen tileColor = bHighlightTiles ? Pens.LightCyan : Pens.Green;
      for (int sy = 0; sy < _config.TilesPerSector.Height; sy++)
        for (int sx = 0; sx < _config.TilesPerSector.Width; sx++)
        {
          e.Graphics.DrawRectangle(tileColor, (float)trect.X + (float)sx * dtx, (float)trect.Y + (float)sy * dty, dtx, dty);
        }

    }


    /// <summary>
    ///  Moving Sector position supported only Extension
    /// </summary>        
    private bool bDragStart = false;
    PictureBox pDragLayout = null;
    Point pStartLocation = Point.Empty;
    Rectangle tempRect = Rectangle.Empty;

    private void pictureBox1_MouseDown(object sender, MouseEventArgs e)
    {
      if (NewTerrain == true)
        return;

      if (CheckboxEnableMovingSectorPosition.Checked == false || CheckboxEnableMovingSectorPosition.Visible == false)
        return;

      if (rOldRect.Size == rNewRect.Size) // Cannot move anywhere
        return;


      if (rNewRect.Contains(rOldRect) == true && rOldRect.Contains(e.Location))
      {
        // Move Old Rect
        tempRect = rOldRect;
      }
      else if (rOldRect.Contains(rNewRect) == true && rNewRect.Contains(e.Location))
      {
        // Move New Rect
        tempRect = rNewRect;
      }
      else
      {
        return;
      }

      bDragStart = true;

      pDragLayout = new PictureBox();
      pDragLayout.BackColor = Color.FromArgb(100, Color.Beige);

      pictureBox1.Controls.Add(pDragLayout);

      pDragLayout.Size = tempRect.Size;
      pDragLayout.Top = tempRect.Top;
      pDragLayout.Left = tempRect.Left;

      pStartLocation = e.Location;
    }

    private void pictureBox1_MouseMove(object sender, MouseEventArgs e)
    {
      if (CheckboxEnableMovingSectorPosition.Checked == false || CheckboxEnableMovingSectorPosition.Visible == false)
        return;

      if (NewTerrain == true)
        return;

      if (bDragStart == true)
      {
        pDragLayout.Left = tempRect.Left + (e.Location.X - pStartLocation.X);
        pDragLayout.Top = tempRect.Top + (e.Location.Y - pStartLocation.Y);
      }
    }

    private void pictureBox1_MouseUp(object sender, MouseEventArgs e)
    {
      if (CheckboxEnableMovingSectorPosition.Checked == false || CheckboxEnableMovingSectorPosition.Visible == false)
        return;

      if (NewTerrain == true)
        return;

      if (tempRect == rNewRect)
      {
        pNewRectPoint.X = pDragLayout.Left;
        pNewRectPoint.Y = pDragLayout.Top;

        Point pOffsetNewRect = GetIndexInRect(pNewRectPoint, rMaxRect, (int)dsx, (int)dsy, iMaxSectorSizeX, iMaxSectorSizeY, _config.SectorCount.Width, _config.SectorCount.Height);
        pOffsetNewRect.X = System.Math.Min(pOffsetNewRect.X, iMaxSectorSizeX - _config.SectorCount.Width);
        pOffsetNewRect.Y = System.Math.Min(pOffsetNewRect.Y, iMaxSectorSizeY - _config.SectorCount.Height);
        pOffsetNewRect.X = System.Math.Max(pOffsetNewRect.X, 0);
        pOffsetNewRect.Y = System.Math.Max(pOffsetNewRect.Y, 0);
        pNewRectPointIndex = pOffsetNewRect;

        pictureBox1.Invalidate();
      }
      else if (tempRect == rOldRect)
      {
        pOldRectPoint.X = pDragLayout.Left;
        pOldRectPoint.Y = pDragLayout.Top;

        Point pOffsetOldRect = GetIndexInRect(pOldRectPoint, rMaxRect, (int)dsx, (int)dsy, iMaxSectorSizeX, iMaxSectorSizeY, pInitialSectorSize.X, pInitialSectorSize.Y);
        // Do not move the rectangle out of the bigger rectangle.
        pOffsetOldRect.X = System.Math.Min(pOffsetOldRect.X, iMaxSectorSizeX - pInitialSectorSize.X);
        pOffsetOldRect.Y = System.Math.Min(pOffsetOldRect.Y, iMaxSectorSizeY - pInitialSectorSize.Y);
        pOffsetOldRect.X = System.Math.Max(pOffsetOldRect.X, 0);
        pOffsetOldRect.Y = System.Math.Max(pOffsetOldRect.Y, 0);
        pOldRectPointIndex = pOffsetOldRect;


        pictureBox1.Invalidate();
      }

      pictureBox1.Controls.Clear();
      pDragLayout = null;
      bDragStart = false;
      tempRect = Rectangle.Empty;
    }

  }
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
