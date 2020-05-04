// ---------------------------------------------------------------------------

#include "label.h"

// ---------------------------------------------------------------------------
TCardLabel::TCardLabel() {
	InitLabel();
}

// ---------------------------------------------------------------------------
void TCardLabel::InitLabel() {
	m_Name = "";
	m_nColor = 0x0;
	m_bEnable = true;
	m_bShow = true;
	m_bHide = false;
	m_bFold = false;

	m_nSize = 100;

	m_fX = -1000.0f;
	m_fY = -1000.0f;
	m_bFixed = false;

	m_fTouched = 0.0;
	m_nTouchedOrder = 0;
}

// ---------------------------------------------------------------------------
TCardLabel::TCardLabel(UnicodeString S) {
	InitLabel();
	Decode(S);
}

// ---------------------------------------------------------------------------
TCardLabel::~TCardLabel() {
}

// ---------------------------------------------------------------------------
void TCardLabel::Decode(UnicodeString S) {
	WideString S2 = S;

	while (S2 != WideString("")) {
		WideString S1;
		int p = S2.Pos(",");
		if (p) {
			S1 = S2.SubString(1, p - 1);
			S2 = S2.SubString(p + 1, S2.Length());
		}
		else {
			S1 = S2;
			S2 = "";
		}

		WideString Key = S1.SubString(1, 2);
		S1 = S1.SubString(3, S1.Length());

		if (Key == WideString("Co")) {
			m_nColor = StrToIntDef(S1, 0);
		}
		else if (Key == WideString("En")) {
			m_bEnable = StrToIntDef(S1, 0);
		}
		else if (Key == WideString("Sh")) {
			m_bShow = StrToIntDef(S1, 0);
		}
		else if (Key == WideString("Hi")) {
			m_bHide = StrToIntDef(S1, 0);
		}
		else if (Key == WideString("Fo")) {
			m_bFold = StrToIntDef(S1, 0);
		}
		else if (Key == WideString("Si")) {
			m_nSize = StrToIntDef(S1, 0);
		}
		else if (Key == WideString("Na")) {
			m_Name = S1;
		}
	}
}

// ---------------------------------------------------------------------------
void TCardLabel::Decode_006(UnicodeString S) {
	int p;
	WideString S2 = S;

	p = S2.Pos(",");
	m_nColor = StrToIntDef(S2.SubString(1, p - 1), 0);
	S2 = S2.SubString(p + 1, S2.Length());

	p = S2.Pos(",");
	m_bEnable = StrToIntDef(S2.SubString(1, p - 1), 0);
	S2 = S2.SubString(p + 1, S2.Length());

	p = S2.Pos(",");
	m_bShow = StrToIntDef(S2.SubString(1, p - 1), 0);
	S2 = S2.SubString(p + 1, S2.Length());

	p = S2.Pos(",");
	m_bHide = StrToIntDef(S2.SubString(1, p - 1), 0);
	S2 = S2.SubString(p + 1, S2.Length());

	p = S2.Pos(",");
	m_bFold = StrToIntDef(S2.SubString(1, p - 1), 0);
	S2 = S2.SubString(p + 1, S2.Length());

	p = S2.Pos(",");
	m_nSize = StrToIntDef(S2.SubString(1, p - 1), 0);
	S2 = S2.SubString(p + 1, S2.Length());

	m_Name = S2;
}

// ---------------------------------------------------------------------------
void TCardLabel::Decode_003(UnicodeString S) {
	int p;
	WideString S2 = S;

	p = S2.Pos(",");
	m_nColor = StrToIntDef(S2.SubString(1, p - 1), 0);
	S2 = S2.SubString(p + 1, S2.Length());

	p = S2.Pos(",");
	m_bShow = StrToIntDef(S2.SubString(1, p - 1), 0);
	S2 = S2.SubString(p + 1, S2.Length());

	p = S2.Pos(",");
	m_bHide = StrToIntDef(S2.SubString(1, p - 1), 0);
	S2 = S2.SubString(p + 1, S2.Length());

	p = S2.Pos(",");
	m_bFold = StrToIntDef(S2.SubString(1, p - 1), 0);
	S2 = S2.SubString(p + 1, S2.Length());

	p = S2.Pos(",");
	m_nSize = StrToIntDef(S2.SubString(1, p - 1), 0);
	S2 = S2.SubString(p + 1, S2.Length());

	m_Name = S2;
}

// ---------------------------------------------------------------------------
void TCardLabel::Decode_002(UnicodeString S) {
	int p;
	WideString S2 = S;

	p = S2.Pos(",");
	m_nColor = StrToIntDef(S2.SubString(1, p - 1), 0);
	S2 = S2.SubString(p + 1, S2.Length());

	p = S2.Pos(",");
	m_bShow = StrToIntDef(S2.SubString(1, p - 1), 0);
	S2 = S2.SubString(p + 1, S2.Length());

	p = S2.Pos(",");
	m_bHide = StrToIntDef(S2.SubString(1, p - 1), 0);
	S2 = S2.SubString(p + 1, S2.Length());

	p = S2.Pos(",");
	m_bFold = StrToIntDef(S2.SubString(1, p - 1), 0);
	S2 = S2.SubString(p + 1, S2.Length());

	m_Name = S2;
}

