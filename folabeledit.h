//---------------------------------------------------------------------------

#ifndef folabeleditH
#define folabeleditH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Grids.hpp>

#include "document.h"
#include <Dialogs.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TFo_LabelEdit : public TForm
{
__published:    // IDE 管理のコンポーネント
    TButton *Bu_OK;
    TStringGrid *SG_Labels;
    TButton *Bu_Delete;
    TColorDialog *CD;
    TButton *Bu_New;
    TTimer *Ti_Check;
    void __fastcall FormShow(TObject *Sender);
    void __fastcall SG_LabelsDrawCell(TObject *Sender, int ACol, int ARow,
          TRect &Rect, TGridDrawState State);
    void __fastcall SG_LabelsSelectCell(TObject *Sender, int ACol,
          int ARow, bool &CanSelect);
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall SG_LabelsDblClick(TObject *Sender);
    void __fastcall Bu_DeleteClick(TObject *Sender);
    void __fastcall Bu_NewClick(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall Ti_CheckTimer(TObject *Sender);
    void __fastcall FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
private:    // ユーザー宣言
public:        // ユーザー宣言
    __fastcall TFo_LabelEdit(TComponent* Owner);
public:
    TDocument *m_Document;
    TUndoRedo *m_UndoRedo;
    bool m_bEdited;
    int m_LType;
private:
    int m_nLastCol, m_nLastRow;
    void Refresh();
    void BackupSub();
public:
    //多言語対応
    void ApplyLanguageSetting();
};
//---------------------------------------------------------------------------
extern PACKAGE TFo_LabelEdit *Fo_LabelEdit;
//---------------------------------------------------------------------------
#endif
