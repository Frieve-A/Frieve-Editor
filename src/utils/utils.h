//---------------------------------------------------------------------------

#ifndef utilsH
#define utilsH
//---------------------------------------------------------------------------
#include <vcl.h>
//---------------------------------------------------------------------------
//����Ini�ǂݍ���
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
    TList* m_Sections;//�e�Z�N�V����
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
//�t�@�C���֘A
void FileListCreator(UnicodeString TopDir, TStringList *SL, UnicodeString Exts, bool SubFolder = true);
bool IsFileNameOrURL(UnicodeString S);
//---------------------------------------------------------------------------
//������֌W
int CountStr(WideString S, WideString CountChar);//CountChar�̐��𐔂���
WideString SplitStrBy(WideString &S, WideString SplitChar);//SplitChar�̈ʒu��S�𕪊�
WideString ReplaceText(WideString S, WideString From, WideString To);
UnicodeString IntToDigit(int i, int digit);
UnicodeString SizeToStr(int i);
UnicodeString ExtractFileNameOnly(UnicodeString S);
UnicodeString ForFileName(UnicodeString S);//�t�@�C���l�[���Ɏg�p�ł��Ȃ�����������
UnicodeString DeleteActionKey(UnicodeString S);//&File, (&F)�Ȃǂ̃A�N�V�����L�[�����������
//---------------------------------------------------------------------------
//�F�֌W
TColor HalfColor(int C1, int C2, float A);
TColor InverseColor(int C1);
TColor BrightColor(int C1, float A);
TColor GetColor(float f, int base, int cont);
//---------------------------------------------------------------------------
//�ȈՃK�E�X����
float GaussianRand();
//---------------------------------------------------------------------------
#endif
