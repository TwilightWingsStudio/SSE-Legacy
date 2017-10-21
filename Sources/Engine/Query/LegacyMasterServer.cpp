/* Copyright (c) 2017 ZCaliptium

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
#include <Engine/Query/MSLegacy.h>

#define MSPORT      28900
#define BUFFSZ      8192
#define BUFFSZSTR   4096

#define CHK_BUFFSTRLEN if ((iLen < 0) || (iLen > BUFFSZSTR)) { \
                        CPrintF("\n" \
                            "Error: the used buffer is smaller than how much needed (%d < %d)\n" \
                            "\n", iLen, BUFFSZSTR); \
                            if (cMsstring) free (cMsstring); \
                            closesocket(_sock); \
                            WSACleanup(); \
                        }

#define CLEANMSSRUFF1       closesocket(_sock); \
                            WSACleanup();

#define CLEANMSSRUFF2       if (cResponse) free (cResponse); \
                            closesocket(_sock); \
                            WSACleanup();

#define SERIOUSSAMKEY       "AKbna4\0"
#define SERIOUSSAMSTR       "serioussamse"

#define PCK         "\\gamename\\%s" \
                    "\\enctype\\%d" \
                    "\\validate\\%s" \
                    "\\final\\" \
                    "\\queryid\\1.1" \
                    "\\list\\cmp" \
                    "\\gamename\\%s" \
                    "\\gamever\\1.05" \
                    "%s%s" \
                    "\\final\\"

#define PCKQUERY    "\\gamename\\%s" \
                    "\\gamever\\%s" \
                    "\\location\\%s" \
                    "\\hostname\\%s" \
                    "\\hostport\\%hu" \
                    "\\mapname\\%s" \
                    "\\gametype\\%s" \
                    "\\activemod\\" \
                    "\\numplayers\\%d" \
                    "\\maxplayers\\%d" \
                    "\\gamemode\\openplaying" \
                    "\\difficulty\\Normal" \
                    "\\friendlyfire\\%d" \
                    "\\weaponsstay\\%d" \
                    "\\ammosstay\\%d" \
                    "\\healthandarmorstays\\%d" \
                    "\\allowhealth\\%d" \
                    "\\allowarmor\\%d" \
                    "\\infinitearmor\\%d" \
                    "\\respawninplace\\%d" \
                    "\\password\\0" \
                    "\\vipplayers\\1"

#define PCKINFO     "\\hostname\\%s" \
                    "\\hostport\\%hu" \
                    "\\mapname\\%s" \
                    "\\gametype\\%s" \
                    "\\numplayers\\%d" \
                    "\\maxplayers\\%d" \
                    "\\gamemode\\openplaying" \
                    "\\final\\" \
                    "\\queryid\\8.1"

#define PCKBASIC    "\\gamename\\%s" \
                    "\\gamever\\%s" \
                    "\\location\\EU" \
                    "\\final\\" \
                    "\\queryid\\1.1"

extern const CSessionProperties* _getSP();
extern CTString _getGameModeName(INDEX iGameMode);

extern sockaddr_in _sinFrom;
extern CHAR* _szBuffer;
extern CHAR* _szIPPortBuffer;
extern INT   _iIPPortBufferLen;
extern CHAR* _szIPPortBufferLocal;
extern INT   _iIPPortBufferLocalLen;

extern BOOL _bServer;
extern BOOL _bInitialized;
extern BOOL _bActivated;
extern BOOL _bActivatedLocal;

extern void _initializeWinsock(void);
extern void _uninitWinsock();
extern void _sendPacket(const char* szBuffer);
extern void _sendPacket(const char* pubBuffer, INDEX iLen);
extern void _sendPacketTo(const char* szBuffer, sockaddr_in* addsin);
extern void _sendPacketTo(const char* pubBuffer, INDEX iLen, sockaddr_in* sin);

extern CDynamicStackArray<CServerRequest> ga_asrRequests;

// --------------------------------------------------------------------------------------
// Builds hearthbeat packet.
// --------------------------------------------------------------------------------------
void MSLegacy_BuildHearthbeatPacket(CTString &strPacket)
{
  strPacket.PrintF("\\heartbeat\\%hu\\gamename\\serioussamse", (_pShell->GetINDEX("net_iPort") + 1));
}

void MSLegacy_EnumTrigger(BOOL bInternet)
{
  // Local Search with Legacy Protocol
  if (!bInternet) {
    // make sure that there are no requests still stuck in buffer
    ga_asrRequests.Clear();

    // we're not a server
    _bServer = FALSE;
    _pNetwork->ga_strEnumerationStatus = ".";
	
    WORD     _wsaRequested;
    WSADATA  wsaData;
    PHOSTENT _phHostinfo;
    ULONG    _uIP,*_pchIP = &_uIP;
    USHORT   _uPort,*_pchPort = &_uPort;
    INT      _iLen;
    char     _cName[256],*_pch,_strFinal[8] = {0};

    struct in_addr addr;

    // make the buffer that we'll use for packet reading
    if (_szIPPortBufferLocal != NULL) {
       return;
    }
    _szIPPortBufferLocal = new char[1024];
	
    // start WSA
    _wsaRequested = MAKEWORD( 2, 2 );
    if (WSAStartup(_wsaRequested, &wsaData) != 0) {
      CPrintF("Error initializing winsock!\n");
      if (_szIPPortBufferLocal != NULL) {
        delete[] _szIPPortBufferLocal;
      }
      _szIPPortBufferLocal = NULL;
      _uninitWinsock();
      _bInitialized = FALSE;
      _pNetwork->ga_bEnumerationChange = FALSE;
      _pNetwork->ga_strEnumerationStatus = "";
      WSACleanup();

      return;
    }

    _pch = _szIPPortBufferLocal;
    _iLen = 0;
    strcpy(_strFinal,"\\final\\");
	
    if (gethostname ( _cName, sizeof(_cName)) == 0)
    {
      if ((_phHostinfo = gethostbyname(_cName)) != NULL)
      {
        int _iCount = 0;
        while(_phHostinfo->h_addr_list[_iCount])
        {
          addr.s_addr = *(u_long *) _phHostinfo->h_addr_list[_iCount];
          _uIP = htonl(addr.s_addr);
          
          for (UINT uPort = 25601; uPort < 25622; ++uPort){
            _uPort = htons(uPort);
            memcpy(_pch,_pchIP,4);
            _pch  +=4;
            _iLen +=4;
            memcpy(_pch,_pchPort,2);
            _pch  +=2;
            _iLen +=2;
          }
          ++_iCount;
        }
        memcpy(_pch,_strFinal, 7);
        _pch  +=7;
        _iLen +=7;
        _pch[_iLen] = 0x00;
      }
	  }

    _iIPPortBufferLocalLen = _iLen;

    _bActivatedLocal = TRUE;
    _bInitialized = TRUE;
    _initializeWinsock();	
    return;
	
  // Internet Search
  } else {

    // make sure that there are no requests still stuck in buffer
    ga_asrRequests.Clear();
    // we're not a server
    _bServer = FALSE;
    _pNetwork->ga_strEnumerationStatus = ".";

    struct  sockaddr_in peer;

    SOCKET  _sock               = NULL;
    u_int   uiMSIP;
    int     iErr,
            iLen,
            iDynsz,
            iEnctype             = 0;
    u_short usMSport             = MSPORT;

    u_char  ucGamekey[]          = {SERIOUSSAMKEY},
            ucGamestr[]          = {SERIOUSSAMSTR},
            *ucSec               = NULL,
            *ucKey               = NULL;

    char    *cFilter             = "",
            *cWhere              = "",
            cMS[128]             = {0},
            *cResponse           = NULL,
            *cMsstring           = NULL,
            *cSec                = NULL;


    strcpy(cMS,ga_strMSLegacy);

    WSADATA wsadata;
    if (WSAStartup(MAKEWORD(2,2), &wsadata) != 0) {
        CPrintF("Error initializing winsock!\n");
        return;
    }

    /* Open a socket and connect to the Master server */

    peer.sin_addr.s_addr = uiMSIP = resolv(cMS);
    peer.sin_port        = htons(usMSport);
    peer.sin_family      = AF_INET;

    _sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (_sock < 0) {
        CPrintF("Error creating TCP socket!\n");
        WSACleanup();
        return;
    }
    if (connect(_sock, (struct sockaddr *)&peer, sizeof(peer)) < 0) {
        CPrintF("Error connecting to TCP socket!\n");
        CLEANMSSRUFF1;
        return;
    }

    /* Allocate memory for a buffer and get a pointer to it */

    cResponse = (char*) malloc(BUFFSZSTR + 1);
    if (!cResponse) {
        CPrintF("Error initializing memory buffer!\n");
        CLEANMSSRUFF1;
        return;
    }

    /* Reading response from Master Server - returns the string with the secret key */

    iLen = 0;
    iErr = recv(_sock, (char*)cResponse + iLen, BUFFSZSTR - iLen, 0);
    if (iErr < 0) {
        CPrintF("Error reading from TCP socket!\n");
        CLEANMSSRUFF2;
        return;
    }

    iLen += iErr;
    cResponse[iLen] = 0x00;

    /* Allocate memory for a buffer and get a pointer to it */

    ucSec = (u_char*) malloc(BUFFSZSTR + 1);
    if (!ucSec) {
        CPrintF("Error initializing memory buffer!\n");
        CLEANMSSRUFF2;
        return;
    }
    memcpy ( ucSec, cResponse,  BUFFSZSTR);
    ucSec[iLen] = 0x00;

    /* Geting the secret key from a string */

    cSec = strstr(cResponse, "\\secure\\");
    if (!cSec) {
        CPrintF("Not valid master server response!\n");
        CLEANMSSRUFF2;
        return;
    } else {
        ucSec  += 15;

    /* Creating a key for authentication (Validate key) */

        ucKey = gsseckey(ucSec, ucGamekey, iEnctype);
    }
    ucSec -= 15;
    if (cResponse) free (cResponse);
    if (ucSec) free (ucSec);

    /* Generate a string for the response (to Master Server) with the specified (Validate ucKey) */

    cMsstring = (char*) malloc(BUFFSZSTR + 1);
    if (!cMsstring) {
        CPrintF("Not valid master server response!\n");
        CLEANMSSRUFF1;
        return;
    }

    iLen = _snprintf(
        cMsstring,
        BUFFSZSTR,
        PCK,
        ucGamestr,
        iEnctype,
        ucKey,
        ucGamestr,
        cWhere,
        cFilter);

    /* Check the buffer */

    CHK_BUFFSTRLEN;

    /* The string sent to master server */

    if (send(_sock,cMsstring, iLen, 0) < 0){
        CPrintF("Error reading from TCP socket!\n");
        if (cMsstring) free (cMsstring);
        CLEANMSSRUFF1;
        return;
    }
    if (cMsstring) free (cMsstring);

    /* Allocate memory for a buffer and get a pointer to it */

    if (_szIPPortBuffer ) {
        CLEANMSSRUFF1;
        return;
    };

    _szIPPortBuffer = (char*) malloc(BUFFSZ + 1);
    if (!_szIPPortBuffer) {
        CPrintF("Error reading from TCP socket!\n");
        CLEANMSSRUFF1;
        return;
    }
    iDynsz = BUFFSZ;


    /* The received encoded data after sending the string (Validate key) */

    iLen = 0;
    while((iErr = recv(_sock, _szIPPortBuffer + iLen, iDynsz - iLen, 0)) > 0) {
        iLen += iErr;
        if (iLen >= iDynsz) {
            iDynsz += BUFFSZ;
            _szIPPortBuffer = (char*)realloc(_szIPPortBuffer, iDynsz);
            if (!_szIPPortBuffer) {
                CPrintF("Error reallocation memory buffer!\n");
                if (_szIPPortBuffer) free (_szIPPortBuffer);
                CLEANMSSRUFF1;
                return;
            }
        }
    }
    CLEANMSSRUFF1;
    _iIPPortBufferLen = iLen;

    _bActivated = TRUE;
    _bInitialized = TRUE;
    _initializeWinsock();
     
  }
}

