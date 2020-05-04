// ---------------------------------------------------------------------------

#include <math.h>

#include "utils.h"

// ---------------------------------------------------------------------------
// 高速Ini
TFastIniSection::TFastIniSection(UnicodeString Name, TStringList *SL, int start,
	int end) : m_Name(Name), m_nLastIndex(-1) {
	m_KeyValues = new TStringList();
	for (int i = start; i <= end; i++) {
		m_KeyValues->Add(SL->Strings[i]);
	}
}

TFastIniSection::~TFastIniSection() {
	delete m_KeyValues;
}

int TFastIniSection::SearchKey(UnicodeString Key) {
	int index = (m_nLastIndex + 1);
	if (m_KeyValues->Count) {
		index %= m_KeyValues->Count;
	}
	int startindex = index;
	bool first = true;
	while (first || index != startindex) {
		if (m_KeyValues->Names[index] == Key) {
			m_nLastIndex = index;
			return index;
		}
		index++;
		index %= m_KeyValues->Count;
		first = false;
	}
	/*
	 for (int i = 0 ; i < m_KeyValues->Count ; i++){
	 if (m_KeyValues->Names[i] == Key){
	 return i;
	 }
	 }
	 */
	return -1;
}

UnicodeString TFastIniSection::Values(int index) {
	// return m_KeyValues->Values[index];
	int p = m_KeyValues->Strings[index].Pos("=");
	return m_KeyValues->Strings[index].SubString(p + 1,
		m_KeyValues->Strings[index].Length());
}

UnicodeString TFastIniSection::ReadString(UnicodeString Key,
	UnicodeString Default) {
	int index = SearchKey(Key);
	if (index >= 0) {
		return Values(index);
	}
	else {
		return Default;
	}
}

int TFastIniSection::ReadInteger(UnicodeString Key, int Default) {
	int index = SearchKey(Key);
	if (index >= 0) {
		return StrToIntDef(Values(index), Default);
	}
	else {
		return Default;
	}
}

float TFastIniSection::ReadFloat(UnicodeString Key, float Default) {
	int index = SearchKey(Key);
	if (index >= 0) {
		return StrToFloatDef(Values(index), Default);
	}
	else {
		return Default;
	}
}

TFastIni::TFastIni(TStringList *SL) : m_LastSectionIndex(-1) {
	m_Sections = new TList();

	LoadFromString(SL);
}

void TFastIni::LoadFromString(TStringList *SL) {
	int lastline = 0;
	UnicodeString SectionName, NextSectionName;
	for (int i = 0; i < SL->Count; i++) {
		bool Split = false;
		if (SL->Strings[i].SubString(1, 1) == "[") {
			NextSectionName =
				SL->Strings[i].SubString(2, SL->Strings[i].Length() - 2);
			Split = true;
		}

		if (Split) {
			if (SectionName != "") {
				TFastIniSection *FIS =
					new TFastIniSection(SectionName, SL, lastline, i - 1);
				m_Sections->Add(FIS);
			}

			lastline = i + 1;
			SectionName = NextSectionName;
		}
	}
	if (lastline < SL->Count) {
		TFastIniSection *FIS = new TFastIniSection(SectionName, SL, lastline,
			SL->Count - 1);
		m_Sections->Add(FIS);
	}

}

TFastIni::TFastIni(UnicodeString FN) : m_LastSectionIndex(-1) {
	m_Sections = new TList();

	TStringList *SL = new TStringList();
	if (FileExists(FN)) {
		SL->LoadFromFile(FN);
	}

	LoadFromString(SL);

	delete SL;
}

TFastIni::~TFastIni() {
	for (int i = 0; i < m_Sections->Count; i++) {
		delete(TFastIniSection*)m_Sections->Items[i];
	}
	delete m_Sections;
}

// ---------------------------------------------------------------------------
TFastIniSection *TFastIni::SearchSection(UnicodeString Section) {
	if (m_LastSection != Section) {
		// 最後に読んだSectionと違う場合

		// 該当Sectionを検索
		m_LastSectionIndex = -1;
		for (int i = 0; i < m_Sections->Count; i++) {
			if (((TFastIniSection*)m_Sections->Items[i])->m_Name == Section) {
				m_LastSection = Section;
				m_LastSectionIndex = i;
			}
		}
	}

	if (m_LastSectionIndex >= 0) {
		// 該当セクションあり
		return (TFastIniSection*)m_Sections->Items[m_LastSectionIndex];
	}
	else {
		// 該当セクションなし
		return NULL;
	}
}

UnicodeString TFastIni::ReadString(UnicodeString Section, UnicodeString Key,
	UnicodeString Default) {
	TFastIniSection *FIS = SearchSection(Section);
	if (FIS) {
		return FIS->ReadString(Key, Default);
	}
	else {
		return Default;
	}
}

int TFastIni::ReadInteger(UnicodeString Section, UnicodeString Key, int Default)
{
	TFastIniSection *FIS = SearchSection(Section);
	if (FIS) {
		return FIS->ReadInteger(Key, Default);
	}
	else {
		return Default;
	}
}

