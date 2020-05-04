//---------------------------------------------------------------------------

#ifndef settingH
#define settingH
//---------------------------------------------------------------------------
#include <vcl.h>
#include <IniFiles.hpp>
#include "document.h"
#include "utils.h"
//---------------------------------------------------------------------------

class TSettingView
{
public:
    //�����A�j��
    TSettingView();
    virtual ~TSettingView();
public:
    //�ǂݏ���
    void WriteToIni(TIniFile *Ini, UnicodeString Section);
	void ReadFromIni(TIniFile *Ini, UnicodeString Section);
public:
    //�ݒ�

    //Font
    TFont *m_Font;

	//Link
    int m_nLinkDefaultShape;
    bool m_bLinkShadow;
    bool m_bLinkHemming;
    bool m_bLinkVisible;
    bool m_bLinkDirectionVisible;
    bool m_bLinkNameVisible;

    //Label
    bool m_bLabelCircleVisible;
    bool m_bLabelRectangleVisible;
    bool m_bLavelCRFocusedOnTop;//�t�H�[�J�X���ꂽ�B���ʂ͔�g�p�B�ǂݍ��ݐ�p�B
    bool m_bLabelFill;
    bool m_bLabelNameVisible;

    //Card
    bool m_bFocusCursor;
    bool m_bFocusCursorAnimation;
    int m_nCardDefaultShape;
    int m_nCardDefaultSize;
    bool m_bCardDefaultFixed;
    bool m_bTickerVisible;//�J�[�h�̈�ԉ���Ticker Marquee�\��
    int m_nTickerLines;
    bool m_bCardShadow;
    bool m_bCardHemming;
    bool m_bCardGradation;
    bool m_bImageVisible;
    bool m_bVideoVisible;
    bool m_bDrawingVisible;
	int m_nImageLimitation;
    bool m_bMagnifyFocused;//�t�H�[�J�X���ꂽ�J�[�h���g��

    //Text
    bool m_bTextVisible;
    bool m_bTextCentering;
    bool m_bTextWordWrap;

    bool m_bEditInBrowser;//Browser��Ŗ{���ҏW
    bool m_bEditInBrowserAlways;//Browser��Ŗ{���ҏW�i��Ɂj
    int m_nEditInBrowserPos;//�ҏW�ʒu�B0=�J�[�h�̉��A1=�u���E�U�E�A2=�u���E�U��
    int m_nEditorWidthInBrowser;//Browser��ŕҏW����ۂ�Width
    int m_nEditorHeightInBrowser;//Browser��ŕҏW����ۂ�Height

    //Size Limitation
    int m_nSizeLimitation;//�T�C�Y�����ȏ�̃J�[�h��\�����邩�B�f�t�H���g��100�i�W���T�C�Y�ȏ�ŕ\���j
    bool m_bSizeLimitation;//�T�C�Y�����̗L���B�f�t�H���g�͂Ȃ�

    //Link Limitation
    int m_bLinkLimitation;//�����N����ON�OFF
    int m_nLinkLimitation;//�������N�H���ĕ\�����邩�B�f�t�H���g��0�i�S�\���j
    bool m_bLinkDirection;//�����N�����̐����B�f�t�H���g�͂Ȃ�
    bool m_bLinkBackward;//�t���������N����
    int m_nLinkTarget;//�����N�^�[�Q�b�g�J�[�h��ID

    //Date Limitation
    bool m_bDateLimitation;//���t�ɂ��\������ON/OFF
    int m_nDateLimitation;//���t�ɂ��\�������̑傫���i�Ӗ���DateLimitationType�ɂ��j
    int m_DateLimitationType;//���t�ɂ�鐧���̎�ށiOld, New, Near, Older, Newer�j
    int m_DateLimitationDateType;//���t�̎�ށiCreated, Edited, Viewed�j

    //CardVisible
    bool m_bNoLabelVisible;
    bool m_bNoLinkLabelVisible;

    //�A�j���[�V����
    bool m_bAnimationPaused;//�|�[�Y

    int m_nAnimationRCCards;//RandomCard�ŕ\������J�[�h��
    int m_nAnimationRCSpeed;//RandomCard�̃A�j���[�V�������x

    //�w�i�A�j���[�V����
    bool m_bBGAnimation;
    int m_nBGAnimationType;
    bool m_bNoScrollLag;

    //Score
    bool m_bScore;//�J�[�h�̃X�R�A
    int m_ScoreType;//0=Authenticity, 1=Starting Point, 2=Destination

    //Read
    bool m_bRead;//�ǂݏグ
    UnicodeString m_ReadAgentFile;//�G�[�W�F���g

    //Others
    bool m_bCardList;
    bool m_bFileList;

    int m_nFourgroundColor;
    int m_nBackgroundColor;

    bool m_bAntiAlias;
    int m_nAntiAliasX;

