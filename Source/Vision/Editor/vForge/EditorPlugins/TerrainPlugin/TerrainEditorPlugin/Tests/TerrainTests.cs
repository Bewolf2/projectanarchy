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
using System.IO;
using NUnit.Framework;
using CSharpFramework;
using CSharpFramework.Math;
using CSharpFramework.Shapes;
using TerrainEditorPlugin.Shapes;
using TerrainBase.Editing;
using TerrainManaged;
using CSharpFramework.Actions;

/*
- Bug-Eintraege in Hansoft (auch fuer fehlende Textur bei Terrain-Neuerzeugung), mit Verweis auf automatisierte Test sowie manuelle Repro-Methode
- Test aus Build nehmen (properties) und in Plugin-Init auskommentieren
- Einchecken
- Einen Hansoft-Bug eintragen: Terrain-Tests derzeit deaktiviert, da sie fehlschlagen
*/


namespace TerrainEditorPlugin.Tests
{

  [TestFixture]
  public class TerrainTests
  {

    [SetUp]
    public void SetUp()
    {
      // Clear the undo/redo stack, to ensure that our tests only undo "local" operations
      EditorManager.Actions.Reset();

      TestManager.Helpers.CreateTestScene("TerrainTest.scene");

      TestManager.Helpers.ProcessEvents();

      //Change camera pos/rot to something useful
      EditorManager.ActiveView.SetCameraPosition(new Vector3F(-200.0f, 0.0f, 400.0f));
      EditorManager.ActiveView.SetCameraRotation(new Vector3F(0.0f, 40.0f, 0.0f));

      EditorManager.ActiveView.UpdateView(true);

      TestManager.Helpers.ProcessEvents();


      //Add Terrain
      EditorManager.GetShapeCreatorPluginByNameAndCategory("Geometry", "Terrain").ExecutePlugin();
      TestManager.Helpers.ProcessEvents();
      //Undo of Add Terrain is unsupported!
      EditorManager.Actions.Reset();
    }

    /// <summary>
    /// Clears the test data
    /// </summary>
    [TearDown]
    public void TearDown()
    {
      EditorManager.Scene.ActiveLayer.RemoveShape(TerrainEditor.CurrentTerrain);
      TestManager.Helpers.CloseTestProject();
    }


    
    //FIXME [HS:6411] [Test]
    public void CreatingTerrain()
    {
      //Check whether terrain shape is in scene
      Assert.IsTrue(IsTerrainInScene());

      //Remove Terrain
      EditorManager.Actions.Add(RemoveShapeAction.CreateRemoveShapeAction(TerrainEditor.CurrentTerrain));

      //Check that terrain shape is NOT in scene
      Assert.IsTrue(!IsTerrainInScene());

      //Add again
      EditorManager.GetShapeCreatorPluginByNameAndCategory("Geometry", "Terrain").ExecutePlugin();
      TestManager.Helpers.ProcessEvents();
      //Undo of Add Terrain is unsupported!
      EditorManager.Actions.Reset();

      
      //Check whether terrain shape is in scene
      Assert.IsTrue(IsTerrainInScene());

      // Test the various painting methods
      PaintTerrain(TerrainManaged.HeightmapEditMode_e.Elevate, 300, 128, 4);
      PaintTerrain(TerrainManaged.HeightmapEditMode_e.Flatten, 300, 128, 4);

      PaintTerrain(TerrainManaged.HeightmapEditMode_e.Elevate, 300, 128, 3);
      PaintTerrain(TerrainManaged.HeightmapEditMode_e.Flatten, 300, 128, 3);

      PaintTerrain(TerrainManaged.HeightmapEditMode_e.Elevate, 300, 128, 3);
      SmoothTerrain(300, 128);

      //Toggle lighting
      bool lighting = TerrainEditor.DirectionalLighting;
      TerrainEditor.DirectionalLighting = !TerrainEditor.DirectionalLighting;
      Assert.AreEqual(TerrainEditor.DirectionalLighting, !lighting);
      EditorManager.ActiveView.UpdateView(true);

      TerrainEditor.DirectionalLighting = !TerrainEditor.DirectionalLighting;
      Assert.AreEqual(TerrainEditor.DirectionalLighting, lighting);
      EditorManager.ActiveView.UpdateView(true);

      //Change lighting dir & color
      Vector3F dir = new Vector3F(0.0f, 0.2f, -0.8f);
      dir.Normalize();
      TerrainEditor.CurrentTerrain.EngineTerrain.SetDirectionalLightProperties(dir, System.Drawing.Color.FromArgb(96, 64, 32), System.Drawing.Color.FromArgb(196, 128, 96));
      
      //Undo/redo test (everything)
      while (EditorManager.Actions.Undo()) {}
      while (EditorManager.Actions.Redo()) {}

      //Verify redone state (e.g. at least that the terrain exists)
      Assert.IsTrue(IsTerrainInScene());
    }


