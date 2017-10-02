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
#include <Engine/CurrentVersion.h>
#include "MenuManager.h"
#include "MenuPrinting.h"
#include "MenuStuff.h"
#include "MAudioOptions.h"

#define VOLUME_STEPS  50

ENGINE_API extern INDEX snd_iFormat;

// --------------------------------------------------------------------------------------
// Intializes Audio options menu.
// --------------------------------------------------------------------------------------
void CAudioOptionsMenu::Initialize_t(void)
{
  // Initialize title label.
  gm_pTitle = new CMGTitle(TRANS("AUDIO"));
  gm_pTitle->mg_boxOnScreen = BoxTitle();

  // Initialize "Auto-Adjust" trigger.
  gm_pAudioAutoTrigger = new CMGTrigger(TRANS("AUTO-ADJUST"));
  gm_pAudioAutoTrigger->mg_strTip = TRANS("adjust quality to fit your system");
  gm_pAudioAutoTrigger->mg_pmgUp = gm_pApplyButton;
  gm_pAudioAutoTrigger->mg_pmgDown = gm_pFrequencyTrigger;
  gm_pAudioAutoTrigger->mg_boxOnScreen = BoxMediumRow(0);
  gm_pAudioAutoTrigger->mg_astrTexts = astrNoYes;
  gm_pAudioAutoTrigger->mg_ctTexts = sizeof(astrNoYes) / sizeof(astrNoYes[0]);
  gm_pAudioAutoTrigger->mg_strValue = astrNoYes[0];

  // Initialize "Frequency" trigger.
  gm_pFrequencyTrigger = new CMGTrigger(TRANS("FREQUENCY"));
  gm_pFrequencyTrigger->mg_strTip = TRANS("select sound quality or turn sound off");
  gm_pFrequencyTrigger->mg_pmgUp = gm_pAudioAutoTrigger;
  gm_pFrequencyTrigger->mg_pmgDown = gm_pAudioAPITrigger;
  gm_pFrequencyTrigger->mg_boxOnScreen = BoxMediumRow(1);
  gm_pFrequencyTrigger->mg_astrTexts = astrFrequencyRadioTexts;
  gm_pFrequencyTrigger->mg_ctTexts = sizeof(astrFrequencyRadioTexts) / sizeof(astrFrequencyRadioTexts[0]);
  gm_pFrequencyTrigger->mg_strValue = astrFrequencyRadioTexts[0];
  
  // Initialize "Sound System" trigger.
  gm_pAudioAPITrigger = new CMGTrigger(TRANS("SOUND SYSTEM"));
  gm_pAudioAPITrigger->mg_strTip = TRANS("choose sound system (API) to use");
  gm_pAudioAPITrigger->mg_pmgUp = gm_pFrequencyTrigger;
  gm_pAudioAPITrigger->mg_pmgDown = gm_pWaveVolume;
  gm_pAudioAPITrigger->mg_boxOnScreen = BoxMediumRow(2);
  gm_pAudioAPITrigger->mg_astrTexts = astrSoundAPIRadioTexts;
  gm_pAudioAPITrigger->mg_ctTexts = sizeof(astrSoundAPIRadioTexts) / sizeof(astrSoundAPIRadioTexts[0]);
  gm_pAudioAPITrigger->mg_strValue = astrSoundAPIRadioTexts[0];

  // Initialize "Sound Effects Volume" slider.
  gm_pWaveVolume = new CMGSlider(TRANS("SOUND EFFECTS VOLUME"));
  gm_pWaveVolume->mg_boxOnScreen = BoxMediumRow(3);
  gm_pWaveVolume->mg_strTip = TRANS("adjust volume of in-game sound effects");
  gm_pWaveVolume->mg_pmgUp = gm_pAudioAPITrigger;
  gm_pWaveVolume->mg_pmgDown = gm_pMPEGVolume;

  // Initialize "Music Volume" slider.
  gm_pMPEGVolume = new CMGSlider(TRANS("MUSIC VOLUME"));
  gm_pMPEGVolume->mg_boxOnScreen = BoxMediumRow(4);
  gm_pMPEGVolume->mg_strTip = TRANS("adjust volume of in-game music");
  gm_pMPEGVolume->mg_pmgUp = gm_pWaveVolume;
  gm_pMPEGVolume->mg_pmgDown = gm_pApplyButton;

  // Initialize "Apply" button.
  gm_pApplyButton = new CMGButton(TRANS("APPLY"));
  gm_pApplyButton->mg_bfsFontSize = BFS_LARGE;
  gm_pApplyButton->mg_boxOnScreen = BoxBigRow(4);
  gm_pApplyButton->mg_strTip = TRANS("activate selected options");
  gm_pApplyButton->mg_pmgUp = gm_pMPEGVolume;
  gm_pApplyButton->mg_pmgDown = gm_pAudioAutoTrigger;

  // Add components.
  AddChild(gm_pTitle);
  AddChild(gm_pAudioAutoTrigger);
  AddChild(gm_pFrequencyTrigger);
  AddChild(gm_pAudioAPITrigger);
  AddChild(gm_pWaveVolume);
  AddChild(gm_pMPEGVolume);
  AddChild(gm_pApplyButton);
}

