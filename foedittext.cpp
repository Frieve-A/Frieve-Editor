//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "foedittext.h"
#include "setting.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFo_EditText *Fo_EditText;
//---------------------------------------------------------------------------
void TFo_EditText::ApplyLanguageSetting()
{
    if (!FileExists(LanguageFileName())){
        return;
    }
    TFastIni *Ini = new TFastIni(LanguageFileName());

    Bu_OK->Caption = Ini->ReadString("Dialog", "Bu_OK", Bu_OK->Caption);
    Bu_Cancel->Caption = Ini->ReadString("Dialog", "Bu_Cancel", Bu_Cancel->Caption);

    delete Ini;
}
//---------------------------------------------------------------------
__fastcall TFo_EditText::TFo_EditText(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------


void __fastcall TFo_EditText::Bu_CancelClick(TObject *Sender)
{
    Close();
}
//---------------------------------------------------------------------------
void __fastcall TFo_EditText::FormShow(TObject *Sender)
{
    ApplyLanguageSetting();

    Ed_Text->SelectAll();    
}
//---------------------------------------------------------------------------

