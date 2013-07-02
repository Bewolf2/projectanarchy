/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Editor/vForge/EditorPlugins/VisionPlugin/VisionManaged/VisionManagedPCH.h>
#include <Vision/Editor/vForge/EditorPlugins/VisionPlugin/VisionManaged/EngineInstances/EngineInstanceCustomVolume.hpp>
#include <Vision/Tools/Libraries/Geom2/vGeom2.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Entities/VCustomVolumeObject.hpp>

using namespace ManagedFramework;
using namespace ManagedBase;
using namespace System::Drawing;
using namespace System::Drawing::Imaging;
using namespace System::Diagnostics;

#ifdef _MSC_VER
// disable warning C4996: deprecated function
#pragma warning( disable : 4996)
#endif

namespace VisionManaged
{
  EngineInstanceCustomVolumeObject::EngineInstanceCustomVolumeObject(Shape3D ^ownerShape)
  {
    m_pOwnerShape = ownerShape;
    m_fHeight = 10.0f;
    m_bCreationFinished = false;
    m_bReverseWinding = false;
    m_bValid = true;
    m_pCustomVolumeEntity = new VCustomVolumeObject();
    m_pCustomVolumeEntity->SetCreatedFromEditor();
    m_pCustomVolumeEntity->AddRef();
    m_pspLightClippingVolumeDisplayMesh = new VCompiledTechniquePtr();

    if (!Vision::Shaders.LoadShaderLibrary("\\Shaders/LightClippingVolumes.ShaderLib", SHADERLIBFLAG_HIDDEN))
    {
      Vision::Error.Warning("Shader lib file for light clipping volumes could not be loaded!");
      return;
    }

    VCompiledEffectPtr effect = Vision::Shaders.CreateEffect("LightClippingVolumesDisplayMesh", NULL);
    if(effect)
      *m_pspLightClippingVolumeDisplayMesh = effect->GetDefaultTechnique();
  }

  void EngineInstanceCustomVolumeObject::DisposeObject()
  {
    V_SAFE_DISPOSEANDRELEASE(m_pCustomVolumeEntity);
    V_SAFE_DELETE(m_pspLightClippingVolumeDisplayMesh);
  }

  void EngineInstanceCustomVolumeObject::OnRenderHook(ShapeBase ^owner, int iConstant)
  {
    VASSERT(m_pCustomVolumeEntity != NULL);
    if(m_pCustomVolumeEntity->GetCustomStaticMesh())
    {
      VisStaticMesh_cl* pStaticMesh = m_pCustomVolumeEntity->GetStaticMesh();

      if(pStaticMesh != NULL)
      {
        VisMeshBuffer_cl* pMeshBuffer = pStaticMesh->GetMeshBuffer();
        VASSERT(pMeshBuffer != NULL);
        if(pMeshBuffer->GetIndexCount() <= 0)
          return;

        Vision::RenderLoopHelper.BeginMeshRendering();
        Vision::RenderLoopHelper.ResetMeshStreams();
        Vision::RenderLoopHelper.AddMeshStreams(pMeshBuffer, (*m_pspLightClippingVolumeDisplayMesh)->GetShader(0)->GetStreamMask() | VERTEX_STREAM_INDEXBUFFER);

        hkvMat4 transform (hkvNoInitialization);
        transform.setIdentity ();
        transform.setRotationalPart(m_pCustomVolumeEntity->GetRotationMatrix());
        transform.setTranslation(m_pCustomVolumeEntity->GetPosition());
        transform.setScalingFactors( m_pCustomVolumeEntity->GetScale() );

        Vision::RenderLoopHelper.SetMeshTransformationMatrix(transform);

        Vision::RenderLoopHelper.RenderMeshes((*m_pspLightClippingVolumeDisplayMesh)->GetShader(0), pMeshBuffer->GetPrimitiveType(), 0, pMeshBuffer->GetIndexCount() / 3, pMeshBuffer->GetVertexCount());
        Vision::RenderLoopHelper.EndMeshRendering();
      }
    }
  }

  void EngineInstanceCustomVolumeObject::SetPosition(float x, float y, float z)
  {
    IEngineInstanceObject3D::SetPosition(x, y, z);

    // inform owner object and its components, that position has changed
    if (GetO3DPtr())
      GetO3DPtr()->SendMsg(GetO3DPtr(), VIS_MSG_EDITOR_PROPERTYCHANGED, (INT_PTR) "Position", 0 );
  }

  void EngineInstanceCustomVolumeObject::SetOrientation(float yaw,float pitch,float roll)
  {
    IEngineInstanceObject3D::SetOrientation(yaw, pitch, roll);

    // inform owner object and its components, that orientation has changed
    if (GetO3DPtr())
      GetO3DPtr()->SendMsg(GetO3DPtr(), VIS_MSG_EDITOR_PROPERTYCHANGED, (INT_PTR) "Orientation", 0 );
  }

  void EngineInstanceCustomVolumeObject::SetScaling(float x, float y, float z)
  {
    if(m_pCustomVolumeEntity != NULL)
      m_pCustomVolumeEntity->SetScale( hkvVec3(x, y, z) );

    // inform owner object and its com ponents, that scaling has changed
    if (GetO3DPtr())
      GetO3DPtr()->SendMsg(GetO3DPtr(), VIS_MSG_EDITOR_PROPERTYCHANGED, (INT_PTR) "Scaling", 0 );
  }

