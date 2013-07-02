/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef VLUA_APIDOC

%nodefaultctor VisGame_cl;
%nodefaultdtor VisGame_cl;
class VisGame_cl
{
public:
  
  // create engine objects

  %extend{
    
    //implemented native:
    //VisBaseEntity_cl * CreateEntity(const hkvVec3* pPos, const char *szClassName = "VisBaseEntity_cl", const char *szModelFile = NULL , const char* szKey = NULL, const char *szVarString = NULL)
    
    VisLightSource_cl * CreateLight(const hkvVec3* pPos, int iType = LIGHT_POINT, float fRadius = 100, const char* szKey = NULL)
    {
      if(pPos!=NULL)
      {
        VisLightSource_cl *pLight = Vision::Game.CreateLight(*pPos, (VisLightSourceType_e) iType, fRadius);
        if(szKey!=NULL)
          pLight->SetObjectKey(szKey);
        return pLight;
      } 
      return NULL;
    }

    VisPath_cl * CreatePath(int iNodes = -1, bool bClosed = false, const char * szKey = NULL)
    {
      if(iNodes==-1)
        return new VisPath_cl();
      return new VisPath_cl(iNodes, bClosed, szKey);
    }
    
    VisPathNode_cl * CreatePathNode(const hkvVec3* pPos, const char * szKey = NULL)
    {
      if(pPos!=NULL)
      {
        VisPathNode_cl * pNode = new VisPathNode_cl(*pPos);
        if(szKey!=NULL)
          pNode->SetObjectKey(szKey);
        return pNode;
      }
      return NULL;
    }
    
    VisParticleEffect_cl * CreateEffect(const hkvVec3*  pPos, const char * szEffectFile, const char * szKey = NULL)
    {
      if(pPos!=NULL && szEffectFile!=NULL)
      {
        VisParticleEffectFile_cl *pResource = VisParticleGroupManager_cl::GlobalManager().LoadFromFile(szEffectFile);
        if(pResource==NULL)
          return NULL;
        
        VisParticleEffect_cl *pEffect = pResource->CreateParticleEffectInstance(*pPos, hkvVec3::ZeroVector());
        if(szKey!=NULL)
          pEffect->SetObjectKey(szKey);
        return pEffect;
      }
      return NULL;
    }
    
    VisStaticMeshInstance_cl * CreateStaticMeshInstance(const hkvVec3* pPos, const char * szMeshFile, bool bNotifyPhysics = true, const char * szKey = NULL)
    {
      if(pPos!=NULL && szMeshFile!=NULL)
      {
        // load the resource (will return existing resource if loaded already)
        VisStaticMesh_cl *pRes = VisStaticMesh_cl::GetResourceManager().LoadStaticMeshFile(szMeshFile);
        if(pRes==NULL)
          return NULL;
        
        // create an instance
        hkvMat4 transform;
        transform.setTranslation(*pPos);
        VisStaticMeshInstance_cl *pMeshInst = pRes->CreateInstance(transform, NULL, bNotifyPhysics);
        
        if(pMeshInst!=NULL && szKey!=NULL)
          pMeshInst->SetObjectKey(szKey);
        
        return pMeshInst;
      }
      return NULL;
    }
    
    VDynamicMesh * CreateDynamicMesh(const char * szDynamicMeshFile, bool bForceLoad, bool bLoadAnim=true)
    {
      if(szDynamicMeshFile!=NULL)
      {
        return Vision::Game.LoadDynamicMesh(szDynamicMeshFile, bForceLoad, bLoadAnim);
      }
      return NULL;
    }
	
    %apply (SWIGTYPE** INPUT, int) {(VDynamicMesh** ppMeshesToMerge, int iCount)};
    VDynamicMesh * MergeMeshes(VDynamicMesh ** ppMeshesToMerge, int iCount)
    {
      if(ppMeshesToMerge==NULL)
      {
        Vision::Error.Warning("[Lua] MergeDynamicMesh: No model meshes specified!");
        return NULL;
      }
      
      if(iCount<2||iCount>32)
      {
        Vision::Error.Warning("[Lua] MergeDynamicMesh: Cannot not merge %u mesh files. Range must be between 2 and 32!", iCount);
        return NULL;
      }

      MeshMergeInfo_t info[32];
      unsigned int uiBitmask = 0;
      
      for(int i=0;i<iCount;++i)
      {
        info[i].m_pMesh = ppMeshesToMerge[i];
        uiBitmask <<= info[i].m_pMesh==NULL ? 0 : 1;
      }
      
      VMeshManager &modelManager(VDynamicMesh::GetResourceManager());
      VString sName(32);
      sName.Format("LuaMergedModel%1.2f[%p]", Vision::GetTimer()->GetTime(), ppMeshesToMerge);
      VDynamicMesh *pMergedModel = modelManager.MergeDynamicMeshes(sName.AsChar(), info, uiBitmask);
      
      if(pMergedModel==NULL)
      {
        Vision::Error.Warning("[Lua] MergeDynamicMesh: Failed not merge %u mesh files!", iCount);
        return NULL;
      }
      
      return pMergedModel;
    }
    
    %apply (const char** INPUT, int) {(const char** pszMeshesToMerge, int iCount)};
    VDynamicMesh * MergeMeshesByString(const char** pszMeshesToMerge, int iCount)
    {
      if(pszMeshesToMerge==NULL)
      {
        Vision::Error.Warning("[Lua] MergeDynamicMesh: No model mesh strings specified!");
        return NULL;
      }
      
      if(iCount<2||iCount>32)
      {
        Vision::Error.Warning("[Lua] MergeDynamicMesh: Cannot not merge %u mesh files. Range must be between 2 and 32!", iCount);
        return NULL;
      }

      MeshMergeInfo_t info[32];
      unsigned int uiBitmask = 0;
      
      for(int i=0;i<iCount;++i)
      {
        info[i].m_pMesh = Vision::Game.LoadDynamicMesh(pszMeshesToMerge[i], true);
        uiBitmask <<= info[i].m_pMesh==NULL ? 0 : 1;
      }
      
      VMeshManager &modelManager(VDynamicMesh::GetResourceManager());
      VString sName(32);
      sName.Format("LuaMergedModel%1.2f[%p]", Vision::GetTimer()->GetTime(), pszMeshesToMerge);
      VDynamicMesh *pMergedModel = modelManager.MergeDynamicMeshes(sName.AsChar(), info, uiBitmask);
      
      if(pMergedModel==NULL)
      {
        Vision::Error.Warning("[Lua] MergeDynamicMesh: Failed not merge %u mesh files!", iCount);
        return NULL;
      }
      
      return pMergedModel;
    }
    
    VTextureObject * CreateTexture(const char * szTextureFile, bool b3DTexture=false)
    {
      if(szTextureFile!=NULL)
      {
        if(b3DTexture) return Vision::TextureManager.Load3DTexture(szTextureFile);
        else           return Vision::TextureManager.Load2DTexture(szTextureFile);
      }
      return NULL;
    }
    
    VisContextCamera_cl * CreateCamera(bool bReplaceMainRenderContextCamera = true, const char * szKey = NULL)
    {
      VisContextCamera_cl *pCamera = new VisContextCamera_cl();
      if(bReplaceMainRenderContextCamera)
        VisRenderContext_cl::GetMainRenderContext()->SetCamera(pCamera);
        
      if(szKey!=NULL)
        pCamera->SetObjectKey(szKey);
        
      return pCamera;
    }
    
    VisScreenMask_cl * CreateScreenMask(float x, float y, const char * szTexturePath, const char * szKey = NULL)
    {
      if(szTexturePath==NULL)
        return NULL;
        
      VisScreenMask_cl *pMask = new VisScreenMask_cl(szTexturePath);
      
      pMask->SetPos(x,y);
        
      if(szKey!=NULL)
        pMask->SetKey(szKey);
        
      return pMask;
    }
    
  }

