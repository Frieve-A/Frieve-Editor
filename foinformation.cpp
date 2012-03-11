//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "foinformation.h"
#include "document.h"
#include "setting.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "SHDocVw_OCX"
#pragma resource "*.dfm"
TFo_Information *Fo_Information;
//---------------------------------------------------------------------------
void TFo_Information::ApplyLanguageSetting()
{
	if (!FileExists(LanguageFileName())){
		return;
	}
	TFastIni *Ini = new TFastIni(LanguageFileName());

	Caption = Ini->ReadString("Information", "Caption", Caption);
	Ch_DontCheckNew->Caption = Ini->ReadString("Information", "Ch_DontCheckNew", Ch_DontCheckNew->Caption);

	delete Ini;
}
//---------------------------------------------------------------------
__fastcall TFo_Information::TFo_Information(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TFo_Information::FormShow(TObject *Sender)
{
	ApplyLanguageSetting();

    WB->Align = alClient;
    WideString ws = InfoURL;
    WB->Navigate(ws);
    Ch_DontCheckNew->Checked = !SettingFile.m_bCheckNew;
}
//---------------------------------------------------------------------------
void __fastcall TFo_Information::FormClose(TObject *Sender,
      TCloseAction &Action)
{
    Action = caFree;
}
//---------------------------------------------------------------------------

void __fastcall TFo_Information::Ch_DontCheckNewClick(TObject *Sender)
{
    SettingFile.m_bCheckNew = !Ch_DontCheckNew->Checked;
}
//---------------------------------------------------------------------------
