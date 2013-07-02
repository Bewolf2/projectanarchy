/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VParam.hpp

#ifndef _VISION_VPARAM_HPP
#define _VISION_VPARAM_HPP

#include <Vision/Runtime/Base/String/VStringTokenizer.hpp>
#include <Vision/Runtime/Base/String/VStrRef.hpp>
#include <Vision/Runtime/Base/System/Param/VParamType.hpp>

// this class is not relevant for the engine
#ifndef _VISION_DOC

#define IsController(type)   (type >= V_TYPE_FIRST_CTRL && type <= V_TYPE_LAST_CTRL)

// Is this a complex parameter that we can't serialize directly (ie. a pointer)
#define IsComplexParam(type)   (type >= V_TYPE_FIRST_COMPLEX && type <= V_TYPE_LAST_COMPLEX)

// Parameter flags
// Bits 0..9 are editor settings
// Bits 10..27 are hint flags
// Bits 28..31 are automatically assigned internal flags 
// (more internal flags can be added if needed by taking from the hint flags)

#define PF_HIDDEN                 (1 << 0)        ///< Parameter is internal and can not be seen or edited
#define PF_READONLY               (1 << 1)        ///< Parameter can be seen but not edited in property editor or setvar action (but the param can be changed with the 

// If a variable has this flag, then changing this variable will cause the editor 
// views to be redraw.  Used for variable on shapes & nodes that affect the shape. 
// Examples of this are "width", "segments","sides","color",...
#define PF_EDITOR_VISUAL_EFFECT   (1 << 2)

#define PF_NON_PERSISTENT         (1 << 3)         ///< not saved or loaded

//Hint to what a field contains (Used internally in the editor)
//The hint flags can overlap since the parameter type determines which hint flags to use


//Hints for V_TYPE_COLOR and V_TYPE_COLORFLOAT
#define PF_HINT_USEALPHA        (1 << 10)  ///< this color uses has an alpha value
#define PF_HINT_ASFLOATS        (1 << 11)  ///< the color values should be converted into three floats during export (used for shaders)

//Hints for V_TYPE_FLOAT_VECTOR (NOT for V_TYPE_FLOAT_VEC2 and V_TYPE_FLOAT_VEC4)
  #define PF_HINT_POSITION        (1 << 10)  ///< This is coordinate based and might need to be transformed on export
#define PF_HINT_VECTOR          (1 << 11)  ///< This is coordinate based and might need to be transformed (but not translated)
#define PF_HINT_ROTATION        (1 << 12)  ///< radian based angles

//Hints for V_TYPE_BOOL 
#define PF_HINT_SWITCH          (1 << 10)  ///< A boolean with that hint is exported as a switch (used for shaders)

//Hints for V_TYPE_STRING
#define PF_HINT_TEXTURE_FILE    (1 << 10)   ///< String is a texture file
#define PF_HINT_MODEL_FILE      (1 << 11)   ///< String is a model file
#define PF_HINT_SOUND_FILE      (1 << 12)   ///< String is a sound file
#define PF_HINT_MAP_FILE        (1 << 13)   ///< String is a map file
#define PF_HINT_RESOURCE_FILE   (1 << 14)   ///< String is a generic resource file
#define PF_HINT_LIGHT_KEY       (1 << 18)   ///< String is a light key
#define PF_HINT_PRIMITIVE_KEY   (1 << 19)   ///< String is a polygon key
#define PF_HINT_ENTITY_KEY      (1 << 20)   ///< String is a entity key
#define PF_HINT_EVENTAREA_KEY   (1 << 21)   ///< String is an event area key
#define PF_HINT_CUBEMAPENTITY_KEY (1 << 22) ///< String is a cubemap-entity key (used for shaders)
#define PF_HINT_PATH_KEY        (1 << 23)   ///< String is a path key
#define PF_HINT_TEXTURE_FORMAT  (1 << 24)   ///< String is a texture format string (used for shaders)
#define PF_HINT_CUSTOM_DIALOG   (1 << 25)   ///< the parameter has a customer/user dialog assigned
  
#define PF_NUMERIC_VAL          (1 << 26)  ///< Parameter has a numeric value
#define PF_SPINNER_VAL          (1 << 27)  ///< Parameter has a spinner value


//Automatically assigned flags. Do NOT add these yourself to the parameter description
#define PF_DEFAULT_VAL  (1 << 28)   ///< has a default value
#define PF_MINIMUM_VAL  (1 << 29)   ///< has a minimum value
#define PF_MAXIMUM_VAL  (1 << 30)   ///< has a maximum value
#define PF_VARIABLE     (1 << 31)   ///< linked to a variable

// Combination flags used to check for the presence of one flag

#define PF_HINT_FILE  (PF_HINT_TEXTURE_FILE | PF_HINT_SOUND_FILE | PF_HINT_MODEL_FILE | PF_HINT_MAP_FILE | PF_HINT_RESOURCE_FILE)
#define PF_HINT_KEY   (PF_HINT_LIGHT_KEY | PF_HINT_PRIMITIVE_KEY | PF_HINT_ENTITY_KEY | PF_HINT_EVENTAREA_KEY | PF_HINT_PATH_KEY )

///****End of new parameter sysrem flags*****
//
#define V_INVALID_PARAM -1


/// \brief Internal: String & int pair for dictionary entries
struct dictpair_t
{
  const char* m_pKey;
  int m_iValue;
};

/// \brief Internal: Used by VParam
union ParamValUnion
{
  ParamValUnion()
  {
    m_pString = NULL;
  }

