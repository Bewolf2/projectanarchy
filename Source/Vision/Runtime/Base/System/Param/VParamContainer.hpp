/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VParamContainer.hpp

#ifndef _VISION_VPARAMCONTAINER_HPP
#define _VISION_VPARAMCONTAINER_HPP

#include <Vision/Runtime/Base/System/VRefTarget.hpp>

#include <Vision/Runtime/Base/System/Param/VParam.hpp>
#include <Vision/Runtime/Base/System/Param/VParamBlock.hpp>
#include <Vision/Runtime/Base/System/Param/VParamDesc.hpp>
#include <Vision/Runtime/Base/System/Param/VParamContainerBase.hpp>
#include <Vision/Runtime/Base/System/IO/Serialization/VArchive.hpp>

// these classes should not be used in the engine
#ifndef _VISION_DOC

/// \brief
///   Possible values for the VSenderRecevier::OnReceive iMsg variable value
enum ePARAMETERBROADCAST_IDS
{
  PARAM_MSG_VAR_CHANGED,       ///< a parameter value is changed
  PARAM_MSG_PRE_PARAM_CHANGE,  ///< before a parameter is changed
  PARAM_MSG_POST_PARAM_CHANGE  ///< after a parameter is changed
};

struct VParamBroadcastData
{
  VParam *pParam;
  VParamBlock *pParamBlock;
  VParamContainerBase *pParamContainer;
};

#define V_PARAM_BROADCAST(parametermessage, parameter, paramblock, parametercontainer) \
  if(parametercontainer) \
  { \
    VParamBroadcastData pData = {parameter, paramblock, parametercontainer}; \
    parametercontainer->GetTypeId()->m_pModule->GetParamBroadcaster()->Send(parametermessage, &pData);\
  }  
  

//Param Update Flags
#define PUF_ON_PARAM_CHANGE   1

/// \brief Internal: Used by some of Vision's Tools. Might get deprecated at some point. Do not use.
class VParamContainer : public VParamContainerBase
{
protected:
  VBASE_IMPEXP VParamContainer();

public:
  V_DECLARE_SERIAL_DLLEXP(VParamContainer, VBASE_IMPEXP)
  V_DECLARE_PARAM_DESC(VParamContainer,VBASE_IMPEXP)

  VBASE_IMPEXP virtual ~VParamContainer();
  VBASE_IMPEXP VParamBlock *GetParamBlock();
  VBASE_IMPEXP void InitDefaultParams();

  VBASE_IMPEXP VParamContainer* Clone();
  VBASE_IMPEXP void FillIn( VParamContainer& srcParamContainer );

  /// \brief
  ///   flags specifying what type of parameter notifications this object wants to receive
  /// 
  /// \return
  ///   flags (PUF_* combinations)
  /// 
  /// \note
  ///   Previously called CanBuild (but that was a very shape specific name)
  VBASE_IMPEXP virtual int GetParamUpdateFlags() { return m_iParamUpdateFlags; }
  
  /// \brief
  ///   Sets the flags used for determining what type of parameter notification a object receives
  /// 
  /// \return
  ///   Flag state before setting new flags
  /// 
  /// \note
  ///   Previously called EnableBuild (but that was a very shape specific name)
  /// 
  /// \example
  ///   \code
  ///   int iFlags = SetParamUpdateFlags( GetParamUpdateFlags() & ~PUF_ON_PARAM_CHANGE );
  ///   .. set object parameters (object is not rebuilt)
  ///   SetParamUpdateFlags( iFlags );
  ///   OnParamChange();  // call to rebuild the object (all parameters have changed)
  ///   \endcode
  VBASE_IMPEXP virtual int SetParamUpdateFlags( int iFlags) { int old = m_iParamUpdateFlags; m_iParamUpdateFlags = iFlags; return old;}

  /// \brief
  ///   called when any parameter changes
  /// 
  /// this method gets called when any parameter of the param container derived object changes
  /// 
  /// \param pParam
  ///   [in] parameter which has changed. If this pointer is null then it is supposed that all
  ///   parameters have changed.
  VBASE_IMPEXP virtual void OnParamChange(VParam *pParam = 0);  

  /// \brief
  ///   called before any parameter changes
  /// 
  /// this method is called before the parameter of the container is to be changed. The method can
  /// modify the value to be set if needed.
  /// 
  /// \param pParam
  ///   [in] parameter to be changed
  /// 
  /// \param pNewVal
  ///   [in,out] value to be set. It can be changed by the method.
  VBASE_IMPEXP virtual void OnPreParamChange( VParam* pParam, void* pNewVal )
  {
    // the default implementation does nothing
  }

  //VTypedObject
  VBASE_IMPEXP virtual void AssertValid();

protected:
  friend class VParamBlock;

  // -------------------------------------------------------------------------------
  // Method:  ParamSerializeCallback
  // Purpose: The callback which is called by VParamContainer ::Serialize to give 
  //          derived classes a chance to handle translation for newer and older 
  //          versions. It is called before and after the parameters of an object 
  //          are serialized.
  // Params:
  // eEvent  [in]  serialization event - see. declaration of the SerializeEvent
  // ar      [in]  archive to write to/read from.
  // pBlock  [in]  if loading then it contains loaded param block, if saving
  //               then it contains parameter block about to be saved/just saved.
  // -------------------------------------------------------------------------------
  
  VBASE_IMPEXP virtual void ParamSerializeCallback( SerializeEvent eEvent, VArchive &ar, 
                                       VParamBlock *pBlock );
  
  // -------------------------------------------------------------------------------
  // Method:  CleanupAfterSerialize
  // Purpose: Static callback function registered with ar.RegisterCloseCallback().
  //          Makes sure all our old descriptors get cleaned up
  // -------------------------------------------------------------------------------

