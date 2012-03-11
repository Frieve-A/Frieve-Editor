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
    //データ
    int m_nFromID;//リンク元
    int m_nDestID;//リンク先
    int m_bDirection;//方向性ありなし
    int m_nShape;//向き
    TLabelList *m_Labels;//ラベル
    AnsiString m_Name;//リンク名

    //一時
    bool m_bVisible;
    TColor m_Color;
public:
    //作成、破棄
    void InitLink();
    TLink();
    TLink(AnsiString S);
    TLink(TLink &Link);
    virtual ~TLink();
public:
    //読み書き
    void Decode(AnsiString S);
    void Decode_005(AnsiString S);//LinkShape追加前
    void Decode_006(AnsiString S);//下位互換前
    AnsiString Encode();
};
#endif
