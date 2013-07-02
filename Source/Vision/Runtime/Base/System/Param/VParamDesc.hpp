/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VParamDesc.hpp

#ifndef _VISION_VPARAMDESC_HPP
#define _VISION_VPARAMDESC_HPP

#include <Vision/Runtime/Base/System/Param/VParam.hpp>

// these classes should not be used by the engine
#ifndef _VISION_DOC


/*
The V_BEGIN_PARAM_DESC() and V_END_PARAM_DESC macros are used to create a VParamDesc
instance for the node classes. 
You can put parameter (variable) descriptions between the BEGIN and END macros.

A node class MUST have a parameter description. If it has no variables it
just has nothing between the BEGIN/END macros.

*/

// Tokens start at a specific number (0x7f34A300) to give us a good chance of recognizing non-tokens
// If the parameter description is wrong (ex. int where float is expected) the allignment will be
// off (since float gets upgraded to double). We can then easily detect if the next token is valid
// since it is in a very specific range.

//Tokens for building the parameter description
// Tokens ending with _VAL have macros that you should rather use
enum 
{
  V_PARAM_TYPE = 0x7f34A300,   
  V_PARAM_LONGNAME,
  V_PARAM_SHORTNAME,
  V_PARAM_CATEGORY,
  V_PARAM_DESCRIPTION,
  V_PARAM_DEFAULT,       ///< NB.  Type _must_ match variable type. (ex. float/int), V_TYPE_STRING doesn't support default value
  V_PARAM_MIN,           ///< NB.  Type _must_ match variable type. (ex. float/int)
  V_PARAM_MAX,           ///< NB.  Type _must_ match variable type. (ex. float/int)
  V_PARAM_FLAGS,         ///< eg. PF_READONLY, PF_HIDDEN 
  V_PARAM_ENUM,
  V_PARAM_ENUMSTRING,
  V_PARAM_FLAG,          
  V_PARAM_SPINNER,       ///< parameter could be inc/decemented
  V_PARAM_DIALOG,        ///< specify a custom/user dialog name
  // tokens that you should use the macros for
  _V_PARAM_ID,           ///< use V_PARAM_ID macro
  _V_PARAM_VARIABLE,     ///< use V_PARAM_VARIABLE macro
  
  V_PARAM_MAX_VAL
};

//Macros that help you use some of the tokens. The macros are used to prevent the programmer 
//from forgetting critical pointer casts and other small details.

#define V_PARAM_ID(val)                 _V_PARAM_ID, &val 
#define V_PARAM_VARIABLE(klass,val)     _V_PARAM_VARIABLE, offsetof(klass,val)


#if defined(_WIN32)  || defined(_VISION_XENON)  ///< do it always for MSDev projects (windows and Xenon)

#define V_DECLARE_PARAM_DESC(class_name,impexp)           \
public:                                                   \
  impexp static VParamDesc class_name##_fullDesc;         \
  impexp static VParamDesc class_name##_desc;             \
  impexp virtual VParamDesc *GetFullParamDesc();        \
  impexp virtual VParamDesc *GetParamDesc();            \
  impexp static VParamDesc   *GetClassFullParamDesc();    \
  impexp static VParamDesc   *GetClassParamDesc();


#elif defined(_VISION_PS3)

#define V_DECLARE_PARAM_DESC(class_name,impexp)           \
public:                                                   \
  static VParamDesc class_name##_fullDesc;                \
  static VParamDesc class_name##_desc;                    \
  virtual VParamDesc *GetFullParamDesc();               \
  virtual VParamDesc *GetParamDesc();                   \
  static VParamDesc   *GetClassFullParamDesc();           \
  static VParamDesc   *GetClassParamDesc();




#elif defined(_VISION_WIIU)

#define V_DECLARE_PARAM_DESC(class_name,impexp)           \
public:                                                   \
  static VParamDesc class_name##_fullDesc;                \
  static VParamDesc class_name##_desc;                    \
  virtual VParamDesc *GetFullParamDesc();               \
  virtual VParamDesc *GetParamDesc();                   \
  static VParamDesc   *GetClassFullParamDesc();           \
  static VParamDesc   *GetClassParamDesc();



#elif defined(_VISION_POSIX)

#define V_DECLARE_PARAM_DESC(class_name,impexp)           \
public:                                                   \
  static VParamDesc class_name##_fullDesc;                \
  static VParamDesc class_name##_desc;                    \
  virtual VParamDesc *GetFullParamDesc();               \
  virtual VParamDesc *GetParamDesc();                   \
  static VParamDesc   *GetClassFullParamDesc();           \
  static VParamDesc   *GetClassParamDesc();


#elif defined(_VISION_PSP2)

