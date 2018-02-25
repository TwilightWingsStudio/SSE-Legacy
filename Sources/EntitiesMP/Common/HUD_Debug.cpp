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
#include "GameMP/SEColors.h"

#include "HUD.h"

#include <Engine/Graphics/DrawPort.h>

#include <EntitiesMP/EnemyBase.h>

extern const CPlayer *_penPlayer;
extern COLOR _colHUD;

extern void HUD_DrawDebugMonitor()
{
  const FLOAT fExtraEnemyStrength = GetSP()->sp_fExtraEnemyStrength;
  const FLOAT fExtraStrengthPerPlayer = GetSP()->sp_fExtraEnemyStrengthPerPlayer;
  const INDEX ctCredits = GetSP()->sp_ctCredits;
  const BOOL bSharedLives = GetSP()->sp_bSharedLives;
  const BOOL bSharedKeys = GetSP()->sp_bSharedKeys;

  INDEX ctMaxPlayers = CEntity::GetMaxPlayers();
  INDEX ctAlive = 0;
  INDEX ctDead = 0;
  INDEX ctDeadCanRespawn = 0;
  INDEX ctLocal = 0;
  INDEX ctHaveKeys = 0;
  
  for (INDEX iPlayer = 0; iPlayer < ctMaxPlayers; iPlayer++)
  {
    CEntity *penEntity = CEntity::GetPlayerEntity(iPlayer);
    
    // If player is invalid then skip him.
    if (penEntity == NULL) {
      continue;
    }
    
    if (_pNetwork->IsPlayerLocal(penEntity)) {
      ctLocal++;
    }
    
    CPlayer *penPlayer = static_cast<CPlayer*>(penEntity);
    
    if (penPlayer->m_ulKeys != 0) {
      ctHaveKeys++;
    }

    if (penEntity->IsAlive()) {
      ctAlive++;
    } else {
      ctDead++;
      
      if (!bSharedLives) {
        if (penPlayer->m_iLives > 0) {
          ctDeadCanRespawn++;
        }
      }
    }
  }
  
  if (bSharedLives)
  {
    INDEX ctCreditsLeft = GetSP()->sp_ctCreditsLeft;

    // If all dead can be respawned then count them all.
    if (ctDead <= ctCreditsLeft) {
      ctDeadCanRespawn = ctDead;
    } else { // If not enough lives for all.
      ctDeadCanRespawn = ctCreditsLeft;
    }
  }
  
  CTString strReport;

  strReport += "^bDEBUG MONITOR^r\n";
  strReport += "\n";
  strReport += "[Common]\n";
  strReport.PrintF("%s^cCCCCCCPlayers: %02d / %02d\n", strReport, ctAlive + ctDead, ctMaxPlayers);
  strReport.PrintF("%s^cCCCCCCLocal:   %02d\n", strReport, ctLocal);
  strReport += "\n";
  strReport.PrintF("%s^c00FF00Alive: %02d\n", strReport, ctAlive);
  strReport.PrintF("%s^cFF0000Dead:  %02d\n", strReport, ctDead);

  // If using lives.
  if (ctCredits >= 0) {
    strReport.PrintF("%s^cCCCCCCCan Respawn: %02d\n", strReport, ctDeadCanRespawn);

    strReport.PrintF("%s^cCCCCCCLives: %s\n", strReport, bSharedLives ? "Shared" : "Personal");
    
    BOOL bRaisingLiveCost = GetSP()->sp_bRaisingLiveCost;
    
    if (bRaisingLiveCost) {
      strReport.PrintF("%s^cCCCCCCCost Mul: %0.2f\n", strReport, bSharedLives ? GetSP()->sp_fLiveCostMultiplier : _penPlayer->m_fLiveCostMultiplier);
    }
  }

  strReport += "\n";
  strReport.PrintF("%s^cCCCCCCKeys:  %s\n", strReport, bSharedKeys ? "Shared" : "Personal");
  if (!bSharedKeys) {
    strReport.PrintF("%s^cCCCCCCHave Keys: %d\n", strReport, ctHaveKeys);
  }

  strReport.PrintF("%s^cCCCCCCLevel: %d\n", strReport, _penPlayer->m_iLevel);
  strReport.PrintF("%s^cCCCCCC#0 Money: %d\n", strReport, _penPlayer->m_iCurrency1);
  strReport.PrintF("%s^cCCCCCC#1 Sup-s: %d\n", strReport, _penPlayer->m_iCurrency2);
  strReport.PrintF("%s^cCCCCCC#2 Cr.    %d\n", strReport, _penPlayer->m_iCurrency3);
  strReport.PrintF("%s^cCCCCCC#3 Tk.    %d\n", strReport, _penPlayer->m_iCurrency4);
  
  strReport.PrintF("%s^cCCCCCCShields: ", strReport);

  if (_penPlayer->m_fShields >= 10000.0F) {
    strReport.PrintF("%s%.1f k\n", strReport, _penPlayer->m_fShields / 1000.0F);
  } else if (_penPlayer->m_fShields >= 1000.0F) {
    strReport.PrintF("%s%.2f k\n", strReport, _penPlayer->m_fShields / 1000.0F);
  } else {
    strReport.PrintF("%s%.2f\n", strReport, _penPlayer->m_fShields);
  }

  strReport += "\n\n";
  strReport += "^r[Mode Flags]^cCCCCCC\n";
  if (GetSP()->sp_bCooperative) {
    strReport.PrintF("%s  Cooperative\n", strReport);
  }
  
  if (GetSP()->sp_bTeamPlay) {
    strReport.PrintF("%s  TeamPlay\n", strReport);
  }
  
  if (GetSP()->sp_bSinglePlayer) {
    strReport.PrintF("%s  SinglePlayer\n", strReport);
  }
  
  if (GetSP()->sp_bQuickTest) {
    strReport.PrintF("%s  QuickTest\n", strReport);
  }
  
  if (GetSP()->sp_bUseFrags) {
    strReport.PrintF("%s  UseFrags\n", strReport);
  }
  
  if (GetSP()->sp_ulMutatorFlags != 0)
  {
    strReport += "\n\n";
    strReport += "^r[Mutators]^cCCCCCC\n";
    
    if (GetSP()->sp_ulMutatorFlags & MUTF_INSTAGIB) {
      strReport.PrintF("%s  Instagib\n", strReport);
    }
    
    if (GetSP()->sp_ulMutatorFlags & MUTF_VAMPIRE) {
      strReport.PrintF("%s  Vampire\n", strReport);
    }
    
    if (GetSP()->sp_ulMutatorFlags & MUTF_ROCKETJUMP) {
      strReport.PrintF("%s  RocketJump\n", strReport);
    }
  }
  
  strReport += "\n\n";
  strReport += "^r[^cFFFFFFDDA System^r]^cCCCC00\n";
  strReport.PrintF("%sEES:   %.2f\n", strReport, fExtraEnemyStrength);
  strReport.PrintF("%sEESPP: %.2f\n", strReport, fExtraStrengthPerPlayer);
  strReport.PrintF("%sDamage Mul: %.2f\n", strReport, GetGameDamageMultiplier());
  
  FLOAT3D vPlayerPos = _penPlayer->GetPlacement().pl_PositionVector;
  
  strReport += "\n\n";
  strReport += "^r[^cFFFFFFPlacement^r]^cCCCC00\n";
  strReport.PrintF("%sX: %-6.2f\n", strReport, vPlayerPos(1));
  strReport.PrintF("%sY: %-6.2f\n", strReport, vPlayerPos(2));
  strReport.PrintF("%sZ: %-6.2f\n", strReport, vPlayerPos(3));
  
  
  strReport += "\n\n";
  strReport += "^r[^cFFFFFFEnemies^r]^cCCCC00\n";
  
  INDEX ctAliveEnemies = 0;
  INDEX ctAliveBosses = 0;
  INDEX ctDeadEnemies = 0;
  INDEX ctTemplateEnemies = 0;
  
  FOREACHINDYNAMICCONTAINER(((CEntity&)*_penPlayer).GetWorld()->wo_cenEntities, CEntity, iten)
  {
    CEntity *pen = iten;
    
    if (pen == NULL) continue;
    
    if (pen->IsLiveEntity() && IsDerivedFromClass(pen, "Enemy Base")) {
      CEnemyBase *penEnemy = static_cast<CEnemyBase *>(pen);

      if (penEnemy->m_bTemplate) {
        ctTemplateEnemies++;
        continue;
      }
      
      if (penEnemy->IsDead()) {
        ctDeadEnemies++;
      } else {
        ctAliveEnemies++;

        if (penEnemy->m_bBoss) {
          ctAliveBosses++;
        }
      }
    }
  }
  
  strReport.PrintF("%sAlive:      %04d\n", strReport, ctAliveEnemies);
  strReport.PrintF("%sAl. Bosses: %04d\n", strReport, ctAliveBosses);
  strReport.PrintF("%sDead:       %04d\n", strReport, ctDeadEnemies);
  strReport.PrintF("%sTemplate:   %04d\n", strReport, ctTemplateEnemies);

  _pDP->SetFont( _pfdConsoleFont);
  _pDP->SetTextAspect( 1.0f);
  _pDP->SetTextScaling(1.0f);

  _pDP->Fill(_pixDPWidth - 128 ,0, _pixDPWidth, _pixDPHeight, C_BLACK|0, C_BLACK|128, C_BLACK|0, C_BLACK|128);
  _pDP->PutText( strReport, _pixDPWidth - 128 + 4, 65, C_WHITE|CT_OPAQUE);
}

