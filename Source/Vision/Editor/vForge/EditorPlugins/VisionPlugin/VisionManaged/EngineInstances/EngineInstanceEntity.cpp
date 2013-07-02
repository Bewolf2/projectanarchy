/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Editor/vForge/EditorPlugins/VisionPlugin/VisionManaged/VisionManagedPCH.h>
#include <Vision/Editor/vForge/EditorPlugins/VisionPlugin/VisionManaged/EngineInstances/EngineInstanceEntity.hpp>

#using <mscorlib.dll>
#include <Vision/Runtime/Base/Graphics/Shader/vShaderConstantHelper.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/MobileForwardRenderer/VFakeSpecularGenerator.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/CubeMapHandle.hpp>

using namespace System;
using namespace System::Diagnostics;
using namespace ManagedFramework;

namespace VisionManaged
{

  class EntityCreationTemplate_cl : public VisEntityTemplate_cl
  {
  public:
    void SetVariable(const char *szKey, const char *szValue)
    {
      m_propertyDict.Set(szKey, szValue);
    }

    VOVERRIDE void OnEntityCreated(VisObject3D_cl* pObj) override 
    {
      // Use this virtual function to set the entity properties AFTER the entity instance has been created
      // and BEFORE the InitFunction is called.
      VisBaseEntity_cl *pEntity = (VisBaseEntity_cl *) pObj;

      int iLen = m_propertyDict.GetLength();
      for (int i = 0; i < iLen; i++)
        pEntity->SetVariable(m_propertyDict.GetKey(i), m_propertyDict.GetValue(i));
    }

    VStrDictionary m_propertyDict;
  };

  EngineInstanceEntity::EngineInstanceEntity(String ^sEntityClass, String ^sModelFileName, Shape3D ^shape, StringDictionary ^propertyDict, bool bCallInitFunction)
  {
    m_pCustomLinkInfo = new VShapeLinkConfig;
    EntityCreationTemplate_cl entityTemplate;

    VString sConvertedFileName, sConvertedClassName;
    ConversionUtils::StringToVString( sModelFileName, sConvertedFileName );
    ConversionUtils::StringToVString( sEntityClass, sConvertedClassName );

    // Setup the entity properties in the template. This allows us to set the entity properties
    // before the InitFunction of the entity is called.
    if (propertyDict)
    {
      IEnumerator ^enumerator = propertyDict->GetEnumerator();
      while (enumerator->MoveNext())
      {
        DictionaryEntry ^entry = dynamic_cast<DictionaryEntry ^> (enumerator->Current);
        VString sKey, sValue;
        ConversionUtils::StringToVString( dynamic_cast<String ^> (entry->Key), sKey );
        ConversionUtils::StringToVString( dynamic_cast<String ^> (entry->Value), sValue );
        entityTemplate.SetVariable(sKey,sValue);
      }
    }

    // Set the remaining template parameters
    VString params;
    if (shape)
    {
      params.Format("orientation=%f/%f/%f,scale=%f/%f/%f ",
        shape->Yaw, shape->Pitch, shape->Roll,
        shape->ScaleX, shape->ScaleY, shape->ScaleZ );
      entityTemplate.m_vPosition = hkvVec3 (shape->x,shape->y,shape->z);
    }
    
    entityTemplate.m_pszClassName = sConvertedClassName;
    entityTemplate.m_pszVarString = params;
    entityTemplate.m_pszModelFile = sConvertedFileName;
    entityTemplate.m_bCallInitFunction = bCallInitFunction;

    // Create the entity
    VisBaseEntity_cl *pEntity = Vision::Game.CreateEntity(entityTemplate);
    if (!pEntity) // entity class does not exist? default to base entity
    {
      entityTemplate.m_pszClassName = "VisBaseEntity_cl";
      Vision::Error.Warning("Could not create entity model '%s' of class '%s'",sConvertedFileName.AsChar(),sConvertedClassName.AsChar());
      pEntity = Vision::Game.CreateEntity(entityTemplate);
    }

    // reference the entity with a weak pointer. This will make sure that we correctly get a null pointer
    // if the entity gets deleted in the engine
    m_pEntityWP = new VWeakPtr<VisBaseEntity_cl>(pEntity->GetWeakReference());
    m_iVisibleBitmask = 0xffffffff;
    m_bVisible = true;
  }

  void EngineInstanceEntity::CallInitFunction()
  {
    VisBaseEntity_cl *pEnt = GetEntity();
    if (pEnt!=nullptr)
      pEnt->InitFunction();
  }

