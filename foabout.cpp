//---------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "foabout.h"
#include "document.h"
#include "setting.h"
//---------------------------------------------------------------------
#pragma resource "*.dfm"
TFo_About *Fo_About;
//---------------------------------------------------------------------
void TFo_About::ApplyLanguageSetting()
{
    if (!FileExists(LanguageFileName())){
        return;
    }
    TFastIni *Ini = new TFastIni(LanguageFileName());

    Caption = Ini->ReadString("About", "Caption", Caption);

    Bu_OK->Caption = Ini->ReadString("Dialog", "Bu_OK", Bu_OK->Caption);

    delete Ini;
}
//---------------------------------------------------------------------
__fastcall TFo_About::TFo_About(TComponent* AOwner)
    : TForm(AOwner)
{
}
//---------------------------------------------------------------------
void __fastcall TFo_About::FormShow(TObject *Sender)
{
    ApplyLanguageSetting();

    ProductName->Caption =
        AnsiString(AppTitle) +
        AnsiString(" Version ") +
        FormatFloat("0.00", AppVersion / 100.0);

    ProgramIcon->Picture->Assign(Application->Icon);
}
//---------------------------------------------------------------------


