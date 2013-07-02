/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VScriptInput_wrapper.hpp

#ifndef __VSCRIPTCONSTANTS_WRAPPER_HPP
#define __VSCRIPTCONSTANTS_WRAPPER_HPP

  #ifndef VLUA_APIDOC

  #include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/GUI/VMenuIncludes.hpp>

    #ifdef SWIG
      %immutable;
    #endif

    static const int LIGHT_POINT    = VIS_LIGHT_POINT;
    static const int LIGHT_DIRECTED = VIS_LIGHT_DIRECTED;
    static const int LIGHT_SPOT     = VIS_LIGHT_SPOTLIGHT;

    static const int CORONA_NO_SCALE       = VIS_CORONASCALE_NONE;
    static const int CORONA_DISTANCE_SCALE = VIS_CORONASCALE_DISTANCE;
    static const int CORONA_AREA_SCALE     = VIS_CORONASCALE_VISIBLEAREA;
    static const int CORONA_FADEOUT_SCALE  = VIS_CORONASCALE_USEFADEOUT;
    static const int CORONA_ROTATING_SCALE = VIS_CORONASCALE_ROTATING;

    static const int GUI_STATE_DISABLED  = VWindowBase::DISABLED;
    static const int GUI_STATE_NORMAL    = VWindowBase::NORMAL;
    static const int GUI_STATE_MOUSEOVER = VWindowBase::MOUSEOVER;
    static const int GUI_STATE_SELECTED  = VWindowBase::SELECTED;

    static const int GUI_ALIGN_ABSOLUTE       = VDialog::AbsolutePos;
    static const int GUI_ALIGN_CENTER         = VDialog::ScreenCenter;
    static const int GUI_ALIGN_PARENT         = VDialog::ParentPos;
    static const int GUI_ALIGN_PARENT_CENTER  = VDialog::ParentCenter;

    static const int TRACE_IGNORE       = VIS_TRACEACC_NOCOLLISION;
    static const int TRACE_AABOX        = VIS_TRACEACC_AABOX;
    static const int TRACE_OBOX         = VIS_TRACEACC_OBOX;
    static const int TRACE_SPHERE       = VIS_TRACEACC_SPHERE;
    static const int TRACE_POLYGON      = VIS_TRACEACC_POLYGONS;

    static const int EDITOR_NONE        = VisEditorManager_cl::EDITORMODE_NONE;
    static const int EDITOR_ANIMATING   = VisEditorManager_cl::EDITORMODE_ANIMATING;
    static const int EDITOR_RUN         = VisEditorManager_cl::EDITORMODE_PLAYING_IN_EDITOR;
    static const int EDITOR_PLAY        = VisEditorManager_cl::EDITORMODE_PLAYING_IN_GAME;

    static const int   VRAND_MAX        = RAND_MAX;
    static const float VRAND_MAX_INV    = (1.f/(float)RAND_MAX);
    static const int   VRAND_MAX_2      = RAND_MAX>>1;

    static const int TEXTURE_TYPE_DIFFUSE    = VisSurfaceTextures_cl::VTT_Diffuse;
    static const int TEXTURE_TYPE_NORMAL_MAP = VisSurfaceTextures_cl::VTT_NormalMap;
    static const int TEXTURE_TYPE_SPEC_MAP   = VisSurfaceTextures_cl::VTT_SpecularMap;

    static const int BLEND_OPAQUE           = VIS_TRANSP_NONE;
    static const int BLEND_MULTIPLY         = VIS_TRANSP_MULTIPLICATIVE;
    static const int BLEND_ADDITIVE         = VIS_TRANSP_ADDITIVE;
    static const int BLEND_ADDITIVE_NOALPHA = VIS_TRANSP_ADDITIVE_NOALPHA;
    static const int BLEND_ALPHA            = VIS_TRANSP_ALPHA;
    static const int BLEND_ALPHA_TEST       = VIS_TRANSP_ALPHATEST;
    static const int BLEND_ADD_MODULATE     = VIS_TRANSP_ADD_MODULATE;

    static const float MATH_SMALL_EPSILON     = HKVMATH_SMALL_EPSILON;
    static const float MATH_DEFAULT_EPSILON   = HKVMATH_DEFAULT_EPSILON;
    static const float MATH_LARGE_EPSILON     = HKVMATH_LARGE_EPSILON;
    static const float MATH_HUGE_EPSILON      = HKVMATH_HUGE_EPSILON;
    static const float MATH_GIGANTIC_EPSILON  = HKVMATH_GIGANTIC_EPSILON;


    #ifdef SWIG
      %mutable;
    #endif

  #else

    /// \brief Predefined white color.
    VColorRef  V_RGBA_WHITE;
    /// \brief Predefined grey color.
    VColorRef  V_RGBA_GREY;
    /// \brief Predefined black color.
    VColorRef  V_RGBA_BLACK;
    /// \brief Predefined red color.
    VColorRef  V_RGBA_RED;
    /// \brief Predefined yellow color.
    VColorRef  V_RGBA_YELLOW;
    /// \brief Predefined green color.
    VColorRef  V_RGBA_GREEN;
    /// \brief Predefined cyan color.
    VColorRef  V_RGBA_CYAN;
    /// \brief Predefined blue color.
    VColorRef  V_RGBA_BLUE;
    /// \brief Predefined purple color.
    VColorRef  V_RGBA_PURPLE;

  #endif

#endif //__VSCRIPTCONSTANTS_WRAPPER_HPP

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
