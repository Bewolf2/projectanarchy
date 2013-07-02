/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#pragma once

using namespace System;
using namespace System::Collections;
using namespace System::Collections::Specialized;
using namespace CSharpFramework;
using namespace ManagedFramework;
using namespace CSharpFramework::Math;
using namespace CSharpFramework::Shapes;
using namespace CSharpFramework::Scene;
using namespace CSharpFramework::DynamicProperties;
using namespace CSharpFramework::Serialization;

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Components/VSkeletalBoneProxy.hpp>

namespace VisionManaged
{
  public ref class SkeletalBoneDesc
  {
  public:
    SkeletalBoneDesc(String ^name, int parent)
    {
      Name = name;
      iParent = parent;
      TreeNode = nullptr;
    }

    System::Windows::Forms::TreeNode ^TreeNode; // for custom usage
    String ^Name;
    int iParent;
  };

  public ref class EngineInstanceEntity : public IEngineInstanceObject3D
  {
  public:
    /// Constructor
    /// \param bCallInitFunction
    ///    Specifies whether the InitFunction of the entity shall be called during the construction
    ///    process. If set to false, you'll have to call the InitFunction manually via the 
    ///    CallInitFunction method.
    EngineInstanceEntity( String ^sEntityClass, String ^sModelFileName, Shape3D ^shape, StringDictionary ^propertyDict, bool bCallInitFunction);
    virtual void DisposeObject() override;

    // Calls the InitFunction of the entity.
    // Only required if the InitFunction call has been disabled in the constructor
    void CallInitFunction();

    // Overridden IEngineShapeInstance functions
    virtual void SetVisible(bool bStatus) override {m_bVisible=bStatus;UpdateVisibleBitmask();}
    virtual void SetScaling(float x,float y, float z) override;

    virtual void TraceShape(Shape3D ^ownerShape, Vector3F rayStart,Vector3F rayEnd, ShapeTraceResult ^%result) override;
    virtual bool GetLocalBoundingBox(BoundingBox ^%bbox) override;
    virtual bool OnExport(SceneExportInfo ^info) override;
    virtual void OnRenderHook(ShapeBase ^owner, int iConstant) override;

    // Overriden IEngineInstanceObject3D functions
    virtual void *GetObject3D() override;

    // Variable Related Functions
    void SetEntityProperties(StringDictionary ^propertyDict);
    void EngineInstanceEntity::SetEntityProperty(DynamicProperty ^prop);
    bool SetVariable(String ^sVarName, String ^sVarData);
    PropertyFlags_e GetPropertyFlags(DynamicProperty ^pProperty);

    //more entity properties
    void UpdateVisibleBitmask() 
    {
      if (GetEntity())
        GetEntity()->SetVisibleBitmask(m_bVisible ? m_iVisibleBitmask:0);
    }

    void SetVisibleBitmask(unsigned int iMask) {m_iVisibleBitmask=iMask;UpdateVisibleBitmask();}
    void SetLightInfluenceBitmask(unsigned int iMask) {if (GetEntity()) GetEntity()->SetLightInfluenceBitMask(iMask);}
    void SetCastDynamicShadows(bool bStatus)  {if (GetEntity()) GetEntity()->SetCastShadows(bStatus);}
    void SetCastStaticShadows(bool bStatus)  {if (GetEntity()) GetEntity()->SetCastStaticShadows(bStatus);}
    void SetLightgridSamplePosition(float x, float y, float z) {if (GetEntity()) GetEntity()->SetRelativeLightGridSamplePos(hkvVec3(x,y,z));}
    void SetAmbientColor(unsigned int iColor) {if (GetEntity()) {VColorRef color; color.SetRGBA(iColor); GetEntity()->SetAmbientColor(color);}}
    bool SetModelFile(String ^sModelFile);
    void SetCustomMaterialSet(CustomMaterialSetProvider ^set);
    void SetClipDistances(float fNear, float fFar);
    void SetPrimarySortingKey(signed char iValue) {if (GetEntity()) GetEntity()->SetPrimarySortingKey(iValue);}

    //void SetLightmapGranularity(float fValue) {if (GetEntity()) GetEntity()->SetLightmapGranularity(fValue);}
    bool HasLightmappedMaterials();

    void SetCopySource(EngineInstanceEntity ^pOther); // very specific
    void SynchronizeOwnComponents(ShapeBase ^owner, ShapeComponentCollection ^components);

    // access the native object
    VisBaseEntity_cl *GetEntity();
    void GetStandardValues(StringCollection ^pList, String ^key);

    void GetBones(Generic::List<SkeletalBoneDesc ^> ^pList);
    static void GetBones(String ^modelFile, Generic::List<SkeletalBoneDesc ^> ^pList);
    static void GetBones(VDynamicMesh *pModel, Generic::List<SkeletalBoneDesc ^> ^pList);

    bool HasBones();

    // linking
    virtual void GetNativeLinks(Shape3D ^pOwner, LinkCollection ^pSrcCollection, LinkCollection ^pTgtCollection, LinkCollection ^pBiDirCollection) override;
    bool CanLink(int iIndex, IEngineInstanceObject3D ^pObj3D, String ^otherName);
    void OnLink(int iIndex, IEngineInstanceObject3D ^pObj3D, String ^otherName, bool bStatus);

  private:

    VShapeLinkConfig *m_pCustomLinkInfo;
    VWeakPtr<VisBaseEntity_cl> *m_pEntityWP;
    unsigned int m_iVisibleBitmask;
    bool m_bVisible;
  };