  // "getters"

  %extend{
    VisLightSource_cl * GetLight(const char *szKey)
    {
      if(szKey==NULL)
        return NULL;
      return VisLightSource_cl::FindByKey(szKey, NULL);
    }
    
    VisPath_cl * GetPath(const char * szKey)
    {
      if(szKey==NULL)
        return NULL;
      return VisPath_cl::FindByKey(szKey,NULL);
    }
    
    VisParticleEffect_cl * GetEffect(const char * szKey)
    {
      if(szKey==NULL)
        return NULL;
      return VisParticleGroupManager_cl::GlobalManager().Instances().FindByKey(szKey,NULL);
    }
    
    VisStaticMeshInstance_cl * GetStaticMeshInstance(const char * szKey)
    {
      if(szKey==NULL)
        return NULL;
      return VisStaticMeshInstance_cl::FindByKey(szKey,NULL);
    }
    
    VDynamicMesh * GetDynamicMesh(const char * szDynamicMeshFile)
    {
      if(szDynamicMeshFile==NULL)
        return NULL;
      return VDynamicMesh::FindDynamicMesh(szDynamicMeshFile);
    }
    
    VisContextCamera_cl * GetCamera(const char * szKey = NULL)
    {
      if(szKey==NULL)
        return VisRenderContext_cl::GetMainRenderContext()->GetCamera();
      return VisContextCamera_cl::FindByKey(szKey, NULL);
    }
    
    VisScreenMask_cl * GetScreenMask(const char * szKey)
    {
      if(szKey==NULL)
        return NULL;
      return VisScreenMask_cl::FindByKey(szKey,NULL);
    }

    CubeMapHandle_cl * GetCubeMap(const char * szKey)
    {
      if(szKey==NULL)
        return NULL;

      return vdynamic_cast<CubeMapHandle_cl*>(CubeMapHandle_cl::FindByKey(szKey,NULL));
    }
  }

