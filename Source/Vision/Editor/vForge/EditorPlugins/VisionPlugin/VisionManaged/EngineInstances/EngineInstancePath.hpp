/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#pragma once

using namespace System;
using namespace CSharpFramework;
using namespace CSharpFramework::Math;
using namespace CSharpFramework::Shapes;
using namespace ManagedFramework;
using namespace CSharpFramework::Scene;

#include <Vision/Runtime/Engine/SceneElements/VisApiPath.hpp>
#include <Vision/Runtime/Engine/SceneElements/VisApiPathNode.hpp>


namespace VisionManaged
{
  /*public */ref class EngineInstancePathNode; // forward

  public ref class EngineInstancePath : public IEngineInstanceObject3D
  {
  public:
    enum class PathLineDisplayMode_e
    {
      Disabled = 0,
      Always = 1,
      WhenSelected = 2
    };

    EngineInstancePath();
    VOVERRIDE void DisposeObject() override;

    // overridden IEngineShapeInstance functions
    VOVERRIDE void SetVisible(bool bStatus) override  {}
    VOVERRIDE void SetScaling(float x,float y, float z) override  {}

    // Overriden IEngineInstanceObject3D functions
    VOVERRIDE void *GetObject3D() override  {return m_pPath;}

    VOVERRIDE void TraceShape(Shape3D ^ownerShape, Vector3F rayStart,Vector3F rayEnd, ShapeTraceResult ^%result) override;
    VOVERRIDE bool GetLocalBoundingBox(BoundingBox ^%bbox) override;
    VOVERRIDE bool OnExport(SceneExportInfo ^info) override;
    VOVERRIDE void OnBeforeExport(SceneExportInfo ^info) override {if (m_pPath!=NULL) m_pPath->SetMarkInsideSerializationSession(true);}
    VOVERRIDE void OnAfterExport(SceneExportInfo ^info) override {if (m_pPath!=NULL) m_pPath->SetMarkInsideSerializationSession(false);}

    // special
    void RenderShape(VisionViewBase ^view, ShapeRenderMode mode);
    inline void SetPickRendering(bool bPickRendering) {m_bPickRendering=bPickRendering;}

    //void SetKey(String ^key);
    void SetDisplayColor(UINT iColor) {m_iColor = iColor;}
    void SetLineDisplayMode(PathLineDisplayMode_e mode) {m_eLineDisplayMode=mode;}

    void Reset() {m_pPath->DeleteAllPathNodes();}
    void AddPathNode(EngineInstancePathNode ^pNodeInst);
    void SetClosed(bool bState) {m_pPath->SetClosed(bState);}
    inline void InvalidateBoundingBox() {bBBoxValid=false;}

    void EvaluatePathPositionSmooth(float t, Vector3F %pDest)
    {
      hkvVec3 vPos;
      m_pPath->EvalPointSmooth(t,vPos);
      pDest.X = vPos.x;
      pDest.Y = vPos.y;
      pDest.Z = vPos.z;
    }

    void EvaluatePathPosition(float t, Vector3F %pDest)
    {
      hkvVec3 vPos;
      m_pPath->EvalPoint(t,vPos);
      pDest.X = vPos.x;
      pDest.Y = vPos.y;
      pDest.Z = vPos.z;
    }
    void EvaluatePathDirection(float t, Vector3F %pDest)
    {
      hkvVec3 vPos;
      hkvVec3 vDir;
      m_pPath->EvalPoint(t,vPos,&vDir);
      vDir.normalizeIfNotZero();
      pDest.X = vDir.x;
      pDest.Y = vDir.y;
      pDest.Z = vDir.z;
    }

    float GetPathTimeAtScreenPos(float fScreenX, float fScreenY, float fStart, float fEnd, int iRecursions);

    bool GetPathDistFromRay(const hkvVec3& vStart, const hkvVec3& vEnd, float &fMinDist);

  private:
    VisPath_cl *m_pPath;
    unsigned int m_iColor; // used as VColorRef
    PathLineDisplayMode_e m_eLineDisplayMode;
    bool bBBoxValid;
    float bx1,by1,bz1,bx2,by2,bz2; // local bounding box
    bool m_bPickRendering;
  };