    [Test]
    public void TextureTest()
    {
      TerrainEditor.EditorPanel.PaintDetailTextureMode = true;

      //Create a detail texture to paint with
      DetailTextureResource texture = TerrainEditor.CurrentTerrain.CreateDetailTexture();
      texture.DiffuseFilename = "Textures/sand.dds";
      texture.Name = System.IO.Path.GetFileNameWithoutExtension(texture.DiffuseFilename);
      EditorManager.Actions.Add(new Actions.AddDetailTextureAction(TerrainEditor.CurrentTerrain, texture));
      
      TerrainEditor.CurrentDetailTexture = texture;
      TestManager.Helpers.ProcessEvents();

      //Create another detail texture to paint with
      texture = TerrainEditor.CurrentTerrain.CreateDetailTexture();
      texture.DiffuseFilename = "Textures/foam.dds";
      texture.Name = System.IO.Path.GetFileNameWithoutExtension(texture.DiffuseFilename);
      EditorManager.Actions.Add(new Actions.AddDetailTextureAction(TerrainEditor.CurrentTerrain, texture));

      TerrainEditor.CurrentDetailTexture = texture;
      TestManager.Helpers.ProcessEvents();

      //Paint detail texture
      PaintDetailTexture(200, 200);
      TestManager.Helpers.ProcessEvents();

      //Create a third detail texture to paint with
      texture = TerrainEditor.CurrentTerrain.CreateDetailTexture();
      texture.DiffuseFilename = "Textures/Havok_Logo_128x64.dds";
      texture.Name = System.IO.Path.GetFileNameWithoutExtension(texture.DiffuseFilename);
      EditorManager.Actions.Add(new Actions.AddDetailTextureAction(TerrainEditor.CurrentTerrain, texture));

      TerrainEditor.CurrentDetailTexture = texture;
      TestManager.Helpers.ProcessEvents();

      //Paint some more
      PaintDetailTexture(200, 500);
      TestManager.Helpers.ProcessEvents();

      //Undo/redo test (everything)
      while (EditorManager.Actions.Undo()) {}
      while (EditorManager.Actions.Redo()) {}

      //Verify redone state (e.g. at least that the terrain exists)
      Assert.IsTrue(IsTerrainInScene());
    }


    //FIXME [HS:6411] [Test]
    public void VegetationTest()
    {
      // Create a decoration model to paint with
      DecorationModelResource model = TerrainEditor.CurrentTerrain.CreateDecorationModelResource();
      model.Filename = "Models\\MissingModel.model";
      model.Name = System.IO.Path.GetFileNameWithoutExtension(model.Filename);
      EditorManager.Actions.Add(new Actions.AddDecorationModelAction(TerrainEditor.CurrentTerrain, model));

      // Configure the model
      TerrainEditor.CurrentDecorationModel = model;
      TerrainEditor.EditorPanel.PaintDecorationMode = true;
      model.FarClipDistance = 10000.0f;
      model.Density = 0.4f;

      float oldAverageScale = model.AverageScale;
      float oldDensity = model.Density;
      float oldSizeVariation = model.SizeVariation;

      // Test painting vegetation
      TerrainEditor.Cursor3DProperties.Radius = 1000.0f;
      PaintVegetation(200, 200);

      int numVegetationAfterFirstPainting = TerrainEditor.CurrentTerrain.DecorationObjectCount;

      // Test painting vegetation
      TerrainEditor.Cursor3DProperties.Radius = 500.0f;
      PaintVegetation(400, 400);

      int numVegetationAfterSecondPainting = TerrainEditor.CurrentTerrain.DecorationObjectCount;

      // Change the painting parameters and verify that the number of decoration object changes as well
      model.Density = 0.8f;
      EditorManager.ActiveView.UpdateView(true);
      int numVegetationAfterDensityChange = TerrainEditor.CurrentTerrain.DecorationObjectCount;

      Assert.IsTrue(TerrainEditor.CurrentTerrain.DecorationObjectCount > numVegetationAfterSecondPainting);
      
      //Test undo/redo
      model.Density = oldDensity;
      model.AverageScale = oldAverageScale;
      model.SizeVariation = oldSizeVariation;

      EditorManager.Actions.Undo();
      Assert.IsTrue(TerrainEditor.CurrentTerrain.DecorationObjectCount == numVegetationAfterFirstPainting);
      
      EditorManager.Actions.Undo();
      Assert.IsTrue(TerrainEditor.CurrentTerrain.DecorationObjectCount == 0);

      EditorManager.Actions.Redo();
      Assert.IsTrue(TerrainEditor.CurrentTerrain.DecorationObjectCount == numVegetationAfterFirstPainting);

      EditorManager.Actions.Redo();
      Assert.IsTrue(TerrainEditor.CurrentTerrain.DecorationObjectCount == numVegetationAfterSecondPainting);
    }