  %extend{
  
    // "deleteAll"

    void DeleteAllUnrefScreenMasks()
    {
      VisScreenMask_cl::ElementManagerDeleteAllUnRef();
    }
    
    void DeleteAllUnrefLights()
    {
      VisLightSource_cl::ElementManagerDeleteAllUnRef();
    }
    
    void DeleteAllUnrefPaths()
    {
      VisPath_cl::ElementManagerDeleteAllUnRef();
    }
    
    void DeleteAllUnrefStaticMeshInstances()
    {
      VisStaticMeshInstance_cl::ElementManagerDeleteAllUnRef();
    }
    
    void DeleteAllUnrefCameras()
    {
      VisContextCamera_cl::ElementManagerDeleteAllUnRef();
    }

    // execute an action -> maybe part of the console wrapper..?
  
    bool ExecuteAction(const char * szAction)
    {
      VActionManager *pManager = Vision::GetActionManager();
      return pManager->Execute(szAction, FALSE) == TRUE;
    }
  }
  
};

//Implement this method native in order to return the concrete
//type instance instead of VisBaseEntity_cl
%native(VisGame_cl_CreateEntity) int VisGame_cl_CreateEntity(lua_State *L);
%{
  SWIGINTERN int VisGame_cl_CreateEntity(lua_State *L)
  {
    IS_MEMBER_OF(VisGame_cl) //this will move this function to the method table of the specified class

    //param #1: we do not need the first parameter at all ;)

    //param #2: the position where to spawn the entity
    SWIG_CONVERT_POINTER(L, 2, hkvVec3, pPos)    
    
    //param #3: optional class name
    const char * szClassName = lua_isstring(L, 3) ? lua_tostring(L, 3) : "VisBaseEntity_cl";
    
    //param #4: optional model file name
    const char * szModelFile = lua_isstring(L, 4) ? lua_tostring(L, 4) : NULL;
    
    //param #5: optional entity key
    const char * szKey = lua_isstring(L, 5) ? lua_tostring(L, 5) : NULL;
    
    //param #6: optional var string
    const char * szVarString = lua_isstring(L, 6) ? lua_tostring(L, 6) : NULL;

    //clean stack (remove all call params including self)
    lua_settop(L, 0);                              //stack: TOP
    
    VisBaseEntity_cl *pEntity = NULL;
    if(pPos)
    {
      pEntity = Vision::Game.CreateEntity(szClassName, *pPos, szModelFile, szVarString);
      if(pEntity!=NULL && szKey) pEntity->SetObjectKey(szKey);
    }
    
    //will handle NULL as well
    LUA_PushObjectProxy(L, pEntity);              //stack: result, TOP
    
    return 1;
  }
%}

//Implement this method native in order to return the concrete type instance
//instead of the interface IVObjectComponent
%native(CreateComponent) int VisGame_cl_CreateComponent(lua_State *L);
%{
  SWIGINTERN int VisGame_cl_CreateComponent(lua_State *L)
  {
    IS_MEMBER_OF(VisGame_cl) //this will move this function to the method table of the specified class

    //param #1: we do not need the first parameter at all ;)
    
    //param #2: type name of the component
    if(!SWIG_lua_isnilstring(L,2))
    {
      luaL_error(L, "Expected a string value as parameter 2 for VisGame_cl_CreateComponent");
      lua_pushnil(L);                                     //stack: ..., nil
      return 1;                                           //return 1 stack item (nil)
    }
    
    const char * szComponentType = lua_tostring(L, 2);       
    
    //param #3: optional name of the component
    const char * szComponentName = NULL;
    if(lua_isstring(L, 3))  szComponentName = lua_tostring(L, 3);
          
    lua_settop(L, 0); //remove all items from the stack
        
    // Search for a component type with the specified name
    VType *pType = Vision::GetTypeManager()->GetType(szComponentType);
    if (pType==NULL)
    {
      Vision::Error.Warning("[Lua] CreateComponent: did not find component of the specified type: %s", szComponentType);
      lua_pushnil(L);
      return 1;
    }

    // Create an instance of that component type
    IVObjectComponent *pComponent = (IVObjectComponent *)pType->CreateInstance();
    if (pComponent==NULL)
    {
      Vision::Error.Warning("[Lua] CreateComponent: Failed construction an instance of the specified component type: %s", szComponentType);
      lua_pushnil(L);
      return 1;
    }

    if(szComponentName) pComponent->SetComponentName(szComponentName);
    
    //will handle NULL as well
    LUA_PushObjectProxy(L, pComponent); //stack: cmpnt, TOP

    //we return the component...
    return 1;
  }
%}

%native(VisGame_cl_WaitSeconds) int VisGame_cl_WaitSeconds(lua_State *L);
%{
  SWIGINTERN int VisGame_cl_WaitSeconds(lua_State *L)
  {
    IS_MEMBER_OF(VisGame_cl) //this will move this function to the method table of the specified class

    DECLARE_ARGS_OK;

    //we can ignore arg1 because it is a static function

    GET_ARG(2, float, fSec);
    
    if(ARGS_OK)
    {
      //Find out which script instance we are calling this from
      VScriptInstance*  pScript = VScriptResourceManager::GetScriptInstanceForState(L);
      VASSERT(pScript);
      
      pScript->WaitSeconds(L, fSec);
    }
    
    return lua_yield(L, 0);
  }
%}

