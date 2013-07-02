/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

using System;
using System.Collections.Generic;
using System.Text;

namespace Editor
{
    public delegate void SetTextDelegate(string text);
    public delegate bool LoadFileDelegate(string path, bool prompt);

    /// <summary>
    /// This class is aimed at containing a set of delegates. 
    /// These delegates can be called by EditorApp to invoke some functions implemented in the main form. 
    /// 
    /// This allow to use vForge in any Form container, without the need for an interface, and letting the user choosing the custom method to call for such features. 
    /// </summary>
    public class EditorAppDelegates
    {
        /// <summary>
        /// Delegate called when project or scene name has changed. Used basically to set the mainform text. 
        /// </summary>
        public SetTextDelegate SetFormTextDelegate;

        /// <summary>
        /// Delegate called when a Project file (.project) is dropped on the Vision EnginePanel. Used basically to ask vForge to load this project.
        /// </summary>
        public LoadFileDelegate LoadProjectDelegate;

        /// <summary>
        /// Delegate called when a Scene file (.scene) is dropped on the Vision EnginePanel. Used basically to ask vForge to load this scene.
        /// </summary>
        public LoadFileDelegate LoadSceneDelegate;
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