void MSLegacy_EnumUpdate(void)
{
  if (_bActivated)
  {
    HANDLE  _hThread;
    DWORD   _dwThreadId;

    _hThread = CreateThread(NULL, 0, _MS_Thread, 0, 0, &_dwThreadId);
    if (_hThread != NULL) {
      CloseHandle(_hThread);
    }
    _bActivated = FALSE;		
  }

  if (_bActivatedLocal)
  {
    HANDLE  _hThread;
    DWORD   _dwThreadId;

    _hThread = CreateThread(NULL, 0, _LocalNet_Thread, 0, 0, &_dwThreadId);
    if (_hThread != NULL) {
      CloseHandle(_hThread);
    }

    _bActivatedLocal = FALSE;		
  }	
}

extern void MSLegacy_ProcessReceivedPacket()
{
  char *sPch1 = NULL, *sPch2 = NULL, *sPch3 = NULL, *sPch4 = NULL;
  sPch1 = strstr(_szBuffer, "\\status\\");
  sPch2 = strstr(_szBuffer, "\\info\\");
  sPch3 = strstr(_szBuffer, "\\basic\\");
  sPch4 = strstr(_szBuffer, "\\players\\");

  // status request
  if (sPch1) {
    CTString strPacket;
    CTString strLocation;
    strLocation = _pShell->GetString("net_strLocalHost");

    if (strLocation == ""){
      strLocation = "Heartland";
    }

    strPacket.PrintF( PCKQUERY,
      _pShell->GetString("sam_strGameName"),
      _SE_VER_STRING,
      //_pShell->GetString("net_strLocalHost"),
      strLocation,
      _pShell->GetString("gam_strSessionName"),
      _pShell->GetINDEX("net_iPort"),
      _pNetwork->ga_World.wo_strName,
      _getGameModeName(_getSP()->sp_gmGameMode),
      _pNetwork->ga_srvServer.GetPlayersCount(),
      _pNetwork->ga_sesSessionState.ses_ctMaxPlayers,
      _pShell->GetINDEX("gam_bFriendlyFire"),
      _pShell->GetINDEX("gam_bWeaponsStay"),
      _pShell->GetINDEX("gam_bAmmoStays"),
      _pShell->GetINDEX("gam_bHealthArmorStays"),
      _pShell->GetINDEX("gam_bAllowHealth"),
      _pShell->GetINDEX("gam_bAllowArmor"),
      _pShell->GetINDEX("gam_bInfiniteAmmo"),
      _pShell->GetINDEX("gam_bRespawnInPlace"));

      for (INDEX i=0; i<_pNetwork->ga_srvServer.GetPlayersCount(); i++)
      {
        CPlayerBuffer &plb = _pNetwork->ga_srvServer.srv_aplbPlayers[i];
        CPlayerTarget &plt = _pNetwork->ga_sesSessionState.ses_apltPlayers[i];
        if (plt.plt_bActive) {
          CTString strPlayer;
          plt.plt_penPlayerEntity->GetMSLegacyPlayerInf(plb.plb_Index, strPlayer);

          // if we don't have enough space left for the next player
          if (strlen(strPacket) + strlen(strPlayer) > 2048) {
            // send the packet
            _sendPacketTo(strPacket, &_sinFrom);
            strPacket = "";
          }
          strPacket += strPlayer;
        }
      }

    strPacket += "\\final\\\\queryid\\333.1";
    _sendPacketTo(strPacket, &_sinFrom);

  // info request
  } else if (sPch2){

    CTString strPacket;
    strPacket.PrintF( PCKINFO,
      _pShell->GetString("gam_strSessionName"),
      _pShell->GetINDEX("net_iPort"),
      _pNetwork->ga_World.wo_strName,
      _getGameModeName(_getSP()->sp_gmGameMode),
      _pNetwork->ga_srvServer.GetPlayersCount(),
      _pNetwork->ga_sesSessionState.ses_ctMaxPlayers);
    _sendPacketTo(strPacket, &_sinFrom);

  // basis request
  } else if (sPch3){

    CTString strPacket;
    CTString strLocation;
    strLocation = _pShell->GetString("net_strLocalHost");
    if (strLocation == ""){
      strLocation = "Heartland";
    }
    strPacket.PrintF( PCKBASIC,
      _pShell->GetString("sam_strGameName"),
      _SE_VER_STRING,
      //_pShell->GetString("net_strLocalHost"));
      strLocation);
    _sendPacketTo(strPacket, &_sinFrom);

  // players request
  } else if (sPch4){

    // send the player status response
    CTString strPacket;
    strPacket = "";

    for (INDEX i=0; i<_pNetwork->ga_srvServer.GetPlayersCount(); i++) {
      CPlayerBuffer &plb = _pNetwork->ga_srvServer.srv_aplbPlayers[i];
      CPlayerTarget &plt = _pNetwork->ga_sesSessionState.ses_apltPlayers[i];
      if (plt.plt_bActive) {
        CTString strPlayer;
        plt.plt_penPlayerEntity->GetMSLegacyPlayerInf(plb.plb_Index, strPlayer);

        // if we don't have enough space left for the next player
        if (strlen(strPacket) + strlen(strPlayer) > 2048) {
          // send the packet
          _sendPacketTo(strPacket, &_sinFrom);
          strPacket = "";
        }

        strPacket += strPlayer;
      }
    }

    strPacket += "\\final\\\\queryid\\6.1";
    _sendPacketTo(strPacket, &_sinFrom);

  } else {
    CPrintF("Unknown query server response!\n");
    return;
  }
}