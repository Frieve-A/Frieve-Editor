// ---------------------------------------------------------------------------

#ifndef documentH
#define documentH
// ---------------------------------------------------------------------------
#include <vcl.h>
// ---------------------------------------------------------------------------
#include "card.h"
#include "label.h"
// ---------------------------------------------------------------------------
/*
 helpに以下を追加

 ReadOnly=1
 DefaultView=1
 */

#define AppTitle "Frieve Editor"
#define AppVersion 140
#define FileVersion 007
#define AppURL "http://www.frieve.com/"
#define SoftURL "http://www.frieve.com/feditor/"
#define InfoURL "http://www.frieve.com/feditor/info.html"

class IDocCtrl {
public:
	virtual int Request(char *Type, int Value, float fValue, void *option) = 0;
	virtual TCard *NewCard(int insertindex) = 0;
	virtual void SortCards(int sorttype, bool inverse) = 0;
	virtual TCard* GetCardByIndex(int nIndex) = 0;
	virtual TCard* GetCard(int nID) = 0;
	virtual void SetCardTitle(TCard *Card, UnicodeString S) = 0;
	virtual void SetCardText(TCard *Card, UnicodeString S) = 0;
	// virtual void SetCardText(int nID, UnicodeString S) = 0;
	virtual void DeleteCard(int nID) = 0;
	virtual int SearchCardIndex(int nID) = 0;
	virtual void ClearCards() = 0;
	virtual void ClearCardSelection() = 0;
	virtual void SwapCard(int idx1, int idx2) = 0;
	virtual int SearchParent(int CardID, bool bChild, bool bFocus) = 0;
	// 親（もしくは子）カードを捜す
	virtual int SearchBrother(int CurrentID, int ParentID, bool bInverse,
		bool bChild, bool bFocus) = 0; // 時計回り（もしくは逆周り）に兄弟ノードを探す
	virtual int SearchLast(int CardID, bool bFocus) = 0; // 直前に表示していたカードを捜す
	virtual void RefreshCardLevel() = 0; // Card->m_bTopとリンクに従って階層レベル設定、カードソート

	virtual void AddLabelToCard(TCard* Card, int label) = 0;
	virtual void DeleteLabelFromCard(TCard* Card, int label) = 0;
	// データアクセス（リンク）
	virtual TLink *NewLink() = 0;
	virtual TLink *GetLinkByIndex(int index) = 0;
	virtual void SetLinkName(TLink *Link, UnicodeString S) = 0;
	virtual void DeleteLinkByIndex(int index) = 0;
	virtual void ClearLinks() = 0;

	virtual void AddLabelToLink(TLink* Link, int label) = 0;
	virtual void DeleteLabelFromLink(TLink* Link, int label) = 0;

	// データアクセス（ラベル）
	virtual TCardLabel *NewLabel(int ltype) = 0;
	virtual TCardLabel *GetLabelByIndex(int ltype, int index) = 0;
	virtual TCardLabel *GetLabel(int ltype, UnicodeString S) = 0;
	virtual void SetLabelName(TCardLabel *Label, UnicodeString S) = 0;
	virtual void DeleteLabelByIndex(int ltype, int index) = 0;
	virtual void InitLabel(int ltype) = 0;
	virtual void ClearLabels(int ltype) = 0;

	virtual bool LabelIsFold(TCard *Card) = 0; // カードのラベルが全て折りたたまれているか調べる
	virtual int CountEnableLabel(TCard *Card) = 0; // カードの有効なラベル数を得る
	virtual bool LabelIsSame(TCard *Card1, TCard *Card2) = 0;
	// 2つのカードのラベルが同じか調べる
	// 表示更新用
	virtual void RefreshList() = 0;
	virtual void RefreshLink() = 0;
	virtual void RefreshLabel() = 0;
	// ファイル
	virtual bool Load(UnicodeString FN, bool bSoftLoad = false) = 0;
	// SoftLoadでは、元データの座標を維持
	virtual bool Save() = 0;
	virtual bool LoadFromString(TStringList *SL, UnicodeString FN) = 0;
	virtual bool SoftLoadFromString(TStringList *SL, UnicodeString FN) = 0;
	virtual bool SaveToString(TStringList *SL) = 0;
	// 情報取得
	virtual int GetCheckCount() = 0; // 1回画面が更新されるたびにインクリメント
	virtual int GetCardID() = 0; // 現在フォーカス中のカードID
	virtual int CardCount() = 0; // カード数
	virtual int LabelCount(int ltype) = 0; // ラベル数
	virtual int LinkCount() = 0; // リンク数
	// クリップボード
	virtual void CopyToClipboard() = 0;
	virtual void PasteFromClipboard(float fSpan) = 0;
};

