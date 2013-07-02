/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VParamBlock.hpp

#ifndef _VISION_VPARAMBLOCK_HPP
#define _VISION_VPARAMBLOCK_HPP

#include <Vision/Runtime/Base/System/Param/VParamDesc.hpp>

// these classes should not be used in the engine
#ifndef _VISION_DOC

class VString;
class VCompositeParam;

//Parameter Block flags - These flags are for variables inside a parameter block
#define PBF_CONTROLLER  (1 << 0)
#define PBF_READONLY    (1 << 1) ///< Parameter can be seen but not edited in property editor or setvar action (but the param can be changed with the 
#define PBF_HIDDEN      (1 << 2) ///< Parameter is internal and can not be seen or edited

#define PBF_ALL_FLAGS (PBF_CONTROLLER | PBF_READONLY | PBF_HIDDEN)

class VParam;
class VParamDesc;
class VParamContainer;
class VParamContainerBase;
class VParamArray;

// helper structure to store the results of FindParamsByFlags()
struct VFindParamInfo
{
  VParamBlock *pParamBlock;
  int id;
};

/// \brief Internal: Used by some of Vision's Tools. Might get deprecated at some point. Do not use.
class VBASE_IMPEXP_CLASS VParamBlock
{
  V_DECLARE_SERIALX( VParamBlock, VBASE_IMPEXP )
  
public:
  VParamBlock();
  ~VParamBlock();
  
  void LoadDefaultValues();
  void SetDefaultValue(int id);
  
  int FindParam(const char *pName);   ///<returns -1 if not found
  VParam *GetParam(int idx);          ///<returns the param


  /// \brief
  ///   return parameter from path ( for nested parameters like composite and  pc_object parameters
  ///   )
  /// 
  /// \param szPath
  ///   path contain composite or pc_object paramters
  /// 
  /// \return
  ///   VParam*: founded Parameter or NULL
  /// 
  /// \note
  ///   this function is needed to find parameters in the propertyTree callBack  functions. Work
  ///   also for unnexted parameters ( path = "position" )
  /// 
  /// \example
  ///   \code
  ///   path = "compParam1/copmParam2/position
  ///   where copmParam1 and compParam2 are composite or pc_object parameters and 
  ///   position is a parameter for copmParam2
  ///   \endcode
  VParam* GetNestedParam(const char* szPath);

  /// \brief
  ///   Get the nested pc_object parameter
  /// 
  /// \param szPath
  ///   path contain pc_object paramters
  /// 
  /// \return
  ///   VParamContainer*: founded pc_object parameter or NULL
  VParamContainer* GetNestedContainer(const char* szPath);

  // -------------------------------------------------------------------------------
  // Method:  GetNumParam
  // Purpose: Get the number of the parameters in this param block
  // -------------------------------------------------------------------------------

  int GetNumParam() { return m_pDesc->GetNumParam(); }
  
  // -------------------------------------------------------------------------------
  // Method:  GetOwner
  // Purpose: Get the parameter container which owns this parameter block
  // -------------------------------------------------------------------------------
  
  VParamContainerBase* GetOwner() { return m_pOwner; }

  // -------------------------------------------------------------------------------
  // Method:  CopyValues
  // Purpose: Copy values of all parameters from the source param block which are 
  //          also in this param block to this param block.
  // -------------------------------------------------------------------------------
  
  void CopyValues( VParamBlock* pSrc );

