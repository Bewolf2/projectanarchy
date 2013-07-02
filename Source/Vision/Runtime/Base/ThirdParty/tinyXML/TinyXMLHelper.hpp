/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef TINYXMLHELPER_HPP_INCLUDED
#define TINYXMLHELPER_HPP_INCLUDED

#include <Vision/Runtime/Base/ThirdParty/tinyXML/tinyxml.h>

// Helper functions written by Trinigy based on TinyXml
namespace XMLHelper
{
  // string <-> value conversion
  VBASE_IMPEXP const char *IntsToString(const int *pArray, int iCount, char *szBuffer=NULL);
  VBASE_IMPEXP int StringToInts(const char *szBuffer,int *pDestArray,int iMaxCount,int iSeparator=',');
  VBASE_IMPEXP const char *UIntsToString(const unsigned int *pArray, int iCount, char *szBuffer=NULL);
  VBASE_IMPEXP int StringToUInts(const char *szBuffer,unsigned int *pDestArray,int iMaxCount,int iSeparator=',');

  VBASE_IMPEXP const char *FloatsToString(const float *pArray, int iCount, char *szBuffer=NULL);
  VBASE_IMPEXP int StringToFloats(const char *szBuffer,float *pDestArray,int iMaxCount,int iSeparator=',');

  VBASE_IMPEXP const char *ColorToString(VColorRef iColor, int iComponentCount=4,char *szBuffer=NULL);
  VBASE_IMPEXP int StringToColor(const char *szBuffer,VColorRef &dstColor,int iMaxCount=4);

  // serializing
  VBASE_IMPEXP TiXmlElement* SubNode(TiXmlElement* pParent,const char *szSubNodeType, bool bWrite);

  /// \brief
  ///  Returns a sub node of type szSubNodeType which has the 'name' attribute set to szNameAttributeValue.
  VBASE_IMPEXP TiXmlElement* NamedSubNode(TiXmlElement* pParent, const char* szSubNodeType, const char* szNameAttributeValue, bool bCreate=false);

  VBASE_IMPEXP void Exchange_Int(TiXmlElement *pParent,const char *szAttribName, int &iVar, bool bWrite);
  VBASE_IMPEXP int Exchange_Ints(TiXmlElement *pParent,const char *szAttribName, int *pArray, int iMaxCount, bool bWrite);
  VBASE_IMPEXP void Exchange_UInt(TiXmlElement *pParent,const char *szAttribName, unsigned int &iVar, bool bWrite);
  VBASE_IMPEXP int Exchange_UInts(TiXmlElement *pParent,const char *szAttribName, unsigned int *pArray, int iMaxCount, bool bWrite);
  VBASE_IMPEXP void Exchange_Bool(TiXmlElement *pParent,const char *szAttribName, bool &bVar, bool bWrite);
  VBASE_IMPEXP void Exchange_Float(TiXmlElement *pParent,const char *szAttribName, float &fVar, bool bWrite);
  VBASE_IMPEXP int Exchange_Floats(TiXmlElement *pParent,const char *szAttribName, float *pArray, int iMaxCount, bool bWrite);
  VBASE_IMPEXP void Exchange_Color(TiXmlElement *pParent,const char *szAttribName, VColorRef &iColor, bool bWrite);
  VBASE_IMPEXP void Exchange_BBox(TiXmlElement *pParent,const char *szAttribName, float pMin[3], float pMax[3], bool bWrite);
  VBASE_IMPEXP const char* Exchange_String(TiXmlElement *pParent,const char *szAttribName, const char *szString, bool bWrite);
  VBASE_IMPEXP const char* Exchange_VString(TiXmlElement *pParent,const char *szAttribName, VString &sString, bool bWrite);
  VBASE_IMPEXP bool Exchange_Transparency(TiXmlElement *pParent,const char *szAttribName, VIS_TransparencyType &iVar, bool bWrite);
  VBASE_IMPEXP bool Exchange_LightingMethod(TiXmlElement *pParent, const char *szAttribName, VisLightingMethod_e &mode, bool bWrite);
  VBASE_IMPEXP bool Exchange_PassType(TiXmlElement *pParent, const char *szAttribName, VPassType_e &pass, bool bWrite);
  VBASE_IMPEXP bool Exchange_Enum(TiXmlElement *pParent,const char *szAttribName, int &eVar, int iEnumCount, const char* pszEnumNames[], const int pEnumValues[], bool bWrite);
  VBASE_IMPEXP bool Exchange_Bitmask(TiXmlElement *pParent,const char *szAttribName, int &eVar, int iEnumCount, const char* pszEnumNames[], const int pEnumValues[], bool bWrite);
}


