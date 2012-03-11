//---------------------------------------------------------------------------

#ifndef labelH
#define labelH
//---------------------------------------------------------------------------
#include <vcl.h>
//---------------------------------------------------------------------------
#define MAXLABELS 16
//---------------------------------------------------------------------------
class TCardLabel
{
public:
    //�f�[�^
    AnsiString m_Name;
    int m_nColor;

    int m_nSize;//�T�C�Y

    //�l�b�g���L����Ȃ��f�[�^�i�ǉ��̍ۂ́ADocument->SoftLoad���C������K�v����j
    bool m_bEnable;//�L��
    bool m_bShow;//�\������
    bool m_bHide;//�B��
    bool m_bFold;//���
public:
    //�ꎞ
    float m_fX, m_fY;//���x���̒��S���W
    bool m_bFixed;//���S���W�Œ�

    double m_fTouched;//�Ō�ɐG��������
    int m_nTouchedOrder;//�G��������
public:
    //�쐬�A�j��
    TCardLabel();
    TCardLabel(AnsiString S);
    void InitLabel();
    //TCardLabel(TCardLabel &Label);
    virtual ~TCardLabel();
public:
    //�ǂݏ���
    void Decode(AnsiString S);
    void Decode_006(AnsiString S);//���ʌ݊��O
    void Decode_003(AnsiString S);//Enabled�ǉ��O
    void Decode_002(AnsiString S);//Size�ǉ��O
    void Decode_001(AnsiString S);
    void Decode_Old(AnsiString S);
    AnsiString Encode();
};
//---------------------------------------------------------------------------
class TLabelList : public TList
{
public:
    //�ʏ�A�N�Z�X
    int GetLabel(int index);
    void SetLabel(int index, int labelid);
    void AddLabel(int labelid);
    void DeleteLabel(int labelid);
    bool Contain(int labelid);
    //bool IsSame(TLabelList *Labels);    //Label�̃p�����[�^������K�v���o�Ă������߁A���g�p
public:
    //�ǂݏ���
    AnsiString Encode();
    void Decode(AnsiString S);
};
//---------------------------------------------------------------------------
#endif
