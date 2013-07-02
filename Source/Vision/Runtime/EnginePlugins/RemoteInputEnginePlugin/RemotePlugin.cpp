/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/RemoteInputEnginePlugin/vRemoteInputPlugin.hpp>
#include <Vision/Runtime/EnginePlugins/RemoteInputEnginePlugin/mongoose.h>
#include <Vision/Runtime/Base/Container/VMaps.hpp>
#include <Vision/Runtime/EnginePlugins/RemoteInputEnginePlugin/IVRemoteInput.hpp>
#ifndef _VISION_WINRT
#include <WinSock.h>
#else
#include <WinSock2.h>
#endif

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/VScriptManager.hpp>

#define OFFSET 25
#define EXPECTED_UPDATE_RATE 0.040f
//#define USE_LINEAR_PREDICTION

class VRemoteInput : public IVRemoteInput, IVisCallbackHandler_cl
{
public:
  enum VTouchType_e {
    VTT_INVALID = 0,
    VTT_MOVE = 1,
    VTT_START = 2,
    VTT_END = 3,
    VTT_ORIENTATION = 4,
    VTT_ACCELERATION = 5,
    VTT_INIT = 6
  };

private:
  struct TouchPoint {
    int64 iId;
    bool used;
    int iRemappedId;
    float fLastPosX, fLastPosY;
    float fNextPosX, fNextPosY;
#ifdef USE_LINEAR_PREDICTION
    float fSendNextPos;
#else
    float fPosX,fPosY;
#endif
  };

  struct TouchInfo {
    TouchInfo() : iRemappedId(-1), iAbsX(-1), iAbsY(-1), TouchType(VTT_INVALID) {}
    TouchInfo(int iRemappedId, int iIndex, int iAbsX,int iAbsY,VTouchType_e TouchType) : iRemappedId(iRemappedId), iIndex(iIndex), iAbsX(iAbsX), iAbsY(iAbsY), TouchType(TouchType) {}
    TouchInfo(int iRemappedId,VTouchType_e TouchType) : iRemappedId(iRemappedId), iAbsX(0), iAbsY(0), TouchType(TouchType) {}
    TouchInfo(VTouchType_e TouchType, float fX, float fY, float fZ) : fX(fX), fY(fY), fZ(fZ), TouchType(TouchType) {}
    
    union {
      struct {
        int iRemappedId;
        int iAbsX;
        int iAbsY;
        int iIndex;
      };
      struct {
        float fX,fY,fZ;
      };
    };

    VTouchType_e TouchType;
  };

  struct VariableInfo {
    VariableInfo() : bNeedsUpdate(false) {}
    VariableInfo(const char* varName, const char* varValue) : sVarName(varName), sVarValue(varValue), bNeedsUpdate(true) {}

    VString sVarName;
    VString sVarValue;
    bool bNeedsUpdate;
  };

  enum { NUMBER_TOUCH_POINTS = 10 };

  mg_context *m_ctx; //mongoose context
  TouchPoint m_touchPoints[NUMBER_TOUCH_POINTS];
  int m_iNextTouchId;
  VArray<TouchInfo> m_touchQueue;
  VMutex m_queueMutex, m_variableMutex;
  int m_iXRes, m_iYRes; //last known screen resolution
  VArray<VariableInfo> m_variables;
  VString m_sDataDirectory;
  float m_fTimeSinceLastUpdate;
  float m_fRealUpdateRate;
  float m_fAverageUpdateRate;
  float m_fIncomingWidth,m_fIncomingHeight;


  char m_szHostname[128];
  int m_iNumAddresses;
  in_addr m_hostaddesses[8];
  int m_iPort;
  bool m_bDisplayUrl;
  bool m_bResizeOnConnect;
  bool m_bSmoothTouchInput;
  bool m_bDisableMouseInput;

  VRemoteInput(const VRemoteInput& other); //no copy constructor
  void operator = (const VRemoteInput& other); //don't assign anything

public:



  VRemoteInput();

  virtual void OnInitEnginePlugin() HKV_OVERRIDE;
  virtual void OnDeInitEnginePlugin() HKV_OVERRIDE;

  virtual void OnHandleCallback(IVisCallbackDataObject_cl *pData) HKV_OVERRIDE;

  virtual const char *GetPluginName() HKV_OVERRIDE
  {
    return "vRemoteInput";  //must match DLL name
  }


  virtual void DebugDrawTouchAreas(VColorRef color) HKV_OVERRIDE; 

  virtual void DebugDrawTouchPoints(VColorRef color) HKV_OVERRIDE;

  virtual bool StartServer(const char* dataDirectory, int iPort = 8080) HKV_OVERRIDE;

