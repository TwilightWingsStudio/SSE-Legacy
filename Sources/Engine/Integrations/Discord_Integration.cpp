/* Copyright (c) 2018 ZCaliptium. 
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

#include <Engine/Base/Console.h>
#include <Engine/Base/Shell.h>
#include <Engine/Base/ErrorReporting.h>
#include <Engine/Base/Translation.h>

#include <Engine/Network/Network.h>
#include <Engine/Network/SessionState.h>
#include <Engine/Network/Server.h>

#include <Engine/Integrations/Discord_Integration.h>

#include <Game/SessionProperties.h> // TODO: GET RID OF THIS!

#include <stdint.h>
#include <time.h>

static int64_t _llStartTime;
static BOOL _bDiscordEnabled = FALSE;
static BOOL _bLastGameActive = FALSE;
static TIME _tmLastPresenceUpdate = 0;
static HINSTANCE _hDiscordLib = NULL;

extern CTString _getGameModeShortName(INDEX iGameMode);
extern const CSessionProperties* _getSP();

static void FailFunction_t(const char *strName) {
  ThrowF_t(TRANS("Function %s not found."), strName);
}

typedef struct DiscordRichPresence {
    const char* state;   /* max 128 bytes */
    const char* details; /* max 128 bytes */
    int64_t startTimestamp;
    int64_t endTimestamp;
    const char* largeImageKey;  /* max 32 bytes */
    const char* largeImageText; /* max 128 bytes */
    const char* smallImageKey;  /* max 32 bytes */
    const char* smallImageText; /* max 128 bytes */
    const char* partyId;        /* max 128 bytes */
    int partySize;
    int partyMax;
    const char* matchSecret;    /* max 128 bytes */
    const char* joinSecret;     /* max 128 bytes */
    const char* spectateSecret; /* max 128 bytes */
    int8_t instance;
} DiscordRichPresence;

typedef struct DiscordJoinRequest {
    const char* userId;
    const char* username;
    const char* discriminator;
    const char* avatar;
} DiscordJoinRequest;

typedef struct DiscordEventHandlers {
    void (*ready)(void);
    void (*disconnected)(int errorCode, const char* message);
    void (*errored)(int errorCode, const char* message);
    void (*joinGame)(const char* joinSecret);
    void (*spectateGame)(const char* spectateSecret);
    void (*joinRequest)(const DiscordJoinRequest* request);
} DiscordEventHandlers;

static const char* APPLICATION_ID = "421325065930539018";

#define DISCORD_REPLY_NO 0
#define DISCORD_REPLY_YES 1
#define DISCORD_REPLY_IGNORE 2

