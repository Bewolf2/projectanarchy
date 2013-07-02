/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

using System;
using System.Collections;
using System.Collections.Specialized;
using System.Collections.Generic;
using System.Text;
using CSharpFramework;
using CSharpFramework.Actions;
using CSharpFramework.Shapes;
using CSharpFramework.Scene;
using CSharpFramework.Serialization;
using System.Reflection;
using CSharpFramework.Helper;
using System.IO;


namespace Editor.Shapes
{
  class VForgeShapeFactory : IShapeFactory
  {
    static public bool ThrowExceptions = true;

    /// <summary>
    /// Constructor
    /// </summary>
    public VForgeShapeFactory()
    {
      IProject.ProjectUnloaded += new EventHandler(IProject_ProjectUnloaded);
    }

    /// <summary>
    /// Destructor
    /// </summary>
    ~VForgeShapeFactory()
    {
      IProject.ProjectUnloaded -= new EventHandler(IProject_ProjectUnloaded);
    }

    void IProject_ProjectUnloaded(object sender, EventArgs e)
    {
      _cachedClassTypes.Clear(); // uncache everything since plugins might change next time
    }


    public override ShapeLink CreateShapeLink(string classname, Shape3D ownerShape, string name, string primaryID)
    {
      Type t = _cachedClassTypes.ContainsKey(classname) ? _cachedClassTypes[classname] : null;
      if (t == null)
      {
        // loop through all assemblies and find the type by name
        Assembly[] assemblies = AssemblyHelper.GetEditorAssemblies();
        foreach (Assembly assembly in assemblies)
        {
          Type[] types = null;
          try
          {
            types = assembly.GetTypes();
          }
          catch (Exception ex)
          {
            EditorManager.IgnoreException(ex);
          }
          if (types == null)
            continue;

          foreach (Type type in types)
            if (type.Name == classname && type.IsSubclassOf(typeof(ShapeLink)))
            {
              t = type;
              break;
            }
          if (t != null)
            break;
        }
        _cachedClassTypes[classname] = t; // can be null, which is also cached
      }

      if (t == null)
      {
        if (ThrowExceptions)
          throw new Exception("Unknown shape class '" + classname + "'");
        return null;
      }

      string moreFailInfo = "";
      try
      {
        object[] args = new object[3] { ownerShape, name, primaryID }; // always use this constructor
        ShapeLink newObject = Activator.CreateInstance(t, args) as ShapeLink;
        if (newObject == null)
        {
          if (ThrowExceptions)
            throw new Exception("Failed to create instance of class '" + classname + "'");
          return null;
        }
        return newObject;
      }
      catch (Exception ex)
      {
        EditorManager.DumpException(ex, false);
        moreFailInfo = "\nDetailed: ";
        if (ex.InnerException != null)
          moreFailInfo += ex.InnerException.ToString();
        else
          moreFailInfo += ex.Message;
      }

      if (ThrowExceptions)
        throw new Exception("Failed to create instance of class '" + classname + "'" + moreFailInfo);
      return null;
    }


    // for faster assembly searching
    Dictionary<string, Type> _cachedClassTypes = new Dictionary<string, Type>();

    public override Type GetTypeByName(string classname, Type baseclass, bool bThrowException)
    {
      Type t = _cachedClassTypes.ContainsKey(classname) ? _cachedClassTypes[classname] : null;
      if (t == null)
      {
        // loop through all assemblies and find the type by name
        Assembly[] assemblies = AssemblyHelper.GetEditorAssemblies();
        foreach (Assembly assembly in assemblies)
        {
          Type[] types = null;
          try
          {
            types = assembly.GetTypes();
          }
          catch (Exception ex)
          {
            EditorManager.IgnoreException(ex);
            continue;
          }
          foreach (Type type in types)
            if (type.Name == classname && (baseclass == null || baseclass == type || type.IsSubclassOf(baseclass)))
            {
              t = type;
              break;
            }
          if (t != null)
            break;
        }
        _cachedClassTypes[classname] = t; // can be null, which is also cached
      }
      if (t == null)
      {
        if (bThrowException)
          throw new Exception("Unknown class '" + classname + "'");
        return null;
      }
      return t;
    }


