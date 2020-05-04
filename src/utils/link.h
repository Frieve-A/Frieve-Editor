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
    UnicodeString m_Name;//�����N��

    //�ꎞ
    bool m_bVisible;
    TColor m_Color;
public:
    //�쐬�A�j��
    void InitLink();
    TLink();
    TLink(UnicodeString S);
    TLink(TLink &Link);
    virtual ~TLink();
public:
    //�ǂݏ���
    void Decode(UnicodeString S);
    void Decode_005(UnicodeString S);//LinkShape�ǉ��O
    void Decode_006(UnicodeString S);//���ʌ݊��O
    UnicodeString Encode();
};
#endif