  /// <summary>
  /// Sets the passed dynamic entity properties on the entity instances
  /// </summary>
  void EngineInstanceEntity::SetEntityProperties(StringDictionary ^propertyDict)
  {
    if (!propertyDict)
      return;
    IEnumerator ^enumerator = propertyDict->GetEnumerator();
    while (enumerator->MoveNext())
    {
      DictionaryEntry ^entry = dynamic_cast<DictionaryEntry ^> (enumerator->Current);
      VString sKey, sValue;
      ConversionUtils::StringToVString( dynamic_cast<String ^> (entry->Key), sKey );
      ConversionUtils::StringToVString( dynamic_cast<String ^> (entry->Value), sValue );
      SetVariable( ConversionUtils::VStringToString(sKey), ConversionUtils::VStringToString(sValue) );
    }
  }

  // nb: code duplication from EngineComponentManager::SetNativeComponentProperty
  void EngineInstanceEntity::SetEntityProperty(DynamicProperty ^prop)
  {
    VisBaseEntity_cl *pEntity = GetEntity();
    if (!pEntity)
      return;
    VString sProperty, sValue;

    // do not pass the enum values as strings, see also GetEntityPropertyAsString in EntityShape
    // note we are not checking prop->PropertyType->IsEnum, which would be different
    if (prop->PropertyType->NativeType->IsEnum)
    {
      int iAsInt = (int)prop->Value;
      sValue.Format("%i", iAsInt);
    } else
    {
      //if (prop->PropertyType->NativeType == ShapeReference && )
      String ^val = SerializationHelper::GetEngineStringFromObject(prop->Value);
      ConversionUtils::StringToVString(val,sValue);
    }
    ConversionUtils::StringToVString(prop->PropertyType->Name,sProperty);
    pEntity->SetVariable(sProperty, sValue);
    return;
  }

    
  PropertyFlags_e EngineInstanceEntity::GetPropertyFlags(DynamicProperty ^pProperty)
  {
    return ConversionUtils::GetPropertyFlags(pProperty, GetEntity());
  }


  void EngineInstanceEntity::DisposeObject()
  {
    VisBaseEntity_cl *pEntity = GetEntity();
    V_SAFE_DISPOSEOBJECT(pEntity);
    V_SAFE_DELETE(m_pEntityWP);
    V_SAFE_DELETE(m_pCustomLinkInfo);
  }

  void *EngineInstanceEntity::GetObject3D()
  {
    return GetEntity();
  }

  bool EngineInstanceEntity::HasLightmappedMaterials()
  {
    if (GetEntity()==nullptr || !GetEntity()->HasMesh())
      return false;

    VDynamicMesh *pModel = GetEntity()->GetMesh();

    // does the model have a lightmap texture coordinate stream?
    if ((pModel->GetSupportedVertexStreams()&VERTEX_STREAM_TEX1)==0)
      return false;

    for (int i=0;i<(int)pModel->GetSurfaceCount();i++)
    {
      VisSurface_cl *pSurf = pModel->GetSurface(i);
      if (pSurf->GetLightingMode()==VIS_LIGHTING_LIGHTMAPPING)
        return true;
    }

    return false;
  }

  void EngineInstanceEntity::TraceShape(Shape3D ^ownerShape, Vector3F rayStart,Vector3F rayEnd, ShapeTraceResult ^%result)
  {
    if ( GetEntity() == nullptr )
      return;

    ShapeTraceResult ^pResult = result;

    VisBaseEntity_cl *list = GetEntity();
    VisEntityCollection_cl traceCollection(1, &list, 1, 1);
    static VisTraceLineInfo_t traceInfo;//@@@@

    // Fill-out the query struct
    VisTraceLineQuery_t query;
	  query.m_traceCount	= 0;
    query.m_start		= hkvVec3(rayStart.X,rayStart.Y,rayStart.Z);
    query.m_end			= hkvVec3(rayEnd.X,rayEnd.Y,rayEnd.Z);
    query.m_flags		= VIS_TRACESETTINGS_FORCEOBJ_POLYGONACC | VIS_TRACESETTINGS_STORESURFACEINFO;
	
    // Get components and tell them to traceShape
    if ( list )
    {
      const int numComponents = list->Components().Count();
      for (int k = 0; k < numComponents; k++)
      {
        // Get component and tell it to traceShape
        IVObjectComponent* objComponent = list->Components().GetAt(k);
        query.m_traceCount = 0;
        objComponent->SendMsg(list, VIS_MSG_EDITOR_TRACE_SHAPE, (INT_PTR)&query);

        // Process results
        if ( query.m_traceCount > 0 )
        {
          pResult->SetShapeHit_IfCloser(ownerShape, query.m_traceInfo.distance);
        }
      }
	}

	// Collide against main entity
    const int iTraceCount = Vision::CollisionToolkit.TraceLineEntities(query.m_start, query.m_end, traceCollection, &traceInfo, query.m_flags);
    if ( iTraceCount  > 0 )
	{
    pResult->SetShapeHit_IfCloser(ownerShape,traceInfo.distance);
  }
  }