  void EngineInstanceCustomVolumeObject::RenderShape(VisionViewBase ^view, ShapeRenderMode mode)
  {
    if(!m_pCustomVolumeEntity->GetCustomStaticMesh())
    {
      //Compute the offset vector
      Vector3F topOffset;
      {
        hkvVec3 vOffset(0.f, 0.f, m_fHeight * m_pCustomVolumeEntity->GetScale().z);

        Vector3F thisOrientation;
        GetOrientation(thisOrientation);
        hkvMat3 rot(hkvNoInitialization);
        rot.setFromEulerAngles (thisOrientation.Z, thisOrientation.Y, thisOrientation.X);

        vOffset = rot.transformDirection(vOffset);

        topOffset.X = vOffset.x; topOffset.Y = vOffset.y; topOffset.Z = vOffset.z;
      }

      //Draw all the vertices
      VColorRef color;
      if (m_bValid)
        color.SetRGB(0, 200, 0);
      else
        color.SetRGB(255, 0, 0);

      ShapeCollection ^vertices = m_pOwnerShape->ChildCollection;
      System::Collections::Generic::IEnumerator<ShapeBase^> ^it = vertices->GetEnumerator();

      it->MoveNext();
      ShapeBase ^lastVertex = it->Current;
      ShapeBase ^firstVertex = lastVertex;
      for(; it->MoveNext();)
      {
        ShapeBase ^vertex = it->Current;
        Vector3F from,to;
        Vector3F fromTop,toTop;
        lastVertex->_engineInstance->GetPosition(from);
        vertex->_engineInstance->GetPosition(to);
        fromTop = from + topOffset;
        toTop = to + topOffset;
        lastVertex = vertex;

        Vision::Game.DrawSingleLine(from.X,from.Y,from.Z,to.X,to.Y,to.Z,color);
        Vision::Game.DrawSingleLine(fromTop.X,fromTop.Y,fromTop.Z,toTop.X,toTop.Y,toTop.Z,color);
        Vision::Game.DrawSingleLine(from.X,from.Y,from.Z,fromTop.X,fromTop.Y,fromTop.Z,color);
      }

      if(lastVertex != firstVertex)
      {
        Vector3F from,to;
        Vector3F fromTop,toTop;
        lastVertex->_engineInstance->GetPosition(from);
        firstVertex->_engineInstance->GetPosition(to);
        fromTop = from + topOffset;
        toTop = to + topOffset;

        Vision::Game.DrawSingleLine(from.X,from.Y,from.Z,to.X,to.Y,to.Z,color);
        Vision::Game.DrawSingleLine(fromTop.X,fromTop.Y,fromTop.Z,toTop.X,toTop.Y,toTop.Z,color);
        Vision::Game.DrawSingleLine(from.X,from.Y,from.Z,fromTop.X,fromTop.Y,fromTop.Z,color);
      }
    }
  }

  bool EngineInstanceCustomVolumeObject::GetLocalBoundingBox(BoundingBox ^%bbox)
  {
    if(m_pCustomVolumeEntity->GetCustomStaticMesh())
    {
      if(m_pCustomVolumeEntity->GetStaticMesh() == NULL)
        return false;

      hkvAlignedBBox nativeBbox = m_pCustomVolumeEntity->GetStaticMesh()->GetBoundingBox();
      bbox->Set(nativeBbox.m_vMin.x, nativeBbox.m_vMin.y, nativeBbox.m_vMin.z,
                nativeBbox.m_vMax.x, nativeBbox.m_vMax.y, nativeBbox.m_vMax.z);
      bbox->AddBorder(2.0f);
    }
    else 
    {
      //the offset vector
      Vector3F topOffset(0.f,0.f,m_fHeight);

      ShapeCollection ^vertices = m_pOwnerShape->ChildCollection;
      System::Collections::Generic::IEnumerator<ShapeBase^> ^it = vertices->GetEnumerator();
      if(!it->MoveNext())
      {
        const float fSize = 15.f * EditorManager::Settings->GlobalUnitScaling;
        bbox->Set(-fSize,-fSize,-fSize,fSize,fSize,fSize);
        return true;
      }

      ShapeBase ^vertex = it->Current;

      Vector3F vertexPos = ((EngineInstanceCustomVolumeVertex^)vertex->_engineInstance)->localPosition;
      bbox->vMin = vertexPos;
      bbox->vMax = vertexPos;
      bbox->AddPoint(vertexPos+topOffset);

      for(; it->MoveNext();)
      {
        vertex = it->Current;
        vertexPos = ((EngineInstanceCustomVolumeVertex^)vertex->_engineInstance)->localPosition;
        bbox->AddPoint(vertexPos);
        bbox->AddPoint(vertexPos+topOffset);
      }

      if(!bbox->Valid)
      {
        const float fSize = 15.f * EditorManager::Settings->GlobalUnitScaling;
        bbox->Set(-fSize,-fSize,-fSize,fSize,fSize,fSize);
        return true;
      }

      bbox->AddBorder(2.0f);
    }
    return true;
  }