//Implement this method native in order to return the concrete type instance
//instead of the base class VisObject3D_cl
%native(VisGame_cl_GetEntity) int VisGame_cl_GetEntity(lua_State *L);
%{
  SWIGINTERN int VisGame_cl_GetEntity(lua_State *L)
  {
    IS_MEMBER_OF(VisGame_cl) //this will move this function to the method table of the specified class

    if(!lua_isstring(L,2))
    {
      luaL_error(L, "Expected a string value as parameter 2 for VisGame_cl_GetEntity");
      lua_pushnil(L);                                     //stack: ..., nil
      return 1;                                           //return 1 stack item (nil)
    }
    
    const char * pszKey = lua_tostring(L, 2);       
    
    VisBaseEntity_cl * pObj = Vision::Game.SearchEntity(pszKey);
      
    LUA_PushObjectProxy(L, pObj); //will handle NULL as well

    return 1;
  }
%}

//Implement this method native in order to return the concrete type instance
//instead of the base class VisObject3D_cl
%native(VisGame_cl_GetObject) int VisGame_cl_GetObject(lua_State *L);
%{
  SWIGINTERN int VisGame_cl_GetObject(lua_State *L)
  {
    IS_MEMBER_OF(VisGame_cl) //this will move this function to the method table of the specified class

    if(!lua_isstring(L,2))
    {
      luaL_error(L, "Expected a string value as parameter 2 for VisGame_cl_GetObject");
      lua_pushnil(L);                                     //stack: ..., nil
      return 1;                                           //return 1 stack item (nil)
    }
    
    const char * pszKey = lua_tostring(L, 2);       
    
    VisTypedEngineObject_cl * pObj;
    pObj = Vision::Game.SearchEntity(pszKey);
    if (!pObj) pObj = Vision::Game.SearchLightSource(pszKey);
    if (!pObj) pObj = VisParticleGroupManager_cl::GlobalManager().Instances().FindByKey(pszKey);
    if (!pObj) pObj = Vision::Game.SearchPath(pszKey);
    if (!pObj) pObj = VisContextCamera_cl::FindByKey(pszKey);
    if (!pObj) pObj = VisStaticMeshInstance_cl::FindByKey(pszKey);
    if (!pObj)
    {
      //since VisScreenMask_cl is not a typed engine object we cannot use LUA_PushObjectProxy
      VisScreenMask_cl * pMask = VisScreenMask_cl::FindByKey(pszKey);
      if(pMask)
      {
        VSWIG_Lua_NewPointerObj(L, pMask, SWIGTYPE_p_VisScreenMask_cl, VLUA_MANAGE_MEM_MANUAL);
      }
      else
      {
        lua_pushnil(L);
      }
      return 1;
    }
      
    LUA_PushObjectProxy(L, pObj); //will handle NULL as well

    return 1;
  }
%}

//Implement this method native in order to return the concrete type instance
//instead of the base class VisObject3D_cl
%native(VisGame_cl_InstantiatePrefab) int VisGame_cl_InstantiatePrefab(lua_State *L);
%{
  SWIGINTERN int VisGame_cl_InstantiatePrefab(lua_State *L)
  {
    IS_MEMBER_OF(VisGame_cl) //this will move this function to the method table of the specified class
  
    //param #1: we do not need the first parameter at all ;)

    //param #2: the position where to spawn the prefab
    SWIG_CONVERT_POINTER(L, 2, hkvVec3, pPos)    
    
    //param #3: the prefab file
    const char * szFilename = lua_isstring(L, 3) ? lua_tostring(L, 3) : NULL;
    
    //param #4: the optional parent entity
    VisObject3D_cl *pParent = NULL;
    if(!lua_isnil(L,4) && SWIG_isptrtype(L, 4))
    {
      if( !SWIG_IsOK(SWIG_ConvertPtr(L, 4,(void**)&pParent, SWIGTYPE_p_VisObject3D_cl,0)) )
      {
        luaL_error(L, "Unable to convert parent paremter 4 to VisObject3D_cl");
        lua_pushnil(L);                                     //stack: ..., nil
        return 1;                                           //return 1 stack item (nil)
      }
    }
  
    VPrefab* pPrefab = VPrefabManager::GlobalManager().LoadPrefab( szFilename );
    if(pPrefab==NULL)
    {
      Vision::Error.Warning("[Lua] Failed to load prefab '%s' in VisGame_cl_InstantiatePrefab", szFilename);
      lua_pushnil(L);                                       //stack: ..., nil
      return 1;                                             //return 1 stack item (nil)
    }
    
    // information to instantiate
    VPrefabInstanceInfo info;
    info.m_vInstancePos = *pPos;
    info.m_vInstanceEuler = hkvVec3(0.0f,0.0f,0.0f);
    info.m_pParentObject = pParent;
    info.m_bOutputInstances = true;
    
    if(pPrefab->Instantiate(info)==FALSE)
    {
      lua_pushnil(L);                                       //stack: ..., nil
      return 1;                                             //return 1 stack item (nil)
    }
    
    //everything looks alright
    lua_newtable(L);                                        //stack: ..., table

    //push the objects on the stack
    for(unsigned int i = 0; i < info.m_Instances.GetValidSize(); ++i )
    {
      if( info.m_Instances[i]->IsOfType(V_RUNTIME_CLASS(VTypedObject)) )
      {
        LUA_PushObjectProxy(L, (VTypedObject *)info.m_Instances[i]);
                                                            //stack: ..., table, object
        lua_rawseti(L, -2, i+1);                            //stack: ..., table
      }
    }
    
    return 1;                                               //return 1 stack item (table)
  }
%}