  bool EngineInstanceEntity::GetLocalBoundingBox(BoundingBox ^%bbox)
  {
    if ( GetEntity() == nullptr )
      return false;

    //*bbox = nullptr;
    VDynamicMesh *pMB = GetEntity()->GetMesh();
    if (!pMB || !pMB->GetFilename()) return false;

    hkvAlignedBBox temp;
    if (!pMB->GetVisibilityBoundingBox(temp)) return false;

    (*bbox).Set(temp.m_vMin.x,temp.m_vMin.y,temp.m_vMin.z, temp.m_vMax.x,temp.m_vMax.y,temp.m_vMax.z );
    return true;
  }

  void EngineInstanceEntity::SetScaling(float x,float y, float z)
  {
    if ( GetEntity() != nullptr )
      GetEntity()->SetScaling(hkvVec3(x,y,z));

    // Notify entity instance that scaling has changed
    GetEntity()->SendMsg( GetEntity(), VIS_MSG_EDITOR_PROPERTYCHANGED, (INT_PTR) "Scaling", 0 );
  }

  bool EngineInstanceEntity::OnExport(SceneExportInfo ^info) 
  {
    Debug::Assert( GetEntity() != nullptr );
    VArchive &ar = *((VArchive *)info->NativeShapeArchivePtr.ToPointer());
    ar << GetEntity();
    return true;
  }

  void EngineInstanceEntity::OnRenderHook(ShapeBase ^owner, int iConstant)
  {
    VisBaseEntity_cl *pEntity = GetEntity();
    if (!Vision::Editor.IsAnimatingOrPlaying() && pEntity!=NULL && pEntity->WasVisibleLastFrame(Vision::Contexts.GetCurrentContext()))
       ((VisionEngineManager ^)EditorManager::EngineManager)->RenderEntityHighlighting(pEntity);
  }


  VisBaseEntity_cl *EngineInstanceEntity::GetEntity()
  {
    if (!m_pEntityWP)
      return NULL;
    return m_pEntityWP->GetPtr();
  }

  bool EngineInstanceEntity::SetModelFile(String ^sModelFile)
  {
     if (!GetEntity())
       return false;

     VString sConvertedModelFileName;
     ConversionUtils::StringToVString( sModelFile, sConvertedModelFileName );
     
     GetEntity()->SetCustomTextureSet(NULL);
     bool bSuccess = GetEntity()->SetMesh(sConvertedModelFileName)==TRUE;
     if (!bSuccess)
     {
       VDynamicMesh *pClearModel = nullptr;
       GetEntity()->SetMesh(pClearModel);
     }

     // Notify entity instane that model file has been changed
     GetEntity()->SendMsg( GetEntity(), VIS_MSG_EDITOR_PROPERTYCHANGED, (INT_PTR) "ModelFile", 0 );
       
     return bSuccess;
  }

  void EngineInstanceEntity::SetCustomMaterialSet(CustomMaterialSetProvider ^set)
  {
    VisBaseEntity_cl *pEntity = GetEntity();
    if (!pEntity)
      return;
    if (set==nullptr)
      pEntity->SetCustomTextureSet(NULL);
    else
      pEntity->SetCustomTextureSet(set->GetTextureSet());
  }


  /// <summary>
  /// Sets the value of a variable in the engine entity instance
  /// </summary>
  bool EngineInstanceEntity::SetVariable(String ^sVarName, String ^sVarData)
  {
    VisBaseEntity_cl *pEntity = GetEntity();
    if (pEntity == nullptr )
      return false;

    // set the variable on the entity
    VString sEngineVarName, sEngineVarData;
    ConversionUtils::StringToVString( sVarName, sEngineVarName );
    ConversionUtils::StringToVString( sVarData, sEngineVarData );

    // result can be false if the property is from XML template:
    bool bResult = (pEntity->SetVariable(sEngineVarName, sEngineVarData) == TRUE);

    // inform the entity about the property change
    pEntity->SendMsg( pEntity, VIS_MSG_EDITOR_PROPERTYCHANGED, (INT_PTR) sEngineVarName.AsChar(), (INT_PTR) sEngineVarData.AsChar() );

    return bResult;
  }
  
  void EngineInstanceEntity::GetStandardValues(StringCollection ^pList, String ^key)
  {
    VisBaseEntity_cl *pEntity = GetEntity();
    if (pEntity == nullptr )
      return;
    if (String::Compare(key,"Animation")==0)
    {
      bool bSkeletal = true;
      bool bVertex = true;
      VDynamicMesh *pModel = pEntity->GetMesh();
      if (!pModel)
        return;
      VisAnimSequenceSetCollection_cl* pAllSets = pModel->GetSequenceSetCollection();
      for (int i=0;i<pAllSets->Count();i++)
      {
        VisAnimSequenceSet_cl *pSet = pAllSets->GetSequenceSet(i);
        for (int j=0;j<pSet->GetSequenceCount();j++)
        {
          VisAnimSequence_cl* pSeq = pSet->GetSequence(j);
          if (pSeq->GetType()==VIS_MODELANIM_VERTEX && !bVertex)
            continue;
          if (pSeq->GetType()==VIS_MODELANIM_SKELETAL && !bSkeletal)
            continue;
          pList->Add(ConversionUtils::VStringToString(pSeq->GetName()));
        }
      }
      return;
    }

    // else forward request to native entity message function:
    ConversionUtils::RetrieveStandardValues(pEntity,key,pList);
  }