  virtual void StopServer() HKV_OVERRIDE;

  virtual void InitEmulatedDevices() HKV_OVERRIDE;

  virtual void DeinitEmulatedDevices() HKV_OVERRIDE;

  virtual bool AddVariableDirect(const char* varName, const char* value) HKV_OVERRIDE;

  virtual bool AddVariable(const char* varName, float value) HKV_OVERRIDE;

  virtual bool AddVariable(const char* varName, int value) HKV_OVERRIDE;

  virtual bool AddVariable(const char* varName, const char* value ) HKV_OVERRIDE;

  virtual bool UpdateVariableDirect(const char* varName, const char* value) HKV_OVERRIDE;

  virtual bool UpdateVariable(const char* varName, float value) HKV_OVERRIDE;

  virtual bool UpdateVariable(const char* varName, int value) HKV_OVERRIDE;

  virtual bool UpdateVariable(const char* varName, const char* value) HKV_OVERRIDE;
   
  virtual bool RemoveVariable(const char* varName) HKV_OVERRIDE;

  virtual void SetResizeOnConnect(bool bOn) HKV_OVERRIDE;

  virtual void SetSmoothTouchInput(bool bOn) HKV_OVERRIDE;

  virtual void SetDisableMouseInput(bool bOn) HKV_OVERRIDE;

  void* OnRequest(mg_event event, mg_connection *conn, const mg_request_info *request_info);
};

VRemoteInput g_RemoteInputPlugin;

// declare a module for the serialization
DECLARE_THIS_MODULE(g_vRemoteInputModule, MAKE_VERSION(1,0),
"Remote Input Plugin", "Havok",
"Module for the Havok binding", &g_RemoteInputPlugin);

//  Use this to get and initialize the plugin when you link statically
VEXPORT IVisPlugin_cl* GetEnginePlugin_vRemoteInput()
{
  return &g_RemoteInputPlugin;
}

#if (defined _DLL) || (defined _WINDLL)
//  The engine uses this to get and initialize the plugin dynamically
VEXPORT IVisPlugin_cl* GetEnginePlugin()
{
  return GetEnginePlugin_vRemoteInput();
}
#endif // _DLL or _WINDLL

VRemoteInput::VRemoteInput() :
m_iNextTouchId(0),
m_iXRes(0),
m_iYRes(0),
m_bDisplayUrl(false),
m_bResizeOnConnect(true),
m_iNumAddresses(0),
m_fTimeSinceLastUpdate(0.0f),
m_fRealUpdateRate(EXPECTED_UPDATE_RATE),
m_fAverageUpdateRate(EXPECTED_UPDATE_RATE),
m_bSmoothTouchInput(true),
m_bDisableMouseInput(true),
m_fIncomingWidth(1.0f),
m_fIncomingHeight(1.0f)
{
  for(int i=0;i<NUMBER_TOUCH_POINTS;i++)
  {
    m_touchPoints[i].used = false;
  }

  m_szHostname[0] = '\0';
  memset(m_hostaddesses,0,sizeof(m_hostaddesses));
}

//Forwards the callback to the global VRemoteInput instance
void* callback(mg_event event, mg_connection *conn, const mg_request_info *request_info)
{
  return g_RemoteInputPlugin.OnRequest(event,conn,request_info);
}

bool isValidFloat(float f)
{
  if (!hkvMath::isValidFloat (f)) // NaN, +/- Infinity
    return false;
  if((f > 1000.f) || (f < -1000.f))
    return false;
  return true;
}