  public ref class EngineInstancePathNode : public IEngineShapeInstance
  {
  public:

    enum class PathNodeDisplayMode_e
    {
      None = 0,
      Unscaled = 1,
      Scaled = 2
    };

    EngineInstancePathNode();
    VOVERRIDE void DisposeObject() override;

    // path node should not return a value here as it does not inherit from VisTypedEngineObject_cl
    VOVERRIDE IntPtr GetNativeObject() override  {return System::IntPtr::Zero;}
    //VOVERRIDE IntPtr GetNativeObject() override  {return System::IntPtr((void*)m_pPathNode);}

    // overridden IEngineShapeInstance functions
    VOVERRIDE void SetVisible(bool bStatus) override  {}
    VOVERRIDE void SetOrientation(float yaw,float pitch,float roll) override  {}
    VOVERRIDE void SetPosition(float x,float y,float z) override {m_pPathNode->SetPosition(hkvVec3(x,y,z));UpdateControlVertices();}
    VOVERRIDE void SetScaling(float x,float y, float z) override  {}
    VOVERRIDE void SetObjectKey(String ^key) override;

    VOVERRIDE bool GetPosition(Vector3F %enginePosition) override
    {
      if (!m_pPathNode)
        return false;
      hkvVec3 worldPos = m_pPathNode->GetPosition();
      enginePosition = Vector3F(worldPos.x,worldPos.y,worldPos.z);
      return true;
    }
    VOVERRIDE bool GetOrientation(Vector3F %engineOrientation) override
    {
      engineOrientation = Vector3F(0,0,0);
      return true;
    }


    VOVERRIDE void TraceShape(Shape3D ^ownerShape, Vector3F rayStart,Vector3F rayEnd, ShapeTraceResult ^%result) override;
    VOVERRIDE bool GetLocalBoundingBox(BoundingBox ^%bbox) override;
    VOVERRIDE bool OnExport(SceneExportInfo ^info) override  {return true;}
    //VOVERRIDE void OnPostEngineInstanceCreation() override {ConversionUtils::CallOnDeserializationFunction(m_pPathNode) override;}

    // special
    void RenderShape(VisionViewBase ^view, ShapeRenderMode mode);
    VisPathNode_cl* GetEngineNode() {return m_pPathNode;}
//    void MakeControlVerticesColinear() {m_pPathNode->MakeControlVerticesColinear();}

    void UpdateControlVertices() 
    {
//      hkvVec3 vNodePos = m_pPathNode->GetPosition();
      hkvVec3 vNodePos(0,0,0); // tangents are passed in WS
      m_pPathNode->SetControlVertices(vNodePos+hkvVec3(tx1,ty1,tz1),vNodePos+hkvVec3(tx2,ty2,tz2));
      //Vision::Game.DrawCube(hkvVec3(tx1,ty1,tz1),2.f);
      //Vision::Game.DrawCube(hkvVec3(tx2,ty2,tz2),2.f);
    }
    void SetTangentIn(float x, float y, float z) { tx1=x; ty1=y; tz1=z; UpdateControlVertices();}
    void SetTangentOut(float x, float y, float z) { tx2=x; ty2=y; tz2=z; UpdateControlVertices();}

    void SetLinearIn(bool bStatus) {m_pPathNode->SetTypeIn(bStatus ? VisPathNode_cl::LINEAR : VisPathNode_cl::BEZIER);}
    void SetLinearOut(bool bStatus) {m_pPathNode->SetTypeOut(bStatus ? VisPathNode_cl::LINEAR : VisPathNode_cl::BEZIER);}

    // static properties:
    static void SetNodeDisplayMode(PathNodeDisplayMode_e mode) {g_eDisplayMode=mode;}
  private:
    VisPathNode_cl *m_pPathNode;
    VTextureObject *m_pNodeTex;
    float tx1,ty1,tz1; // tangent in
    float tx2,ty2,tz2; // tangent out

    static PathNodeDisplayMode_e g_eDisplayMode = PathNodeDisplayMode_e::Unscaled; 
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
