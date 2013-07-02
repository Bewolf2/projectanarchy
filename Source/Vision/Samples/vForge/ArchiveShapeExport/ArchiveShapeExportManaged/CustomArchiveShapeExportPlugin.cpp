/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

// This is the main DLL file.

#include <Vision/Samples/vForge/ArchiveShapeExport/ArchiveShapeExportManaged/CustomArchiveShapeExportManagedPCH.h>

#include <Vision/Samples/vForge/ArchiveShapeExport/ArchiveShapeExportManaged/CustomArchiveShapeExportPlugin.h>

#include <Vision/Runtime/Base/VBase.hpp>
#include <Vision/Runtime/Base/System/IO/Serialization/VArchive.hpp>
#include <Vision/Runtime/Base/System/IO/Stream/VMemoryStream.hpp>

#include <Vision/Runtime/Engine/System/Vision.hpp>


using namespace Microsoft::Win32;

using namespace ManagedFramework;
using namespace CSharpFramework;
using namespace CSharpFramework::Shapes;
using namespace CSharpFramework::Scene;
using namespace CSharpFramework::Dialogs;
using namespace System::Windows::Forms;



namespace CustomArchiveShapeExportPlugin {

	/// <summary>
	/// EditorPlugin : this class must exist and implement the IEditorPluginModule functions to add the shape creators (for instance)
	/// Also, the namespace must be the same as the plugin dll name, in this case CustomArchiveShapeExportPlugin.EditorPlugin.dll
	/// This is because the editor plugin manager searches for the CustomArchiveShapeExportPlugin.EditorPlugin class when opening the 
	/// CustomArchiveShapeExportPlugin.EditorPlugin.dll assembly
	/// </summary>
  public ref class EditorPlugin : IEditorPluginModule
  {
    public:

    /// <summary>
    /// Constructor of EditorPlugin that sets up plugin information
    /// </summary>
    EditorPlugin()
    {
      _version = 0;                 //version used for serialization
      _name = "CustomArchiveShapeExportPlugin";
    }


    /// <summary>
    /// InitPluginModule : called at plugin initialisation time: Add the relevant shape creators here
    /// </summary>
    virtual bool InitPluginModule() override
    {
      //Add a new menu entry
      ToolStripMenuItem ^ samplesMenuItem = FindOrAddMenuItem(EditorManager::GUI->MainMenu->Items, "vForgeSamples");
      _traverseMenuItem = gcnew ToolStripMenuItem("Export selected shapes to archive", nullptr, gcnew EventHandler(this, &EditorPlugin::_export_Click));
      samplesMenuItem->DropDownItems->Add( _traverseMenuItem ); 
      return true;
    }

    /// <summary>
    /// DeInitPluginModule : called at plugin deinitialisation time: Remove relevant data
    /// </summary>
    virtual bool DeInitPluginModule() override
    {
      //remove menu item
      ToolStripMenuItem ^ samplesMenuItem = FindOrAddMenuItem(EditorManager::GUI->MainMenu->Items, "vForgeSamples");
      samplesMenuItem->DropDownItems->Remove(_traverseMenuItem);
      if (samplesMenuItem->DropDownItems->Count == 0) //also remove "vForgeSamples" item it is has no subitems left 
      {
        EditorManager::GUI->MainMenu->Items->Remove(samplesMenuItem);
      }
      return true;
    }


    /// <summary>
    /// Overridable that determines whether this is a standard plugin that is always loaded and initialized
    /// </summary>
    property bool IsStandardPlugin 
    {
      virtual bool get() override {return false;}
    }

    //Show the export settings dialog
    void _export_Click(Object ^sender, EventArgs ^e)
    {
      ShapeCollection ^ selectedShapes = EditorManager::SelectedShapes;
      if (selectedShapes == nullptr || selectedShapes->Count <= 0)
      {
        MessageBox::Show("Please select a shape before you execute this operation");
        return;
      }

      //add a dialog here to determine the absolute path and the location for the new archive file
      CreateFileDlg ^ fileDlg = gcnew CreateFileDlg();
      fileDlg->Caption = "Exported Custom Archive";
      fileDlg->Description = "Export selected shape to engine-readable archive file.";
      fileDlg->InitialDirectory = EditorManager::Project->ProjectDir;
      fileDlg->Ext = ".VCustomArchive";
      fileDlg->Filter = gcnew array<String^> {".VCustomArchive"};
      fileDlg->AllowOverwrite = true;

      if (fileDlg->ShowDialog() != DialogResult::OK)
        return;

      if (!StartExportShapeToArchive(selectedShapes, fileDlg->FileName))
        MessageBox::Show("The custom archive could not be saved.");
     
    }

    //helper function to find or add a menu item
    ToolStripMenuItem^ FindOrAddMenuItem(ToolStripItemCollection ^list, String ^name)
    {
      int iCount = list->Count;
      for (int i=0;i<iCount;i++)
      {
        ToolStripMenuItem ^ item = (ToolStripMenuItem^)list[i];
        if (String::Compare(item->Text, name,true)==0)
          return item;
      }
      //not found, so create menu item
      ToolStripMenuItem ^ itemToAdd = gcnew ToolStripMenuItem(name);
      list->Add(itemToAdd);
      return itemToAdd;
    }



  private: 

    ToolStripMenuItem ^ _traverseMenuItem;

    bool StartExportShapeToArchive(ShapeCollection ^ selectedShapes, String ^ fileName)
    {
      //convert path for native usage
      VString sPath;
      ConversionUtils::StringToVString(fileName, sPath);

      //create outstream that is needed for VArchive
      IVFileOutStream *pOutStream = Vision::File.Create( sPath );
      if (!pOutStream) Vision::Error.FatalError("Couldn't create file!");

      VArchive* pArchive = new VArchive( sPath, pOutStream, Vision::GetTypeManager() );

      SceneExportInfo^ sceneinfo = gcnew SceneExportInfo();
      sceneinfo->ExportType = SceneExportInfo::ExportType_e::Custom;
      sceneinfo->AbsoluteFilename = fileName;
      sceneinfo->NativeShapeArchivePtr = System::IntPtr((void*)pArchive);;

      // write out the count of the objects that get serialized
      *pArchive << selectedShapes->Count;
      // write out the selected shapes
      for each (ShapeBase ^ shapeToExport in selectedShapes)
      {
        shapeToExport->OnExport(sceneinfo);
      }

      //to load the archive again you can use the following code:
      // ar >> iCount;
      //for (int i=0;i<iCount;i++)
      //  ar.ReadObject();
      
      pArchive->Close();
      pOutStream->Close();
      delete pArchive;

      return true;
    }

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
