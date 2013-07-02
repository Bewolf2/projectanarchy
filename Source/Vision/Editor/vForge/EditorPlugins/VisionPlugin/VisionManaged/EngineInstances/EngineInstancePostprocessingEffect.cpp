/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Editor/vForge/EditorPlugins/VisionPlugin/VisionManaged/VisionManagedPCH.h>
#include <Vision/Editor/vForge/EditorPlugins/VisionPlugin/VisionManaged/EngineInstances/EngineInstancePostprocessingEffect.hpp>

#using <mscorlib.dll>
using namespace ManagedFramework;
using namespace System::Diagnostics;
using namespace CSharpFramework::Serialization;


namespace VisionManaged
{

  #define SAFE_RELEASE_AND_DISPOSE(p) {if (p) {p->Dispose();p->Release();p=NULL;}}

  EngineInstancePostProcessingEffect::EngineInstancePostProcessingEffect()
  {
    //m_pEffect = nullptr;
    //m_bActivated = false;
  }

  void EngineInstancePostProcessingEffect::DisposeObject()
  {
    //Deactivate();
    //SAFE_RELEASE_AND_DISPOSE(m_pEffect);
  }

  bool EngineInstancePostProcessingEffect::OnExport(SceneExportInfo ^info)
  {
    //VArchive &ar = *((VArchive *)info->NativeShapeArchivePtr.ToPointer());
    //ar << m_pEffect;
    return true;
  }

  bool EngineInstancePostProcessingEffect::SetClassName(String ^className)
  {
    //SAFE_RELEASE_AND_DISPOSE(m_pEffect);
    //VString sClass;
    //ConversionUtils::StringToVString(className,sClass);
    //VPostProcessingTemplateBase *pTemplate = VPostProcessingManager::GlobalManager().CreatePostProcessingTemplate(sClass);
    //if (!pTemplate)
    //  return false;

    //m_pEffect = new VPostProcessingEffect(pTemplate);
    //m_pEffect->AddRef();

    //if (m_bActivated) // the engine instance has to remember
    //  Activate();

    return true;
  }

  void EngineInstancePostProcessingEffect::SetKey(String ^sKey)
  {
    //VString szKey;
    //ConversionUtils::StringToVString(sKey,szKey);
    //if (m_pEffect)
    //  m_pEffect->SetKey(szKey.IsEmpty() ? NULL : szKey.AsChar());
  }


  void EngineInstancePostProcessingEffect::Activate()
  {
    //if (!m_pEffect)
    //  return;
    //VPostProcessingManager::GlobalManager().SetPostProcessingEffect(m_pEffect);
    //m_bActivated = true;
  }

  void EngineInstancePostProcessingEffect::Deactivate()
  {
    //if (m_bActivated)
    //  VPostProcessingManager::GlobalManager().SetPostProcessingEffect(NULL);
    //m_bActivated = false;
  }

  void EngineInstancePostProcessingEffect::DeactivateAll()
  {
    //VPostProcessingManager::GlobalManager().SetPostProcessingEffect(NULL);
  }

  

  DynamicPropertyCollectionType ^ EngineInstancePostProcessingEffect::GetClassPropertyType(DynamicPropertyManager ^manager, String ^className)
  {
   // // the collection type is named like the effect template (although we actually get properties
   // // of its param block)
   // VString szTemplateClass;
   // ConversionUtils::StringToVString(className, szTemplateClass);
   // if (szTemplateClass.IsEmpty())
   //   return NULL;

   // // if already in the manager, return that one
   // DynamicPropertyCollectionType ^pClassProperties = manager->GetCollectionType(szTemplateClass);
   // if (pClassProperties)
   //   return pClassProperties;

   // VPostProcessingTemplateBase *pTemplate = VPostProcessingManager::GlobalManager().CreatePostProcessingTemplate(szTemplateClass);
   // if (!pTemplate)
   //   return NULL; // should not happen though

   // // create a parameter block to access its properties
   // VPostProcessingParamBlock *pParams = pTemplate->CreateParamBlock();
   // if (!pParams)
   //   return NULL;

   // // build a new property type from the param block's variable list
   // pClassProperties = manager->CreateNewType(szTemplateClass, DynamicPropertyCollectionType::DynamicPropertyCollectionFlags_e::None);
   // VARIABLE_ELEM *elem = pParams->GetVariableList()->first;
	  //while (elem)
	  //{
		 // VisVariable_cl *var = (VisVariable_cl *) elem->value;
   //   DynamicPropertyType ^pPropType = ConversionUtils::ConvertVariable(var);
   //   if (!pPropType)
   //     continue;

   //   pClassProperties->Add(pPropType);

		 // elem = elem->next;
	  //}

   // delete pParams;
   // manager->AddCollectionType(pClassProperties);
   // return pClassProperties;
    return nullptr;
  }


  void EngineInstancePostProcessingEffect::SetVariable(String ^varName, String ^varValue)
  {
    //if (!m_pEffect || !m_pEffect->GetParamBlock())
    //  return;

    //VString sVar, sValue;
    //ConversionUtils::StringToVString(varName,sVar);
    //ConversionUtils::StringToVString(varValue,sValue);
    //m_pEffect->GetParamBlock()->SetVariable(sVar, sValue);
    //m_pEffect->GetTemplate()->OnEffectParameterChanged(m_pEffect, sVar);
  }

   
  void EngineInstancePostProcessingEffect::SetAllVariables(DynamicPropertyCollection ^pProperties)
  {
    //if (!m_pEffect || !m_pEffect->GetParamBlock() || !pProperties)
    //  return;

    //for (int i=0;i<pProperties->get_Properties()->Count;i++)
    //{
    //  DynamicProperty ^pProp =  static_cast<DynamicProperty ^>(pProperties->get_Properties()->GetAt(i));
    //  VString sVar, sValue;
    //  ConversionUtils::StringToVString(pProp->get_PropertyType()->get_Name(),sVar);
    //  ConversionUtils::StringToVString(SerializationHelper::GetEngineStringFromObject(pProp->get_Value()),sValue);
    //  m_pEffect->GetParamBlock()->SetVariable(sVar, sValue);
    //}

    //m_pEffect->GetTemplate()->OnEffectParameterChanged(m_pEffect, NULL);

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
