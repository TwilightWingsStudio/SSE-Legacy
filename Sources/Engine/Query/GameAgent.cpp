/* Copyright (c) 2002-2012 Croteam Ltd. 
This program is free software; you can redistribute it and/or modify
it under the terms of version 2 of the GNU General Public License as published by
the Free Software Foundation


This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA. */

#include "StdH.h"

#include <Engine/Engine.h>
#include <Engine/CurrentVersion.h>
#include <Engine/Entities/Entity.h>
#include <Engine/Base/Shell.h>
#include <Engine/Base/Console.h>
#include <Engine/Base/CTString.h>
#include <Engine/Network/Server.h>
#include <Engine/Network/Network.h>
#include <Engine/Network/SessionState.h>
#include <GameMP/SessionProperties.h> // TODO: GET RID OF THIS!

#include <Engine/Query/GameAgent.h>

#pragma comment(lib, "wsock32.lib")

WSADATA* _wsaData = NULL;
SOCKET _socket = NULL;

sockaddr_in* _sin = NULL;
sockaddr_in* _sinLocal = NULL;
sockaddr_in _sinFrom;

CHAR* _szBuffer = NULL;
CHAR* _szIPPortBuffer = NULL;
INT   _iIPPortBufferLen = 0;
CHAR* _szIPPortBufferLocal = NULL;
INT   _iIPPortBufferLocalLen = 0;

BOOL _bServer = FALSE;
BOOL _bInitialized = FALSE;
BOOL _bActivated = FALSE;
BOOL _bActivatedLocal = FALSE;

TIME _tmLastHeartbeat = 0;

CDynamicStackArray<CServerRequest> ga_asrRequests;

//extern CTString ga_strServer = "master1.croteam.org";
extern CTString ga_strServer = "master1.42amsterdam.net";
//extern CTString ga_strMSLegacy = "master1.croteam.org";
extern CTString ga_strMSLegacy = "42amsterdam.net";
extern CTString ga_strDarkPlacesMS = "192.168.1.4";

extern BOOL ga_bMSLegacy = TRUE;

extern BOOL ga_bDarkPlacesMS = TRUE;

//BOOL ga_bMSLegacy = FALSE;

void _uninitWinsock();

void _initializeWinsock(void)
{
  if (_wsaData != NULL && _socket != NULL) {
    return;
  }

  _wsaData = new WSADATA;
  _socket = NULL;

  // make the buffer that we'll use for packet reading
  if (_szBuffer != NULL) {
    delete[] _szBuffer;
  }
  _szBuffer = new char[2050];

  // start WSA
  if (WSAStartup(MAKEWORD(2, 2), _wsaData) != 0) {
    CPrintF("Error initializing winsock!\n");
    _uninitWinsock();
    return;
  }

  // get the host IP
  hostent* phe;
  
  if (ga_bDarkPlacesMS) {
    phe = gethostbyname(ga_strDarkPlacesMS);
  } else if (!ga_bMSLegacy) {
    phe = gethostbyname(ga_strServer);
  } else {
    phe = gethostbyname(ga_strMSLegacy);
  }

  // if we couldn't resolve the hostname
  if (phe == NULL) {
    // report and stop
    CPrintF("Couldn't resolve GameAgent server %s.\n", ga_strServer);
    _uninitWinsock();
    return;
  }

  // create the socket destination address
  _sin = new sockaddr_in;
  _sin->sin_family = AF_INET;
  _sin->sin_addr.s_addr = *(ULONG*)phe->h_addr_list[0];
  
  // [SSE]
  if (ga_bDarkPlacesMS) {
    _sin->sin_port = htons(27950);
  } else if (!ga_bMSLegacy) {
    _sin->sin_port = htons(9005);
  } else {
    _sin->sin_port = htons(27900);
  }

  // create the socket
  _socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

  // if we're a server
  if (_bServer) {
    // create the local socket source address
    _sinLocal = new sockaddr_in;
    _sinLocal->sin_family = AF_INET;
    _sinLocal->sin_addr.s_addr = inet_addr("0.0.0.0");
    _sinLocal->sin_port = htons(_pShell->GetINDEX("net_iPort") + 1);

    // bind the socket
    bind(_socket, (sockaddr*)_sinLocal, sizeof(*_sinLocal));
  }

  // set the socket to be nonblocking
  DWORD dwNonBlocking = 1;
  if (ioctlsocket(_socket, FIONBIO, &dwNonBlocking) != 0) {
    CPrintF("Error setting socket to nonblocking!\n");
    _uninitWinsock();
    return;
  }
}