  void EngineInstanceCustomVolumeObject::TraceShape(Shape3D ^ownerShape, Vector3F rayStart,Vector3F rayEnd, ShapeTraceResult ^%result)
  {
    bool hit = false;
    if(! m_pCustomVolumeEntity->GetCustomStaticMesh())
    {
      ShapeCollection ^vertices = m_pOwnerShape->ChildCollection;
      System::Collections::Generic::IEnumerator<ShapeBase^> ^it = vertices->GetEnumerator();

      //Prefer child objects for picking
      for(; it->MoveNext();)
      {
        it->Current->_engineInstance->TraceShape((Shape3D^)it->Current,rayStart,rayEnd,result);
        if(result->hitShape == it->Current)
          hit = true;
      }
    }

    VisStaticMesh_cl* pStaticMesh = m_pCustomVolumeEntity->GetStaticMesh();

    if (!hit && pStaticMesh != NULL && pStaticMesh->GetSubmeshCount())
    {
      IVCollisionMesh* pTraceMesh = pStaticMesh->GetTraceMesh(true);
      VTraceHit traceHit;
      VisTraceLineInfo_t info;
      hkvVec3 vStart(rayStart.X,rayStart.Y,rayStart.Z);
      hkvVec3 vEnd(rayEnd.X,rayEnd.Y,rayEnd.Z);

      hkvMat4 transform; transform.setIdentity ();
      transform.setRotationalPart(m_pCustomVolumeEntity->GetRotationMatrix());
      transform.setTranslation(m_pCustomVolumeEntity->GetPosition());
      transform.setScalingFactors( m_pCustomVolumeEntity->GetScale() );
      transform.invert();

      vStart = transform.transformPosition(vStart);
      vEnd = transform.transformPosition(vEnd);

      hkvVec3 vDir = vStart - vEnd;

      hkvAlignedBBox rayBox(vStart, vStart);
      rayBox.expandToInclude(vEnd);
      rayBox.addBoundary(hkvVec3(1.0f, 1.0f, 1.0f));

      if (pTraceMesh->GetTriangleCount() > 0)
      {
        int iHitCount = pTraceMesh->PerformIndexedCollisionMeshTraceTest(&traceHit, NULL, vStart, vEnd, vDir.getLength(), rayBox, true, 0, pTraceMesh->GetTriangleCount(), 1, &info, false);
        if(iHitCount > 0)
        {
          result->SetShapeHit_IfCloser(m_pOwnerShape, info.distance);
        }
      }
      hit = true; //We did not actually hit something but we don't want to do the bounding box test
    }

    if ( !hit && ConversionUtils::TraceOrientedBoundingBox(m_pOwnerShape->LocalScaledBoundingBox, m_pOwnerShape->Position, m_pOwnerShape->RotationMatrix, rayStart, rayEnd, result))
      result->hitShape = m_pOwnerShape;
  }

  void EngineInstanceCustomVolumeObject::OnBeforeExport(SceneExportInfo ^info)
  {
    IEngineInstanceObject3D::OnBeforeExport(info);

    if( !m_pCustomVolumeEntity->GetCustomStaticMesh() )
    {
      //String^ path = info->AbsoluteExportDataFolder;
      String^ path = System::IO::Path::GetDirectoryName(info->AbsoluteFilename);
      path += String::Format("\\Volume_{0:x8}_{1:x8}.vmesh", m_pOwnerShape->ParentLayer->LayerID, m_pOwnerShape->LocalID);
      VString destPath;
      ConversionUtils::StringToVString(path,destPath);

      bool alreadyExists = false;
      if(System::IO::File::Exists(path))
      {
        ManagedBase::RCS::GetProvider()->EditFile(path);
        alreadyExists = true;
      }

      // Do we have anything to export?
      ShapeCollection ^childVertices = m_pOwnerShape->ChildCollection;
      if (childVertices->Count != 0)
      {
        EarClippingToVMeshFile earClipping(destPath);
        RunEarClipping(earClipping);

        /// Set the relative path to the static mesh
        char relativePath[FS_MAX_PATH];
        VString projectPath;
        ConversionUtils::StringToVString( EditorManager::Project->ProjectDir, projectPath );
        VPathHelper::MakePathRelative(relativePath,projectPath,destPath);
        m_pCustomVolumeEntity->SetStaticMeshPath(relativePath);

        // Set filename
        m_pCustomVolumeEntity->GetStaticMesh()->SetFilename(relativePath);

        if(!alreadyExists)
          ManagedBase::RCS::GetProvider()->AddFile(path, true /* Binary file */);
      } 
    }
  }

  bool EngineInstanceCustomVolumeObject::OnExport(SceneExportInfo ^info) 
  {
    Debug::Assert( m_pCustomVolumeEntity != nullptr );
    VArchive &ar = *((VArchive *)info->NativeShapeArchivePtr.ToPointer());
    ar << m_pCustomVolumeEntity;
    return true;
  }

  struct BorderVertex
  {
    int index;
    hkvVec2 pos;
  };

  int GetNextIndex(int index, int count)
  {
    return (index + 1) % count;
  }

  int GetPreviousIndex(int index, int count)
  {
    index = (index - 1) % count;
    if (index < 0)
      index = count + index;
    return index;
  }

  bool PointInTriangle(const hkvVec2& tp1, const hkvVec2& tp2, const hkvVec2& tp3, const hkvVec2& p)
  {
    float b0 = ((tp2.x - tp1.x) * (tp3.y - tp1.y) - (tp3.x - tp1.x) * (tp2.y - tp1.y));
    if (b0 != 0)
    {
      float b1 = (((tp2.x - p.x) * (tp3.y - p.y) - (tp3.x - p.x) * (tp2.y - p.y)) / b0);
      float b2 = (((tp3.x - p.x) * (tp1.y - p.y) - (tp1.x - p.x) * (tp3.y - p.y)) / b0);
      float b3 = 1 - b1 - b2;

      return (b1 > 0) && (b2 > 0) && (b3 > 0);
    }
    else
      return false;
  }