#else

/// \brief
///   Class for game related functions like entity creation. The global instance in Lua is called \b Game.
/// \par Example
///   \code
///     local entity = Game:CreateEntity(Vision.hkvVec3(10,23,0), "VisBaseEntity_cl", "Models/Warrior/Warrior.model", "MyHero")
///   \endcode
class VisGame_cl {
public:

  /// @name Create Game Objects
  /// @{
  
  /// \brief Create an entity (VisBaseEntity_cl or derrived).
  /// \param pos The position where to create the entity.
  /// \param className (\b optional) The class name of the entity instance.
  /// \param modelFile (\b optional) The model file name.
  /// \param key (\b optional) The object key, which can be used to search for the entity ( Game:GetEntity(key) ).
  /// \param varString (\b optional) Variable string which define a set of properties of the entity (see VisBaseEntity_cl::SetVariablesByString).
  /// \return An entity instance or nil.
  /// \par Example
  ///   \code
  ///     local entity = Game:CreateEntity(Vision.hkvVec3(10,23,0), "VisBaseEntity_cl", "Models/Warrior/Warrior.model", "MyHero")
  ///   \endcode
  /// \see GetEntity
  mixed CreateEntity(hkvVec3 pos, string className = "VisBaseEntity_cl", string modelFile = nil , string key = nil, string varString = nil);
  
  /// \brief Create a light source.
  /// \param pos The position where to create the entity.
  /// \param type (\b optional) The tyoe of light source, possible vlueas are:
  ///  - LIGHT_POINT : A point light.
  ///  - LIGHT_DIRECTED : A directional light.
  ///  - LIGHT_SPOT: A spot light.
  /// \param radius (\b optional) The radius of the light source.
  /// \param key (\b optional) The object key, which can be used to search for the light ( Game:GetLight(key) ).
  /// \return A light source instance.
  /// \par Example
  ///   \code
  ///     local light = Game:CreateLight(Vision.hkvVec3(100,100,200), Vision.LIGHT_POINT, 350, "MyLight")
  ///   \endcode
  /// \see GetLight
  VisLightSource_cl CreateLight(hkvVec3 pos, number type = LIGHT_POINT, float radius = 100, string key = nil);
  
  /// \brief Create a path.
  /// \param numNodes (\b optional) Initial number of path nodes (will internally reserve an array).
  /// \param closed (\b optional) Set to true in order to close this path, which causes the path to wrap at the end of the path.
  /// \param key (\b optional) The object key, which can be used to search for the path ( Game:GetPath(key) ).
  /// \return A path instance.
  /// \par Example
  ///   \code
  ///     local path = Game:CreatePath(4, true, "MyPath")
  ///   \endcode
  /// \see GetPath
  VisPath_cl CreatePath(number numNodes = -1, boolean closed = false, string  key = nil);
  
  /// \brief Create a path node.
  /// \param pos The position of the path node.
  /// \param key (\b optional) The object key, which can be used to search for the path node ( VisPath_cl::GetPathNode(key) ).
  /// \return A path node.
  /// \par Example
  ///   \code
  ///     local path = Game:CreatePath(4, true, "MyPath")
  ///     path:AddPathNode(Game:CreatePathNode(Vision.hkvVec3( 100,0,0), "Node0"))
  ///     path:AddPathNode(Game:CreatePathNode(Vision.hkvVec3 (0,100,0), "Node1"))
  ///     path:AddPathNode(Game:CreatePathNode(Vision.hkvVec3(-100,0,0), "Node2"))
  ///     path:AddPathNode(Game:CreatePathNode(Vision.hkvVec3(0,-100,0), "Node3"))
  ///     ...
  ///     -- you can also use this more convenient function:
  ///     path:AddPathNode(Vision.hkvVec3(0,-200,0), "Node4")
  ///     path:AddPathNode(Vision.hkvVec3(50,-50,0), "Node5")
  ///   \endcode
  /// \see GetPath
  /// \see VisPath_cl::AddPathNode
  /// \see VisPath_cl::GetPathNode
  VisPathNode_cl CreatePathNode(hkvVec3 pos, string key = nil);
  