void _uninitWinsock()
{
  if (_wsaData != NULL) {
    closesocket(_socket);
    delete _wsaData;
    _wsaData = NULL;
  }
  _socket = NULL;
}

void _sendPacketTo(const char* pubBuffer, INDEX iLen, sockaddr_in* sin)
{
  sendto(_socket, pubBuffer, iLen, 0, (sockaddr*)sin, sizeof(sockaddr_in));
}

void _sendPacketTo(const char* szBuffer, sockaddr_in* addsin)
{
  sendto(_socket, szBuffer, strlen(szBuffer), 0, (sockaddr*)addsin, sizeof(sockaddr_in));
}

void _sendPacket(const char* pubBuffer, INDEX iLen)
{
  _initializeWinsock();
  _sendPacketTo(pubBuffer, iLen, _sin);
}

void _sendPacket(const char* szBuffer)
{
  _initializeWinsock();
  _sendPacketTo(szBuffer, _sin);
}

int _recvPacket()
{
  int fromLength = sizeof(_sinFrom);
  return recvfrom(_socket, _szBuffer, 2048, 0, (sockaddr*)&_sinFrom, &fromLength);
}

CTString _getGameModeName(INDEX iGameMode)
{
  // get function that will provide us the info about gametype
  CShellSymbol *pss = _pShell->GetSymbol("GetGameTypeNameSS", /*bDeclaredOnly=*/ TRUE);

  if (pss == NULL) {
    return "";
  }

  CTString (*pFunc)(INDEX) = (CTString (*)(INDEX))pss->ss_pvValue;
  return pFunc(iGameMode);
}

CTString _getGameModeShortName(INDEX iGameMode)
{
  // get function that will provide us the info about gametype
  CShellSymbol *pss = _pShell->GetSymbol("GetGameTypeShortNameSS", /*bDeclaredOnly=*/ TRUE);

  if (pss == NULL) {
    return "";
  }

  CTString (*pFunc)(INDEX) = (CTString (*)(INDEX))pss->ss_pvValue;
  return pFunc(iGameMode);
}

const CSessionProperties* _getSP()
{
  return ((const CSessionProperties *)_pNetwork->GetSessionProperties());
}

// --------------------------------------------------------------------------------------
// Builds hearthbeat packet.
// --------------------------------------------------------------------------------------
void GameAgent_BuildHearthbeatPacket(CTString &strPacket, INDEX iChallenge)
{
  strPacket.PrintF("0;challenge;%d;players;%d;maxplayers;%d;level;%s;gametype;%s;version;%s;product;%s",
      iChallenge,
      _pNetwork->ga_srvServer.GetPlayersCount(),
      _pNetwork->ga_sesSessionState.ses_ctMaxPlayers,
      _pNetwork->ga_World.wo_strName,
      _getGameModeName(_getSP()->sp_gmGameMode),
      _SE_VER_STRING,
      _pShell->GetString("sam_strGameName"));
}

void _sendHeartbeat(INDEX iChallenge)
{
  CTString strPacket;
  
  // [SSE]
  if (ga_bDarkPlacesMS) {
    DarkPlaces_BuildHearthbeatPacket(strPacket);

  // GameAgent
  } else if (!ga_bMSLegacy) {
    GameAgent_BuildHearthbeatPacket(strPacket, iChallenge);

  // MSLegacy
  } else {
    MSLegacy_BuildHearthbeatPacket(strPacket);
  }

  _sendPacket(strPacket);
  _tmLastHeartbeat = _pTimer->GetRealTimeTick();
}

extern void _setStatus(const CTString &strStatus)
{
  _pNetwork->ga_bEnumerationChange = TRUE;
  _pNetwork->ga_strEnumerationStatus = strStatus;
}

CServerRequest::CServerRequest(void)
{
  Clear();
}

CServerRequest::~CServerRequest(void) { }

void CServerRequest::Clear(void)
{
  sr_ulAddress = 0;
  sr_iPort = 0;
  sr_tmRequestTime = 0;
}

// --------------------------------------------------------------------------------------
// Called on every network server startup.
// --------------------------------------------------------------------------------------
extern void MS_OnServerStart(void)
{
  // join
  _bServer = TRUE;
  _bInitialized = TRUE;
  
  // [SSE]
  if (ga_bDarkPlacesMS) {
    CTString strPacket;
    strPacket.PrintF("\xFF\xFF\xFF\xFFheartbeat DarkPlaces\x0A");
    
    _sendPacket(strPacket);
    
    return;
  }
  //

  // GameAgent
  if (!ga_bMSLegacy) {
    _sendPacket("q");
    
  // MSLegacy
  } else {
    CTString strPacket;
    strPacket.PrintF("\\heartbeat\\%hu\\gamename\\serioussamse", (_pShell->GetINDEX("net_iPort") + 1));
    _sendPacket(strPacket);
  }
}

