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
__published: // IDE 管理のコンポーネント
	TCppWebBrowser *WB;
	TPanel *Pa_Top_;
	TCheckBox *Ch_DontCheckNew;

	void __fastcall FormShow(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall Ch_DontCheckNewClick(TObject *Sender);

private: // ユーザー宣言
public: // ユーザー宣言
	__fastcall TFo_Information(TComponent* Owner);
	// 多言語対応
	void ApplyLanguageSetting();
};

// ---------------------------------------------------------------------------
extern PACKAGE TFo_Information *Fo_Information;
// ---------------------------------------------------------------------------
#endif
