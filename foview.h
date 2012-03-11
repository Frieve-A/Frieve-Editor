//---------------------------------------------------------------------------

#ifndef foviewH
#define foviewH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <CheckLst.hpp>
#include "document.h"
#include <Grids.hpp>
#include <Menus.hpp>
//---------------------------------------------------------------------------
class TFo_View : public TForm
{
__published:	// IDE 管理のコンポーネント
    TPageControl *PC_View;
    TTabSheet *TS_Label;
    TTabSheet *TS_Limitation;
	TLabel *La_Label;
	TLabel *La_LabelShow;
	TLabel *La_LabelHide;
	TLabel *La_LabelFold;
    TButton *Bu_LabelShowAll;
    TButton *Bu_LabelShowClear;
    TButton *Bu_LabelHideAll;
    TButton *Bu_LabelFoldAll;
    TButton *Bu_LabelClearAll;
    TButton *Bu_LabelHideClear;
    TStringGrid *SG_Labels;
	TLabel *La_LinkLabel;
	TLabel *La_LinkLabelShow;
	TLabel *La_LinkLabelHide;
    TButton *Bu_LinkLabelShowAll;
    TButton *Bu_LinkLabelShowClear;
    TButton *Bu_LinkLabelHideAll;
    TButton *Bu_LinkLabelHideClear;
    TStringGrid *SG_LinkLabels;
    TCheckBox *Ch_SizeLimitation;
    TCheckBox *Ch_DateLimitation;
    TBevel *Bevel1;
    TGroupBox *GB_SizeLimitation;
    TTrackBar *TB_SizeLimitation;
    TLabel *La_Size;
    TGroupBox *GB_LinkLimitation;
    TLabel *La_Links;
    TTrackBar *TB_LinkLimitation;
    TGroupBox *GB_DateLimitation;
	TLabel *La_DateType;
	TLabel *La_DateLimitationType;
    TTrackBar *TB_DateLimitation;
    TComboBox *Co_DateType;
    TComboBox *Co_DateLimitationType;
    TRadioButton *RB_LinkForward;
    TRadioButton *RB_LinkBackward;
	TLabel *La_LinkTarget;
    TCheckBox *Ch_LinkDirection;
    TButton *Bu_LinkTarget;
    TCheckBox *Ch_LinkLimitation;
    TPopupMenu *PM_LinkTarget;
    TMenuItem *PT_NoAssign;
    TMenuItem *N1;
    TMenuItem *PT_Select;
    void __fastcall FormDeactivate(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall TB_LinkLimitationChange(TObject *Sender);
    void __fastcall Bu_LabelShowAllClick(TObject *Sender);
    void __fastcall Bu_LabelHideAllClick(TObject *Sender);
    void __fastcall Bu_LabelFoldAllClick(TObject *Sender);
    void __fastcall Ch_LinkDirectionClick(TObject *Sender);
    void __fastcall SG_LabelsMouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall SG_LabelsDrawCell(TObject *Sender, int ACol, int ARow,
          TRect &Rect, TGridDrawState State);
    void __fastcall Ch_SizeLimitationClick(TObject *Sender);
    void __fastcall TB_SizeLimitationChange(TObject *Sender);
    void __fastcall Ch_DateLimitationClick(TObject *Sender);
    void __fastcall TB_DateLimitationChange(TObject *Sender);
    void __fastcall Co_DateTypeChange(TObject *Sender);
    void __fastcall Co_DateLimitationTypeChange(TObject *Sender);
    void __fastcall SG_LabelsKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
    void __fastcall RB_LinkForwardClick(TObject *Sender);
    void __fastcall Ch_LinkLimitationClick(TObject *Sender);
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall PT_NoAssignClick(TObject *Sender);
    void __fastcall PT_SelectClick(TObject *Sender);
    void __fastcall Bu_LinkTargetClick(TObject *Sender);
    void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
private:	// ユーザー宣言
public:		// ユーザー宣言
    __fastcall TFo_View(TComponent* Owner);
public:
    TDocument *m_Document;
    bool m_bRefreshRequest;
private:
    void SettingToGUI();
    void RefreshLinkTarget();//LinkTargetボタン上のテキスト
public:
    //多言語対応
    void ApplyLanguageSetting();
};
//---------------------------------------------------------------------------
extern PACKAGE TFo_View *Fo_View;
//---------------------------------------------------------------------------
#endif
