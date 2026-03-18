// ---------------------------------------------------------------------------

#ifndef foinformationH
#define foinformationH
// ---------------------------------------------------------------------------
#include "SHDocVw_OCX.h"
#include <Classes.hpp>
#include <Controls.hpp>
#include <ExtCtrls.hpp>
#include <Forms.hpp>
#include <OleCtrls.hpp>
#include <StdCtrls.hpp>

// ---------------------------------------------------------------------------
class TFo_Information : public TForm {
__published: // IDE-managed components
  TCppWebBrowser *WB;
  TPanel *Pa_Top_;
  TCheckBox *Ch_DontCheckNew;

  void __fastcall FormShow(TObject *Sender);
  void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
  void __fastcall Ch_DontCheckNewClick(TObject *Sender);

private: // User declarations
public:  // User declarations
  __fastcall TFo_Information(TComponent *Owner);
  // Multi-language support
  void ApplyLanguageSetting();
};

// ---------------------------------------------------------------------------
extern PACKAGE TFo_Information *Fo_Information;
// ---------------------------------------------------------------------------
#endif
