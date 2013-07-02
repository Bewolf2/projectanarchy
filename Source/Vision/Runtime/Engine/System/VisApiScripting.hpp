/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiScripting.hpp

#ifndef VISAPISCRIPTING_HPP_INCLUDED
#define VISAPISCRIPTING_HPP_INCLUDED

class IVObjectComponent;
class IVScriptInstance;
class VisStaticMeshInstance_cl;
class VTerrainSector;
class IVisDecorationGroup_cl;

/// \brief
///   Smart pointer to an IVScriptInstance
typedef VSmartPtr<IVScriptInstance> IVScriptInstancePtr;


/// \brief
///   This structure contains information about the collider relevant for the scripting callbacks.
struct VScriptColliderInfo
{
  /// \brief
  ///   Default constructor; initializes the structure to zero values.
  inline VScriptColliderInfo() : m_pObjectComponent(NULL), m_pStaticMesh(NULL), m_pTerrainSector(NULL), m_pDecorationGroup(NULL) {}

  IVObjectComponent *m_pObjectComponent;       ///< The physics object (can be NULL)
  VisStaticMeshInstance_cl *m_pStaticMesh;     ///< Collider static mesh (can be NULL)
  const VTerrainSector *m_pTerrainSector;      ///< Collider terr sector (can be NULL)
  IVisDecorationGroup_cl *m_pDecorationGroup;  ///< Terrain decoration group collider (can be NULL)

  /// \brief
  ///   Returns the collider type as a string.
  inline const char * GetColliderType()
  {
    if(m_pObjectComponent) return "Entity";
    if(m_pStaticMesh) return "Mesh";
    if(m_pTerrainSector) return "Terrain";
    if(m_pDecorationGroup) return "Decoration";

    return "Unknown";
  }
};

/// \brief
///   This structure contains information about the collision relevant for the scripting callbacks.
struct VScriptCollisionInfo
{
  /// \brief
  ///   Default constructor; initializes the structure to zero values.
  inline VScriptCollisionInfo() : m_vPoint(), m_vNormal(), m_fForce(0.f), m_fRelativeVelocity(0.f) {}

  hkvVec3 m_vPoint;                   ///< Impact position
  hkvVec3 m_vNormal;                  ///< Impact normal
  float m_fForce;                     ///< Force value
  float m_fRelativeVelocity;          ///< Result velocity vector of the collision

  VScriptColliderInfo m_ownInfo;      ///< own collider info
  VScriptColliderInfo m_colliderInfo; ///< information about the other collider
};

#pragma warning(push)  
#pragma warning(disable:4793)   ///<Disable deprecation warnings

/// \brief
///   Virtual interface that represents one script instance that is associated with a scriptable
///   object such as entities
/// 
/// A script instance can only be attached to one object at a time.
/// 
/// This script instance interface is implementation independent. Engine objects internally just
/// operate on the interface, e.g. when serializing.
class IVScriptInstance : public VisTypedEngineObject_cl, public VRefCounter
{
public:

  /// \brief
  ///   Constructor
  inline IVScriptInstance()
  {
    m_pParentComponent=NULL;
  }

