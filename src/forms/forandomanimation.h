// ---------------------------------------------------------------------------

#ifndef forandomanimationH
#define forandomanimationH
// ---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>

// ---------------------------------------------------------------------------
class TFo_RandomAnimation : public TForm {
__published: // IDE 管理のコンポーネント
	TLabel *La_Cards;
	TLabel *La_Speed;
	TTrackBar *TB_Cards;
	TTrackBar *TB_Speed;
	TLabel *La_CardNum;
	TSpeedButton *SB_Pause;
	TSpeedButton *SB_Stop;
	TLabel *La_Zoom;
	TTrackBar *TB_Zoom;
	TSpeedButton *SB_FullScreen;
	TTimer *Ti_Check;

	void __fastcall FormDeactivate(TObject *Sender);
	void __fastcall TB_CardsChange(TObject *Sender);
	void __fastcall TB_SpeedChange(TObject *Sender);
	void __fastcall SB_StopClick(TObject *Sender);
	void __fastcall SB_PauseClick(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall Ti_CheckTimer(TObject *Sender);
	void __fastcall SB_FullScreenClick(TObject *Sender);
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall TB_ZoomChange(TObject *Sender);
	void __fastcall TB_ZoomKeyDown(TObject *Sender, WORD &Key,
		TShiftState Shift);

private: // ユーザー宣言
public: // ユーザー宣言
	__fastcall TFo_RandomAnimation(TComponent* Owner);

	bool m_bFullScreenRequest; // フルスクリーン切り替え要求
	bool m_bDontClose;
	bool m_bZoomChanged;

	void SettingToGUI();
	// 多言語対応
	void ApplyLanguageSetting();
};

// ---------------------------------------------------------------------------
extern PACKAGE TFo_RandomAnimation *Fo_RandomAnimation;
// ---------------------------------------------------------------------------
#endif