class TDocument : public IDocCtrl {
public:
	// 作成、破棄
	void InitDocument();
	TDocument();
	void CopyFrom(TDocument *Doc);
	TDocument(TDocument &Doc);
	virtual ~TDocument();

	// コピー対象データ
	// データ
	TList *m_Cards;

	// リンクデータ
	TList *m_Links;

	// ラベルデータ
	TList *m_Labels[2];

	// 画面更新カウンタ
	int m_nCheckCount;
	// 表示更新用
	int m_nRefreshListCount;
	int m_nRefreshLinkCount;
	int m_nRefreshLabelCount;

	// その他
	bool m_bChanged;
	UnicodeString m_FN;
	bool m_bReadOnly;

	int m_nCardID; // 現在表示中のカード（保存用）
	int m_nDefaultView; // 読み込んだ瞬間どの画面を表示するか（-1=なし、0=Browser、1=Editor）
	// データアクセス
	int m_nMaxCardID;
	int* m_CardIDToIndex;

	void CreateCardIDToIndex();
	void FreeCardIDToIndex();

	TCard *NewCard(int insertindex);
	void SortCards(int sorttype, bool inverse);
	TCard* GetCardByIndex(int nIndex);
	TCard* GetCard(int nID);
	void SetCardTitle(TCard *Card, UnicodeString S);
	void SetCardText(TCard *Card, UnicodeString S);
	// void SetCardText(int nID, char *szChar);
	void DeleteCard(int nID);
	int SearchCardIndex(int nID);
	void ClearCards();
	void ClearCardSelection();
	void SwapCard(int idx1, int idx2);
	void RefreshDateOrder(); // m_fCreatedOrderなどに正しい値を代入
	void RefreshDateOrder_Label(); // m_fTouchedOrderなどに正しい値を代入
	int SearchParent(int CardID, bool bChild, bool bFocus); // 親（もしくは子）カードを捜す
	int SearchBrother(int CurrentID, int ParentID, bool bInverse, bool bChild,
		bool bFocus); // 時計回り（もしくは逆周り）に兄弟ノードを探す
	int SearchLast(int CardID, bool bFocus); // 直前に表示していたカードを捜す
	void RefreshCardLevel(); // Card->m_bTopとリンクに従って階層レベル設定、カードソート

	void AddLabelToCard(TCard* Card, int label);
	void DeleteLabelFromCard(TCard* Card, int label);

	TList *GetRelatedCard(bool bInverse, bool bVisibleOnly);
	// 各カードから張られたリンク先のカードをIndex順に入れる
	int RelatedCardNum(TList *RCard, int cardindex);
	int RelatedIndex(TList *RCard, int cardindex, int index);
	void FreeRelatedCard(TList *RCard); // RCardの破棄

private:
	inline TCard* GetCardByIndex_(int nIndex);

public:
	// データアクセス（リンク）
	TLink *NewLink();
	inline TLink *GetLinkByIndex(int index);
	void SetLinkName(TLink *Link, UnicodeString S);
	void DeleteLinkByIndex(int index);
	void ClearLinks();

	void AddLabelToLink(TLink* Link, int label);
	void DeleteLabelFromLink(TLink* Link, int label);