    public override void RecursiveParseProperties(object target, PropertyEntryCollection properties)
    {
      Type t = target.GetType();
      int iSuccess = 0;
      if (properties != null)
      {
        // match up properties from passed dictionary
        PropertyInfo[] props = t.GetProperties();
        foreach (PropertyInfo prop in props)
        {
          if (!prop.CanWrite)
            continue;
          PropertyEntry entry;
          if (!properties.TryGetValue(prop.Name, out entry))
            continue;
          if (entry == null || (!entry.HasSubProperties && string.IsNullOrEmpty(entry.ValueString)))
            continue;

          iSuccess++;

          // convert string value to object
          object newValue = SerializationHelper.GetObjectFromStringData(prop.PropertyType, entry.ValueString, ",");

          if (newValue != null)
          {
            if ((newValue is string) && !string.IsNullOrEmpty(FilenameResolvePath))
            {
              object[] isFilename = prop.GetCustomAttributes(typeof(PrefabResolveFilenameAttribute), false);
              if (isFilename != null && isFilename.Length > 0)
              {
                //ensure that only backslashes are in the string. Otherwise could have mixed back and frontslashes in the string
                newValue = ((string)newValue).Replace("/", @"\");
                newValue = Path.Combine(FilenameResolvePath, (string)newValue);
                newValue = FileHelper.ResolveFilename((string)newValue);
              }
            }

            prop.SetValue(target, newValue, null);
          } 

          // assume we have a class here:
          if (entry.SubProperties != null)
          {
            // is the property there already? otherwise create a new instance...
            object childObj = prop.GetValue(target, null);
            if (childObj == null)
            {
              throw new Exception("Prefab xml reading error: Property value is not set for '" + prop.Name + "'");
            }
            // parse the sub-property:
            RecursiveParseProperties(childObj, entry.SubProperties);
            continue; // everything OK
          }

          if (ThrowExceptions && SerializationHelper.LastException != null)
            throw SerializationHelper.LastException;
        }

        //////////////////////////////
        // The following check is for debug purposes only:
        // The number of successfully set properties should match the size of the passed property list.
        if (iSuccess != properties.Count)
        {
          string missingProperties = "";

          // At this point it is not very easy to find out *which* one was missing, so loop the other way around:
          string[] sourceKeys = properties.AllKeys;
          foreach (string key in sourceKeys)
          {
            bool bFound = false;
            bool bExact = false;
            foreach (PropertyInfo prop in props)
              if (prop.CanWrite && string.Compare(prop.Name, key, true) == 0)
              {
                bExact = string.Compare(prop.Name, key, false) == 0;
                bFound = true;
                break;
              }
            if (!bFound || !bExact)
            {
              if (!bFound)
                missingProperties += "\n" + key + " : missing";
              else if (!bExact)
                missingProperties += "\n" + key + " : Casing (warning)";
            }
          }
          EditorManager.EditorConsole.PrintError("The definition contains some invalid properties:\n" + missingProperties);
        }
        //////////////////////////////////
      }
    }

    public override ShapeBase CreateShape(string classname, string name, PropertyEntryCollection properties)
    {
      Type t = GetTypeByName(classname,typeof(ShapeBase),ThrowExceptions);
      if (t == null)
        return null;

      string moreFailInfo = "";
      try
      {
        object[] args = new object[1] { name }; // always use the "name" constructor
        ShapeBase newObject = Activator.CreateInstance(t, args) as ShapeBase;
        if (newObject == null)
        {
          if (ThrowExceptions)
            throw new Exception("Failed to create instance of class '" + classname + "'");
          return null;
        }

        RecursiveParseProperties(newObject, properties);
        return newObject;
      }
      catch (Exception ex)
      {
        EditorManager.DumpException(ex, false);
        moreFailInfo = "\nDetailed: ";
        if (ex.InnerException != null)
          moreFailInfo += ex.InnerException.ToString();
        else
          moreFailInfo += ex.Message;
      }

      if (ThrowExceptions)
        throw new Exception("Failed to create instance of class '" + classname + "'" + moreFailInfo);

      return null;
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
