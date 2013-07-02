/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef VLUA_APIDOC

%nodefaultctor VTypedObject;
%nodefaultdtor VTypedObject;

class VTypedObject
{
public:
  %extend {

    const char * GetType() const
    {
      return self->GetTypeId()->m_lpszClassName;
    }
    
    bool IsOfType(const char *szType)
    {
      return self->IsOfType(szType)==TRUE;
    }
    
    const char * GetPropertyType(const char * propName)
    {
      if(propName==NULL)
        return NULL; // avoid problems with nil values
      VisVariable_cl *pVar = self->GetVariable(propName);
      int type = pVar ? pVar->type : -1;
      switch (type) {
          case VULPTYPE_STRING:
          case VULPTYPE_VSTRING:
          case VULPTYPE_MODEL:
          case VULPTYPE_PSTRING:
          case VULPTYPE_ENTITY_KEY:
          case VULPTYPE_PRIMITIVE_KEY:
          case VULPTYPE_VERTEX_KEY:
          case VULPTYPE_LIGHTSOURCE_KEY:
          case VULPTYPE_WORLDANIMATION_KEY:
          case VULPTYPE_PATH_KEY:
            return "string";
          case VULPTYPE_ENUM:
          case VULPTYPE_INT:
          case VULPTYPE_FLOAT:
          case VULPTYPE_DOUBLE:
            return "number";
          case VULPTYPE_BOOL:	
            return "boolean";
          case VULPTYPE_VECTOR_INT:
          case VULPTYPE_VECTOR_FLOAT:
          case VULPTYPE_VECTOR_DOUBLE:
            return "hkvVec3";
          case VULPTYPE_BYTE_COLOR4:
            return "VColorRef";
          default:
            Vision::Error.Warning("Type of property '%s' is unknown in Lua.", propName);
            return NULL;
      }
    }
        
    bool operator == (const VTypedObject& other)
    {
      return self==&other;
    }
  }
  
  %rename(GetNumProperties) GetNumVariables() const;
  int GetNumVariables() const;
  
  %rename(UpdateProperty) SetVariable(const char * name, const char * value);
  bool SetVariable(const char * name, const char * value);
};

//Implement GetProperties native because it returns an array of strings
%native(VTypedObject_GetProperties) int VTypedObject_GetProperties(lua_State *L);
%{
  SWIGINTERN int VTypedObject_GetProperties(lua_State *L)
  {
    IS_MEMBER_OF(VTypedObject) //this will move this function to the method table of the specified class
    
    SWIG_CONVERT_POINTER(L, 1, VTypedObject, pTypedObject)

    lua_newtable(L); //create an empty table (or an array if you would like to see it this way)
    
    int iCount = pTypedObject->GetNumVariables();

    for(int i=0;i<iCount;++i)
    {
      const char * szName = pTypedObject->GetVariableName(i);
      
      lua_newtable(L);             // create a new sub table             stack: table, table, TOP      
      lua_pushstring(L, "Name");   // push the key                       stack: table, table, key, TOP
      lua_pushstring(L, szName);   // push the value                     stack: table, table, key, value, TOP
      lua_settable(L, -3);         // set key and value as entry         stack: table, table, TOP
      
      lua_pushstring(L, "Type");   // push the key                       stack: table, table, key, TOP
      
      switch(pTypedObject->GetVariable(szName)->type)
      {
        case VULPTYPE_STRING:
        case VULPTYPE_VSTRING:
        case VULPTYPE_MODEL:
        case VULPTYPE_PSTRING:
        case VULPTYPE_ENTITY_KEY:
        case VULPTYPE_PRIMITIVE_KEY:
        case VULPTYPE_VERTEX_KEY:
        case VULPTYPE_LIGHTSOURCE_KEY:
        case VULPTYPE_WORLDANIMATION_KEY:
        case VULPTYPE_PATH_KEY:
          lua_pushstring(L, "string");
          break;
        case VULPTYPE_ENUM:	
        case VULPTYPE_INT:	
        case VULPTYPE_FLOAT:
        case VULPTYPE_DOUBLE:
          lua_pushstring(L, "number");
          break;
        case VULPTYPE_BOOL:
          lua_pushstring(L, "boolean");
          break;
        case VULPTYPE_VECTOR_INT:
        case VULPTYPE_VECTOR_FLOAT:
        case VULPTYPE_VECTOR_DOUBLE:
          lua_pushstring(L, "hkvVec3");
          break;
        case VULPTYPE_BYTE_COLOR4:
          lua_pushstring(L, "VColorRef");
          break;
        default:
          lua_pushstring(L, "unknown");
          break;
      }
      //after switch:                                                 stack: table, table, key, value, TOP
      
      lua_settable(L, -3);      //set key and value as entry          stack: table, table, TOP
      lua_rawseti(L, -2, i+1);  //add to overall table                stack: table, TOP
    }
    
    return 1; //the table is always on the stack (even if empty)
  }
%}

