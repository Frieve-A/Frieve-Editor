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
    void Finalize();//ソートされたIDとIndexのデータを作る
    int SearchIndex(int ID);//IDのIndexを検索
};
//---------------------------------------------------------------------------
typedef int __fastcall (*TFListSortCompare)(void * Item1, void * Item2);
//---------------------------------------------------------------------------
class TFList
{
private:
    //ポインタリスト
    void **m_Data;

    //ポインタの数
    int m_nCount;

    //ポインタリストのサイズ（数単位）
    int m_nSize;
public:
    //コンストラクタ
    TFList();

    //デストラクタ
    virtual __fastcall ~TFList();
private:
    //ポインタリストを倍にする
    void Twice();

    //ポインタリストを半分にする
    void Half();

    //クイックソート
    void QuickSort(TFListSortCompare CompFunc, int nLeft, int nRight);
public:
    //追加
    inline void *Add(void *p);

    //変更
    void *Set(int nIndex, void *p);

    //挿入
    void *Insert(int nIndex, void *p);

    //数を得る
    inline int FCount();
    __property int Count = {read = FCount}; 

    //削除
    void Delete(int nindex);

    //参照
    inline void *FItems(int nindex);
    inline void FWrite(int nindex, void *P);
    __property void *Items[int nindex] = {read = FItems, write = FWrite};

    //全削除
    void Clear();

    //ソート
    void Sort(TFListSortCompare CompFunc);

    //シャッフル
    void Shuffle();
};
//---------------------------------------------------------------------------
#define TFListforTP TList
class TWideStringList : public TFListforTP
{
public:
    //生成、破棄
	virtual _fastcall ~TWideStringList();
public:
    //追加
    void Add(WideString S);
    inline void AddA(WideString &S);
    inline void AddP(WideString *S);

    //削除
    void Delete(int nindex);
    inline void SetNULL(int nindex);

    //クリア
    void _fastcall Clear();

    //参照
    WideString &Strings(int nindex);
	inline WideString *StringsP(int nindex);

    //昇順ソート
    void Sort();
};
//---------------------------------------------------------------------------
typedef struct WSandValue_{
    WideString *WS;
    int Value;
    bool Enabled;

    int From;//対応する上位単語の開始Index。対応するものが無い場合は-1
    int To;//対応する上位単語の終了Index

    int SN;//シリアルナンバー
} WSandValue;

class TWSandValueList : public TFListforTP
{
public:
    //生成、破棄
    virtual _fastcall ~TWSandValueList();
public:
    //追加
    void Add(WideString S);
    inline void AddA(WideString &S);
    inline void AddP(WideString *S);

    //削除
    void Delete(int nindex);

    //クリア
    void _fastcall Clear();

    //参照
	WideString &Strings(int nindex);
	int &Values(int nindex);
	bool &Enabled(int nindex);
	int &From(int nindex);
	int &To(int nindex);
	int &SN(int nindex);

    //昇順ソート
    void Sort();
    void Sort2();//2文字目以降でソート
    void SortValue();//値でソート

    int Search(int from, int to, WideString &Key);
};
//---------------------------------------------------------------------------
class TTextDecomposer
{
public:
    int m_nMaxCombi;//N
    int m_nMaxSN;//最大シリアル番号(0～m_nMaxSN-1）のシリアルがついている
    TWSandValueList **m_Gram;//1~Nグラムの単語リストと出現回数
public:
	//maxcombi=N-gram
	//maxcount=最大頻度。maxcountより何度も出てくる単語は使わない（句読点など意味のない文字の可能性があるため）
	//pgmin、pgmax=プログレスバーの最大、最小値。表示しないなら両方ゼロ。pgposに現在の進捗位置が入る
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
    TFListforTP *m_Items;//各要素（ItemsはSMatrixItem）
    TFListforTP *m_Rows, *m_Cols;//行、列でアクセス用（それぞれItemsはTList）。そのリストのItemがSMatrixItem
    float *RowNorm, *ColNorm;//行毎、列毎のノルム
public:
    TSMatrix();
    virtual ~TSMatrix();
public:
    inline SMatrixItem &Item(int idx){
        return *(SMatrixItem*)m_Items->Items[idx];
    }
    void Add(int col, int row, float value);
    void Finalize(bool bAdd, float *pgpos = NULL, bool *terminated = NULL, float pgstep = 0.0f);//重複項目削除、m_Rows, m_Colsソート
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

    void DeleteSameCol(bool bAdd);//同じところに値が入っているColを削除（ただしColのIndexは変わらない）

    void PrepareCosCol();//ノルムを事前計算
    void PrepareCosRow();//ノルムを事前計算
    float CosCol(int col1, int col2);//2つの列のCos距離
    float CosRow(int row1, int row2);//2つの行のCos距離

    void MergeCol(int block, float threshold, int target);

    void Dummy();//inlineが削除される問題逃れ
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
