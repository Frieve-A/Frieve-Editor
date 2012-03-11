//---------------------------------------------------------------------------

#ifndef cardH
#define cardH
//---------------------------------------------------------------------------
#include <vcl.h>
//---------------------------------------------------------------------------
#include "link.h"
#include "label.h"
#include "drawing.h"
//---------------------------------------------------------------------------
WideString EncodeES(WideString S);//�G�X�P�[�v�V�[�P���X�̃G���R�[�h�i13��\\n�A\��\\�j
WideString DecodeES(WideString S, WideString Ret = "\r\n");//�G�X�P�[�v�V�[�P���X�̃f�R�[�h�i13��\\n�A\��\\�j

class TCard
{
public:
	//���f�[�^��ǉ�������A�������ƃR�s�[�R���X�g���N�^���ҏW

	//�f�[�^
	int m_nID;
	TLabelList *m_Labels;//���x��
	AnsiString m_Title;//�^�C�g��
	TStringList *m_Lines;//�{��
	TDrawing *m_Drawing;//�G
	bool m_bVisible;//�\���A��\��
	int m_nShape;//�`
	int m_nSize;//�T�C�Y�i100�����j
	bool m_bTop;//�K�w�̃g�b�v�B�f�t�H���g��false
	bool m_bFixed;//�ʒu�Œ�B
	bool m_bFold;//�܂肽���݁B�f�t�H���g��false

	//�l�b�g���L����Ȃ��f�[�^�i�ǉ��̍ۂ́ADocument->SoftLoad���C������K�v����j
	double m_fX, m_fY;//���W
	double m_fCreated, m_fUpdated, m_fViewed;//���t
public:
	//�ۑ�����Ȃ��f�[�^
	bool m_bGetFocus;//�J�[�\���ړ��Ńt�H�[�J�X�𓾂邩�ǂ���

	int m_nLevel;//�K�w

	int m_nCreatedOrder, m_nUpdatedOrder, m_nViewedOrder;//���t���\�[�g����0.0�`100.0�ɐ��K����������
	double m_fScore;//�X�R�A
	AnsiString m_ImageFN;//�摜�t�@�C�����i�{���ōŏ��ɏo�Ă����摜�t�@�C���j
    AnsiString m_VideoFN;//�r�f�I�t�@�C�����i�{���ōŏ��ɏo�Ă�������t�@�C���j

    int m_nMatrixX, m_nMatrixY;//�i�q��ɔz�u�����ۂ̍��W
    float m_fMatrixSpeed;//�i�q�Ԃ��ړ����鑬�x�i�`1.0�j

    bool m_bLabelIsFold;//���̃J�[�h�̑S�Ẵ��x�����܂肽���܂�Ă��邩�ǂ���
    TColor m_Color;//�`�掞�̐F

    float m_fTickerPos;//Ticker�B�E�[���牽�h�b�g���ɃX�N���[�����Ă��邩

    bool m_bVisibleBak;//Visible���̃o�b�N�A�b�v�iVisible�̕ω��𑨂���j

    bool m_bSelected;
    int m_nSelected;//�I���A�j���[�V�����p�i0=��I���A100=�I���j

    int m_nParentID;//�K�w�\�����̐e�J�[�hID
    bool m_bHasChild;//�K�w�\�����̎q�J�[�h�����邩�ǂ���

    int m_nTmp;//�e���|�����B�d���g�p�ɒ���
public:
    //�쐬�A�j��
    TCard();
    TCard(TCard &Card);
    virtual ~TCard();
public:
    //Util
    void CheckImageFN();//�摜�A�r�f�I�Ȃǂ悭�Q�Ƃ����O�������N�̃t�@�C�������`�F�b�N����
public:
    //�ǂݏ���
    void LoadFromString(TStringList *SL, int &line, int version);
    void SaveToString(TStringList *SL);

    //�݊��p
    void LoadFromFile(AnsiString FN);
    void SaveToFile(AnsiString FN);
};
//---------------------------------------------------------------------------
#endif
