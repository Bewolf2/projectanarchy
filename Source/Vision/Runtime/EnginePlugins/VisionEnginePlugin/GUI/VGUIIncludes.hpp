/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VGUIIncludes.hpp

#ifndef  VISION_GUI_INCLUDES_HPP
#define  VISION_GUI_INCLUDES_HPP

//This file includes the macros and headers that you need for using the GUI classes

struct VItemRenderInfo;

#ifdef WIN32
  #ifdef GUI_ENGINEPLUGIN_EXPORTS
    #define GUI_IMPEXP      __declspec(dllexport)
    #define GUI_IMPEXP_DATA __declspec(dllexport)
  #elif defined (GUI_ENGINEPLUGIN_IMPORTS)
    #define GUI_IMPEXP      __declspec(dllimport)
    #define GUI_IMPEXP_DATA __declspec(dllimport)
  #else
    #define GUI_IMPEXP      
    #define GUI_IMPEXP_DATA 
  #endif
#else
    #define GUI_IMPEXP
    #define GUI_IMPEXP_DATA 
#endif

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/GUI/VGUIManager.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/GUI/VGUIContext.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/GUI/VCursor.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/GUI/VDialog.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/GUI/Controls/VControlHelperObjects.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/GUI/Controls/VDialogStandardControls.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/GUI/VDialogResource.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/GUI/IVMenuSystemResource.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/GUI/IVRenderableItem.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/GUI/VDialogFrame.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/GUI/VDlgControlBase.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/GUI/VWindowBase.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/GUI/VTooltip.hpp>

// specialized:
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/GUI/Controls/VListControls.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/GUI/Controls/VMapLookupControl.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/GUI/Controls/VSliderControl.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/GUI/Controls/VTextControl.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/GUI/Controls/VModelPreviewControl.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/GUI/Controls/VItemContainer.hpp>

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
