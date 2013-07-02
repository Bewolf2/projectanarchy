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
using TerrainEditorPlugin.Editing;
using System.Drawing.Design;
using TerrainEditorPlugin.Shapes;
using System.ComponentModel;
using CSharpFramework.PropertyEditors;
using CSharpFramework;
using CSharpFramework.Scene;

namespace TerrainEditorPlugin.Filter
{
  [TypeConverter(typeof(PropertySorter))]
  public class ImportWeightmapFilter : ITerrainObject, ICloneable, IPropertyFlagsProvider
  {

    public ImportWeightmapFilter(TerrainShape owner)
    {
      _owner = owner;
    }

    [SortedCategory("Input", 1), PropertyOrder(1)]
    [EditorAttribute(typeof(BitmapBrowserEditor), typeof(UITypeEditor))]
    [Description("Select which texture should be used as a weightmap source.")]
    public string WeightmapFilename
    {
      get { return _filename; }
      set
      {
        _filename = value;
        _swizzleMask = TerrainManaged.EngineInstanceTerrain.QueryTextureSwizzleMask(_filename);

        if (!_swizzleMask.Contains("R"))
        {
          DestinationRed = null;
        }
        if (!_swizzleMask.Contains("G"))
        {
          DestinationGreen = null;
        }
        if (!_swizzleMask.Contains("B"))
        {
          DestinationBlue = null;
        }
        if (!_swizzleMask.Contains("A"))
        {
          DestinationAlpha = null;
        }
      }
    }

    [SortedCategory("Output", 2), PropertyOrder(1)]
    [RefreshProperties(System.ComponentModel.RefreshProperties.All)]
    [EditorAttribute(typeof(DetailTextureComboBoxEditorNoneNoBase), typeof(UITypeEditor))]
    [Description("The selected texture has a red color channel. Select to which detail texture this channel should be applied as a weightmap.")]
    public DetailTextureResource DestinationRed
    {
      get
      {
        return _destChannelRed;
      }
      set
      {
        _destChannelRed = value;
      }
    }

    [SortedCategory("Output", 2), PropertyOrder(2)]
    [RefreshProperties(System.ComponentModel.RefreshProperties.All)]
    [EditorAttribute(typeof(DetailTextureComboBoxEditorNoneNoBase), typeof(UITypeEditor))]
    [Description("The selected texture has a green color channel. Select to which detail texture this channel should be applied as a weightmap.")]
    public DetailTextureResource DestinationGreen
    {
      get
      {
        return _destChannelGreen;
      }
      set
      {
        _destChannelGreen = value;
      }
    }

    [SortedCategory("Output", 2), PropertyOrder(3)]
    [RefreshProperties(System.ComponentModel.RefreshProperties.All)]
    [EditorAttribute(typeof(DetailTextureComboBoxEditorNoneNoBase), typeof(UITypeEditor))]
    [Description("The selected texture has a blue color channel. Select to which detail texture this channel should be applied as a weightmap.")]
    public DetailTextureResource DestinationBlue
    {
      get
      {
        return _destChannelBlue;
      }
      set
      {
        _destChannelBlue = value;
      }
    }

    [SortedCategory("Output", 2), PropertyOrder(4)]
    [RefreshProperties(System.ComponentModel.RefreshProperties.All)]
    [EditorAttribute(typeof(DetailTextureComboBoxEditorNoneNoBase), typeof(UITypeEditor))]
    [Description("The selected texture has an alpha channel. Select to which detail texture this channel should be applied as a weightmap.")]
    public DetailTextureResource DestinationAlpha
    {
      get
      {
        return _destChannelAlpha;
      }
      set
      {
        _destChannelAlpha = value;
      }
    }

    #region IPropertyFlagsProvider Members

    public virtual PropertyFlags_e GetPropertyFlags(PropertyFlagsProviderInfo pd)
    {
      if (pd.Name == "DestinationRed" && !_swizzleMask.Contains("R"))
      {
        return PropertyFlags_e.Hidden;
      }
      if (pd.Name == "DestinationGreen" && !_swizzleMask.Contains("G"))
      {
        return PropertyFlags_e.Hidden;
      }
      if (pd.Name == "DestinationBlue" && !_swizzleMask.Contains("B"))
      {
        return PropertyFlags_e.Hidden;
      }
      if (pd.Name == "DestinationAlpha" && !_swizzleMask.Contains("A"))
      {
        return PropertyFlags_e.Hidden;
      }

      return PropertyFlags_e.None;
    }

    public virtual List<PropertyDescriptor> GetAdditionalRootProperties(ITypeDescriptorContext context)
    {
      return null;
    }

    #endregion

    #region ITerrainObject Members

    [Browsable(false)]
    public TerrainShape OwnerTerrain
    {
      get { return _owner; }
      set { _owner=value; }
    }

    #endregion

    #region ICloneable Members

    public object Clone()
    {
      ImportWeightmapFilter copy = (ImportWeightmapFilter)this.MemberwiseClone();
      return copy;
    }

    #endregion

    #region Private Members

    TerrainShape _owner;
    string _filename = "";
    string _swizzleMask = "";
    DetailTextureResource _destChannelRed = null;
    DetailTextureResource _destChannelGreen = null;
    DetailTextureResource _destChannelBlue = null;
    DetailTextureResource _destChannelAlpha = null;

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
