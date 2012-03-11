//---------------------------------------------------------------------------

#ifndef foselectcardH
#define foselectcardH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
#include "document.h"
//---------------------------------------------------------------------------
class TFo_Select : public TForm
{
__published:    // IDE �Ǘ��̃R���|�[�l���g
    TButton *Bu_OK;
    TButton *Bu_Cancel;
    TListBox *LB_Items;
    void __fastcall FormShow(TObject *Sender);
    void __fastcall Bu_OKClick(TObject *Sender);
    void __fastcall LB_ItemsClick(TObject *Sender);
    void __fastcall LB_ItemsDblClick(TObject *Sender);
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall FormDestroy(TObject *Sender);
private:    // ���[�U�[�錾
public:        // ���[�U�[�錾
    __fastcall TFo_Select(TComponent* Owner);
public:
    TDocument *m_Document;
    int m_nType;
    int m_nID;
    TList *m_IDs;
    bool m_bMultiSelect;
public:
    //������Ή�
    void ApplyLanguageSetting();
};
//---------------------------------------------------------------------------
extern PACKAGE TFo_Select *Fo_Select;
//---------------------------------------------------------------------------
#endif