  /// \brief
  ///   pure virtual overridable that executes a script function and passes an arbitrary number of
  ///   parameters
  /// 
  /// \param szFunction
  ///   Name of the script function to call
  /// 
  /// \param szArgFormat
  ///   Format of the arguments passed to this function. A script function can also return an
  ///   arbitrary number of parameters which have to be passed as references. See Example. Every
  ///   character in this string represents the format of one parameter. That is:
  ///   \li 'd' : double,
  /// 
  ///   \li 'f' : float,
  /// 
  ///   \li 'i' : integer (32bit),
  ///
  ///   \li 'b' : bool (BOOL - important, do not use 'bool' in C++ code)
  /// 
  ///   \li 's' : string (char*),
  /// 
  ///   \li 'm' : bitmask (32bit int used as a bitmask, not supported as return parameter),
  /// 
  ///   \li 'v' : vector3 (pointer to a hkvVec3),
  /// 
  ///   \li 'c' : color (pointer to a VColorRef),
  /// 
  ///   \li '#' : bounding box (pointer to a hkvAlignedBBox),
  /// 
  ///   \li 'o' : typed object (pointer to an object derived from VisTypedEngineObject, not supported as return parameter).
  /// 
  ///   \li More types might be supported for specific implementations. The '>' character separates
  ///     the input parameter list from the return values. Thus, a function that takes two floats
  ///     as input and returns an integer should pass "ff>i" and pass the parameters accordingly.
  ///     Furthermore, for member functions the 'this' pointer has to be passed always as the first
  ///     parameter. This is done by passing '*' as the first character in the szArgFormat string.
  ///     This advises the function to put a proxy object on the stack that stands for the owner
  ///     object. See second example.
  /// 
  ///   \li \b VScriptInstance \b Specific \b Parameters (\b Lua):
  ///   		
  ///   \li 'u' : light user data pointer (as void *),
  ///   		    
  ///   \li '{' + ... + '}' : Create a table containing arbitrary arguments as defined in the variadic parameter.
  ///   	E.g. a function using a table with two strings as parameters, returning an integer:
  ///   	\code
  ///   	pScriptInst->ExecuteFunctionArg("ScoreDifference","{ss}>i", player1->GetName().AsChar(), player2->GetName().AsChar(), &iScore);
  ///   	\endcode
  ///   	This is only allowed in the input list - before '>'! You can nest table for
  ///   	ARGV_MAX_NESTED_TABLES times (default setting for this constant is 4).
  ///   	
  ///   \li '[' + ... + ']' : Create a table where the contents of the table ist stored in a void ** array.
  ///   	E.g. a function using a table with two strings as parameters, returning an integer:
  ///   	\code
  ///   	void ** ppArray = {NULL, NULL};
  ///   	ppArray[0] = (void *)player1->GetName().AsChar();
  ///   	ppArray[1] = (void *)player2->GetName().AsChar();
  ///   	pScriptInst->ExecuteFunctionArg("ScoreDifference","[ss]>i", ppArray, &iScore);
  ///   	\endcode
  ///   	This is only allowed in the input list - before '>'! You cannot nest this type of table!
  /// 
  /// \param argPtr
  ///   Pointer to the variable argument list that contains the argument as specified in
  ///   szArgFormat
  /// 
  /// \return
  ///   \c TRUE if the function call was successful
  /// 
  /// \note
  ///   Please use the ExecuteFunctionArg convenience function instead.
  /// 
  /// \sa ExecuteFunctionArg
  /// \sa VScriptInstance
  /// 
  /// \example
  ///   \code
  ///   pScriptInst->ExecuteFunctionArg("GetItemValue","s>f","SwordItem",&fSwordValue);
  ///   pScriptInst->ExecuteFunctionArg("OnThink","*"); // call the OnThink member function (no additional parameters)
  ///   \endcode
  VISION_APIFUNC virtual BOOL ExecuteFunctionArgV(const char *szFunction, const char *szArgFormat, va_list argPtr) = 0;

  /// \brief
  ///   This non-virtual function calls ExecuteFunctionArgV(szFunction,szArgFormat,va_list), i.e.
  ///   it creates a va_list from the ellipsis and forwards it to ExecuteFunctionArgV. Usage of
  ///   this function is preferred over ExecuteFunctionArgV.
  /// 
  /// \param szFunction
  ///   Name of the script function to call
  /// 
  /// \param szArgFormat
  ///   See ExecuteFunctionArgV for details.
  ///
  /// \sa ExecuteFunctionArgV
  VISION_APIFUNC BOOL ExecuteFunctionArg(const char *szFunction, const char *szArgFormat, ...)
  {
    va_list argPtr;
    va_start(argPtr, szArgFormat);
    BOOL res = ExecuteFunctionArgV(szFunction, szArgFormat, argPtr);
    va_end(argPtr);
    return res;
  }

  /// \brief
  ///   This non-virtual function calls ExecuteFunctionArg(szFunction,NULL), i.e. just a function
  ///   with no parameters or return value.
  /// 
  /// \param szFunction
  ///   Name of the script function to call
  /// 
  /// \sa ExecuteFunctionArg
  /// \sa ExecuteFunctionArgV
  inline BOOL ExecuteFunction(const char *szFunction)
  {
    return ExecuteFunctionArg(szFunction,NULL);
  }