  VBASE_IMPEXP static void CleanupAfterSerialize( VArchive &ar );

 
  // -------------------------------------------------------------------------------
  // Method:  Serialize
  // Purpose: Serializes a VParamContainer class by serializing the descriptor and all the 
  //          values of the parameters.
  //  
  // When loading, the saved descriptor is loaded and only the variables that are 
  // present in the current parameter descriptor are set.
  //   
  // Because the old parameter descriptors are shared by multiple classes, we can't 
  // free them immediately. That is why we register a callback (for when the archive
  // is finished loading) and keep a list of which descriptors we need to delete. 
  // The old parameter descriptors are not needed after loading has finished since
  // we have the current parameter descriptors.
  // 
  // Each object has its own param block and they are thus not shared. That means 
  // we can delete the old param block as soon as we are done with it.
  // -------------------------------------------------------------------------------
    
  VBASE_IMPEXP virtual void Serialize( VArchive& ar );

protected:
  VParamBlock *m_pParamBlock;

public:
  
  // Functions to directly access internal parameters of VParamContainer derived classes. 

  // Wrappers around GetXXX functions for internal parameters
  inline VBool GetBool(int id) { return m_pParamBlock->GetBool(id);}

  inline int  GetInt(int id) { return m_pParamBlock->GetInt(id);}
  
  inline float  GetFloat(int id) { return m_pParamBlock->GetFloat(id);}
  
  inline void  GetFloatVector(int id,hkvVec3& val){m_pParamBlock->GetFloatVector(id,val);}
  
  inline float GetFloatVector(int id, const char* pIndex){return m_pParamBlock->GetFloatVector(id, pIndex);}
  
  inline void  GetFloatVec2(int id, float* val){m_pParamBlock->GetFloatVec2(id,val);}

  inline void  GetFloatVec4(int id, float* val){m_pParamBlock->GetFloatVec4(id,val);}
  
  inline void  GetString(int id, VString &res, const char* pIndex = NULL){m_pParamBlock->GetString(id,res,pIndex);}

  inline int  GetColor(int id) { return m_pParamBlock->GetColor(id);}
  
  inline int  GetEnum(int id) { return m_pParamBlock->GetEnum(id);}
  
  inline int  GetFlag(int id) { return m_pParamBlock->GetFlag(id);}
  
  inline int   GetFlag(int id, int iFlag) {return m_pParamBlock->GetFlag(id,iFlag);}

  inline int  GetFlag(int id,const char* szFlagName ) { return m_pParamBlock->GetFlag(id,szFlagName);}
  
  inline VParamContainer* GetPCObject(int id) { return m_pParamBlock->GetPCObject(id); }
  
  inline int GetParamFlags(int id) { return m_pParamBlock->GetParamFlags(id); }
  

  // Wrappers around SetXXX functions for internal parameters
  
  inline void SetBool(int id,VBool val){m_pParamBlock->SetBool(id,val);}

  inline void SetInt(int id,int val){m_pParamBlock->SetInt(id,val);}
  
  inline void SetFloat(int id,float val){m_pParamBlock->SetFloat(id,val);}
  
  inline void SetColor(int id,int val){m_pParamBlock->SetColor(id,val);}
  
  inline void SetFloatVector(int id,hkvVec3& val){m_pParamBlock->SetFloatVector(id,val);}

  inline void SetFloatVector(int id,float val[3]){m_pParamBlock->SetFloatVector(id, val[0], val[1], val[2]);}
  
  inline void SetFloatVector(int id,float x,float y,float z){m_pParamBlock->SetFloatVector(id,x,y,z);}
  
  inline void SetFloatVector(int id, float val, const char* pIndex){m_pParamBlock->SetFloatVector(id,val,pIndex);}
  
  inline void SetFloatVec2(int id,float x, float y){m_pParamBlock->SetFloatVec2(id,x,y);}
  inline void SetFloatVec2(int id,float val, const char* pIndex){m_pParamBlock->SetFloatVec2(id,val,pIndex);}
  
  inline void SetFloatVec4(int id,float v1,float v2, float v3, float v4){m_pParamBlock->SetFloatVec4(id,v1,v2,v3,v4);}
  inline void SetFloatVec4(int id,float val, const char* pIndex){m_pParamBlock->SetFloatVec4(id,val,pIndex);}
  
  inline VBool SetString(int id, const char *pValue, const char *pIndex = NULL){return m_pParamBlock->SetString(id,pValue, pIndex);}
  
  inline void SetEnum(int id, int val){m_pParamBlock->SetEnum(id,val);}
  
  inline void SetFlag(int id, int iFlag, VBool bState){m_pParamBlock->SetFlag(id,iFlag,bState);}
  
  inline void SetFlag(int id, const char *pName, VBool bState){m_pParamBlock->SetFlag(id,pName,bState);}
  
  inline void SetFlag(int id, int iFlag){m_pParamBlock->SetFlag(id,iFlag);}
  
  inline void SetPCObject(int id, VParamContainer* pContainer){ m_pParamBlock->SetPCObject(id, pContainer); }
  
  inline void SetParamFlags(int id, int iFlags) { m_pParamBlock->SetParamFlags(id, iFlags); }
  
  // Wrapper  around FindParam function for internal parameters
  inline int FindParam(const char *pName) {return m_pParamBlock->FindParam(pName);}
  
private:
  
  // These flags indicates whether the object wants to receive param related
  // notifications. If the PUF_ON_PARAM_CHANGE flag is not set then OnParamChange
  // is never called. More flags to be added later
  
  int m_iParamUpdateFlags;
};


DECLARE_LIST(VParamContainerList, VParamContainer, VBASE_IMPEXP )

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
