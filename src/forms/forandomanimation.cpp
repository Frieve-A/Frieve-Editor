//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "forandomanimation.h"
#include "setting.h"
#include "fofullscreen.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFo_RandomAnimation *Fo_RandomAnimation;
//---------------------------------------------------------------------------
void TFo_RandomAnimation::ApplyLanguageSetting()
{
	if (!FileExists(LanguageFileName())){
		return;
	}
	TFastIni *Ini = new TFastIni(LanguageFileName());

	Caption = Ini->ReadString("RandomAnimation", "Caption", Caption);
	La_Zoom->Caption = Ini->ReadString("RandomAnimation", "La_Zoom", La_Zoom->Caption);
	La_Speed->Caption = Ini->ReadString("RandomAnimation", "La_Speed", La_Speed->Caption);
	La_Cards->Caption = Ini->ReadString("RandomAnimation", "La_Cards", La_Cards->Caption);
	SB_FullScreen->Caption = Ini->ReadString("RandomAnimation", "SB_FullScreen", SB_FullScreen->Caption);
	SB_Pause->Caption = Ini->ReadString("RandomAnimation", "SB_Pause", SB_Pause->Caption);
	SB_Stop->Caption = Ini->ReadString("RandomAnimation", "SB_Stop", SB_Stop->Caption);

	delete Ini;
}
//---------------------------------------------------------------------------
__fastcall TFo_RandomAnimation::TFo_RandomAnimation(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------

void __fastcall TFo_RandomAnimation::FormDeactivate(TObject *Sender)
{
    if (!m_bDontClose){
        Close();
    }
}
//---------------------------------------------------------------------------
void TFo_RandomAnimation::SettingToGUI()
{
	TB_Cards->Position = SettingView.m_nAnimationRCCards;
    TB_CardsChange(this);
    TB_Speed->Position = SettingView.m_nAnimationRCSpeed;
    SB_Pause->Down = SettingView.m_bAnimationPaused;
}
//---------------------------------------------------------------------------
void __fastcall TFo_RandomAnimation::TB_CardsChange(TObject *Sender)
{
	La_CardNum->Caption = TB_Cards->Position;
    SettingView.m_nAnimationRCCards = TB_Cards->Position;
}
//---------------------------------------------------------------------------
void __fastcall TFo_RandomAnimation::TB_SpeedChange(TObject *Sender)
{
    SettingView.m_nAnimationRCSpeed = TB_Speed->Position;
}
//---------------------------------------------------------------------------
void __fastcall TFo_RandomAnimation::SB_StopClick(TObject *Sender)
{
    Close();    
}
//---------------------------------------------------------------------------
void __fastcall TFo_RandomAnimation::SB_PauseClick(TObject *Sender)
{
    SettingView.m_bAnimationPaused = SB_Pause->Down;    
}
//---------------------------------------------------------------------------
void __fastcall TFo_RandomAnimation::FormShow(TObject *Sender)
{
	ApplyLanguageSetting();

    SettingToGUI();
	m_bDontClose = false;
	m_bFullScreenRequest = false;
	m_bZoomChanged = false;

	Ti_Check->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TFo_RandomAnimation::FormKeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
    if (Key == 27){
        Close();
    }
}
//---------------------------------------------------------------------------
void __fastcall TFo_RandomAnimation::FormClose(TObject *Sender,
      TCloseAction &Action)
{
    Ti_Check->Enabled = false;
}
//---------------------------------------------------------------------------

void __fastcall TFo_RandomAnimation::Ti_CheckTimer(TObject *Sender)
{
    if (SB_FullScreen->Down != Fo_FullScreen->Visible){
        SB_FullScreen->Down = Fo_FullScreen->Visible;
    }
}
//---------------------------------------------------------------------------

void __fastcall TFo_RandomAnimation::SB_FullScreenClick(TObject *Sender)
{
    m_bFullScreenRequest = true;
}
//---------------------------------------------------------------------------

void __fastcall TFo_RandomAnimation::FormCreate(TObject *Sender)
{
    m_bFullScreenRequest = false;
}
//---------------------------------------------------------------------------

void __fastcall TFo_RandomAnimation::TB_ZoomChange(TObject *Sender)
{
    m_bZoomChanged = true;    
}
//---------------------------------------------------------------------------

void __fastcall TFo_RandomAnimation::TB_ZoomKeyDown(TObject *Sender,
      WORD &Key, TShiftState Shift)
{
    //PgUp,Down‚É‚æ‚éƒY[ƒ€
    switch(Key){
	case 33://PgUp
        {
            int pos = TB_Zoom->Position;
            pos -= (TB_Zoom->Max - TB_Zoom->Min) / 50;
            if (pos < TB_Zoom->Min){
                pos = TB_Zoom->Min;
            }
            TB_Zoom->Position = pos;
        }
        Key = 0;
        break;
    case 34://PgDn
        {
            int pos = TB_Zoom->Position;
            pos += (TB_Zoom->Max - TB_Zoom->Min) / 50;
            if (pos > TB_Zoom->Max){
                pos = TB_Zoom->Max;
            }
            TB_Zoom->Position = pos;
        }
        Key = 0;
        break;
    }
}
//---------------------------------------------------------------------------

