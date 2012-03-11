//---------------------------------------------------------------------------

#ifndef foedittextH
#define foedittextH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
class TFo_EditText : public TForm
{
__published:    // IDE �Ǘ��̃R���|�[�l���g
    TLabel *La_Text;
    TEdit *Ed_Text;
    TButton *Bu_OK;
    TButton *Bu_Cancel;
    void __fastcall Bu_CancelClick(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
private:    // ���[�U�[�錾
public:        // ���[�U�[�錾
    __fastcall TFo_EditText(TComponent* Owner);
public:
    //������Ή�
    void ApplyLanguageSetting();
};
//---------------------------------------------------------------------------
extern PACKAGE TFo_EditText *Fo_EditText;
//---------------------------------------------------------------------------
#endif