// --------------------------------------------------------------------------------------
// Called if server has been stopped.
// --------------------------------------------------------------------------------------
extern void MS_OnServerEnd(void)
{
  if (!_bInitialized) {
    return;
  }

  if (ga_bDarkPlacesMS) {
    _sendHeartbeat(0);
    _sendHeartbeat(0);
    // TODO: Write here something
  } else if (ga_bMSLegacy) {
    CTString strPacket;
    strPacket.PrintF("\\heartbeat\\%hu\\gamename\\serioussamse\\statechanged", (_pShell->GetINDEX("net_iPort") + 1));
    _sendPacket(strPacket);
  }

  _uninitWinsock();
  _bInitialized = FALSE;
}

static void GameAgent_ProcessReceivedPacket()
{
  // check the received packet ID
  switch (_szBuffer[0])
  {
    case 1: // server join response
    {
      int iChallenge = *(INDEX*)(_szBuffer + 1);
      // send the challenge
      _sendHeartbeat(iChallenge);
      break;
    }

    case 2: // server status request
    {
      // send the status response
      CTString strPacket;
      strPacket.PrintF("0;players;%d;maxplayers;%d;level;%s;gametype;%s;version;%s;gamename;%s;sessionname;%s",
        _pNetwork->ga_srvServer.GetPlayersCount(),
        _pNetwork->ga_sesSessionState.ses_ctMaxPlayers,
        _pNetwork->ga_World.wo_strName,
        _getGameModeName(_getSP()->sp_gmGameMode),
        _SE_VER_STRING,
        _pShell->GetString("sam_strGameName"),
        _pShell->GetString("gam_strSessionName"));
      _sendPacketTo(strPacket, &_sinFrom);
      break;
    }

    case 3: // player status request
    {
      // send the player status response
      CTString strPacket;
      strPacket.PrintF("\x01players\x02%d\x03", _pNetwork->ga_srvServer.GetPlayersCount());
      for (INDEX i=0; i<_pNetwork->ga_srvServer.GetPlayersCount(); i++) {
        CPlayerBuffer &plb = _pNetwork->ga_srvServer.srv_aplbPlayers[i];
        CPlayerTarget &plt = _pNetwork->ga_sesSessionState.ses_apltPlayers[i];
        if (plt.plt_bActive) {
          CTString strPlayer;
          plt.plt_penPlayerEntity->GetGameAgentPlayerInfo(plb.plb_Index, strPlayer);

          // if we don't have enough space left for the next player
          if (strlen(strPacket) + strlen(strPlayer) > 2048) {
            // send the packet
            _sendPacketTo(strPacket, &_sinFrom);
            strPacket = "";
          }

          strPacket += strPlayer;
        }
      }

      strPacket += "\x04";
      _sendPacketTo(strPacket, &_sinFrom);
      break;
    }

    case 4: // ping
    {
      // just send back 1 byte and the amount of players in the server (this could be useful in some cases for external scripts)
      CTString strPacket;
      strPacket.PrintF("\x04%d", _pNetwork->ga_srvServer.GetPlayersCount());
      _sendPacketTo(strPacket, &_sinFrom);
      break;
    }
  }
}

// --------------------------------------------------------------------------------------
// Regular network server update.
// Responds to enumeration pings and sends pings to masterserver.
// --------------------------------------------------------------------------------------
extern void MS_OnServerUpdate(void)
{
  if ((_socket == NULL) || (!_bInitialized)) {
    return;
  }

  memset(&_szBuffer[0], 0, 2050);
  int iLen = _recvPacket();

  if (iLen > 0)
  {
    // [SSE]
    if (ga_bDarkPlacesMS) {
      DarkPlaces_ServerParsePacket(iLen);
    } else if (!ga_bMSLegacy) {
      GameAgent_ProcessReceivedPacket();
    } else {
      _szBuffer[iLen] = 0;
      MSLegacy_ProcessReceivedPacket();
    }
 }

 // send a heartbeat every 150 seconds
 if (_pTimer->GetRealTimeTick() - _tmLastHeartbeat >= 150.0f) {
    _sendHeartbeat(0);
 }
}