    UnicodeString m_WallPaper;
    bool m_bFixWallPaper;
    bool m_bTileWallPaper;

    bool m_bOverview;//�S�̐}

    //����
    UnicodeString m_Language;

    //��ʔz�u
    int m_nFileListWidth;
    int m_nCardListWidth;
    int m_nLinkListHeight;
    bool m_bEditorRelatedVisible;
    int m_nEditorRelatedWidth;
    int m_nGlobalSearchWidth;

    //Status�o�[
    bool m_bStatusBar;

    //���j���[�ɂȂ�����
    bool m_bLoadImageatOnce;//�T���l�C���Ȃǂ̉摜����x�ɑS�ă��[�h
    bool m_bRedrawInBackground;//�o�b�N�O���E���h��Ԃł���ʂ��X�V

    //�ꎞ
    int m_nSpecialPaint;
    UnicodeString m_SpecialCaption;
};
//---------------------------------------------------------------------------
class TSettingFile
{
public:
    //�����A�j��
    TSettingFile();
    virtual ~TSettingFile();
public:
    //�ǂݏ���
	void WriteToIni(TIniFile *Ini, UnicodeString Section);
	void ReadFromIni(TIniFile *Ini, UnicodeString Section);
public:
    //�ݒ�

    bool m_bCheckNew;

    UnicodeString m_RecentFiles[10];
    UnicodeString m_RecentFolders[10];

    int m_nUndoTimes;

    UnicodeString m_WebSearch;//�f�t�H���g��WebSearch��Key

    //�v���O�C��
    UnicodeString m_PlugIn;
    HINSTANCE m_hPlugIn;//�v���O�C���n���h���iLoadLibrary�j
    void __stdcall (*fepLoadPlugIn)(void);
    void __stdcall (*fepTimeOut)(IDocCtrl *Doc);
    bool __stdcall (*fepOperation)(IDocCtrl *Doc, wchar_t *operation);
    void __stdcall (*fepUnloadPlugIn)(void);
};
//---------------------------------------------------------------------------
class TSetting2Function
{
public:
    //�����A�j��
    TSetting2Function();
    virtual ~TSetting2Function();
public:
    //�ǂݏ���
    void WriteToIni(TIniFile *Ini);
	void ReadFromIni(TIniFile *Ini);
public:
    //�ݒ�

    TStringList *m_WebSearch;//�l�b�g�����ݒ�
};
//---------------------------------------------------------------------------
extern TSettingView SettingView;
extern TSettingFile SettingFile;
extern TSetting2Function Setting2Function;

class TMLText
{
public:
	TMLText();
public:
	UnicodeString Cards;//�J�[�h
	UnicodeString Links;//�����N
	UnicodeString Labels;//���x��
	UnicodeString Line;//�s
	UnicodeString Len;//����
	UnicodeString Size;//�T�C�Y
	UnicodeString Title;//���O
	UnicodeString Created;//�쐬����
	UnicodeString Edited;//�ҏW����
	UnicodeString Viewed;//�{������
	UnicodeString Hits;//��

	UnicodeString EditCardTitle;
    UnicodeString EditCardText;
	UnicodeString EditLinkTitle;
	UnicodeString EditDrawing;

    UnicodeString ChangeSize;//�T�C�Y�̕ύX
    UnicodeString FixCardPosition;//�J�[�h�ʒu�̌Œ�
    UnicodeString SetAsRootCard;//���[�g�J�[�h�w��

    UnicodeString EditLink;//�����N�̕ҏW

    UnicodeString AddLabel;//���x���̒ǉ�
    UnicodeString DeleteLabel;//���x���̍폜
	UnicodeString ChangeLabel;//���x���̕ύX
	UnicodeString LabelName;
	UnicodeString SelectLinkDestination;
	UnicodeString SelectLabelOfDestination;
	UnicodeString SelectLabel;

	UnicodeString Color;

	UnicodeString ArrangeCards;

	UnicodeString ProcessingTextAnalysis;

	UnicodeString Enable;//�L��
	UnicodeString Fold;//�܏�
	UnicodeString Show;//�\��
	UnicodeString Hide;//�B��
	UnicodeString NoAssign;//���w��
	UnicodeString NoLabel;//���x���Ȃ�
	UnicodeString SelectTargetCard;//�����N��J�[�h�̑I��
	UnicodeString Old;//�Â���
	UnicodeString New;//�V������
	UnicodeString Near;//�߂���
	UnicodeString Older;//���Â�
	UnicodeString Newer;//���V����
	UnicodeString SaveCheck;//�f�[�^�͕ύX����Ă��܂��B�ۑ����܂����H
	UnicodeString FailedToSave;
	UnicodeString NotFound;
	UnicodeString AANotEnoughMemory;
};

extern TMLText MLText;

UnicodeString LanguageFileName();
//---------------------------------------------------------------------------
#endif