  void EngineInstanceCustomVolumeObject::RunEarClipping(EarClippingCallbacks& callbacks)
  {
    //#define DEBUG_EARCLIPPING
    #ifdef DEBUG_EARCLIPPING
    String^ path = "C:\\VisionDebug\\" + String::Format("{0}",Vision::GetTimer()->GetTime());
    #endif

    ShapeCollection ^childVertices = m_pOwnerShape->ChildCollection;
    System::Collections::Generic::IEnumerator<ShapeBase^> ^it = childVertices->GetEnumerator();

    VArray<hkvVec3> tmp(childVertices->Count, childVertices->Count);
    VArray<hkvVec3> vertices(childVertices->Count*2, childVertices->Count*2);

    //No vertices -> nothing to do
    if(vertices.GetLength() == 0)
      return;

    //Check if all vertices have valid engine instances
    for(; it->MoveNext();)
    {
      if(it->Current->_engineInstance == nullptr)
        return;
    }

    callbacks.Init(vertices.GetLength());

    int iNumVertices = 0;

    // add vertices to tmp array and determine winding
    it = childVertices->GetEnumerator();
    for(; it->MoveNext();)
    {
      Vector3F pos = ((EngineInstanceCustomVolumeVertex^)it->Current->_engineInstance)->localPosition;
      tmp[iNumVertices] = hkvVec3(pos.X,pos.Y,pos.Z);
      iNumVertices++;
    }

    float polygonArea = 0.0f;
    for(int i=0; i<iNumVertices; i++)
    {
      int curIndex = i;
      int nextIndex = GetNextIndex(i, iNumVertices);

      const hkvVec3& cur = tmp[curIndex];
      const hkvVec3& next = tmp[nextIndex];

      polygonArea += cur.x * (next.y - cur.y) - cur.y * (next.x - cur.x);
    }

    m_bReverseWinding = false;
    if (polygonArea < 0.0f)
    {
      m_bReverseWinding = true;
      for (int i = 0; i < iNumVertices/2; i++)
      {
        hkvMath::swap(tmp[i], tmp[iNumVertices - i - 1]);
      }
    }

    // Add all vertices
    for (int i = 0; i < iNumVertices; i++)
    {
      const hkvVec3& pos = tmp[i];
      callbacks.AddVertex(pos.x, pos.y, pos.z);
      vertices[i] = pos;
    }
    for (int i = 0; i < iNumVertices; i++)
    {
      hkvVec3 pos = tmp[i];
      pos.z += m_fHeight;
      callbacks.AddVertex(pos.x, pos.y, pos.z);
      vertices[i + iNumVertices] = pos;
    }

    VArray<BorderVertex> border(iNumVertices, iNumVertices);
    
    const int iHalfNumVerts = iNumVertices;
    iNumVertices *= 2;

    for(int i=0; i<iHalfNumVerts; i++)
    {
      //first side triangle
      callbacks.AddTriangle(i, 
                            GetNextIndex(i, iHalfNumVerts), 
                            i + iHalfNumVerts);
      //second side triangle
      callbacks.AddTriangle(GetNextIndex(i, iHalfNumVerts),
                            GetNextIndex(i, iHalfNumVerts) + iHalfNumVerts,
                            i + iHalfNumVerts);

      border[i].index = i;
      border[i].pos = hkvVec2(vertices[i].x, vertices[i].y);
    }

#ifdef DEBUG_EARCLIPPING
    int iStep = 0;

    Bitmap^ debugBitmap = gcnew Bitmap(512,512,PixelFormat::Format24bppRgb);
    Graphics^ g = Graphics::FromImage(debugBitmap);
    g->FillRectangle(gcnew SolidBrush(Color::White),0,0,512,512);

    Pen^ blackPen = gcnew Pen(Color::Black,4);
    Pen^ grayPen = gcnew Pen(Color::Gray,4);
    Pen^ redPen = gcnew Pen(Color::Red);
    Pen^ greenPen = gcnew Pen(Color::LimeGreen);
    Font^ font = gcnew Font("Tahoma", 12);
    BoundingBox^ bbox = gcnew BoundingBox();
    GetLocalBoundingBox(bbox);

    hkvVec2 scale(1.0f/bbox->SizeX*512.0f, 1.0f/bbox->SizeY*512.0f);
    hkvVec2 center = hkvVec2(bbox->Center.X, bbox->Center.Y);
    hkvVec2 offset = hkvVec2(bbox->SizeX/2.0f, bbox->SizeY/2.0f);
    for (int i=0; i<border.GetLength(); i++)
    {
      hkvVec2 from = scale.compMul(border[i].pos - center + offset);
      hkvVec2 to = scale.compMul(border[GetNextIndex(i, border.GetLength())].pos - center + offset);
      g->DrawLine(blackPen, (int)from.x, (int)from.y, (int)to.x, (int)to.y );
      g->DrawString("P"+border[i].index, font, Brushes::Black, from.x, from.y+5.f);
    }
#endif

    // Earclipping algorithm
    int iCurrent = 0;
    int iLastear = -1; //prevent endless loop on invalid polygon

    do
    {
      if (border.GetLength() == 3)
        break;

      iLastear++;

      int iPrev = GetPreviousIndex(iCurrent, border.GetLength());
      int iNext = GetNextIndex(iCurrent, border.GetLength());

      hkvVec2 p1 = border[iPrev].pos;
      hkvVec2 p = border[iCurrent].pos;
      hkvVec2 p2 = border[iNext].pos;

      float l = ((p1.x - p.x) * (p2.y - p.y) - (p1.y - p.y) * (p2.x - p.x));
      if (l < 0.0f)
      {
        bool inTriangle = false;
        for(int j = 2; j < border.GetLength() - 1; j++)
        {
          hkvVec2 pt = border[(iCurrent + j) % border.GetLength()].pos;
          if (PointInTriangle(p1, p2, p, pt))
          {
            inTriangle = true;
            break;
          }
        }

        #ifdef DEBUG_EARCLIPPING
          if (inTriangle)
          {
            hkvVec2 from = scale.compMul(p1 - center + offset);
            hkvVec2 to = scale.compMul(p2 - center + offset);
            g->DrawLine(redPen, (int)from.x, (int)from.y, (int)to.x, (int)to.y);
          }
        #endif

        if (!inTriangle)
        {
          //Valid triangle

          //bottom triangle
          callbacks.AddTriangle( border[iPrev].index,
                                 border[iNext].index,
                                 border[iCurrent].index);

          //top triangle
          callbacks.AddTriangle( border[iPrev].index + iHalfNumVerts,
                                 border[iCurrent].index + iHalfNumVerts,
                                 border[iNext].index + iHalfNumVerts );

          border.RemoveAt(iCurrent);

          #ifdef DEBUG_EARCLIPPING
            hkvVec2 from = scale.compMul(p1 - center + offset);
            hkvVec2 to = scale.compMul(p2 - center + offset);
            g->DrawLine(greenPen, (int)from.x, (int)from.y, (int)to.x, (int)to.y);
            debugBitmap->Save(path + "debug" + iStep + ".bmp" );
            iStep++;

            debugBitmap = gcnew Bitmap(512,512,PixelFormat::Format24bppRgb);
            g = Graphics::FromImage(debugBitmap);
            g->FillRectangle(gcnew SolidBrush(Color::White),0,0,512,512);

            for (int i=0; i<border.GetLength(); i++)
            {
              hkvVec2 from = scale.compMul(border[i].pos - center + offset);
              hkvVec2 to = scale.compMul(border[GetNextIndex(i, border.GetLength())].pos - center + offset);
              g->DrawLine(blackPen, (int)from.x, (int)from.y, (int)to.x, (int)to.y );
              g->DrawString("P"+border[i].index, font, Brushes::Black, from.x, from.y+5.f);
            }
          #endif

          iLastear = 0;
          iCurrent--;
        }
      }

      iCurrent = (iCurrent+1) % border.GetLength();
    }
    while (iLastear <= border.GetLength() * 2 && border.GetLength() > 3);

    if (border.GetLength() == 3)
    {
      //bottom triangle
      callbacks.AddTriangle(border[0].index,
                            border[2].index,
                            border[1].index);

      //top triangle
      callbacks.AddTriangle(border[0].index + iHalfNumVerts,
                            border[1].index + iHalfNumVerts,
                            border[2].index + iHalfNumVerts);

      m_bValid = true;
    }
    else
    {
      // used for debug rendering
      m_bValid = false;
    }
   
#ifdef DEBUG_EARCLIPPING
    debugBitmap->Save(path + "debug" + iStep + ".bmp" );
#endif
    
    callbacks.Finish();
  }