void* VRemoteInput::OnRequest(mg_event event, mg_connection *conn, const mg_request_info *request_info) 
{
    if (event == MG_NEW_REQUEST) {
      //Vision::Error.SystemMessage(request_info->uri);
      if(strcmp(request_info->uri,"/") == 0){
        IVFileInStream* pFile = Vision::File.Open("\\RemoteInput\\touch.html");
        if(!pFile){
          mg_printf(conn, "HTTP/1.1 404\r\n"
                    "Content-Type: text/plain\r\n\r\n"
                    "File not found: RemoteInput\\touch.html");
          return "";
        }

        LONG uiFileSize = pFile->GetSize();
        VMemoryTempBuffer<4096> buffer(uiFileSize+1);
        pFile->Read(buffer.GetBuffer(), uiFileSize);
        ((char*)buffer.GetBuffer())[uiFileSize] = '\0';

        VString sHtml = (char*)buffer.GetBuffer();
        
        IVFileInStream* pUserGuiFile = Vision::File.Open(m_sDataDirectory + "\\gui.html");
        if(pUserGuiFile)
        {
          uiFileSize = pUserGuiFile->GetSize();
          VMemoryTempBuffer<4096> userGuiBuffer(uiFileSize+1);
          pUserGuiFile->Read(userGuiBuffer.GetBuffer(), uiFileSize);
          ((char*)userGuiBuffer.GetBuffer())[uiFileSize] = '\0';
          sHtml.Replace("<!-- USER GUI HTML -->",(char*)userGuiBuffer.GetBuffer(),true);
          pUserGuiFile->Close();
        }
          
        mg_printf(conn, "HTTP/1.1 200 OK\r\n"
          "Content-Type: text/html\r\n\r\n");
        mg_write(conn, sHtml.AsChar(), sHtml.GetSize() );
        pFile->Close();
      }
      else if(strstr(request_info->uri,"/static/") == request_info->uri)
      {

        //Vision::Error.SystemMessage("sending %s",request_info->uri);
        VString sFileName = "\\RemoteInput\\";
        sFileName += &request_info->uri[8];
        
        IVFileInStream* pFile = Vision::File.Open(sFileName);

        if(!pFile)
        {
          sFileName = m_sDataDirectory + "\\";
          sFileName += &request_info->uri[8];

          pFile = Vision::File.Open(sFileName);
        }

        if(!pFile)
        {
          mg_printf(conn, "HTTP/1.1 404\r\n"
            "Content-Type: text/plain\r\n\r\n"
            "static file not found: %s",sFileName.AsChar());
          return "";
        }

        if(!EndsWithI(request_info->uri,"jpg") &&
           !EndsWithI(request_info->uri,"gif") &&
           !EndsWithI(request_info->uri,"png")) 
        {
          mg_printf(conn, "HTTP/1.1 404\r\n"
            "Content-Type: text/plain\r\n\r\n"
            "unkown file ending: %s",sFileName.AsChar());
          return "";
        }

        const char* szFileEnding = VPathHelper::GetExtension(sFileName);
        if(strcmp(szFileEnding,"jpg") == 0){
          szFileEnding = "jpeg";
        }
        //Vision::Error.SystemMessage("File ending %s",szFileEnding);

        LONG uiFileSize = pFile->GetSize();
        VMemoryTempBuffer<4096> buffer(uiFileSize);
        pFile->Read(buffer.GetBuffer(), uiFileSize);
        mg_printf(conn, "HTTP/1.1 200 OK\r\n"
                        "Content-Type: image/%s\r\n\r\n", szFileEnding);
        mg_write(conn, buffer.GetBuffer(), uiFileSize );
        pFile->Close();
      }
      else if(strcmp(request_info->uri,"/event") == 0){
        char buffer[4096];
        int iReadBytes = mg_read(conn,buffer,sizeof(buffer)-1);
        //Vision::Error.SystemMessage("%d bytes content",iReadBytes);
        buffer[iReadBytes] = '\0';
        //Vision::Error.SystemMessage("content: %s",buffer);


        m_queueMutex.Lock();

        m_fRealUpdateRate = m_fTimeSinceLastUpdate;
        if(!isValidFloat(m_fRealUpdateRate))
          m_fRealUpdateRate = EXPECTED_UPDATE_RATE;
        m_fAverageUpdateRate = (m_fAverageUpdateRate * 0.95f) + (m_fRealUpdateRate * 0.05f);
        m_fTimeSinceLastUpdate = 0.0f;

        VStringTokenizerInPlace toc1(buffer,'|');
        while(char* pTouchPoint = toc1.Next())
        {
          VStringTokenizerInPlace toc2(pTouchPoint,',');
          if(toc2.GetTokenCount() == 4){
            int iTouchType = atoi(toc2.Next());
            if(iTouchType == VTT_ACCELERATION || iTouchType == VTT_ORIENTATION)
            {
              float fX = (float)atof(toc2.Next());
              float fY = (float)atof(toc2.Next());
              float fZ = (float)atof(toc2.Next());
              if(isValidFloat(fX) && isValidFloat(fY) && isValidFloat(fZ))
                m_touchQueue.Append(TouchInfo((VTouchType_e)iTouchType,fX,fY,fZ));
            }
            else {
              int64 iTouchId = _atoi64(toc2.Next());
              double fPosX = atof(toc2.Next()) / m_fIncomingWidth;
              double fPosY = atof(toc2.Next()) / m_fIncomingHeight;

              if(iTouchType == VTT_START){
                //search for a free place to insert the touch point into
                int iIndex = -1;
                for(int i=0;i<NUMBER_TOUCH_POINTS;i++)
                {
                  if(m_touchPoints[i].used == false){
                    iIndex = i;
                    break;
                  }
                }

                if(iIndex != -1)  //only insert start if we have a slot aviable
                {
                  TouchPoint& point = m_touchPoints[iIndex];
                  point.iId = iTouchId;
                  point.iRemappedId = m_iNextTouchId++;
                  point.used = true;

                  TouchInfo info(point.iRemappedId,
                    iIndex,
                    (int)(fPosX * Vision::Video.GetXRes()),
                    (int)(fPosY * Vision::Video.GetYRes()),
                    VTT_START);
                  m_touchQueue.Append(info);
                }

              }
              else {
                //search if the touch point does already exist
                int iIndex = -1;
                for(int i=0;i<NUMBER_TOUCH_POINTS;i++)
                {
                  if(m_touchPoints[i].used && m_touchPoints[i].iId == iTouchId){
                    iIndex = i;
                    break;
                  }
                }

                if(iIndex != -1) //only insert update if we have a slot aviable
                {
                  TouchPoint& point = m_touchPoints[iIndex];
                  if(iTouchType == VTT_END)
                  {
                    point.used = false;
                    m_touchQueue.Append(TouchInfo(point.iRemappedId,VTT_END));
                  }
                  else {
                    TouchInfo info(point.iRemappedId,
                      iIndex,
                      (int)(fPosX * Vision::Video.GetXRes()),
                      (int)(fPosY * Vision::Video.GetYRes()),
                      VTT_MOVE);

                    //search if there is already a update for this touchpoint
                    TouchInfo* last = NULL;
                    for(int i = 0; i < m_touchQueue.GetLength(); i++)
                    {
                      if(m_touchQueue[i].iRemappedId == info.iRemappedId){
                        last = &m_touchQueue[i];
                        break;
                      }
                    }

                    //update already existing entry
                    if(last != NULL){
                      last->iAbsX = info.iAbsX;
                      last->iAbsY = info.iAbsY;
                    }
                    else { //add new entry
                      m_touchQueue.Append(info);
                    }
                  }
                }
              }
            }
          }
        }
        m_queueMutex.Unlock();


        mg_printf(conn, "HTTP/1.1 200 OK\r\n"
                        "Content-Type: text/plain\r\n\r\n");
      }
      else if(strcmp(request_info->uri,"/init") == 0)
      {
        char buffer[128];
        int iReadBytes = mg_read(conn,buffer,sizeof(buffer)-1);
        buffer[iReadBytes] = '\0';

        Vision::Error.SystemMessage("remote device connected");

        m_queueMutex.Lock();

        VStringTokenizerInPlace toc(buffer,',');
        if(toc.GetTokenCount() == 2){
          int iWidth = atoi(toc.Next());
          int iHeight = atoi(toc.Next());

          m_fIncomingWidth = (float)iWidth;
          m_fIncomingHeight = (float)iHeight;

          //insert resize message into queue
          m_touchQueue.Append( TouchInfo(-1, -1, iWidth, iHeight, VTT_INIT) );
        }

        //Deactivate all active touchpoints
        for(int i=0; i < NUMBER_TOUCH_POINTS; i++)
        {
          TouchPoint& point = m_touchPoints[i];
          if(point.used)
          {
            m_touchQueue.Append( TouchInfo(point.iRemappedId, VTT_END) );
            point.used = false;
          }
        }

        m_bDisplayUrl = false;

        m_queueMutex.Unlock();
      }
      else if(strcmp(request_info->uri,"/update") == 0)
      {
        mg_printf(conn, "HTTP/1.1 200 OK\r\n"
          "Content-Type: text/plain\r\n\r\n");
        m_variableMutex.Lock();
        for(int i=0; i < m_variables.GetLength(); i++)
        {
          if(m_variables[i].bNeedsUpdate)
          {
            m_variables[i].bNeedsUpdate = false;
            mg_printf(conn,"variables['%s'] = %s;",m_variables[i].sVarName.AsChar(),m_variables[i].sVarValue.AsChar());
          }
        }
        m_variableMutex.Unlock();
      }
      else {
        mg_printf(conn, "HTTP/1.1 404\r\n"
          "Content-Type: text/plain\r\n\r\n"
          "not found %s", request_info->uri);
      }
      return "";  // Mark as processed
    } else {
      return NULL;
    }
}