  /// \brief Create a particle effect.
  /// \param pos The position where to create the particle effect.
  /// \param effectFile The class name of the entity instance.
  /// \param key (\b optional) The object key, which can be used to search for the particle effect ( Game:GetEffect(key) ).
  /// \return A particle effect instance or nil.
  /// \par Example
  ///   \code
  ///     function OnCreate(self)
  ///       Game:CreateEffect(self:GetPosition(), "GameEffects\\Particles\\Fire\\BigFireSmoke.xml", "BigFire")
  ///     end
  ///   \endcode
  /// \see GetEffect
  VisParticleEffect_cl CreateEffect(hkvVec3 pos, string effectFile, string key = nil);
  
  /// \brief Create a static mesh instance.
  /// \param pos The position where to create the static mesh instance.
  /// \param meshFile The mesh file to instantiate.
  /// \param notifyPhysics Set to true if the physics engine should be notified.
  /// \param key (\b optional) The object key, which can be used to search for the mesh instance ( Game:GetStaticMeshInstance(key) ).
  /// \return A static mesh instance component or nil.
  /// \par Example
  ///   \code
  ///     function OnCreate(self)
  ///       Game:CreateStaticMeshInstance(self:GetPosition(), "Data\\Maps\\SimpleGround\\ground.vmesh", true, "Ground")
  ///     end
  ///   \endcode
  /// \see GetStaticMeshInstance
  VisStaticMeshInstance_cl CreateStaticMeshInstance(hkvVec3 pos, string meshFile, boolean notifyPhysics = true, string key = nil);
 
  /// \brief Create an object component.
  /// \param typeName Name of the component's type (i.e. class name)
  /// \param componentName (\b optional) Name of the desired component.
  /// \return Instnace of the requested component or nil.
  /// \note Most of the time AddComponentOfType is more convenient!
  /// \par Example
  ///   \code
  ///     function OnCreate(self)
  ///       local shadow = Game:CreateComponent("VBlobShadow")
  ///       self:AddComponent(shadow)
  ///     end
  ///   \endcode
  /// \see VisTypedEngineObject_cl::AddComponent
  /// \see VisTypedEngineObject_cl::AddComponentOfType
  /// \see VisTypedEngineObject_cl::RemoveComponent
  /// \see VisTypedEngineObject_cl::GetComponentOfType
  /// \see VisTypedEngineObject_cl::GetComponentOfBaseType
  mixed CreateComponent(string typeName, string componentName = nil);


  /// \brief Instantiate a prefab.
  /// \param pos The position where to instantiate the prefab
  /// \param prefabFilename The filename of the prefab resource.
  /// \param optionalParent (\b optional) Specify a parent for all created objects (e.g. to rotate or move them all together).
  /// \return A table containing all created objects available in Lua Script (instances of VTypedObject or dereived)
  /// \par Example
  ///   \code
  ///     function OnCreate(self)
  ///       local objects = InstantiatePrefab(Vision.hkvVec3(0,0,0), "myPrefab.prefab", self) --use self as parent
  ///       Debug:PrinLine("Instantiated " .. #objects .. " child objects via prefab!")
  ///     end
  ///   \endcode
  table InstantiatePrefab(hkvVec3 pos, string prefabFilename, VisObject3D_cl optionalParent);


  /// \brief Create a dynamic mesh.
  /// \param dynamicMeshFile The mesh file.
  /// \param forceLoad If true, the returned resource is guaranteed to be loaded.
  /// \param loadAnim (\b optional) Load animations of the mesh as well.
  /// \return A dynamic mesh instance or nil if the operation failed.
  /// \par Example
  ///   \code
  ///     function OnCreate(self)
  ///       local mesh = Game:CreateDynamicMesh("Models/Warrior/Warrior.model", true)
  ///       if mesh ~= nil then self:SetMesh(mesh) end
  ///     end
  ///   \endcode
  /// \see GetDynamicMesh
  VDynamicMesh CreateDynamicMesh(string dynamicMeshFile, boolean forceLoad, boolean loadAnim=true);
  
  /// \brief Create a new dynamic mesh by merging 2 or more dynamic meshes together.
  /// \param meshes A table containin VDynamicMesh instances.
  /// \return A dynamic mesh instance or nil if the operation failed.
  /// \par Example
  ///   \code
  ///     function OnCreate(self)
  ///
  ///       local merge = { }
  ///       merge[1] = Game:CreateDynamicMesh("Body.model", true)
  ///       merge[2] = Game:CreateDynamicMesh("Shield.model", true)
  ///       merge[3] = Game:CreateDynamicMesh("Sword.model", true)
  ///
  ///       local mesh = Game:MergeMeshes(merge)
  ///       if mesh ~= nil then self:SetMesh(mesh) end
  ///     end
  ///   \endcode
  /// \see CreateDynamicMesh
  /// \see MergeMeshesByString
  VDynamicMesh MergeMeshes(table meshes);
  
