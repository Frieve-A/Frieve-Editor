//---------------------------------------------------------------------------

#ifndef fofullscreenH
#define fofullscreenH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
class TFo_FullScreen : public TForm
{
__published:	// IDE �Ǘ��̃R���|�[�l���g
    TButton *Bu_Enter;
    TButton *Bu_Dummy;
    void __fastcall FormShow(TObject *Sender);
    void __fastcall FormResize(TObject *Sender);
private:	// ���[�U�[�錾
public:		// ���[�U�[�錾
    __fastcall TFo_FullScreen(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFo_FullScreen *Fo_FullScreen;
//---------------------------------------------------------------------------
#endif