	// データアクセス（ラベル）
	TCardLabel *NewLabel(int ltype);
	inline TCardLabel *GetLabelByIndex(int ltype, int index);
	TCardLabel *GetLabel(int ltype, UnicodeString S);
	void SetLabelName(TCardLabel *Label, UnicodeString S);
	void DeleteLabelByIndex(int ltype, int index);
	void InitLabel(int ltype);
	void ClearLabels(int ltype);

	bool LabelIsFold(TCard *Card); // カードのラベルが全て折りたたまれているか調べる
	int CountEnableLabel(TCard *Card); // カードの有効なラベル数を得る
	bool LabelIsSame(TCard *Card1, TCard *Card2); // 2つのカードのラベルが同じか調べる

	// 表示更新用
	void RefreshList();
	void RefreshLink();
	void RefreshLabel();
	// ファイル
	bool Load(UnicodeString FN, bool bSoftLoad = false);
	// SoftLoadでは、元データの座標を維持
	bool Save();
	bool LoadFromString(TStringList *SL, UnicodeString FN);
	bool SoftLoadFromString(TStringList *SL, UnicodeString FN);
	bool SaveToString(TStringList *SL);

	// フォルダ分けしていた時の互換用
	bool Load_Old(UnicodeString FN);
	bool Save_Old();
	int Request(char *Type, int Value, float fValue, void *option);

	// 情報取得
	int GetCheckCount(); // 1回画面が更新されるたびにインクリメント
	int GetCardID(); // 現在フォーカス中のカードID
	int CardCount(); // カード数
	int LabelCount(int ltype); // ラベル数
	int LinkCount(); // リンク数
	// クリップボード
	void CopyToClipboard();
	void PasteFromClipboard(float fSpan);
};

class TUndoRedoData {
public:
	UnicodeString m_Name;
	TDocument *m_Doc;
	int m_nCardID; // 編集中のカードID
	int m_nSelStart; // エディタ上のカーソル位置
	int m_nSelLength;

	TUndoRedoData(TDocument *Doc, UnicodeString Name, int m_nCardID = -1,
		int SelStart = 0, int SelLength = 0);
	virtual ~TUndoRedoData();
};

class TUndoRedo {
private:
	int m_nMaxUndo;
	TList *m_Undos;
	TList *m_Redos;

	inline TUndoRedoData *UndoData(int index);
	inline TUndoRedoData *RedoData(int index);

public:
	TUndoRedo(int maxundo);
	virtual ~TUndoRedo();
	// Undo、Redo
	void ClearUndos();
	void ClearRedos();
	void Backup(TDocument *Doc, UnicodeString editname, int CardID = -1,
		int SelStart = 0, int SelLength = 0);
	void Undo(TDocument *Doc, int CardID, int SelStart, int SelLength,
		int *NextCardID, int *NextSelStart, int *NextSelLength);
	void Redo(TDocument *Doc, int CardID, int SelStart, int SelLength,
		int *NextCardID, int *NextSelStart, int *NextSelLength);
	bool GetCanUndo(UnicodeString &editname);
	bool GetCanRedo(UnicodeString &editname);

	int m_bChanged; // Undo、Redoが行われたことを示す
};

// ---------------------------------------------------------------------------
// ドキュメントからの操作リクエスト(-1でリクエストなし)
extern int bReqArrange; // アレンジのON/OFF
extern int nReqArrangeMode; // 0から順にRepulsion、Link、Label、Index
extern int bReqAutoScroll; // オートスクロール
extern int bReqAutoZoom; // オートズーム
extern int bReqFullScreen; // フルスクリーン
extern int bReqExit; // 終了
extern float fReqZoom, fReqX, fReqY; // ズームと座標
extern int nReqTargetCard; // ターゲットカード変更
extern int bReqSizeLimitation, bReqLinkLimitation, bReqDateLimitation;
// 表示制限のON/OFF
extern int nReqSizeLimitation;
extern int nReqLinkLimitation, bReqLinkDirection, bReqLinkBackward,
	nReqLinkTarget;
extern int nReqDateLimitation, ReqDateLimitationDateType, ReqDateLimitationType;
extern int nReqKeyDown; // 所定のキーを押した処理をする
#endif