void VRemoteInput::OnInitEnginePlugin()
{
  Vision::RegisterModule(&g_vRemoteInputModule);

  InitEmulatedDevices();

  Vision::Callbacks.OnFrameUpdatePostRender += this;
  Vision::Callbacks.OnVideoInitialized += this;
  Vision::Callbacks.OnVideoChanged += this;
  IVScriptManager::OnRegisterScriptFunctions += this;
}

void VRemoteInput::OnDeInitEnginePlugin()
{
  if(m_ctx != NULL)
  {
    mg_stop(m_ctx);
    m_ctx = NULL;
  }

  Vision::UnregisterModule(&g_vRemoteInputModule);

  IVScriptManager::OnRegisterScriptFunctions -= this;
  Vision::Callbacks.OnFrameUpdatePostRender -= this;
  Vision::Callbacks.OnVideoInitialized -= this;
  Vision::Callbacks.OnVideoChanged -= this;
}

extern "C" int luaopen_RemoteInput(lua_State *);

void VRemoteInput::OnHandleCallback(IVisCallbackDataObject_cl *pData)
{
  if(pData->m_pSender==&IVScriptManager::OnRegisterScriptFunctions)
  {
    IVScriptManager* pScriptManager = Vision::GetScriptManager();
    if (pScriptManager)
    {
      lua_State* pLuaState = static_cast<VScriptResourceManager*>(pScriptManager)->GetMasterState();
      if(pLuaState)
      {
        lua_getglobal(pLuaState, "RemoteInput");
        int iType = lua_type(pLuaState, -1);
        lua_pop(pLuaState, 1);

        if(iType!=LUA_TUSERDATA)
        {
          luaopen_RemoteInput(pLuaState);
          int iRetParams = LUA_CallStaticFunction(pLuaState,"RemoteInput","IVRemoteInput","Cast","v>v", this);
          if (iRetParams==1)
          {
            if(lua_isnil(pLuaState, -1))
            {
              lua_pop(pLuaState, iRetParams);
            }
            else
            {
              lua_setglobal(pLuaState, "RemoteInput");
              return;
            }
          }
        }
        else
        {
          return; //already loaded
        }
      }

      Vision::Error.Warning("Unable to create Lua Fmod Module, lua_State is NULL or cast failed!");
    }
    return;
  }
  else if(pData->m_pSender == &Vision::Callbacks.OnFrameUpdatePostRender)
  {
    if(m_bDisplayUrl)
    {
      for(int i=0; i < m_iNumAddresses; i++)
      {
        int iPosX = Vision::Video.GetXRes()-240;
#if defined( HK_ANARCHY )
        iPosX -= 64; // cater for the Anarchy watermark
#endif
        Vision::Message.SetTextColor(V_RGBA_WHITE);
        Vision::Message.Print(0, iPosX, 10 + (20 * i), 
          "Remote url: http://%s:%d", inet_ntoa(m_hostaddesses[i]), m_iPort);
      }
    }

    m_queueMutex.Lock();

    //Vision::Message.Print(0,20,200,"Real update rate %f\nAverage update rate %f",1.0f/m_fRealUpdateRate,1.0f/m_fAverageUpdateRate);

    m_fTimeSinceLastUpdate += Vision::GetTimer()->GetTimeDifference();

    VMultiTouchInputPC& multiTouch = VInputManager::GetMultitouch();
    VMotionInputPC& motion = VInputManager::GetMotionSensor();

    for(int i = 0; i < m_touchQueue.GetLength(); i++)
    {
      TouchInfo& info = m_touchQueue[i];
      switch(info.TouchType)
      {
        case VTT_MOVE:
          {
            //Vision::Error.SystemMessage("UpdateTouch %d %d %d %f", info.iRemappedId, info.iAbsX, info.iAbsY,Vision::GetTimer()->GetTime());
            if(m_bSmoothTouchInput)
            {
#ifdef USE_LINEAR_PREDICTION
              int iDiffX = (info.iAbsX - m_touchPoints[info.iIndex].iLastPosX) / 3;
              int iDiffY = (info.iAbsY - m_touchPoints[info.iIndex].iLastPosY) / 3;
              int iInterpolatedPosX = iDiffX + m_touchPoints[info.iIndex].iLastPosX;
              int iInterpolatedPosY = iDiffY + m_touchPoints[info.iIndex].iLastPosY;
              multiTouch.UpdateTouch(info.iRemappedId, iInterpolatedPosX, iInterpolatedPosY);
              m_touchPoints[info.iIndex].iLastPosX = info.iAbsX;
              m_touchPoints[info.iIndex].iLastPosY = info.iAbsY;
              m_touchPoints[info.iIndex].iNextPosX = info.iAbsX - iDiffX;
              m_touchPoints[info.iIndex].iNextPosY = info.iAbsY - iDiffY;
              m_touchPoints[info.iIndex].fSendNextPos = m_fAverageUpdateRate/3.f;
#else
              m_touchPoints[info.iIndex].fNextPosX = (float)info.iAbsX;
              m_touchPoints[info.iIndex].fNextPosY = (float)info.iAbsY;
#endif
            }
            else
            {
              multiTouch.UpdateTouch(info.iRemappedId,(float)info.iAbsX,(float)info.iAbsY);
            }
          }
          break;
        case VTT_START:
          //Vision::Error.SystemMessage("AddNewTouch %d %d %d", info.iRemappedId, info.iAbsX, info.iAbsY);
          multiTouch.AddNewTouch(info.iRemappedId, (float)info.iAbsX, (float)info.iAbsY);
          m_touchPoints[info.iIndex].fLastPosX = (float)info.iAbsX;
          m_touchPoints[info.iIndex].fLastPosY = (float)info.iAbsY;
#ifndef USE_LINEAR_PREDICTION
          m_touchPoints[info.iIndex].fNextPosX = (float)info.iAbsX;
          m_touchPoints[info.iIndex].fNextPosY = (float)info.iAbsY;
          m_touchPoints[info.iIndex].fPosX = info.iAbsX / (float)Vision::Video.GetXRes();
          m_touchPoints[info.iIndex].fPosY = info.iAbsY / (float)Vision::Video.GetYRes();
#else
          m_touchPoints[info.iIndex].fSendNextPos = 0.0f;
#endif  
          break;
        case VTT_END:
          //Vision::Error.SystemMessage("RemoveTouch %d", info.iRemappedId);
          multiTouch.RemoveTouch(info.iRemappedId);
          break;
        case VTT_INIT:
          if(m_bResizeOnConnect)
          {
            if(Vision::Editor.IsInEditor())
              Vision::Editor.Set3DCanvasSize(info.iAbsX,info.iAbsY);
            else
              Vision::Video.ChangeScreenResolution(info.iAbsX,info.iAbsY,false,0);
          }
          if(m_bDisableMouseInput)
          {
            V_PC_MOUSE.SetActive(false);
          }
          OnDeviceConnected.TriggerCallbacks(NULL);
          break;
        case VTT_ORIENTATION:
          motion.SetOrientation(info.fX,info.fY,info.fZ);
          break;
        case VTT_ACCELERATION:
          motion.SetAcceleration(info.fX / 9.81f,info.fY / 9.81f,info.fZ / 9.81f);
          break;
        default:
          VASSERT_MSG(0, "unexpected enum value");
      }
    }
    m_touchQueue.RemoveAll();

    // search for pending interpolated deltas
    if(m_bSmoothTouchInput)
    {
      for(int i = 0; i < NUMBER_TOUCH_POINTS; i++)
      {
        TouchPoint& point = m_touchPoints[i];
  #ifdef USE_LINEAR_PREDICTION
        if(point.used && point.fSendNextPos > 0.0f)
        {
          point.fSendNextPos -= Vision::GetTimer()->GetTimeDifference();
          if(point.fSendNextPos <= 0.0f)
          {
            //Vision::Error.SystemMessage("Interpolated update %d %d %d %f",point.iRemappedId,point.iNextPosX,point.iNextPosY,Vision::GetTimer()->GetTime());
            multiTouch.UpdateTouch(point.iRemappedId, point.iNextPosX, point.iNextPosY );
            //another interpolated coordinate?
            if(point.iNextPosX != point.iLastPosX || point.iNextPosY != point.iNextPosY){
              point.iNextPosX = point.iLastPosX;
              point.iNextPosY = point.iLastPosY;
              point.fSendNextPos = m_fAverageUpdateRate/3.0f;
            }
          }
        }
  #else
        if(point.used)
        {
          static const float fFactor = 400.0f;
          const float fScreenWidth = (float)Vision::Video.GetXRes();
          const float fScreenHeight = (float)Vision::Video.GetYRes();
          float fCurPosX = point.fPosX;
          float fCurPosY = point.fPosY;
          float fTargetPosX = point.fNextPosX / fScreenWidth;
          float fTargetPosY = point.fNextPosY / fScreenHeight;
          float fDirX = fTargetPosX - fCurPosX;
          float fDirY = fTargetPosY - fCurPosY;
          float fMultiplier = fFactor * Vision::GetTimer()->GetTimeDifference() * hkvMath::sqrt(fDirX * fDirX + fDirY * fDirY);
          fCurPosX += fDirX * fMultiplier;
          fCurPosY += fDirY * fMultiplier;
          point.fPosX = fCurPosX;
          point.fPosY = fCurPosY;
          multiTouch.UpdateTouch(point.iRemappedId,fCurPosX * fScreenWidth,fCurPosY * fScreenHeight);
        }
  #endif
      }
    }

    m_queueMutex.Unlock();
  }
  else if(pData->m_pSender == &Vision::Callbacks.OnVideoInitialized)
  {
    m_iXRes = Vision::Video.GetXRes();
    m_iYRes = Vision::Video.GetYRes();
  }
  else if(pData->m_pSender == &Vision::Callbacks.OnVideoChanged)
  {
    int iXRes = Vision::Video.GetXRes();
    int iYRes = Vision::Video.GetYRes();
    VInputManager::GetMultitouch().RecalculateAfterResize(m_iXRes,m_iYRes,iXRes,iYRes);
    m_iXRes = iXRes;
    m_iYRes = iYRes;
  }
}