class VModelInfoXMLDocument : public TiXmlDocument
{
public:
  inline VModelInfoXMLDocument()
  {
    m_pCachedMaterialsNode = NULL;
    m_iReplaceTextureRefs = -1;
  }
  VBASE_IMPEXP TiXmlElement* GetRootNode(bool bCreate=false);
  VBASE_IMPEXP TiXmlElement* GetMaterialsNode(bool bCreate=false);
                            
  VBASE_IMPEXP TiXmlElement* GetMaterial(TiXmlElement* pMaterials, const char* szMaterialName);
  VBASE_IMPEXP TiXmlElement* CreateMaterial(TiXmlElement* pMaterials, const char* szMaterialName);
  VBASE_IMPEXP static TiXmlElement* GetSubNode(TiXmlElement* pNode, const char* szSubNode, bool bCreate=false);
 
  VBASE_IMPEXP bool GetReplaceTextureRefs();
  inline void SetReplaceTextureRefs(bool bState)
  {
    XMLHelper::Exchange_Bool(GetMaterialsNode(),"override",bState,true);
    m_iReplaceTextureRefs = bState ? 1:0;
  }

  //Getting
  VBASE_IMPEXP static const char *GetMaterialAttributeString(TiXmlElement *pMaterialNode, const char *szAttributeName, const char *szAlternativeName);

  inline static const char *GetDiffuseTexture(TiXmlElement *pMaterialNode, const char *szAlternativeName)   {return GetMaterialAttributeString(pMaterialNode,"diffuse",szAlternativeName);}
  inline static const char *GetNormalmapTexture(TiXmlElement *pMaterialNode, const char *szAlternativeName) {return GetMaterialAttributeString(pMaterialNode,"normalmap",szAlternativeName);}
  inline static const char *GetSpecularTexture(TiXmlElement *pMaterialNode, const char *szAlternativeName) {return GetMaterialAttributeString(pMaterialNode,"specularmap",szAlternativeName);}
  VBASE_IMPEXP static int GetAuxiliaryTextures(TiXmlElement *pMaterialNode, const char **szAlternativeNames);
  inline static VIS_TransparencyType GetTransparency(TiXmlElement *pMaterialNode, VIS_TransparencyType alternative) {XMLHelper::Exchange_Transparency(pMaterialNode,"transparency",alternative,false);return alternative;}
  inline static VisLightingMethod_e GetLightingMethod(TiXmlElement *pMaterialNode, VisLightingMethod_e alternative) {XMLHelper::Exchange_LightingMethod(pMaterialNode,"Lighting",alternative,false);return alternative;}
  inline static VPassType_e GetPassType(TiXmlElement *pMaterialNode, VPassType_e alternative) {XMLHelper::Exchange_PassType(pMaterialNode,"pass",alternative,false);return alternative;}
  inline static const char *GetUserString(TiXmlElement *pMaterialNode, const char *szAlternativeName) {return GetMaterialAttributeString(pMaterialNode,"userstring",szAlternativeName);}
  inline static int GetUserFlags(TiXmlElement *pMaterialNode, int iAlternative) {XMLHelper::Exchange_Int(pMaterialNode, "userflags", iAlternative, false); return iAlternative;}
  inline static bool GetDoublesidednessFlag(TiXmlElement *pMaterialNode, bool bAlternative) {XMLHelper::Exchange_Bool(pMaterialNode,"doublesided",bAlternative,false);return bAlternative;}
  inline static bool GetDepthWriteFlag(TiXmlElement *pMaterialNode, bool bAlternative) {XMLHelper::Exchange_Bool(pMaterialNode,"depthwrite",bAlternative,false);return bAlternative;}
  inline static void GetDepthBiasParams(TiXmlElement *pMaterialNode, float &fBias, float &fBiasClamp, float &fSlopeScale)
  {
    float fValues[3] = {fBias, fBiasClamp, fSlopeScale};
    XMLHelper::Exchange_Floats(pMaterialNode,"zbias",fValues,3,false);
    fBias=fValues[0]; fBiasClamp=fValues[1]; fSlopeScale=fValues[2];
  }

  inline static float GetSpecularMultiplier(TiXmlElement *pMaterialNode, float fDefault)  {XMLHelper::Exchange_Float(pMaterialNode,"specmul",fDefault,false);return fDefault;}
  inline static float GetSpecularExponent(TiXmlElement *pMaterialNode, float fDefault)    {XMLHelper::Exchange_Float(pMaterialNode,"specexp",fDefault,false);return fDefault;}
  inline static unsigned char GetSortingKey(TiXmlElement *pMaterialNode, unsigned char cDefault) 
  {
    unsigned int iDefault = (unsigned int)cDefault;
    XMLHelper::Exchange_UInt(pMaterialNode,"sortingkey", iDefault, false);
    return (unsigned char)iDefault;
  }

