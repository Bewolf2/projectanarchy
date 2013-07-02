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
using System.Linq;
using System.Text;
using System.Windows.Forms;

using CSharpFramework;
using CSharpFramework.Scene;
using CSharpFramework.Shapes;
using VisionEditorPlugin.Shapes;
using VisionManaged;
using Editor;


namespace Editor.Dialogs
{
  public partial class SceneWizardDlg : Form
  {

    #region Enum - Steps

    private enum SceneWizardStep_e
    {
      Renderer,
      PostProcessors,
      Lighting,
      
      Finish
    }

    #endregion

    #region Constructor

    public SceneWizardDlg()
    {
      InitializeComponent();

      // Adaptive tone mapper disabled for now([HS:#4044])
      label_AdaptiveTM.Visible = false;
      pictureBox_AdaptiveTM.Visible = false;
      checkBox_AdaptiveTM.Visible = false;

    }

    #endregion

    #region Events

    private void SceneWizardDlg_Load(object sender, EventArgs e)
    {
      // Check settings
      checkBox_Options_Show.Checked = EditorManager.Settings.ShowSceneWizardOnSceneCreation;
      checkBox_Options_Description.Checked = EditorManager.Settings.ShowSceneWizardOptionDescriptions;
      checkBox_Lighting.Checked = true;
      // Bring to top
      panel_Renderer.Dock = DockStyle.Fill;
      panel_PostProcessor.Dock = DockStyle.Fill;
      panel_Lighting.Dock = DockStyle.Fill;
      panel_Finish.Dock = DockStyle.Fill;

#if HK_ANARCHY
      // apply Anarchy limitations
      radioButton_Forward.Enabled = false;
      radioButton_Forward.Checked = false;
      radioButton_Deferred.Enabled = false;
      radioButton_ForwardMobile.Checked = true;
#endif

      // Make first step active
      StepUpdate();
    }

    private void button_Back_Click(object sender, EventArgs e)
    {
      StepBackward();
    }

    private void button_Next_Click(object sender, EventArgs e)
    {
      StepForward();
    }

    private void button_OK_Click(object sender, EventArgs e)
    {
      Hide();
      ApplySettingsOnScene();
    }

    private void checkBox_TOD_CheckedChanged(object sender, EventArgs e)
    {
      checkBox_Sunlight.Enabled  = checkBox_TOD.Checked;
      checkBox_Backlight.Enabled = checkBox_Sunlight.Enabled && checkBox_Sunlight.Checked;
      checkBox_Shadows.Enabled = checkBox_Sunlight.Enabled && checkBox_Sunlight.Checked;

#if !HK_ANARCHY
      checkBox_Sunglare.Enabled  = checkBox_Sunlight.Enabled && checkBox_Sunlight.Checked;
#endif
    }
    
    private void checkBox_Sunlight_CheckedChanged(object sender, EventArgs e)
    {
      checkBox_Shadows.Enabled = checkBox_Sunlight.Enabled && checkBox_Sunlight.Checked;
#if !HK_ANARCHY
      checkBox_Sunglare.Enabled  = checkBox_Sunlight.Enabled && checkBox_Sunlight.Checked;
#endif
      checkBox_Backlight.Enabled = checkBox_Sunlight.Enabled && checkBox_Sunlight.Checked;
    }

    private void checkBox_Options_Description_CheckedChanged(object sender, EventArgs e)
    {
      EditorManager.Settings.ShowSceneWizardOptionDescriptions = checkBox_Options_Description.Checked;
      UpdateDescriptions();
      StepUpdate();
    }

    private void checkBox_Options_Show_CheckedChanged(object sender, EventArgs e)
    {
      EditorManager.Settings.ShowSceneWizardOnSceneCreation = checkBox_Options_Show.Checked;
    }

    #endregion 

    #region Helper

