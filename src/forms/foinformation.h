// ---------------------------------------------------------------------------

#ifndef foinformationH
#define foinformationH
// ---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "SHDocVw_OCX.h"
#include <OleCtrls.hpp>
#include <ExtCtrls.hpp>

// ---------------------------------------------------------------------------
class TFo_Information : public TForm {
__published: // IDE �Ǘ��̃R���|�[�l���g
	TCppWebBrowser *WB;
	TPanel *Pa_Top_;
	TCheckBox *Ch_DontCheckNew;

	void __fastcall FormShow(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall Ch_DontCheckNewClick(TObject *Sender);

private: // ���[�U�[�錾
public: // ���[�U�[�錾
	__fastcall TFo_Information(TComponent* Owner);
	// ������Ή�
	void ApplyLanguageSetting();
};

// ---------------------------------------------------------------------------
extern PACKAGE TFo_Information *Fo_Information;
// ---------------------------------------------------------------------------
#endif