  float    f;
  int      i;
  int flagData[2];        ///< [0] = flag number   [1] = new state (0/1)
  float  v[4];            ///< new float vector to store four floats
  const char* m_pString;  ///< shared static read only string
};


/// \brief Internal class used in some of Vision's Tools. Might get deprecated at some point. Do not use.
class VParam
{
  V_DECLARE_SERIALX( VParam, VBASE_IMPEXP )

public :
  VBASE_IMPEXP VParam();
  VBASE_IMPEXP VParam(const VParam &src);
  VBASE_IMPEXP virtual ~VParam();
  
  VBASE_IMPEXP VParam *Clone();  

  // Deletes the pair entries in m_list
  VBASE_IMPEXP void Reset();

  // Does a case insensitive comparison of the string with the long and short param name
  VBASE_IMPEXP bool NameMatches(const char *pszParam);
  
  // Used for Enum and Flags to find the value of a string matching is _not_ case-sensitive
  VBASE_IMPEXP VBool FindString(const char *pName, int &res);

  //helpers
  VBASE_IMPEXP static VBool IsValidType(VParamType type);
  VBASE_IMPEXP static VParamType GetType( const char *pTypeStr );
  VBASE_IMPEXP static VBool EvalBool(const char *pValue, BOOL* res);

  // pParam is only required for flags and enumerations
  // pIndex is only required for flags and vectors
  VBASE_IMPEXP static VBool ParseString(VParamType eType,const char *pString, ParamValUnion &val, VParam *pParam = NULL, const char *pIndex = NULL);

  /// \brief
  ///   test if the string can be used as parameter name
  /// 
  /// this method checks if the input string can be used as valid parameter name. The valid name
  /// cannot contain some special characters (e.g spaces).
  /// 
  /// \return
  ///   true if the string can be used as parameter name, false otherwise
  VBASE_IMPEXP static bool IsValidParamName( const char* szParamName );
  
  /// \brief
  ///   Sets the VParamType and flags from a engine type. Used by vEdit to build parameters of
  ///   shaders and entities
  /// 
  /// \return
  ///   true if successful
  VBASE_IMPEXP bool SetEngineType(int iEngineType);

  // parameter lock support
  inline VBool    GetLock()            {return  m_bLocked;}
  inline void     SetLock(VBool bLock) {m_bLocked = bLock;}
  
  /// Return if the parameter is a int, float or float vector (with 2, 3, or 4 components)
  /// 
  /// \return
  ///   VBool: TRUE if the parameter represent an int or float (vector) value
 inline VBool IsNumeric() const { return  (m_iFlags & PF_NUMERIC_VAL ) > 0; }
  
  VBASE_IMPEXP float GetFloatDefaultValue(const char* pIndex = NULL);
  VBASE_IMPEXP float GetFloatSpinnerValue();

  /// Sets the default value for a string parameter. Used by entity plugin system to setup the
  /// default string. This value is used for dynamically created parameters and not those created
  /// using the macros
  VBASE_IMPEXP void SetDefaultString(const char *pStr);

  /// \brief
  ///   Test if the parameter is valid.
  /// 
  /// This function does test if the long name, short name (if exists) and the enum names (used for
  /// flags also) are valid variable names. See IsValidVariableName() function (VStringUtil.cpp)
  /// for more info.
  /// 
  /// \return
  ///   VBool: TRUE if the parameter is valid.
  VBASE_IMPEXP VBool IsValid();

  // Adds an enum entry
  VBASE_IMPEXP dictpair_t* AddEnum(const char* pszEnum, int iVal);
  
  // Sorts the enum entries alphabetically
  VBASE_IMPEXP void SortEnums();


  //////////////////////////////////////////////
  //  variables
  //////////////////////////////////////////////
  
  VParamType m_eType;
  int m_iFlags;
  
  int m_id;       ///< variable id. Used to acces the variable with. 
                  // Must match position in paramdesc list. (m_paramList[i]->m_id == i)
  int *m_pId;     ///< ptr to user supplied variable we fill in with the variable id. 
                  // Used later to access the variable
  int m_iParamOffs; ///< offset of variable in paramblock 
  int m_iVarOffs; ///< offset of external variable from start of _class_ (if PF_VARIABLE is set)
  
  //info strings
  const char *m_pShortName;
  VStrRef m_longName;
  VStrRef m_descr;
  const char *m_pCategory;

  VStrRef m_userDialog;  ///< name of the user dialog asigned to this parameter

  // The default, min and max values are only used if the flag is set and
  // the flag is set when the value is set, so we don't have to init them. 
  
  char *m_pDefString; ///< default value for strings of dynamic parameters
  ParamValUnion m_default;  ///< default value
  ParamValUnion m_minimum;  ///< minimum value
  ParamValUnion m_maximum;  ///< maximum value
  union _spinner// spinner value
  {
    int i;
    float f;
  } m_spinner;
  

  VPList   m_list; ///< dictpar_t for enums and flags,
  VBool    m_bOwnStrings; ///< for V_PARAM_ENUMSTRING parameter must own the tokens
  VBool    m_bLocked; ///< indicates whether or not the parameter is locked
  
  //table with sizes of different parameter types
  VBASE_IMPEXP static int ParamSize[V_TYPE_MAX];
  
protected:
  VBASE_IMPEXP void SerializeX( VArchive& ar );
};


// VParam pointer list
DECLARE_LIST_SERIALX(VParamList,VParam, VBASE_IMPEXP )

#endif // VISION_DOC

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