  void EngineInstanceCustomVolumeObject::OnCreationFinished()
  {
    m_pCustomVolumeEntity->Init();
    if(!m_pCustomVolumeEntity->GetCustomStaticMesh())
     InitStaticMesh();
  }

  void EngineInstanceCustomVolumeObject::InitStaticMesh()
  {
    VisStaticMesh_cl* pStaticMesh = m_pCustomVolumeEntity->GetStaticMesh();
    VASSERT(pStaticMesh != NULL);

    pStaticMesh->EnsureMeshCreated();
    pStaticMesh->AllocateSubmeshes(1);
    pStaticMesh->AllocateSurfaces(1);

    {
      VisStaticSubmesh_cl& submesh = *pStaticMesh->GetSubmesh(0);
      VisSurface_cl& surface = *pStaticMesh->GetSurface(0);

      surface.m_spDiffuseTexture = Vision::TextureManager.GetPlainWhiteTexture();
      surface.SetTransparencyType(VIS_TRANSP_NONE);
      surface.SetAmbientColor(VColorRef(255,255,255));
      //surface.SetDoubleSided(true);
      surface.SetLightingMode(VIS_LIGHTING_FULLBRIGHT);
      surface.SetShaderMode(VisSurface_cl::VSM_Auto);

      submesh.SetSurface(&surface,0);
    }

    m_bCreationFinished = true;
    UpdateStaticMesh(VUpdateType_e::VUT_UPDATE_RETRIANGULATE);
    pStaticMesh->RemoveResourceFlag(VRESOURCEFLAG_ALLOWUNLOAD); // the mesh is generated in code and should not be reloaded from disk
    pStaticMesh->FlagAsLoaded();
  }

  String^ EngineInstanceCustomVolumeObject::CheckIsStaticMeshValid()
  {
    if(m_pCustomVolumeEntity->GetStaticMesh() == NULL)
    {
      return gcnew String("Static mesh could not be loaded");
    }

    if(m_pCustomVolumeEntity->GetStaticMesh()->GetSubmeshCount() != 1)
    {
      return String::Format("If a static mesh is used as a volume it may only contain exactly 1 submesh. The given static mesh contains {0} submeshes", m_pCustomVolumeEntity->GetStaticMesh()->GetSubmeshCount());
    }

    if(m_pCustomVolumeEntity->GetStaticMesh()->GetMeshBuffer()->GetPrimitiveType() != VisMeshBuffer_cl::MB_PRIMTYPE_INDEXED_TRILIST)
    {
      return gcnew String("Static meshes used for volumes have to consist of indexed triangles");
    }

    return nullptr;
  }

