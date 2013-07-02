/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/VScriptIncludes.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/VBitmask.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/VisionLuaModule_wrapper.hpp>
#include <Vision/Runtime/Base/System/Memory/VMemDbg.hpp>

SwigTypeDataAccessor::SwigTypeDataAccessor(const char * szSwigTypeInfo)
: IVRSDUserDataAccessor(szSwigTypeInfo)
{
}

//Internal helper to decode integer user input from vRSD,
//supports int reading for formats like 0xabd, 0xABD, 123, cde, ACD
bool ReadHexOrDecValue(const char * szValue, int &iValue, int iMin = INT_MIN, int iMax = INT_MAX)
{
  int val=0;
  //0xabd
  if(sscanf(szValue, "0x%x", &val)==1)
  {
    if(iMin!=INT_MIN && val<iMin) val = iMin;
    else if(iMax!=INT_MAX && val>iMax) val = iMax;

    iValue = val;
    return true;
  }
  //0xABD
  else if(sscanf(szValue, "0x%X", &val)==1)
  {
    if(iMin!=INT_MIN && val<iMin) val = iMin;
    else if(iMax!=INT_MAX && val>iMax) val = iMax;

    iValue = val;
    return true;
  }
  //567
  else if(sscanf(szValue, "%i", &val)==1)
  {
    if(iMin!=INT_MIN && val<iMin) val = iMin;
    else if(iMax!=INT_MAX && val>iMax) val = iMax;

    iValue = val;
    return true;
  }
  //dea
  else if(sscanf(szValue, "%x", &val)==1)
  {
    if(iMin!=INT_MIN && val<iMin) val = iMin;
    else if(iMax!=INT_MAX && val>iMax) val = iMax;

    iValue = val;
    return true;
  }
  //FED
  else if(sscanf(szValue, "%X", &val)==1)
  {
    if(iMin!=INT_MIN && val<iMin) val = iMin;
    else if(iMax!=INT_MAX && val>iMax) val = iMax;

    iValue = val;
    return true;
  }

  return false;
}