    private void StepForward()
    {
      // If no renderer node was selected we skip the post processing and lighting step
      if (_step == SceneWizardStep_e.Renderer && radioButton_NoRenderer.Checked)
      {
        // If no renderer node is selected disable all post processors
        // Adaptive tone mapper disabled for now([HS:#4044])
        checkBox_TM.Checked = /*checkBox_AdaptiveTM.Checked =*/ false;
        check_Renderer.Visible = true;
        check_PostProcessor.Visible = true;
        check_Lighting.Visible = true;
        _step = SceneWizardStep_e.Finish;
      }

      switch(_step)
      {
        case SceneWizardStep_e.Renderer: _step = SceneWizardStep_e.PostProcessors;
          break;
        case SceneWizardStep_e.PostProcessors: _step = SceneWizardStep_e.Lighting;
          break;
        case SceneWizardStep_e.Lighting: _step = SceneWizardStep_e.Finish;
          break;
      }
      StepUpdate();
    }

    private void StepBackward()
    {
      // If no renderer node was selected we skip the post processing and lighting step
      if (_step == SceneWizardStep_e.Finish && radioButton_NoRenderer.Checked)
      {
        // If no renderer node is selected enable at least one post processors
        checkBox_TM.Checked = true;
        check_Renderer.Visible = false;
        check_PostProcessor.Visible = false;
        check_Lighting.Visible = false;
        _step = SceneWizardStep_e.Renderer;
      }

      switch (_step)
      {
        case SceneWizardStep_e.PostProcessors: _step = SceneWizardStep_e.Renderer;
          break;
        case SceneWizardStep_e.Lighting: _step = SceneWizardStep_e.PostProcessors;
          break;
        case SceneWizardStep_e.Finish: _step = SceneWizardStep_e.Lighting;
          break;
      }
      StepUpdate();
    }

    private void StepUpdate()
    {
      // Disable all step panels
      panel_Renderer.Enabled = false;
      panel_Renderer.Visible = false;
      panel_PostProcessor.Enabled = false;
      panel_PostProcessor.Visible = false;
      panel_Lighting.Enabled = false;
      panel_Lighting.Visible = false;
      panel_Finish.Enabled = false;
      panel_Finish.Visible = false;
      button_Back.Enabled = false;
      button_Next.Enabled = false;
      button_OK.Enabled = false;

      // Make current active
      switch (_step)
      {
        case SceneWizardStep_e.Renderer:
          {
            panel_Renderer.Dock = DockStyle.Fill;
            panel_Renderer.Enabled = true;
            panel_Renderer.Visible = true;
            button_Next.Enabled = true;
            check_Renderer.Visible = false;
            check_PostProcessor.Visible = false;
            check_Lighting.Visible = false;
            break;
          }
        case SceneWizardStep_e.PostProcessors:
          {
            panel_PostProcessor.Dock = DockStyle.Fill;
            panel_PostProcessor.Enabled = true;
            panel_PostProcessor.Visible = true;
            button_Back.Enabled = true;
            button_Next.Enabled = true;
            check_Renderer.Visible = true;
            check_PostProcessor.Visible = false;
            check_Lighting.Visible = false;
            StepPostProcessor();
            break;
          }
        case SceneWizardStep_e.Lighting:
          {
            panel_Lighting.Dock = DockStyle.Fill;
            panel_Lighting.Enabled = true;
            panel_Lighting.Visible = true;
            button_Back.Enabled = true;
            button_Next.Enabled = true;
            check_PostProcessor.Visible = true;
            check_Lighting.Visible = false;
            break;
          }
        case SceneWizardStep_e.Finish:
          {
            panel_Finish.Dock = DockStyle.Fill;
            panel_Finish.Enabled = true;
            panel_Finish.Visible = true;
            button_Back.Enabled = true;
            button_Next.Enabled = false;
            button_OK.Enabled = true;
            check_Lighting.Visible = true;
            StepFinish();
            break;
          }
      }
    }

