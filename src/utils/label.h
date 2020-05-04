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
    UnicodeString m_Name;
    int m_nColor;

    int m_nSize;//�T�C�Y

    //�l�b�g���L����Ȃ��f�[�^�i�ǉ��̍ۂ́ADocument->SoftLoad���C������K�v����j
	int m_bEnable;//�L��
	int m_bShow;//�\������
	int m_bHide;//�B��
	int m_bFold;//���
public:
	//�ꎞ
	float m_fX, m_fY;//���x���̒��S���W
    int m_bFixed;//���S���W�Œ�

    double m_fTouched;//�Ō�ɐG��������
    int m_nTouchedOrder;//�G��������
public:
    //�쐬�A�j��
    TCardLabel();
    TCardLabel(UnicodeString S);
    void InitLabel();
    //TCardLabel(TCardLabel &Label);
    virtual ~TCardLabel();
public:
    //�ǂݏ���
    void Decode(UnicodeString S);
    void Decode_006(UnicodeString S);//���ʌ݊��O
    void Decode_003(UnicodeString S);//Enabled�ǉ��O
    void Decode_002(UnicodeString S);//Size�ǉ��O
    void Decode_001(UnicodeString S);
    void Decode_Old(UnicodeString S);
    UnicodeString Encode();
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
    UnicodeString Encode();
    void Decode(UnicodeString S);
};
//---------------------------------------------------------------------------
#endif
