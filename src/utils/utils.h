//---------------------------------------------------------------------------

#ifndef utilsH
#define utilsH
//---------------------------------------------------------------------------
#include <vcl.h>
//---------------------------------------------------------------------------
//高速Ini読み込み
class TFastIniSection
{
private:
    int m_nLastIndex;
public:
    UnicodeString m_Name;
    TStringList *m_KeyValues;
    int SearchKey(UnicodeString Key);
    UnicodeString Values(int index);
public:
    TFastIniSection(UnicodeString Name, TStringList *SL, int start, int end);
    virtual ~TFastIniSection();
public:
    UnicodeString ReadString(UnicodeString Key, UnicodeString Default);
    int ReadInteger(UnicodeString Key, int Default);
    float ReadFloat(UnicodeString Key, float Default);
};

class TFastIni
{
private:
    TList* m_Sections;//各セクション
    int m_LastSectionIndex;
    UnicodeString m_LastSection;
    TFastIniSection *SearchSection(UnicodeString Section);
    void LoadFromString(TStringList *SL);
public:
    TFastIni(TStringList *SL);
    TFastIni(UnicodeString FN);
    virtual ~TFastIni();
public:
    UnicodeString ReadString(UnicodeString Section, UnicodeString Key, UnicodeString Default);
    int ReadInteger(UnicodeString Section, UnicodeString Key, int Default);
    bool ReadBool(UnicodeString Section, UnicodeString Key, int Default);
    float ReadFloat(UnicodeString Section, UnicodeString Key, float Default);
    void ReadSectionValues(UnicodeString Section, TStrings *S);
};
//---------------------------------------------------------------------------
//ファイル関連
void FileListCreator(UnicodeString TopDir, TStringList *SL, UnicodeString Exts, bool SubFolder = true);
bool IsFileNameOrURL(UnicodeString S);
//---------------------------------------------------------------------------
//文字列関係
int CountStr(WideString S, WideString CountChar);//CountCharの数を数える
WideString SplitStrBy(WideString &S, WideString SplitChar);//SplitCharの位置でSを分割
WideString ReplaceText(WideString S, WideString From, WideString To);
UnicodeString IntToDigit(int i, int digit);
UnicodeString SizeToStr(int i);
UnicodeString ExtractFileNameOnly(UnicodeString S);
UnicodeString ForFileName(UnicodeString S);//ファイルネームに使用できない文字を除く
UnicodeString DeleteActionKey(UnicodeString S);//&File, (&F)などのアクションキー文字列を除く
//---------------------------------------------------------------------------
//色関係
TColor HalfColor(int C1, int C2, float A);
TColor InverseColor(int C1);
TColor BrightColor(int C1, float A);
TColor GetColor(float f, int base, int cont);
//---------------------------------------------------------------------------
//簡易ガウス乱数
float GaussianRand();
//---------------------------------------------------------------------------
#endif
