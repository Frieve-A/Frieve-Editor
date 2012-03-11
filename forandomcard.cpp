//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "forandomcard.h"
#include "setting.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFo_RandomCard *Fo_RandomCard;
//---------------------------------------------------------------------------
__fastcall TFo_RandomCard::TFo_RandomCard(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------

void __fastcall TFo_RandomCard::FormDeactivate(TObject *Sender)
{
    Close();
}
//---------------------------------------------------------------------------
void TFo_RandomCard::SettingToGUI()
{
    TB_Cards->Position = SettingView.m_nAnimationRCCards;
    TB_CardsChange(this);
    TB_Speed->Position = SettingView.m_nAnimationRCSpeed;
    SB_Pause->Down = SettingView.m_bAnimationPaused;
}
//---------------------------------------------------------------------------
void __fastcall TFo_RandomCard::TB_CardsChange(TObject *Sender)
{
    La_Cards->Caption = TB_Cards->Position;
    SettingView.m_nAnimationRCCards = TB_Cards->Position;
}
//---------------------------------------------------------------------------
void __fastcall TFo_RandomCard::TB_SpeedChange(TObject *Sender)
{
    SettingView.m_nAnimationRCSpeed = TB_Speed->Position;
}
//---------------------------------------------------------------------------
void __fastcall TFo_RandomCard::SB_StopClick(TObject *Sender)
{
    Close();    
}
//---------------------------------------------------------------------------
void __fastcall TFo_RandomCard::SB_PauseClick(TObject *Sender)
{
    SettingView.m_bAnimationPaused = SB_Pause->Down;    
}
//---------------------------------------------------------------------------
void __fastcall TFo_RandomCard::FormShow(TObject *Sender)
{
    SettingToGUI();    
}
//---------------------------------------------------------------------------
void __fastcall TFo_RandomCard::FormKeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
    if (Key == 27){
        Close();
    }
}
//---------------------------------------------------------------------------
