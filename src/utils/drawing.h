//---------------------------------------------------------------------------

#ifndef drawingH
#define drawingH
//---------------------------------------------------------------------------
#include <vcl.h>
//---------------------------------------------------------------------------
class TDrawingItem : public TList
{
public:
	TDrawingItem();
	TDrawingItem(TDrawingItem &Source);
	TDrawingItem(UnicodeString S);//�����񂩂�f�R�[�h
	__fastcall virtual ~TDrawingItem();

	UnicodeString Encode();//������ŋL�q
private:
	void UpdateFreeHandRect(float x, float y);
	void FinishFreeHandRect();

	float m_fDragStartX, m_fDragStartY;//�h���b�O�J�n���W
	float m_DragRect[4];//�`����W�B�h���b�O���ꎞ�I�Ɏg��

public:
	int m_nType;//�v�f�̃^�C�v�B1=�t���[�n���h�A2=�����A3=�����`�A4=�~�A5=�e�L�X�g
	TColor m_nPenColor;//���̐F�B0x7fffffff��FGColor�A-1�ŐF�Ȃ�
	TColor m_nBrushColor;//�u���V�̐F
	float m_Rect[4];//�`����W�B�t���[�n���h�̏ꍇ�͗v�f�̊܂܂���`�B�����͎n�_�ƏI�_�B�e�L�X�g�͍�����W

	bool m_bSelected;//�I������Ă��邩
	bool m_bExist;//�\���ł���󋵂ɂ��邩

	void SetType(int nType);
	bool MouseDown(float x, float y);//�A��l�͕ҏW���ꂽ���ǂ���
	bool MouseMove(float x, float y);
	bool MouseUp(float x, float y);

	int CheckDrag(float x, float y, int size);//�h���b�O�J�n�\���𒲂ׂ�
	bool Drag(float x, float y, int dragmode);

	void Draw(TCanvas *C, TRect &R);

	int SetPenColor(TColor C);//�F��ύX�B�ύX�ł�����true��Ԃ�
	int SetBrushColor(TColor C);
};
//---------------------------------------------------------------------------
class TDrawing : public TList
{
public:
	TDrawing();
	TDrawing(TDrawing &Source);//�R�s�[�R���X�g���N�^
	TDrawing(UnicodeString S);//�����񂩂�f�R�[�h
	__fastcall virtual ~TDrawing();

	UnicodeString Encode(bool bAll = false);//������ŋL�q�BbAll=false�őI������Ă���A�C�e���̂݁BbAll=true�ł��ׂ�
private:
	TDrawingItem *m_TmpItem;
	void FreeTmpItem();
	TDrawingItem *DItem(int index);
public:
	int m_nTool;//���݂̃c�[���B0=�I���A1=�t���[�n���h�A2=�����A3=�����`�A4=�~�A5=�e�L�X�g
	bool m_bDrawRequest;
	int m_nDragMode;//0x10�`0x13=�[�����̃h���b�O�A0x20=�ړ�
	int m_nDragIndex;//�[�������h���b�O����Item��Index
	float m_fDragX, m_fDragY;//�h���b�O�J�n���W
	TColor m_nPenColor;//���̐F�B0x7fffffff��FGColor�A-1�ŐF�Ȃ�
	TColor m_nBrushColor;//�u���V�̐F

	bool m_bModified;//�ҏW���ꂽ���ǂ���

	void SetTool(int nToolID);
	void MouseDown(float x, float y, int size, TShiftState Shift);
	void MouseMove(float x, float y);
	void MouseUp(float x, float y);

	void Draw(TCanvas *C, TRect &R);

	void SelectRect(float x1, float y1, float x2, float y2);
	void ClearSelection();
	void SelectAll();
	int Selected();//�I�𒆂̗v�f����Ԃ�

	//�ҏW����
	void Cut();
	void Copy();
	void Paste();
	void DDelete();

	void ChangePenColor(TColor C);
	void ChangeBrushColor(TColor C);
};
//---------------------------------------------------------------------------
#endif
