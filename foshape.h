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
__published:    // IDE �Ǘ��̃R���|�[�l���g
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
private:    // ���[�U�[�錾
public:        // ���[�U�[�錾
    __fastcall TFo_Shape(TComponent* Owner);

    TImageList *IL_Shape;
    int m_nMode;//0�`3�ŁA�J�[�h�A�����N�A�S�J�[�h�A�S�����N
    int m_nItemIndex;
    bool m_bIgnoreChange;//�\�������u��Index���ς��̂�h������
    bool m_bSelected;
};
//---------------------------------------------------------------------------
extern PACKAGE TFo_Shape *Fo_Shape;
//---------------------------------------------------------------------------
#endif