// ---------------------------------------------------------------------------
void TCardLabel::Decode_001(UnicodeString S) {
	int p;
	WideString S2 = S;

	p = S2.Pos(",");
	m_nColor = StrToIntDef(S2.SubString(1, p - 1), 0);
	S2 = S2.SubString(p + 1, S2.Length());

	p = S2.Pos(",");
	m_bShow = StrToIntDef(S2.SubString(1, p - 1), 0);
	S2 = S2.SubString(p + 1, S2.Length());

	p = S2.Pos(",");
	m_bHide = StrToIntDef(S2.SubString(1, p - 1), 0);
	S2 = S2.SubString(p + 1, S2.Length());

	m_Name = S2;
}

// ---------------------------------------------------------------------------
void TCardLabel::Decode_Old(UnicodeString S) {
	int p;
	WideString S2 = S;
	p = S2.Pos(",");
	m_Name = S2.SubString(1, p - 1);
	S = S2.SubString(p + 1, S2.Length());
	m_nColor = StrToIntDef(S, 0);
}

// ---------------------------------------------------------------------------
UnicodeString TCardLabel::Encode() {
	return UnicodeString("Co") + IntToStr(m_nColor) + // Co
		UnicodeString(",") + UnicodeString("En") + IntToStr(m_bEnable) + // En
		UnicodeString(",") + UnicodeString("Sh") + IntToStr(m_bShow) + // Sh
		UnicodeString(",") + UnicodeString("Hi") + IntToStr(m_bHide) + // Hi
		UnicodeString(",") + UnicodeString("Fo") + IntToStr(m_bFold) + // Fo
		UnicodeString(",") + UnicodeString("Si") + IntToStr(m_nSize) + // Si
		UnicodeString(",") + UnicodeString("Na") + m_Name; // Na
}

// ---------------------------------------------------------------------------
int TLabelList::GetLabel(int index) {
	return (int)Items[index];
}

// ---------------------------------------------------------------------------
void TLabelList::SetLabel(int index, int labelid) {
	if (Items[index] == (void*)labelid) {
		// 変更の必要なし
		return;
	}

	// 重複チェック
	bool exist = false;
	for (int il = 0; il < Count; il++) {
		if (GetLabel(il) == labelid) {
			// 重複
			exist = true;
			break;
		}
	}

	if (exist) {
		// 重複あり

		// 変更前のラベルを消して終了
		DeleteLabel((int)Items[index]);
	}
	else {
		// 重複無し
		Items[index] = (void*)labelid;
	}
}

// ---------------------------------------------------------------------------
void TLabelList::AddLabel(int labelid) {
	if (Count >= MAXLABELS) {
		// これ以上付けられない
		return;
	}

	for (int i = 0; i < Count; i++) {
		if (GetLabel(i) == labelid) {
			// 重複
			return;
		}
	}

	Add((void*)labelid);
}

// ---------------------------------------------------------------------------
void TLabelList::DeleteLabel(int labelid) {
	for (int i = 0; i < Count; i++) {
		if (GetLabel(i) == labelid) {
			// 削除
			Delete(i);
			break;
		}
	}
}

// ---------------------------------------------------------------------------
bool TLabelList::Contain(int labelid) {
	for (int i = 0; i < Count; i++) {
		if (GetLabel(i) == labelid) {
			return true;
		}
	}
	return false;
}

/*
 //---------------------------------------------------------------------------
 bool TLabelList::IsSame(TLabelList *Labels)
 {
 //Labelのパラメータも見る必要が出てきたため、未使用
 if (Count != Labels->Count){
 return false;
 }
 for (int i = 0 ; i < Count ; i++){
 if (!Labels->Contain(GetLabel(i))){
 return false;
 }
 }
 return true;
 }
 */
// ---------------------------------------------------------------------------
UnicodeString TLabelList::Encode() {
	UnicodeString result;
	for (int i = 0; i < Count; i++) {
		if (i > 0) {
			result += ",";
		}

		result += IntToStr(GetLabel(i));
	}

	return result;
}

// ---------------------------------------------------------------------------
void TLabelList::Decode(UnicodeString S) {
	while (S != "") {
		int p = S.Pos(",");
		UnicodeString S2;
		if (p) {
			S2 = S.SubString(1, p - 1);
			S = S.SubString(p + 1, S.Length());
		}
		else {
			S2 = S;
			S = "";
		}
		int labelid = StrToIntDef(S2, 0);
		if (labelid > 0) {
			AddLabel(labelid);
		}
	}
}
// ---------------------------------------------------------------------------

#pragma package(smart_init)
