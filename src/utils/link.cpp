// ---------------------------------------------------------------------------

#include "link.h"
#include "setting.h"

// ---------------------------------------------------------------------------
TLink::TLink() {
	InitLink();
}

// ---------------------------------------------------------------------------
void TLink::InitLink() {
	m_nFromID = 0;
	m_nDestID = 0;
	m_bDirection = true;
	m_Labels = new TLabelList();
	m_Name = "";

	// m_nShape = 0;
	m_nShape = SettingView.m_nLinkDefaultShape;

	m_bVisible = true;
	m_Color = TColor(0);
}

// ---------------------------------------------------------------------------
TLink::TLink(UnicodeString S) {
	InitLink();
	Decode(S);
}

// ---------------------------------------------------------------------------
TLink::TLink(TLink &Link) {
	m_Labels = new TLabelList();
	for (int i = 0; i < Link.m_Labels->Count; i++) {
		m_Labels->AddLabel(Link.m_Labels->GetLabel(i));
	}

	m_nFromID = Link.m_nFromID;
	m_nDestID = Link.m_nDestID;
	m_bDirection = Link.m_bDirection;
	m_Name = Link.m_Name;

	m_nShape = Link.m_nShape;

	m_bVisible = Link.m_bVisible;
	m_Color = Link.m_Color;
}

// ---------------------------------------------------------------------------
TLink::~TLink() {
	delete m_Labels;
}

// ---------------------------------------------------------------------------
void TLink::Decode(UnicodeString S) {
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

		if (Key == WideString("Fr")) {
			m_nFromID = StrToIntDef(S1, 0);
		}
		else if (Key == WideString("De")) {
			m_nDestID = StrToIntDef(S1, 0);
		}
		else if (Key == WideString("Di")) {
			m_bDirection = StrToIntDef(S1, 0);
		}
		else if (Key == WideString("Sh")) {
			m_nShape = StrToIntDef(S1, 0);
		}
		else if (Key == WideString("La")) {
			m_Labels->AddLabel(StrToIntDef(S1, 0));
		}
		else if (Key == WideString("Na")) {
			m_Name = S1;
		}
	}
}

// ---------------------------------------------------------------------------
void TLink::Decode_006(UnicodeString S) {
	int p;
	p = S.Pos(",");
	m_nFromID = StrToIntDef(S.SubString(1, p - 1), 0);
	S = S.SubString(p + 1, S.Length());

	p = S.Pos(",");
	if (p == 0) {
		p = S.Length() + 1;
	}
	m_nDestID = StrToIntDef(S.SubString(1, p - 1), 0);
	S = S.SubString(p + 1, S.Length());

	p = S.Pos(",");
	if (p == 0) {
		p = S.Length() + 1;
	}
	m_bDirection = StrToIntDef(S.SubString(1, p - 1), true);
	S = S.SubString(p + 1, S.Length());

	p = S.Pos(",");
	if (p == 0) {
		p = S.Length() + 1;
	}
	m_nShape = StrToIntDef(S.SubString(1, p - 1),
		SettingView.m_nLinkDefaultShape);
	S = S.SubString(p + 1, S.Length());

	p = S.Pos(",");
	if (p == 0) {
		p = S.Length() + 1;
	}
	int labelnum = StrToIntDef(S.SubString(1, p - 1), 0);
	S = S.SubString(p + 1, S.Length());

	for (int i = 0; i < labelnum; i++) {
		p = S.Pos(",");
		if (p == 0) {
			p = S.Length() + 1;
		}
		m_Labels->AddLabel(StrToIntDef(S.SubString(1, p - 1), 0));
		S = S.SubString(p + 1, S.Length());
	}

	p = S.Pos(",");
	if (p == 0) {
		p = S.Length() + 1;
	}
	m_Name = S.SubString(1, p - 1);
}

// ---------------------------------------------------------------------------
void TLink::Decode_005(UnicodeString S) {
	int p;
	p = S.Pos(",");
	m_nFromID = StrToIntDef(S.SubString(1, p - 1), 0);
	S = S.SubString(p + 1, S.Length());

	p = S.Pos(",");
	if (p == 0) {
		p = S.Length() + 1;
	}
	m_nDestID = StrToIntDef(S.SubString(1, p - 1), 0);
	S = S.SubString(p + 1, S.Length());

	p = S.Pos(",");
	if (p == 0) {
		p = S.Length() + 1;
	}
	m_bDirection = StrToIntDef(S.SubString(1, p - 1), true);
	S = S.SubString(p + 1, S.Length());

	m_nShape = 0;

	p = S.Pos(",");
	if (p == 0) {
		p = S.Length() + 1;
	}
	int labelnum = StrToIntDef(S.SubString(1, p - 1), 0);
	S = S.SubString(p + 1, S.Length());

	for (int i = 0; i < labelnum; i++) {
		p = S.Pos(",");
		if (p == 0) {
			p = S.Length() + 1;
		}
		m_Labels->AddLabel(StrToIntDef(S.SubString(1, p - 1), 0));
		S = S.SubString(p + 1, S.Length());
	}

	p = S.Pos(",");
	if (p == 0) {
		p = S.Length() + 1;
	}
	m_Name = S.SubString(1, p - 1);
}

// ---------------------------------------------------------------------------
UnicodeString TLink::Encode() {
	UnicodeString S = UnicodeString("Fr") + IntToStr(m_nFromID) + // Fr
		UnicodeString(",") + UnicodeString("De") + IntToStr(m_nDestID) + // De
		UnicodeString(",") + UnicodeString("Di") +
		IntToStr(m_bDirection) + // Di
		UnicodeString(",") + UnicodeString("Sh") + IntToStr(m_nShape); // Sh
	/*
	 UnicodeString(",") +
	 UnicodeString("") + IntToStr(m_Labels->Count);
	 */

	for (int i = 0; i < m_Labels->Count; i++) {
		S += UnicodeString(",La") + IntToStr(m_Labels->GetLabel(i)); // La
	}

	S += UnicodeString(",Na") + // Na
		m_Name;

	return S;
}
// ---------------------------------------------------------------------------

#pragma package(smart_init)
