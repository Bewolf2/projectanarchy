/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

// ***********************************************************************************************
// Tutorial 05 : Serialization
// Copyright (C) Havok.com Inc. All rights reserved.
// ***********************************************************************************************
// How to create a custom entity class with serialization
// ***********************************************************************************************
//  This sample shows :
//   -How to make an entity class serializable
//   -How to implement a simple game load/save feature
//  See also:
//   - Tutorial 04: Custom Entities
//   - Serialization Sample (More advanced)
// ***********************************************************************************************
#include <Vision/Samples/Engine/Tutorial05/Tutorial05PCH.h>

#define MY_VERSION_MAJOR 0x01
#define MY_VERSION_MINOR 0x00

//Module to register our classes with
DECLARE_THIS_MODULE( g_MyModule, MAKE_VERSION(MY_VERSION_MAJOR, MY_VERSION_MINOR),
                     "Engine Sample", "Havok", "Vision Engine Sample", NULL );


#define ARCHIVE_START_TAG "SERIALIZE_TUTORIAL"
#define ARCHIVE_END_TAG   "END_OF_ARCHIVE"

VisEntityCollection_cl g_entityList(10);

//Save our entities to disk
bool SaveGame(const char *pszFilename)
{
  IVFileOutStream* pOut = Vision::File.Create(pszFilename);
  // creating the file didn't work!
  if (!pOut)
    return false;

  VArchive ar( pszFilename, pOut, Vision::GetTypeManager() );

  // serialize global game data
  ar << ARCHIVE_START_TAG; 		      // magic number
  
  //The engine will need to know the version when loading it's classes again
  int iEngineVersion = Vision::GetArchiveVersion();
  ar << iEngineVersion;             // engine's class version 
  
  int iSavegameVersion = 0;
  ar << iSavegameVersion;           // Our own version used for versioning the savegame data

  //Save our entities
  int iCount = g_entityList.GetNumEntries(); 
  ar << iCount; //number of entities
  for (int i=0; i<iCount; i++)
    ar << g_entityList.GetEntry(i);  //save entity (and it's dependencies)

  // store end tag - useful to verify a valid archive
  ar << ARCHIVE_END_TAG;
  
  ar.Close();
  pOut->Close();

  return true;
}

//Load our entities from disk
bool LoadGame(const char *pszFilename)
{
  int i;
  char szArchiveTag[FS_MAX_PATH];
  
  IVFileInStream* pIn = Vision::File.Open(pszFilename);
  // opening the file didn't work!
  if (!pIn)
  {
    Vision::Error.Warning("LoadGame:Can't open file %s", pszFilename);
    return false;
  } 
  
  VArchive ar( pszFilename, pIn, Vision::GetTypeManager() );

  ar.ReadStringBinary(szArchiveTag, FS_MAX_PATH);
  if (strcmp(szArchiveTag, ARCHIVE_START_TAG) != 0)
  { 
    Vision::Error.Warning("LoadGame: Invalid archive start for '%s'", pszFilename);
    pIn->Close();
    return false;
  }

  //The engine needs to know the version for loading it's classes
  ar >> i;                        // loading version
  ar.SetLoadingVersion(i);
  if (! Vision::IsValidArchive(ar))
  {
    Vision::Error.Warning("LoadGame: Invalid archive '%s'", pszFilename);
    pIn->Close();
    return false;
  }

  int iSavegameVersion = 0;
  ar >> iSavegameVersion;         // Our own version used for versioning the savegame data
  VASSERT(iSavegameVersion==0);   // only one version supported until now

  int iCount;
  ar >> iCount;
  VASSERT(iCount >=0);
  VASSERT(iCount <=65000);  //sanity check
  
  // re-create all our entities
  for (i = 0; i < iCount; i++)
  {
    VisBaseEntity_cl *pEntity;
    // get the entity from the archive...
    ar >> pEntity;
    g_entityList.AppendEntry(pEntity);
  }

  // check if the ARCHIVE_END_TAG exists
  ar.ReadStringBinary(szArchiveTag, FS_MAX_PATH);
  if (strcmp(szArchiveTag, ARCHIVE_END_TAG) != 0)
  {
    Vision::Error.Warning("No (or invalid) end tag found in save game!");
    ar.Close();
    pIn->Close();
    return false;
  }
  
  ar.Close();
  pIn->Close();
      
  return true;
}

VisSampleAppPtr spApp;

VISION_INIT
{
  VISION_SET_DIRECTORIES(false);
  // *********************************************************
  // Register the module so the engine knows about the classes
  // If you forget this, then your class will not be able to 
  // deserialize from file.
  // *********************************************************
  Vision::RegisterModule(&g_MyModule);
    
  // Create an application
  spApp = new VisSampleApp();
  
  // Init the application
  if (!spApp->InitSample("Maps\\ViewerMap" /*DataDir*/, "ViewerMap" /*SampleScene*/ ))
    return false;
  
  return true;
}

VISION_SAMPLEAPP_AFTER_LOADING
{
  // Create a mouse controlled camera
  spApp->EnableMouseCamera();
  
  bool bCreateAndSave = true;
  if (bCreateAndSave)
  {
    // Create entities of type MyEntity_cl in front of the camera
    for (int i=-1; i<=1; i++)
    {
      //Create the entity and cast it to our type
      MyEntity_cl *pEntity = (MyEntity_cl*) Vision::Game.CreateEntity("MyEntity_cl", 
                                  hkvVec3 (250.f, (float)i * 150.f ,-120), "Models\\Warrior\\Warrior.MODEL");
      VASSERT(pEntity);
      pEntity->RotateSpeed = 20.f + (float)i * 10.f;    //each entity has a different speed
      pEntity->SetCastShadows(true);
      
      g_entityList.AppendEntry(pEntity); //remember the entity
    }
    
    //Save our entities to disk
    VVERIFY(SaveGame("SerializeSample.dat"));
    
    //Delete our entities
    int iCount = g_entityList.GetNumEntries();
    for (int i=0; i<iCount; i++)
      Vision::Game.RemoveEntity( g_entityList.GetEntry(i) );
    g_entityList.Clear();
  }
  
  //Load them from disk again
  VVERIFY(LoadGame("SerializeSample.dat"));
  VASSERT(g_entityList.GetNumEntries()==3);
}
 
VISION_SAMPLEAPP_RUN
{
  // Run the main loop of the application until we quit
  return spApp->Run();
  //Entities will automatically rotate
}

VISION_DEINIT
{
  // Deinit the application
  spApp->DeInitSample();
  spApp = NULL;
  return 0;
}

VISION_MAIN_DEFAULT

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