    private void StepPostProcessor()
    {
      // Default Forward/ Deferred only post processors
      bool bSupported = radioButton_Forward.Checked || radioButton_Deferred.Checked;
      checkBox_DOF.Visible = checkBox_DOF.Enabled = bSupported;
      checkBox_SSAO.Visible = checkBox_SSAO.Enabled = bSupported;
      checkBox_AA.Visible = checkBox_AA.Enabled = bSupported;

      label_DOF.Visible = label_DOF.Enabled = bSupported && checkBox_Options_Description.Checked;
      label_SSAO.Visible = label_SSAO.Enabled = bSupported && checkBox_Options_Description.Checked;
      label_AA.Visible = label_AA.Enabled = bSupported && checkBox_Options_Description.Checked;

      pictureBox_DOF.Visible = pictureBox_DOF.Enabled = bSupported && checkBox_Options_Description.Checked;
      pictureBox_SSAO.Visible = pictureBox_SSAO.Enabled = bSupported && checkBox_Options_Description.Checked;
      pictureBox_AA.Visible = pictureBox_AA.Enabled = bSupported && checkBox_Options_Description.Checked;
      
      // Deferred only post processors
      bSupported = radioButton_Deferred.Checked;
      checkBox_Lighting.Visible = checkBox_Lighting.Enabled = bSupported;
      checkBox_RimLight.Visible = checkBox_RimLight.Enabled = bSupported;
      checkBox_DepthFog.Visible = checkBox_DepthFog.Enabled = bSupported;

      label_Lighting.Visible = label_Lighting.Enabled = bSupported && checkBox_Options_Description.Checked;
      label_RimLight.Visible = label_RimLight.Enabled = bSupported && checkBox_Options_Description.Checked;
      label_DepthFog.Visible = label_DepthFog.Enabled = bSupported && checkBox_Options_Description.Checked;
      
      pictureBox_Lighting.Visible = pictureBox_Lighting.Enabled = bSupported && checkBox_Options_Description.Checked;
      pictureBox_RimLight.Visible = pictureBox_RimLight.Enabled = bSupported && checkBox_Options_Description.Checked;
      pictureBox_DepthFog.Visible = pictureBox_DepthFog.Enabled = bSupported && checkBox_Options_Description.Checked;

      label_PostProcessorTitle.Text = radioButton_Deferred.Checked ? "2. Post Processors (Deferred)" : "2. Post Processors (Forward)";
    }

    private void StepFinish()
    {
      // Check which renderer is selected
      bool bIsForward = radioButton_Forward.Checked;
      bool bIsDeferred = radioButton_Deferred.Checked;
      bool bIsMobile = radioButton_ForwardMobile.Checked;
      bool bIsSimple = radioButton_NoRenderer.Checked;

      // Renderer String
      {
        if (bIsForward)
          label_Info_Renderer.Text = "Forward";
        else if (bIsMobile)
          label_Info_Renderer.Text = "Forward Mobile";
        else if (bIsDeferred)
          label_Info_Renderer.Text = "Deferred";
        else
          label_Info_Renderer.Text = "Simple";
      }

      // Post Processors String
      {
        // Generally define which Post processors are supported
        bool bSupportsSSAO = bIsForward || bIsDeferred;
        bool bSupportsGlow = bIsForward || bIsDeferred || bIsMobile;
        bool bSupportsTonemapping = bIsForward || bIsDeferred || bIsMobile;
        bool bSupportsDOF = bIsForward || bIsDeferred;
        bool bSupportsLighting = bIsDeferred;
        bool bSupportsRim = bIsDeferred;
        bool bSupportsDepthFog = bIsDeferred;
        bool bSupportsAntiAliasing = bIsForward || bIsDeferred;

        string post = "";

        post += bSupportsTonemapping && checkBox_TM.Checked ? "Tone Mapping / " : "";
        post += bSupportsSSAO && checkBox_SSAO.Checked ? "SSAO / " : "";
        post += bSupportsGlow && checkBox_Glow.Checked ? "Glow / " : "";
        post += bSupportsDOF && checkBox_DOF.Checked ? "Depth of Field / " : "";
        post += bSupportsLighting && checkBox_Lighting.Checked ? "Lighting / " : "";
        post += bSupportsRim && checkBox_RimLight.Checked ? "Rim Lighting / " : "";
        post += bSupportsDepthFog && checkBox_DepthFog.Checked ? "Depth Fog / " : "";
        post += bSupportsAntiAliasing && checkBox_AA.Checked ? "FXAA / " : "";
        post = post.TrimEnd(" /".ToCharArray());

        if (string.IsNullOrEmpty(post))
          post = "None";

        label_Info_PP.Text = post;
      }

      // Lighting
      {
        // they are all supported as long as any renderer node is set
        bool bTimeOfDay = bIsForward || bIsDeferred || bIsMobile;
        bool bSunLight  = bIsForward || bIsDeferred || bIsMobile;
        bool bBackLight = bIsForward || bIsDeferred || bIsMobile;
        bool bShadows   = bIsForward || bIsDeferred || bIsMobile;
        bool bSunGlare  = bIsForward || bIsDeferred || bIsMobile;

        // Check what we have checked in the UI
        bTimeOfDay = bTimeOfDay && checkBox_TOD.Checked;
        bSunLight  = bSunLight && checkBox_Sunlight.Checked;
        bBackLight = bBackLight && checkBox_Backlight.Checked;
        bShadows   = bShadows && checkBox_Shadows.Checked;
        bSunGlare  = bSunGlare && checkBox_Sunglare.Checked;

        // now disable everything that is dependent on something that is not selected

        bSunLight  = bSunLight && bTimeOfDay;
        bBackLight = bBackLight && bSunLight;
        bShadows   = bShadows && bSunLight;
        bSunGlare  = bSunGlare && bSunLight;

        string light = "";
        light += bTimeOfDay ? "Time of Day / " : "";
        light += bSunLight ? "Sun Light / " : "";
        light += bBackLight ? "Back Light / " : "";
        light += bShadows ? "Dynamic Shadows / " : "";
        light += bSunGlare ? "Sun Glare / " : "";
        light = light.TrimEnd(" /".ToCharArray());

        if (string.IsNullOrEmpty(light))
          light += "None";

        label_Info_Lighting.Text = light;
      }
    }

