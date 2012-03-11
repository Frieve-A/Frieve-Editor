//---------------------------------------------------------------------------

#ifndef forandomcardH
#define forandomcardH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <Buttons.hpp>
//---------------------------------------------------------------------------
class TFo_RandomCard : public TForm
{
__published:    // IDE 管理のコンポーネント
    TLabel *Label1;
    TLabel *Label2;
    TTrackBar *TB_Cards;
    TTrackBar *TB_Speed;
    TLabel *La_Cards;
    TSpeedButton *SB_Pause;
    TSpeedButton *SB_Stop;
    void __fastcall FormDeactivate(TObject *Sender);
    void __fastcall TB_CardsChange(TObject *Sender);
    void __fastcall TB_SpeedChange(TObject *Sender);
    void __fastcall SB_StopClick(TObject *Sender);
    void __fastcall SB_PauseClick(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
private:    // ユーザー宣言
public:        // ユーザー宣言
    __fastcall TFo_RandomCard(TComponent* Owner);
public:
    void SettingToGUI();
};
//---------------------------------------------------------------------------
extern PACKAGE TFo_RandomCard *Fo_RandomCard;
//---------------------------------------------------------------------------
#endif