  /// \brief
  ///   Executes the passed script code on this instance
  ///
  /// This method dynamically compiles and executes the passed script code. The 
  /// owner of the script instance can be accessed via a global "self" variable 
  /// within the script code.
  ///
  /// \param szText
  ///   Script code to execute
  /// \param bUseGlobals
  ///   Optional boolean, which specifies if the executed script code should use
  ///   (and be able to modify) globals (=true) or should use an own, private global
  ///   environment for execution (=false). Default is false.
  /// \return
  ///   True on success, False if not implemented or the passed script code is invalid
  VISION_APIFUNC virtual BOOL RunScriptCode(const char *szText, bool bUseGlobals = false) { return FALSE; }

  /// \brief
  ///   Indicates whether the script has a function with the specified name
  /// 
  /// \param szFunction
  ///   Name of the script function to test
  VISION_APIFUNC virtual BOOL HasFunction(const char *szFunction) = 0;
  

  /// \brief
  ///   Returns the component this script object is attached to
  ///
  /// \returns
  ///   IVObjectComponent* pComponent: Parent component of this script.
  inline IVObjectComponent* GetParentComponent() const 
  {
    return m_pParentComponent;
  }

  /// \brief
  ///   Virtual overridable to set the parent component. This base implementation just sets the
  ///   m_pParentComponent member.
  ///
  /// \param pOwner
  ///   Parent component to set.
  virtual void SetParentComponent(IVObjectComponent* pOwner)
  {
    m_pParentComponent = pOwner;
  }

  /// \brief
  ///   Executes the specified collision callback with the provided collision data.
  ///
  /// \param pszFunctionName
  ///   The name of the collision callback.
  ///
  /// \param pColInfo
  ///   Structure containing all the collision data.
  ///
  /// \returns
  ///   true on success running the callback.
  VISION_APIFUNC virtual bool ExecuteCollisionFunc(const char *pszFunctionName, VScriptCollisionInfo *pColInfo)
  {
    return false;
  }

protected:
  IVObjectComponent* m_pParentComponent;
};



/// \brief
///   Virtual interface that manages all script instances
/// 
/// Virtual interface that is manages all script instances
/// 
/// This manager interface is implementation independent. It can be globally set in the engine via
/// Vision::SetScriptManager() resp. Vision::GetScriptManager()
class IVScriptManager
{
public:

  /// \brief
  ///   Constructor
  inline IVScriptManager()
    : m_spGameScript()
    , m_spSceneScript()
    , m_pObjFactory(NULL)
    , m_bPaused(false)
    , m_fThinkingPos(0.f)
    , m_fThinkingInterval(0.f)
  {
  }

  virtual ~IVScriptManager()
  {
    // Nothing to be done
  }

  /// \brief
  ///   Returns the script instance of an object if it has one.
  VISION_APIFUNC virtual IVScriptInstance* GetScriptInstance( VisTypedEngineObject_cl *pObj) = 0;

  /// \brief
  ///   Creates and returns a new script instance that runs the script file specified by szFilename
  VISION_APIFUNC virtual IVScriptInstance* CreateScriptInstanceFromFile(const char *szFilename) = 0;
  
  /// \brief
  ///   Associates a script instance with an object
  /// 
  /// Please note that this function replaces any already existing script component on the passed object.
  /// If you want to use multiple script components for your object, you can explicitly create and add
  /// the necessary script component instead.
  ///
  /// \param pObj
  ///  Object with which to associate the script instance
  ///
  /// \param pInst
  ///  Script instance to associate with the object
  VISION_APIFUNC virtual void SetScriptInstance( VisTypedEngineObject_cl *pObj, IVScriptInstance* pInst) = 0;

  /// \brief
  ///   Sets the global instance of the game script. For more information about the game script,
  ///   please refer to the documentation.
  VISION_APIFUNC virtual void SetGameScript(IVScriptInstance* pScriptObj) 
  {
    if (m_spGameScript==pScriptObj)
      return;
    V_SAFE_DISPOSEOBJECT(m_spGameScript); ///< should be unique
    m_spGameScript=pScriptObj;
  }

  /// \brief
  ///   Returns the game script instance
  inline IVScriptInstance* GetGameScript() const 
  {
    return m_spGameScript;
  }
  
  /// \brief
  ///   Sets the global instance of the scene script. For more information about the scene script,
  ///   please refer to the documentation.
  VISION_APIFUNC virtual void SetSceneScript(IVScriptInstance* pScriptObj) 
  {
    if (m_spSceneScript==pScriptObj)
      return;
    V_SAFE_DISPOSEOBJECT(m_spSceneScript); ///< should be unique
    m_spSceneScript=pScriptObj;
  }

