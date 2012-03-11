//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "fofullscreen.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFo_FullScreen *Fo_FullScreen;
//---------------------------------------------------------------------------
__fastcall TFo_FullScreen::TFo_FullScreen(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TFo_FullScreen::FormShow(TObject *Sender)
{
    Left = 0;
    Top = 0;
    Width = Screen->Width;
    Height = Screen->Height;     
}
//---------------------------------------------------------------------------
void __fastcall TFo_FullScreen::FormResize(TObject *Sender)
{
    Bu_Enter->Left = Width + 100;
    Bu_Dummy->Left = Width + 100;
}
//---------------------------------------------------------------------------

