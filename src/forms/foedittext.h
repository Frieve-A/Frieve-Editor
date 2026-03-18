// ---------------------------------------------------------------------------

#ifndef foedittextH
#define foedittextH
// ---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <Forms.hpp>
#include <StdCtrls.hpp>

// ---------------------------------------------------------------------------
class TFo_EditText : public TForm {
__published: // IDE-managed components
  TLabel *La_Text;
  TEdit *Ed_Text;
  TButton *Bu_OK;
  TButton *Bu_Cancel;

  void __fastcall Bu_CancelClick(TObject *Sender);
  void __fastcall FormShow(TObject *Sender);

private: // User declarations
public:  // User declarations
  __fastcall TFo_EditText(TComponent *Owner);
  // Multi-language support
  void ApplyLanguageSetting();
};

// ---------------------------------------------------------------------------
extern PACKAGE TFo_EditText *Fo_EditText;
// ---------------------------------------------------------------------------
#endif
