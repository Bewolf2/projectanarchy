/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file IVRenderableItem.hpp

#ifndef IVRENDERABLEITEM_HPP_INCLUDED
#define IVRENDERABLEITEM_HPP_INCLUDED


#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/GUI/VGUIManager.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/GUI/VGUICommandBuffer.hpp>
struct VGraphicsInfo;

/// \brief
///   Base class for a renderable item, that can be optionally created from an XML template
class IVRenderableItem
{
public:
  virtual ~IVRenderableItem(){}

  /// \brief
  ///   Pure virtual function that is called to render this item
  GUI_IMPEXP virtual void OnPaint(VGraphicsInfo &Graphics, const VItemRenderInfo &parentState) = 0;

  /// \brief
  ///   Virtual function that is called every frame by the GUI system
  GUI_IMPEXP virtual void OnTick(float dtime) {}

  /// \brief
  ///   Is called to build this item from an XML template node
  GUI_IMPEXP virtual bool Build(TiXmlElement *pNode, const char *szPath, bool bWrite=false) {return pNode!=NULL;}

  /// \brief
  ///   Is called after Build has been successfully finished
  GUI_IMPEXP virtual void OnBuildFinished() {}
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