//Update data fields via a string (from vRSD)
bool SwigTypeDataAccessor::UpdateMemberFromString(void* pUserDataPointer, void *pEnvironment, const char* szMemberName, const char* szUpdateString)
{
  if(!pUserDataPointer|| !pEnvironment) return false;

  swig_lua_userdata* usr = (swig_lua_userdata*)pUserDataPointer;

  //consider a change to LUA_TestUserData(L,2,"VisObject3D_cl")

  //let's check if we can cast to a VisObject3D_cl
  swig_cast_info *cast = VSWIG_TypeCheckStruct(usr->type,SWIGTYPE_p_VisObject3D_cl);

  if (cast)
  {
    int iNewMemory = 0; // SWIG Internal: iNewMemory handling not yet implemented -> has to remain 0
    VisObject3D_cl* pObject3D = (VisObject3D_cl *)VSWIG_TypeCast(cast,usr->ptr,&iNewMemory);
    if(!pObject3D || iNewMemory) return false;  //cast failed

    if(!strcmp("Key", szMemberName))
    {
      pObject3D->SetObjectKey(szUpdateString);
      return true;
    }
    else if(!strcmp("Position", szMemberName))
    {
      float x=0,y=0,z=0;
      if(sscanf(szUpdateString, "%f/%f/%f", &x,&y,&z)==3)
      {
        pObject3D->SetPosition(x,y,z);
        return true;
      }
    }
    else if(!strcmp("Orientation", szMemberName))
    {
      float x=0,y=0,z=0;
      if(sscanf(szUpdateString, "%f/%f/%f", &x,&y,&z)==3)
      {
        pObject3D->SetOrientation(x,y,z);
        return true;
      }
    }
    else if(!strcmp("Motion Delta (World Space)", szMemberName))
    {
      float x=0,y=0,z=0;
      if(sscanf(szUpdateString, "%f/%f/%f", &x,&y,&z)==3)
      {
        pObject3D->SetMotionDeltaWorldSpace(hkvVec3 (x,y,z));
        return true;
      }
    }
    else if(!strcmp("Motion Delta (Local Space)", szMemberName))
    {
      float x=0,y=0,z=0;
      if(sscanf(szUpdateString, "%f/%f/%f", &x,&y,&z)==3)
      {
        pObject3D->SetMotionDeltaLocalSpace(hkvVec3 (x,y,z));
        return true;
      }
    }
    else if(!strcmp("Rotation Delta", szMemberName))
    {
      float x=0,y=0,z=0;
      if(sscanf(szUpdateString, "%f/%f/%f", &x,&y,&z)==3)
      {
        pObject3D->SetRotationDelta(hkvVec3 (x,y,z));
        return true;
      }
    }

    return false;
  }

  //try other primitive data types...

  //hkvVec3
  cast = VSWIG_TypeCheckStruct(usr->type,SWIGTYPE_p_hkvVec3);

  if(cast)
  {
    int iNewMemory = 0; // SWIG Internal: iNewMemory handling not yet implemented -> has to remain 0
    hkvVec3* pVec = (hkvVec3* )VSWIG_TypeCast(cast,usr->ptr,&iNewMemory);
    if(!pVec || iNewMemory) return false; //cast failed

    if(!_stricmp("X", szMemberName))
    {
      float val=0;
      if(sscanf(szUpdateString, "%f", &val)==1)
      {
        pVec->x = val;
        return true;
      }
    }
    else if(!_stricmp("Y", szMemberName))
    {
      float val=0;
      if(sscanf(szUpdateString, "%f", &val)==1)
      {
        pVec->y = val;
        return true;
      }
    }
    else if(!_stricmp("Z", szMemberName))
    {
      float val=0;
      if(sscanf(szUpdateString, "%f", &val)==1)
      {
        pVec->z = val;
        return true;
      }
    }
    //else if(!_stricmp("W", szMemberName))
    //{
    //  float val=0;
    //  if(sscanf(szUpdateString, "%f", &val)==1)
    //  {
    //    pVec->w = val;
    //    return true;
    //  }
    //}

    return false;
  }

  //VBitmask
  cast = VSWIG_TypeCheckStruct(usr->type,SWIGTYPE_p_VBitmask);

  if(cast)
  {
    int iNewMemory = 0; // SWIG Internal: iNewMemory handling not yet implemented -> has to remain 0
    VBitmask* pMask = (VBitmask *)VSWIG_TypeCast(cast,usr->ptr,&iNewMemory);
    if(!pMask || iNewMemory) return false; //cast failed

    if(!strcmp("Mask", szMemberName))
    {
      int val=0;
      if(ReadHexOrDecValue(szUpdateString, val))
      {
        pMask->RawSet((unsigned int)val);
        return true;
      }
    }
    
    return false;
  }

  //VColorRef
  cast = VSWIG_TypeCheckStruct(usr->type,SWIGTYPE_p_VColorRef);

  if(cast)
  {
    int iNewMemory = 0;
    VColorRef* pColor = (VColorRef *)VSWIG_TypeCast(cast,usr->ptr,&iNewMemory);
    if(!pColor || iNewMemory)  return false; //cast failed

    if(!strcmp("Red", szMemberName))
    {
      int val=0;
      if(ReadHexOrDecValue(szUpdateString, val, 0, 255))
      {
        pColor->r = (UBYTE) val;
        return true;
      }
    }
    else if(!strcmp("Green", szMemberName))
    {
      int val=0;
      if(ReadHexOrDecValue(szUpdateString, val, 0, 255))
      {
        pColor->g = (UBYTE) val;
        return true;
      }
    }
    else if(!strcmp("Blue", szMemberName))
    {
      int val=0;
      if(ReadHexOrDecValue(szUpdateString, val, 0, 255))
      {
        pColor->b = (UBYTE) val;
        return true;
      }
    }
    else if(!strcmp("Alpha", szMemberName))
    {
      int val=0;
      if(ReadHexOrDecValue(szUpdateString, val, 0, 255))
      {
        pColor->a = (UBYTE) val;
        return true;
      }
    }

    return false;
  }

  // hkvAlignedBBox
  cast = VSWIG_TypeCheckStruct(usr->type,SWIGTYPE_p_hkvAlignedBBox);

  if(cast)
  {
    int iNewMemory = 0;
    hkvAlignedBBox* pBox = (hkvAlignedBBox *)VSWIG_TypeCast(cast,usr->ptr,&iNewMemory);
    if(!pBox || iNewMemory) return false; //cast failed

    if(!strcmp("Min", szMemberName))
    {
      float x=0,y=0,z=0;
      if(sscanf(szUpdateString, "%f/%f/%f", &x,&y,&z)==3)
      {
        pBox->m_vMin.set(x,y,z);
        return true;
      }
    }
    else if(!strcmp("Max", szMemberName))
    {
      float x=0,y=0,z=0;
      if(sscanf(szUpdateString, "%f/%f/%f", &x,&y,&z)==3)
      {
        pBox->m_vMax.set(x,y,z);
        return true;
      }
    }

    return false;
  }

  cast = VSWIG_TypeCheckStruct(usr->type,SWIGTYPE_p_hkvMat3);

  if(cast)
  {
    int iNewMemory = 0;
    hkvMat3 * pMatrix = (hkvMat3 *)VSWIG_TypeCast(cast,usr->ptr,&iNewMemory);
    if(!pMatrix || iNewMemory) return false; //cast failed

    hkvVec3 v1,v2,v3;
    pMatrix->getAxisXYZ(&v1,&v2,&v3);

    if(!strcmp("Vector1: m[0,3,6]", szMemberName))
    {
      float x=0,y=0,z=0;
      if(sscanf(szUpdateString, "%f/%f/%f", &x,&y,&z)==3)
      {
        v1.set(x,y,z);
        pMatrix->setAxisXYZ (v1,v2,v3);
        return true;
      }
    }
    else if(!strcmp("Vector2: m[1,4,7]", szMemberName))
    {
      float x=0,y=0,z=0;
      if(sscanf(szUpdateString, "%f/%f/%f", &x,&y,&z)==3)
      {
        v2.set(x,y,z);
        pMatrix->setAxisXYZ (v1,v2,v3);
        return true;
      }
    }
    else if(!strcmp("Vector3: m[2,5,8]", szMemberName))
    {
      float x=0,y=0,z=0;
      if(sscanf(szUpdateString, "%f/%f/%f", &x,&y,&z)==3)
      {
        v3.set(x,y,z);
        pMatrix->setAxisXYZ (v1,v2,v3);
          return true;
      }
    }

    return false;
  }

  return false;
}