void VRemoteInput::DebugDrawTouchAreas(VColorRef color)
{
  const VPListT<VTouchArea>& touchAreas = VInputManager::GetMultitouch().GetTouchAreas();
  for(int i=0; i < touchAreas.GetLength(); i++)
  {
    const VRectanglef& rect = touchAreas[i]->GetArea();
    Vision::Game.DrawSingleLine2D(rect.m_vMin.x, rect.m_vMin.y, rect.m_vMax.x, rect.m_vMin.y, color);
    Vision::Game.DrawSingleLine2D(rect.m_vMax.x, rect.m_vMin.y, rect.m_vMax.x, rect.m_vMax.y, color);
    Vision::Game.DrawSingleLine2D(rect.m_vMax.x, rect.m_vMax.y, rect.m_vMin.x, rect.m_vMax.y, color);
    Vision::Game.DrawSingleLine2D(rect.m_vMin.x, rect.m_vMax.y, rect.m_vMin.x, rect.m_vMin.y, color);
  }
}

void VRemoteInput::DebugDrawTouchPoints(VColorRef color)
{
  VMultiTouchInputPC& touchscreen = VInputManager::GetMultitouch();
  int count = touchscreen.GetMaximumNumberOfTouchPoints();
  for(int i = 0; i < count; i++)
  {
    if(!touchscreen.IsActiveTouch(i))
      continue;

    // Get touchpoint values and deltas
    float valueX = touchscreen.GetTouchPointValue(i, CT_TOUCH_ABS_X);
    float valueY = touchscreen.GetTouchPointValue(i, CT_TOUCH_ABS_Y);
    float diffX = touchscreen.GetTouchPointValue(i, CT_TOUCH_ABS_DELTA_X);
    float diffY = touchscreen.GetTouchPointValue(i, CT_TOUCH_ABS_DELTA_Y);

    // Draw crosshair
    Vision::Game.DrawSingleLine2D(valueX - OFFSET, valueY - OFFSET, valueX + OFFSET, valueY - OFFSET, color, 3.f);
    Vision::Game.DrawSingleLine2D(valueX - OFFSET, valueY + OFFSET, valueX + OFFSET, valueY + OFFSET, color, 3.f);
    Vision::Game.DrawSingleLine2D(valueX - OFFSET, valueY + OFFSET, valueX - OFFSET, valueY - OFFSET, color, 3.f);
    Vision::Game.DrawSingleLine2D(valueX + OFFSET, valueY + OFFSET, valueX + OFFSET, valueY - OFFSET, color, 3.f);
    Vision::Game.DrawSingleLine2D(valueX - OFFSET, valueY, valueX + OFFSET, valueY, color, 3.f);
    Vision::Game.DrawSingleLine2D(valueX, valueY + OFFSET, valueX, valueY - OFFSET, color, 3.f);

    // Draw delta vector
    Vision::Game.DrawSingleLine2D(valueX, valueY, valueX + diffX * 100.f, valueY + diffY * 100.f, color, 3.f);

    const int BufferSize = 30;
    char debugID[BufferSize];
    char debugX[BufferSize];
    char debugY[BufferSize];

    // Print ID & position for every active touchpoint          
    vis_snprintf(debugID, BufferSize, "ID: %i", i);
    Vision::Game.GetDebugRenderInterface()->DrawText2D(valueX + 30, valueY - 30, debugID, color);
    vis_snprintf(debugX, BufferSize, "X:  %.0f", valueX);
    Vision::Game.GetDebugRenderInterface()->DrawText2D(valueX + 30, valueY - 20, debugX, color);
    vis_snprintf(debugY, BufferSize, "Y:  %.0f", valueY);
    Vision::Game.GetDebugRenderInterface()->DrawText2D(valueX + 30, valueY - 10, debugY, color);
  }
}

