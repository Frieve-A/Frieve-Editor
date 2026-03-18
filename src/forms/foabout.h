// ----------------------------------------------------------------------------
#ifndef foaboutH
#define foaboutH
// ----------------------------------------------------------------------------
#include <Buttons.hpp>
#include <Classes.hpp>
#include <Controls.hpp>
#include <ExtCtrls.hpp>
#include <Forms.hpp>
#include <Graphics.hpp>
#include <StdCtrls.hpp>
#include <SysUtils.hpp>
#include <System.hpp>
#include <Windows.hpp>

// ----------------------------------------------------------------------------
class TFo_About : public TForm {
__published:
  TButton *Bu_OK;
  TImage *ProgramIcon;
  TLabel *ProductName;
  TLabel *Copyright;
  TLabel *Comments;
  TLabel *Label1;

  void __fastcall FormShow(TObject *Sender);

private:
public:
  virtual __fastcall TFo_About(TComponent *AOwner);
  // Multi-language support
  void ApplyLanguageSetting();
};

// ----------------------------------------------------------------------------
extern PACKAGE TFo_About *Fo_About;
// ----------------------------------------------------------------------------
#endif