  inline static float GetParallaxScale(TiXmlElement *pMaterialNode, float fDefault)   {XMLHelper::Exchange_Float(pMaterialNode,"parallaxscale",fDefault,false);return fDefault;}
  inline static float GetParallaxBias(TiXmlElement *pMaterialNode, float fDefault)    {XMLHelper::Exchange_Float(pMaterialNode,"parallaxbias",fDefault,false);return fDefault;}
  inline static VColorRef GetAmbientColor(TiXmlElement *pMaterialNode, VColorRef iDefault)    {XMLHelper::Exchange_Color(pMaterialNode,"ambient",iDefault,false);return iDefault;}
  inline static float GetAlphaTestThreshold(TiXmlElement *pMaterialNode, float fDefault)    {XMLHelper::Exchange_Float(pMaterialNode,"alphathreshold",fDefault,false);return fDefault;}
  inline static void GetLightmapPageSize(TiXmlElement *pMaterialNode, short &iSizeX, short &iSizeY, float &fGranularity)
  {
    int iValues[2] = {iSizeX,iSizeY}; 
    XMLHelper::Exchange_Ints(pMaterialNode,"lightmapsize",iValues,2,false);
    XMLHelper::Exchange_Float(pMaterialNode,"lightmapgran",fGranularity,false);
    iSizeX=iValues[0];iSizeY=iValues[1];
  }
  inline static int GetLightmapPageID(TiXmlElement *pMaterialNode, int iDefaultID)    {XMLHelper::Exchange_Int(pMaterialNode,"lightmapID",iDefaultID,false);return iDefaultID;}
  inline static bool GetCastStaticShadows(TiXmlElement *pMaterialNode, bool bAlternative) {XMLHelper::Exchange_Bool(pMaterialNode,"staticshadows",bAlternative,false);return bAlternative;}
  inline static UBYTE GetDeferredID(TiXmlElement *pMaterialNode, UBYTE iDefaultID)    {int iID = iDefaultID;XMLHelper::Exchange_Int(pMaterialNode,"deferredID",iID,false);return iID;}
#ifdef SUPPORTS_PHYSICS_MATERIALS
  // physics
  inline static void GetPhysicsBaseProperties(TiXmlElement *pMaterialNode,unsigned int &iCollisionBitMask, float &fStaticFriction, float &fDynamicFriction, float &fRestitution)
  {
    float fFriction[3]= {fStaticFriction, fDynamicFriction, fRestitution};
    XMLHelper::Exchange_UInt(pMaterialNode,"collisionbits",iCollisionBitMask,false);
    XMLHelper::Exchange_Floats(pMaterialNode,"friction",fFriction,3,false);
    fStaticFriction=fFriction[0];fDynamicFriction=fFriction[1];fRestitution=fFriction[2];
  }
#endif

  //Setting
  VBASE_IMPEXP static void SetMaterialAttributeString(TiXmlElement *pMaterialNode, const char *szAttributeName, const char *szAlternativeName, bool bRemoveIfEmpty=false);

  inline static void SetDiffuseTexture(TiXmlElement *pMaterialNode, const char *szAlternativeName)       {SetMaterialAttributeString(pMaterialNode,"diffuse",szAlternativeName,true);}
  inline static void SetNormalmapTexture(TiXmlElement *pMaterialNode, const char *szAlternativeName)     {SetMaterialAttributeString(pMaterialNode,"normalmap",szAlternativeName,true);}
  inline static void SetSpecularTexture(TiXmlElement *pMaterialNode, const char *szAlternativeName) {SetMaterialAttributeString(pMaterialNode,"specularmap",szAlternativeName,true);}
  VBASE_IMPEXP static void SetAuxiliaryTextures(TiXmlElement *pMaterialNode, int iCount, const char **szTextureNames);
  inline static void SetTransparency(TiXmlElement *pMaterialNode, VIS_TransparencyType alternative) {XMLHelper::Exchange_Transparency(pMaterialNode,"transparency",alternative,true);}
  inline static void SetLightingMethod(TiXmlElement *pMaterialNode, VisLightingMethod_e mode) {XMLHelper::Exchange_LightingMethod(pMaterialNode,"Lighting",mode,true);}
  inline static void SetPassType(TiXmlElement *pMaterialNode, VPassType_e mode) {XMLHelper::Exchange_PassType(pMaterialNode,"pass",mode,true);}
  inline static void SetUserString(TiXmlElement *pMaterialNode, const char *szAlternativeName)          {SetMaterialAttributeString(pMaterialNode,"userstring",szAlternativeName,true);}
  inline static void SetUserFlags(TiXmlElement *pMaterialNode, int iFlags) {XMLHelper::Exchange_Int(pMaterialNode, "userflags", iFlags, true);}
  inline static void SetDoublesidednessFlag(TiXmlElement *pMaterialNode, bool bAlternative) {XMLHelper::Exchange_Bool(pMaterialNode,"doublesided",bAlternative,true);}
  inline static void SetDepthWriteFlag(TiXmlElement *pMaterialNode, bool bAlternative) {XMLHelper::Exchange_Bool(pMaterialNode,"depthwrite",bAlternative,true);}
  inline static void SetDepthBiasParams(TiXmlElement *pMaterialNode, float fBias, float fBiasClamp, float fSlopeScale)
  {
    float fValues[3] = {fBias, fBiasClamp, fSlopeScale};
    XMLHelper::Exchange_Floats(pMaterialNode,"zbias",fValues,3,true);
  }
  