  public ref class EngineInstanceCubemapEntity : public EngineInstanceEntity
  {
  public:
    enum class CubeMapRenderingType_e {
      Scene,
      Specular,
      RendererNode
    };

    EngineInstanceCubemapEntity(Shape3D ^shape);
    virtual void DisposeObject() override;

    // overridden entity functions
    virtual bool OnExport(SceneExportInfo ^info) override;

    void SetBlurPasses(int iNumPasses);
    void SetAutoGenMipMaps(bool bStatus);
    void SetCubemapKey(String ^key, int iEdgeSize);
    void SetPreviewVisible(bool bStatus);
    void SetRendererConfig(String^ rendererConfig);
    void UpdateCubemap();

    // special functions:
    void SetRenderFilterMask(unsigned int iMask);
    void SetClipPlanes(float fNear, float fFar);
    void SetUpdateParams(bool bContinuous, float fInterval, int iCount, bool bAlternate);
    bool SaveToFile(String ^filename);
    void SetExportAsEntity(bool bStatus) {m_bExport=bStatus;}
    void AssignCubemapShader(const char *szCubemapKey);
    void SetRenderingType(CubeMapRenderingType_e eRenderingType);
    void SetSpecularPower(float fSpecularPower);
    bool m_bExport;
  };

  public ref class EngineInstanceBoneProxy : public IEngineInstanceObject3D
  {
  public:
    EngineInstanceBoneProxy();
    virtual void DisposeObject() override;
    virtual void *GetObject3D() override  {return m_pBoneProxy;}
    virtual void SetVisible(bool bStatus) override  {}
    virtual void SetScaling(float x,float y, float z) override  {}
    virtual void TraceShape(Shape3D ^ownerShape, Vector3F rayStart,Vector3F rayEnd, ShapeTraceResult ^%result) override  {}
    virtual bool GetLocalBoundingBox(BoundingBox ^%bbox) override;
    virtual bool OnExport(SceneExportInfo ^info) override;
    virtual bool GetPosition(Vector3F %enginePosition) override;
    virtual bool GetOrientation(Vector3F %engineOrientation) override;
    virtual void SetObjectKey(String ^key) override;



    VisBaseEntity_cl *GetParentEntity();
    void RenderShape(VisionViewBase ^view, ShapeRenderMode mode);
    void AttachToEntity(EngineInstanceEntity ^pParent, String ^boneName);

    VSkeletalBoneProxyObject *m_pBoneProxy;
  };

}

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