  /// \brief Create a new dynamic mesh by merging 2 or more dynamic meshes together.
  /// \param meshes A table containing strings of the desired dynamic meshes to merge.
  /// \return A dynamic mesh instance or nil if the operation failed.
  /// \par Example
  ///   \code
  ///     function OnCreate(self)
  ///       local merge = { "Body.model", "Shield.model", "Sword.model" }
  ///       local mesh = Game:MergeMeshesByString(merge)
  ///       if mesh ~= nil then self:SetMesh(mesh) end
  ///     end
  ///   \endcode
  /// \see CreateDynamicMesh
  /// \see MergeMeshes
  VDynamicMesh MergeMeshesByString(table meshes);
  
  /// \brief Create a texture object.
  /// \param textureFile The texture file.
  /// \param create3DTexture (\b optional) Create a 3D texture object.
  /// \return A texture object or nil.
  /// \par Example
  ///   \code
  ///     function OnCreate(self)
  ///       local surface = GetMesh():GetSurface(0)
  ///       local tex = Game:CreateTexture("Textures/colormap2.dds")
  ///       surface:SetDiffuseTexture(tex)
  ///     end
  ///   \endcode
  VTextureObject CreateTexture(string textureFile, boolean create3DTexture=false);
  
  /// \brief Create a new context camera - \b please \b check \b the \b details \b !
  /// \param replaceMainRenderContextCamera (\b optional) Replace the cureent camera of the main rener context with this instance (set as active).
  /// \param key (\b optional) The object key, which can be used to search for the context camera ( Game:GetCamera(key) ).
  /// \return A texture object or nil.
  /// \note \b IMPORTANT: If you would like to control the camera via VisObject3D_cl::IncOrientation / VisObject3D_cl::SetOrientation,
  ///   you have to enable Euler angles using VisObject3D_cl::SetUseEulerAngles(true) since this is not default!
  /// \par Example
  ///   \code
  ///     ...
  ///     local cam = Game:CreateCamera()
  ///     ...
  ///   \endcode
  /// \see VisObject3D_cl::SetUseEulerAngles
  VisContextCamera_cl CreateCamera(boolean replaceMainRenderContextCamera = true, string key = nil);
  
  /// \brief Create a new screen mask.
  /// \param x X start position of the screen mask.
  /// \param y Y start position of the screen mask.
  /// \param textureFile The texture file to use for the screen mask.
  /// \param key (\b optional) The object key, which can be used to search for the screen mask.
  /// \return A texture object or nil.
  /// \note \b IMPORTANT: Screen mask won't be cleaned up when changing the editor mode in vForge.
  ///          Therefore it's important to maintain them manually or use VisScreenMask_cl::DeleteAllUnrefScreenMasks
  ///          to clean up.
  /// \par Example
  ///   \code
  ///     function OnAfterSceneLoaded(self)
  ///       local screenMask = Game:CreateScreenMask(50,50, "Textures/vision_logo.dds")
  ///       screenMask:SetBlending(Vision.BLEND_ALPHA)
  ///     end
  ///     
  ///     function OnBeforeSceneUnloaded(self)
  ///       Game:DeleteAllUnrefScreenMasks()
  ///     end
  ///   \endcode
  /// \see VisScreenMask_cl::DeleteAllUnrefScreenMasks
  /// \see VisScreenMask_cl::Unload
  VisScreenMask_cl CreateScreenMask(number x, number y, string textureFile, string key = nil);
  
  /// @}
  /// @name Get Game Objects (Search)
  /// @{
  
  /// \brief Search for an entity with the specified object key.
  /// \param key The search key.
  /// \return An entity or nil if there is no entity with the specified object key.
  /// \par Example
  ///   \code
  ///     ...
  ///     local myEntity = Game:GetEntity("SpaceShip01")
  ///     ...
  ///   \endcode
  mixed GetEntity(string key);
  
  /// \brief Search for a light source with the specified object key.
  /// \param key The search key.
  /// \return A light source or nil if there is no light source with the specified object key.
  /// \par Example
  ///   \code
  ///     ...
  ///     local beacon = Game:GetLight("Beacon")
  ///     ...
  ///   \endcode
  VisLightSource_cl GetLight(string key);
  
  /// \brief Search for a path with the specified object key.
  /// \param key The search key.
  /// \return A path or nil if there is no path with the specified object key.
  /// \par Example
  ///   \code
  ///     ...
  ///     local loop = Game:GetPath("OuterLoop")
  ///     ...
  ///   \endcode
  VisPath_cl GetPath(string key);
  
  /// \brief Search for a particle effect with the specified object key.
  /// \param key The search key.
  /// \return A particle effect or nil if there is no particle effect with the specified object key.
  /// \par Example
  ///   \code
  ///     ...
  ///     local bigBang = Game:GetEffect("BigBang")
  ///     ...
  ///   \endcode
  VisParticleEffect_cl GetEffect(string key);
  
