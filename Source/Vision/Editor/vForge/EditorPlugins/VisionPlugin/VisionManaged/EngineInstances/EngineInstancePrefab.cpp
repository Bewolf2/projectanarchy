/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Editor/vForge/EditorPlugins/VisionPlugin/VisionManaged/VisionManagedPCH.h>
#include <Vision/Editor/vForge/EditorPlugins/VisionPlugin/VisionManaged/EngineInstances/EngineInstancePrefab.hpp>

using namespace VisionManaged;
using namespace System::Diagnostics;

EngineInstancePrefab::EngineInstancePrefab(void)
{
  m_bExport = false;
  m_instance = (VPrefabInstance *)Vision::Game.CreateEntity("VPrefabInstance", hkvVec3(0,0,0));
  m_instance->AddRef();
}

void EngineInstancePrefab::DisposeObject()
{
  V_SAFE_DISPOSEANDRELEASE(m_instance);
}

void EngineInstancePrefab::TraceShape(Shape3D ^ownerShape, Vector3F rayStart,Vector3F rayEnd, ShapeTraceResult ^%result)
{
}

bool EngineInstancePrefab::GetLocalBoundingBox(BoundingBox ^%bbox)
{
 return false;
}

bool EngineInstancePrefab::OnExport(SceneExportInfo ^info)
{
  if(m_bExport)
  {
    Debug::Assert( m_instance != nullptr );
    VArchive &ar = *((VArchive *)info->NativeShapeArchivePtr.ToPointer());
    ar << m_instance;
  }
  return true;
}

void EngineInstancePrefab::SetExportInstance(bool exportInstance)
{
  m_bExport = exportInstance;
}

bool EngineInstancePrefab::GetExportInstance()
{
  return m_bExport;
}

void EngineInstancePrefab::SetFilename(String ^filename)
{
  VString vFilename;
  if(filename && filename != "")
  {
    String^ bin_filename = nullptr;

    int iDot = filename->LastIndexOf(".");
    if (iDot >= 0)
      bin_filename = filename->Substring(0, iDot);
    else
      bin_filename = (String^) filename->Clone();

    bin_filename += ".vprefab";
    ConversionUtils::StringToVString(bin_filename,vFilename);
    m_instance->SetFileName(vFilename);
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
