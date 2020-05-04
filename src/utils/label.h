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
    UnicodeString m_Name;
    int m_nColor;

    int m_nSize;//サイズ

    //ネット共有されないデータ（追加の際は、Document->SoftLoadを修正する必要あり）
	int m_bEnable;//有効
	int m_bShow;//表示する
	int m_bHide;//隠す
	int m_bFold;//畳む
public:
	//一時
	float m_fX, m_fY;//ラベルの中心座標
    int m_bFixed;//中心座標固定

    double m_fTouched;//最後に触った日時
    int m_nTouchedOrder;//触った順序
public:
    //作成、破棄
    TCardLabel();
    TCardLabel(UnicodeString S);
    void InitLabel();
    //TCardLabel(TCardLabel &Label);
    virtual ~TCardLabel();
public:
    //読み書き
    void Decode(UnicodeString S);
    void Decode_006(UnicodeString S);//下位互換前
    void Decode_003(UnicodeString S);//Enabled追加前
    void Decode_002(UnicodeString S);//Size追加前
    void Decode_001(UnicodeString S);
    void Decode_Old(UnicodeString S);
    UnicodeString Encode();
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
    UnicodeString Encode();
    void Decode(UnicodeString S);
};
//---------------------------------------------------------------------------
#endif