  /// \brief
  ///   Returns the scene script instance
  inline IVScriptInstance* GetSceneScript() const 
  {
    return m_spSceneScript;
  }

  /// \brief
  ///   Sets a factory that is used to create new script object instances. Can be used to create custom classes without changing the plugin code.
  ///
  /// \param pFactory
  ///   New factory instance. Can be NULL for default behavior. This manager does not take ownership and thus does not delete any factory instances
  inline void SetScriptObjectFactory(IVObjectInstanceFactory *pFactory)
  {
    m_pObjFactory = pFactory;
  }

  /// \brief
  ///   Returns the current factory. By default this factory is NULL.
  inline IVObjectInstanceFactory *GetScriptObjectFactory()
  {
    return m_pObjFactory;
  }

  /// \brief
  ///   Helper function to validate a script and output potential problems into the log. Used by
  ///   vForge.
  VISION_APIFUNC virtual BOOL ValidateScript(const char *szText, int iLen, IVLog *pLog) = 0;

  /// \brief
  ///   Global callback that is triggered to register all script functions.
  /// 
  /// Listen to this callback to register custom script functions.
  /// 
  /// The function prototypes as well as the registration process itself is implementation
  /// dependent.
  /// 
  /// Please refer to the samples for examples on how to use this callback.
  VISION_APIDATA static VisCallback_cl OnRegisterScriptFunctions;

  /// \brief
  ///   Global callback that is triggered to create proxies of instance.
  /// 
  /// Listen to this callback to create your custom script instances.
  /// 
  /// The function prototypes as well as the registration process itself is implementation
  /// dependent.
  /// 
  /// Please refer to the samples for examples on how to use this callback.
  VISION_APIDATA static VisCallback_cl OnScriptProxyCreation;

  /// \brief
  ///   Global callback that is triggered when a scripting error occurs
  /// 
  /// Specific scripting implementations might pass a data object with exact description of the
  /// error including script file, line numbers and stack trace.
  VISION_APIDATA static VisCallback_cl OnError;

  /// \brief
  ///   Global callback that is triggered when a scripting debug event occurs
  /// 
  /// Specific scripting implementations might pass a data object with exact description of the
  /// debug event.
  /// 
  /// \sa VScriptDebugEventObject
  VISION_APIDATA static VisCallback_cl OnDebugEvent;

  /// \brief
  ///   Helper function to call a script method on an object. The object must have a script
  ///   instance attached
  VISION_APIFUNC bool CallScriptMethod(VisTypedEngineObject_cl *pObj, const char* pszFunc)
  {
    IVScriptInstance* pInst = GetScriptInstance(pObj);
    if (!pInst)
      return false;
      
    return pInst->ExecuteFunctionArg(pszFunc,"*")==TRUE;
  }
  
  /// \brief
  ///   Virtual overridable to pause script processing (tick function). call this base function to
  ///   set the internal state that is returned via IsPaused
  VISION_APIFUNC virtual void SetPause(bool bStatus)
  {
    m_bPaused = bStatus;
  }

  /// \brief
  ///   Returns the current paused status flag
  inline bool IsPaused() const {return m_bPaused;}

  /// \brief
  ///   Globally sets the interval (in seconds) in which relevant script objects call their
  ///   "OnThink" function
  /// 
  /// The default value is 0.1 (100 ms resp. 10 times per second). The interval can be set to 0.0
  /// to be called every frame. This is however not recommended.
  ///
  /// \param fInterval
  ///   Time interval in seconds
  inline void SetThinkInterval(float fInterval)
  {
    m_fThinkingPos = 0.f;
    m_fThinkingInterval=fInterval;
  }

  /// \brief
  ///   Returns the interval that has been set via SetThinkInterval
  inline float GetThinkInterval() const 
  {
    return m_fThinkingInterval;
  }

protected:
  IVScriptInstancePtr m_spGameScript;
  IVScriptInstancePtr m_spSceneScript;
  IVObjectInstanceFactory *m_pObjFactory;
  bool m_bPaused;
  float m_fThinkingPos;
  float m_fThinkingInterval;
};


#pragma warning(pop)  

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
