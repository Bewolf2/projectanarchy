/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VLuaHelpers.hpp

#ifndef VLUAHELPERS_HPP_INCLUDED
#define VLUAHELPERS_HPP_INCLUDED

class VScriptComponent;
class NativeClassInfo;
class VLUATypeInfo;
class VLUAEnumInfo;

class IVRSDUserDataAccessor;
class VScriptMember;

#define VLUA_EPSILON 4.76837158203125E-7f

#define VLUA_TFIRSTUSERDATATYPE 1024

#define VLUA_MANAGE_MEM_MANUAL 0
#define VLUA_MANAGE_MEM_BY_LUA 1

#define LUA_STACK_DUMPS

#define VLUA_TVECTOR3       100
#define VLUA_TCOLOR         101
#define VLUA_TBOUNDINGBOX   102
#define VLUA_TBITMASK       103
#define VLUA_TPATHNODE      104
#define VLUA_TSOUNDRESOURCE 105
#define VLUA_TSOUNDOBJECT   106


/****************************************************************************/
// SWIG related macros
/****************************************************************************/

#define VSWIG_PUSH_PROXY(L, type, pointer_to_instance) \
  VSWIG_Lua_NewPointerObj(L, pointer_to_instance, SWIGTYPE_p_ ## type, VLUA_MANAGE_MEM_MANUAL);

#define VSWIG_PUSH_INSTANCE(L, type) \
  VSWIG_Lua_NewPointerObj(L, new type (), SWIGTYPE_p_ ## type, VLUA_MANAGE_MEM_BY_LUA);

#define VSWIG_PUSH_PROXY_OR_NIL(L, type, pointer_to_instance, condition) \
{ \
  if(condition) { VSWIG_PUSH_PROXY(L, type, pointer_to_instance) } \
    else { lua_pushnil(L); } \
}

#define VSWIG_PUSH_INSTANCE_OR_NIL(L, type, condition) \
{ \
  if(condition) { VSWIG_PUSH_INSTANCE(L, type) } \
      else { lua_pushnil(L); } \
}

#define SWIG_CONVERT_POINTER(L, stack_pos, type, pointer_to_instance) \
  type * pointer_to_instance; \
{ \
  if(!SWIG_isptrtype(L, stack_pos)) luaL_error(L, "Expected %s* as parameter %d", #type, stack_pos); \
  if(!SWIG_IsOK(SWIG_ConvertPtr(L, stack_pos,(void**)&pointer_to_instance,SWIGTYPE_p_##type,0))) luaL_error(L, "Unable to convert self to %s*", #type); \
}

#define VSWIG_CONVERT_POINTER(L, stack_pos, type, pointer_to_instance) \
  type * pointer_to_instance; \
{ \
  if(!(lua_isuserdata(L,stack_pos) || lua_isnil(L,stack_pos))) luaL_error(L, "Expected %s* as parameter %d", #type, stack_pos); \
  if(VSWIG_Lua_ConvertPtr(L, stack_pos,(void**)&pointer_to_instance,SWIGTYPE_p_##type,0)<0) luaL_error(L, "Unable to convert self to %s*", #type); \
}

/****************************************************************************/
// Helper functions for working with LUA
/****************************************************************************/

//additionally export type safe functions

// Helper function to push a hkvVec3 to the stack
SCRIPT_IMPEXP void LUA_PushObjectProxy(lua_State* L, hkvVec3* pVector); 

// Helper function to push a VColorRef to the stack
SCRIPT_IMPEXP void LUA_PushObjectProxy(lua_State* L, VColorRef *pColorRef); 

// Helper function to push a hkvAlignedBBox to the stack
SCRIPT_IMPEXP void LUA_PushObjectProxy(lua_State* L, hkvAlignedBBox *pBox);

// Helper function to push a VBitmask to the stack
SCRIPT_IMPEXP void LUA_PushBitmask(lua_State* L, unsigned int iMask);

//Create an object proxy on the stack, specify an intended owner if you would like to perform a one time initialization of the object
SCRIPT_IMPEXP void LUA_PushObjectProxy(lua_State* L, VTypedObject* pObject, VisTypedEngineObject_cl* pIntendedOwner = NULL);

//Create a new wrapper on the stack, specify an intended owner if you would like to perform a one time initialization of the object
SCRIPT_IMPEXP void LUA_CreateNewWrapper(lua_State* L, VTypedObject* pObject, VisTypedEngineObject_cl* pIntendedOwner = NULL);

//Compatiblity method
SCRIPT_IMPEXP VisTypedEngineObject_cl * LUA_GetObject(lua_State* L, int iStackIndex);


//Push the scripting proxy for an object on the stack
SCRIPT_IMPEXP void LUA_PushObjectProxy(lua_State* L, VScriptComponent *pObj);

//Resets the scripting proxy
SCRIPT_IMPEXP void LUA_ResetObjectProxy(lua_State* L, VScriptComponent *pObj);

//Creates a new metatable and links it to itself
SCRIPT_IMPEXP void LUA_NewMetatable(lua_State* L, const char *pszName);

//Registers metafunctions into a metatable on the stack
SCRIPT_IMPEXP void LUA_RegMetatable(lua_State* L, const luaL_Reg* pReg);

//Create and register a wrapper classes of a native class
SCRIPT_IMPEXP void LUA_CreateWrapperClass(lua_State* L, const NativeClassInfo *pInfo);

//Registers a new LUA data type and its meta methods (operators) and other methods
SCRIPT_IMPEXP void LUA_OpenLibrary(lua_State* L, const VLUATypeInfo *pInfo, IVRSDUserDataAccessor* pUserDataAccessor = NULL);

//Get a function of an object from its meta table and push it on the stack (0,+1,-)
SCRIPT_IMPEXP void LUA_GetFunction(lua_State* L);

//Registers a Vision enum type and its constants for scripting
SCRIPT_IMPEXP void LUA_CreateEnumWrapper(lua_State* L, const VLUAEnumInfo *pInfo);

//Pushes a value of a registered Vision enum type on the lua stack
SCRIPT_IMPEXP bool LUA_PushEnum(lua_State* L, const VLUAEnumInfo *pInfo, int iEnumVal);

//Creates a new locals table for this state. 
//Links the new table to the master state's global table so scripts can find the global functions
SCRIPT_IMPEXP void LUA_CreateLocalsTable(lua_State* L);

//reset the pointer-to-wrapper lookup table in the Lua registry
SCRIPT_IMPEXP void LUA_ResetWrapperLookupTable(lua_State* L);

//Registers an object in the LUA registry
SCRIPT_IMPEXP int LUA_RegisterObject(lua_State* L);

//De registers an object from the LUA registry
SCRIPT_IMPEXP void LUA_DeregisterObject(lua_State* L, int id);

// Helper function to test if a Lua object is of a specified userdata type (taken from beta 5.2 branch)
SCRIPT_IMPEXP void* LUA_TestUserData(lua_State* L, int ud, const char* tname);

// Helper to retrieve swig data types
SCRIPT_IMPEXP void LUA_GetSwigTypes(lua_State *L, VStrList *pTypes);

// Helper to push a global user data to the stack top (will return the number of new stack items)
SCRIPT_IMPEXP int LUA_PushGlobalUserData(lua_State* L, const char* szVariable);

//Helper to determinate the swig type of a stack item or class name
//Note: Current implementation is only checking datatypes of the Vision module
SCRIPT_IMPEXP void* LUA_GetSwigType(lua_State* L, int iStackIndex);

//Helper to determinate the swig type of a stack item or class name
//Note: Current implementation is only checking datatypes of the Vision module
SCRIPT_IMPEXP void* LUA_GetSwigType(lua_State* L, const char * szSwigTypeName);

//Helper to get the name of a lua data type
SCRIPT_IMPEXP const char * LUA_GetDataTypeName(lua_State* L, int iStackIndex);

// \brief Helper function to call a static lua function.
// \param L:The lua state.
// \param szModuleName: The name of the module, where the static function is defined in.
// \param szTypeName: The class type name we are going to create.
// \param szFunctionName: The static function name.
// \param szArgFormat: This parameter specifies the signature of our function.
// E.g.: "E>E" means that there is one input parameter ('E' for entiy) and one
// output parameter (this will be always the same for a cast function, only the
// types will change). Possible character are:
//  - 'E' for anything derived from Entity
//  - 'C' for anything derived from object component
//  - 'O' for anything derived from typed engine object
//  - 'T' for anything derived from typed object
//  - 'v' for void * - use this with caution, since there is no type check!!!
// \param ... all input parameters (the return parameter will remain on the Lua stack)
// \return Returns the amount of return parameters.
SCRIPT_IMPEXP int LUA_CallStaticFunction(lua_State* L, const char * szModuleName, const char * szTypeName, const char * szFunctionName, const char * szArgFormat, ...);

// Helper to have a look at the current Lua Stack (Debug only)
#ifdef LUA_STACK_DUMPS
  SCRIPT_IMPEXP void LUA_stackDump (lua_State *L);
#  define LUA_STACK_DUMP(L) LUA_stackDump(L)
#else
#  define LUA_STACK_DUMP(L)  
#endif


/////////////////////////////////////////////////////////////////////////////
// Helper functions and macros for accessing arguments from wrapper functions
/////////////////////////////////////////////////////////////////////////////


#ifdef _VISION_PS3
#pragma diag_push
#pragma diag_suppress=178
#endif



// VerfiyValidFloats checks in debug mode the floats for nan and infinite values
static void VerifyValidNumbers(const float *pBlock, int iCount, lua_Number fMax=1000000.0)
{
  for (int i=0;i<iCount;i++)
  {
    VASSERT_MSG (hkvMath::isValidFloat (pBlock[i]), "value is not a valid float");
    VASSERT_MSG (hkvMath::isInRange (pBlock[i], -fMax, fMax), "value is out of range");
  }
}

// VerfiyValidFloats checks in debug mode the floats for nan and infinite values
static void VerifyValidNumbers(const double *pBlock, int iCount, lua_Number fMax=1000000.0)
{
  for (int i=0;i<iCount;i++)
  {
    VASSERT_MSG (hkvMath::isValidFloat ((float) pBlock[i]), "value is not a valid float");
    VASSERT_MSG (hkvMath::isInRange ((float) pBlock[i], -fMax, fMax), "value is out of range");
  }
}


#ifdef _VISION_PS3
#pragma diag_pop
#endif


#ifdef HK_DEBUG_SLOW
#define VALIDATE_LUA_NUMBERS_RANGE( Numbers, Count, Range ) VerifyValidNumbers( Numbers, Count, Range )
#else
#define VALIDATE_LUA_NUMBERS_RANGE( Numbers, Count, Range ) { }
#endif

#ifdef HK_DEBUG_SLOW
#define VALIDATE_LUA_NUMBERS( Numbers, Count ) VerifyValidNumbers( Numbers, Count )
#else
#define VALIDATE_LUA_NUMBERS( Numbers, Count ) { }
#endif


//Defines and initializes the ARGS_OK variable.
//The ARGS_OK variable is used to keep track of erros encountered during
//GET_OBJECT_AT and GET_ARG calls.
#define DECLARE_ARGS_OK bool ARGS_OK = true;

//Retrieves an argument if possible, sets an error if not available
// _id   : argument index number
// _type : class type
// _var  : variable name
#define GET_ARG(_id, _type, _var)   \
    _type _var;                     \
    if (!LUA_GetValue(L, _id, _var)) \
    {\
      luaL_argerror (L, _id+1, "Expected " #_type);\
      ARGS_OK = false;\
    }\

//Retrieves an optional argument if possible, but doesn't set an error if not available
// _def : default value to use
#define GET_OPT_ARG(_id, _type, _var, _def)   \
    _type _var = _def;                     \
    LUA_GetValue(L, _id, _var);

extern lua_Number dummyVector[3];

SCRIPT_IMPEXP bool LUA_GetValue(lua_State* L, int id, bool &value);

SCRIPT_IMPEXP bool LUA_GetValue(lua_State* L, int id, int &value);

SCRIPT_IMPEXP bool LUA_GetValue(lua_State* L, int id, float &value);

SCRIPT_IMPEXP bool LUA_GetValue(lua_State* L, int id, double &value);

SCRIPT_IMPEXP bool LUA_GetValue(lua_State* L, int id, VString &value);

//NB. The string pointers only valid while the object is still on the stack
SCRIPT_IMPEXP bool LUA_GetValue(lua_State* L, int id, const char* &value);

SCRIPT_IMPEXP bool LUA_GetValue(lua_State* L, int id, VisTypedEngineObject_cl* &pObj);

SCRIPT_IMPEXP bool LUA_GetValue(lua_State* L, int id, VTypedObject* &value);

SCRIPT_IMPEXP bool LUA_GetValue(lua_State* L, int id, IVObjectComponent* &pObj);

SCRIPT_IMPEXP bool LUA_GetFloatField(lua_State* L, int id, const char *pszField, float &value);

SCRIPT_IMPEXP bool LUA_GetValue(lua_State* L, int id, hkvVec3& value);

SCRIPT_IMPEXP bool LUA_GetValue(lua_State* L, int id, VColorRef &value);

SCRIPT_IMPEXP bool LUA_GetValue(lua_State* L, int id, hkvAlignedBBox &value);

//bitmask access
SCRIPT_IMPEXP bool LUA_GetValue(lua_State* L, int id, int*& bitmask);

//Gets a Vision enum from the LUA stack and return a pointer to the correct value
SCRIPT_IMPEXP const int* LUA_GetEnum(lua_State* L, int id);

SCRIPT_IMPEXP bool LUA_GetValue(lua_State* L, int id, VisLightSourceType_e &value);

SCRIPT_IMPEXP bool LUA_GetValue(lua_State* L, int id, VisTraceAccuracy_e &value);

#if defined(__SNC__)
#pragma diag_push
#pragma diag_suppress=369
#endif

/// \brief
///   LUA binding specific: Class for storing class information. This is used for creating wrapper
///   classes of native classes
class NativeClassInfo
{ 
public:
  const char* pszClassName;           ///< Name of this class
  const NativeClassInfo* m_pParent;   ///< Parent class
  const luaL_Reg*  m_pMethods;        ///< Array of methods to register
};


/// \brief
///   LUA binding specific: Class for storing LUA data type information. This is used for creating
///   custom LUA data types (such as vector3)
class VLUATypeInfo
{ 
public:
  const char* pszTypeName;            ///< Name of this type
  const luaL_Reg* m_pMethods;         ///< Array of methods and operators to register
  const luaL_Reg m_Constructor;       ///< The function to use as constructor for this type
};


/// \brief
///   LUA binding specific: Class for storing Vision enum information. This is used for creating
///   wrapper types for the constants of native enums.
struct VLUAEnumReg
{
  const char* pszName;
  const int m_iValue;
};

/// \brief
///   LUA binding specific: Class for storing Vision enum information. This is used for creating
///   wrapper types for native enums.
class VLUAEnumInfo
{ 
public:
  const char* pszEnumName;            ///< Name of this enum
  const int m_iNumConstants;          ///< Number of constants in this enum
  const VLUAEnumReg* m_pConstants;    ///< Array of constants to register for this enum
};

#if defined(__SNC__)
#pragma diag_pop
#endif


/// \brief
///  Safe abstract iterator for pointer types, intended for Lua style
///  (because auf extensive checkes) iteration over tables.
///  Look at VLuaStringIterator for more details.
template <typename CLASS_TYPE> class ISafeIterator
{
protected:
  //these methods have to be implemented by the specific iterator:

  //get next element (only called when hasNext is successful)
  virtual CLASS_TYPE * next() = 0;

  //check if there is a next element (ensures init call)
  virtual bool hasNext() = 0;

  //initialization which will will be automatically called before the first next/hasNext occurs
  virtual void init() = 0;

  //deinit, can be invoked by DeInit of if not performed by the destructor
  virtual void deInit() = 0;

private:
  bool m_bInitialized;
  bool m_bDeInitialized;
  bool m_bExecutedHasNext;

public:

  /// \brief Default constructor just initializes private members
  ISafeIterator() : m_bInitialized(false), m_bDeInitialized(false), m_bExecutedHasNext(false)
  {
  }

  /// \brief Ensures call of the DeInit function
  virtual ~ISafeIterator()
  {
    DeInit();
  }

  /// \brief Forward iterator by one and return next element (could be NULL)
  CLASS_TYPE * Next()
  {
    if(Init())
    {
      if(!m_bExecutedHasNext) HasNext(); //just ensure the call (we don't care about the result)
      m_bExecutedHasNext = false;
      return next();
    }
    else
    {
      return NULL;
    }
  }

  /// \brief Check if there is another element
  bool HasNext()
  {
    if(!Init()) return false;

    m_bExecutedHasNext = true;

    if(!hasNext())
    {
      DeInit();
      return false;
    }

    return true;
  }

  /// \brief Initialize iterator, will call init
  bool Init()
  {
    if(!m_bInitialized)
    {
      init();
      m_bInitialized = true;
    }

    return !m_bDeInitialized;
  }

  /// \brief Deinitialize iterator, will call deInit
  void DeInit()
  {
    if(m_bInitialized&&!m_bDeInitialized)
    {
      deInit();
      m_bDeInitialized = true;
    }
  }

};

/// \brief
///  Iterate over all string entries of a table
///  (non string entries will result in NULL's returned by next)
class VLuaTableIterator : public ISafeIterator<VScriptMember> {

public:
  /// \brief
  ///   Initialize this string table iterator with the lua state and the stack index of the desired table
  VLuaTableIterator(lua_State* pLuaState, int iTableIndex)
    : ISafeIterator<VScriptMember>(), m_pLuaState(pLuaState), m_iTableIndex(iTableIndex), m_iStackItems(0), m_pCurrent(NULL)
  {
  }

protected:
  lua_State* m_pLuaState;
  int m_iTableIndex;
  int m_iStackItems;
  VScriptMember *m_pCurrent;

  void init();
  void deInit();
  bool hasNext();
  VScriptMember * next();

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