// --------------------------------------------------------------------------------------
// Notify master server that the server state has changed.
// --------------------------------------------------------------------------------------
extern void MS_OnServerStateChanged(void)
{
  if (!_bInitialized) {
    return;
  }

  if (ga_bDarkPlacesMS) {
    // TODO: Write here something
  } else if (!ga_bMSLegacy) {
    _sendPacket("u");
  } else {
    CTString strPacket;
    strPacket.PrintF("\\heartbeat\\%hu\\gamename\\serioussamse\\statechanged", (_pShell->GetINDEX("net_iPort") + 1));
    _sendPacket(strPacket);
  }
}

void GameAgent_EnumTrigger(BOOL bInternet)
{
  // Make sure that there are no requests still stuck in buffer.
  ga_asrRequests.Clear();

  // We're not a server.
  _bServer = FALSE;
  // Initialization.
  _bInitialized = TRUE;
  // Send enumeration packet to masterserver.
  _sendPacket("e");
  _setStatus(".");
}

// --------------------------------------------------------------------------------------
// Request serverlist enumeration. Sends request packet.
// --------------------------------------------------------------------------------------
extern void MS_EnumTrigger(BOOL bInternet)
{
  if (_pNetwork->ga_bEnumerationChange) {
    return;
  }
  
  if (ga_bDarkPlacesMS) {
    DarkPlaces_EnumTrigger(bInternet);
    return; 
  }
  
  if (ga_bMSLegacy) {
    MSLegacy_EnumTrigger(bInternet);
    return;
  }
  
  GameAgent_EnumTrigger(bInternet);
}

void GameAgent_EnumUpdate(void)
{
  int iLen = _recvPacket();

  if (iLen == -1) {
    return;
  }

  // null terminate the buffer
  _szBuffer[iLen] = 0;

  switch (_szBuffer[0])
  {
    case 's':
    {
      struct sIPPort {
        UBYTE bFirst;
        UBYTE bSecond;
        UBYTE bThird;
        UBYTE bFourth;
        USHORT iPort;
      };

      _pNetwork->ga_strEnumerationStatus = "";
  
      sIPPort* pServers = (sIPPort*)(_szBuffer + 1);
      while(iLen - ((CHAR*)pServers - _szBuffer) >= sizeof(sIPPort)) {
        sIPPort ip = *pServers;
  
        CTString strIP;
        strIP.PrintF("%d.%d.%d.%d", ip.bFirst, ip.bSecond, ip.bThird, ip.bFourth);
  
        sockaddr_in sinServer;
        sinServer.sin_family = AF_INET;
        sinServer.sin_addr.s_addr = inet_addr(strIP);
        sinServer.sin_port = htons(ip.iPort + 1);
  
        // insert server status request into container
        CServerRequest &sreq = ga_asrRequests.Push();
        sreq.sr_ulAddress = sinServer.sin_addr.s_addr;
        sreq.sr_iPort = sinServer.sin_port;
        sreq.sr_tmRequestTime = _pTimer->GetHighPrecisionTimer().GetMilliseconds();
  
        // send packet to server
        _sendPacketTo("\x02", &sinServer);
  
        pServers++;
      }
    } break;
    
    case '0':
    {
      CTString strPlayers;
      CTString strMaxPlayers;
      CTString strLevel;
      CTString strGameType;
      CTString strVersion;
      CTString strGameName;
      CTString strSessionName;
  
      CHAR* pszPacket = _szBuffer + 2; // we do +2 because the first character is always ';', which we don't care about.
  
      BOOL bReadValue = FALSE;
      CTString strKey;
      CTString strValue;
  
      while(*pszPacket != 0) {
        switch (*pszPacket) {
        case ';':
          if (strKey != "sessionname") {
            if (bReadValue) {
              // we're done reading the value, check which key it was
              if (strKey == "players") {
                strPlayers = strValue;
              } else if (strKey == "maxplayers") {
                strMaxPlayers = strValue;
              } else if (strKey == "level") {
                strLevel = strValue;
              } else if (strKey == "gametype") {
                strGameType = strValue;
              } else if (strKey == "version") {
                strVersion = strValue;
              } else if (strKey == "gamename") {
                strGameName = strValue;
              } else {
                CPrintF("Unknown GameAgent parameter key '%s'!", strKey);
              }
  
              // reset temporary holders
              strKey = "";
              strValue = "";
            }
          }
          bReadValue = !bReadValue;
          break;
  
        default:
          // read into the value or into the key, depending where we are
          if (bReadValue) {
            strValue.InsertChar(strlen(strValue), *pszPacket);
          } else {
            strKey.InsertChar(strlen(strKey), *pszPacket);
          }
          break;
        }
  
        // move to next character
        pszPacket++;
      }
  
      // check if we still have a sessionname to back up
      if (strKey == "sessionname") {
        strSessionName = strValue;
      }
  
      // insert the server into the serverlist
      CNetworkSession &ns = *new CNetworkSession;
      _pNetwork->ga_lhEnumeratedSessions.AddTail(ns.ns_lnNode);
  
      long long tmPing = -1;
      // find the request in the request array
      for (INDEX i=0; i<ga_asrRequests.Count(); i++) {
        CServerRequest &req = ga_asrRequests[i];
        if (req.sr_ulAddress == _sinFrom.sin_addr.s_addr && req.sr_iPort == _sinFrom.sin_port) {
          tmPing = _pTimer->GetHighPrecisionTimer().GetMilliseconds() - req.sr_tmRequestTime;
          ga_asrRequests.Delete(&req);
          break;
        }
      }
  
      if (tmPing == -1) {
        // server status was never requested
        break;
      }
  
      // add the server to the serverlist
      ns.ns_strSession = strSessionName;
      ns.ns_strAddress = inet_ntoa(_sinFrom.sin_addr) + CTString(":") + CTString(0, "%d", htons(_sinFrom.sin_port) - 1);
      ns.ns_tmPing = (tmPing / 1000.0f);
      ns.ns_strWorld = strLevel;
      ns.ns_ctPlayers = atoi(strPlayers);
      ns.ns_ctMaxPlayers = atoi(strMaxPlayers);
      ns.ns_strGameType = strGameType;
      ns.ns_strMod = strGameName;
      ns.ns_strVer = strVersion;
    } break;
    
    default:
      CPrintF("Unknown enum packet ID %x!\n", _szBuffer[0]);
      break;
    }
}