    private void UpdateDescriptions()
    {
      label_Forward.Visible = label_Forward.Enabled = checkBox_Options_Description.Checked;
	    label_ForwardMobile.Visible = label_ForwardMobile.Enabled = checkBox_Options_Description.Checked;
      label_Deferred.Visible = label_Deferred.Enabled = checkBox_Options_Description.Checked;
      label_NoRenderer.Visible = label_NoRenderer.Enabled = checkBox_Options_Description.Checked;
      label_ToneMapping.Visible = label_ToneMapping.Enabled = checkBox_Options_Description.Checked;
      // Adaptive tone mapper disabled for now([HS:#4044])
      label_AdaptiveTM.Visible = false;// label_AdaptiveTM.Enabled = checkBox_Options_Description.Checked;

      label_SSAO.Visible = label_SSAO.Enabled = checkBox_Options_Description.Checked;
      label_Glow.Visible = label_Glow.Enabled = checkBox_Options_Description.Checked;
      label_DOF.Visible = label_DOF.Enabled = checkBox_Options_Description.Checked;
      label_Lighting.Visible = label_Lighting.Enabled = checkBox_Options_Description.Checked;
      label_RimLight.Visible = label_RimLight.Enabled = checkBox_Options_Description.Checked;
      label_DepthFog.Visible = label_DepthFog.Enabled = checkBox_Options_Description.Checked;
      label_AA.Visible = label_AA.Enabled = checkBox_Options_Description.Checked;
      label_TOD.Visible = label_TOD.Enabled = checkBox_Options_Description.Checked;
      label_Sunlight.Visible = label_Sunlight.Enabled = checkBox_Options_Description.Checked;
      label_Sunglare.Visible = label_Sunglare.Enabled = checkBox_Options_Description.Checked;
      label_Shadows.Visible = label_Shadows.Enabled = checkBox_Options_Description.Checked;
      label_Backlight.Visible = label_Backlight.Enabled = checkBox_Options_Description.Checked;

      pictureBox_Forward.Visible = pictureBox_Forward.Enabled = checkBox_Options_Description.Checked;
	  pictureBox_ForwardMobile.Visible = pictureBox_ForwardMobile.Enabled = checkBox_Options_Description.Checked;
      pictureBox_Deferred.Visible = pictureBox_Deferred.Enabled = checkBox_Options_Description.Checked;
      pictureBox_NoRenderer.Visible = pictureBox_NoRenderer.Enabled = checkBox_Options_Description.Checked;
      pictureBox_TM.Visible = pictureBox_TM.Enabled = checkBox_Options_Description.Checked;
      // Adaptive tone mapper disabled for now([HS:#4044])
      pictureBox_AdaptiveTM.Visible = false; //pictureBox_AdaptiveTM.Enabled = checkBox_Options_Description.Checked;

      pictureBox_SSAO.Visible = pictureBox_SSAO.Enabled = checkBox_Options_Description.Checked;
      pictureBox_Glow.Visible = pictureBox_Glow.Enabled = checkBox_Options_Description.Checked;
      pictureBox_DOF.Visible = pictureBox_DOF.Enabled = checkBox_Options_Description.Checked;
      pictureBox_Lighting.Visible = pictureBox_Lighting.Enabled = checkBox_Options_Description.Checked;
      pictureBox_RimLight.Visible = pictureBox_RimLight.Enabled = checkBox_Options_Description.Checked;
      pictureBox_DepthFog.Visible = pictureBox_DepthFog.Enabled = checkBox_Options_Description.Checked;
      pictureBox_AA.Visible = pictureBox_AA.Enabled = checkBox_Options_Description.Checked;
      pictureBox_TOD.Visible = pictureBox_TOD.Enabled = checkBox_Options_Description.Checked;
      pictureBox_SunLight.Visible = pictureBox_SunLight.Enabled = checkBox_Options_Description.Checked;
      pictureBox_SunGlare.Visible = pictureBox_SunGlare.Enabled = checkBox_Options_Description.Checked;
      pictureBox_Shadows.Visible = pictureBox_Shadows.Enabled = checkBox_Options_Description.Checked;
      pictureBox_BackLight.Visible = pictureBox_BackLight.Enabled = checkBox_Options_Description.Checked;

      // Adaptive tone mapper disabled for now([HS:#4044])
      checkBox_AdaptiveTM.Visible = false;
    }

