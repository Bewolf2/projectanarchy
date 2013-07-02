/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

// FmodManaged.cpp : Defines the entry point for the DLL application.
//

#include <Vision/Editor/vForge/EditorPlugins/ThirdParty/FmodPlugin/FmodManaged/FmodManagedPCH.h>
#include <Vision/Editor/vForge/EditorPlugins/ThirdParty/FmodPlugin/FmodManaged/EngineInstances/EngineInstanceFmodIncludes.hpp>

#include <tlhelp32.h>

#if (_MSC_VER < 1400)  //MSVC 8.0
  // Microsoft Knowledge Base 814472
  #include "_vcclrit.h"
#endif

using namespace ManagedFramework;
using namespace CSharpFramework;


VIMPORT IVisPlugin_cl* GetEnginePlugin_vFmodEnginePlugin();

namespace FmodManaged
{
  public ref class ManagedModule
  {
  public:
  
    static void InitManagedModule()
    {
#if (_MSC_VER < 1400)  //pre MSVC 8.0
      // Microsoft Knowledge Base 814472
      __crt_dll_initialize();
#endif
      //Init the entity plugin directly since we link statically to it
      GetEnginePlugin_vFmodEnginePlugin()->InitEnginePlugin();

      s_iEventGroupNodeImageIndex = EditorManager::GUI->ShapeTreeImages->AddBitmap(System::IO::Path::Combine(EditorManager::AppDataDir, "bitmaps\\Shapes\\shapes_group.bmp.bmp"), System::Drawing::Color::Magenta);
      s_iEventNodeImageIndex = EditorManager::GUI->ShapeTreeImages->AddBitmap(System::IO::Path::Combine(EditorManager::AppDataDir, "bitmaps\\Shapes\\event_loudspeaker.bmp"), System::Drawing::Color::Magenta);
    }

    static void DeInitManagedModule()
    {
      //Deinit the entity plugin directly since we link statically to it
      GetEnginePlugin_vFmodEnginePlugin()->DeInitEnginePlugin();

#if (_MSC_VER < 1400)  //pre MSVC 8.0
      // Microsoft Knowledge Base 814472
      __crt_dll_terminate();
#endif
    }

    // recursive helper function to fill up a event-group tree
    static void GetEventSubGroup(FMOD::EventGroup *pEventGroup, System::Windows::Forms::TreeNodeCollection ^nodes, bool bAddEvents)
    {
      int iNumGroups = 0;
      pEventGroup->getNumGroups(&iNumGroups);
      if (iNumGroups==0)
      {
        if (bAddEvents)
        {
          int iNumEvents = 0;
          pEventGroup->getNumEvents(&iNumEvents);
          for (int i=0;i<iNumEvents;i++)
          {
            FMOD::Event *pEvent = NULL;
            pEventGroup->getEventByIndex(i, FMOD_EVENT_INFOONLY, &pEvent);
            if (pEvent)
            {
              char *szName = NULL;
              pEvent->getInfo(NULL, &szName, NULL);
              System::String ^sName = ConversionUtils::VStringToString(szName);
              System::Windows::Forms::TreeNode ^node = nodes->Add(sName);
              node->ImageIndex = s_iEventNodeImageIndex;
              node->SelectedImageIndex = s_iEventNodeImageIndex;
            }
          }
        }
        return;
      }

      for (int i=0;i<iNumGroups;i++)
      {
        FMOD::EventGroup *pEventSubGroup = NULL;
        pEventGroup->getGroupByIndex(i, false, &pEventSubGroup); 
        if (pEventSubGroup)
        {
          int iIndex = 0;
          char *szName = NULL;
          pEventSubGroup->getInfo(&iIndex, &szName);
          System::String ^sName = ConversionUtils::VStringToString(szName);
          System::Windows::Forms::TreeNode ^node = nodes->Add(sName);
          node->ImageIndex = s_iEventGroupNodeImageIndex;
          node->SelectedImageIndex = s_iEventGroupNodeImageIndex;
          GetEventSubGroup(pEventSubGroup, node->Nodes, bAddEvents);
        }
      }
    }

    // gets complete event-group tree for corresponding event-project
    static bool GetEventGroupTree(System::String ^eventProjectPath, System::Windows::Forms::TreeView ^eventGroupTree)
    {
      FMOD::EventSystem *pEventSystem = VFmodManager::GlobalManager().GetFmodEventSystem();
      VASSERT(pEventSystem != NULL);
      
      VString sEventProjectPath;
      ConversionUtils::StringToVString(eventProjectPath, sEventProjectPath);

      // First check, whether event project has been loaded. 
      FMOD::EventProject *pEventProject = NULL; 
      const char *szEventProjectName = VPathHelper::GetFilename(sEventProjectPath);
      pEventSystem->getProject(szEventProjectName, &pEventProject);

      // If event project has not been loaded, then try to load it now. First try the to load
      // the platform-specific exported event project. When that fails, then try to load the 
      // platform-nonspecific version.
      if (!pEventProject)
      {
        VStaticString<FS_MAX_PATH> sEventProjectPathExt(sEventProjectPath);
        VPathHelper::GetFileDir(sEventProjectPath, sEventProjectPathExt);
        if(!sEventProjectPathExt.IsEmpty())
          sEventProjectPathExt += "/";
        sEventProjectPathExt += VFMOD_PLATFORM_STR; 
        sEventProjectPathExt += "/";
        sEventProjectPathExt += szEventProjectName;
        sEventProjectPathExt += ".fev"; // add .fev file-extension
        pEventSystem->load(sEventProjectPathExt, 0, &pEventProject);
        if (!pEventProject)
        {
          sEventProjectPathExt = sEventProjectPath; 
          sEventProjectPathExt += ".fev"; // add .fev file-extension  
          pEventSystem->load(sEventProjectPathExt, 0, &pEventProject);
          if (!pEventProject)
            return false;
        }
      }

      if(eventGroupTree != nullptr)
      {
        int iNumGroups = 0;
        pEventProject->getNumGroups(&iNumGroups);
        for(int i=0; i<iNumGroups; i++)
        {
          FMOD::EventGroup *pEventGroup = NULL;
          pEventProject->getGroupByIndex(i, false, &pEventGroup);
          if (pEventGroup)
          {
            int iIndex = 0;
            char *szName = NULL;;
            pEventGroup->getInfo(&iIndex, &szName);
            System::String ^sName = ConversionUtils::VStringToString(szName);
            System::Windows::Forms::TreeNode ^node = eventGroupTree->Nodes->Add(sName);
            node->ImageIndex = s_iEventGroupNodeImageIndex;
            node->SelectedImageIndex = s_iEventGroupNodeImageIndex;
            GetEventSubGroup(pEventGroup, node->Nodes, false);
          }
        }
      }
     
      return true;
    }

    // gets event-group tree and as leaf nodes the actual events for the corresponding event-group
    static bool GetEventTree(System::String ^eventProjectPath, System::String ^eventGroupName, System::Windows::Forms::TreeView ^eventGroupTree)
    {
      FMOD::EventSystem *pEventSystem = VFmodManager::GlobalManager().GetFmodEventSystem();
      VASSERT(pEventSystem != NULL);

      VString sEventProjectPath;
      ConversionUtils::StringToVString(eventProjectPath, sEventProjectPath);
      VString sEventGroupName;
      ConversionUtils::StringToVString(eventGroupName, sEventGroupName);

      FMOD::EventGroup *pEventGroup = NULL;
      const char *szEventProjectName = VPathHelper::GetFilename(sEventProjectPath);
      VStaticString<FS_MAX_PATH> sEventGroupPath(szEventProjectName);
      sEventGroupPath += "/";
      sEventGroupPath += sEventGroupName;
      pEventSystem->getGroup(sEventGroupPath, false, &pEventGroup);
      if (!pEventGroup)
        return false;
   
      if (eventGroupTree != nullptr)
        GetEventSubGroup(pEventGroup, eventGroupTree->Nodes, true);   

      return true; 
    }

    // check, whether Fmod Designer tool is already running
    static bool FindDesignerProcess()
    {
       return FindProcess(VFMOD_DESIGNER_EXE);
    }

    // launch Fmod Designer tool
    static void LaunchDesigner(System::String ^eventProjectPath)
    {
      VString sDesignerPath;
      if (!VFmodManager::GlobalManager().GetDesignerPath(sDesignerPath))
        return;
        
      STARTUPINFO si;
      PROCESS_INFORMATION pi;
      memset(&si, 0, sizeof(STARTUPINFO));
      memset(&pi, 0, sizeof(PROCESS_INFORMATION));
      si.cb = sizeof(STARTUPINFO);

      VString sCmdLine;
      if (eventProjectPath == nullptr)
      {
        sCmdLine.Format("\"%s\"", sDesignerPath.AsChar());
      }
      else
      {
        VString sEventProjectPath;
        ConversionUtils::StringToVString(eventProjectPath, sEventProjectPath);
        sCmdLine.Format("\"%s\" \"%s\"", sDesignerPath.AsChar(), sEventProjectPath.AsChar());
      } 
    
      ANALYSIS_IGNORE_WARNING_ONCE(6335) 
      if(!CreateProcess(NULL, sCmdLine.GetChar(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
      {
        MessageBox(NULL, "Could not launch Fmod Designer.\n", "Launch Error", MB_OK | MB_ICONEXCLAMATION);
      }
    }

    // get native reverb preset
    static FmodReverbProps^ GetReverbPreset(FmodReverbPresets_e presetType)
    {
      VFmodReverbProps reverbPresets[] = VFMOD_ALL_REVERBPRESETS;
      return gcnew FmodReverbProps(reverbPresets[(int)presetType], presetType);
    }
 
  private:
    static bool FindProcess(const char *szProcessName)
    {
      PROCESSENTRY32 entry;
      entry.dwSize = sizeof(PROCESSENTRY32);

      HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
      if (!Process32First(snapshot, &entry)) 
        return false;
      do
      {
        if (_stricmp(entry.szExeFile, szProcessName) == 0)
        {
          HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, entry.th32ProcessID);
          if (hProcess)
          {
            CloseHandle(hProcess);
            CloseHandle(snapshot);
            return true;
          }
        }
      }
      while (Process32Next(snapshot, &entry));
      CloseHandle(snapshot);
      return false;
    }

    static int s_iEventGroupNodeImageIndex; 
    static int s_iEventNodeImageIndex; 

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