  void EngineInstanceEntity::GetBones(Generic::List<SkeletalBoneDesc ^> ^pList)
  {
    VisBaseEntity_cl *pEntity = GetEntity();
    if (pEntity == nullptr )
      return;
    GetBones(pEntity->GetMesh(),pList);
  }


  void EngineInstanceEntity::GetBones(String ^modelFile, Generic::List<SkeletalBoneDesc ^> ^pList)
  {
    if (String::IsNullOrEmpty(modelFile))
      return;
    VString sModel;
    ConversionUtils::StringToVString(modelFile,sModel);
    VDynamicMesh *pModel = Vision::Game.LoadDynamicMesh(sModel,true,false);
    GetBones(pModel,pList);
  }

  void EngineInstanceEntity::GetBones(VDynamicMesh *pModel, Generic::List<SkeletalBoneDesc ^> ^pList)
  {
    if (!pModel || !pModel->GetSkeleton())
      return;
    VisSkeleton_cl *pSkeleton = pModel->GetSkeleton();
    for (int i=0;i<pSkeleton->GetBoneCount();i++)
    {
      const VisSkeletalBone_cl *pBone = pSkeleton->GetBone(i);
      pList->Add(gcnew SkeletalBoneDesc(
        ConversionUtils::VStringToString(pBone->m_sBoneName), pBone->m_iParentIndex));
    }
  }


  bool EngineInstanceEntity::HasBones()
  {
    VisBaseEntity_cl *pEntity = GetEntity();
    if (pEntity == nullptr )
      return false;
    VDynamicMesh *pModel = pEntity->GetMesh();
    if (!pModel || !pModel->GetSkeleton() || pModel->GetSkeleton()->GetBoneCount()==0)
      return false;
    return true;
  }



  void EngineInstanceEntity::GetNativeLinks(Shape3D ^pOwner, LinkCollection ^pSrcCollection, LinkCollection ^pTgtCollection, LinkCollection ^pBiDirCollection)
  {
    // base function: gathers trigger links from components
    IEngineInstanceObject3D::GetNativeLinks(pOwner, pSrcCollection, pTgtCollection, pBiDirCollection);

    // the rest of the code could be moved to IEngineInstanceObject3D, so custom links vis message function
    // would be available for all object3D's
    VisBaseEntity_cl *pEntity = GetEntity();
    if (pEntity == nullptr )
      return;

    // query links from native object
    m_pCustomLinkInfo->FreeLinks();
    pEntity->MessageFunction(VIS_MSG_EDITOR_GETLINKS, (INT_PTR)m_pCustomLinkInfo, 0);

    // tag for latter removal, but leave other types unaffected:
    pSrcCollection->CustomTag = false;
    pTgtCollection->CustomTag = false;
    pBiDirCollection->CustomTag = false;
    pSrcCollection->TagLinksOfType(LinkSourceObject3D::typeid,true,true);
    pTgtCollection->TagLinksOfType(LinkTargetObject3D::typeid,true,true);
    pBiDirCollection->TagLinksOfType(LinkBiDirectionalObject3D::typeid,true,true);

    // convert them to C# link objects
    for (int i=0;i<m_pCustomLinkInfo->GetLinkCount();i++)
    {
      VShapeLinkInfo &info = m_pCustomLinkInfo->GetLinkInfo(i);
      if (info.m_eType==VShapeLinkInfo::LINK_SOURCE)
      {
        LinkSourceObject3D ^pNewLink = static_cast<LinkSourceObject3D ^>(pSrcCollection->GetLinkByPrimaryId(ConversionUtils::VStringToString(info.m_sUniqueStringID)));
        if (!pNewLink)
          pNewLink = gcnew LinkSourceObject3D(pOwner, ConversionUtils::VStringToString(info.m_sDisplayName), ConversionUtils::VStringToString(info.m_sUniqueStringID));
        if (!info.m_sIconFilename.IsEmpty())
          pNewLink->HotspotIconFilename = ConversionUtils::VStringToString(info.m_sIconFilename);

        pNewLink->iIndex = i;
        pNewLink->CustomTag = false; // do not remove
        pSrcCollection->AddUnique(pNewLink);
      }
      if (info.m_eType==VShapeLinkInfo::LINK_TARGET)
      {
        LinkTargetObject3D ^pNewLink = static_cast<LinkTargetObject3D ^>(pTgtCollection->GetLinkByPrimaryId(ConversionUtils::VStringToString(info.m_sUniqueStringID)));
        if (!pNewLink)
          pNewLink = gcnew LinkTargetObject3D(pOwner,ConversionUtils::VStringToString(info.m_sDisplayName),ConversionUtils::VStringToString(info.m_sUniqueStringID));
        if (!info.m_sIconFilename.IsEmpty())
          pNewLink->HotspotIconFilename = ConversionUtils::VStringToString(info.m_sIconFilename);
        pNewLink->iIndex = i;
        pNewLink->CustomTag = false; // do not remove
        pTgtCollection->AddUnique(pNewLink);
      }
      if (info.m_eType==VShapeLinkInfo::LINK_BIDIRECTIONAL)
      {
        LinkBiDirectionalObject3D ^pNewLink = static_cast<LinkBiDirectionalObject3D ^>(
          pBiDirCollection->GetLinkByPrimaryId(ConversionUtils::VStringToString(info.m_sUniqueStringID)));
        if (!pNewLink)
          pNewLink = gcnew LinkBiDirectionalObject3D(pOwner, ConversionUtils::VStringToString(info.m_sDisplayName), ConversionUtils::VStringToString(info.m_sUniqueStringID));
        if (!info.m_sIconFilename.IsEmpty())
          pNewLink->HotspotIconFilename = ConversionUtils::VStringToString(info.m_sIconFilename);
        pNewLink->iIndex = i;
        pNewLink->CustomTag = false; // do not remove
        pBiDirCollection->AddUnique(pNewLink);
      }
    }

    pSrcCollection->BreakOtherLinksForTagged();
    pTgtCollection->BreakOtherLinksForTagged();
    pBiDirCollection->BreakOtherLinksForTagged();

    // remove unused:
    pSrcCollection->RemoveTagged();
    pTgtCollection->RemoveTagged();
    pBiDirCollection->RemoveTagged();

  }

