/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisShareEntityPlugin.hpp

#ifndef DC_VISSHARED_ENTITY_PLUGIN_HPP
#define DC_VISSHARED_ENTITY_PLUGIN_HPP

#include <Vision/Runtime/Base/Container/VTraits.hpp>


// Code to for Entity Plugins
typedef void (*PD_InitProc)(VARIABLE_LIST *varList);
typedef void (*PD_DeinitProc)(VARIABLE_LIST *pVarList);

#ifndef _VISION_DOC

class VCreateTypeVarListSelfInit
{
public:
  VCreateTypeVarListSelfInit(VType *pType,PD_InitProc initProc,PD_DeinitProc deInitProc, const char *szDesc, int iFlags, const char *szModel)
  {
    _pType = pType;
    _deInitProc = deInitProc;
    if (!pType->m_pVarList)
      pType->m_pVarList = new VARIABLE_LIST(szDesc,iFlags,szModel);
    initProc(pType->m_pVarList);
  }

  ~VCreateTypeVarListSelfInit()
  {
    if (_pType->m_pVarList)
    {
      _deInitProc(_pType->m_pVarList);
      delete _pType->m_pVarList;
      _pType->m_pVarList = NULL;
    }
  }
  VType *_pType;
  PD_DeinitProc _deInitProc;
};


#define DO_STATIC_INITIALIZATION(klass,_desc,_flags,_model) \
  VCreateTypeVarListSelfInit selfInit_##klass(V_RUNTIME_CLASS(klass), klass::klass##_BuildVarList, klass::klass##_DeInitVarList,_desc,_flags,_model);