void VRemoteInput::InitEmulatedDevices()
{
  VInputManager::GetMultitouch().Init(true);
  VInputManager::GetMultitouch().SetEnabled(true);
  VInputManager::GetMotionSensor().SetEnabled(true);
}

void VRemoteInput::DeinitEmulatedDevices()
{
  VInputManager::GetMultitouch().SetEnabled(false);
  VInputManager::GetMultitouch().DeInit();
  VInputManager::GetMotionSensor().SetEnabled(false);
}


bool VRemoteInput::StartServer(const char* dataDirectory, int iPort /* = 8080 */)
{
  if(m_ctx != NULL)
  {
      mg_stop(m_ctx);
      m_ctx = NULL;
  }

  m_sDataDirectory = dataDirectory;
  m_bDisplayUrl = true;

  char port[16];
  sprintf_s(port,sizeof(port),"%d",iPort);
  m_iPort = iPort;

  const char *options[] = {"listening_ports", port, "num_threads", "10", NULL};

  m_ctx = mg_start(&callback, NULL, options);
  if (m_ctx == NULL)
    return false;

  if (gethostname(m_szHostname, sizeof(m_szHostname)) == SOCKET_ERROR) {
    return 1;
  }
  else {
    hostent *phe = gethostbyname(m_szHostname);
    if( phe != NULL )
    {
      int i=0;
      for(; phe->h_addr_list[i] != 0 && i < 8;i++) 
      {
        memcpy(&m_hostaddesses[i], phe->h_addr_list[i], sizeof(in_addr));
      }
      m_iNumAddresses = i;
    }
  }

  return true;
}

