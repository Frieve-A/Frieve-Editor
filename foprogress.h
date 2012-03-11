//---------------------------------------------------------------------------

#ifndef foprogressH
#define foprogressH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>

//---------------------------------------------------------------------------
class IProgress
{
public:
    virtual void ProgressFunc() = 0;
};

class TFo_Progress : public TForm
{
__published:	// IDE �Ǘ��̃R���|�[�l���g
    TButton *Bu_Cancel;
    TProgressBar *PG_Progress;
    TTimer *Ti_Check;
    TBevel *Be_Progress;
    TLabel *La_Progress;
    void __fastcall Bu_CancelClick(TObject *Sender);
    void __fastcall FormActivate(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall Ti_CheckTimer(TObject *Sender);
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
private:	// ���[�U�[�錾
public:		// ���[�U�[�錾
    __fastcall TFo_Progress(TComponent* Owner);
public:
    bool m_bTerminated;
    IProgress *PFunc;
    bool m_bFinished;
    unsigned int m_nStartTime;

    float fMax, fPos;
public:
    //������Ή�
    void ApplyLanguageSetting();
};
//---------------------------------------------------------------------------
extern PACKAGE TFo_Progress *Fo_Progress;
//---------------------------------------------------------------------------
#endif