// --------------------------------------------------------------------------------------
// Client update for enumerations.
// --------------------------------------------------------------------------------------
extern void MS_EnumUpdate(void)
{
  if ((_socket == NULL) || (!_bInitialized)) {
    return;
  }

  // [SSE]
  if (ga_bDarkPlacesMS) {
    DarkPlaces_EnumUpdate();
  
  // GameAgent
  } else if (!ga_bMSLegacy) {
    GameAgent_EnumUpdate();

  // MSLegacy
  } else {
    MSLegacy_EnumUpdate();
  }
}

// --------------------------------------------------------------------------------------
// Cancel the master server serverlist enumeration.
// --------------------------------------------------------------------------------------
extern void MS_EnumCancel(void)
{
  if (_bInitialized) {
    CPrintF("...MS_EnumCancel!\n");
    ga_asrRequests.Clear();
    _uninitWinsock();
  }
}

DWORD WINAPI _MS_Thread(LPVOID lpParam)
{
  SOCKET _sockudp = NULL;
  struct _sIPPort {
    UBYTE bFirst;
    UBYTE bSecond;
    UBYTE bThird;
    UBYTE bFourth;
    USHORT iPort;
  };

  _setStatus("");
  _sockudp = socket(AF_INET, SOCK_DGRAM, 0);

  if (_sockudp == INVALID_SOCKET){
    WSACleanup();
    return -1;
  }

  _sIPPort* pServerIP = (_sIPPort*)(_szIPPortBuffer);
  while(_iIPPortBufferLen >= 6)
  {
    if (!strncmp((char *)pServerIP, "\\final\\", 7)) {
      break;
    }

      _sIPPort ip = *pServerIP;

      CTString strIP;
      strIP.PrintF("%d.%d.%d.%d", ip.bFirst, ip.bSecond, ip.bThird, ip.bFourth);

      sockaddr_in sinServer;
      sinServer.sin_family = AF_INET;
      sinServer.sin_addr.s_addr = inet_addr(strIP);
      sinServer.sin_port = ip.iPort;

      // insert server status request into container
      CServerRequest &sreq = ga_asrRequests.Push();
      sreq.sr_ulAddress = sinServer.sin_addr.s_addr;
      sreq.sr_iPort = sinServer.sin_port;
      sreq.sr_tmRequestTime = _pTimer->GetHighPrecisionTimer().GetMilliseconds();

      // send packet to server
      sendto(_sockudp,"\\status\\",8,0,
          (sockaddr *) &sinServer, sizeof(sinServer));

      sockaddr_in _sinClient;
      int _iClientLength = sizeof(_sinClient);

      fd_set readfds_udp;                         // declare a read set
      struct timeval timeout_udp;                 // declare a timeval for our timer
      int iRet = -1;

      FD_ZERO(&readfds_udp);                      // zero out the read set
      FD_SET(_sockudp, &readfds_udp);                // add socket to the read set
      timeout_udp.tv_sec = 0;                     // timeout = 0 seconds
      timeout_udp.tv_usec = 50000;               // timeout += 0.05 seconds
      int _iN = select(_sockudp + 1, &readfds_udp, NULL, NULL, &timeout_udp);
      if (_iN > 0) {
        /** do recvfrom stuff **/
        iRet =  recvfrom(_sockudp, _szBuffer, 2048, 0, (sockaddr*)&_sinClient, &_iClientLength);
        FD_CLR(_sockudp, &readfds_udp);
        if (iRet != -1 && iRet > 100 && iRet != SOCKET_ERROR) {
          // null terminate the buffer
          _szBuffer[iRet] = 0;
          char *sPch = NULL;
          sPch = strstr(_szBuffer, "\\gamename\\serioussamse\\");
          if (!sPch) {
              CPrintF("Unknown query server response!\n");
              return -1;
          } else {

              CTString strPlayers;
              CTString strMaxPlayers;
              CTString strLevel;
              CTString strGameType;
              CTString strVersion;
              CTString strGameName;
              CTString strSessionName;

              CTString strGamePort;
              CTString strServerLocation;
              CTString strGameMode;
              CTString strActiveMod;

              CHAR* pszPacket = _szBuffer + 1; // we do +1 because the first character is always '\', which we don't care about.

              BOOL bReadValue = FALSE;
              CTString strKey;
              CTString strValue;

              while(*pszPacket != 0) {
              switch (*pszPacket) {
              case '\\':
                  if (strKey != "gamemode") {
                    if (bReadValue) {
                      // we're done reading the value, check which key it was
                      if (strKey == "gamename") {
                          strGameName = strValue;
                      } else if (strKey == "gamever") {
                          strVersion = strValue;
                      } else if (strKey == "location") {
                          strServerLocation = strValue;
                      } else if (strKey == "hostname") {
                          strSessionName = strValue;
                      } else if (strKey == "hostport") {
                          strGamePort = strValue;
                      } else if (strKey == "mapname") {
                          strLevel = strValue;
                      } else if (strKey == "gametype") {
                          strGameType = strValue;
                      } else if (strKey == "activemod") {
                          strActiveMod = strValue;
                      } else if (strKey == "numplayers") {
                          strPlayers = strValue;
                      } else if (strKey == "maxplayers") {
                          strMaxPlayers = strValue;
                      } else {
                          //CPrintF("Unknown GameAgent parameter key '%s'!", strKey);
                      }
                      // reset temporary holders
                      strKey = "";
                      strValue = "";
                    }
                  }
                  bReadValue = !bReadValue;
                  break;

              default:
                  // read into the value or into the key, depending where we are
                  if (bReadValue) {
                      strValue.InsertChar(strlen(strValue), *pszPacket);
                  } else {
                      strKey.InsertChar(strlen(strKey), *pszPacket);
                  }
                  break;
                }
                // move to next character
                pszPacket++;
              }

              // check if we still have a maxplayers to back up
              if (strKey == "gamemode") {
                  strGameMode = strValue;
              }
              if (strActiveMod != "") {
                  strGameName = strActiveMod;
              }
      
              long long tmPing = -1;
              // find the request in the request array
              for (INDEX i=0; i<ga_asrRequests.Count(); i++) {
                  CServerRequest &req = ga_asrRequests[i];
                  if (req.sr_ulAddress == _sinClient.sin_addr.s_addr && req.sr_iPort == _sinClient.sin_port) {
                      tmPing = _pTimer->GetHighPrecisionTimer().GetMilliseconds() - req.sr_tmRequestTime;
                      ga_asrRequests.Delete(&req);
                      break;
                  }
              }

              if (tmPing > 0 && tmPing < 2500000) {
          // insert the server into the serverlist
                  CNetworkSession &ns = *new CNetworkSession;
                  _pNetwork->ga_lhEnumeratedSessions.AddTail(ns.ns_lnNode);
        
                  // add the server to the serverlist
                  ns.ns_strSession = strSessionName;
                  ns.ns_strAddress = inet_ntoa(_sinClient.sin_addr) + CTString(":") + CTString(0, "%d", htons(_sinClient.sin_port) - 1);
                  ns.ns_tmPing = (tmPing / 1000.0f);
                  ns.ns_strWorld = strLevel;
                  ns.ns_ctPlayers = atoi(strPlayers);
                  ns.ns_ctMaxPlayers = atoi(strMaxPlayers);
                  ns.ns_strGameType = strGameType;
                  ns.ns_strMod = strGameName;
                  ns.ns_strVer = strVersion;
              }
          }
        } else {
          // find the request in the request array
          for (INDEX i=0; i<ga_asrRequests.Count(); i++) {
            CServerRequest &req = ga_asrRequests[i];
            if (req.sr_ulAddress == _sinClient.sin_addr.s_addr && req.sr_iPort == _sinClient.sin_port) {
              ga_asrRequests.Delete(&req);
              break;
            }
          }
        }
      }
      pServerIP++;
      _iIPPortBufferLen -= 6;
  }

  if (_szIPPortBuffer) free (_szIPPortBuffer);
  _szIPPortBuffer = NULL;

  closesocket(_sockudp);
  _uninitWinsock();
  _bInitialized = FALSE;
  _pNetwork->ga_bEnumerationChange = FALSE;
  WSACleanup();

  return 0;
}