void VRemoteInput::StopServer()
{
  if(m_ctx != NULL)
  {
    mg_stop(m_ctx);
    m_ctx = NULL;
  }
  m_bDisplayUrl = false;
}

bool VRemoteInput::AddVariableDirect(const char* varName, const char* value)
{
  m_variableMutex.Lock();
  for(int i=0; i < m_variables.GetLength(); i++)
  {
    if(m_variables[i].sVarName == varName){
      m_variableMutex.Unlock();
      return false;
    }
  }

  m_variables.Append(VariableInfo(varName,value));
  m_variableMutex.Unlock();
  return true;
}

bool VRemoteInput::AddVariable(const char* varName, float value)
{
  char sValue[32];
  sprintf_s(sValue,sizeof(sValue),"%f",value);
  return AddVariableDirect(varName,sValue);
}

bool VRemoteInput::AddVariable(const char* varName, int value)
{
  char sValue[32];
  sprintf_s(sValue,sizeof(sValue),"%d",value);
  return AddVariableDirect(varName,sValue);
}

bool VRemoteInput::AddVariable(const char* varName, const char* value)
{
  VString sValue = value;
  sValue.ReplaceAll("\\","\\\\");
  sValue.ReplaceAll("'","\\'");
  sValue.InsertAt(0,'\'');
  sValue.InsertAt(sValue.GetLen(),'\'');
  return AddVariableDirect(varName,sValue.AsChar());
}

