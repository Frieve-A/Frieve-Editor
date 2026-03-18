// ---------------------------------------------------------------------------

#ifndef foprogressH
#define foprogressH
// ---------------------------------------------------------------------------
#include <Classes.hpp>
#include <ComCtrls.hpp>
#include <Controls.hpp>
#include <ExtCtrls.hpp>
#include <Forms.hpp>
#include <StdCtrls.hpp>

// ---------------------------------------------------------------------------
class IProgress : public IUnknown {
public:
  virtual void ProgressFunc() = 0;
};

class TFo_Progress : public TForm {
__published: // IDE-managed components
  TButton *Bu_Cancel;
  TProgressBar *PG_Progress;
  TTimer *Ti_Check;
  TBevel *Be_Progress;
  TLabel *La_Progress;

  void __fastcall Bu_CancelClick(TObject *Sender);
  void __fastcall FormActivate(TObject *Sender);
  void __fastcall FormShow(TObject *Sender);
  void __fastcall Ti_CheckTimer(TObject *Sender);
  void __fastcall FormCreate(TObject *Sender);
  void __fastcall FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);

private: // User declarations
public:  // User declarations
  __fastcall TFo_Progress(TComponent *Owner);

  bool m_bTerminated;
  IProgress *PFunc;
  bool m_bFinished;
  unsigned int m_nStartTime;

  float fMax, fPos;

  // Multi-language support
  void ApplyLanguageSetting();
};

// ---------------------------------------------------------------------------
extern PACKAGE TFo_Progress *Fo_Progress;
// ---------------------------------------------------------------------------
#endif