    //FIXME [HS:6411] [Test]
    public void HeightMapExportImport()
    {
      // Do some painting
      PaintTerrain(TerrainManaged.HeightmapEditMode_e.Elevate, 300, 128, 3);

      float heightBefore = TerrainHeight(300, 128);

      //Export the heightmap un-normalized
      string filename0 = EditorManager.Project.MakeAbsolute("editortest0.dds");
      ExportHeightmap(filename0, false);
      
      //Remove old terrain and add new terrain
      EditorManager.Scene.ActiveLayer.RemoveShape(TerrainEditor.CurrentTerrain);
      EditorManager.GetShapeCreatorPluginByNameAndCategory("Geometry", "Terrain").ExecutePlugin();
      TestManager.Helpers.ProcessEvents();
      //Undo of Add Terrain is unsupported!
      EditorManager.Actions.Reset();

      //Import a previously exported heightmap
      ImportHeightmap(filename0);

      TerrainEditor.EditorPanel.SetHeightmapEditMode(TerrainManaged.HeightmapEditMode_e.Elevate);
      float heightAfter = TerrainHeight(300, 128);

      //Test whether the heights are about the same
      Assert.IsTrue(Math.Abs(heightBefore - heightAfter) < 0.1f);  
    }


    void ExportHeightmap(string filename, bool normalized)
    {
      bool result = false;
      try
      {
        File.Delete(filename);
        result = TerrainEditor.CurrentTerrain.EngineTerrain.ExportHeightmapAsDDS(filename, normalized);
      }
      catch (Exception e)
      {
        EditorManager.DumpException(e);
        result = false;
      }

      Assert.IsTrue(result);
      Assert.IsTrue(File.Exists(filename));
    }


    void ImportHeightmap(string filename)
    {
      foreach (Object item in TerrainEditor.EditorPanel.comboBox_ImportHeightfield.Items)
      {
        IHeightmapImporterFromDDSFile filter = item as IHeightmapImporterFromDDSFile;
        if (filter != null)
        {
          // Create the filter
          filter.FileName = filename;
          Assert.IsTrue(filter.Valid);

          // Apply the filter
          if (!filter.OnStart(TerrainEditor.CurrentTerrain.Heightmap, 0, 0, -1, -1))
            Assert.Fail("HeightmapFilter: OnStart failed");

          TerrainEditor.CurrentTerrain.EngineTerrain.ApplyHeightmapFilter(filter, null);
          filter.OnFinish();

          // Trigger the action that has the backed-up height values
          EditorManager.Actions.Add(new TerrainChangedAction(TerrainEditor.CurrentTerrain.EngineTerrain, "Import Heightmap : " + filter.Name));

          // Update the view
          EditorManager.ActiveView.UpdateView(true);

          break;
        }
      }
    }


    bool IsTerrainInScene()
    {
      foreach (Shape3D shape in EditorManager.Scene.ActiveLayer.Root.ChildCollection)
        if (shape is TerrainShape) return true;
  
       return false;
    }

    // should not be called "TestPaintTerrain" because all functions starting with "Test" are used as unit-tests
    public void PaintTerrain(TerrainManaged.HeightmapEditMode_e mode, int x, int y, int brushId)
    {
      //Paint some Terrain
      TerrainEditor.EditorPanel.SetHeightmapEditMode(mode);
      TerrainEditor.EditorPanel.BrushListCtrl1.SelectBrushById(brushId);

      for (int i = 0; i < 5; ++i)
      {
        int dx = x + i * 5;
        float heightBefore = TerrainHeight(dx, y);

        EditorManager.ActiveView.CurrentContext.OnMouseDown(new System.Windows.Forms.MouseEventArgs(System.Windows.Forms.MouseButtons.Left, 1, dx, y, 0), CSharpFramework.Contexts.KeyModifier.Ctrl, 0, 0);
        EditorManager.ActiveView.UpdateView(true);

        float heightAfter = TerrainEditor.CurrentTerrain.TerrainHeightAtPosition(TerrainEditor.CurrentTerrain.EngineTerrain.GetCurrentCursorCenter());

        //Test whether there actually is a difference between the heights
        Assert.IsTrue(Math.Abs(heightBefore - heightAfter) > 0.1f); 
      }

      EditorManager.ActiveView.CurrentContext.OnMouseUp(new System.Windows.Forms.MouseEventArgs(System.Windows.Forms.MouseButtons.Left, 1, 0, 0, 0), CSharpFramework.Contexts.KeyModifier.None, 0, 0);
      EditorManager.ActiveView.UpdateView(true);


      //Test undo/redo
      float a = TerrainHeight(x, y);
      EditorManager.Actions.Undo();

      float b = TerrainHeight(x, y);
      Assert.IsTrue(Math.Abs(a - b) > 0.1f);

      EditorManager.Actions.Redo();
      float c = TerrainHeight(x, y);
      Assert.IsTrue(a == c);


      TestManager.Helpers.ProcessEvents();
    }