  bool EngineInstanceEntity::CanLink(int iIndex, IEngineInstanceObject3D ^pObj3D, String ^otherName)
  {
    VisBaseEntity_cl *pEntity = GetEntity();
    VisObject3D_cl *pEngineObj3D = (VisObject3D_cl *)pObj3D->GetObject3D();
    if (pEntity == nullptr || pEngineObj3D==nullptr)
      return false;

    VShapeLink link(m_pCustomLinkInfo->GetLinkInfo(iIndex),pEngineObj3D);
    ConversionUtils::StringToVString(otherName,link.m_sOtherUniqueStringID);
    pEntity->MessageFunction(VIS_MSG_EDITOR_CANLINK, (INT_PTR)&link, 0);

    return link.m_bResult;
  }

  void EngineInstanceEntity::OnLink(int iIndex, IEngineInstanceObject3D ^pObj3D, String ^otherName, bool bStatus)
  {
    VisBaseEntity_cl *pEntity = GetEntity();
    VisObject3D_cl *pEngineObj3D = (VisObject3D_cl *)pObj3D->GetObject3D();
    if (pEntity == nullptr || pEngineObj3D==nullptr)
      return;

    VShapeLink link(m_pCustomLinkInfo->GetLinkInfo(iIndex),pEngineObj3D);
    ConversionUtils::StringToVString(otherName,link.m_sOtherUniqueStringID);
    int iMsg = bStatus ? VIS_MSG_EDITOR_ONLINK:VIS_MSG_EDITOR_ONUNLINK;
    pEntity->MessageFunction(iMsg, (INT_PTR)&link, 0);
  }


  void EngineInstanceEntity::SetCopySource(EngineInstanceEntity ^pOther)
  {
    VisBaseEntity_cl *pSourceEnt = pOther!=nullptr ? pOther->GetEntity() : NULL;
    VisBaseEntity_cl *pThisEntity = GetEntity();
    if (pThisEntity!=nullptr)
      pThisEntity->MessageFunction(VIS_MSG_EDITOR_CLONESOURCE,(INT_PTR)pSourceEnt, 0);
    
  }

  void EngineInstanceEntity::SynchronizeOwnComponents(ShapeBase ^owner, ShapeComponentCollection ^components)
  {
    // tag all
    components->Tagged = true;

    VisBaseEntity_cl *pThisEntity = GetEntity();
    if (pThisEntity==NULL)
      return;

    // un-tag all components that have not been created through this code path
    for (int i=0;i<components->Count;i++)
      if (!components[i]->CreatedByNative)
        components[i]->Tagged = false;

    int iCount = pThisEntity->Components().Count();
    for (int i=0;i<iCount;i++)
    {
      IVObjectComponent *pComp = pThisEntity->Components().GetAt(i);
      if (pComp->GetComponentName()==NULL || (pComp->m_iComponentFlags&VIS_OBJECTCOMPONENTFLAG_EXPOSETOEDITOR)==0)
        continue;
      // component still exists? The re-use it

      // first lookup by native pointer
      ShapeComponent ^existing = components->GetComponentByNativePointer(IntPtr(pComp));
      // or by unique string:
      if (existing==nullptr)
        existing = components->GetComponentByName(ConversionUtils::VStringToString(pComp->GetComponentName()));
      if (existing!=nullptr)
      {
        existing->Tagged = false;
        existing->NativePtr = IntPtr(pComp);
        // set the properties in the C# object, because these are the correct ones
        EditorManager::EngineManager->ComponentClassManager->SetNativeComponentProperty(existing, nullptr);
      }
      else
      {
        existing = ConversionUtils::ComponentFromNative(owner, pComp);
        components->Add(existing);
      }
    }

    // remove all that are still tagged
    components->RemoveWithTaggedStatus(true);
  }


