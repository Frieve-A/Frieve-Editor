//---------------------------------------------------------------------------

#ifndef linkH
#define linkH
//---------------------------------------------------------------------------
#include <vcl.h>
#include "label.h"
//---------------------------------------------------------------------------
class TLink
{
public:
    //�f�[�^
    int m_nFromID;//�����N��
    int m_nDestID;//�����N��
    int m_bDirection;//����������Ȃ�
    int m_nShape;//����
    TLabelList *m_Labels;//���x��
    AnsiString m_Name;//�����N��

    //�ꎞ
    bool m_bVisible;
    TColor m_Color;
public:
    //�쐬�A�j��
    void InitLink();
    TLink();
    TLink(AnsiString S);
    TLink(TLink &Link);
    virtual ~TLink();
public:
    //�ǂݏ���
    void Decode(AnsiString S);
    void Decode_005(AnsiString S);//LinkShape�ǉ��O
    void Decode_006(AnsiString S);//���ʌ݊��O
    AnsiString Encode();
};
#endif