  void EngineInstanceCustomVolumeObject::UpdateStaticMesh(VUpdateType_e updateType)
  {
    VisStaticMesh_cl* pStaticMesh = m_pCustomVolumeEntity->GetStaticMesh();
    // Only update the static mesh if the custom volume has been completely created, and if
    // there is no custom static mesh set. The latter is important, otherwise we'd overwrite
    // the custom mesh with the mesh generated from our vertices.
    if(!m_bCreationFinished || pStaticMesh == NULL || m_pCustomVolumeEntity->GetCustomStaticMesh())
      return;

    switch(updateType)
    {
    case VUpdateType_e::VUT_UPDATE_POSITION:
      {
        if(pStaticMesh && m_pOwnerShape)
        {
          ShapeCollection ^childVertices = m_pOwnerShape->ChildCollection;
          int iStart,iNumVertices;
          pStaticMesh->GetSubmesh(0)->GetRenderVertexRange(iStart, iNumVertices);

          if(iNumVertices == 0) //nothing to update
            return;

          if(childVertices->Count * 2 != iNumVertices)
          {
            UpdateStaticMesh(VUpdateType_e::VUT_UPDATE_RETRIANGULATE);
            break;
          }

          VisMeshBuffer_cl* pMeshBuffer = pStaticMesh->GetMeshBuffer();
          VASSERT(pMeshBuffer != NULL);

          hkvVec3 *posData = (hkvVec3*)pMeshBuffer->LockVertices(VIS_LOCKFLAG_DISCARDABLE);
          System::Collections::Generic::IEnumerator<ShapeBase^> ^it = childVertices->GetEnumerator();

          //Add all vertices
          const int iHalfNumVerts = iNumVertices / 2;
          if (m_bReverseWinding)
          {
            for(int i = iHalfNumVerts-1; it->MoveNext(); i--)
            {
              Vector3F pos = ((EngineInstanceCustomVolumeVertex^)it->Current->_engineInstance)->localPosition;
              posData[i] = hkvVec3(pos.X, pos.Y, pos.Z);
              posData[i+iHalfNumVerts] = hkvVec3(pos.X, pos.Y, pos.Z + m_fHeight);
            }
          }
          else
          {
            for(int i = 0; it->MoveNext(); i++)
            {
              Vector3F pos = ((EngineInstanceCustomVolumeVertex^)it->Current->_engineInstance)->localPosition;
              posData[i] = hkvVec3(pos.X, pos.Y, pos.Z);
              posData[i+iHalfNumVerts] = hkvVec3(pos.X, pos.Y, pos.Z + m_fHeight);
            }
          }

          posData = NULL;
          pMeshBuffer->UnLockVertices();
        }
      }
      break;
    case VUpdateType_e::VUT_UPDATE_RETRIANGULATE:
      {
        EarClippingToStaticMesh earClipping(*pStaticMesh);
        RunEarClipping(earClipping);
      }
      break;
      default:
        VASSERT_MSG(false,"missing implementation");
    }

    // inform owner object and its components, that volume geometry has changed
    if (GetO3DPtr())
      GetO3DPtr()->SendMsg(GetO3DPtr(), VIS_MSG_EDITOR_PROPERTYCHANGED, (INT_PTR) "VolumeGeometry", 0 );
  }

  void EngineInstanceCustomVolumeObject::SetCustomStaticMesh(bool bValue)
  {
    m_pCustomVolumeEntity->SetCustomStaticMesh(bValue);

    //Check if a new empty static mesh was created and if we need to triangulate
    if(!bValue && m_pCustomVolumeEntity->GetStaticMesh() && m_pCustomVolumeEntity->GetStaticMesh()->GetSubmeshCount() == 0)
    {
      InitStaticMesh();
    }

    // inform owner object and its components, that using custom static mesh state has changed
    if (GetO3DPtr())
      GetO3DPtr()->SendMsg(GetO3DPtr(), VIS_MSG_EDITOR_PROPERTYCHANGED, (INT_PTR) "CustomStaticMesh", 0 );
  }

  void EngineInstanceCustomVolumeObject::SetStaticMeshPath(String^ path)
  {
    VString temp;
    ConversionUtils::StringToVString(path,temp);
    m_pCustomVolumeEntity->SetStaticMeshPath(temp);

    // inform owner object and its components, that static mesh path has changed
    if (GetO3DPtr())
      GetO3DPtr()->SendMsg(GetO3DPtr(), VIS_MSG_EDITOR_PROPERTYCHANGED, (INT_PTR) "StaticMeshPath", 0 );
  }

  //////////////////////////////////////
  // Vertex
  //////////////////////////////////////

  EngineInstanceCustomVolumeVertex::EngineInstanceCustomVolumeVertex(Shape3D ^owner)
  {
    m_pIcon = Vision::TextureManager.Load2DTexture("textures\\pin_green32.dds",VTM_FLAG_DEFAULT_NON_MIPMAPPED);
    if(m_pIcon != NULL)
     m_pIcon->AddRef();
    m_pOwner = owner;
  }

  void EngineInstanceCustomVolumeVertex::DisposeObject()
  {
    V_SAFE_RELEASE(m_pIcon);
  }

