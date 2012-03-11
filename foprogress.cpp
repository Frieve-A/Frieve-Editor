//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include <mmsystem.hpp>

#include "foprogress.h"
#include "setting.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFo_Progress *Fo_Progress;
//---------------------------------------------------------------------------
void TFo_Progress::ApplyLanguageSetting()
{
    if (!FileExists(LanguageFileName())){
        return;
    }
    TFastIni *Ini = new TFastIni(LanguageFileName());

    Bu_Cancel->Caption = Ini->ReadString("Dialog", "Bu_Cancel", Bu_Cancel->Caption);

    delete Ini;
}
//---------------------------------------------------------------------------
__fastcall TFo_Progress::TFo_Progress(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TFo_Progress::Bu_CancelClick(TObject *Sender)
{
    m_bTerminated = true;
}
//---------------------------------------------------------------------------
void __fastcall TFo_Progress::FormActivate(TObject *Sender)
{
    m_nStartTime = timeGetTime();
    PFunc->ProgressFunc();
    m_bFinished = true;
}
//---------------------------------------------------------------------------
void __fastcall TFo_Progress::FormShow(TObject *Sender)
{
    ApplyLanguageSetting();

    PG_Progress->Position = 0;
    Width = 0;
    Height = 0;
    Be_Progress->Align = alClient;
    m_bFinished = false;
    m_bTerminated = false;
}
//---------------------------------------------------------------------------
void __fastcall TFo_Progress::Ti_CheckTimer(TObject *Sender)
{
    if (m_bFinished){
        Close();
    }else{
        int p = (fPos * PG_Progress->Max) / fMax;
        if (PG_Progress->Position != p){
            PG_Progress->Position = p;
        }
        if (timeGetTime() - m_nStartTime > 500 && PG_Progress->Position < PG_Progress->Max / 2 && Width == 0){
            //•\Ž¦
            int w = PG_Progress->Width + PG_Progress->Left * 2;
            int h = La_Progress->Top + Bu_Cancel->Top + Bu_Cancel->Height;
            Left = Left - w / 2;
            Top = Top - h / 2;
            Width = w;
            Height = h;
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TFo_Progress::FormCreate(TObject *Sender)
{
    fMax = 1.0f;
}
//---------------------------------------------------------------------------
void __fastcall TFo_Progress::FormKeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
    if (Key == 27){
        m_bTerminated = true;
    }
}
//---------------------------------------------------------------------------
