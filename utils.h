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
    AnsiString m_Name;
    TStringList *m_KeyValues;
    int SearchKey(AnsiString Key);
    AnsiString Values(int index);
public:
    TFastIniSection(AnsiString Name, TStringList *SL, int start, int end);
    virtual ~TFastIniSection();
public:
    AnsiString ReadString(AnsiString Key, AnsiString Default);
    int ReadInteger(AnsiString Key, int Default);
    float ReadFloat(AnsiString Key, float Default);
};

class TFastIni
{
private:
    TList* m_Sections;//各セクション
    int m_LastSectionIndex;
    AnsiString m_LastSection;
    TFastIniSection *SearchSection(AnsiString Section);
    void LoadFromString(TStringList *SL);
public:
    TFastIni(TStringList *SL);
    TFastIni(AnsiString FN);
    virtual ~TFastIni();
public:
    AnsiString ReadString(AnsiString Section, AnsiString Key, AnsiString Default);
    int ReadInteger(AnsiString Section, AnsiString Key, int Default);
    bool ReadBool(AnsiString Section, AnsiString Key, int Default);
    float ReadFloat(AnsiString Section, AnsiString Key, float Default);
    void ReadSectionValues(AnsiString Section, TStrings *S);
};
//---------------------------------------------------------------------------
//ファイル関連
void FileListCreator(AnsiString TopDir, TStringList *SL, AnsiString Exts, bool SubFolder = true);
bool IsFileNameOrURL(AnsiString S);
//---------------------------------------------------------------------------
//文字列関係
int CountStr(WideString S, WideString CountChar);//CountCharの数を数える
WideString SplitStrBy(WideString &S, WideString SplitChar);//SplitCharの位置でSを分割
WideString ReplaceText(WideString S, WideString From, WideString To);
AnsiString IntToDigit(int i, int digit);
AnsiString SizeToStr(int i);
AnsiString ExtractFileNameOnly(AnsiString S);
AnsiString ForFileName(AnsiString S);//ファイルネームに使用できない文字を除く
AnsiString DeleteActionKey(AnsiString S);//&File, (&F)などのアクションキー文字列を除く
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