  void Grow(int size, int iParamCount);
  void Shrink(int size, int iParamCount);

//Set
  void SetBool(int id,VBool val);
  void SetInt(int id,int val);
  void SetFloat(int id,float val);
  void SetColor(int id,int val);
  void SetFloatVector(int id,hkvVec3& val);
  void SetFloatVector(int id,float val[3]);
  void SetFloatVector(int id,float x,float y,float z);
  void SetFloatVector(int id, float val, const char* pIndex);
  void SetFloatVec2(int id, float x, float y);
  void SetFloatVec2(int id, float val, const char* pIndex);
  void SetFloatVec4(int id, float v1, float v2, float v3, float v4);
  void SetFloatVec4(int id, float val, const char* pIndex);
  VBool SetString(int id, const char *pValue, const char *pIndex = NULL);
  void SetEnum(int id, int val);
  void SetFlag(int id, int iFlag, VBool bState); ///<toggles flags on/off
  void SetFlag(int id, const char *pName, VBool bState); ///<toggles flags on/off
  void SetFlag(int id, int iFlag); ///<replaces flags
  VBool SetLock(int id, VBool val); ///<set the parameter's lock state
  void SetPCObject(int id, VParamContainer* pContainer);

  VBool SetString( const char *pRootIndex, const char *szValue);

//Get
  VBool GetBool(int id);
  int   GetInt(int id);
  float GetFloat(int id);
  int   GetColor(int id);
  void  GetFloatVector(int id,hkvVec3& val);
  float GetFloatVector(int id, const char* pIndex);
  void  GetFloatVec2(int id, float *val);
  void  GetFloatVec4(int id, float *val);
  void  GetString(int id, VString &res, const char* pIndex = NULL);
  int   GetEnum(int id);
  int   GetFlag(int id); ///<get all the flags
  int   GetFlag(int id, int iFlag); ///<get a specific single flag or a combination of flags  
  int   GetFlag(int id, const char* szFlagName);  ///<get a specific single flag
  VBool GetLock(int id); ///<find out whether the parameter is locked
  VParamContainer* GetPCObject(int id);
    
  void  GetString(const char* pRootIndex, VString &res);
  
  // ---------------------------------------------------------------------------------
  // Method:  FindParamsByFlags
  // Purpose: Finds all params that match the given flags
  // ---------------------------------------------------------------------------------
  VBool FindParamsByFlags(int iFlags, VPList &paramResultList);
    
    
  VCompositeParam *GetCompositeParam(int id);
  // ---------------------------------------------------------------------------------
  // Method:  AssignComposite
  // Purpose: Removes existing Composite parameter, and assigns a new Composite parameter 
  // ---------------------------------------------------------------------------------
  void AssignComposite(int id, VCompositeParam *pComp);

  // ---------------------------------------------------------------------------------
  // Method:  AssignArrayParam
  // Purpose: Removes existing Array parameter, and assigns a new Array parameter 
  // ---------------------------------------------------------------------------------
  void AssignArrayParam(int id, VParamArray *pParamArray);
  VParamArray *GetArrayParam(int id);

  void SetParamFlags(int id, int iFlags/*, bool bAllowTreeRefresh = true*/);
  int GetParamFlags(int id);

  void Reset();
  void AssertValid();

  // [M_TEST] VARIABLE SUPPORT
  void SetOwner(VParamContainerBase *pCont);
  void *GetParamPtr(VParamContainerBase *pOwner, VParam *pParam);
  void *GetParamPtr(VParam *pParam) { return GetParamPtr(m_pOwner, pParam); }
  // [M_TEST] VARIABLE SUPPORT - END
  

//variables
  int                 m_iSize;
  void                *m_pData;
  VParamDesc          *m_pDesc;

protected:
  friend class VParamDesc;

  int                 *m_pFlags; ///< parameters flags
  int                 m_iParamCount;

  VParamContainerBase *m_pOwner;

public:


//helpers
  static VParamBlock *BuildFrom(VParamDesc *pDesc);

  VParamBlock *Clone();
  void FillIn(VParamBlock &src);

  VBool GetValueFromExpression(VParam *pParam, int id, const char *pValue, const char *pIndex, VString &res);


protected:
  // ---------------------------------------------------------------------------------
  // Method:  SerializeX
  // Purpose: Serialize parameter block to/from the archive
  // ---------------------------------------------------------------------------------
  void SerializeX( VArchive& ar );
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
