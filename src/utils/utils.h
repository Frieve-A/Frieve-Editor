// ---------------------------------------------------------------------------

#ifndef utilsH
#define utilsH
// ---------------------------------------------------------------------------
#include <vcl.h>

// ---------------------------------------------------------------------------
// Fast INI reading
class TFastIniSection {
private:
	int m_nLastIndex;

public:
	UnicodeString m_Name;
	TStringList *m_KeyValues;

	int SearchKey(UnicodeString Key);
	UnicodeString Values(int index);
	TFastIniSection(UnicodeString Name, TStringList *SL, int start, int end);
	virtual ~TFastIniSection();
	UnicodeString ReadString(UnicodeString Key, UnicodeString Default);
	int ReadInteger(UnicodeString Key, int Default);
	float ReadFloat(UnicodeString Key, float Default);
private:
	TFastIniSection(const TFastIniSection &source);
	TFastIniSection& operator=(const TFastIniSection &source);
};

class TFastIni {
private:
	TList* m_Sections; // sections
	int m_LastSectionIndex;
	UnicodeString m_LastSection;

	TFastIniSection *SearchSection(UnicodeString Section);
	void LoadFromString(TStringList *SL);

public:
	TFastIni(TStringList *SL);
	TFastIni(UnicodeString FN);
	virtual ~TFastIni();
	UnicodeString ReadString(UnicodeString Section, UnicodeString Key,
		UnicodeString Default);
	int ReadInteger(UnicodeString Section, UnicodeString Key, int Default);
	bool ReadBool(UnicodeString Section, UnicodeString Key, int Default);
	float ReadFloat(UnicodeString Section, UnicodeString Key, float Default);
	void ReadSectionValues(UnicodeString Section, TStrings *S);
private:
	TFastIni(const TFastIni &source);
	TFastIni& operator=(const TFastIni &source);
};

// ---------------------------------------------------------------------------
// File related
void FileListCreator(UnicodeString TopDir, TStringList *SL, UnicodeString Exts,
	bool SubFolder = true);
bool IsFileNameOrURL(UnicodeString S);
// ---------------------------------------------------------------------------
// String related
int CountStr(WideString S, WideString CountChar); // Count occurrences of CountChar
WideString SplitStrBy(WideString &S, WideString SplitChar); // Split S at SplitChar position
WideString ReplaceText(WideString S, WideString From, WideString To);
UnicodeString IntToDigit(int i, int digit);
UnicodeString SizeToStr(int i);
UnicodeString ExtractFileNameOnly(UnicodeString S);
UnicodeString ForFileName(UnicodeString S); // Remove chars invalid for filenames
UnicodeString DeleteActionKey(UnicodeString S); // Remove action key strings like &File, (&F)
// ---------------------------------------------------------------------------
// Color related
TColor HalfColor(int C1, int C2, float A);
TColor InverseColor(int C1);
TColor BrightColor(int C1, float A);
TColor GetColor(float f, int base, int cont);
// ---------------------------------------------------------------------------
// Simple Gaussian random
float GaussianRand();
// ---------------------------------------------------------------------------
#endif
