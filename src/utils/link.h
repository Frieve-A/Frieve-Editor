// ---------------------------------------------------------------------------

#ifndef linkH
#define linkH
// ---------------------------------------------------------------------------
#include <vcl.h>
#include "label.h"

// ---------------------------------------------------------------------------
class TLink {
public:
	// データ
	int m_nFromID; // リンク元
	int m_nDestID; // リンク先
	int m_bDirection; // 方向性ありなし
	int m_nShape; // 向き
	TLabelList *m_Labels; // ラベル
	UnicodeString m_Name; // リンク名

	// 一時
	bool m_bVisible;
	TColor m_Color;

	// 作成、破棄
	void InitLink();
	TLink();
	TLink(UnicodeString S);
	TLink(TLink &Link);
	virtual ~TLink();
	// 読み書き
	void Decode(UnicodeString S);
	void Decode_005(UnicodeString S); // LinkShape追加前
	void Decode_006(UnicodeString S); // 下位互換前
	UnicodeString Encode();
};
#endif
