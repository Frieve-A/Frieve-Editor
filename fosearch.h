//---------------------------------------------------------------------------

#ifndef fosearchH
#define fosearchH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Menus.hpp>
//---------------------------------------------------------------------------
class TFo_Search : public TForm
{
__published:    // IDE �Ǘ��̃R���|�[�l���g
    TLabel *La_Keyword;
    TButton *Bu_FindNext;
    TButton *Bu_Cancel;
    TCheckBox *Ch_CardTitle;
    TCheckBox *Ch_Text;
    TPopupMenu *Po_Search;
    TMenuItem *PM_Cut;
    TMenuItem *PM_Copy;
    TMenuItem *PM_Paste;
    TMenuItem *PM_Undo;
    TMenuItem *PM_SelectAll;
    TCheckBox *Ch_MatchCase;
    TCheckBox *Ch_GlobalSearch;
    TCheckBox *Ch_Backward;
    TComboBox *Co_Keyword;
    TEdit *Ed_Keyword;
    TCheckBox *Ch_InvisibleCard;
    TLabel *La_ReplaceTo;
    TEdit *Ed_ReplaceTo;
    void __fastcall Bu_CancelClick(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall Ed_KeywordChange(TObject *Sender);
    void __fastcall Bu_FindNextClick(TObject *Sender);
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall PM_UndoClick(TObject *Sender);
    void __fastcall PM_CutClick(TObject *Sender);
    void __fastcall PM_CopyClick(TObject *Sender);
    void __fastcall PM_PasteClick(TObject *Sender);
    void __fastcall PM_SelectAllClick(TObject *Sender);
    void __fastcall Co_KeywordChange(TObject *Sender);
    void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
private:    // ���[�U�[�錾
public:        // ���[�U�[�錾
    __fastcall TFo_Search(TComponent* Owner);
public:
    //������Ή�
    void ApplyLanguageSetting();
    int bReplace;//�u�����[�h
};
//---------------------------------------------------------------------------
extern PACKAGE TFo_Search *Fo_Search;
extern WideString SearchKeyword;//�����L�[���[�h
extern WideString ReplaceKeyword;//�u����L�[���[�h
extern int bSearchRequest;//�����v���B0x1=CardTitle, 0x2=Text, 0x8=�����_�C�A���O���猟���A0x4 �u���E�U��Find�{�^�����猟��,0x40 Global Search, 0x80 backward, 0x100 �u��
extern int bLastSearchRequest;//�����v���B0x1=CardTitle, 0x2=Text, 0x8=�����_�C�A���O���猟���A0x4 �u���E�U��Find�{�^�����猟��,0x40 Global Search
//---------------------------------------------------------------------------
#endif
