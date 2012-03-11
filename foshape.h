//---------------------------------------------------------------------------

#ifndef foshapeH
#define foshapeH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TFo_Shape : public TForm
{
__published:    // IDE 管理のコンポーネント
    TPanel *Pa;
    TPaintBox *PB;
    void __fastcall FormDeactivate(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall PBPaint(TObject *Sender);
    void __fastcall PBMouseMove(TObject *Sender, TShiftState Shift, int X,
          int Y);
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
    void __fastcall PBMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
private:    // ユーザー宣言
public:        // ユーザー宣言
    __fastcall TFo_Shape(TComponent* Owner);

    TImageList *IL_Shape;
    int m_nMode;//0～3で、カード、リンク、全カード、全リンク
    int m_nItemIndex;
    bool m_bIgnoreChange;//表示した瞬間Indexが変わるのを防ぐため
    bool m_bSelected;
};
//---------------------------------------------------------------------------
extern PACKAGE TFo_Shape *Fo_Shape;
//---------------------------------------------------------------------------
#endif