//Implement GetProperty native because it has a mixed return value
%native(VTypedObject_GetProperty) int VTypedObject_GetProperty(lua_State *L);
%{
  SWIGINTERN int VTypedObject_GetProperty(lua_State *L)
  {
    IS_MEMBER_OF(VTypedObject) //this will move this function to the method table of the specified class
    
    DECLARE_ARGS_OK;
  
    SWIG_CONVERT_POINTER(L, 1, VTypedObject, pTypedObject)

    GET_ARG(2, const char *, pszName);
    lua_pop(L,2);

    if (ARGS_OK) {
      VisVariable_cl *pVar = pTypedObject->GetVariable(pszName);
      if (!pVar) {
        luaL_error(L, "Called getter of unknown property '%s'", pszName);
        lua_pushnil(L);
        return 1;
      }
      
      switch (pVar->type) {
        case VULPTYPE_STRING:
        case VULPTYPE_MODEL:
        case VULPTYPE_PSTRING:
        case VULPTYPE_ENTITY_KEY:
        case VULPTYPE_PRIMITIVE_KEY:
        case VULPTYPE_VERTEX_KEY:
        case VULPTYPE_LIGHTSOURCE_KEY:
        case VULPTYPE_WORLDANIMATION_KEY:
        case VULPTYPE_PATH_KEY:
          {
            const char *pStr = NULL;
            pVar->GetValueDirect(pTypedObject, (void*) &pStr, true);
            lua_pushstring(L, pStr);
          }
          break;
        case VULPTYPE_VSTRING:
          {
            VString vstr;
            pVar->GetValueDirect(pTypedObject, (void*) &vstr, true);
            lua_pushstring(L, vstr.AsChar());
          }
          break;
        case VULPTYPE_ENUM:	
        case VULPTYPE_INT:	
          {
            int n;
            pVar->GetValueDirect(pTypedObject, (void*) &n, true);
            lua_pushnumber(L, (lua_Number) n);
          }
          break;
        case VULPTYPE_FLOAT:
          {
            float n;
            pVar->GetValueDirect(pTypedObject, (void*) &n, true);
            lua_pushnumber(L, (lua_Number) n);
          }
          break;
        case VULPTYPE_DOUBLE:
          {
            double n;
            pVar->GetValueDirect(pTypedObject, (void*) &n, true);
            lua_pushnumber(L, (lua_Number) n);
          }
          break;
        case VULPTYPE_BOOL:	
          {
            BOOL b;
            pVar->GetValueDirect(pTypedObject, (void*) &b, true);
            lua_pushboolean(L, (int) b);
          }
          break;
        case VULPTYPE_VECTOR_INT:
          {
            int pN[3];
            pVar->GetValueDirect(pTypedObject, (void*) pN, true);
            hkvVec3 *vector = new hkvVec3((float)pN[0], (float)pN[1], (float)pN[2]);
            SWIG_Lua_NewPointerObj(L,vector,SWIGTYPE_p_hkvVec3, VLUA_MANAGE_MEM_BY_LUA);
          }
          break;
        case VULPTYPE_VECTOR_FLOAT:
          {
            float pN[3];
            pVar->GetValueDirect(pTypedObject, (void*) pN, true);
            hkvVec3 *vector = new hkvVec3(pN[0], pN[1], pN[2]);
            SWIG_Lua_NewPointerObj(L,vector,SWIGTYPE_p_hkvVec3, VLUA_MANAGE_MEM_BY_LUA);
          }
          break;
        case VULPTYPE_VECTOR_DOUBLE:
          {
            double pN[3];
            pVar->GetValueDirect(pTypedObject, (void*) pN, true);
            hkvVec3 *vector = new hkvVec3((float)pN[0], (float)pN[1], (float)pN[2]);
            SWIG_Lua_NewPointerObj(L,vector,SWIGTYPE_p_hkvVec3, VLUA_MANAGE_MEM_BY_LUA);
          }
          break;
        case VULPTYPE_BYTE_COLOR4:
          {
            VColorRef *color = new VColorRef();
            pVar->GetValueDirect(pTypedObject, (void*) color, true);
            SWIG_Lua_NewPointerObj(L,color,SWIGTYPE_p_VColorRef, VLUA_MANAGE_MEM_BY_LUA);
          }
          break;
        default:
          luaL_error(L, "Called getter of unknown type %d (%s)", pVar->type, pszName);
          lua_pushnil(L);
          break;
      }
      return 1;
    }
    return 0;
  }
%}