  void EngineInstanceCustomVolumeVertex::OnPostEngineInstanceCreation()
  {
    IEngineInstanceObject3D::OnPostEngineInstanceCreation();
  }

  void EngineInstanceCustomVolumeVertex::RenderShape(VisionViewBase ^view, ShapeRenderMode mode)
  {
    IVRenderInterface *pRenderer = (static_cast<VisionView ^>(view))->GetRenderInterface();

    Vector3F pos;
    GetPosition(pos);

    if (mode==ShapeRenderMode::Selected)
    {
      const float fRad = 7.f;
      hkvAlignedBBox bbox (hkvVec3 (pos.X-fRad,pos.Y-fRad,pos.Z-fRad), hkvVec3 (pos.X+fRad,pos.Y+fRad,pos.Z+fRad));
      pRenderer->DrawLineBox(bbox,V_RGBA_YELLOW,2.f);
    }

    VSimpleRenderState_t state(VIS_TRANSP_ALPHA);
    Vector3F vCamPos = view->CameraPosition;
    hkvVec3 diff(pos.X-vCamPos.X, pos.Y-vCamPos.Y,pos.Z-vCamPos.Z);
    float fSize = 0.05f*diff.getLength(); // unscaled

    pRenderer->DrawSprite(hkvVec3 (pos.X,pos.Y,pos.Z),m_pIcon,V_RGBA_WHITE,state,fSize,fSize);
  }

  bool EngineInstanceCustomVolumeVertex::GetLocalBoundingBox(BoundingBox ^%bbox)
  {
    const float fSize = 15.f * EditorManager::Settings->GlobalUnitScaling;
    (*bbox).Set(-fSize,-fSize,-fSize,fSize,fSize,fSize);
    return true;
  }

  void EngineInstanceCustomVolumeVertex::TraceShape(Shape3D ^ownerShape, Vector3F rayStart,Vector3F rayEnd, ShapeTraceResult ^%result)
  {
    if (ConversionUtils::TraceOrientedBoundingBox(ownerShape->LocalBoundingBox, ownerShape->Position, ownerShape->RotationMatrix, rayStart, rayEnd, result))
      result->hitShape = ownerShape;
  }

  bool EngineInstanceCustomVolumeVertex::GetPosition(Vector3F %enginePosition)
  {
    hkvMat3 rot(hkvNoInitialization);
    rot.setFromEulerAngles (m_pOwner->Orientation.Z, m_pOwner->Orientation.Y, m_pOwner->Orientation.X);
    hkvMat4 transform (hkvNoInitialization);
    transform.setIdentity ();
    transform.setRotationalPart(rot);
    transform.setTranslation(hkvVec3(m_pOwner->Position.X, m_pOwner->Position.Y, m_pOwner->Position.Z));
    transform.setScalingFactors(hkvVec3(m_pOwner->ScaleX,m_pOwner->ScaleY,m_pOwner->ScaleZ));
    hkvVec3 pos = transform.transformPosition(hkvVec3(localPosition.X,localPosition.Y,localPosition.Z));
    enginePosition.X = pos.x; enginePosition.Y = pos.y; enginePosition.Z = pos.z;
    return true;
  }

  void EngineInstanceCustomVolumeVertex::SetPosition(float x, float y, float z)
  {
    hkvMat3 rot(hkvNoInitialization);
    rot.setFromEulerAngles (m_pOwner->Orientation.Z, m_pOwner->Orientation.Y, m_pOwner->Orientation.X);

    hkvMat4 transform (hkvNoInitialization);
    transform.setIdentity ();
    transform.setRotationalPart(rot);
    transform.setTranslation(hkvVec3(m_pOwner->Position.X, m_pOwner->Position.Y, m_pOwner->Position.Z));
    transform.setScalingFactors(hkvVec3(m_pOwner->ScaleX,m_pOwner->ScaleY,m_pOwner->ScaleZ));
    transform.invert();
    hkvVec3 pos = transform.transformPosition(hkvVec3(x,y,z));
    localPosition.X = pos.x; localPosition.Y = pos.y; localPosition.Z = pos.z;
    if(m_pOwner->_engineInstance != nullptr)
      ((EngineInstanceCustomVolumeObject^)m_pOwner->_engineInstance)->UpdateStaticMesh(EngineInstanceCustomVolumeObject::VUpdateType_e::VUT_UPDATE_RETRIANGULATE);
  }

  bool EngineInstanceCustomVolumeVertex::GetOrientation(Vector3F %engineOrientation)
  {
    if(m_pOwner != nullptr)
      return m_pOwner->_engineInstance->GetOrientation(engineOrientation);
    return false;
  }

  void EngineInstanceCustomVolumeVertex::OnRemoveFromScene()
  {
    if(m_pOwner->_engineInstance != nullptr)
      ((EngineInstanceCustomVolumeObject^)m_pOwner->_engineInstance)->UpdateStaticMesh(EngineInstanceCustomVolumeObject::VUpdateType_e::VUT_UPDATE_RETRIANGULATE);
  }

  EarClippingToVMeshFile::EarClippingToVMeshFile(VString filename) :
    m_scene(),
    m_mesh(m_scene.CreateMesh( VGVertex::VertexMask(VGVertex::VGVM_POSITION | VGVertex::VGVM_NORMAL) )),
    m_sFilename(filename)
  {

  }

  void EarClippingToVMeshFile::Init(int numNeededVertices)
  {
  }

