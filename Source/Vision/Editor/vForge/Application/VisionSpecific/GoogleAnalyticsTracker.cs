/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

using System;
using CSharpFramework;
using CSharpFramework.Scene;
using System.IO;
using System.Globalization;
using System.Drawing;
using System.Windows.Forms;
using CSharpFramework.Shapes;
using CSharpFramework.Math;
using ManagedFramework;
using System.Collections.Generic;

#if HK_ANARCHY // TODO: Replace with inclusion tag

namespace Editor.Vision
{
  /// <summary>
  /// Implements functionality to track statistics about vForge usage
  /// </summary>
  class GoogleAnalyticsTracker
  {
    private bool _isSDKScene = false;

    private Timer _viewTrackingTimer;

    /// <summary>
    /// Constructor
    /// </summary>
    public GoogleAnalyticsTracker()
    {
      // Send Google analytics
      VisionGoogleAnalytics.Initialize();
      VisionGoogleAnalytics.SendInitialization();

      EditorManager.SceneChanged += new SceneChangedEventHandler(EditorManager_SceneChanged);
      IProject.NewProjectLoaded += new EventHandler(IProject_NewProjectLoaded);
      EditorManager.ApplicationLayout.ActiveLayoutChanged += new CSharpFramework.Layout.LayoutManager.ActiveLayoutChangedEventHandler(ApplicationLayout_ActiveLayoutChanged);

      _viewTrackingTimer = new Timer();
      _viewTrackingTimer.Interval = 10 * 60 * 1000;
      _viewTrackingTimer.Tick += new EventHandler(viewTrackingTimer_Tick);
      _viewTrackingTimer.Enabled = true;
      _viewTrackingTimer.Start();
    }

    void viewTrackingTimer_Tick(object sender, EventArgs e)
    {
      if (_isSDKScene)
        return;

      SendLayout(EditorManager.ApplicationLayout.ActiveLayout);
    }

    void ApplicationLayout_ActiveLayoutChanged(object sender, CSharpFramework.Layout.LayoutManager.ActiveLayoutChangedArgs e)
    {
      SendLayout(e.NewLayout);
    }

    void IProject_NewProjectLoaded(object sender, EventArgs e)
    {
      IProject project = EditorManager.Project;

      string sdkDir = Environment.GetEnvironmentVariable("VISION_SDK");

      _isSDKScene = false;

      if (!string.IsNullOrEmpty(sdkDir))
      {
        _isSDKScene = project.ProjectDir.StartsWith(sdkDir);
      }
      
    }

    void EditorManager_SceneChanged(object sender, SceneChangedArgs e)
    {
      if (e.newscene == null)
        return;

      CheckSceneStatistics();
    }

    void CheckSceneStatistics()
    {
      if (EditorManager.Scene == null || _isSDKScene)
        return;

      bool physicsUsed = false;
      bool aiUsed = false;
      bool behaviorUsed = false;

      int scriptedObjectCount = 0;

      Stack<ShapeBase> TempStack = new Stack<ShapeBase>();

      foreach (Layer layer in EditorManager.Scene.Layers)
      {
        TempStack.Push(layer.Root);
      }

      while (TempStack.Count > 0)
      {
        ShapeBase current = TempStack.Pop();
        foreach (ShapeBase shape in current.ChildCollection)
        {
          TempStack.Push(shape);
        }

        if (!physicsUsed && current.Components != null)
        {
          if (current.Components.GetComponentByEngineName("vHavokRigidBody") != null)
            physicsUsed = true;

          if (current.Components.GetComponentByEngineName("vHavokCharacterController") != null)
            physicsUsed = true;

          if (current.Components.GetComponentByEngineName("vHavokTriggerVolume") != null)
            physicsUsed = true;

          if (current.Components.GetComponentByEngineName("vHavokRagdoll") != null)
            physicsUsed = true;
        }

        if (!aiUsed)
        {
          if (current.GetType().Name == "HavokNavMeshShape")
            aiUsed = true;
        }

        if (!behaviorUsed && current.Components != null)
        {
          if (current.Components.GetComponentByEngineName("vHavokBehaviorComponent") != null)
            behaviorUsed = true;
        }

        if (current.Components != null && current.Components.GetComponentByEngineName("VScriptComponent") != null)
          scriptedObjectCount++;
      }

      VisionGoogleAnalytics.RecordEvent("Usage", "Havok Products", "Physics", physicsUsed ? 1 : 0);
      VisionGoogleAnalytics.RecordEvent("Usage", "Havok Products", "Behavior", behaviorUsed ? 1 : 0);
      VisionGoogleAnalytics.RecordEvent("Usage", "Havok Products", "AI", aiUsed ? 1 : 0);

      VisionGoogleAnalytics.RecordEvent("Usage", "Builtin", "Scripted Objects", scriptedObjectCount);

    }

    void SendLayout(CSharpFramework.Layout.Layout layout)
    {
      if (layout == null || _isSDKScene)
        return;

      VisionGoogleAnalytics.RecordView("/vForge/" + layout.Name);
    }



  }
}

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