%native(VTypedObject_SetProperty) int VTypedObject_SetProperty(lua_State *L);
%{
  SWIGINTERN int VTypedObject_SetProperty(lua_State *L)
  {
    IS_MEMBER_OF(VTypedObject) //this will move this function to the method table of the specified class
    
    DECLARE_ARGS_OK;

    SWIG_CONVERT_POINTER(L, 1, VTypedObject, pTypedObject)

    GET_ARG(2, const char *, pszName);

    if (ARGS_OK)
    {
      VisVariable_cl *pVar = pTypedObject->GetVariable(pszName);

      if (!pVar)
      {
        luaL_error(L, "Called setter of unknown property '%s'", pszName);
        lua_pushnil(L);
        return 1;
      }
      
      switch (pVar->type) {
        case VULPTYPE_STRING:
        case VULPTYPE_VSTRING:
        case VULPTYPE_MODEL:
        case VULPTYPE_PSTRING:
        case VULPTYPE_ENTITY_KEY:
        case VULPTYPE_PRIMITIVE_KEY:
        case VULPTYPE_VERTEX_KEY:
        case VULPTYPE_LIGHTSOURCE_KEY:
        case VULPTYPE_WORLDANIMATION_KEY:
        case VULPTYPE_PATH_KEY:
          {
            GET_ARG(3, const char *, pszVal);
            pVar->SetValue(pTypedObject, pszVal, true);
          }
          break;
        case VULPTYPE_ENUM:	
        case VULPTYPE_INT:	
          {
            GET_ARG(3, lua_Number, n);
            int i = (int) n;
            pVar->SetValueDirect(pTypedObject, (void*) &i, true);
          }
          break;
        case VULPTYPE_FLOAT:
          {
            GET_ARG(3, lua_Number, n);
            float f = (float) n;
            pVar->SetValueDirect(pTypedObject, (void*) &f, true);
          }
          break;
        case VULPTYPE_DOUBLE:
          {
            GET_ARG(3, lua_Number, n);
            double d = (double) n;
            pVar->SetValueDirect(pTypedObject, (void*) &d, true);
          }
          break;
        case VULPTYPE_BOOL:	
          {
            GET_ARG(3, bool, n);
            BOOL b = (BOOL) n;
            pVar->SetValueDirect(pTypedObject, (void*) &b, true);
          }
          break;
        case VULPTYPE_VECTOR_INT:
          {
            GET_ARG(3, hkvVec3, vec);
            int pI[3] = {(int) vec.x, (int) vec.y, (int) vec.z};
            pVar->SetValueDirect(pTypedObject, (void*) pI, true);
          }
          break;
        case VULPTYPE_VECTOR_FLOAT:
          {
            GET_ARG(3, hkvVec3, vec);
            float pF[3] = {vec.x, vec.y, vec.z};
            pVar->SetValueDirect(pTypedObject, (void*) pF, true);
          }
          break;
        case VULPTYPE_VECTOR_DOUBLE:
          {
            GET_ARG(3, hkvVec3, vec);
            double pD[3] = {(double) vec.x, (double) vec.y, (double) vec.z};
            pVar->SetValueDirect(pTypedObject, (void*) pD, true);
          }
          break;
        case VULPTYPE_BYTE_COLOR4:
          {
            GET_ARG(3, VColorRef, col);
            pVar->SetValueDirect(pTypedObject, (void*) &col, true);
          }
          break;
        default:
          luaL_error(L, "Called setter of unknown type %d (%s)", pVar->type, pszName);
          break;
      }
    }

    //remove all stack items
    lua_settop(L, 0);
    //LUA_STACK_DUMP(L);
    return 0;
  }
%}

//add lua tostring and concat operators
VSWIG_CREATE_CONCAT(VTypedObject, 128, "%s", self->GetClassTypeId()->m_lpszClassName)
VSWIG_CREATE_TOSTRING(VTypedObject, "%s: 0x%p", self->GetClassTypeId()->m_lpszClassName, self)

#else

/// \brief Base class for any typed object.
class VTypedObject {
public:

  /// @name Base Functions
  /// @{
  
  /// \brief Returns the type name of this instance.
  /// \note Consider using IsOfType to check if the instance is derived from the type.
  /// \return The type name of this instance.
  /// \par Example
  ///   \code
  ///     local obj = Game:GetObject("object1")
  ///     if obj:GetType() == "VisLightSource_cl" then
  ///       obj:SetMultiplier(2)
  ///     end
  ///   \endcode
  /// \see IsOfType
  string GetType();