DWORD WINAPI _LocalNet_Thread(LPVOID lpParam)
{
  SOCKET _sockudp = NULL;
  struct _sIPPort {
    UBYTE bFirst;
    UBYTE bSecond;
    UBYTE bThird;
    UBYTE bFourth;
    USHORT iPort;
  };

  _sockudp = socket(AF_INET, SOCK_DGRAM, 0);

  if (_sockudp == INVALID_SOCKET)
  {
    WSACleanup();
    _pNetwork->ga_strEnumerationStatus = "";

    if (_szIPPortBufferLocal != NULL) {
      delete[] _szIPPortBufferLocal;
    }

    _szIPPortBufferLocal = NULL;		
    return -1;
  }

  _sIPPort* pServerIP = (_sIPPort*)(_szIPPortBufferLocal);

  while(_iIPPortBufferLocalLen >= 6)
  {
    if (!strncmp((char *)pServerIP, "\\final\\", 7)) {
      break;
    }

    _sIPPort ip = *pServerIP;

    CTString strIP;
    strIP.PrintF("%d.%d.%d.%d", ip.bFourth, ip.bThird, ip.bSecond, ip.bFirst);

    sockaddr_in sinServer;
    sinServer.sin_family = AF_INET;
    sinServer.sin_addr.s_addr = inet_addr(strIP);
    sinServer.sin_port = ip.iPort;

    // insert server status request into container
    CServerRequest &sreq = ga_asrRequests.Push();
    sreq.sr_ulAddress = sinServer.sin_addr.s_addr;
    sreq.sr_iPort = sinServer.sin_port;
    sreq.sr_tmRequestTime = _pTimer->GetHighPrecisionTimer().GetMilliseconds();

    // send packet to server
    sendto(_sockudp,"\\status\\",8,0,
        (sockaddr *) &sinServer, sizeof(sinServer));

    sockaddr_in _sinClient;
    int _iClientLength = sizeof(_sinClient);

    fd_set readfds_udp;                         // declare a read set
    struct timeval timeout_udp;                 // declare a timeval for our timer
    int iRet = -1;

    FD_ZERO(&readfds_udp);                      // zero out the read set
    FD_SET(_sockudp, &readfds_udp);             // add socket to the read set
    timeout_udp.tv_sec = 0;                     // timeout = 0 seconds
    timeout_udp.tv_usec = 50000;                // timeout += 0.05 seconds
    int _iN = select(_sockudp + 1, &readfds_udp, NULL, NULL, &timeout_udp);

    if (_iN > 0)
    {
      /** do recvfrom stuff **/
      iRet =  recvfrom(_sockudp, _szBuffer, 2048, 0, (sockaddr*)&_sinClient, &_iClientLength);
      FD_CLR(_sockudp, &readfds_udp);
      if (iRet != -1 && iRet > 100 && iRet != SOCKET_ERROR) {
        // null terminate the buffer
        _szBuffer[iRet] = 0;
        char *sPch = NULL;
        sPch = strstr(_szBuffer, "\\gamename\\serioussamse\\");

        if (!sPch) {
            CPrintF("Unknown query server response!\n");
          if (_szIPPortBufferLocal != NULL) {
            delete[] _szIPPortBufferLocal;
          }
          _szIPPortBufferLocal = NULL;               
          WSACleanup();
          return -1;
        } else {

            CTString strPlayers;
            CTString strMaxPlayers;
            CTString strLevel;
            CTString strGameType;
            CTString strVersion;
            CTString strGameName;
            CTString strSessionName;

            CTString strGamePort;
            CTString strServerLocation;
            CTString strGameMode;
            CTString strActiveMod;

            CHAR* pszPacket = _szBuffer + 1; // we do +1 because the first character is always '\', which we don't care about.

            BOOL bReadValue = FALSE;
            CTString strKey;
            CTString strValue;

            while(*pszPacket != 0) {
            switch (*pszPacket) {
            case '\\':
                if (strKey != "gamemode") {
                  if (bReadValue) {
                    // we're done reading the value, check which key it was
                    if (strKey == "gamename") {
                        strGameName = strValue;
                    } else if (strKey == "gamever") {
                        strVersion = strValue;
                    } else if (strKey == "location") {
                        strServerLocation = strValue;
                    } else if (strKey == "hostname") {
                        strSessionName = strValue;
                    } else if (strKey == "hostport") {
                        strGamePort = strValue;
                    } else if (strKey == "mapname") {
                        strLevel = strValue;
                    } else if (strKey == "gametype") {
                        strGameType = strValue;
                    } else if (strKey == "activemod") {
                        strActiveMod = strValue;
                    } else if (strKey == "numplayers") {
                        strPlayers = strValue;
                    } else if (strKey == "maxplayers") {
                        strMaxPlayers = strValue;
                    } else {
          //CPrintF("Unknown GameAgent parameter key '%s'!", strKey);
                    }
                    // reset temporary holders
                    strKey = "";
                    strValue = "";
                  }
                }
                bReadValue = !bReadValue;
                break;

            default:
                // read into the value or into the key, depending where we are
                if (bReadValue) {
                    strValue.InsertChar(strlen(strValue), *pszPacket);
                } else {
                    strKey.InsertChar(strlen(strKey), *pszPacket);
                }
                break;
              }
              // move to next character
              pszPacket++;
            }

            // check if we still have a maxplayers to back up
            if (strKey == "gamemode") {
                strGameMode = strValue;
            }
            if (strActiveMod != "") {
                strGameName = strActiveMod;
            }

            long long tmPing = -1;
            // find the request in the request array
            for (INDEX i=0; i<ga_asrRequests.Count(); i++) {
                CServerRequest &req = ga_asrRequests[i];
                if (req.sr_ulAddress == _sinClient.sin_addr.s_addr && req.sr_iPort == _sinClient.sin_port) {
                    tmPing = _pTimer->GetHighPrecisionTimer().GetMilliseconds() - req.sr_tmRequestTime;
                    ga_asrRequests.Delete(&req);
                    break;
                }
            }

            if (tmPing > 0 && tmPing < 2500000) {
                // insert the server into the serverlist
                _pNetwork->ga_strEnumerationStatus = "";
                CNetworkSession &ns = *new CNetworkSession;
                _pNetwork->ga_lhEnumeratedSessions.AddTail(ns.ns_lnNode);
      
                // add the server to the serverlist
                ns.ns_strSession = strSessionName;
                ns.ns_strAddress = inet_ntoa(_sinClient.sin_addr) + CTString(":") + CTString(0, "%d", htons(_sinClient.sin_port) - 1);
                ns.ns_tmPing = (tmPing / 1000.0f);
                ns.ns_strWorld = strLevel;
                ns.ns_ctPlayers = atoi(strPlayers);
                ns.ns_ctMaxPlayers = atoi(strMaxPlayers);
                ns.ns_strGameType = strGameType;
                ns.ns_strMod = strGameName;
                ns.ns_strVer = strVersion;
            }
        }
      } else {
        // find the request in the request array
        for (INDEX i=0; i<ga_asrRequests.Count(); i++) {
          CServerRequest &req = ga_asrRequests[i];
          if (req.sr_ulAddress == _sinClient.sin_addr.s_addr && req.sr_iPort == _sinClient.sin_port) {
            ga_asrRequests.Delete(&req);
            break;
          }
        }
      }
    }

    pServerIP++;
    _iIPPortBufferLocalLen -= 6;
  }

  if (_szIPPortBufferLocal != NULL) {
    delete[] _szIPPortBufferLocal;
  }

  _szIPPortBufferLocal = NULL;

  closesocket(_sockudp);
  _uninitWinsock();
  _bInitialized = FALSE;
  _pNetwork->ga_bEnumerationChange = FALSE;
  _pNetwork->ga_strEnumerationStatus = "";
  WSACleanup();

  return 0;
}