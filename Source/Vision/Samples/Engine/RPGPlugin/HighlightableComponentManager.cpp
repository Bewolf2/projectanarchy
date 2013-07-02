/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#include <Vision/Samples/Engine/RPGPlugin/Precompiled.h>

#include <Vision/Samples/Engine/RPGPlugin/HighlightableComponentManager.h>

int const kHighlightableTraceBitmask = 2;

void RPG_HighlightableComponentManager::OneTimeInit()
{
  Vision::Callbacks.OnWorldInit += this;
  Vision::Callbacks.OnWorldDeInit += this;
  Vision::Callbacks.OnRenderHook += this;
  Vision::Callbacks.OnReassignShaders += this;
  Vision::Callbacks.OnUpdateSceneFinished += this;
}

void RPG_HighlightableComponentManager::OneTimeDeInit()
{
  Vision::Callbacks.OnRenderHook -= this;
  Vision::Callbacks.OnWorldDeInit -= this;
  Vision::Callbacks.OnWorldInit -= this;
  Vision::Callbacks.OnReassignShaders -= this;
  Vision::Callbacks.OnUpdateSceneFinished -= this;

  VASSERT(GetHighlightables().Count() == 0);
}

RPG_HighlightableComponentCollection& RPG_HighlightableComponentManager::GetHighlightables()
{
  return m_highlightableComponents;
}

RPG_HighlightableComponentCollection const& RPG_HighlightableComponentManager::GetHighlightables() const
{
  return m_highlightableComponents;
}

void RPG_HighlightableComponentManager::InitShaders()
{
  if(Vision::Shaders.LoadShaderLibrary("\\Shaders\\RPG.ShaderLib"))
  {
    m_xrayTechnique = Vision::Shaders.CreateTechnique("EntityXRay", NULL);
    if(m_xrayTechnique)
    {
      InitShader(m_xrayTechnique, m_xrayPassColorRegisters);
    }

    m_highlightTechnique = Vision::Shaders.CreateTechnique("EntityHighlight", NULL);
    if(m_highlightTechnique)
    {
      InitShader(m_highlightTechnique, m_highlightPassColorRegisters);
    }
  }
}

void RPG_HighlightableComponentManager::InitShader(VCompiledTechnique *shader, DynArray_cl<int>& regsOut)
{
  regsOut.Resize(shader->GetShaderCount());
  for(int i = 0; i < shader->GetShaderCount(); i++)
  {
    VCompiledShaderPass *const shaderPass = shader->GetShader(i);
    regsOut[i] = shader->GetShader(i)->GetConstantBuffer(VSS_VertexShader)->GetRegisterByName("HighlightColor");
  }
}

void RPG_HighlightableComponentManager::UpdateHighlightableEntities(RPG_HighlightableComponentCollection& highlightables)
{
  float time = Vision::GetTimer()->GetTime();
  for(int i = 0; i < highlightables.Count(); i++)
  {
    RPG_HighlightableComponent *comp = highlightables.GetAt(i);    
    comp->Update(time);  
  }
}

void RPG_HighlightableComponentManager::RenderHighlightableEntities(VCompiledTechnique *shader, DynArray_cl<int> const& regs, RPG_HighlightableComponentCollection const& highlightables)
{
  for(int i = 0; i < highlightables.Count(); i++)
  {
    RPG_HighlightableComponent const *const comp = highlightables.GetAt(i);

    VisBaseEntity_cl *const ent = static_cast<VisBaseEntity_cl *>(comp->GetOwner());

    // KGC demo hack (?)
    if(!ent->GetAnimConfig() || ent->GetAnimConfig()->GetSkinningMeshBuffer() == NULL)
      continue;

    VColorRef color = comp->GetColor();
    hkvVec4 const color4 = color.getAsVec4(); //(0.15f, 0.15f, 0.15f, 1.0f);

    for(int j = 0; j < shader->GetShaderCount(); j++)
    {
      VCompiledShaderPass *const shaderPass = shader->GetShader(j);
      if(regs[j] >= 0)
      {
        shaderPass->GetConstantBuffer(VSS_VertexShader)->SetSingleRegisterF(regs[j], color4.data);
        shaderPass->m_bModified = true;
      }
    }

    Vision::RenderLoopHelper.RenderEntityWithShaders(ent, shader->GetShaderCount(), shader->GetShaderList());
  }
}

void RPG_HighlightableComponentManager::OnHandleCallback(IVisCallbackDataObject_cl *data)
{
  if(data->m_pSender == &Vision::Callbacks.OnWorldInit)
  {
    InitShaders();
  }
  else if(data->m_pSender == &Vision::Callbacks.OnWorldDeInit)
  {
    m_xrayTechnique = NULL;
    m_highlightTechnique = NULL;
  }
  else if(data->m_pSender == &Vision::Callbacks.OnReassignShaders)
  {
    InitShaders();
  }
  else if(data->m_pSender == &Vision::Callbacks.OnRenderHook)
  {
    VisRenderHookDataObject_cl const *const renderHookData = static_cast<VisRenderHookDataObject_cl *>(data);
    switch(renderHookData->m_iEntryConst)
    {
      // XRay renders before opaque entities
    case VRH_PRE_PRIMARY_OPAQUE_PASS_ENTITIES:
      {
        if(m_xrayTechnique)
        {
          RenderHighlightableEntities(m_xrayTechnique, m_xrayPassColorRegisters, GetHighlightables());
        }
      }
      break;

      // Highlight renders after opaque entities
    case VRH_PRE_OCCLUSION_TESTS:
      {
        if(m_highlightTechnique)
        {
          RenderHighlightableEntities(m_highlightTechnique, m_highlightPassColorRegisters, GetHighlightables());
        }
      }
      break;

    default:
      break;
    }
  }
  else if (data->m_pSender == &Vision::Callbacks.OnUpdateSceneFinished)
  {
    UpdateHighlightableEntities(GetHighlightables());
  }
}

int RPG_HighlightableComponentManager::GetTraceBitmask() const
{
  return kHighlightableTraceBitmask;
}

RPG_HighlightableComponentManager RPG_HighlightableComponentManager::s_instance;

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
