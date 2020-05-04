//---------------------------------------------------------------------------

#ifndef textprocH
#define textprocH
#include <vcl.h>
#define MINFLISTBUFSIZE 256
//---------------------------------------------------------------------------
typedef struct IDValue_{
    int ID;
    int Value;
} IDValue;
//---------------------------------------------------------------------------
class TDMatrix
{
public:
    int m_nDim;
    int *m_IDs;
    TList *m_IDValue;
    float *m_Data;
public:
    TDMatrix(int num);
    virtual ~TDMatrix();
public:
	int &ID(int idx);
	float &S(int source, int target);

    void FreeIDValue();
    void Finalize();//�\�[�g���ꂽID��Index�̃f�[�^�����
    int SearchIndex(int ID);//ID��Index������
};
//---------------------------------------------------------------------------
typedef int __fastcall (*TFListSortCompare)(void * Item1, void * Item2);
//---------------------------------------------------------------------------
class TFList
{
private:
    //�|�C���^���X�g
    void **m_Data;

    //�|�C���^�̐�
    int m_nCount;

    //�|�C���^���X�g�̃T�C�Y�i���P�ʁj
    int m_nSize;
public:
    //�R���X�g���N�^
    TFList();

    //�f�X�g���N�^
    virtual __fastcall ~TFList();
private:
    //�|�C���^���X�g��{�ɂ���
    void Twice();

    //�|�C���^���X�g�𔼕��ɂ���
    void Half();

    //�N�C�b�N�\�[�g
    void QuickSort(TFListSortCompare CompFunc, int nLeft, int nRight);
public:
    //�ǉ�
    inline void *Add(void *p);

    //�ύX
    void *Set(int nIndex, void *p);

    //�}��
    void *Insert(int nIndex, void *p);

    //���𓾂�
    inline int FCount();
    __property int Count = {read = FCount}; 

    //�폜
    void Delete(int nindex);

    //�Q��
    inline void *FItems(int nindex);
    inline void FWrite(int nindex, void *P);
    __property void *Items[int nindex] = {read = FItems, write = FWrite};

    //�S�폜
    void Clear();

    //�\�[�g
    void Sort(TFListSortCompare CompFunc);

    //�V���b�t��
    void Shuffle();
};
//---------------------------------------------------------------------------
#define TFListforTP TList
class TWideStringList : public TFListforTP
{
public:
    //�����A�j��
	virtual _fastcall ~TWideStringList();
public:
    //�ǉ�
    void Add(WideString S);
    inline void AddA(WideString &S);
    inline void AddP(WideString *S);

    //�폜
    void Delete(int nindex);
    inline void SetNULL(int nindex);

    //�N���A
    void _fastcall Clear();

    //�Q��
    WideString &Strings(int nindex);
	inline WideString *StringsP(int nindex);

    //�����\�[�g
    void Sort();
};
//---------------------------------------------------------------------------
typedef struct WSandValue_{
    WideString *WS;
    int Value;
    bool Enabled;

    int From;//�Ή������ʒP��̊J�nIndex�B�Ή�������̂������ꍇ��-1
    int To;//�Ή������ʒP��̏I��Index

    int SN;//�V���A���i���o�[
} WSandValue;

class TWSandValueList : public TFListforTP
{
public:
    //�����A�j��
    virtual _fastcall ~TWSandValueList();
public:
    //�ǉ�
    void Add(WideString S);
    inline void AddA(WideString &S);
    inline void AddP(WideString *S);

    //�폜
    void Delete(int nindex);

    //�N���A
    void _fastcall Clear();

    //�Q��
	WideString &Strings(int nindex);
	int &Values(int nindex);
	bool &Enabled(int nindex);
	int &From(int nindex);
	int &To(int nindex);
	int &SN(int nindex);

    //�����\�[�g
    void Sort();
    void Sort2();//2�����ڈȍ~�Ń\�[�g
    void SortValue();//�l�Ń\�[�g

    int Search(int from, int to, WideString &Key);
};
//---------------------------------------------------------------------------
class TTextDecomposer
{
public:
    int m_nMaxCombi;//N
    int m_nMaxSN;//�ő�V���A���ԍ�(0�`m_nMaxSN-1�j�̃V���A�������Ă���
    TWSandValueList **m_Gram;//1~N�O�����̒P�ꃊ�X�g�Əo����
public:
	//maxcombi=N-gram
	//maxcount=�ő�p�x�Bmaxcount��艽�x���o�Ă���P��͎g��Ȃ��i��Ǔ_�ȂǈӖ��̂Ȃ������̉\�������邽�߁j
	//pgmin�Apgmax=�v���O���X�o�[�̍ő�A�ŏ��l�B�\�����Ȃ��Ȃ痼���[���Bpgpos�Ɍ��݂̐i���ʒu������
	TTextDecomposer(TWideStringList *SL, int maxcombi, int maxcount, float pgmin, float pgmax, float &pgpos, bool &terminated, int option = 0x0);
    virtual ~TTextDecomposer();
public:
    //WideString *GetStr(int list, int index);
};
//---------------------------------------------------------------------------
typedef struct SMatrixItem_{
    int Col, Row;
    float Value;
    bool Flag;
} SMatrixItem;
//---------------------------------------------------------------------------
class TSMatrix
{
public:
    TFListforTP *m_Items;//�e�v�f�iItems��SMatrixItem�j
    TFListforTP *m_Rows, *m_Cols;//�s�A��ŃA�N�Z�X�p�i���ꂼ��Items��TList�j�B���̃��X�g��Item��SMatrixItem
    float *RowNorm, *ColNorm;//�s���A�񖈂̃m����
public:
    TSMatrix();
    virtual ~TSMatrix();
public:
    inline SMatrixItem &Item(int idx){
        return *(SMatrixItem*)m_Items->Items[idx];
    }
    void Add(int col, int row, float value);
    void Finalize(bool bAdd, float *pgpos = NULL, bool *terminated = NULL, float pgstep = 0.0f);//�d�����ڍ폜�Am_Rows, m_Cols�\�[�g
    inline TFListforTP *ColList(int col){
        return (TFListforTP*)m_Cols->Items[col];
    }
    inline TFListforTP *RowList(int row){
        return (TFListforTP*)m_Rows->Items[row];
    }
    inline SMatrixItem &ColItem(int col, int idx){
        return *(SMatrixItem*)((TFListforTP*)m_Cols->Items[col])->Items[idx];
    }
    inline SMatrixItem &RowItem(int row, int idx){
        return *(SMatrixItem*)((TFListforTP*)m_Rows->Items[row])->Items[idx];
    }

    void DeleteSameCol(bool bAdd);//�����Ƃ���ɒl�������Ă���Col���폜�i������Col��Index�͕ς��Ȃ��j

    void PrepareCosCol();//�m���������O�v�Z
    void PrepareCosRow();//�m���������O�v�Z
    float CosCol(int col1, int col2);//2�̗��Cos����
    float CosRow(int row1, int row2);//2�̍s��Cos����

    void MergeCol(int block, float threshold, int target);

    void Dummy();//inline���폜������蓦��
};
//---------------------------------------------------------------------------
/*
class TTextToFeature
{
public:
    TTextToFeature(TWideStringList *SL, int maxcombi, int maxcount);
    virtual ~TTextToFeature();
};
*/
//---------------------------------------------------------------------------
#endif