static VString hkvVec3_ToString (const hkvVec3& v)
{
  VString s;
  s.Format ("(%.2f | %.2f | %.2f)", v.x, v.y, v.z);
  return s;
}


bool SwigTypeDataAccessor::GetUserDataMembers(void* pUserDataPointer, void *pEnvironment, DynArray_cl<VRSDScriptSymbol>& dynArrMembers, unsigned int& iMemberCount)
{
  if(!pUserDataPointer|| !pEnvironment) return false;

  lua_State *L = (lua_State *)pEnvironment;

  //Attach "dynamic properties" from Lua
  int iDynProperties = GetMembersFromGlobal(pUserDataPointer, L, dynArrMembers, iMemberCount);

  //let's check if we can cast to a VisTypedEngineObject
  swig_lua_userdata* usr = (swig_lua_userdata*)pUserDataPointer;
  swig_cast_info *cast = VSWIG_TypeCheckStruct(usr->type,SWIGTYPE_p_VTypedObject);

  if (cast)
  {
    //cast is possible, so we do it
    int iNewMemory = 0; // SWIG Internal: iNewMemory handling not yet implemented -> has to remain 0
    VTypedObject* pObject = (VTypedObject *)VSWIG_TypeCast(cast,usr->ptr,&iNewMemory);
    if(!pObject || iNewMemory)
    {
      AddMembersAndAttributes((swig_lua_class *) usr->type->clientdata, L,dynArrMembers, iMemberCount);
      return false; //cast failed
    }

    //attach all variables to the object
    int iProps = pObject->GetNumVariables();
    char szBuffer[512];

    for(int i=0;i<iProps;i++)
    {
      const char *szVarName = pObject->GetVariableName(i);
      if(szVarName==NULL)
        continue;

      VisVariable_cl *pVar = pObject->GetVariable(szVarName);
      if(pVar==NULL)
        continue;

      //at this point we have a valid variable
      pObject->GetVariableValue(pVar->GetName(), szBuffer);

      switch(pVar->type)
      {
        case VULPTYPE_BOOL:
          Helper_AddMember(dynArrMembers, iMemberCount, szVarName, szBuffer, VRSDScriptSymbol::SYMBOL_BOOLEAN, false);
          break;

        case VULPTYPE_ENUM:
        case VULPTYPE_INT:
        case VULPTYPE_UINT:
        case VULPTYPE_BITMASK:
        case VULPTYPE_FLOAT:
        case VULPTYPE_DOUBLE:
          Helper_AddMember(dynArrMembers, iMemberCount, szVarName, szBuffer, VRSDScriptSymbol::SYMBOL_NUMBER, false);
          break;

        default:
          Helper_AddMember(dynArrMembers, iMemberCount, szVarName, szBuffer, VRSDScriptSymbol::SYMBOL_STRING, false);
          break;
      }
    }

    //attach components
    //check if it is a object component...
    if(pObject->IsOfType(V_RUNTIME_CLASS(VisTypedEngineObject_cl)))
    {
      VisTypedEngineObject_cl * pEngineObject = (VisTypedEngineObject_cl *)pObject;
      int iComps = pEngineObject->Components().Count();
      for(int i=0;i<iComps;i++)
      {
        IVObjectComponent *pComp = pEngineObject->Components().GetAt(i);
        const char * szIdString = pComp->GetIDString(pComp->GetComponentID());

        VString nameBuffer, valueBuffer;

        nameBuffer.Format(" - Component [%d] (%s)", i, pComp->GetTypeId()->m_lpszClassName);

        if(szIdString==NULL)  valueBuffer.Format("Name: %s, ID: %d", pComp->GetComponentName(), pComp->GetComponentID());
        else                  valueBuffer.Format("Name: %s, ID: %s (%d)", pComp->GetComponentName(), szIdString, pComp->GetComponentID());

        Helper_AddMember(dynArrMembers, iMemberCount, nameBuffer.AsChar(), valueBuffer.AsChar(), 
          VRSDScriptSymbol::SYMBOL_STRING, false);
      }
    

      //check if it is a object component...
      if(pEngineObject->IsOfType(V_RUNTIME_CLASS(IVObjectComponent)))
      {
        IVObjectComponent* pComponent = (IVObjectComponent *)pEngineObject;

        VString sBuffer;
        const char * szIdString = pComponent->GetIDString(pComponent->GetComponentID());

        if(szIdString==NULL)  sBuffer.Format("%d", pComponent->GetComponentID());
        else                  sBuffer.Format("%s (%d)",szIdString, pComponent->GetComponentID());
      
        Helper_AddMember(dynArrMembers, iMemberCount, "Component ID",  sBuffer.AsChar(), 
          VRSDScriptSymbol::SYMBOL_NUMBER, false);
        Helper_AddMember(dynArrMembers, iMemberCount, "Component Name", pComponent->GetComponentName(), 
          VRSDScriptSymbol::SYMBOL_STRING, false);
        return true;
      }

      //try to cast the object to VisObject3D
      if(!pEngineObject->IsOfType(V_RUNTIME_CLASS(VisObject3D_cl)))
      {
        AddMembersAndAttributes((swig_lua_class *) usr->type->clientdata, L,dynArrMembers, iMemberCount);
        return true;  //cast failed, but we already have a few members collected
      }
      VisObject3D_cl* pObject3D = (VisObject3D_cl *)pEngineObject;

      //add VisObject3D members (all as string for now)
      Helper_AddMember(dynArrMembers, iMemberCount, "Key", pObject3D->GetObjectKey(), VRSDScriptSymbol::SYMBOL_STRING);

      hkvVec3 vec;
      pObject3D->GetPosition(vec);
      Helper_AddMember(dynArrMembers, iMemberCount, "Position", hkvVec3_ToString (vec).AsChar (), VRSDScriptSymbol::SYMBOL_STRING);

      pObject3D->GetOrientation(vec);
      Helper_AddMember(dynArrMembers, iMemberCount, "Orientation", hkvVec3_ToString (vec).AsChar (), VRSDScriptSymbol::SYMBOL_STRING);

      //optionally add delta values if != 0 
      vec = pObject3D->GetMotionDeltaWorldSpace();
      Helper_AddMember(dynArrMembers, iMemberCount, "Motion Delta (World Space)", hkvVec3_ToString (vec).AsChar (), VRSDScriptSymbol::SYMBOL_STRING);

      vec = pObject3D->GetMotionDeltaLocalSpace();
      Helper_AddMember(dynArrMembers, iMemberCount, "Motion Delta (Local Space)", hkvVec3_ToString (vec).AsChar (), VRSDScriptSymbol::SYMBOL_STRING);

      vec = pObject3D->GetRotationDelta();
      Helper_AddMember(dynArrMembers, iMemberCount, "Rotation Delta", hkvVec3_ToString (vec).AsChar (), VRSDScriptSymbol::SYMBOL_STRING);

      //try to cast the object to VisBaseEntity_cl
      if(!pEngineObject->IsOfType(V_RUNTIME_CLASS(VisBaseEntity_cl)))
      {
        AddMembersAndAttributes((swig_lua_class *) usr->type->clientdata, L,dynArrMembers, iMemberCount);
        return true;  //cast failed, but we already have a few members collected
      }
      VisBaseEntity_cl* pEntity = (VisBaseEntity_cl *)pEngineObject;

      //add mesh name if present
      VDynamicMesh *pMesh = pEntity->GetMesh();
      Helper_AddMember(dynArrMembers, iMemberCount, "Mesh", pMesh==NULL?"[not set]":pMesh->GetFilename(), VRSDScriptSymbol::SYMBOL_STRING, false);
      VString buffer;

      //add uid
      __int64 uid = pEntity->GetUniqueID();
      buffer.Format("%08x:%08x", (int)(uid>>32), (int)uid);
      Helper_AddMember(dynArrMembers, iMemberCount, "Unique ID", buffer.AsChar(), VRSDScriptSymbol::SYMBOL_STRING, false);
    
      AddMembersAndAttributes((swig_lua_class *) usr->type->clientdata, L,dynArrMembers, iMemberCount);
    }
    return true;
  }

  //try primitive data types...

  //hkvVec3
  cast = VSWIG_TypeCheckStruct(usr->type,SWIGTYPE_p_hkvVec3);

  if(cast)
  {
    int iNewMemory = 0; // SWIG Internal: iNewMemory handling not yet implemented -> has to remain 0
    hkvVec3* pVec = (hkvVec3* )VSWIG_TypeCast(cast,usr->ptr,&iNewMemory);
    
    if(!pVec || iNewMemory)
    {
      AddMembersAndAttributes((swig_lua_class *) usr->type->clientdata, L,dynArrMembers, iMemberCount);
      return false; //cast failed
    }

    VString sBuffer;
    sBuffer.Format("%f", pVec->x);
    Helper_AddMember(dynArrMembers, iMemberCount, "X", sBuffer.AsChar(), VRSDScriptSymbol::SYMBOL_NUMBER);
    sBuffer.Format("%f", pVec->y);
    Helper_AddMember(dynArrMembers, iMemberCount, "Y", sBuffer.AsChar(), VRSDScriptSymbol::SYMBOL_NUMBER);
    sBuffer.Format("%f", pVec->z);
    Helper_AddMember(dynArrMembers, iMemberCount, "Z", sBuffer.AsChar(), VRSDScriptSymbol::SYMBOL_NUMBER);
    //sBuffer.Format("%f", pVec->w);
    //Helper_AddMember(dynArrMembers, iMemberCount, "W", sBuffer.AsChar(), VRSDScriptSymbol::SYMBOL_NUMBER);

    return true;
  }

  //VBitmask
  cast = VSWIG_TypeCheckStruct(usr->type,SWIGTYPE_p_VBitmask);

  if(cast)
  {
    int iNewMemory = 0; // SWIG Internal: iNewMemory handling not yet implemented -> has to remain 0
    VBitmask* pMask = (VBitmask *)VSWIG_TypeCast(cast,usr->ptr,&iNewMemory);
    
    if(!pMask || iNewMemory)
    {
      AddMembersAndAttributes((swig_lua_class *) usr->type->clientdata, L, dynArrMembers, iMemberCount);
      return false; //cast failed
    }

    VString sBuffer;
    sBuffer.Format("0x%x", pMask->Get());
    //the bitmask is no string, but otherwise vRSD will not accept hex values when updating...
    Helper_AddMember(dynArrMembers, iMemberCount, "Mask", sBuffer.AsChar(), VRSDScriptSymbol::SYMBOL_STRING);

    return true;
  }

  //VColorRef
  cast = VSWIG_TypeCheckStruct(usr->type,SWIGTYPE_p_VColorRef);

  if(cast)
  {
    int iNewMemory = 0;
    VColorRef* pColor = (VColorRef *)VSWIG_TypeCast(cast,usr->ptr,&iNewMemory);

    if(!pColor || iNewMemory)
    {
      AddMembersAndAttributes((swig_lua_class *) usr->type->clientdata, L, dynArrMembers, iMemberCount);
      return false; //cast failed
    }

    VString sBuffer;
    sBuffer.Format("0x%x", pColor->r);
    //well, the color vales are no string, but otherwise vRSD will not accept hex values when updating...
    Helper_AddMember(dynArrMembers, iMemberCount, "Red", sBuffer.AsChar(), VRSDScriptSymbol::SYMBOL_STRING);
    sBuffer.Format("0x%x", pColor->g);
    Helper_AddMember(dynArrMembers, iMemberCount, "Green", sBuffer.AsChar(), VRSDScriptSymbol::SYMBOL_STRING);
    sBuffer.Format("0x%x", pColor->b);
    Helper_AddMember(dynArrMembers, iMemberCount, "Blue", sBuffer.AsChar(), VRSDScriptSymbol::SYMBOL_STRING);
    sBuffer.Format("0x%x", pColor->a);
    Helper_AddMember(dynArrMembers, iMemberCount, "Alpha", sBuffer.AsChar(), VRSDScriptSymbol::SYMBOL_STRING);

    return true;
  }

  //hkvAlignedBBox
  cast = VSWIG_TypeCheckStruct(usr->type,SWIGTYPE_p_hkvAlignedBBox);

  if(cast)
  {
    int iNewMemory = 0;
    hkvAlignedBBox* pBox = (hkvAlignedBBox *)VSWIG_TypeCast(cast,usr->ptr,&iNewMemory);
    
    if(!pBox || iNewMemory)
    {
      AddMembersAndAttributes((swig_lua_class *) usr->type->clientdata, L, dynArrMembers, iMemberCount);
      return false; //cast failed
    }
    //handle vectors as string symbols in vRSD (update is more convenient)
    Helper_AddMember(dynArrMembers, iMemberCount, "Min", hkvVec3_ToString (pBox->m_vMin).AsChar (), VRSDScriptSymbol::SYMBOL_STRING);
    Helper_AddMember(dynArrMembers, iMemberCount, "Max", hkvVec3_ToString (pBox->m_vMax).AsChar (), VRSDScriptSymbol::SYMBOL_STRING);

    return true;
  }

  //hkvMat3 
  cast = VSWIG_TypeCheckStruct(usr->type,SWIGTYPE_p_hkvMat3);

  if(cast)
  {
    int iNewMemory = 0;
    hkvMat3 * pMatrix = (hkvMat3 *)VSWIG_TypeCast(cast,usr->ptr,&iNewMemory);
    
    if(!pMatrix || iNewMemory)
    {
      AddMembersAndAttributes((swig_lua_class *) usr->type->clientdata, L, dynArrMembers, iMemberCount);
      return false; //cast failed
    }

    hkvVec3 v1,v2,v3;
    pMatrix->getAxisXYZ(&v1,&v2,&v3);
    //handle vectors as string symbols in vRSD (update is more convenient)
    Helper_AddMember(dynArrMembers, iMemberCount, "Vector1: m[0,3,6]", hkvVec3_ToString (v1).AsChar (), VRSDScriptSymbol::SYMBOL_STRING);
    Helper_AddMember(dynArrMembers, iMemberCount, "Vector2: m[1,4,7]", hkvVec3_ToString (v2).AsChar (), VRSDScriptSymbol::SYMBOL_STRING);
    Helper_AddMember(dynArrMembers, iMemberCount, "Vector3: m[2,5,8]", hkvVec3_ToString (v3).AsChar (), VRSDScriptSymbol::SYMBOL_STRING);

    return true;
  }

  //backup - iterate over all types without special handling
  for(int i=0; swig_types[i]!=0;i++)
  {
    cast = VSWIG_TypeCheckStruct(usr->type, swig_types[i]);
    if(cast)
    {
      int iNewMemory = 0;
      void * pGeneric = VSWIG_TypeCast(cast,usr->ptr,&iNewMemory);

      if(pGeneric!=NULL && iNewMemory==0)
      {
        //we at least now that it is something swig related
        AddMembersAndAttributes((swig_lua_class *) usr->type->clientdata, L, dynArrMembers, iMemberCount);
        return true;
      }
    }
  }

  return false;
}

