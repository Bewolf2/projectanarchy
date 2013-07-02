/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

using System;
using CSharpFramework;
using CSharpFramework.Contexts;
using System.Drawing.Design;
using System.Windows.Forms;
using System.Drawing;
using System.Collections;
using TerrainBase.Shapes;
using TerrainEditorPlugin.Shapes;
using TerrainEditorPlugin.Dialogs;
using TerrainEditorPlugin.Filter;
using CSharpFramework.Serialization;
using NUnit.Core;


namespace TerrainEditorPlugin
{
	/// <summary>
	/// EditorPlugin : this class must exist and implement the IEditorPluginModule functions to add the shape creators (for instance)
	/// Also, the namespace (SpeedtreeEditorPlugin) must be the same as the plugin dll name, in this case SpeedtreeEditorPlugin.EditorPlugin.dll
	/// This is because the editor plugin manager searches for the SpeedtreeEditorPlugin.EditorPlugin class when opening the 
	/// SpeedtreeEditorPlugin.EditorPlugin.dll assembly
	/// </summary>
	public class EditorPlugin : IEditorPluginModule
	{
    /// <summary>
    /// Constructor of EditorPlugin that sets up plugin information
    /// </summary>
    public EditorPlugin()
    {
      _version = 0;                 //version used for serialization
      _name = "TerrainEditorPlugin";
    }

    /// <summary>
    /// This scope holds one global instance of the plugin info. It is returned by all shape's overridden GetPluginInformation function.
    /// This global info is filled inside InitPluginModule
    /// </summary>
    public static EditorPluginInfo EDITOR_PLUGIN_INFO = new EditorPluginInfo();

		TerrainShapeCreator _heightFieldCreator;
		TerrainEditorPanel _panel;

    #region InitPluginModule

    /// <summary>
    /// InitPluginModule : called at plugin initialisation time: Add the relevant shape creators here
    /// </summary>
    public override bool InitPluginModule()
    {
      TerrainManaged.ManagedModule.InitManagedModule();
      EDITOR_PLUGIN_INFO.NativePluginNames = new string[] { "VisionEnginePlugin" };

			TerrainEditor.Init();

      // Add IShapeCreatorPlugin
			_heightFieldCreator = new TerrainShapeCreator();
			EditorManager.ShapeCreatorPlugins.Add(_heightFieldCreator);

      // add default filter:
      TerrainEditor.HeightmapImporterList.Add(new ImportHeightmapTEX16bpp());
      TerrainEditor.HeightmapImporterList.Add(new ImportHeightmapRAW());
      TerrainEditor.HeightmapImporterList.Add(new ImportHeightmapDDS());

      TerrainEditor.HeightmapFilterList.Add(new HeightmapClearFilter());
      TerrainEditor.HeightmapFilterList.Add(new HeightmapScaleElevateFilter());

      TerrainEditor.DecorationFilterList.Add(new DecorationClearFilter());
      TerrainEditor.DecorationFilterList.Add(new DecorationImportFromLuminanceFilter());
      TerrainEditor.DecorationFilterList.Add(new DecorationFromDetailTextureFilter());
      TerrainEditor.DecorationFilterList.Add(new DecorationFromSlopeFilter());
      
      
			// create panel
			_panel = new TerrainEditorPanel(EditorManager.ApplicationLayout.DockingArea);
			_panel.ShowDockable();
      TerrainEditor.EditorPanel = _panel;

      // register tests:
      TestSuiteBuilder testBuilder = new TestSuiteBuilder();
      TestSuite testSuite = testBuilder.Build(typeof(EditorPlugin).Assembly.FullName);
      TestManager.AddTestSuite(testSuite);

      return true;
    }

    #endregion

    #region DeInitPluginModule

    /// <summary>
    /// DeInitPluginModule : called at plugin deinitialisation time: Remove relevant data
    /// </summary>
    public override bool DeInitPluginModule()
    {
      // Remove IShapeCreatorPlugins
			EditorManager.ShapeCreatorPlugins.Remove(_heightFieldCreator);

			TerrainEditor.DeInit();

			//_panel.Hide();
      TerrainManaged.ManagedModule.DeInitManagedModule();
      return true;
    }

    #endregion
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
