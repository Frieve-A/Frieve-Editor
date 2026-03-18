// ---------------------------------------------------------------------------

#ifndef textprocH
#define textprocH
#include <vcl.h>
#define MINFLISTBUFSIZE 256

// ---------------------------------------------------------------------------
typedef struct IDValue_ {
  int ID;
  int Value;
} IDValue;

// ---------------------------------------------------------------------------
class TDMatrix {
public:
  int m_nDim;
  int *m_IDs;
  TList *m_IDValue;
  float *m_Data;

  TDMatrix(int num);
  virtual ~TDMatrix();
  int &ID(int idx);
  float &S(int source, int target);

  void FreeIDValue();
  void Finalize();         // Build sorted ID-to-Index mapping
  int SearchIndex(int ID); // Search Index by ID
private:
  TDMatrix(const TDMatrix &source);
  TDMatrix &operator=(const TDMatrix &source);
};

// ---------------------------------------------------------------------------
typedef int __fastcall (*TFListSortCompare)(void *Item1, void *Item2);

// ---------------------------------------------------------------------------
class TFList {
private:
  // Pointer list
  void **m_Data;

  // Count
  int m_nCount;

  // Buffer size (power of 2)
  int m_nSize;

public:
  // Constructor
  TFList();

  // Destructor
  virtual __fastcall ~TFList();

private:
  // Double buffer size
  void Twice();

  // Halve buffer size
  void Half();

  // Quick sort
  void QuickSort(TFListSortCompare CompFunc, int nLeft, int nRight);

public:
  // Add
  inline void *Add(void *p);

  // Change
  void *Set(int nIndex, void *p);

  // Insert
  void *Insert(int nIndex, void *p);

  // Get count
  inline int FCount();
  __property int Count = {read = FCount};

  // Delete
  void Delete(int nindex);

  // Access
  inline void *FItems(int nindex);
  inline void FWrite(int nindex, void *P);
  __property void *Items[int nindex] = {read = FItems, write = FWrite};

  // Clear all
  void Clear();

  // Sort
  void Sort(TFListSortCompare CompFunc);

  // Shuffle
  void Shuffle();
};
// ---------------------------------------------------------------------------
#define TFListforTP TList

class TWideStringList : public TFListforTP {
public:
  // Destructor
  virtual _fastcall ~TWideStringList();
  // Add
  void Add(WideString S);
  inline void AddA(WideString &S);
  inline void AddP(WideString *S);

  // Delete
  void Delete(int nindex);
  inline void SetNULL(int nindex);

  // Clear
  void _fastcall Clear();

  // Access
  WideString &Strings(int nindex);
  inline WideString *StringsP(int nindex);

  // Ascending sort
  void Sort();
};

// ---------------------------------------------------------------------------
typedef struct WSandValue_ {
  WideString *WS;
  int Value;
  bool Enabled;

  int From; // Index of shorter n-gram that maps to this; -1 if none
  int To;   // Index of longer n-gram that maps to this

  int SN; // Serial number
} WSandValue;

class TWSandValueList : public TFListforTP {
public:
  // Destructor
  virtual _fastcall ~TWSandValueList();
  // Add
  void Add(WideString S);
  inline void AddA(WideString &S);
  inline void AddP(WideString *S);

  // Delete
  void Delete(int nindex);

  // Clear
  void _fastcall Clear();

  // Access
  WideString &Strings(int nindex);
  int &Values(int nindex);
  bool &Enabled(int nindex);
  int &From(int nindex);
  int &To(int nindex);
  int &SN(int nindex);

  // Ascending sort
  void Sort();
  void Sort2();     // Sort by 2nd character onward
  void SortValue(); // Sort by value

  int Search(int from, int to, WideString &Key);
};

// ---------------------------------------------------------------------------
class TTextDecomposer {
public:
  int m_nMaxCombi;          // N
  int m_nMaxSN;             // Max serial number (0 to m_nMaxSN-1)
  TWSandValueList **m_Gram; // 1~N-gram storage

  // maxcombi=N-gram
  // maxcount=Max occurrence count; if exceeded, combination is disabled
  // pgmin, pgmax=Progress range; pgpos=current progress
  TTextDecomposer(TWideStringList *SL, int maxcombi, int maxcount, float pgmin,
                  float pgmax, float &pgpos, bool &terminated,
                  int option = 0x0);
  virtual ~TTextDecomposer();
  // WideString *GetStr(int list, int index);
private:
  TTextDecomposer(const TTextDecomposer &source);
  TTextDecomposer &operator=(const TTextDecomposer &source);
};

// ---------------------------------------------------------------------------
typedef struct SMatrixItem_ {
  int Col, Row;
  float Value;
  bool Flag;
} SMatrixItem;

// ---------------------------------------------------------------------------
class TSMatrix {
public:
  TFListforTP *m_Items; // Items are SMatrixItem
  TFListforTP *m_Rows, *m_Cols;
  // Rows/Cols are TList of Item pointers
  float *RowNorm, *ColNorm; // Row/column norm

  TSMatrix();
  virtual ~TSMatrix();

  inline SMatrixItem &Item(int idx) {
    return *(SMatrixItem *)m_Items->Items[idx];
  }
  void Add(int col, int row, float value);
  void Finalize(bool bAdd, float *pgpos = NULL, bool *terminated = NULL,
                float pgstep = 0.0f); // Sort m_Rows, m_Cols; remove duplicates

  inline TFListforTP *ColList(int col) {
    return (TFListforTP *)m_Cols->Items[col];
  }

  inline TFListforTP *RowList(int row) {
    return (TFListforTP *)m_Rows->Items[row];
  }

  inline SMatrixItem &ColItem(int col, int idx) {
    return *(SMatrixItem *)((TFListforTP *)m_Cols->Items[col])->Items[idx];
  }

  inline SMatrixItem &RowItem(int row, int idx) {
    return *(SMatrixItem *)((TFListforTP *)m_Rows->Items[row])->Items[idx];
  }

  void DeleteSameCol(bool bAdd); // Remove duplicate Cols (Col Index unchanged)

  void PrepareCosCol();             // Precompute column norm
  void PrepareCosRow();             // Precompute row norm
  float CosCol(int col1, int col2); // Cosine similarity of 2 columns
  float CosRow(int row1, int row2); // Cosine similarity of 2 rows

  void MergeCol(int block, float threshold, int target);

  void Dummy(); // Prevent inline expansion
private:
  TSMatrix(const TSMatrix &source);
  TSMatrix &operator=(const TSMatrix &source);
};
// ---------------------------------------------------------------------------
/*
 class TTextToFeature
 {
 public:
 TTextToFeature(TWideStringList *SL, int maxcombi, int maxcount);
 virtual ~TTextToFeature();
 };
 */
String NormalizeLineBreaks(const String input,
                           const String &lineBreak = L"\r\n");
// ---------------------------------------------------------------------------
#endif
