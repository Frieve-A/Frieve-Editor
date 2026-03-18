// ---------------------------------------------------------------------------

#ifndef fofullscreenH
#define fofullscreenH
// ---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <Forms.hpp>
#include <StdCtrls.hpp>

// ---------------------------------------------------------------------------
class TFo_FullScreen : public TForm {
__published: // IDE-managed components
  TButton *Bu_Enter;
  TButton *Bu_Dummy;

  void __fastcall FormShow(TObject *Sender);
  void __fastcall FormResize(TObject *Sender);

private: // User declarations
public:  // User declarations
  __fastcall TFo_FullScreen(TComponent *Owner);
};

// ---------------------------------------------------------------------------
extern PACKAGE TFo_FullScreen *Fo_FullScreen;
// ---------------------------------------------------------------------------
#endif