#define V_DECLARE_PARAM_DESC(class_name,impexp)           \
public:                                                   \
  static VParamDesc class_name##_fullDesc;                \
  static VParamDesc class_name##_desc;                    \
  virtual VParamDesc *GetFullParamDesc();               \
  virtual VParamDesc *GetParamDesc();                   \
  static VParamDesc   *GetClassFullParamDesc();           \
  static VParamDesc   *GetClassParamDesc();

#else
  #error Undefined platform!
#endif

class VParamDesc;

//fill the VType of this class in with the param description (normal & full) at startup

struct V_REGISTER_PARAM_DESC
{
  V_REGISTER_PARAM_DESC(VType* pType, VParamDesc *pParamDesc,VParamDesc *(*pGetFullParamDesc)())
  {
    pType->m_pParamDesc = pParamDesc;
    pType->GetFullParamDesc = pGetFullParamDesc;
  }
};



#define V_BEGIN_PARAM_DESC(class_name,parent_class)                                             \
  V_REGISTER_PARAM_DESC _regparams_##class_name(                                                \
    V_RUNTIME_CLASS(class_name),                                                                \
    &class_name::class_name##_desc,                                                             \
    class_name::GetClassFullParamDesc);                                                         \
                                                                                                \
  VParamDesc *class_name::GetParamDesc()                                                        \
  {    return class_name::GetClassParamDesc();  }                                               \
  VParamDesc *class_name::GetFullParamDesc()                                                    \
  {    return class_name::GetClassFullParamDesc();  }                                           \
                                                                                                \
  VBool class_name##_bFullDescReady = FALSE;                                                    \
  VParamDesc *class_name::GetClassParamDesc() {return &class_name##_desc;}                      \
  VParamDesc *class_name::GetClassFullParamDesc()                                               \
  {                                                                                             \
    if (!class_name##_bFullDescReady)                                                           \
    {  /* first time this is requested */                                                       \
       /* create it from our parent classes description and our description */                  \
      class_name##_fullDesc.Merge( parent_class::GetClassFullParamDesc(), &class_name##_desc ); \
      class_name##_bFullDescReady = TRUE;                                                       \
    }                                                                                           \
    return &class_name##_fullDesc;                                                              \
  }                                                                                             \
                                                                                                \
  VParamDesc class_name::class_name##_fullDesc;                                                 \
  VParamDesc class_name::class_name##_desc(                                 


#define V_END_PARAM_DESC(class_name) \
  V_PARAM_MAX_VAL);


class VParamBlock;

class VParamDesc : public VTypedObject
{
  V_DECLARE_SERIAL_DLLEXP( VParamDesc, VBASE_IMPEXP )

public:

  VBASE_IMPEXP ~VParamDesc();
  VBASE_IMPEXP VParamDesc();
  VBASE_IMPEXP VParamDesc(int first, ...);
  VBASE_IMPEXP VParamBlock *GetNewParamBlock();

  VBASE_IMPEXP void Merge(VParamDesc *pParent,VParamDesc *pNew);
  VBASE_IMPEXP VBool IsValid(VParam* pParam);

  VBASE_IMPEXP int FindParam(const char *pName);  ///<returns -1 if not found
  inline VParam *GetParam(const char *pName) 
  {
    int idx = FindParam(pName);

    if (idx == -1)
      return NULL;

    return (VParam *) m_paramList[idx];
  }
  inline VParam *GetParam(int idx) 
  {
    if ((idx < 0) || (idx >= m_paramList.GetLength() ) )
      return NULL;

    return (VParam *) m_paramList[idx];
  }
  
  VBASE_IMPEXP void Append(VParam *pParam, VParamBlock *pBlock, VBool bAsArrayParam = FALSE);
  VBASE_IMPEXP void Delete(VParam *pParam, VParamBlock *pBlock);

  VBASE_IMPEXP VParamDesc *Clone();
  VBASE_IMPEXP void FillIn(const VParamDesc &src);

  // -------------------------------------------------------------------------------
  // Method:  GetNumParam
  // Purpose: Get number of the params in the parameter list of this descriptor
  // -------------------------------------------------------------------------------

  inline int GetNumParam() const { return m_paramList.GetLength(); }
  

  //variables
  VParamList m_paramList; ///< TODO: make private!
  
  
protected:
  VBASE_IMPEXP void Serialize( VArchive& ar );
  friend class VParamContainer; ///< for loading old maps (ver<=06) //TODO:remove later
  
private:
  friend class VCompositeParam;
  friend class VParamArray;

  VBASE_IMPEXP void Reset();
  VBASE_IMPEXP void ResetParamList();
  
  VParamBlock *m_pDefaultBlock;
  VBool        m_bParamWrong;

  //helper functions
  VBASE_IMPEXP static VBool ParseValArg(VParamType eType,ParamValUnion &val,va_list &marker);
};

#endif // _VISION_DOC

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