    void PaintDetailTexture(int x, int y)
    {
      for (int i = 0; i < 20; ++i)
      {
        int dx = x + i * 50;
        EditorManager.ActiveView.CurrentContext.OnMouseDown(new System.Windows.Forms.MouseEventArgs(System.Windows.Forms.MouseButtons.Left, 1, dx, y, 0), CSharpFramework.Contexts.KeyModifier.Ctrl, 0, 0);
        EditorManager.ActiveView.UpdateView(true);
      }

      EditorManager.ActiveView.CurrentContext.OnMouseUp(new System.Windows.Forms.MouseEventArgs(System.Windows.Forms.MouseButtons.Left, 1, 0, 0, 0), CSharpFramework.Contexts.KeyModifier.None, 0, 0);
      EditorManager.ActiveView.UpdateView(true);
    }


    public void PaintVegetation(int x, int y)
    {
      int numVegetationBefore = TerrainEditor.CurrentTerrain.DecorationObjectCount;

      EditorManager.ActiveView.CurrentContext.OnMouseDown(new System.Windows.Forms.MouseEventArgs(System.Windows.Forms.MouseButtons.Left, 1, x, y, 0), CSharpFramework.Contexts.KeyModifier.Ctrl, 0, 0);
      EditorManager.ActiveView.UpdateView(true);
      EditorManager.ActiveView.CurrentContext.OnMouseUp(new System.Windows.Forms.MouseEventArgs(System.Windows.Forms.MouseButtons.Left, 1, 0, 0, 0), CSharpFramework.Contexts.KeyModifier.None, 0, 0);
      EditorManager.ActiveView.UpdateView(true);

      Assert.IsTrue(TerrainEditor.CurrentTerrain.DecorationObjectCount > numVegetationBefore);
    }



    void SmoothTerrain(int x, int y)
    {
      TerrainEditor.EditorPanel.SetHeightmapEditMode(TerrainManaged.HeightmapEditMode_e.Smooth);
      TerrainEditor.EditorPanel.BrushListCtrl1.SelectBrushById(4);

      bool bChangedAtAll = false;
      for (int i = 0; i < 10; ++i)
      {
        int dx = x + i * 5;
        float heightBefore = TerrainHeight(dx, y);

        EditorManager.ActiveView.CurrentContext.OnMouseDown(new System.Windows.Forms.MouseEventArgs(System.Windows.Forms.MouseButtons.Left, 1, dx, y, 0), CSharpFramework.Contexts.KeyModifier.Ctrl, 0, 0);
        EditorManager.ActiveView.UpdateView(true);

        float heightAfter = TerrainEditor.CurrentTerrain.TerrainHeightAtPosition(TerrainEditor.CurrentTerrain.EngineTerrain.GetCurrentCursorCenter());

        // Smoothing is a bit difficult to verify, since we don't necessarily have a change at a specific texel.
        // Thus take a more relaxed test and ensure that the heightmap changes once at least.

        //Test whether there actually is a difference between the heights
        if (Math.Abs(heightBefore - heightAfter) > 0.1f)  
          bChangedAtAll = true;
      }

      Assert.IsTrue(bChangedAtAll);

      EditorManager.ActiveView.CurrentContext.OnMouseUp(new System.Windows.Forms.MouseEventArgs(System.Windows.Forms.MouseButtons.Left, 1, 0, 0, 0), CSharpFramework.Contexts.KeyModifier.None, 0, 0);
    }


    float TerrainHeight(int x, int y)
    {
      EditorManager.ActiveView.CurrentContext.OnMouseMove(new System.Windows.Forms.MouseEventArgs(System.Windows.Forms.MouseButtons.None, 1, x, y, 0), CSharpFramework.Contexts.KeyModifier.Ctrl, 0, 0);
      EditorManager.ActiveView.UpdateView(true);
      return TerrainEditor.CurrentTerrain.TerrainHeightAtPosition(TerrainEditor.CurrentTerrain.EngineTerrain.GetCurrentCursorCenter());
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