void SwigTypeDataAccessor::AddMembersAndAttributes(void *pSwigLuaClass, lua_State *L, DynArray_cl<VRSDScriptSymbol>& dynArrMembers, unsigned int& iMemberCount)
{
  VASSERT(pSwigLuaClass);
  VASSERT(L);

  swig_lua_class *clazz = (swig_lua_class *) pSwigLuaClass;

  if(clazz)
  {
    int iIndex = 0;
    if(clazz->attributes)
    {
      while(clazz->attributes[iIndex].name)
      {
        Helper_AddMember(dynArrMembers, iMemberCount, clazz->attributes[iIndex].name, clazz->name, VRSDScriptSymbol::SYMBOL_USERDATA, false);
        iIndex++;
      }
    }

    iIndex = 0;
    if(clazz->methods)
    {
      while(clazz->methods[iIndex].name)
      {
        Helper_AddMember(dynArrMembers, iMemberCount, clazz->methods[iIndex].name, clazz->name, VRSDScriptSymbol::SYMBOL_FUNCTION, false);
        iIndex++;
      }
    }

    iIndex = 0;
    if(clazz->bases)
    {
      while(clazz->bases[iIndex])
      {
        AddMembersAndAttributes(clazz->bases[iIndex], L,dynArrMembers, iMemberCount);
        iIndex++;
      }
    }
  }
}