CRationalEntity *DBG_prenStackOutputEntity = NULL;

void HUD_SetEntityForStackDisplay(CRationalEntity *pren)
{
  DBG_prenStackOutputEntity = pren;
  return;
}

extern void HUD_DrawEntityStack()
{
  CTString strTemp;
  PIX pixFontHeight;
  ULONG pixTextBottom;

  if (tmp_ai[9]==12345)
  {
    if (DBG_prenStackOutputEntity!=NULL)
    {
      pixFontHeight = _pfdConsoleFont->fd_pixCharHeight;
      pixTextBottom = _pixDPHeight*0.83;
      _pDP->SetFont( _pfdConsoleFont);
      _pDP->SetTextScaling( 1.0f);

      INDEX ctStates = DBG_prenStackOutputEntity->en_stslStateStack.Count();
      strTemp.PrintF("-- stack of '%s'(%s)@%gs\n", DBG_prenStackOutputEntity->GetName(),
        DBG_prenStackOutputEntity->en_pecClass->ec_pdecDLLClass->dec_strName,
        _pTimer->CurrentTick());
      _pDP->PutText( strTemp, 1, pixTextBottom-pixFontHeight*(ctStates+1), _colHUD|_ulAlphaHUD);

      for(INDEX iState=ctStates-1; iState>=0; iState--) {
        SLONG slState = DBG_prenStackOutputEntity->en_stslStateStack[iState];
        strTemp.PrintF("0x%08x %s\n", slState,
          DBG_prenStackOutputEntity->en_pecClass->ec_pdecDLLClass->HandlerNameForState(slState));
        _pDP->PutText( strTemp, 1, pixTextBottom-pixFontHeight*(iState+1), _colHUD|_ulAlphaHUD);
      }
    }
  }
}