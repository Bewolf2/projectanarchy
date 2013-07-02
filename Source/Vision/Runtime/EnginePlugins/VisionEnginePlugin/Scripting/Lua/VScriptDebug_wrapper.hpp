/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VScriptInput_wrapper.hpp

#ifndef __VSCRIPTDEBUG_WRAPPER_HPP
#define __VSCRIPTDEBUG_WRAPPER_HPP

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/VScriptDraw_wrapper.hpp>

/// 
/// \brief
///   Small wrapper to unify debugging inside the scripting binding
///
class VScriptDebug_wrapper
{
private:
  bool m_bEnabled;
public:

#ifdef HK_DEBUG_SLOW
  VScriptDebug_wrapper() : m_bEnabled(true) {}
#else
  VScriptDebug_wrapper()
  {
    //enable debug switch in vForge
    m_bEnabled = (Vision::IsInitialized() && Vision::Editor.IsInEditor());
  }
#endif

  ~VScriptDebug_wrapper() {}

  // public Member
  VScriptDraw_wrapper Draw;

  inline void Enable(bool bEnable) { m_bEnabled = bEnable; Draw.Enable(bEnable); }
  inline bool IsEnabled() { return m_bEnabled; }

  inline void PrintLine(const char * szLine, VColorRef color = V_RGBA_WHITE)
  {
    if(m_bEnabled)
      Vision::Message.Add(szLine, 1, color);
  }

  inline void ClearLines()
  {
    if(m_bEnabled)
      Vision::Message.reset();
  }

  inline void PrintAt(float x, float y, const char * szLine, VColorRef color = V_RGBA_WHITE, const char * szFont=NULL)
  {
    if(m_bEnabled)
    {
      if(szFont!=NULL)
      {
        VisFont_cl *pFont = Vision::Fonts.LoadFont(szFont);
        
        if(pFont!=NULL)
        {
          pFont->PrintText(NULL, hkvVec2(x,y),szLine, color);
          return;
        }
      }

      Vision::Message.SetTextColor(color);
      Vision::Message.Print(1, (int)x, (int)y, szLine);
    }
  }

  inline void PrintAt(const hkvVec3* pPos, const char * szLine, VColorRef color = V_RGBA_WHITE, const char * szFont=NULL)
  {
    if(m_bEnabled && pPos!=NULL)
    {
      if(szFont!=NULL)
      {
        float x,y;
        if(Vision::Contexts.GetCurrentContext()->Project2D(*pPos,x,y))
        {
          VisFont_cl *pFont = Vision::Fonts.LoadFont(szFont);

          if(pFont!=NULL)
          {
            pFont->PrintText(NULL, hkvVec2(x,y),szLine, color);
            return;
          }
        }
      }

      Vision::Message.SetTextColor(color);
      Vision::Message.DrawMessage3D(szLine, *pPos, 1);
    }
  }

  inline void SetupLines(int iLines, float fDuration = 2.f)
  {
    if(m_bEnabled)
    {
      Vision::Message.SetMaxNum(iLines);
      Vision::Message.SetDuration(fDuration);
    }
  }

  inline void Log(const char * szLogMsg)
  {
    Vision::Error.SystemMessage(szLogMsg);
  }

  inline void Error(const char * szError)
  {
    Vision::Error.FatalError(szError);
  }
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
