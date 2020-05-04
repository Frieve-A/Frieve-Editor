//---------------------------------------------------------------------------

#ifndef drawingH
#define drawingH
//---------------------------------------------------------------------------
#include <vcl.h>
//---------------------------------------------------------------------------
class TDrawingItem : public TList
{
public:
	TDrawingItem();
	TDrawingItem(TDrawingItem &Source);
	TDrawingItem(UnicodeString S);//文字列からデコード
	__fastcall virtual ~TDrawingItem();

	UnicodeString Encode();//文字列で記述
private:
	void UpdateFreeHandRect(float x, float y);
	void FinishFreeHandRect();

	float m_fDragStartX, m_fDragStartY;//ドラッグ開始座標
	float m_DragRect[4];//描画座標。ドラッグ時一時的に使う

public:
	int m_nType;//要素のタイプ。1=フリーハンド、2=直線、3=正方形、4=円、5=テキスト
	TColor m_nPenColor;//線の色。0x7fffffffでFGColor、-1で色なし
	TColor m_nBrushColor;//ブラシの色
	float m_Rect[4];//描画座標。フリーハンドの場合は要素の含まれる矩形。直線は始点と終点。テキストは左上座標

	bool m_bSelected;//選択されているか
	bool m_bExist;//表示できる状況にあるか

	void SetType(int nType);
	bool MouseDown(float x, float y);//帰り値は編集されたかどうか
	bool MouseMove(float x, float y);
	bool MouseUp(float x, float y);

	int CheckDrag(float x, float y, int size);//ドラッグ開始可能かを調べる
	bool Drag(float x, float y, int dragmode);

	void Draw(TCanvas *C, TRect &R);

	int SetPenColor(TColor C);//色を変更。変更できたらtrueを返す
	int SetBrushColor(TColor C);
};
//---------------------------------------------------------------------------
class TDrawing : public TList
{
public:
	TDrawing();
	TDrawing(TDrawing &Source);//コピーコンストラクタ
	TDrawing(UnicodeString S);//文字列からデコード
	__fastcall virtual ~TDrawing();

	UnicodeString Encode(bool bAll = false);//文字列で記述。bAll=falseで選択されているアイテムのみ。bAll=trueですべて
private:
	TDrawingItem *m_TmpItem;
	void FreeTmpItem();
	TDrawingItem *DItem(int index);
public:
	int m_nTool;//現在のツール。0=選択、1=フリーハンド、2=直線、3=正方形、4=円、5=テキスト
	bool m_bDrawRequest;
	int m_nDragMode;//0x10～0x13=端っこのドラッグ、0x20=移動
	int m_nDragIndex;//端っこをドラッグ中のItemのIndex
	float m_fDragX, m_fDragY;//ドラッグ開始座標
	TColor m_nPenColor;//線の色。0x7fffffffでFGColor、-1で色なし
	TColor m_nBrushColor;//ブラシの色

	bool m_bModified;//編集されたかどうか

	void SetTool(int nToolID);
	void MouseDown(float x, float y, int size, TShiftState Shift);
	void MouseMove(float x, float y);
	void MouseUp(float x, float y);

	void Draw(TCanvas *C, TRect &R);

	void SelectRect(float x1, float y1, float x2, float y2);
	void ClearSelection();
	void SelectAll();
	int Selected();//選択中の要素数を返す

	//編集操作
	void Cut();
	void Copy();
	void Paste();
	void DDelete();

	void ChangePenColor(TColor C);
	void ChangeBrushColor(TColor C);
};
//---------------------------------------------------------------------------
#endif