  /// \brief Test if the instance is of the requested type or is derived from that type.
  /// \param className The class name (type) to test.
  /// \return True if the current instance is of the requested type, otherwise false.
  /// \par Example
  ///   \code
  ///     local obj = Game:GetObject("object1")
  ///     if obj:IsOfType("VisLightSource_cl") then
  ///       obj:SetMultiplier(2)
  ///     end
  ///   \endcode
  boolean IsOfType(string className);

  /// @}  
  /// @name Properties
  /// @{
  
  /// \brief Directly assigns a new value to an exposed object property (public member variables, defined inside the START_VAR_TABLE - END_VAR_TABLE block).
  /// \details
  ///   Assigns a new value to an exposed property. These are the same as the properties in the EntityProperties category in vForge.
  ///   Please note that this function will not trigger any callbacks, it directly changes the value. Consider using UpdateProperty to invoke notifications.
  ///   If you are not sure about the correct property value, you should prefere UpdateProperty because it will check if the value is valid.
  /// \param propName The name of the property.
  /// \param val The new value for the property. Note that this value MUST be of the same type as the property otherwise an error will occur.
  /// \par Example
  ///   \code
  ///     entity:SetProperty("MyStringProperty", "Hello, Lua.")
  ///     typedObj:SetProperty("MyBooleanProperty", true)
  //      anotherObj:SetProperty("MyColorProperty", Vision.VColorRef(255,0,0))
  ///   \endcode
  /// \see UpdateProperty
  void SetProperty(string propName, mixed val);
  
  /// \brief Assigns a new value to an exposed object property (public member variable), checks if this new value is valid and notifies all sub systems.
  /// \param propName The name of the property.
  /// \param propValue The property values as string representation.
  /// \par Example
  ///   \code
  ///     entity:SetProperty("MyStringProperty", "Hello, Lua.")
  ///     typedObj:SetProperty("MyBooleanProperty", "AnotherValue")
  ///   \endcode
  boolean UpdateProperty(string propName, string propValue);

  /// \brief Gets the value of an exposed property (public member varaibles, defined inside the START_VAR_TABLE - END_VAR_TABLE block).
  /// \details
  ///   Gets the value from an exposed property. These are the same as the properties in the EntityProperties category in vForge.
  /// \param propName The name of the property.
  /// \return The value of the property. Its type is the same as the property's type, for example a string property will return a string and a floating point property will return a number. If the property doesn't exist, \b nil is returned.
  /// \par Example
  ///   \code
  ///     local str = entity:GetProperty("MyStringProperty")
  ///     local num = typedObj:GetProperty("MyNumberProperty")
  ///     local vector = anotherObj:GetProperty("MyPositionProperty")
  ///   \endcode
  /// \see GetProperties
  mixed GetProperty(string propName);

  /// \brief Gets the type of an exposed property (public member varaibles, defined inside the START_VAR_TABLE - END_VAR_TABLE block).
  /// \param propName The name of the property.
  /// \return
  ///   The type of a property. The type of the property is translated to a Lua type and represented as a string.
  ///   Currently, this function can return one of the following types:
  ///   - "string"
  ///   - "number"
  ///   - "boolean"
  ///   - "hkvVec3"
  ///   - "VColorRef"
  ///   (or "unkknown" for any other type)
  /// \par Example
  ///   \code
  ///     function OnCreate(self)
  ///       local propType = self:GetPropertyType("MyProperty")
  ///       if (propType == "string") then
  ///         Debug:PrinLine("MyProperty: " .. self:GetProperty("MyProperty"))
  ///       end
  ///     end
  ///   \endcode
  /// \see GetProperties
  string GetPropertyType(string propName);

  /// \brief Get all properties of the current object.
  /// \return A table containing all properties from index 1 to LEN, where each entry consists of Name and Type.
  /// \par Example
  ///   \code
  ///     function OnAfterSceneLoaded(self)
  ///       local properties = self:GetProperties()
  ///       for i=1,#properties do
  ///         Debug:PrintLine("'" .. properties[i].Name .. "': " .. properties[i].Type)
  ///       end
  ///     end
  ///   \endcode
  table GetProperties();
  
  /// \brief Returns the number of properties associated with the object's type.
  /// \return The number of properties.
  /// \par Example
  ///   \code
  ///    local numProps = self:GetNumProperties();
  ///    Debug:PrintLine("The entity has " .. numProps .. " prop(s)")
  ///   \endcode
  number GetNumProperties();
  
  /// @}  
  /// @name Operators
  /// @{
  
  /// \brief Compare with another VTypedObject, e.g. obj1 == obj2
  /// \param other The other typed object.
  /// \return true if the internal pointers are equal, otherwise false.
  boolean operator == (VTypedObject other);  
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