  void EngineInstanceEntity::SetClipDistances(float fNear, float fFar) 
  {
    VisBaseEntity_cl *pEnt = GetEntity();
    if (pEnt==NULL) 
      return; 
    pEnt->SetNearClipDistance(fNear); 
    pEnt->SetFarClipDistance(fFar);
    pEnt->SetClipMode((fNear>0.f || fFar>0.f) ? VIS_LOD_TEST_BOUNDINGBOX :VIS_LOD_TEST_NONE);
  }


  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // Special EngineInstanceCubemapEntity implementation
  /////////////////////////////////////////////////////////////////////////////////////////////////////
    
  EngineInstanceCubemapEntity::EngineInstanceCubemapEntity(Shape3D ^shape) : EngineInstanceEntity("CubeMapHandle_cl", "gizmo_link.model", shape, nullptr, true)
  {
    m_bExport = true;
    SetPreviewVisible(false);
    VisBaseEntity_cl *pEntity = GetEntity();
    VDynamicMesh *pModel = pEntity->GetMesh();
    pEntity->SetLightInfluenceBitMask(0);
    pModel->SetAllowShaderAssignment(false); // also never update shader assignment
    pModel->SetSupportMaterialEditing(false); // dont show in the material editor
  }
  

  void EngineInstanceCubemapEntity::DisposeObject()
  {
    EngineInstanceEntity::DisposeObject();
  }
 
  void EngineInstanceCubemapEntity::SetPreviewVisible(bool bStatus)
  {
    VisBaseEntity_cl *pEntity = GetEntity();
    pEntity->SetVisibleBitmask(bStatus ? 1:0);
  }

  void EngineInstanceCubemapEntity::SetRendererConfig(String^ rendererConfig)
  {
    VString configFile;
    ConversionUtils::StringToVString(rendererConfig, configFile);

    IVFileInStream* pStream = Vision::File.Open(configFile.AsChar());

    CubeMapHandle_cl* pCubeMap = static_cast<CubeMapHandle_cl*>(GetEntity());

    if(pStream)
    {
      IVRendererNode* pNode = IVRendererNode::ReadFromStream(pStream);
      pCubeMap->SetRendererNode(pNode);
      pStream->Close();
    }
    else
    {
      pCubeMap->SetRendererNode(NULL);
    }
  }


  void EngineInstanceCubemapEntity::AssignCubemapShader(const char *szCubemapKey)
  {
    if (!szCubemapKey || !szCubemapKey[0])
      return;

    char szParamStr[1024];
    sprintf(szParamStr,"Cubemap=%s;PassType=%s;",szCubemapKey, VPassTypeToString(VPT_TransparentPass));
    VisBaseEntity_cl *pEntity = GetEntity();
    Vision::Shaders.LoadShaderLibrary("\\Shaders\\Cubemap.ShaderLib",SHADERLIBFLAG_HIDDEN);
    VCompiledTechnique *pTechnique = Vision::Shaders.CreateTechnique("Cubemap",szParamStr);
    VDynamicMesh *pModel = pEntity->GetMesh();
    if (pModel && pTechnique)
    {
      VisShaderSet_cl *pSet = new VisShaderSet_cl();
      pSet->BuildForDynamicMesh(pModel,NULL,pTechnique);
      pEntity->SetShaderSet(pSet);
    }
  }

  void EngineInstanceCubemapEntity::SetBlurPasses(int iBlurPasses)
  {
    CubeMapHandle_cl *pEntity = (CubeMapHandle_cl *)GetEntity();
    pEntity->SetBlurPasses(iBlurPasses);
  }

  void EngineInstanceCubemapEntity::SetAutoGenMipMaps(bool bStatus)
  {
    CubeMapHandle_cl *pEntity = (CubeMapHandle_cl *)GetEntity();
    pEntity->SetAutoGenMipMaps(bStatus);
  }

  void EngineInstanceCubemapEntity::SetCubemapKey(String ^key, int iEdgeSize)
  {
    VString sKey;
    ConversionUtils::StringToVString(key,sKey);
    CubeMapHandle_cl *pEntity = (CubeMapHandle_cl *)GetEntity();
    pEntity->SetCubemapKey(sKey,iEdgeSize);
    AssignCubemapShader(sKey);
  }