  /// \brief Search for a static mesh instance with the specified object key.
  /// \param key The search key.
  /// \return A static mesh instance or nil if there is no static mesh instance with the specified object key.
  /// \par Example
  ///   \code
  ///     ...
  ///     local myCastle = Game:GetStaticMeshInstance("Castle")
  ///     ...
  ///   \endcode
  VisStaticMeshInstance_cl GetStaticMeshInstance(string key);
  
  /// \brief Search for a dynamic mesh using the filename.
  /// \param filename The filename of the mesh.
  /// \return A dynamic mesh or nil if there is no dynamic mesh with that filename.
  /// \par Example
  ///   \code
  ///     ...
  ///     local model = Game:GetDynamicMesh("Warrior.model")
  ///     ...
  ///   \endcode
  VDynamicMesh GetDynamicMesh(string filename);
  
  /// \brief Search for a context camera with the specified object key or the context camera of the main render context - \b please \b check \b the \b details \b !
  /// \param key (\b optional) The search key - skip this parameter to get the camera context of the main render context.
  /// \return A camera context or nil if there is no context camera with the specified object key.
  /// \note \b IMPORTANT: If you would like to control the camera via VisObject3D_cl::IncOrientation / VisObject3D_cl::SetOrientation,
  ///   you have to enable Euler angles using VisObject3D_cl::SetUseEulerAngles(true) since this is not default!
  /// \par Example
  ///   \code
  ///     ...
  ///     local cam01 = Game:GetCamera("Cam01")
  ///     local mainCam = Game:GetCamera()
  ///     ...
  ///   \endcode
  /// \see VisObject3D_cl::SetUseEulerAngles
  VisContextCamera_cl GetCamera(string key = nil);

  /// \brief Search for a screen mask using the key.
  /// \param key The search key for the screen mask.
  /// \return A screen mask or nil if there is no screen mask with the specified key.
  /// \par Example
  ///   \code
  ///     ...
  ///     local splashScreenMask = Game:GetScreenMask("SPLASH")
  ///     ...
  ///   \endcode
  VisScreenMask_cl GetScreenMask(string key)
  
  /// \brief Search for a vubemap using the key.
  /// \param key The search key for the cubemap.
  /// \return A cubemap or nil if there is no cubemap with the specified key.
  /// \par Example
  ///   \code
  ///     ...
  ///     local cubeMap = Game:GetCubeMap("MyCubemap01")
  ///     ...
  ///   \endcode
  CubeMapHandle_cl GetCubeMap(string key)
  
  /// \brief Search for an arbitrary game object using the object key.
  /// \param key The search key.
  /// \return An instance of the object or nil if there is no object with the specified key.
  /// \details
  ///   Search order:
  ///     - 1. Entities
  ///     - 2. Light Sources
  ///     - 3. Particle Effects
  ///     - 4. Paths
  ///     - 5. Context Cameras
  ///     - 6. Static Mesh Instances
  ///     - 7. Screen Masks
  /// \par Example
  ///   \code
  ///     ...
  ///     local myCam = Game:GetObject("Cam01")
  ///     local myEffect = Game:GetObject("BigBang")
  ///     local myEntity = Game:GetObject("SpaceShip01")
  ///     ...
  ///   \endcode
  mixed GetObject(string key);

  /// @}
  /// @name Delete Game Objects
  /// @{
  
  /// \brief Delte all unreferenced screen masks.
  /// \par Example
  ///   \code
  ///     ...
  ///     Game:DeleteAllUnrefScreenMasks()
  ///     ...
  ///   \endcode
  void DeleteAllUnrefScreenMasks();
  
  /// \brief Delte all unreferenced lights.
  void DeleteAllUnrefLights();
  
  /// \brief Delte all unreferenced screen paths.
  void DeleteAllUnrefPaths();
  
  /// \brief Delte all unreferenced static mesh instances.
  /// \par Example
  ///   \code
  ///     ...
  ///     Game:DeleteAllUnrefStaticMeshInstances()
  ///     ...
  ///   \endcode
  void DeleteAllUnrefStaticMeshInstances();
  
  /// \brief Delte all unreferenced cameras.
  void DeleteAllUnrefCameras();
  
  /// @}
  /// @name Misc. Functions
  /// @{
  
  /// \brief Executes the passed string.
  /// \details
  ///   The passed script string can contain multiple script action separated by semicolons.
  ///   This function extract each script action and calls the VAction::BaseExecuteOneAction to
  ///   execute extracted single action. If one action fails the function returns false.
  /// \param action A string with script commands.
  /// \return false if any command fails, otherwise true.
  /// \par Example
  ///   \code
  ///     ...
  ///     local success = Game:Execute("Profiling on")
  ///     ...
  ///   \endcode
  boolean ExecuteAction(string action);

  /// \brief Suspends the active script for a certain time. 
  /// \param sec The amount of seconds to wait before resuming the script.  
  /// \par Example
  ///   \code
  ///     ...
  ///     Game:WaitSeconds(3.5)
  ///     ...
  ///   \endcode
  void WaitSeconds(number sec);
  
  /// @}
  
};

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
