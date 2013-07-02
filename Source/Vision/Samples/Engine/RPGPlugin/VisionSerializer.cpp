/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#include <Vision/Samples/Engine/RPGPlugin/Precompiled.h>

#include <Vision/Samples/Engine/RPGPlugin/VisionSerializer.h>

void RPG_VisionSerializer::WriteVariableList(VTypedObject const *typedObj, VArchive& ar)
{
  VARIABLE_LIST const *const varList = typedObj->GetVariableList();
  VASSERT(varList);
  if(varList)
  {
    int const numVars = varList->GetSize();
    ar << numVars;

    for(VARIABLE_ELEM const *elem = varList->first; elem; elem = elem->next)
    {
      VisVariable_cl const *const var = elem->value;
      {
        ar << var->name;
        ar << var->type;

        switch(var->type)
        {
        case VULPTYPE_REFERENCED_OBJECT:
          {
            VTypedObjectReference const *const typedObjectReference = 
              (VTypedObjectReference const*)((char const*)typedObj + var->clsOffset);
            ar << typedObjectReference->GetReferencedObject();
          }
          break;

        default:
          {
            char valueString[1000];
            const_cast<VisVariable_cl*>(var)->GetValue(const_cast<VTypedObject*>(typedObj), valueString);
            ar << valueString;
          }
        }
      }
    }
  }
}

void RPG_VisionSerializer::ReadVariableList(VTypedObject *typedObj, VArchive /*const*/& ar)
{
  int numVars;
  ar >> numVars;
  for(int i = 0; i < numVars; ++i)
  {
    VString varName;
    ar >> varName;
      
    int varType;
    ar >> varType;

    VisVariable_cl *const var = typedObj->GetVariable(varName);
    VASSERT(var && var->type == varType);
    if(var && var->type == varType)
    {
      switch(varType)
      {
      case VULPTYPE_REFERENCED_OBJECT:
        {
          VTypedObject *varObj;
          ar >> varObj;

          VTypedObjectReference *const typedObjectReference = (VTypedObjectReference*)((char*)typedObj + var->clsOffset);
          typedObjectReference->SetReferencedObject(varObj);
        }
        break;

      default:
        {
          VString varValue;
          ar >> varValue;

          var->SetValue(typedObj, varValue.AsChar());
        }
      }
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
