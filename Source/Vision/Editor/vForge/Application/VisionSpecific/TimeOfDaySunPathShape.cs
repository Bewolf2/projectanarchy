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
using CSharpFramework.Shapes;
using CSharpFramework;
using System.Runtime.Serialization;
using CSharpFramework.Scene;
using CSharpFramework.Math;

namespace Editor.VisionSpecific
{
  [Serializable]
  [ShapeSearchableAttribute(false)]
  public class TimeOfDaySunPathShape : Shape3D
  {

    public TimeOfDaySunPathShape(string name)
      : base(name)
    {
    }

    #region Shape Overrides

    TimeOfDay.DaytimeParameters daytimeParams = new TimeOfDay.DaytimeParameters();

    public override void RenderShape(VisionViewBase view, ShapeRenderMode mode)
    {
      if (TimeOfDay == null)
        return;
      const int iSteps = 24;
      float fStep = 1.0f / (float)iSteps;
      float fLen = 200.0f * EditorManager.Settings.GlobalUnitScaling;
      float fTime = 0.0f;
      Vector3F lastCorner = Vector3F.Zero;
      for (int i=0; i<iSteps; i++,fTime += fStep)
      {
        if (!EditorManager.RendererNodeManager.GetTimeOfDaySnapshot(fTime, daytimeParams))
          continue;
        Vector3F newCorner = Position + daytimeParams.SunDirection * fLen * 0.75f;
        if (i > 0)
        {
          view.RenderSolidTriangle(Position, newCorner, lastCorner, VisionColors.Get(daytimeParams.AmbientColor));
          view.RenderSolidTriangle(Position, lastCorner, newCorner, VisionColors.Get(daytimeParams.AmbientColor));
        }
        Vector3F vEnd = Position + daytimeParams.SunDirection * fLen;
        view.RenderLine(Position, vEnd, VisionColors.Get(daytimeParams.SunColor), 1.0f);
        uint iColor = (i == 12) ? VisionColors.White : VisionColors.Yellow;
        view.WriteText3D(vEnd, i.ToString()+":00", iColor); // time

        lastCorner = newCorner;
      }
    }

    public override bool CanCopyPaste
    {
      get
      {
        return false;
      }
    }

  
    /*
    public override bool Traceable
    {
      get
      {
        return false;
      }
    }
    */
    #endregion

    public TimeOfDay TimeOfDay = null;

    #region ISerializable

    /// <summary>
    /// Called when deserializing
    /// </summary>
    /// <param name="info"></param>
    /// <param name="context"></param>
    protected TimeOfDaySunPathShape(SerializationInfo info, StreamingContext context)
      : base(info, context)
    {
    }


    /// <summary>
    /// Called when serializing
    /// </summary>
    /// <param name="info"></param>
    /// <param name="context"></param>
    public override void GetObjectData(SerializationInfo info, StreamingContext context) 
    {
      base.GetObjectData(info,context);
    }

    public override void OnDeserialization()
    {
      base.OnDeserialization();
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
