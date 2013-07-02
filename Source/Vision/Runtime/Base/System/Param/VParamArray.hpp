/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VParamArray.hpp

#ifndef _VISION_VPARAMARRAY_HPP
#define _VISION_VPARAMARRAY_HPP

#include <Vision/Runtime/Base/System/Param/VParamBlock.hpp>
#include <Vision/Runtime/Base/System/Param/VParamDesc.hpp>
#include <Vision/Runtime/Base/System/SenderReceiver/VSenderReceiver.hpp>

// this class is not relevant for the engine
#ifndef _VISION_DOC

/// \brief
///   Class encapsulating a parameter array of same type. Used by some of Vision's Tools. Might get deprecated at some point. Do not use.
/// 
/// This parameter array has internally only one parameter and an array/list of values.
/// 
/// The array (or the list) of values is not owned by this class. This class hold only the pointer
/// to. After setting the values with SetArray()/SetList() you can modify  them on the original
/// data.
/// 
/// The values array/list must live at least so long the ParamArray lives.
/// 
/// Only one "parameter" (value) can be accessed at one time.
/// 
/// The parameters can not be accessed by name.
/// 
/// \return
///   class VCORE_IMPEXP_CLASS: 
/// 
/// \note
///   This ParamArray don't serialize anything
/// 
/// \note
///   You can not set the lock for individual parameters. All parameters share the same lock
///   status!
/// 
/// \note
///   Currently you can't make array of arrays
/// 
/// \note
///   Currently you can't make array of composite params 
class VParamArray : public VSenderReceiver
{
  V_DECLARE_SERIAL_DLLEXP(VParamArray, VBASE_IMPEXP)
  
public:
  VBASE_IMPEXP VParamArray();
  VBASE_IMPEXP virtual ~VParamArray();

  VBASE_IMPEXP void SetArray(int iCount, void *pData, VParamType eType);
  VBASE_IMPEXP void SetList(VPList *pList, VParamType eType);
  VBASE_IMPEXP void Reset();
  VBASE_IMPEXP VBool SetIndex(int iIndex);
  VBASE_IMPEXP int GetIndex();
  inline VParamType GetArrayType() { return m_eType; }
  inline int GetItemCount() { return m_iItemCount; }
  inline VParam *GetParameter() { return m_iItemCount ? m_pDesc->m_paramList[0] : NULL; }

  VBASE_IMPEXP VBool SetLock(VBool val); ///< Sets the parameter's lock state.
  VBASE_IMPEXP VBool GetLock(); ///< Finds out whether the parameter is locked.

  // wrapper functions
  VBASE_IMPEXP void SetBool(int id,VBool val);
  VBASE_IMPEXP void SetInt(int id,int val);
  VBASE_IMPEXP void SetFloat(int id,float val);
  VBASE_IMPEXP void SetColor(int id,int val);
  VBASE_IMPEXP void SetFloatVector(int id,hkvVec3& val);
  VBASE_IMPEXP void SetFloatVector(int id,float val[3]);
  VBASE_IMPEXP void SetFloatVector(int id,float x,float y,float z);
  VBASE_IMPEXP void SetFloatVector(int id, float val, const char* pIndex);
  VBASE_IMPEXP void SetFloatVec2(int id, float x, float y);
  VBASE_IMPEXP void SetFloatVec2(int id, float val, const char* pIndex);
  VBASE_IMPEXP void SetFloatVec4(int id, float v1, float v2, float v3, float v4);
  VBASE_IMPEXP void SetFloatVec4(int id, float val, const char* pIndex);
  VBASE_IMPEXP VBool SetString(int id, const char *pValue, const char *pIndex = NULL);
  VBASE_IMPEXP void SetEnum(int id, int val);
  VBASE_IMPEXP void SetFlag(int id, int iFlag, VBool bState); ///< Toggles flags on/off.
  VBASE_IMPEXP void SetFlag(int id, const char *pName, VBool bState); ///< Toggles flags on/off.
  VBASE_IMPEXP void SetFlag(int id, int iFlag); ///< Replaces flags.
  
  VBASE_IMPEXP VBool GetBool(int id);
  VBASE_IMPEXP int   GetInt(int id);
  VBASE_IMPEXP float GetFloat(int id);
  VBASE_IMPEXP int   GetColor(int id);
  VBASE_IMPEXP void  GetFloatVector(int id,hkvVec3& val);
  VBASE_IMPEXP float GetFloatVector(int id, const char* pIndex);
  VBASE_IMPEXP void  GetFloatVec2(int id, float *val);
  VBASE_IMPEXP void  GetFloatVec4(int id, float *val);
  VBASE_IMPEXP void  GetString(int id, VString &res, const char* pIndex = NULL);
  VBASE_IMPEXP int   GetEnum(int id);
  VBASE_IMPEXP int   GetFlag(int id); ///< Gets all the flags.
  VBASE_IMPEXP int   GetFlag(int id, int iFlag); ///< Gets a specific single flag or a combination of flags.
  VBASE_IMPEXP int   GetFlag(int id, const char* szFlagName);  ///< Gets a specific single flag.
  
  VBASE_IMPEXP VParamContainer* GetPCObject(int id);
  
  
  VBASE_IMPEXP VParamArray *Clone();
  VBASE_IMPEXP void FillIn(VParamArray &src);
  
  VParamBlock *m_pBlock;
  VBASE_IMPEXP virtual void AssertValid();
  
protected:
  
  VParamDesc  *m_pDesc;

  int m_iItemSize;

  char *m_pArray;
  int m_iItemCount;
  VPList *m_pList;

  VParamType m_eType;
  int m_iIndex;


  VBASE_IMPEXP virtual void Serialize(VArchive &ar);
  VBASE_IMPEXP void CreateTheParameter(VParamType eType);
  
};

#endif // _VISION_DOC

#endif // _VISION_VPARAMARRAY_HPP

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
