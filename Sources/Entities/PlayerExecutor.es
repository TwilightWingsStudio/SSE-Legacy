/* Copyright (c) 2018 by ZCaliptium.

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

2297
%{
  #include "StdH.h"
  #include "Entities/Player.h"
%}

enum EPlayerExecutorType
{
  0 EPET_RANDOMPLAYER   "Random Player [0]",
  1 EPET_EVERYPLAYER    "Every Player [1]",
};

class CPlayerExecutor : CRationalEntity {
name      "CPlayerExecutor";
thumbnail "Thumbnails\\PlayerExecutor.tbn";
features  "HasName", "IsTargetable";

properties:
   1 CTString m_strName "Name" 'N'      = "Player Executor",
   3 CTString m_strDescription = "",
   
   4 BOOL m_bActive              "Active" 'A' = TRUE,
   5 BOOL m_bDebugMessages    "Debug Messages" = FALSE,
   6 enum EPlayerExecutorType m_eType "Type" = EPET_RANDOMPLAYER,
   
  11 CEntityPointer m_penErrorTarget  "Error Target" 'E',
  12 CEntityPointer m_penTarget       "Target" 'T',
   
components:
  1 model   MODEL_EXECUTOR   "Models\\Editor\\PlayerExecutor.mdl",
  2 texture TEXTURE_EXECUTOR "Models\\Editor\\PlayerExecutor.tex",

functions:
  // --------------------------------------------------------------------------------------
  // [SSE] Extended Engine API
  // Returns TRUE if main entity logic is active.
  // --------------------------------------------------------------------------------------
  virtual BOOL IsActive(void) const
  {
    return m_bActive;
  }

  // --------------------------------------------------------------------------------------
  // Returns short entity description to show it in SED.
  // --------------------------------------------------------------------------------------
  const CTString &GetDescription(void) const
  {
    return m_strDescription;
  }
  
  // --------------------------------------------------------------------------------------
  // Find one player number by random.
  // --------------------------------------------------------------------------------------
  CEntity* GetRandomPlayer(void)
  {
    //CPrintF("Getting random number... ");

    // get maximum number of players in game
    INDEX ctMaxPlayers = GetMaxPlayers();

    // find actual number of players
    INDEX ctActivePlayers = 0;

    for(INDEX i = 0; i < ctMaxPlayers; i++)
    {
      // Skip invalid players.
      if (GetPlayerEntity(i) == NULL) {
        continue;
      }

      ctActivePlayers++;
    }

    // If no any active player.
    if (ctActivePlayers == 0) {
      return NULL;
    }

    // Choose one player by random.
    INDEX iChosenActivePlayer = IRnd()%ctActivePlayers;

    // find its physical index
    INDEX iActivePlayer = 0;

    for (INDEX i = 0; i < ctMaxPlayers; i++)
    {
      CEntity *pen = GetPlayerEntity(i);
      
      // Skip invalid players.
      if (pen == NULL) {
        continue;
      }

      if (iActivePlayer == iChosenActivePlayer) {
        return pen;
      }

      iActivePlayer++;
    }

    ASSERT(FALSE);

    return NULL;
  }

  // --------------------------------------------------------------------------------------
  // Called every time when entity receiving ETrigger entity event.
  // --------------------------------------------------------------------------------------
  void DoExecution()
  {
    if (m_eType == EPET_RANDOMPLAYER) {
      CEntity *pen = GetRandomPlayer();
      
      if (pen) {
        if (m_bDebugMessages) {
          CPrintF("[PE] %s : Sending event from %s\n", GetName(), pen->GetName());
        }
        
        SendToTarget(m_penTarget, EET_TRIGGER, pen);
      } else {
        if (m_bDebugMessages) {
          CPrintF("[PE] %s : Player not found! Processing error!\n", GetName());
        }
        
        SendToTarget(m_penErrorTarget, EET_TRIGGER, NULL);
      }
      
    } else {
      INDEX ctSent = 0;
      
      for (INDEX i = 0; i < GetMaxPlayers(); i++)
      {
        CEntity *pen = GetPlayerEntity(i);
        
        // Skip invalid players.
        if (pen == NULL) {
          continue;
        }

        SendToTarget(m_penTarget, EET_TRIGGER, pen);
        ctSent++;
      }
      
      if (m_bDebugMessages) {
        if (ctSent >= 1) {
          CPrintF("[PE] %s : Sent event for %d players.\n", GetName(), ctSent);
        } else {
          CPrintF("[PE] %s : There are no players! Processing error!\n", GetName());
        }
      }
      
      if (ctSent == 0) {
        SendToTarget(m_penErrorTarget, EET_TRIGGER, NULL);
      }
    }
  }

procedures:
  // --------------------------------------------------------------------------------------
  // The entry point.
  // --------------------------------------------------------------------------------------
  Main()
  {
    InitAsEditorModel();
    SetPhysicsFlags(EPF_MODEL_IMMATERIAL);
    SetCollisionFlags(ECF_IMMATERIAL);

    // set appearance
    SetModel(MODEL_EXECUTOR);
    SetModelMainTexture(TEXTURE_EXECUTOR);
  
    autowait(0.1f);
  
    wait()
    {
      on (EBegin) : { 
        resume;
      }

      on(ETrigger) : {
        if (m_bActive) {
          DoExecution();
        }

        resume;
      }

      on (EActivate) : {
        m_bActive = TRUE;
        resume;
      }

      on (EDeactivate) : {
        m_bActive = FALSE;
        resume;
      }
    }
  }
};

