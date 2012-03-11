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
    //データ
    AnsiString m_Name;
    int m_nColor;

    int m_nSize;//サイズ

    //ネット共有されないデータ（追加の際は、Document->SoftLoadを修正する必要あり）
    bool m_bEnable;//有効
    bool m_bShow;//表示する
    bool m_bHide;//隠す
    bool m_bFold;//畳む
public:
    //一時
    float m_fX, m_fY;//ラベルの中心座標
    bool m_bFixed;//中心座標固定

    double m_fTouched;//最後に触った日時
    int m_nTouchedOrder;//触った順序
public:
    //作成、破棄
    TCardLabel();
    TCardLabel(AnsiString S);
    void InitLabel();
    //TCardLabel(TCardLabel &Label);
    virtual ~TCardLabel();
public:
    //読み書き
    void Decode(AnsiString S);
    void Decode_006(AnsiString S);//下位互換前
    void Decode_003(AnsiString S);//Enabled追加前
    void Decode_002(AnsiString S);//Size追加前
    void Decode_001(AnsiString S);
    void Decode_Old(AnsiString S);
    AnsiString Encode();
};
//---------------------------------------------------------------------------
class TLabelList : public TList
{
public:
    //通常アクセス
    int GetLabel(int index);
    void SetLabel(int index, int labelid);
    void AddLabel(int labelid);
    void DeleteLabel(int labelid);
    bool Contain(int labelid);
    //bool IsSame(TLabelList *Labels);    //Labelのパラメータも見る必要が出てきたため、未使用
public:
    //読み書き
    AnsiString Encode();
    void Decode(AnsiString S);
};
//---------------------------------------------------------------------------
#endif
