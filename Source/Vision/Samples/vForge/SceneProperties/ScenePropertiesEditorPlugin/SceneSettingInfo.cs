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

namespace ScenePropertiesPlugin
{
  /// <summary>
  /// Simple class representing some additional scene info
  /// </summary>
  ///
  [Serializable]
  public class SceneSettingInfo
  {
    public static readonly int    MIN_DIFFICULTY = 0;
    public static readonly int    MAX_DIFFICULTY = 10;

    public static readonly int    MIN_RESPAWN_TIME = 1;
    public static readonly int    MAX_RESPAWN_TIME = 60;
    
    public static readonly int    MIN_RESPAWN_RATE = 1;
    public static readonly int    MAX_RESPAWN_RATE = 100;

    public static readonly string DEFAULT_DESCRIPTION = "<enter description here>";

    private int     _difficulty   = MIN_DIFFICULTY;
    private int     _respawn_time = MIN_RESPAWN_TIME;
    private int     _respawn_rate = MIN_RESPAWN_RATE;
    private string  _description  = DEFAULT_DESCRIPTION;

    public SceneSettingInfo()
    {
    }

    public SceneSettingInfo(int     defaultDifficulty, 
                            int     defaultRespawnTime, 
                            int     defaultRespawnRate, 
                            string  description)
    {
        this._difficulty    = defaultDifficulty;
        ClampValues();
        this._respawn_time  = defaultRespawnTime;
        this._respawn_rate  = defaultRespawnRate;
        this._description   = description;
    }

    public int Difficulty
    {
      get { return _difficulty; }
      set { this._difficulty = value; ClampValues(); }
    }

    public int RespawnTime
    {
      get { return _respawn_time; }
      set { this._respawn_time = value; ClampValues(); }
    }

    public int RespawnRate
    {
      get { return _respawn_rate; }
      set { this._respawn_rate = value; ClampValues(); }
    }

    public string Description
    {
      get { return _description; }
      set { this._description = value; ClampValues(); }
    }

    private void ClampValues()
    {
      ClampValue(ref this._difficulty, MAX_DIFFICULTY, MIN_DIFFICULTY);
      ClampValue(ref this._respawn_rate, MAX_RESPAWN_RATE, MIN_RESPAWN_RATE);
      ClampValue(ref this._respawn_time, MAX_RESPAWN_TIME, MIN_RESPAWN_TIME);
    }

    private void ClampValue(ref int value, int MAX, int MIN)
    {
      if (value > MAX) value = MAX;
      else if (value < MIN) value = MIN;
    }
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
