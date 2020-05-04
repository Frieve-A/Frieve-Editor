//---------------------------------------------------------------------------

#ifndef cardH
#define cardH
//---------------------------------------------------------------------------
#include <vcl.h>
//---------------------------------------------------------------------------
#include "link.h"
#include "label.h"
#include "drawing.h"
//---------------------------------------------------------------------------
WideString EncodeES(WideString S);//エスケープシーケンスのエンコード（13→\\n、\→\\）
WideString DecodeES(WideString S, WideString Ret = "\r\n");//エスケープシーケンスのデコード（13←\\n、\←\\）

class TCard
{
public:
	//※データを追加したら、初期化とコピーコンストラクタも編集

	//データ
	int m_nID;
	TLabelList *m_Labels;//ラベル
	UnicodeString m_Title;//タイトル
	TStringList *m_Lines;//本文
	TDrawing *m_Drawing;//絵
	int m_bVisible;//表示、非表示
	int m_nShape;//形
	int m_nSize;//サイズ（100分率）
	int m_bTop;//階層のトップ。デフォルトでfalse
	int m_bFixed;//位置固定。
	int m_bFold;//折りたたみ。デフォルトでfalse

	//ネット共有されないデータ（追加の際は、Document->SoftLoadを修正する必要あり）
	double m_fX, m_fY;//座標
	double m_fCreated, m_fUpdated, m_fViewed;//日付
public:
	//保存されないデータ
	int m_bGetFocus;//カーソル移動でフォーカスを得るかどうか

	int m_nLevel;//階層

	int m_nCreatedOrder, m_nUpdatedOrder, m_nViewedOrder;//日付をソート順で0.0～100.0に正規化したもの
	double m_fScore;//スコア
	UnicodeString m_ImageFN;//画像ファイル名（本文で最初に出てきた画像ファイル）
	UnicodeString m_VideoFN;//ビデオファイル名（本文で最初に出てきた動画ファイル）

	int m_nMatrixX, m_nMatrixY;//格子状に配置した際の座標
	float m_fMatrixSpeed;//格子間を移動する速度（～1.0）

	int m_bLabelIsFold;//このカードの全てのラベルが折りたたまれているかどうか
	TColor m_Color;//描画時の色

	float m_fTickerPos;//Ticker。右端から何ドット左にスクロールしているか

	int m_bVisibleBak;//Visible情報のバックアップ（Visibleの変化を捉える）

	int m_bSelected;
	int m_nSelected;//選択アニメーション用（0=非選択、100=選択）

	int m_nParentID;//階層表示時の親カードID
	int m_bHasChild;//階層表示時の子カードがあるかどうか

	int m_nTmp;//テンポラリ。重複使用に注意
public:
	//作成、破棄
	TCard();
	TCard(TCard &Card);
	virtual ~TCard();
public:
	//Util
	void CheckImageFN();//画像、ビデオなどよく参照される外部リンクのファイル名をチェックする
public:
	//読み書き
	void LoadFromString(TStringList *SL, int &line, int version);
	void SaveToString(TStringList *SL);

	//互換用
	void LoadFromFile(UnicodeString FN);
	void SaveToFile(UnicodeString FN);
};
//---------------------------------------------------------------------------
#endif