int  SwigTypeDataAccessor::GetMembersFromGlobal(void* pUserDataPointer, lua_State *L, 
                                                DynArray_cl<VRSDScriptSymbol>& dynArrMembers, 
                                                unsigned int& iMemberCount)
{
  VASSERT(pUserDataPointer);
  VASSERT(L);

  int iCount = iMemberCount;

  // first key for the iteration
  lua_pushnil(L);

  while (lua_next(L, LUA_GLOBALSINDEX) != 0)
  {
    // after this the key is at -2 and the value at -1

    // we only want string fields and no other keys
    if (lua_isstring(L, -2))
    {  
      const char* pSymbolName = lua_tostring(L, -2);

      //filter "$node" prefix
      if(pSymbolName==strstr(pSymbolName, LUA_HIDDEN_GLOBAL_PREFIX))
      {
        void* pAdr = 0;
        char szBuffer[512];

        //the global variable is part of the current Lua item if the stored address matches
        //"$node-POINTER_ADDRESS-NAME_OF_THE_VAR$"
        int iAssignedValues = sscanf(pSymbolName, LUA_HIDDEN_GLOBAL_PREFIX"-%p-%s", &pAdr, szBuffer);

        //check if sscanf assigned the two required values and
        //if the address matches the current userdata item
        if(iAssignedValues!=2 || pAdr!=pUserDataPointer)
        {
          lua_pop(L,1);
          continue;
        }
        
        //remove trailing '$'
        if(strlen(szBuffer)>0) szBuffer[strlen(szBuffer)-1]=0;

        // if the variable is a table, get all child variables (one level deep only)
        if(lua_istable(L, -1))
        {
          // add a symbol for the table
          Helper_AddMember(dynArrMembers, iMemberCount, szBuffer, "table", VRSDScriptSymbol::SYMBOL_TABLE, false);
        }
        // numeric member variable
        else if(lua_type(L, -1) == LUA_TNUMBER)
        {
          char buffer[32];
          sprintf(buffer, "%f", lua_tonumber(L, -1));
          Helper_AddMember(dynArrMembers, iMemberCount, szBuffer, buffer, VRSDScriptSymbol::SYMBOL_NUMBER);
        }
        // string member variable
        else if(lua_type(L, -1) == LUA_TSTRING)
        {
          Helper_AddMember(dynArrMembers, iMemberCount, szBuffer, lua_tostring(L, -1), VRSDScriptSymbol::SYMBOL_STRING);
        }
        else if(lua_isfunction(L, -1))
        {
          Helper_AddMember(dynArrMembers, iMemberCount, szBuffer, "function", VRSDScriptSymbol::SYMBOL_FUNCTION, false);
        }
        else if(lua_isuserdata(L, -1))
        {
          char buffer[32];
          sprintf(buffer, "userdata:0x%p", lua_touserdata(L, -1));      
          Helper_AddMember(dynArrMembers, iMemberCount, szBuffer, buffer, VRSDScriptSymbol::SYMBOL_USERDATA, false);
        }
        else if(lua_isboolean(L, -1))
        {
          int iBoolVal = lua_toboolean(L, -1);
          Helper_AddMember(dynArrMembers, iMemberCount, szBuffer, iBoolVal ? "true" : "false", 
            VRSDScriptSymbol::SYMBOL_BOOLEAN);
        }
        else if(lua_isnil(L, -1))
        {
          Helper_AddMember(dynArrMembers, iMemberCount, szBuffer, "nil", VRSDScriptSymbol::SYMBOL_CLASS, false);
        }
      }
    }
    lua_pop(L, 1);  // remove the value, keep the key for the next iteration
  }

  return iMemberCount-iCount;
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