  void EngineInstanceCubemapEntity::SetRenderingType(CubeMapRenderingType_e eRenderingType)
  {
    CubeMapHandle_cl *pEntity = (CubeMapHandle_cl *)GetEntity();
    switch(eRenderingType)
    {
      case CubeMapRenderingType_e::RendererNode:
        pEntity->SetRenderLoop(NULL);
        break;

      case CubeMapRenderingType_e::Specular:
        pEntity->SetRendererNode(NULL);
        pEntity->SetRenderLoop(new VFakeSpecularGenerator);
        break;

      case CubeMapRenderingType_e::Scene:
        pEntity->SetRendererNode(NULL);
        pEntity->SetRenderLoop(new VisionRenderLoop_cl);
        break;
    }
  }

  void EngineInstanceCubemapEntity::SetSpecularPower(float fSpecularPower)
  {
    CubeMapHandle_cl *pEntity = (CubeMapHandle_cl *)GetEntity();
    if(pEntity->GetRenderLoop()->GetTypeId() == VFakeSpecularGenerator::GetClassTypeId())
    {
      static_cast<VFakeSpecularGenerator*>(pEntity->GetRenderLoop())->SetSpecularPower(fSpecularPower);
    }
  }

  void EngineInstanceCubemapEntity::SetRenderFilterMask(unsigned int iMask)
  {
    CubeMapHandle_cl *pEntity = (CubeMapHandle_cl *)GetEntity();
    pEntity->SetRenderFilterMask(iMask);
  }

  void EngineInstanceCubemapEntity::SetClipPlanes(float fNear, float fFar)
  {
    CubeMapHandle_cl *pEntity = (CubeMapHandle_cl *)GetEntity();
    pEntity->SetClipPlanes(fNear, fFar);
  }

  void EngineInstanceCubemapEntity::SetUpdateParams(bool bContinuous, float fInterval, int iCount, bool bAlternate)
  {
    CubeMapHandle_cl *pEntity = (CubeMapHandle_cl *)GetEntity();
    pEntity->SetContinuousUpdate(bContinuous);
    pEntity->SetUpdateParams(fInterval, iCount);
    pEntity->SetAlternatingUpdate(bAlternate);
  }


  void EngineInstanceCubemapEntity::UpdateCubemap()
  {
    CubeMapHandle_cl *pEntity = (CubeMapHandle_cl *)GetEntity();
    pEntity->Invalidate();
  }


  bool EngineInstanceCubemapEntity::OnExport(SceneExportInfo ^info) 
  {
    if (!m_bExport) // do not save as entity
      return true;

    VisBaseEntity_cl *pEntity = GetEntity();
    if (pEntity == NULL)
      return true;

    // export without model assigned:
    VDynamicMeshPtr spOldModel = pEntity->GetMesh();
    pEntity->SetMesh((VDynamicMesh *)NULL);
    VArchive &ar = *((VArchive *)info->NativeShapeArchivePtr.ToPointer());
    ar << pEntity;
    pEntity->SetMesh(spOldModel);
    return true;
  }


  bool EngineInstanceCubemapEntity::SaveToFile(String ^filename)
  {
    CubeMapHandle_cl *pEntity = (CubeMapHandle_cl *)GetEntity();
    if (!pEntity || !pEntity->GetCubeMapTexture())
      return false;

    String ^absFilename = EditorManager::Project->MakeAbsolute(filename);
    VString sFilename,sAbsFilename;
    ConversionUtils::StringToVString(filename,sFilename);
    ConversionUtils::StringToVString(absFilename,sAbsFilename);
    HRESULT res = S_FALSE;

    ManagedBase::RCS::GetProvider()->EditFile(absFilename);

#if defined(_VR_DX11) && defined(_VR_DX11_SUPPORTS_D3DX)
    if (!pEntity->GetCubeMapTexture()->GetD3DResource())
      return false;

    ID3D11Texture2D* originalTexture = static_cast<ID3D11Texture2D*>(pEntity->GetCubeMapTexture()->GetD3DResource());
    D3D11_TEXTURE2D_DESC desc;
    originalTexture->GetDesc(&desc);

    // Dynamically generated cube maps may have a typeless format which cannot be saved as a legacy DDS
    if(desc.Format == DXGI_FORMAT_B8G8R8A8_TYPELESS)
    {
      // Copy the original cubemap into a typed one to force saving as a non-DX10 DDS file
      desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;

      ID3D11Texture2D* newTexture;
      res = VVideo::GetD3DDevice()->CreateTexture2D(&desc, NULL, &newTexture);
      if(res != S_OK)
        return false;

      VVideo::GetD3DDeviceContext()->CopyResource(newTexture, originalTexture);
      
      res = D3DX11SaveTextureToFile(VVideo::GetD3DDeviceContext(), newTexture, D3DX11_IFF_DDS, sAbsFilename.AsChar());
      newTexture->Release();
    } else {
      res = D3DX11SaveTextureToFile(VVideo::GetD3DDeviceContext(), originalTexture, D3DX11_IFF_DDS, sAbsFilename.AsChar());
    }
#elif !defined(_VR_DX11)
    if (!pEntity->GetCubeMapTexture()->GetD3DInterface())
      return false;
    res = D3DXSaveTextureToFile(sAbsFilename.AsChar(),D3DXIFF_DDS,pEntity->GetCubeMapTexture()->GetD3DInterface(),NULL);
#endif
    if (res!=S_OK)
      return false;
    /*    
    VTextureManager& manager = Vision::TextureManager.GetManager();
    VTextureObject *pCubeTex = manager.LoadCubemapTextureFromFile(filename);
    if (!pCubeTex)
      return false;

    pCubeTex->CheckFileModified();
*/
    VFileTime newFileTime;
    if (VFileHelper::GetModifyTime(sAbsFilename.AsChar(), newFileTime))
      pEntity->GetCubeMapTexture()->SetTimeStamp(newFileTime);

    ManagedBase::RCS::GetProvider()->AddFile(absFilename, true);
    return true;
  }



  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // class EngineInstanceBoneProxy
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  EngineInstanceBoneProxy::EngineInstanceBoneProxy()
  {
    m_pBoneProxy = new VSkeletalBoneProxyObject();
    m_pBoneProxy->AddRef();
  }
  
