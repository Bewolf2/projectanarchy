/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/* -----------------------------------------------------------------------------
 * vision.i
 *
 * SWIG library file to support Vision types (windows.i is still required).
 * ----------------------------------------------------------------------------- */
 
typedef long SLONG;
typedef long RETVAL;
typedef short SSHORT;
typedef int VBool;
typedef char SBYTE;
typedef unsigned char UBYTE;
typedef int SINT;

//cast macro for all types using the vision RTTI
%define VSWIG_CREATE_CAST(CAST_TO_CLASS)

  static CAST_TO_CLASS* Cast(VTypedObject * pObject) 
  {
    if(pObject && pObject->IsOfType(CAST_TO_CLASS::GetClassTypeId()))
      return (CAST_TO_CLASS *) pObject;
    Vision::Error.Warning("[Lua] Cannot cast to %s!", #CAST_TO_CLASS);
    return NULL;
  }
  
%enddef

%define VSWIG_CREATE_CAST_UNSAFE(CAST_TO_CLASS)

  static CAST_TO_CLASS* Cast(unsigned long * lObject) 
  {
    return (CAST_TO_CLASS *) lObject;
  }
  
%enddef

%define VSWIG_CONVERT_BOOL_GETTER_CONST(METHOD_NAME)
  bool METHOD_NAME() const
  {
    return self->METHOD_NAME()==TRUE;
  }
%enddef

%define VSWIG_CONVERT_BOOL_GETTER(METHOD_NAME)
  bool METHOD_NAME()
  {
    return self->METHOD_NAME()==TRUE;
  }
%enddef

%define VSWIG_CONVERT_BOOL_SETTER(METHOD_NAME)
  void METHOD_NAME(bool bValue)
  {
    self->METHOD_NAME(bValue);
  }
%enddef

%define VSWIG_BITMASK_GETTER_CONST(METHOD_NAME)
  VBitmask METHOD_NAME() const
  {
    return VBitmask(self->METHOD_NAME());
  }
%enddef

%define VSWIG_BITMASK_GETTER_CONST_RENAME(METHOD_NAME, RENAME_TO)
  VBitmask RENAME_TO() const
  {
    return VBitmask(self->METHOD_NAME());
  }
%enddef

%define VSWIG_BITMASK_SETTER(METHOD_NAME)
  void METHOD_NAME(VBitmask * pBitmask)
  {
    if(pBitmask) self->METHOD_NAME(pBitmask->Get());
  }
%enddef

%define VSWIG_BITMASK_SETTER_RENAME(METHOD_NAME, RENAME_TO)
  void RENAME_TO(VBitmask * pBitmask)
  {
    if(pBitmask) self->METHOD_NAME(pBitmask->Get());
  }
%enddef

%define VSWIG_RETURN_BY_VALUE_CONST(TYPE, METHOD_NAME)
  TYPE METHOD_NAME() const
  {
    return TYPE(self->METHOD_NAME());
  }
%enddef

%define VSWIG_ENSURE_GUI_CONTEXT(MANAGER, CONTEXT)
  IVGUIContext *CONTEXT = MANAGER->Contexts().Count()>0 ? MANAGER->Contexts().GetAt(0) : new VGUIMainContext(MANAGER);
  VASSERT_MSG(CONTEXT!=NULL, "Could not find or create GUI context!");
  CONTEXT->SetActivate(true);
%enddef

%define VSWIG_CREATE_CONCAT(TYPE, SIZE, FORMAT_STRING, ...)
%native(TYPE##___concat) int TYPE##_Concat(lua_State *L);
%{
  SWIGINTERN int TYPE##_Concat(lua_State *L)
  {
    //this will move this function to the method table of the specified class
    IS_MEMBER_OF(TYPE)
    
    bool ARGS_OK = true;
    
    const char *pszString = NULL;
    int iIndex = -1;
    
    //The concat operator allows "foo"..self and self.."bar" so that
    //we have to consider self as first and as second stack element.
    
    //handle string as first (top) element
    if(lua_isstring(L,iIndex))
    {
      pszString = lua_tostring(L,iIndex);
      iIndex--;
    }
    
    SWIG_CONVERT_POINTER(L, iIndex, TYPE, self)
    iIndex--;
    
    //handle string as second element
    if(iIndex==-2)
    {
      pszString = lua_tostring(L,iIndex);
    }
        
    unsigned int uiLen = (unsigned int) strlen(pszString);
    char *pszBuffer = new char[uiLen+SIZE];
   
    sprintf(pszBuffer, FORMAT_STRING, ##__VA_ARGS__); //format as requested
    
    //the new buffer should have the size of the old string and the new format string
    VASSERT_MSG(SIZE>(strlen(pszBuffer)+uiLen), "Please increase your temp buffer size!");
    
    //append or prepend old buffer buffer (depending on the position inside the lua stack)
    if(iIndex==-3) //append old string
    {
      memcpy(pszBuffer+strlen(pszBuffer),pszString,uiLen+1); //also copy the terminator at the end
    }
    else //prepend old string
    {
      memmove(pszBuffer+uiLen,pszBuffer, strlen(pszBuffer)+1); //also move the terminator at the end
      memcpy(pszBuffer,pszString,uiLen); //insert the old string
    }

    lua_pushstring(L, pszBuffer);
  
    V_SAFE_DELETE_ARRAY(pszBuffer);

    return 1;
  }
%}
%enddef

%define VSWIG_CREATE_TOSTRING(TYPE, FORMAT_STRING, ...)
%native(TYPE##___tostring) int TYPE##_ToString(lua_State *L);
%{
  SWIGINTERN int TYPE##_ToString(lua_State *L)
  {
    //this will move this function to the method table of the specified class
    IS_MEMBER_OF(TYPE)
    
    SWIG_CONVERT_POINTER(L, -1, TYPE, self)
    
    char pszBuffer[1024];
    
    sprintf(pszBuffer, FORMAT_STRING, ##__VA_ARGS__); //format as requested
      
    lua_pushstring(L, pszBuffer);
    
    return 1;
  }
%}
%enddef

%define VSWIG_CREATE_TOSTRING_EX(TYPE, COMMANDS, FORMAT_STRING, ...)
%native(TYPE##___tostring) int TYPE##_ToString(lua_State *L);
%{
  SWIGINTERN int TYPE##_ToString(lua_State *L)
  {
    //this will move this function to the method table of the specified class
    IS_MEMBER_OF(TYPE)
    
    SWIG_CONVERT_POINTER(L, -1, TYPE, self)
    
    char pszBuffer[1024];
    
    COMMANDS
    
    sprintf(pszBuffer, FORMAT_STRING, ##__VA_ARGS__); //format as requested
      
    lua_pushstring(L, pszBuffer);
    
    return 1;
  }
%}
%enddef

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