    private void ApplySettingsOnScene()
    {
      // Set Renderer
      if (radioButton_Forward.Checked)
        _scene.V3DLayer.RendererNodeClass = IRendererNodeManager.RENDERERNODECLASS_FORWARD;
      else if (radioButton_ForwardMobile.Checked)
        _scene.V3DLayer.RendererNodeClass = IRendererNodeManager.RENDERERNODECLASS_MOBILE_FORWARD;
      else if (radioButton_Deferred.Checked)
        _scene.V3DLayer.RendererNodeClass = IRendererNodeManager.RENDERERNODECLASS_DEFERRED;
      else if (radioButton_NoRenderer.Checked)
      {
        _scene.V3DLayer.RendererNodeClass = IRendererNodeManager.RENDERERNODECLASS_SIMPLE;
        return;
      }

      // Enable Post Processors
      ShapeComponentType[] compTypes = EditorManager.RendererNodeManager.ComponentTypes;
      _scene.V3DLayer.RendererSetup._rendererComponents.Clear();
      EditorManager.RendererNodeManager.RemovePostprocessors();
      foreach (ShapeComponentType compType in compTypes)
      {
        if (compType.Hidden) 
          continue;

        if (!EditorManager.RendererNodeManager.CanAttachPostprocessor(compType.ProbeComponent))
          continue;

        bool bEnable = false;
        if (compType.UniqueName.Equals("VPostProcessSSAO"))
          bEnable = checkBox_SSAO.Checked;
        else if (compType.UniqueName.Equals("VPostProcessGlow"))
          bEnable = checkBox_Glow.Checked;
        else if (compType.UniqueName.Equals("VPostProcessDepthOfField"))
          bEnable = checkBox_DOF.Checked;
        else if (compType.UniqueName.Equals("VPostProcessToneMapping"))
          bEnable = checkBox_TM.Checked;
        else if (compType.UniqueName.Equals("VPostProcessFXAA"))
          bEnable = checkBox_AA.Checked;
        else if (compType.UniqueName.Equals("VDeferredShadingLights"))
          bEnable = checkBox_Lighting.Checked && radioButton_Deferred.Checked;
        else if (compType.UniqueName.Equals("VDeferredShadingRimLight"))
          bEnable = checkBox_RimLight.Checked && radioButton_Deferred.Checked;
        else if (compType.UniqueName.Equals("VGlobalFogPostprocess"))
          bEnable = checkBox_DepthFog.Checked && radioButton_Deferred.Checked;

        ShapeComponent comp = (ShapeComponent)compType.CreateInstance(_scene.V3DLayer);
        comp.Active = bEnable;
        _scene.V3DLayer.RendererSetup._rendererComponents.Add(comp);
      }

      // Update renderer node
      EditorManager.RendererNodeManager.UpdateRendererNode(_scene.RendererProperties, _scene.Postprocessors);

      // Setup Lighting
      if (checkBox_TOD.Checked)
      {
        _scene.V3DLayer.EnableTimeOfDay = true;

        ShapeBase lightShape = null;
        DynLightShape sunlight = null;
        DynLightShape backlight = null;
#if !HK_ANARCHY
        SunglareShape sunglare = null;
#else
        Shape3D sunglare = null;
#endif

        if (checkBox_Sunlight.Checked)
        {
          sunlight = new DynLightShape("Sun Light");
          lightShape = sunlight;
          sunlight.LightType = LightSourceType_e.Directional;
          sunlight.Position = EditorManager.Scene.CurrentShapeSpawnPosition;
          sunlight.ExportAsStatic = false;
          sunlight.CastModelShadows = checkBox_Shadows.Checked;
          sunlight.CastWorldShadows = checkBox_Shadows.Checked;
          sunlight.MakeTimeOfDayLight(1.0f, true, null);
        }

        // Shadows
        if (checkBox_Shadows.Checked && checkBox_Sunlight.Checked)
        {
#if !HK_ANARCHY
          ShapeComponentType compType = (ShapeComponentType)EditorManager.EngineManager.ComponentClassManager.GetCollectionType("VShadowMapComponentSunlight");
          System.Diagnostics.Debug.Assert(compType != null, "Cannot create component of type VShadowMapComponentSunlight");
#else
          ShapeComponentType compType = (ShapeComponentType)EditorManager.EngineManager.ComponentClassManager.GetCollectionType("VMobileShadowMapComponentSpotDirectional");
          System.Diagnostics.Debug.Assert(compType != null, "Cannot create component of type VMobileShadowMapComponentSpotDirectional");
#endif
          if (compType != null)
          {
            ShapeComponent comp = (ShapeComponent)compType.CreateInstance(sunlight);
            sunlight.AddComponentInternal(comp);
          }
        }

        // Back light
        if (checkBox_Backlight.Checked && checkBox_Sunlight.Checked)
        {
          backlight = new DynLightShape("Back Light");
          backlight.LightType = LightSourceType_e.Directional;
          backlight.Position = EditorManager.Scene.CurrentShapeSpawnPosition;
          backlight.ExportAsStatic = false;
          backlight.CastModelShadows = false;
          backlight.CastWorldShadows = false;
          backlight.MakeTimeOfDayBackLight();
        }

#if !HK_ANARCHY 
        // Sun glare
        if (checkBox_Sunglare.Checked && checkBox_Sunlight.Checked)
        {
          sunglare = new SunglareShape("Sun Glare");
          sunglare.AttachToTimeOfDay = true;
          sunglare.Position = EditorManager.Scene.CurrentShapeSpawnPosition;
        }
#endif

        // Group light shapes
        if (backlight != null || sunglare != null)
        {
          Group3DShape group = new Group3DShape("Sunlight Shapes");
          group.Position = EditorManager.Scene.CurrentShapeSpawnPosition;
          group.AddChild(sunlight);
          if (backlight != null)
            group.AddChild(backlight);
          if (sunglare != null)
            group.AddChild(sunglare);
          lightShape = group;
        }
        
        // Add to scene
        if (lightShape != null)
          _scene.V3DLayer.AddShape(lightShape, null);
      }
      else
      {
        _scene.V3DLayer.EnableTimeOfDay = false;
      }
    }

    #endregion

    #region Scene
     
    public EditorScene Scene
    {
      get { return _scene; }
      set { _scene = value; }
    }
     
    #endregion

    #region Private Data

    private SceneWizardStep_e _step = SceneWizardStep_e.Renderer;
    private EditorScene _scene = null;

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