bool VRemoteInput::UpdateVariableDirect(const char* varName, const char* value)
{
  m_variableMutex.Lock();
  for(int i=0; i < m_variables.GetLength(); i++)
  {
    if(m_variables[i].sVarName == varName)
    {
      m_variables[i].sVarValue = value;
      m_variables[i].bNeedsUpdate = true;
      m_variableMutex.Unlock();
      return true;
    }
  }
  m_variableMutex.Unlock();
  return false;
}

bool VRemoteInput::UpdateVariable(const char* varName, float value)
{
  char sValue[32];
  sprintf_s(sValue,sizeof(sValue),"%f",value);
  return UpdateVariableDirect(varName,sValue);
}

bool VRemoteInput::UpdateVariable(const char* varName, int value)
{
  char sValue[32];
  sprintf_s(sValue,sizeof(sValue),"%d",value);
  return UpdateVariableDirect(varName,sValue);
}

bool VRemoteInput::UpdateVariable(const char* varName, const char* value)
{
  VString sValue = value;
  sValue.ReplaceAll("\\","\\\\");
  sValue.ReplaceAll("'","\\'");
  sValue.InsertAt(0,'\'');
  sValue.InsertAt(sValue.GetLen(),'\'');
  return UpdateVariableDirect(varName,sValue.AsChar());
}

bool VRemoteInput::RemoveVariable(const char* varName)
{
  m_variableMutex.Lock();
  for(int i=0; i < m_variables.GetLength(); i++)
  {
    if(m_variables[i].sVarName == varName)
    {
      m_variables.RemoveAt(i);
      m_variableMutex.Unlock();
      return true;
    }
  }
  m_variableMutex.Unlock();
  return false;
}

void VRemoteInput::SetResizeOnConnect(bool bOn)
{
  m_bResizeOnConnect = bOn;
}

void VRemoteInput::SetSmoothTouchInput(bool bOn)
{
  m_bSmoothTouchInput = bOn;
}

void VRemoteInput::SetDisableMouseInput(bool bOn)
{
  m_bDisableMouseInput = bOn;
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