  void EarClippingToVMeshFile::AddVertex(float x, float y, float z)
  {
    VGVertex vertex( VGVertex::VertexMask(VGVertex::VGVM_POSITION | VGVertex::VGVM_NORMAL) );
    vertex.SetNormal( hkvVec3(0.0f,0.0f,1.0f) );
    vertex.SetPosition( hkvVec3(x,y,z) );
    m_mesh.GetVertexList().AddVertex(vertex);
  }

  void EarClippingToVMeshFile::AddTriangle(int iIndex1, int iIndex2, int iIndex3)
  {
    VGTriangleList::Triangle triangle;
    triangle.tp.materialIndex = -1;
    triangle.tp.geomInfoIndex = -1;
    triangle.tp.groupIndex = 0;
    triangle.tp.visibilityID = 0;
    triangle.tp.triangleFlags = VGTriangleList::VGTF_VISIBLE;
    triangle.tp.physicsInfoIndex = -1;
    triangle.ti[0] = iIndex1;
    triangle.ti[1] = iIndex2;
    triangle.ti[2] = iIndex3;
    m_mesh.GetTriangleList().AddTriangle(triangle);
  }

  void EarClippingToVMeshFile::Finish()
  {
    // force double buffering of static mesh to enable read back of mesh data (vertices/ indices) on all platforms
    m_mesh.SetDoubleBuffering(true, true, true);

    VGBackend backend;
    VGProcessor_VisionExporter exportProcessor;
    IVFileOutStream* pOutStream = Vision::File.GetManager()->Create(m_sFilename);
    exportProcessor.SetOutStream(pOutStream);
    backend.AddProcessor(&exportProcessor);
    backend.RunProcessors(m_scene);
    pOutStream->Flush();
    pOutStream->Close();
  }

  EarClippingToStaticMesh::EarClippingToStaticMesh(VisStaticMesh_cl& staticMesh) :
    m_staticMesh(staticMesh),
    m_vertices(NULL),
    m_iCurVertexIndex(-1)
  {
  }

  void EarClippingToStaticMesh::Init(int numNeededVertices)
  {
    VisMeshBuffer_cl *pMeshBuffer = m_staticMesh.GetMeshBuffer();
    VASSERT(pMeshBuffer != NULL);
    pMeshBuffer->SetResourceFlag(VRESOURCEFLAG_ISLOADED);
    pMeshBuffer->SetFilename("CustomVolumeMesh");
    m_bbox.setZero ();
    if( pMeshBuffer->GetVertexCount() != numNeededVertices )
    {
      pMeshBuffer->FreeVertices();

      VisMBVertexDescriptor_t desc;
      desc.m_iStride = sizeof(hkvVec3);
      desc.m_iPosOfs = 0;
      desc.SetFormatDefaults();
      int iStreamMask = desc.GetStreamMask();
      pMeshBuffer->AllocateVertices(desc,numNeededVertices);
    }

    m_iCurVertexIndex = 0;
    m_triangles.SetSize(0,-1,false);
    m_triangles.Reserve(numNeededVertices * 4);
    m_vertices = (hkvVec3*)pMeshBuffer->LockVertices(VIS_LOCKFLAG_DISCARDABLE);
    #ifdef HK_DEBUG
    m_iMaxvertexIndex = numNeededVertices;
    #endif
  }

  void EarClippingToStaticMesh::AddVertex(float x, float y, float z)
  {
    VASSERT(m_iCurVertexIndex < m_iMaxvertexIndex);
    m_vertices[m_iCurVertexIndex] = hkvVec3(x,y,z);
    m_iCurVertexIndex++;
    m_bbox.expandToInclude(hkvVec3(x,y,z));
  }

  void EarClippingToStaticMesh::AddTriangle(int iIndex1, int iIndex2, int iIndex3)
  {
    m_triangles.Append(Triangle(iIndex1,iIndex2,iIndex3));
  }

  void EarClippingToStaticMesh::Finish()
  {
    VisMeshBuffer_cl *pMeshBuffer = m_staticMesh.GetMeshBuffer();
    VASSERT(pMeshBuffer);
    if( pMeshBuffer->GetIndexCount() != m_triangles.GetLength() * 3 )
    {
      pMeshBuffer->FreeIndexList();
      pMeshBuffer->SetPrimitiveType(VisMeshBuffer_cl::MB_PRIMTYPE_INDEXED_TRILIST);
      pMeshBuffer->AllocateIndexList(m_triangles.GetLength() * 3);
    }
    pMeshBuffer->SetPrimitiveCount(m_triangles.GetLength());
    VASSERT(pMeshBuffer->GetIndexType() == VIS_INDEXFORMAT_16);
    Triangle* pIndices = (Triangle*)pMeshBuffer->LockIndices(VIS_LOCKFLAG_DISCARDABLE);
    memcpy(pIndices,m_triangles.GetData(), m_triangles.GetLength() * sizeof(Triangle));
    pIndices = NULL;
    pMeshBuffer->UnLockIndices();
    pMeshBuffer->UnLockVertices();
    m_staticMesh.UpdateStreamMasks();
    VisStaticSubmesh_cl* pSubMesh = m_staticMesh.GetSubmesh(0);
    VASSERT(pSubMesh);
    pSubMesh->SetRenderRange(0,m_triangles.GetLength()*3);
    pSubMesh->SetRenderVertexRange(0,m_iCurVertexIndex);
    pSubMesh->SetBoundingBox(m_bbox);
    m_staticMesh.SetBoundingBox(m_bbox);
  }
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