#define DLLFUNCTION(dll, output, name, inputs, params, required) \
  output (__cdecl *p##name) inputs = NULL;
#include "Discord_Functions.h"
#undef DLLFUNCTION

static void Discord_SetFunctionPointers_t(void) {
  const char *strName;
  // get discord function pointers
  #define DLLFUNCTION(dll, output, name, inputs, params, required) \
    strName = #name ;  \
    p##name = (output (__cdecl *) inputs) GetProcAddress( _hDiscordLib, strName); \
    if(p##name == NULL) FailFunction_t(strName);
  #include "Discord_Functions.h"
  #undef DLLFUNCTION
}

static void Discord_ClearFunctionPointers(void) {
  // clear discord function pointers
  #define DLLFUNCTION(dll, output, name, inputs, params, required) p##name = NULL;
  #include "Discord_Functions.h"
  #undef DLLFUNCTION
}


#define DISCORD_LIB_NAME "discord-rpc.dll"
void Discord_InitPlugin()
{
  if (_hDiscordLib != NULL) {
    return;
  }
  
  _llStartTime = time(0);

  try {
    _hDiscordLib = ::LoadLibraryA(DISCORD_LIB_NAME);
    
    if( _hDiscordLib == NULL) {
      ThrowF_t(TRANS("Cannot load %s."), DISCORD_LIB_NAME);
    }
    
    Discord_SetFunctionPointers_t();
    
    _bDiscordEnabled = TRUE;
    CPrintF(TRANS("%s loaded, Discord integration enabled\n"), DISCORD_LIB_NAME);

  } catch (char *strError) {
    CPrintF(TRANS("Discord integration disabled: %s\n"), strError);
    return;
  }
  
  
  DiscordEventHandlers handlers;
  memset(&handlers, 0, sizeof(handlers));
  //handlers.ready = handleDiscordReady;
  //handlers.disconnected = handleDiscordDisconnected;
  //handlers.errored = handleDiscordError;
  //handlers.joinGame = handleDiscordJoin;
  //handlers.spectateGame = handleDiscordSpectate;
  //handlers.joinRequest = handleDiscordJoinRequest;
  pDiscord_Initialize(APPLICATION_ID, &handlers, 1, NULL);
}

void Discord_EndPlugin()
{
  if (_bDiscordEnabled)
  {
    pDiscord_Shutdown();
    
    Discord_ClearFunctionPointers();
    FreeLibrary(_hDiscordLib);
    _hDiscordLib = NULL;
    _bDiscordEnabled = FALSE;
  }
}

extern BOOL _bWorldEditorApp;

static const char* GetCurrentGameStateName(BOOL bGameActive)
{
  if (_bWorldEditorApp) {
    return "Working in Editor";
  }

  if (bGameActive)
  {
    if (_pNetwork != NULL) {
      if (!_pNetwork->IsNetworkEnabled()) {
        return "Playing Solo";
      } else {
        if (_pNetwork->IsServer()) {
          return "Hosting Game";
        } else {
          return "Playing on Server";
        }
      }
    }
  }

  return "In Menus";
}

CTString _getCurrentGameModeName()
{
  // get function that will provide us the info about gametype
  CShellSymbol *pss = _pShell->GetSymbol("GetCurrentGameModeNameSS", TRUE);

  if (pss == NULL) {
    return "";
  }

  CTString (*pFunc)(void) = (CTString (*)(void))pss->ss_pvValue;
  return pFunc();
}

void Discord_UpdateInfo(BOOL bGameActive)
{
  if (!_bDiscordEnabled) {
    return;
  }
  
  BOOL bGameActiveToggled = _bLastGameActive != bGameActive;
  
  if (bGameActiveToggled)
  {
    _llStartTime = time(0);
  }
  
  // Force update on game start/stop or wait 10 seconds before updates.
  if (bGameActiveToggled || (_pTimer->GetRealTimeTick() - _tmLastPresenceUpdate >= 10.0f))
  {
    //CPrintF("Discord Presence Update!\n");
    
    char buffer[256];
    char buffer2[256];
    DiscordRichPresence discordPresence;
    memset(&discordPresence, 0, sizeof(discordPresence));

    discordPresence.state = GetCurrentGameStateName(bGameActive);
    discordPresence.largeImageKey = "main-large";
    discordPresence.smallImageKey = "main-star-s";

    if (bGameActive)
    {
      discordPresence.startTimestamp = _llStartTime;
      //discordPresence.endTimestamp = time(0) + 5 * 60;

      sprintf(buffer, _getCurrentGameModeName());
      discordPresence.details = buffer;
      
      if (_pNetwork->ga_sesSessionState.ses_ctMaxPlayers > 1)
      {
        sprintf(buffer2, _pNetwork->ga_World.wo_strName.Undecorated());
        discordPresence.largeImageText = buffer2;
        
        //discordPresence.partyId = "party1234";
        
        if (_pNetwork->IsServer()) {
          discordPresence.partySize = _pNetwork->ga_srvServer.GetPlayersCount();
        } else {
          discordPresence.partySize = _pNetwork->ga_sesSessionState.GetPlayersCount();
        }

        discordPresence.partyMax = _pNetwork->ga_sesSessionState.ses_ctMaxPlayers;
        //discordPresence.joinSecret = "join";
        //discordPresence.spectateSecret = "look";
        //discordPresence.matchSecret = "xyzzy";
        discordPresence.instance = 0;
      }
    }

    pDiscord_UpdatePresence(&discordPresence);

    _tmLastPresenceUpdate = _pTimer->GetRealTimeTick();
  }

  pDiscord_RunCallbacks();

  _bLastGameActive = bGameActive;
}