  inline static void SetSpecularMultiplier(TiXmlElement *pMaterialNode, float fDefault)  {XMLHelper::Exchange_Float(pMaterialNode,"specmul",fDefault,true);}
  inline static void SetSpecularExponent(TiXmlElement *pMaterialNode, float fDefault)    {XMLHelper::Exchange_Float(pMaterialNode,"specexp",fDefault,true);}

  inline static void SetParallaxScale(TiXmlElement *pMaterialNode, float fDefault)   {XMLHelper::Exchange_Float(pMaterialNode,"parallaxscale",fDefault,true);}
  inline static void SetParallaxBias(TiXmlElement *pMaterialNode, float fDefault)    {XMLHelper::Exchange_Float(pMaterialNode,"parallaxbias",fDefault,true);}
  inline static void SetAmbientColor(TiXmlElement *pMaterialNode, VColorRef iDefault)    {XMLHelper::Exchange_Color(pMaterialNode,"ambient",iDefault,true);}
  inline static void SetSortingKey(TiXmlElement *pMaterialNode, unsigned int iValue) {XMLHelper::Exchange_UInt(pMaterialNode,"sortingkey", iValue, true);}
  inline static void SetAlphaTestThreshold(TiXmlElement *pMaterialNode, float fDefault)    {XMLHelper::Exchange_Float(pMaterialNode,"alphathreshold",fDefault,true);}
  inline static void SetLightmapPageSize(TiXmlElement *pMaterialNode, short iSizeX, short iSizeY, float fGranularity)    
  {
    int iValues[2] = {iSizeX,iSizeY}; 
    XMLHelper::Exchange_Ints(pMaterialNode,"lightmapsize",iValues,2,true);
    XMLHelper::Exchange_Float(pMaterialNode,"lightmapgran",fGranularity,true);
  }
  inline static void SetLightmapPageID(TiXmlElement *pMaterialNode, int iID)    {XMLHelper::Exchange_Int(pMaterialNode,"lightmapID",iID,true);}
  inline static void SetCastStaticShadows(TiXmlElement *pMaterialNode, bool bValue) 
  {
    if (!bValue) // only if non-standard:
      XMLHelper::Exchange_Bool(pMaterialNode,"staticshadows",bValue,true);
    else if (pMaterialNode)
      pMaterialNode->RemoveAttribute("staticshadows");
  }
  inline static void SetDeferredID(TiXmlElement *pMaterialNode, UBYTE iID)    
  {
    int iVal = iID;
    if (iVal!=0) // only if non-standard:
      XMLHelper::Exchange_Int(pMaterialNode,"deferredID",iVal,true);
    else if (pMaterialNode)
      pMaterialNode->RemoveAttribute("deferredID");

  }

  inline static void SetMaterialOverride (TiXmlElement *pMaterialNode, const char* szLibrary, const char* szMaterial)  {SetMaterialAttributeString (pMaterialNode, "overridemateriallib", szLibrary, true); SetMaterialAttributeString (pMaterialNode, "overridematerialname", szMaterial, true);}
#pragma warning(push)
#pragma warning(disable:4996)
  inline static void GetMaterialOverride (TiXmlElement *pMaterialNode, char* out_szLibrary, char* out_szMaterial)      {strcpy (out_szLibrary, GetMaterialAttributeString (pMaterialNode, "overridemateriallib", "")); strcpy (out_szMaterial, GetMaterialAttributeString (pMaterialNode, "overridematerialname", ""));}
#pragma warning(pop)

#ifdef SUPPORTS_PHYSICS_MATERIALS
  // physics
  inline static void SetPhysicsBaseProperties(TiXmlElement *pMaterialNode,unsigned int iCollisionBitMask, float fStaticFriction, float fDynamicFriction, float fRestitution)
  {
    float fFriction[3]= {fStaticFriction, fDynamicFriction, fRestitution};
    XMLHelper::Exchange_UInt(pMaterialNode,"collisionbits",iCollisionBitMask,true);
    XMLHelper::Exchange_Floats(pMaterialNode,"friction",fFriction,3,true);
  }
#endif
private:
  TiXmlElement *LookupMaterialsNode(bool bCreate);
  TiXmlElement *m_pCachedMaterialsNode;
  short m_iReplaceTextureRefs; // -1, 0 or 1
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