////////////////////////////////////////////////////////////////////////
#define DEFINE_VAR(klass,the_var_type,the_var_name,var_desc,def_value,hints,editorhintstr)      \
varList->Add(  VisVariable_cl::NewVariable( #the_var_name,var_desc,the_var_type, (int)offsetof(klass,the_var_name) ,(char *)def_value,(VDisplayHintFlags_e)hints,editorhintstr)  )

#define DEFINE_VAR_AND_NAME(klass,the_var_type,the_var,the_name,var_desc,def_value,hints,editorhintstr)      \
  varList->Add(  VisVariable_cl::NewVariable( #the_var,var_desc,the_var_type, (int)offsetof(klass,the_var) ,(char *)def_value,(VDisplayHintFlags_e)hints,editorhintstr, 0, the_name)  )

#define DEFINE_VAR_EX(klass,the_var_type,the_var_name, var_desc,def_value,hints,editorhintstr,extra_info) \
varList->Add(  VisVariable_cl::NewVariable( #the_var_name,var_desc,the_var_type, (int)offsetof(klass,the_var_name) ,(char *)def_value,(VDisplayHintFlags_e)hints,editorhintstr,(size_t)extra_info)  )

#define DEFINE_VAR_EX_AND_NAME(klass,the_var_type,the_var,the_name, var_desc,def_value,hints,editorhintstr,extra_info) \
  varList->Add(  VisVariable_cl::NewVariable( #the_var,var_desc,the_var_type, (int)offsetof(klass,the_var) ,(char *)def_value,(VDisplayHintFlags_e)hints,editorhintstr,(size_t)extra_info, the_name)  )

#define DEFINE_CATEGORY(klass,catname) \
  VisVariable_cl::s_szActiveCategory = catname;

#endif




/// \brief
///   Declares an integer variable in a specific class
/// 
/// \param klass
///   Name of the class the variable is (ex.  FlyingEntity_cl)
/// 
/// \param the_var_name
///   Name of the variable (ex.  speed)
/// 
/// \param desc
///   Description of this variable  (ex. "The speed of the entity")
/// 
/// \param def_value
///   Default value for this variable (ex. "100" or 0 for type default)
/// 
/// \param hints
///   A bit constant from the VDisplayHintFlags_e enum that defines an editing hint (e.g. the file
///   browse dialog) in the editor. Can be 0.
/// 
/// \param editorhints
///   string to provide editing hints for the editor
/// 
/// \example
///   \code
///   DEFINE_VAR_INT(FlyingEntity_cl,speed,"The speed of the entity", "100",0,0)
///   \endcode
#define DEFINE_VAR_INT(klass,the_var_name,desc,def_value,hints,editorhints) DEFINE_VAR(klass,VULPTYPE_INT,the_var_name,desc,def_value,hints,editorhints)

/// \brief The same as DEFINE_VAR_INT, but additionally allows to specify a display name for the GUI.
#define DEFINE_VAR_INT_AND_NAME(klass,the_var, the_name,desc,def_value,hints,editorhints) DEFINE_VAR_AND_NAME(klass,VULPTYPE_INT,the_var,the_name,desc,def_value,hints,editorhints)

/// \brief
///   Same as DEFINE_VAR_INT but maps to an unsigned int
#define DEFINE_VAR_UINT(klass,the_var_name,desc,def_value,hints,editorhints) DEFINE_VAR(klass,VULPTYPE_UINT,the_var_name,desc,def_value,hints,editorhints)

/// \brief The same as DEFINE_VAR_UINT, but additionally allows to specify a display name for the GUI.
#define DEFINE_VAR_UINT_AND_NAME(klass,the_var, the_name,desc,def_value,hints,editorhints) DEFINE_VAR_AND_NAME(klass,VULPTYPE_UINT,the_var,the_name,desc,def_value,hints,editorhints)

/// \brief
///   Same as DEFINE_VAR_UINT but shows a bitmask in vForge
#define DEFINE_VAR_BITMASK(klass,the_var_name,desc,def_value,hints,editorhints) DEFINE_VAR(klass,VULPTYPE_BITMASK,the_var_name,desc,def_value,hints,editorhints)

/// \brief The same as DEFINE_VAR_BITMASK, but additionally allows to specify a display name for the GUI.
#define DEFINE_VAR_BITMASK_AND_NAME(klass,the_var, the_name,desc,def_value,hints,editorhints) DEFINE_VAR_AND_NAME(klass,VULPTYPE_BITMASK,the_var,the_name,desc,def_value,hints,editorhints)

/// \brief
///   Declares an 32bit VColorRef variable in a specific class
/// 
/// \param klass
///   Name of the class the variable is (ex.  FlyingEntity_cl)
/// 
/// \param the_var_name
///   Name of the variable (ex.  speed), must be of type VColorRef
/// 
/// \param desc
///   Description of this variable  (ex. "The speed of the entity")
/// 
/// \param def_value
///   Default value for this variable (ex. "100/100/100/255" or 0 for type default)
/// 
/// \param hints
///   A bit constant from the VDisplayHintFlags_e enum that defines an editing hint (e.g. the file
///   browse dialog) in the editor. Can be 0.
/// 
/// \param editorhints
///   string to provide editing hints for the editor
/// 
/// \example
///   \code
///   DEFINE_VAR_COLORREF(FlyingEntity_cl,color,"The display color", "255/0/0/128",0,0)
///   \endcode
#define DEFINE_VAR_COLORREF(klass,the_var_name,desc,def_value,hints,editorhints) DEFINE_VAR(klass,VULPTYPE_BYTE_COLOR4,the_var_name,desc,def_value,hints,editorhints)

/// \brief The same as DEFINE_VAR_UINT, but additionally allows to specify a display name for the GUI.
#define DEFINE_VAR_COLORREF_AND_NAME(klass,the_var, the_name,desc,def_value,hints,editorhints) DEFINE_VAR_AND_NAME(klass,VULPTYPE_BYTE_COLOR4,the_var,the_name,desc,def_value,hints,editorhints)


#ifndef _VISION_DOC

// compile time asserts - used to generate errors during compilation time if an assert fails
// NOTE: this assert here is usually thrown when you have an entity variable of type bool and want to put it into the
//       var table, where it is considerd to be of type BOOL which has different sizeof than bool
//       solution: change "bool" in class declaration to "BOOL" and do a _rebuild_. 
template <bool B> struct v_tAssert {};
template <> struct v_tAssert<true> { static void v_assert() {} };
#define SV_COMP_ASSERT(__a) {const bool __b = (__a) ? true : false; v_tAssert<__b>::v_assert();}

#endif


/// \brief
///   Declares a boolean variable in a specific class
/// 
/// \param klass
///   Name of the class the variable is (ex.  FlyingEntity_cl)
/// 
/// \param the_var_name
///   Name of the variable (ex.  bCanFly)
/// 
/// \param desc
///   Description of this variable  (ex. "Can the entity fly")
/// 
/// \param def_value
///   Default value for this variable (ex. "TRUE")
/// 
/// \param hints
///   A bit constant from the VDisplayHintFlags_e enum that defines an editing hint (e.g. the file
///   browse dialog) in the editor. Can be 0.
/// 
/// \param editorhints
///   string to provide editing hints for the editor
/// 
/// \example
///   \code
///   DEFINE_VAR_BOOL(FlyingEntity_cl,bCanFly,"Can the entity fly", "TRUE",0,0)
///   \endcode
#define DEFINE_VAR_BOOL(klass,the_var_name,desc,def_value,hints,editorhints)\
  SV_COMP_ASSERT(sizeof(((klass*)0)->the_var_name)==sizeof(BOOL));\
  DEFINE_VAR(klass,VULPTYPE_BOOL,the_var_name,desc,def_value,hints,editorhints)

/// \brief The same as DEFINE_VAR_BOOL, but additionally allows to specify a display name for the GUI.
#define DEFINE_VAR_BOOL_AND_NAME(klass,the_var, the_name,desc,def_value,hints,editorhints)\
  SV_COMP_ASSERT(sizeof(((klass*)0)->the_var)==sizeof(BOOL));\
  DEFINE_VAR_AND_NAME(klass,VULPTYPE_BOOL,the_var,the_name,desc,def_value,hints,editorhints)


/// \brief
///   Declares a float variable in a specific class
/// 
/// \param klass
///   Name of the class the variable is (ex.  FlyingEntity_cl)
/// 
/// \param the_var_name
///   Name of the variable (ex.  speed)
/// 
/// \param desc
///   Description of this variable  (ex. "The speed of the entity")
/// 
/// \param def_value
///   Default value for this variable (ex. "100" or 0 for type default)
/// 
/// \param hints
///   A bit constant from the VDisplayHintFlags_e enum that defines an editing hint (e.g. the file
///   browse dialog) in the editor. Can be 0.
/// 
/// \param editorhints
///   string to provide editing hints for the editor
/// 
/// \example
///   \code
///   DEFINE_VAR_FLOAT(FlyingEntity_cl,speed,"The speed of the entity", "100" ,0,0)
///   \endcode
#define DEFINE_VAR_FLOAT(klass,the_var_name,desc,def_value,hints,editorhints) DEFINE_VAR(klass,VULPTYPE_FLOAT,the_var_name,desc,def_value,hints,editorhints)

/// \brief The same as DEFINE_VAR_FLOAT, but additionally allows to specify a display name for the GUI.
#define DEFINE_VAR_FLOAT_AND_NAME(klass,the_var, the_name,desc,def_value,hints,editorhints) DEFINE_VAR_AND_NAME(klass,VULPTYPE_FLOAT,the_var,the_name,desc,def_value,hints,editorhints)

/// \brief
///   Declares a double variable in a specific class
/// 
/// \param klass
///   Name of the class the variable is (ex.  FlyingEntity_cl)
/// 
/// \param the_var_name
///   Name of the variable (ex.  speed)
/// 
/// \param desc
///   Description of this variable  (ex. "The speed of the entity")
/// 
/// \param def_value
///   Default value for this variable (ex. "100" or 0 for type default)
/// 
/// \param hints
///   A bit constant from the VDisplayHintFlags_e enum that defines an editing hint (e.g. the file
///   browse dialog) in the editor. Can be 0.
/// 
/// \param editorhints
///   string to provide editing hints for the editor
/// 
/// \example
///   \code
///   DEFINE_VAR_DOUBLE(FlyingEntity_cl,speed,"The speed of the entity", "100" ,0,0)
///   \endcode
#define DEFINE_VAR_DOUBLE(klass,the_var_name,desc,def_value,hints,editorhints)  DEFINE_VAR(klass,VULPTYPE_DOUBLE,the_var_name,desc,def_value,hints,editorhints)

/// \brief The same as DEFINE_VAR_DOUBLE, but additionally allows to specify a display name for the GUI.
#define DEFINE_VAR_DOUBLE_AND_NAME(klass,the_var, the_name,desc,def_value,hints,editorhints) DEFINE_VAR_AND_NAME(klass,VULPTYPE_DOUBLE,the_var,the_name,desc,def_value,hints,editorhints)

/// \brief
///   Declares an integer vector variable in a specific class
/// 
/// \param klass
///   Name of the class the variable is (ex.  FlyingEntity_cl)
/// 
/// \param the_var_name
///   Name of the variable (ex.  speed)
/// 
/// \param desc
///   Description of this variable  (ex. "Wind direction")
/// 
/// \param def_value
///   Default value for this variable (ex. "100/0/0" or 0 for type default)
/// 
/// \param hints
///   A bit constant from the VDisplayHintFlags_e enum that defines an editing hint (e.g. the file
///   browse dialog) in the editor. Can be 0.
/// 
/// \param editorhints
///   string to provide editing hints for the editor
/// 
/// \note
///   The vector must have dimension 3
/// 
/// \note
///   Example variable 'int someVector[3]'
/// 
/// \example
///   \code
///   DEFINE_VAR_VETOR_INTEGER(FlyingEntity_cl,speed,"Wind direction", "100/0/0" ,0,0)
///   \endcode
#define DEFINE_VAR_VECTOR_INT(klass,the_var_name,desc,def_value,hints,editorhints)  DEFINE_VAR(klass,VULPTYPE_VECTOR_INT,the_var_name,desc,def_value,hints,editorhints)

/// \brief The same as DEFINE_VAR_VECTOR_INT, but additionally allows to specify a display name for the GUI.
#define DEFINE_VAR_VECTOR_INT_AND_NAME(klass,the_var, the_name,desc,def_value,hints,editorhints) DEFINE_VAR_AND_NAME(klass,VULPTYPE_VECTOR_INT,the_var,the_name,desc,def_value,hints,editorhints)

/// \brief
///   Declares a float vector variable in a specific class
/// 
/// \param klass
///   Name of the class the variable is (ex.  FlyingEntity_cl)
/// 
/// \param the_var_name
///   Name of the variable (ex.  speed)
/// 
/// \param desc
///   Description of this variable  (ex. "Wind direction")
/// 
/// \param def_value
///   Default value for this variable (ex. "100/0/0" or 0 for type default)
/// 
/// \param hints
///   A bit constant from the VDisplayHintFlags_e enum that defines an editing hint (e.g. the file
///   browse dialog) in the editor. Can be 0.
/// 
/// \param editorhints
///   string to provide editing hints for the editor
/// 
/// \note
///   The vector must have dimension 3
/// 
/// \note
///   Example variable 'float someVector[3]'
/// 
/// \example
///   \code
///   DEFINE_VAR_VECTOR_FLOAT(FlyingEntity_cl,speed,"Wind direction", "100/0/0" ,0,0)
///   \endcode
#define DEFINE_VAR_VECTOR_FLOAT(klass,the_var_name,desc,def_value,hints,editorhints)    DEFINE_VAR(klass,VULPTYPE_VECTOR_FLOAT,the_var_name,desc,def_value,hints,editorhints)

/// \brief The same as DEFINE_VAR_VECTOR_FLOAT, but additionally allows to specify a display name for the GUI.
#define DEFINE_VAR_VECTOR_FLOAT_AND_NAME(klass,the_var, the_name,desc,def_value,hints,editorhints) DEFINE_VAR_AND_NAME(klass,VULPTYPE_VECTOR_FLOAT,the_var,the_name,desc,def_value,hints,editorhints)

/// \brief
///   Declares a double vector variable in a specific class
/// 
/// \param klass
///   Name of the class the variable is (ex.  FlyingEntity_cl)
/// 
/// \param the_var_name
///   Name of the variable (ex.  speed)
/// 
/// \param desc
///   Description of this variable  (ex. "Wind direction")
/// 
/// \param def_value
///   Default value for this variable (ex. "100/0/0" or 0 for type default)
/// 
/// \param hints
///   A bit constant from the VDisplayHintFlags_e enum that defines an editing hint (e.g. the file
///   browse dialog) in the editor. Can be 0.
/// 
/// \param editorhints
///   string to provide editing hints for the editor
/// 
/// \note
///   The vector must have dimension 3
/// 
/// \note
///   Example variable 'double someVector[3]'
/// 
/// \example
///   \code
///   DEFINE_VAR_VECTOR_DOUBLE(FlyingEntity_cl,speed,"Wind direction", "100/0/0" ,0,0)
///   \endcode
#define DEFINE_VAR_VECTOR_DOUBLE(klass,the_var_name,desc,def_value,hints,editorhints)   DEFINE_VAR(klass,VULPTYPE_VECTOR_DOUBLE,the_var_name,desc,def_value,hints,editorhints)

/// \brief The same as DEFINE_VAR_VECTOR_DOUBLE, but additionally allows to specify a display name for the GUI.
#define DEFINE_VAR_VECTOR_DOUBLE_AND_NAME(klass,the_var, the_name,desc,def_value,hints,editorhints) DEFINE_VAR_AND_NAME(klass,VULPTYPE_VECTOR_DOUBLE,the_var,the_name,desc,def_value,hints,editorhints)

/// \brief
///   Declares an enumeration variable in a specific class
/// 
/// \param klass
///   Name of the class the variable is (ex.  FlyingEntity_cl)
/// 
/// \param the_var_name
///   Name of the variable (ex.  size)
/// 
/// \param desc
///   Description of this variable  (ex. "Size of the entity")
/// 
/// \param def_value
///   Default value for this variable (ex. "Large")
/// 
/// \param the_enum_list
///   List of possible values seperated by commas. No spaces before/afer commas. (ex.
///   "Large,Medium,Small")
/// 
/// \param hints
///   A bit constant from the VDisplayHintFlags_e enum that defines an editing hint (e.g. the file
///   browse dialog) in the editor. Can be 0.
/// 
/// \param editorhints
///   string to provide editing hints for the editor
/// 
/// \note
///   The variable type of the enumeration is 'int'. The first enumeration's value is 0, the second
///   is 1 etc..
/// 
/// \note
///   ex.  int speed;  in the FlyingEntity_cl class
/// 
/// \example
///   \code
///   DEFINE_VAR_ENUM(FlyingEntity_cl,size,"Size of the entity", "Large", "Large,Medium,Small" ,0,0)
///   \endcode
#define DEFINE_VAR_ENUM(klass,the_var_name,desc,def_value,the_enum_list,hints,editorhints)  DEFINE_VAR_EX(klass,VULPTYPE_ENUM,the_var_name,desc,def_value,hints,editorhints,the_enum_list)

/// \brief The same as DEFINE_VAR_ENUM, but additionally allows to specify a display name for the GUI.
#define DEFINE_VAR_ENUM_AND_NAME(klass,the_var,the_name,desc,def_value,the_enum_list,hints,editorhints)  DEFINE_VAR_EX_AND_NAME(klass,VULPTYPE_ENUM,the_var,the_name,desc,def_value,hints,editorhints,the_enum_list)


/// \brief
///   Declares a string variable in a specific class
///
/// The variable type of the string is 'char []'. Do not use other string classes such as std::string as variable
/// type since this will result in a crash. If you want to use VString then use DEFINE_VAR_VSTRING instead.
/// 
/// \param klass
///   Name of the class the variable is (ex.  MessageBox_cl)
/// 
/// \param the_var_name
///   Name of the variable (ex.  message)
/// 
/// \param desc
///   Description of this variable  (ex. "Message to show")
/// 
/// \param def_value
///   Default value for this variable (ex. "This is a message")
/// 
/// \param thesize
///   The size of the string in characters. thesize-1 characters are allowed because the of the terminating 0.
///   This size must be at most as large as the size of the string buffer. Otherwise a compile assert is triggered ('negative subscript')
/// 
/// \param hints
///   A bit constant from the VDisplayHintFlags_e enum that defines an editing hint (e.g. the file
///   browse dialog) in the editor. Can be 0.
/// 
/// \param editorhints
///   string to provide editing hints for the editor
/// 
/// \note
///   The variable type of the string is 'char []'
/// 
/// \note
///   ex.  char message[64];  in the MessageBox_cl class
/// 
/// \example
///   \code
///   DEFINE_VAR_STRING(MessageBox_cl,message,"Message to show", "This is a message", 64 ,0,0)
///   \endcode
#define DEFINE_VAR_STRING(klass,the_var_name,desc,def_value,thesize,hints,editorhints) \
{\
  V_COMPILE_ASSERT(thesize<=sizeof(((klass*)NULL)->the_var_name));\
  DEFINE_VAR_EX(klass,VULPTYPE_STRING,the_var_name,desc,def_value,hints,editorhints,thesize);\
}

/// \brief The same as DEFINE_VAR_STRING, but additionally allows to specify a display name for the GUI.
#define DEFINE_VAR_STRING_AND_NAME(klass,the_var,the_name,desc,def_value,thesize,hints,editorhints) \
{\
  V_COMPILE_ASSERT(thesize<=sizeof(((klass*)NULL)->the_var));\
  DEFINE_VAR_EX_AND_NAME(klass,VULPTYPE_STRING,the_var,the_name,desc,def_value,hints,editorhints,thesize);\
}


/// \brief
///   Declares a pointer to a string variable in a specific class
/// 
/// \param klass
///   Name of the class the variable is (ex.  MessageBox_cl)
/// 
/// \param the_var_name
///   Name of the variable (ex.  message)
/// 
/// \param desc
///   Description of this variable  (ex. "Message to show")
/// 
/// \param def_value
///   Default value for this variable (ex. "This is a message")
/// 
/// \param thesize
///   The size of the string in characters
/// 
/// \param hints
///   A bit constant from the VDisplayHintFlags_e enum that defines an editing hint (e.g. the file
///   browse dialog) in the editor. Can be 0.
/// 
/// \param editorhints
///   string to provide editing hints for the editor
/// 
/// \note
///   The variable type of the string is 'char *'
/// 
/// \note
///   ex.  char *message;  in the MessageBox_cl class
/// 
/// \note
///   You have to give the size. This is so the editor can allocate enough memory for the string
/// 
/// \example
///   \code
///   DEFINE_VAR_PSTRING(MessageBox_cl,message,"Message to show", "This is a message", 64 ,0,0)
///   \endcode
#define DEFINE_VAR_PSTRING(klass,the_var_name,desc,def_value,thesize,hints,editorhints) DEFINE_VAR_EX(klass,VULPTYPE_PSTRING,the_var_name,desc,def_value,hints,editorhints,thesize)

/// \brief The same as DEFINE_VAR_PSTRING, but additionally allows to specify a display name for the GUI.
#define DEFINE_VAR_PSTRING_AND_NAME(klass,the_var,the_name,desc,def_value,thesize,hints,editorhints) DEFINE_VAR_EX_AND_NAME(klass,VULPTYPE_PSTRING,the_var,the_name,desc,def_value,hints,editorhints,thesize)

/// \brief
///   Declares a VString variable in a specific class
/// 
/// \param klass
///   Name of the class the variable is (ex.  MessageBox_cl)
/// 
/// \param the_var_name
///   Name of the variable (ex.  m_sMessage)
/// 
/// \param desc
///   Description of this variable  (ex. "Message to show")
/// 
/// \param def_value
///   Default value for this variable (ex. "This is a message")
/// 
/// \param thesize
///   The maximum size of the string in characters
/// 
/// \param hints
///   A bit constant from the VDisplayHintFlags_e enum that defines an editing hint (e.g. the file
///   browse dialog) in the editor. Can be 0.
/// 
/// \param editorhints
///   string to provide editing hints for the editor
/// 
/// \note
///   The variable type of the string must be 'VString'
/// 
/// \note
///   ex.  VString m_sMessage;
/// 
/// \note
///   The size could be used to limit the maximum string length
/// 
/// \example
///   \code
///   DEFINE_VAR_VSTRING(MessageBox_cl, m_sMessage,"Message to show", "This is a message", 64 ,0,0)
///   \endcode
#define DEFINE_VAR_VSTRING(klass,the_var_name,desc,def_value,thesize,hints,editorhints) DEFINE_VAR_EX(klass,VULPTYPE_VSTRING,the_var_name,desc,def_value,hints,editorhints,thesize)

/// \brief The same as DEFINE_VAR_VSTRING, but additionally allows to specify a display name for the GUI.
#define DEFINE_VAR_VSTRING_AND_NAME(klass,the_var,the_name,desc,def_value,thesize,hints,editorhints) DEFINE_VAR_EX_AND_NAME(klass,VULPTYPE_VSTRING,the_var,the_name,desc,def_value,hints,editorhints,thesize)

/// \brief
///   Declares a String variable that is not bound to a variable member. Instead it just calls the
///   virtual OnVariableValueChanged function on the parent class
/// 
/// Accordingly the string is not stored in the class
/// 
/// \param klass
///   Owner class
/// 
/// \param the_var_name
///   Name of the variable, not decessarily a data member in this case
/// 
/// \param desc
///   Description of this variable
/// 
/// \param def_value
///   Default value for this variable (ex. "This is a message")
/// 
/// \param hints
///   A bit constant from the VDisplayHintFlags_e enum that defines an editing hint (e.g. the file
///   browse dialog) in the editor. Can be 0.
/// 
/// \param editorhints
///   string to provide editing hints for the editor

// Example:
//    - DEFINE_VAR_STRING_CALLBACK(MyComponent, TextureFile, "Texture to display", "white.dds", 0,"")
//
#define DEFINE_VAR_STRING_CALLBACK(klass,the_var_name,desc,def_value,hints,editorhints) \
  varList->Add(  VisVariable_cl::NewVariable( #the_var_name,desc,VULPTYPE_STRING, -1 ,(char *)def_value,(VDisplayHintFlags_e)hints,editorhints,0)  )

/// \brief The same as DEFINE_VAR_STRING_CALLBACK, but additionally allows to specify a display name for the GUI.
#define DEFINE_VAR_STRING_CALLBACK_AND_NAME(klass,the_var,the_name,desc,def_value,hints,editorhints) \
  varList->Add(  VisVariable_cl::NewVariable( #the_var,desc,VULPTYPE_STRING, -1 ,(char *)def_value,(VDisplayHintFlags_e)hints,editorhints,0, the_name)  )

/// \brief
///   Declares a key variable in a specific class
///   DEPRECATED; use DEFINE_VAR_VSTRING instead, with the editor hints "EntityKeys", "LighsourceKeys", "PathKeys" or "CubemapKeys"
///
/// \param keytype
///   Type of the key: ENTITY, PRIMITIVE, VERTEX, LIGHTSOURCE or PATH
/// 
/// \param klass
///   Name of the class the variable is (ex. FlyingEntity_cl)
/// 
/// \param the_var_name
///   Name of the variable (ex. target)
/// 
/// \param desc
///   Description of this variable  (ex. "Target Entity")
/// 
/// \param def_value
///   Default value for this variable (ex. "steve")
/// 
/// \param hints
///   A bit constant from the VDisplayHintFlags_e enum that defines an editing hint (e.g. the file
///   browse dialog) in the editor. Can be 0.
/// 
/// \param editorhints
///   string to provide editing hints for the editor
/// 
/// \note
///   The variable type of the string is 'char *'
/// 
/// \note
///   ex.  char *someKey;  in the class
/// 
/// \example
///   \code
///   DEFINE_VAR_KEY(ENTITY,FlyingEntity_cl,target,"Target Entity", "steve" ,0,0)
///   \endcode
#define DEFINE_VAR_KEY(keytype,klass,the_var_name,desc,def_value,hints,editorhints)   DEFINE_VAR(klass,VULPTYPE_##keytype##_KEY,the_var_name,desc,def_value,hints,editorhints)

/// \brief
///   The same as DEFINE_VAR_KEY, but additionally allows to specify a display name for the GUI.
///   DEPRECATED; use DEFINE_VAR_VSTRING_AND_NAME instead, with the editor hints "EntityKeys", "LighsourceKeys", "PathKeys" or "CubemapKeys"
#define DEFINE_VAR_KEY_AND_NAME(keytype,klass,the_var,the_name,desc,def_value,hints,editorhints)   DEFINE_VAR_AND_NAME(klass,VULPTYPE_##keytype##_KEY,the_var,the_name,desc,def_value,hints,editorhints)

/// \brief
///   Declares a model variable in a specific class
/// 
/// This model will override the default one in the class definition
/// 
/// \param klass
///   Name of the class the variable is (ex. FlyingEntity_cl)
/// 
/// \param the_var_name
///   Name of the variable (ex.  extraModel)
/// 
/// \param desc
///   Description of this variable  (ex. "Override model")
/// 
/// \param def_value
///   Default value for this variable (ex. "airplane")
/// 
/// \param hints
///   A bit constant from the VDisplayHintFlags_e enum that defines an editing hint (e.g. the file
///   browse dialog) in the editor. Can be 0.
/// 
/// \param editorhints
///   string to provide editing hints for the editor
/// 
/// \note
///   Only one overriding model variable is used in a class. The additional ones are ignored
/// 
/// \note
///   The variable type of the string is 'char *'
/// 
/// \note
///   ex.  char *model;  in the class
/// 
/// \example
///   \code
///   DEFINE_VAR_MODEL(FlyingEntity_cl,extraModel,"Override model", "airplane" ,0,0)
///   \endcode
#define DEFINE_VAR_MODEL(klass,the_var_name,desc,def_value,hints,editorhints)       DEFINE_VAR(klass,VULPTYPE_MODEL,the_var_name,desc,def_value,hints,editorhints)

/// \brief The same as DEFINE_VAR_MODEL, but additionally allows to specify a display name for the GUI.
#define DEFINE_VAR_MODEL_AND_NAME(klass,the_var,the_name,desc,def_value,hints,editorhints)       DEFINE_VAR_AND_NAME(klass,VULPTYPE_MODEL,the_var,the_name,desc,def_value,hints,editorhints)

/// \brief
///   Declares a shader effect variable in a specific class
/// 
/// \param klass
///   Name of the class the variable is (ex. FlyingEntity_cl)
/// 
/// \param the_var_name
///   Name of the variable (ex. extraShaderEffect of type VCompiledEffectPtr)
/// 
/// \param desc
///   Description of this variable (ex. "Override shader effect")
/// 
/// \param def_value
///   Default value for this variable (ex. "Default.ShaderLib|DefaultEffectName|DefaultShaderParms")
/// 
/// \param hints
///   A bit constant from the VDisplayHintFlags_e enum that defines an editing hint (e.g. the file
///   browse dialog) in the editor. Can be 0.
/// 
/// \param editorhints
///   string to provide editing hints for the editor
/// 
/// \note
///   The variable type of the string is 'char *'
#define DEFINE_VAR_SHADEREFFECT(klass, the_var_name, desc, def_value, hints, editorhints) \
  DEFINE_VAR(klass, VULPTYPE_SHADEREFFECT, the_var_name, desc, def_value, hints, editorhints)

/// \brief The same as DEFINE_VAR_SHADEREFFECT, but additionally allows to specify a display name for the GUI.
#define DEFINE_VAR_SHADEREFFECT_AND_NAME(klass, the_var,the_name, desc, def_value, hints, editorhints) \
  DEFINE_VAR_AND_NAME(klass, VULPTYPE_SHADEREFFECT, the_var,the_name, desc, def_value, hints, editorhints)

#if !defined _VISION_DOC
//If you get a compile time error in this function you did not use a VTypeObjectReference 
//for the DEFINE_VAR_OBJECT_REFERENCE macro
class VTypedObjectReference;

template<typename T>
inline void VCheckObjectReference(T& arg) 
{
  typedef typename VTraits::IsImplicitlyConvertible< /*FROM*/ T*, /*TO*/ VTypedObjectReference*> condition;
  V_COMPILE_ASSERT(condition::value);
}
#endif

/// \brief
///   Declares an object reference variable in a specific class
/// 
/// The member in the class must be of type VTypedObjectReference which will hold the reference to the other object
/// You can also use the VObjectReference<T> helper template for type safe usage without casting
/// 
/// \param klass
///   Name of the class the variable is (ex. FlyingEntity_cl)
/// 
/// \param the_var_name
///   Name of the variable (ex. WalkPath)
/// 
/// \param desc
///   Description of this variable (ex. "Reference to the path that this entity should move on")
/// 
/// \param def_value
///   Must be 0
/// 
/// \param hints
///   Not used here, should be 0
/// 
/// \param editorhints
///   Not used here, should be NULL
/// 
/// \example
///   \code
///   DEFINE_VAR_OBJECT_REFERENCE(FlyingEntity_cl,WalkPath,"Path reference", NULL ,0,NULL)
///   \endcode
#define DEFINE_VAR_OBJECT_REFERENCE(klass, the_var_name, desc, def_value, hints, editorhints)     VCheckObjectReference(((klass*)0)->the_var_name); DEFINE_VAR(klass, VULPTYPE_REFERENCED_OBJECT, the_var_name, desc, def_value, hints, editorhints)


#define START_BASE_VAR_TABLE(klass,_desc,_flags,_model)             \
DO_STATIC_INITIALIZATION(klass,_desc,_flags,_model)               \
void klass::klass##_DeInitVarList(VARIABLE_LIST *pVarList) \
{ \
  bool bOwnList = TRUE; \
  VARIABLE_LIST *pList = V_RUNTIME_CLASS(klass)->m_pVarList; \
  if ( pVarList ) \
  { \
    pList = pVarList; \
    bOwnList = FALSE; \
  } \
  if ( !pList ) \
    return; \
  VARIABLE_ELEM *elem = pList->first; \
  while (elem) \
  { \
    VisVariable_cl *var = elem->value; \
    VisVariable_cl::DeleteVariable(var); \
    elem = elem->next; \
  } \
  pList->Clear(); \
  if ( bOwnList ) \
  { \
    delete pList; \
    V_RUNTIME_CLASS(klass)->m_pVarList = NULL; \
  } \
} \
void klass::klass##_BuildVarList(VARIABLE_LIST *varList) \
{\
  VisVariable_cl::s_szActiveCategory = NULL;

/// \brief
///   Starts the variable table of a specific class
/// 
/// \param klass
///   Name of the class the variable table is for (ex.  FlyingEntity_cl)
/// 
/// \param parent_klass
///   Parent class of this class (ex.  VisBaseEntity_cl)
/// 
/// \param _desc
///   Description of this class  (ex. "This is a flying entity")
/// 
/// \param _flag
///   Flags for this entity (ex. VEF_xxxxx.VEDF_xxxxx or 0)
/// 
/// \param _model
///   In case of entity classes: Default model file for an entity of this class (ex. "bird" or ""). 
///   The filename has to be relative to your game project.
///   In case of component classes: Nice name of the component, as displayed in vForge
/// 
/// \note
///   The START_VAR_TABLE macro is followed by zero or more DEFINE_VAR_xxxxx macros
/// 
/// \note
///   Remember to end the variable table with the END_VAR_TABLE macro
/// 
/// \example
///   \code
///   START_VAR_TABLE(FlyingEntity_cl,VisBaseEntity_cl,"This is a flying entity",  VEDF_FIXED_MODEL ,"bird")
///   \endcode
#define START_VAR_TABLE(klass,parent_klass,_desc,_flag,_model)      \
START_BASE_VAR_TABLE(klass,_desc,_flag,_model)              \
  parent_klass::parent_klass##_BuildVarList(varList);

/// \brief
///   Ends the variable table of a specific class
/// 
/// \example
///   \code
///   END_VAR_TABLE()
///   \endcode
#define END_VAR_TABLE }


/// \brief
///   Implements some required functions in a class
/// 
/// \param klass
///   Name of this class
/// 
/// \example
///   \code
///   class SomeClass_cl: public VisBaseEntity_cl
///   { public:
///     ........
///     IMPLEMENT_OBJ_CLASS(SomeClass_cl)
///   };
///   \endcode
#define IMPLEMENT_OBJ_CLASS(klass)                              \
  IMPLEMENT_OBJ_CLASS_IMPEXP(VISION_APIFUNC, klass)


// remap
#define IMPLEMENT_OBJ_CLASS_IMPEXP(_IMPEXP,klass)  V_DECLARE_VARTABLE(klass,_IMPEXP)


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