  void EngineInstanceBoneProxy::DisposeObject()
  {
    V_SAFE_DISPOSEANDRELEASE(m_pBoneProxy)
  }

  
  bool EngineInstanceBoneProxy::OnExport(SceneExportInfo ^info) 
  {
    return true; // only export it implicitly as a parent of something 
    /*
    if (m_pBoneProxy)
    {
      VArchive &ar = *((VArchive *)info->NativeShapeArchivePtr.ToPointer());
      ar << m_pBoneProxy;
    }
    return true;
    */
  }

  VisBaseEntity_cl *EngineInstanceBoneProxy::GetParentEntity()
  {
    if (!m_pBoneProxy)
      return NULL;
    return (VisBaseEntity_cl *)m_pBoneProxy->GetParent();
  }

  bool EngineInstanceBoneProxy::GetLocalBoundingBox(BoundingBox ^%bbox) 
  {
    if (m_pBoneProxy==nullptr || m_pBoneProxy->m_iBoneIndex<0)
      return false;
    VisBaseEntity_cl *pEntity = GetParentEntity();
    if (pEntity==nullptr || !pEntity->HasMesh() || pEntity->GetMesh()->GetSkeleton()==nullptr)
      return false;

    VisSkeletalBone_cl *pBone = pEntity->GetMesh()->GetSkeleton()->GetBone(m_pBoneProxy->m_iBoneIndex);
    const hkvAlignedBBox &b(pBone->m_BoundingBox);
    (*bbox).Set(b.m_vMin.x,b.m_vMin.y,b.m_vMin.z, b.m_vMax.x,b.m_vMax.y,b.m_vMax.z );

    return true;
  }


  void EngineInstanceBoneProxy::RenderShape(VisionViewBase ^view, ShapeRenderMode mode)
  {
  }

  bool EngineInstanceBoneProxy::GetPosition(Vector3F %enginePosition)
  {
    if (!m_pBoneProxy)
      return false;
    m_pBoneProxy->UpdateBoneBinding();
    return IEngineInstanceObject3D::GetPosition(enginePosition);
  }

  bool EngineInstanceBoneProxy::GetOrientation(Vector3F %engineOrientation)
  {
    if (!m_pBoneProxy)
      return false;
    m_pBoneProxy->UpdateBoneBinding();
    return IEngineInstanceObject3D::GetOrientation(engineOrientation);
  }


  void EngineInstanceBoneProxy::AttachToEntity(EngineInstanceEntity ^pParent, String ^boneName)
  {
    if (m_pBoneProxy==nullptr || pParent==nullptr || pParent->GetEntity()==nullptr || String::IsNullOrEmpty(boneName))
      return;

    VString sBone;
    ConversionUtils::StringToVString(boneName,sBone);
    VisBaseEntity_cl *pEntity = pParent->GetEntity();
    m_pBoneProxy->AttachToEntityBone(pEntity,sBone);
  }

  void EngineInstanceBoneProxy::SetObjectKey(String ^key)
  {
    if (m_pBoneProxy==nullptr)
      return;
    VString sKey;
    ConversionUtils::StringToVString(key,sKey);
    if (sKey.IsEmpty())
      m_pBoneProxy->SetObjectKey(NULL);
    else
      m_pBoneProxy->SetObjectKey(sKey);
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