static void RefreshSoundFormat(void)
{
  CAudioOptionsMenu &gmCurrent = _pGUIM->gmAudioOptionsMenu;

  switch (_pSound->GetFormat())
  {
    case CSoundLibrary::SF_NONE:     {gmCurrent.gm_pFrequencyTrigger->mg_iSelected = 0; break; }
    case CSoundLibrary::SF_11025_16: {gmCurrent.gm_pFrequencyTrigger->mg_iSelected = 1; break; }
    case CSoundLibrary::SF_22050_16: {gmCurrent.gm_pFrequencyTrigger->mg_iSelected = 2; break; }
    case CSoundLibrary::SF_44100_16: {gmCurrent.gm_pFrequencyTrigger->mg_iSelected = 3; break; }
    default:                          gmCurrent.gm_pFrequencyTrigger->mg_iSelected = 0;
  }

  gmCurrent.gm_pAudioAutoTrigger->mg_iSelected = Clamp(sam_bAutoAdjustAudio, 0, 1);
  gmCurrent.gm_pAudioAPITrigger->mg_iSelected = Clamp(_pShell->GetINDEX("snd_iInterface"), 0L, 2L);

  gmCurrent.gm_pWaveVolume->mg_iMinPos = 0;
  gmCurrent.gm_pWaveVolume->mg_iMaxPos = VOLUME_STEPS;
  gmCurrent.gm_pWaveVolume->mg_iCurPos = (INDEX)(_pShell->GetFLOAT("snd_fSoundVolume")*VOLUME_STEPS + 0.5f);
  gmCurrent.gm_pWaveVolume->ApplyCurrentPosition();

  gmCurrent.gm_pMPEGVolume->mg_iMinPos = 0;
  gmCurrent.gm_pMPEGVolume->mg_iMaxPos = VOLUME_STEPS;
  gmCurrent.gm_pMPEGVolume->mg_iCurPos = (INDEX)(_pShell->GetFLOAT("snd_fMusicVolume")*VOLUME_STEPS + 0.5f);
  gmCurrent.gm_pMPEGVolume->ApplyCurrentPosition();

  gmCurrent.gm_pAudioAutoTrigger->ApplyCurrentSelection();
  gmCurrent.gm_pAudioAPITrigger->ApplyCurrentSelection();
  gmCurrent.gm_pFrequencyTrigger->ApplyCurrentSelection();
}

void CAudioOptionsMenu::StartMenu(void)
{
  RefreshSoundFormat();
  CGameMenu::StartMenu();
}

// [SSE]
BOOL CAudioOptionsMenu::OnEvent(const SEvent& event)
{
  if (event.EventType == EET_GUI_EVENT)
  {
    if (event.GuiEvent.EventType == EGET_CHANGED)
    {
      if (event.GuiEvent.Caller == gm_pWaveVolume) {
        _pShell->SetFLOAT("snd_fSoundVolume", event.GuiEvent.IntValue / FLOAT(VOLUME_STEPS));

      } else if (event.GuiEvent.Caller == gm_pMPEGVolume) {
        _pShell->SetFLOAT("snd_fMusicVolume", event.GuiEvent.IntValue / FLOAT(VOLUME_STEPS));
      }

    } else if (event.GuiEvent.EventType == EGET_TRIGGERED) {

      if (event.GuiEvent.Caller == gm_pWaveVolume) {
        gm_pWaveVolume->mg_iCurPos -= 5;
        gm_pWaveVolume->ApplyCurrentPosition();

      } else if (event.GuiEvent.Caller == gm_pMPEGVolume) {
        gm_pMPEGVolume->mg_iCurPos -= 5;
        gm_pMPEGVolume->ApplyCurrentPosition();

      } else if (event.GuiEvent.Caller == gm_pApplyButton) {
        sam_bAutoAdjustAudio = gm_pAudioAutoTrigger->mg_iSelected;

        if (sam_bAutoAdjustAudio) {
          _pShell->Execute("include \"Scripts\\Addons\\SFX-AutoAdjust.ini\"");
        } else {
          _pShell->SetINDEX("snd_iInterface", gm_pAudioAPITrigger->mg_iSelected);

          switch (gm_pFrequencyTrigger->mg_iSelected)
          {
            case 0: {_pSound->SetFormat(CSoundLibrary::SF_NONE); break; }
            case 1: {_pSound->SetFormat(CSoundLibrary::SF_11025_16); break; }
            case 2: {_pSound->SetFormat(CSoundLibrary::SF_22050_16); break; }
            case 3: {_pSound->SetFormat(CSoundLibrary::SF_44100_16); break; }
            default: _pSound->SetFormat(CSoundLibrary::SF_NONE);
          }
        }

        RefreshSoundFormat();
        snd_iFormat = _pSound->GetFormat();
      }
    }
  }
  
  return m_pParent ? m_pParent->OnEvent(event) : FALSE;
}