bool TFastIni::ReadBool(UnicodeString Section, UnicodeString Key, int Default) {
	TFastIniSection *FIS = SearchSection(Section);
	if (FIS) {
		return FIS->ReadInteger(Key, Default);
	}
	else {
		return Default;
	}
}

float TFastIni::ReadFloat(UnicodeString Section, UnicodeString Key,
	float Default) {
	TFastIniSection *FIS = SearchSection(Section);
	if (FIS) {
		return FIS->ReadFloat(Key, Default);
	}
	else {
		return Default;
	}
}

void TFastIni::ReadSectionValues(UnicodeString Section, TStrings *S) {
	S->Clear();
	TFastIniSection *FIS = SearchSection(Section);
	if (FIS) {
		for (int i = 0; i < FIS->m_KeyValues->Count; i++) {
			S->Add(FIS->m_KeyValues->Strings[i]);
		}
	}
	else {
		return;
	}
}

// ---------------------------------------------------------------------------
void FileListCreator(UnicodeString TopDir, TStringList *SL, UnicodeString Exts,
	bool SubFolder) {
	void *Handle;
	TWin32FindData Data;
	Exts = UpperCase(Exts);
	Handle = FindFirstFile((TopDir + "\\*.*").c_str(), &Data);
	if (Handle == INVALID_HANDLE_VALUE)
		return;

	if (!((UnicodeString(Data.cFileName) == ".") ||
		(UnicodeString(Data.cFileName) == ".."))) {
		if (Exts.Pos(ExtractFileExt(Data.cFileName).UpperCase())) {
			SL->Add(TopDir + "\\" + Data.cFileName);
		}

		if ((Data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && SubFolder) {
			FileListCreator(TopDir + "\\" + Data.cFileName, SL, Exts);
		}
	}

	while (FindNextFile(Handle, &Data)) {
		if (!((UnicodeString(Data.cFileName) == ".") ||
			(UnicodeString(Data.cFileName) == ".."))) {
			if (Exts.Pos(ExtractFileExt(Data.cFileName).UpperCase())) {
				SL->Add(TopDir + "\\" + Data.cFileName);
			}

			if ((Data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && SubFolder)
			{
				FileListCreator(TopDir + "\\" + Data.cFileName, SL, Exts);
			}
		}
	}

	FindClose(Handle);
}

// ---------------------------------------------------------------------------
bool IsFileNameOrURL(UnicodeString S) {
	if (S.Pos(":\\")) {
		// ファイル（ルートフォルダのあるもの）
		return true;
	}
	else if (S.Pos("\\\\")) {
		// ネットワーク上のファイル？
		return true;
	}
	else if (S.Pos("://")) {
		// URL
		return true;
	}
	return false;
}

// ---------------------------------------------------------------------------
WideString ReplaceText(WideString S, WideString From, WideString To) {
	WideString result;
	while (int len = S.Length()) {
		int p = S.Pos(From);
		if (p) {
			result += S.SubString(1, p - 1) + To;
			S = S.SubString(p + From.Length(), len);
		}
		else {
			result += S;
			S = "";
		}
	}
	return result;
}

// ---------------------------------------------------------------------------
int CountStr(WideString S, WideString CountChar) // CountCharの数を数える
{
	int result = 0;
	while (true) {
		int p = S.Pos(CountChar);
		if (p > 0) {
			result++;
			S = S.SubString(p + CountChar.Length(), S.Length());
		}
		else {
			break;
		}
	}
	return result;
}

// ---------------------------------------------------------------------------
WideString SplitStrBy(WideString &S, WideString SplitChar) {
	WideString result = "";
	int p = S.Pos(SplitChar);
	if (p > 0) {
		result = S.SubString(1, p - 1);
		S = S.SubString(p + SplitChar.Length(), S.Length());
	}
	else {
		result = S;
		S = "";
	}
	return result;
}

// ---------------------------------------------------------------------------
UnicodeString IntToDigit(int i, int digit) {
	UnicodeString S = "0000000" + IntToStr(i);
	return S.SubString(S.Length() - digit + 1, digit);
}

// ---------------------------------------------------------------------------
UnicodeString SizeToStr(int i) {
	if (i < 1000) {
		return IntToStr(i) + "B";
	}
	else if (i < 10000) {
		return FormatFloat("0.00", i / 1000.0) + "KB";
	}
	else if (i < 100000) {
		return FormatFloat("0.0", i / 1000.0) + "KB";
	}
	else if (i < 1000000) {
		return IntToStr(i / 1000) + "KB";
	}
	else if (i < 10000000) {
		return FormatFloat("0.00", i / 1000000.0) + "MB";
	}
	else if (i < 100000000) {
		return FormatFloat("0.0", i / 1000000.0) + "MB";
	}
	else {
		return IntToStr(i / 1000000) + "MB";
	}
}

// ---------------------------------------------------------------------------
UnicodeString ExtractFileNameOnly(UnicodeString S) {
	S = ExtractFileName(S);
	while (true) {
		UnicodeString Ext = ExtractFileExt(S);
		if (Ext != "") {
			S = S.SubString(1, S.Length() - Ext.Length());
			break; // 1段階だけ拡張子を取る。全部とりきる場合はコメントアウト
		}
		else {
			break;
		}
	}
	return S;
}

// ---------------------------------------------------------------------------
UnicodeString ForFileName(UnicodeString S) {
	UnicodeString XStr = "\/:*?\"<>|";
	for (int i = 0; i < XStr.Length(); i++) {
		while (int pos = S.Pos(XStr.SubString(i + 1, 1))) {
			S = S.SubString(1, pos - 1) + UnicodeString("_") +
				S.SubString(pos + 1, S.Length() - pos);
		}
	}
	return S;
}

// ---------------------------------------------------------------------------
UnicodeString DeleteActionKey(UnicodeString S) {
	// &File, (&F)などのアクションキー文字列を除く

	UnicodeString XStr;

	// 日本語アクションキー
	XStr = "(&";
	while (int pos = S.Pos(XStr)) {
		S = S.SubString(1, pos - 1) + S.SubString(pos + 4, S.Length());
	}

	// 英語アクションキー
	XStr = "&";
	while (int pos = S.Pos(XStr)) {
		S = S.SubString(1, pos - 1) + S.SubString(pos + 1, S.Length());
	}

	// 3点リーダ
	XStr = "...";
	while (int pos = S.Pos(XStr)) {
		S = S.SubString(1, pos - 1) + S.SubString(pos + 3, S.Length());
	}

	return S;
}

// ---------------------------------------------------------------------------
TColor HalfColor(int C1, int C2, float A) {
	int r1 = C1 & 0xff;
	int g1 = (C1 & 0xff00) >> 8;
	int b1 = (C1 & 0xff0000) >> 16;
	int r2 = C2 & 0xff;
	int g2 = (C2 & 0xff00) >> 8;
	int b2 = (C2 & 0xff0000) >> 16;

	r1 = r1 * (1.0f - A) + r2 * A;
	if (r1 < 0) {
		r1 = 0;
	}
	if (r1 > 255) {
		r1 = 255;
	}
	g1 = g1 * (1.0f - A) + g2 * A;
	if (g1 < 0) {
		g1 = 0;
	}
	if (g1 > 255) {
		g1 = 255;
	}
	b1 = b1 * (1.0f - A) + b2 * A;
	if (b1 < 0) {
		b1 = 0;
	}
	if (b1 > 255) {
		b1 = 255;
	}

	return (TColor)(r1 | (g1 << 8) | (b1 << 16));
}

// ---------------------------------------------------------------------------
TColor InverseColor(int C1) {
	int r1 = 255 - (C1 & 0xff);
	int g1 = 255 - ((C1 & 0xff00) >> 8);
	int b1 = 255 - ((C1 & 0xff0000) >> 16);

	return (TColor)(r1 | (g1 << 8) | (b1 << 16));
}

// ---------------------------------------------------------------------------
TColor BrightColor(int C1, float A) {
	int r1 = (C1 & 0xff);
	int g1 = ((C1 & 0xff00) >> 8);
	int b1 = ((C1 & 0xff0000) >> 16);

	r1 *= A;
	g1 *= A;
	b1 *= A;

	if (r1 > 255) {
		r1 = 255;
	}
	if (g1 > 255) {
		g1 = 255;
	}
	if (b1 > 255) {
		b1 = 255;
	}

	return (TColor)(r1 | (g1 << 8) | (b1 << 16));
}

// ---------------------------------------------------------------------------
TColor GetColor(float f, int base, int cont) {
	int R = base, G = base, B = base;
	float f2 = fmod(f, 1.0f / 6) * 6;
	if (f <= 1.0 / 6) {
		R = base + cont;
		G = base + f2 * cont;
	}
	else if (f <= 2.0 / 6) {
		R = base + (1.0f - f2) * cont;
		G = base + cont;
	}
	else if (f <= 3.0 / 6) {
		G = base + cont;
		B = base + f2 * cont;
	}
	else if (f <= 4.0 / 6) {
		G = base + (1.0f - f2) * cont;
		B = base + cont;
	}
	else if (f <= 5.0 / 6) {
		B = base + cont;
		R = base + f2 * cont;
	}
	else {
		B = base + (1.0f - f2) * cont;
		R = base + cont;
	}
	return TColor(R + G * 0x100 + B * 0x10000);
}

// ---------------------------------------------------------------------------
// 簡易ガウス乱数
float GaussianRand() {
	float d = 0.0f;
	for (int i = 0; i < 12; i++) {
		d += rand() % 32769;
	}
	return (d - (32768 * 6)) / 32768;
}
// ---------------------------------------------------------------------------
#pragma package(smart_init)
