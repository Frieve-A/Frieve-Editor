// ---------------------------------------------------------------------------

#include <math.h>

#include "textproc.h"

// ---------------------------------------------------------------------------
TDMatrix::TDMatrix(int num) : m_nDim(num) {
	m_IDs = new int[m_nDim];
	m_Data = new float[m_nDim * m_nDim];
	m_IDValue = new TList();

	// Inline workaround
	ID(0);
	S(0, 0);
}

// ---------------------------------------------------------------------------
TDMatrix::~TDMatrix() {
	FreeIDValue();
	delete m_IDValue;
	delete[]m_Data;
	delete[]m_IDs;
}

// ---------------------------------------------------------------------------
int &TDMatrix::ID(int idx) {
	return m_IDs[idx];
}

// ---------------------------------------------------------------------------
float &TDMatrix::S(int source, int target) {
	return m_Data[source * m_nDim + target];
}

// ---------------------------------------------------------------------------
void TDMatrix::FreeIDValue() {
	for (int i = 0; i < m_IDValue->Count; i++) {
		delete(IDValue*)m_IDValue->Items[i];
	}
	m_IDValue->Clear();
}

// ---------------------------------------------------------------------------
int __fastcall func_IDCmp(void * Item1, void * Item2) {
	return ((IDValue *)Item1)->ID - ((IDValue *)Item2)->ID;
}

// ---------------------------------------------------------------------------
void TDMatrix::Finalize() {
	FreeIDValue();

	// Build sorted ID-to-Index mapping
	for (int i = 0; i < m_nDim; i++) {
		IDValue *IDV = new IDValue;
		IDV->ID = ID(i);
		IDV->Value = i;
		m_IDValue->Add(IDV);
	}
	m_IDValue->Sort(func_IDCmp);
}

// ---------------------------------------------------------------------------
int TDMatrix::SearchIndex(int ID) {
	// Search Index by ID
	int step = m_IDValue->Count >> 1;
	int pos = step;
	int count = 0; // Times not found when shifting by 1
	while (true) {
		int Cmp = ID - ((IDValue*)m_IDValue->Items[pos])->ID;
		if (Cmp == 0) {
			return ((IDValue*)m_IDValue->Items[pos])->Value;
		}
		else {
			int laststep = step;
			step >>= 1;
			if (step < 1) {
				step = 1;
				count++;
				if (count >= 2) {
					return -1;
				}
			}
			else if (step << 1 != laststep) {
				count--;
			}
			if (Cmp < 0) {
				pos -= step;
				if (pos < 0) {
					return -1;
				}
			}
			else {
				pos += step;
				if (pos >= m_IDValue->Count) {
					return -1;
				}
			}
		}
	}
	/*
	 // Search Index by ID
	 int step = m_IDValue->Count >> 1;
	 int pos = step;
	 int count = 0; // Times not found when shifting by 1
	 while (count < 2 && pos >= 0 && pos < m_IDValue->Count){
	 int Cmp = ID - ((IDValue*)m_IDValue->Items[pos])->ID;
	 if (Cmp == 0){
	 return ((IDValue*)m_IDValue->Items[pos])->Value;
	 }else{
	 int laststep = step;
	 step >>= 1;
	 if (step < 1){
	 step = 1;
	 count++;
	 }else if (step << 1 != laststep){
	 count--;
	 }
	 if (Cmp < 0){
	 pos -= step;
	 }else{
	 pos += step;
	 }
	 }
	 }
	 return -1;
	 */
}

// ---------------------------------------------------------------------------
// Constructor
TFList::TFList() : m_nSize(0), m_nCount(0), m_Data(NULL) {
}

// ---------------------------------------------------------------------------
// Destructor
__fastcall TFList::~TFList() {
	Clear();
}

// ---------------------------------------------------------------------------
// Double pointer list buffer
void TFList::Twice() {
	// Determine next size
	void **newdata;
	int lastsize = m_nSize;
	if (m_nSize) {
		m_nSize *= 2;
	}
	else {
		m_nSize = MINFLISTBUFSIZE;
	}

	// Allocate new storage
	newdata = new void*[m_nSize];
	if (lastsize) {
		// Copy existing data
		memcpy(newdata, m_Data, sizeof(void*) * m_nCount);
		delete[]m_Data;
	}

	// Assign new storage
	m_Data = newdata;
}

// ---------------------------------------------------------------------------
// Halve pointer list buffer
void TFList::Half() {
	// Determine next size
	void **newdata;
	m_nSize /= 2;

	// Allocate new storage
	if (m_nSize) {
		newdata = new void*[m_nSize];
		// Copy existing data
		memcpy(newdata, m_Data, sizeof(void*) * m_nCount);
		delete[]m_Data;

		// Assign new storage
		m_Data = newdata;
	}
	else {
		delete[]m_Data;
		m_Data = NULL;
	}
}

void __fastcall QuickSort(void **SortList, int L, int R,
	TFListSortCompare SCompare) {
	int I = L;
	while (I < R) {
		I = L;
		int J = R;
		void *P = SortList[(L + R) >> 1];
		while (I <= J) {
			while (SCompare(SortList[I], P) < 0) {
				I++;
			}
			while (SCompare(SortList[J], P) > 0) {
				J--;
			}
			if (I <= J) {
				void *T = SortList[I];
				SortList[I] = SortList[J];
				SortList[J] = T;
				I++;
				J--;
			}
		}
		if (L < J) {
			QuickSort(SortList, L, J, SCompare);
		}
		L = I;
	}
	/*
	 int I = L;
	 while (I < R){
	 I = L;
	 int J = R;
	 void *P = SortList[(L + R) >> 1];
	 while (I <= J){
	 while (SCompare(SortList[I], P) < 0){
	 I++;
	 }
	 while (SCompare(SortList[J], P) > 0){
	 J--;
	 }
	 if (I <= J){
	 void *T = SortList[I];
	 SortList[I] = SortList[J];
	 SortList[J] = T;
	 I++;
	 J--;
	 }
	 }
	 if (L < J){
	 QuickSort(SortList, L, J, SCompare);
	 }
	 L = I;
	 }
	 */
}

// ---------------------------------------------------------------------------
// Quick sort
void TFList::QuickSort(TFListSortCompare CompFunc, int nLeft, int nRight) {
	if (m_nCount <= 1) {
		return;
	}

	// Pivot selection
	void *p = m_Data[(nLeft + nRight + 1) / 2];
	int Left = nLeft;
	int Right = nRight;

	while (true) {
		// Find first >= p from left
		int lefti = Left;
		while (CompFunc(m_Data[lefti], p) > 0) { // Less than p
			lefti++;
		}

		// Find first <= p from right
		int righti = Right;
		while (CompFunc(m_Data[righti], p) < 0) { // Greater than p
			righti--;
		}

		if (lefti < righti) {
			// Swap
			void *tmp = m_Data[righti];
			m_Data[righti] = m_Data[lefti];
			m_Data[lefti] = tmp;
			Left = lefti + 1;
			Right = righti - 1;
		}
		else {
			// Left of pivot <= pivot, right >= pivot

			// Recursively sort left and right
			if (nLeft < lefti - 1) {
				// Sort left
				QuickSort(CompFunc, nLeft, lefti - 1);
			}
			if (lefti < nRight) {
				// Sort right
				QuickSort(CompFunc, lefti, nRight);
			}
			break;
		}
	}
}

// ---------------------------------------------------------------------------
// Get count
int TFList::FCount() {
	return m_nCount;
}

// ---------------------------------------------------------------------------
// Add
void *TFList::Add(void *p) {
	if (m_nCount >= m_nSize) {
		// Expand if storage insufficient
		Twice();
	}
	m_Data[m_nCount++] = p;
	return p;
}

// ---------------------------------------------------------------------------
// Set
void *TFList::Set(int nIndex, void *p) {
	m_Data[nIndex] = p;
	return p;
}

// ---------------------------------------------------------------------------
// Insert
void *TFList::Insert(int nIndex, void *p) {
	if (nIndex < m_nCount) {
		if (m_nCount >= m_nSize) {
			// Expand if storage insufficient
			Twice();
		}

		// Shift elements from insert position backward
		for (int i = m_nCount; i > nIndex; i--) {
			m_Data[i] = m_Data[i - 1];
		}
		m_Data[nIndex] = p;
		m_nCount++;

		return p;
	}
	else {
		// Append (same as Add)
		return Add(p);
	}
}

// ---------------------------------------------------------------------------
// Delete
void TFList::Delete(int nindex) {
	// Delete element at nindex

	// Shift elements after nindex forward
	for (int i = nindex; i < m_nCount - 1; i++) {
		m_Data[i] = m_Data[i + 1];
	}

	// Decrement count
	m_nCount--;

	// Halve storage if too large
	if (m_nCount <= m_nSize / 2 && m_nCount > MINFLISTBUFSIZE) {
		Half();
	}
}

// ---------------------------------------------------------------------------
// Access
void *TFList::FItems(int nindex) {
	return m_Data[nindex];
}

void TFList::FWrite(int nindex, void *P) {
	m_Data[nindex] = P;
}

// ---------------------------------------------------------------------------
// Clear all
void TFList::Clear() {
	if (m_nSize) {
		delete[]m_Data;
		m_nSize = 0;
		m_nCount = 0;
		m_Data = NULL;
	}
}

// ---------------------------------------------------------------------------
// Sort
void TFList::Sort(TFListSortCompare CompFunc) {
	// QuickSort(CompFunc, 0, Count - 1);
	if (m_nCount <= 1) {
		return;
	}
	::QuickSort(m_Data, 0, Count - 1, CompFunc);
}

// ---------------------------------------------------------------------------
// Shuffle
void TFList::Shuffle() {
	for (int i = 0; i < m_nCount; i++) {
		int idx = rand() % m_nCount;
		void *tmp = m_Data[i];
		m_Data[i] = m_Data[idx];
		m_Data[idx] = tmp;
	}
}
// ---------------------------------------------------------------------------
/*
 int __fastcall func_pwsCmp(void * Item1, void * Item2)
 {
 WideString *WS1 = (WideString *)Item1;
 WideString *WS2 = (WideString *)Item2;
 return WideCompareText(*WS1, *WS2);
 }
 */
// #define DEBUG_SPEEDTUNE
#ifdef DEBUG_SPEEDTUNE
#include <mmsystem.hpp>
#endif

// ---------------------------------------------------------------------------
TTextDecomposer::TTextDecomposer(TWideStringList *SL, int maxcombi,
	int maxcount, float pgmin, float pgmax, float &pgpos, bool &terminated,
	int option) : m_nMaxCombi(maxcombi) {
#ifdef DEBUG_SPEEDTUNE
	// Debug variables
	int debug_orgtotal = 0; // Original combination count

	int debug_fewdeleted = 0; // Deleted: shorter combo appeared <= 1 time
	int debug_dplxdeleted = 0; // Deleted: duplicate combinations
	int debug_shortdeleted = 0; // Deleted: shorter combo same count as longer

	int debug_totalremain = 0; // Extracted combination count

	unsigned int time_bunkai = 0, time_sort = 0, time_rekkyo = 0, t;
#endif
	WideString Ret = "\n";

	m_Gram = new TWSandValueList*[maxcombi];
	// N-gram loop
	for (int i = 0; i < m_nMaxCombi; i++) {
		m_Gram[i] = new TWSandValueList();
	}
	TWSandValueList *lastgram = NULL;
	for (int i = 0; i < m_nMaxCombi && !terminated; i++) {
		TWideStringList *Tmp = new TWideStringList(); // Unique combos per card
		if (i > 0) {
			if (lastgram->Count == 0) {
				// No more combinations
				break;
			}
		}

		TWSandValueList *gram = m_Gram[i];

		// Card loop
		for (int ic = 0; ic < SL->Count; ic++) {
			// Lowercase
			// WideString WS = SL->Strings(ic);
			WideString WS;
			if (option & 0x1) {
				// No lowercase
				WS = SL->Strings(ic);
			}
			else {
				// Usually lowercase for robustness
				WS = WideLowerCase(SL->Strings(ic));
			}

#ifdef DEBUG_SPEEDTUNE
			t = timeGetTime();
#endif
			// Decompose combinations
			int len = i + 1;
			int wslen = WS.Length();
			TWideStringList *Tmp2 = new TWideStringList(); // All combos in card
			int count = 0;
			wchar_t *w1 = &WS[1];
			for (int i2 = 0; i2 < wslen; i2++) {
				if (*w1++ == '\n') {
					count = 0;
				}
				else {
					if (count < len) {
						count++;
					}
					if (count == len) {
						Tmp2->Add(WS.SubString(i2 - i + 1, len));
					}
				}
			}
#ifdef DEBUG_SPEEDTUNE
			time_bunkai += timeGetTime() - t;

			t = timeGetTime();
#endif
			Tmp2->Sort();
#ifdef DEBUG_SPEEDTUNE
			debug_orgtotal += Tmp2->Count;
			time_sort += timeGetTime() - t;
#endif

			// Add to this card's combos while removing duplicates
			// WS = "";
#ifdef DEBUG_SPEEDTUNE
			t = timeGetTime();
#endif
			if (Tmp2->Count > 0) {
				if (i > 0) {
					// Keep only longer combos that contain combos appearing 2+ times
					int lastindex = 0;
					int lastcount = lastgram->Count;

					WideString *TSL = NULL; // Previous string

					int tmp2count = Tmp2->Count;
					for (int i3 = 0; i3 < tmp2count; i3++) {
						WideString *TS1 = Tmp2->StringsP(i3); // Text to add
						bool add = i3 == 0 ? true :
							(WideCompareStr(*TSL, *TS1) != 0);
						// bool add = i3 == 0 ? true : (wcscmp(TSL, TS1) != 0);
						if (add) { // Add Tmp2->Strings(i3)
							WideString TSC = TS1->SubString(1, i); // 1-char shorter target
							// WideString TSC = ((WideString)TS1).SubString(1, i);// 1-char shorter target
							int cmp = 1;
							while (lastindex < lastcount) {
								cmp = WideCompareStr
									(lastgram->Strings(lastindex), TSC);
							if (cmp < 0) {
									lastindex++;
								}
								else {
									break;
								}
							}
							if (cmp == 0) {
								Tmp->AddP(TS1);
								Tmp2->SetNULL(i3);
							}
#ifdef DEBUG_SPEEDTUNE
							else {
								debug_fewdeleted++;
							}
#endif
						}
#ifdef DEBUG_SPEEDTUNE
						else {
							debug_fewdeleted++;
						}
#endif
						TSL = TS1;
					}
				}
				else {

					// Keep all combinations
					WideString *WSL = NULL;

					int tmp2count = Tmp2->Count;
					for (int i3 = 0; i3 < tmp2count; i3++) {
						WideString *TS1 = Tmp2->StringsP(i3);
						bool add = i3 == 0 ? true :
							*TS1 != Ret && WideCompareStr(*WSL, *TS1) != 0;
						if (add) {
							// Tmp->AddA(*TS1);
							Tmp->AddP(TS1);
							Tmp2->SetNULL(i3);
						}
#ifdef DEBUG_SPEEDTUNE
						else {
							debug_fewdeleted++;
						}
#endif
						WSL = TS1;
					}
				}
			}
			delete Tmp2;
#ifdef DEBUG_SPEEDTUNE
			time_rekkyo += timeGetTime() - t;
#endif
			/*
			 }else{
			 WideString WSBak = WS;
			 WS = "";
			 for (int i3 = 0 ; i3 < WSBak.Length() ; i3++){
			 WideString W1 = WSBak.SubString(i3 + 1, 1);
			 if (W1 != Ret){
			 WS += W1;
			 }
			 }
			 debug_orgtotal += WS.Length();
			 }
			 */

			// Tmp += WS;
			/*
			 int len2 = Tmp.Length();
			 if (len2 % (i + 1) != 0){
			 Tmp = "";
			 }
			 // */
		} // Card loop

		// Merge results from all cards
		// Decompose combinations
		/*
		 int len = i + 1;
		 int count = Tmp.Length() / len;
		 TWideStringList *Tmp2 = new TWideStringList();// All combos in card
		 for (int i2 = 0 ; i2 < count ; i2++){
		 Tmp2->Add(Tmp.SubString(i2 * len + 1, len));
		 }
		 Tmp2->Sort();
		 */
		Tmp->Sort();

		// Add only combos that appear 2+ times (after dedup)
		int count = 0;
		int tmpcount = Tmp->Count;
		if (tmpcount > 0) {
			WideString *WSL = Tmp->StringsP(0);
			// WideString *WSL = (WideString*)&Tmp->Strings(0);

			for (int i3 = 1; i3 < tmpcount; i3++) {
				WideString *TS = Tmp->StringsP(i3);
				// WideString *TS = (WideString*)&Tmp->Strings(i3);
				if (WideCompareStr(*WSL, *TS) != 0) {
					// Different
					if (count > 0) {
						count++;
						gram->Values(gram->Count - 1) = count;
						if (maxcount > 0 && count >= maxcount) {
							gram->Enabled(gram->Count - 1) = false;
#ifdef DEBUG_SPEEDTUNE
							debug_shortdeleted++;
#endif
						}
					}
#ifdef DEBUG_SPEEDTUNE
					debug_dplxdeleted++;
#endif
					count = 0;
				}
				else {
					// Same
					count++;
					if (count == 1) {
						// gram->AddA(*TS);
						gram->AddP(TS);
						Tmp->SetNULL(i3);
					}
#ifdef DEBUG_SPEEDTUNE
					else {
						debug_dplxdeleted++;
					}
#endif
				}
				WSL = TS;
			}
			if (count > 0) {
				count++;
				gram->Values(gram->Count - 1) = count;
				if (maxcount > 0 && count >= maxcount) {
					gram->Enabled(gram->Count - 1) = false;
#ifdef DEBUG_SPEEDTUNE
					debug_shortdeleted++;
#endif
				}
			}
#ifdef DEBUG_SPEEDTUNE
			debug_dplxdeleted++;
#endif
		}
		/*
		 // Debug: check m_Gram and m_GramCount match
		 if (gram->Count != m_GramCount[i]->Count){
		 m_GramCount[i]->Add(NULL);
		 }
		 */

		// If (N-1)gram has same count as N-gram, remove (N-1)gram; map indices
		if (i > 0) {
			int lastindex = 0;
			int lastcount = lastgram->Count;
			int count = gram->Count;
			for (int i3 = 0; i3 < count; i3++) {
				WideString TSC = gram->Strings(i3).SubString(1, i); // 1-char shorter text
				int cmp = 1;
				while (lastindex < lastcount) {
					if ((cmp = WideCompareStr(lastgram->Strings(lastindex),
						TSC)) < 0) {
						lastindex++;
					}
					else {
						break;
					}
				}
				if (cmp == 0) {
					if (lastgram->From(lastindex) == -1) {
						lastgram->From(lastindex) = i3;
					}
					if (lastgram->Values(lastindex) == gram->Values(i3)) {
						if (lastgram->Enabled(lastindex)) {
							lastgram->Enabled(lastindex) = false;
#ifdef DEBUG_SPEEDTUNE
							debug_shortdeleted++;
#endif
						}
					}
					lastgram->To(lastindex) = i3;
				}
			}
		}

		// Sort by 2nd character onward
		gram->Sort2();

		// If (N-1)gram has same count when sorted by 2nd char, remove (N-1)gram
		if (i > 0) {
			int lastindex = 0;
			int lastcount = lastgram->Count;
			int count = gram->Count;
			for (int i3 = 0; i3 < count; i3++) {
				WideString TSC = gram->Strings(i3).SubString(2, i); // 1-char shorter text
				int cmp = 1;
				while (lastindex < lastcount) {
					if ((cmp = WideCompareStr(lastgram->Strings(lastindex),
						TSC)) < 0) {
						lastindex++;
					}
					else {
						break;
					}
				}
				if (cmp == 0) {
					if (lastgram->Values(lastindex) == gram->Values(i3)) {
						if (lastgram->Enabled(lastindex)) {
							lastgram->Enabled(lastindex) = false;
#ifdef DEBUG_SPEEDTUNE
							debug_shortdeleted++;
#endif
						}
					}
				}
			}
		}

		// Restore order (sort from 1st character)
		gram->Sort();

		delete Tmp;

		lastgram = gram;

		// Progress update
		if (pgmax > pgmin) {
			pgpos = ((pgmax - pgmin) * (i + 1)) / m_nMaxCombi + pgmin;
			Application->ProcessMessages();
		}
	} // N-gram loop

	// Compute total, assign serial numbers
	m_nMaxSN = 0;
	for (int i = 0; i < m_nMaxCombi; i++) {
		TWSandValueList *gram = m_Gram[i];
		int gramcount = gram->Count;
		for (int i3 = 0; i3 < gramcount; i3++)
			if (gram->Enabled(i3)) {
#ifdef DEBUG_SPEEDTUNE
				debug_totalremain++;
#endif
				gram->SN(i3) = m_nMaxSN++;
			}
	}
}

// ---------------------------------------------------------------------------
TTextDecomposer::~TTextDecomposer() {
	for (int i = 0; i < m_nMaxCombi; i++) {
		delete m_Gram[i];
	}
	delete[]m_Gram;
}

// ---------------------------------------------------------------------------
/*
 WideString *TTextDecomposer::GetStr(int list, int index)
 {
 return (WideString *)m_Gram[list]->Items[index];
 }
 */
// ---------------------------------------------------------------------------
// TWideStringList
// ---------------------------------------------------------------------------
_fastcall TWideStringList::~TWideStringList() {
	Clear();
}

// ---------------------------------------------------------------------------
void _fastcall TWideStringList::Clear() {
	/*
	 for (int i = Count - 1 ; i >= 0 ; i--){
	 delete (WideString*)Items[i];
	 }
	 */
	for (int i = 0; i < Count; i++) {
		if (Items[i]) {
			delete(WideString*)Items[i];
		}
	}
	TFListforTP::Clear();
}

// ---------------------------------------------------------------------------
// Add
void TWideStringList::Add(WideString S) {
	WideString *S1 = new WideString;
	*S1 = S;
	TFListforTP::Add(S1);
}

// ---------------------------------------------------------------------------
// Add
void TWideStringList::AddA(WideString &S) {
	WideString *S1 = new WideString;
	*S1 = S;
	TFListforTP::Add(S1);
}

// ---------------------------------------------------------------------------
void TWideStringList::AddP(WideString *S) {
	TFListforTP::Add(S);
}

// ---------------------------------------------------------------------------
void TWideStringList::SetNULL(int nindex) {
	Items[nindex] = NULL;
}

// ---------------------------------------------------------------------------
// Delete
void TWideStringList::Delete(int nindex) {
	if (Items[nindex]) {
		delete(WideString*)Items[nindex];
	}
	TFListforTP::Delete(nindex);
}

// ---------------------------------------------------------------------------
// Access
WideString &TWideStringList::Strings(int nindex) {
	return *(WideString*)Items[nindex];
}

// ---------------------------------------------------------------------------
// Access
WideString *TWideStringList::StringsP(int nindex) {
	return (WideString*)Items[nindex];
}

// ---------------------------------------------------------------------------
int __fastcall WideStringSortCompare(void * Item1, void * Item2) {
	return WideCompareStr(*(WideString*)Item1, *(WideString*)Item2);
}

// ---------------------------------------------------------------------------
// Ascending sort
void TWideStringList::Sort() {
	TFListforTP::Sort(WideStringSortCompare);
}
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// TWSandValueList
// ---------------------------------------------------------------------------
_fastcall TWSandValueList::~TWSandValueList() {
	Clear();
}

// ---------------------------------------------------------------------------
void _fastcall TWSandValueList::Clear() {
	for (int i = 0; i < Count; i++) {
		delete((WSandValue*)Items[i])->WS;
		delete(WSandValue*)Items[i];
	}
	TFListforTP::Clear();
}

// ---------------------------------------------------------------------------
// Add
void TWSandValueList::Add(WideString S) {
	WSandValue *S1 = new WSandValue;
	(*S1).WS = new WideString;
	*(*S1).WS = S;
	(*S1).Value = 0;
	(*S1).Enabled = true;
	(*S1).From = -1;
	(*S1).To = -1;
	(*S1).SN = -1;
	TFListforTP::Add(S1);
}

// ---------------------------------------------------------------------------
// Add
void TWSandValueList::AddA(WideString &S) {
	WSandValue *S1 = new WSandValue;
	(*S1).WS = new WideString;
	*(*S1).WS = S;
	(*S1).Value = 0;
	(*S1).Enabled = true;
	(*S1).From = -1;
	(*S1).To = -1;
	(*S1).SN = -1;
	TFListforTP::Add(S1);
}

// ---------------------------------------------------------------------------
// Add
void TWSandValueList::AddP(WideString *S) {
	WSandValue *S1 = new WSandValue;
	(*S1).WS = S;
	(*S1).Value = 0;
	(*S1).Enabled = true;
	(*S1).From = -1;
	(*S1).To = -1;
	(*S1).SN = -1;
	TFListforTP::Add(S1);
}

// ---------------------------------------------------------------------------
// Delete
void TWSandValueList::Delete(int nindex) {
	delete((WSandValue*)Items[nindex])->WS;
	delete(WSandValue*)Items[nindex];
	TFListforTP::Delete(nindex);
}

// ---------------------------------------------------------------------------
// Access
WideString &TWSandValueList::Strings(int nindex) {
	return *(*(WSandValue*)Items[nindex]).WS;
}

// ---------------------------------------------------------------------------
// Access
int &TWSandValueList::Values(int nindex) {
	return (*(WSandValue*)Items[nindex]).Value;
}

// ---------------------------------------------------------------------------
// Access
bool &TWSandValueList::Enabled(int nindex) {
	return (*(WSandValue*)Items[nindex]).Enabled;
}

// ---------------------------------------------------------------------------
// Access
int &TWSandValueList::From(int nindex) {
	return (*(WSandValue*)Items[nindex]).From;
}

// ---------------------------------------------------------------------------
// Access
int &TWSandValueList::To(int nindex) {
	return (*(WSandValue*)Items[nindex]).To;
}

// ---------------------------------------------------------------------------
// Access
int &TWSandValueList::SN(int nindex) {
	return (*(WSandValue*)Items[nindex]).SN;
}

// ---------------------------------------------------------------------------
int _fastcall WSandValueSortCompare(void * Item1, void * Item2) {
	return WideCompareStr(*(*(WSandValue*)Item1).WS, *(*(WSandValue*)Item2).WS);
}

// ---------------------------------------------------------------------------
int _fastcall WSandValueSortCompare2(void * Item1, void * Item2) {
	// From 2nd character onward
	return WideCompareStr((*(WSandValue*)Item1).WS->SubString(2, 32767),
		(*(WSandValue*)Item2).WS->SubString(2, 32767));
}

// ---------------------------------------------------------------------------
int _fastcall WSandValueSortCompareValue(void * Item1, void * Item2) {
	// By value
	return (*(WSandValue*)Item1).Value - (*(WSandValue*)Item2).Value;
}

// ---------------------------------------------------------------------------
// Ascending sort
void TWSandValueList::Sort() {
	TFListforTP::Sort(TListSortCompare(WSandValueSortCompare));
}

// ---------------------------------------------------------------------------
// Ascending sort
void TWSandValueList::Sort2() {
	TFListforTP::Sort(TListSortCompare(WSandValueSortCompare2));
}

// ---------------------------------------------------------------------------
// Sort by value
void TWSandValueList::SortValue() {
	TFListforTP::Sort(TListSortCompare(WSandValueSortCompareValue));
}

// ---------------------------------------------------------------------------
int TWSandValueList::Search(int from, int to, WideString &Key) {
	// Binary search for Key (exact match) in from~to; assumes sorted
	int step = (to + 1 - from) >> 1;
	int pos = from + step;
	int count = 0; // Times not found when shifting by 1
	while (true) {
		int CmpText = WideCompareStr(Strings(pos), Key);
		if (CmpText == 0) {
			return pos;
		}
		else {
			int laststep = step;
			step >>= 1;
			if (step < 1) {
				step = 1;
				count++;
				if (count >= 2) {
					return -1;
				}
			}
			else if (step << 1 != laststep) {
				count--;
			}
			if (CmpText > 0) { // Key less than pos
				pos -= step;
				if (pos < 0) {
					return -1;
				}
			}
			else {
				pos += step;
				if (pos > to) {
					return -1;
				}
			}
		}
	}
	/*
	 int step = (to + 1 - from) / 2;
	 int pos = from + step;
	 int count = 0; // Times not found when shifting by 1
	 while (count < 2 && pos >= from && pos <= to){
	 int CmpText = WideCompareStr(Strings(pos), Key);
	 if (CmpText == 0){
	 return pos;
	 }else{
	 int laststep = step;
	 step /= 2;
	 if (step < 1){
	 step = 1;
	 count++;
	 }else if (step * 2 != laststep){
	 count--;
	 }
	 if (CmpText > 0){ // Key less than pos
	 pos -= step;
	 }else{
	 pos += step;
	 }
	 }
	 }
	 return -1;
	 */
}

// ---------------------------------------------------------------------------
TSMatrix::TSMatrix() {
	m_Items = new TFListforTP();
	m_Rows = new TFListforTP();
	m_Cols = new TFListforTP();
	RowNorm = NULL;
	ColNorm = NULL;
}

// ---------------------------------------------------------------------------
void TSMatrix::Dummy() {
	Add(0, 0, 0.0f);
}

// ---------------------------------------------------------------------------
TSMatrix::~TSMatrix() {
	if (RowNorm) {
		delete[]RowNorm;
	}
	if (ColNorm) {
		delete[]ColNorm;
	}
	for (int i = 0; i < m_Cols->Count; i++) {
		delete ColList(i);
	}
	delete m_Cols;

	for (int i = 0; i < m_Rows->Count; i++) {
		delete RowList(i);
	}
	delete m_Rows;

	for (int i = 0; i < m_Items->Count; i++) {
		delete &Item(i);
	}
	delete m_Items;
}

// ---------------------------------------------------------------------------
void TSMatrix::Add(int col, int row, float value) {
	if (m_Cols->Count < col + 1) {
		for (int i = m_Cols->Count; i <= col; i++) {
			m_Cols->Add(new TFListforTP());
		}
	}

	if (m_Rows->Count < row + 1) {
		for (int i = m_Rows->Count; i <= row; i++) {
			m_Rows->Add(new TFListforTP());
		}
	}

	SMatrixItem *SMI = new SMatrixItem;
	SMI->Col = col;
	SMI->Row = row;
	SMI->Value = value;
	SMI->Flag = false;

	m_Items->Add(SMI);
	ColList(col)->Add(SMI);
	RowList(row)->Add(SMI);
}

// ---------------------------------------------------------------------------
void TSMatrix::PrepareCosRow() {
	int rowcount = m_Rows->Count;
	if (RowNorm) {
		delete[]RowNorm;
	}
	RowNorm = new float[rowcount];

	// Row loop
	for (int i = 0; i < rowcount; i++) {
		TFListforTP *list = RowList(i);
		int count1 = list->Count;
		float norm = 0.0f;
		for (int ic = 0; ic < count1; ic++) {
			float f = ((SMatrixItem*)list->Items[ic])->Value;
			// RowItem(i, ic).Value;
			norm += f * f;
		}
		if (norm > 0.0f) {
			RowNorm[i] = 1.0f / sqrt(norm);
		}
		else {
			RowNorm[i] = 0.0f;
		}
	}
}

// ---------------------------------------------------------------------------
float TSMatrix::CosCol(int col1, int col2) {
	if (col1 >= m_Cols->Count || col2 >= m_Cols->Count) {
		return 0.0f;
	}

	// Cosine similarity
	int count1 = ColList(col1)->Count;
	int count2 = ColList(col2)->Count;
	if (count1 > 0 && count2 > 0) {
		float cov = 0.0f;
		float norm1 = 0.0f, norm2 = 0.0f;
		int index = 0;
		for (int i = 0; i < count1; i++) {
			while (index < count2) {
				if (ColItem(col2, index).Row < ColItem(col1, i).Row) {
					norm2 += ColItem(col2, index).Value * ColItem(col2,
					index).Value;
					index++;
				}
				else {
					break;
				}
			}
			if (index < count2) {
				if (ColItem(col2, index).Row == ColItem(col1, i).Row) {
					cov += ColItem(col2, index).Value * ColItem(col1, i).Value;
				}
			}
			norm1 += ColItem(col1, i).Value * ColItem(col1, i).Value;
		}
		for (int i = index; i < count2; i++) {
			norm2 += ColItem(col2, i).Value * ColItem(col2, i).Value;
		}

		if (norm1 > 0.0f && norm2 > 0.0f) {
			float cos = (float)(cov / (sqrt(norm1) * sqrt(norm2)));
			return cos;
		}
		else {
			return 0.0f;
		}
	}
	else {
		return 0.0f;
	}
}

// ---------------------------------------------------------------------------
float TSMatrix::CosRow(int row1, int row2) {
	if (row1 >= m_Rows->Count || row2 >= m_Rows->Count) {
		return 0.0f;
	}

	if (RowNorm) {
		// Norm precomputed
		// Cosine similarity
		if (RowNorm[row1] > 0.0f && RowNorm[row2] > 0.0f) {
			TFListforTP * list1 = RowList(row1);
			TFListforTP * list2 = RowList(row2);
			int count1 = list1->Count;
			int count2 = list2->Count;

			float cov = 0.0f;
			int index = 0;
			for (int i = 0; i < count1; i++) {
				int cmp = 1;
				SMatrixItem *SMI1 = (SMatrixItem*)list1->Items[i];
				// &RowItem(row1, i);
				SMatrixItem *SMI2 = NULL;
				while (index < count2) {
					SMI2 = (SMatrixItem*)list2->Items[index];
					// &RowItem(row2, index);
					cmp = SMI2->Col - SMI1->Col;
					if (cmp < 0) {
						index++;
					}
					else {
						break;
					}
				}
				if (cmp == 0) {
					cov += SMI2->Value * SMI1->Value;
				}
			}

			return cov * RowNorm[row1] * RowNorm[row2];
		}
		else {
			return 0.0f;
		}
	}
	else {
		// Cosine similarity
		int count1 = RowList(row1)->Count;
		int count2 = RowList(row2)->Count;
		if (count1 > 0 && count2 > 0) {
			float cov = 0.0f;
			float norm1 = 0.0f, norm2 = 0.0f;
			int index = 0;
			for (int i = 0; i < count1; i++) {
				int cmp = 1;
				SMatrixItem *SMI1 = &RowItem(row1, i);
				SMatrixItem *SMI2 = NULL;
				while (index < count2) {
					SMI2 = &RowItem(row2, index);
					cmp = SMI2->Col - SMI1->Col;
					if (cmp < 0) {
						norm2 += SMI2->Value * SMI2->Value;
						index++;
					}
					else {
						break;
					}
				}
				if (cmp == 0) {
					cov += SMI2->Value * SMI1->Value;
				}
				norm1 += SMI1->Value * SMI1->Value;
			}
			for (int i = index; i < count2; i++) {
				SMatrixItem *SMI2 = &RowItem(row2, i);
				norm2 += SMI2->Value * SMI2->Value;
			}

			if (norm1 > 0.0f && norm2 > 0.0f) {
				float cos = (float)(cov / (sqrt(norm1) * sqrt(norm2)));
				return cos;
			}
			else {
				return 0.0f;
			}
		}
		else {
			return 0.0f;
		}
	}
}

// ---------------------------------------------------------------------------
int __fastcall func_SMIrowCmp(void * Item1, void * Item2) {
	return ((SMatrixItem*)Item1)->Row - ((SMatrixItem*)Item2)->Row;
}

// ---------------------------------------------------------------------------
int __fastcall func_SMIcolCmp(void * Item1, void * Item2) {
	return ((SMatrixItem*)Item1)->Col - ((SMatrixItem*)Item2)->Col;
}

// ---------------------------------------------------------------------------
void TSMatrix::Finalize(bool bAdd, float *pgpos, bool *terminated, float pgstep)
{
	bool term = false;
	if (terminated == NULL) {
		terminated = &term;
	}
	// Sort each Col by Row to find duplicates
	for (int i = 0; i < m_Cols->Count; i++) {
		TFListforTP *list = ColList(i);
		int listcount = list->Count;
		if (listcount) {
			list->Sort(func_SMIrowCmp);
			SMatrixItem *last = (SMatrixItem*)list->Items[0]; // &ColItem(i, 0);
			for (int i2 = 1; i2 < listcount;) {
				SMatrixItem *smi = (SMatrixItem*)list->Items[i2];
				// &ColItem(i, i2);
				if (last->Row == smi->Row) {
					last->Flag = true; // Mark item for deletion
					if (bAdd) {
						// Sum values
						smi->Value += last->Value;
						last->Value = 0.0f;
					}
					list->Delete(i2 - 1); // Remove from ColList
					listcount--;
				}
				else {
					i2++;
					last = smi;
				}
			}
		}
	}
	if (pgpos) {
		*pgpos += pgstep;
		Application->ProcessMessages();
	}

	if (!*terminated) {
		// Remove from RowList
		for (int i = 0; i < m_Rows->Count; i++) {
			TFListforTP *list = RowList(i);
			int listcount = list->Count;
			list->Sort(func_SMIcolCmp);
			for (int i2 = 0; i2 < listcount;) {
				if (((SMatrixItem*)list->Items[i2])->Flag)
				{
					list->Delete(i2); // Remove from RowList
					listcount--;
				}
				else {
					i2++;
				}
			}
		}
		if (pgpos) {
			*pgpos += pgstep;
			Application->ProcessMessages();
		}
	}

	// Remove from Items
	if (!*terminated) {
		int listcount = m_Items->Count;
		for (int i2 = 0; i2 < listcount;) {
			SMatrixItem *item = &Item(i2);
			if (item->Flag) {
				delete item;
				m_Items->Delete(i2);
				listcount--;
			}
			else {
				i2++;
			}
		}

		if (pgpos) {
			*pgpos += pgstep;
			Application->ProcessMessages();
		}
	}
}

// ---------------------------------------------------------------------------
int __fastcall func_valueCmp(void * Item1, void * Item2) {
	return ((IDValue *)Item1)->Value - ((IDValue *)Item2)->Value;
}

// ---------------------------------------------------------------------------
void TSMatrix::DeleteSameCol(bool bAdd) {
	// Remove Cols with identical values (Col Index unchanged)

	// Sort each Col by count to find same Cols
	int docnum = m_Rows->Count;
	TFListforTP *IDVList = new TFListforTP();
	for (int i = 0; i < m_Cols->Count; i++) {
		IDValue *IDV = new IDValue;
		IDV->ID = i;
		int count = ColList(i)->Count;
		if (count) {
			IDV->Value = count * docnum + ColItem(i, 0).Row;
		}
		else {
			IDV->Value = 0;
		}
		IDVList->Add(IDV);
	}
	IDVList->Sort(func_valueCmp);

	if (IDVList->Count) {
		for (int il = 0; il < IDVList->Count; il++) {
			int value = ((IDValue*)IDVList->Items[il])->Value;
			int col1 = ((IDValue*)IDVList->Items[il])->ID;
			TFListforTP *list1 = ColList(col1);
			int count = list1->Count;
			for (int il2 = il + 1; il2 < IDVList->Count; il2++) {
				int value2 = ((IDValue*)IDVList->Items[il2])->Value;
				if (value2 == value) {
					// Same count
					bool same = true;

					// Check equality
					int col2 = ((IDValue*)IDVList->Items[il2])->ID;
					TFListforTP *list2 = ColList(col2);
					for (int i = 0; i < count && same; i++) {
						// same &= (ColItem(col1, i).Row == ColItem(col2, i).Row);
						same &= ((SMatrixItem *)list1->Items[i])->Row ==
							((SMatrixItem *)list2->Items[i])->Row;
					}
					if (same) {
						// Match
						for (int i = 0; i < count; i++) {
							SMatrixItem * item1 = (SMatrixItem*)list1->Items[i];
							if (bAdd) {
								((SMatrixItem*)list2->Items[i])->Value +=
									item1->Value;
								item1->Value = 0.0f;
							}
							item1->Flag = true;
							/*
							 if (bAdd){
							 ColItem(col2, i).Value += ColItem(col1, i).Value;
							 ColItem(col1, i).Value = 0.0f;
							 }
							 ColItem(col1, i).Flag = true;
							 */
						}
						list1->Clear();
						break;
					}
				}
				else {
					break;
				}
			}
		}
	}
	for (int i = 0; i < IDVList->Count; i++) {
		delete(IDValue*)IDVList->Items[i];
	}
	delete IDVList;

	// Remove from RowList
	for (int i = 0; i < m_Rows->Count; i++) {
		TFListforTP *list = RowList(i);
		int listcount = list->Count;
		for (int i2 = 0; i2 < listcount;) {
			if (((SMatrixItem*)list->Items[i2])->Flag)
			{
				list->Delete(i2);
				listcount--;
			}
			else {
				i2++;
			}
		}
	}

	// Remove from Items
	{
		int listcount = m_Items->Count;
		for (int i2 = 0; i2 < listcount;) {
			SMatrixItem *item = &Item(i2);
			if (item->Flag) {
				delete item;
				m_Items->Delete(i2);
				listcount--;
			}
			else {
				i2++;
			}
		}
	}
}

// ---------------------------------------------------------------------------
typedef struct ColColValue_ {
	int Col1, Col2;
	float Value;
} ColColValue;

// ---------------------------------------------------------------------------
int __fastcall func_ivalueCmp(void * Item1, void * Item2) {
	float f1 = ((IDValue*)Item1)->Value;
	float f2 = ((IDValue*)Item2)->Value;
	if (f1 > f2) {
		return -1;
	}
	else if (f1 < f2) {
		return 1;
	}
	else {
		return 0;
	}
}

// ---------------------------------------------------------------------------
void TSMatrix::MergeCol(int block, float threshold, int target) {
	int processed = 1; // Number of processed pairs
	int wcount;
	while (processed) {
		// Sort each Col by count to find same Cols
		wcount = 0; // Valid word count
		processed = 0;
		TFListforTP *IDVList = new TFListforTP();
		for (int i = 0; i < m_Cols->Count; i++) {
			IDValue *IDV = new IDValue;
			IDV->ID = i;
			IDV->Value = ColList(i)->Count;
			if (IDV->Value) {
				wcount++;
			}
			IDVList->Add(IDV);
		}

		if (wcount > target) {
			// Target word count not reached

			IDVList->Sort(func_valueCmp);

			// Process in blocks
			for (int ib = 0; ib <= wcount / block; ib++) {
				int boffset = ib * block;
				int bcount = wcount - boffset; // Count for this block
				if (bcount > block) {
					bcount = block;
				}
				if (ib > 0 && bcount < block / 2) {
					// Skip merge if block has less than half
					break;
				}

				// Build similar pair list
				TFListforTP *SimList = new TFListforTP();
				float debug_maxcos = 0.0f;
				for (int i = 0; i < bcount; i++) {
					int col1 = ((IDValue*)IDVList->Items[i + boffset])->ID;
					for (int i2 = i + 1; i2 < bcount; i2++) {
						int col2 = ((IDValue*)IDVList->Items[i2 + boffset])->ID;
						float cos = CosCol(col1, col2);
						if (cos > debug_maxcos) {
							debug_maxcos = cos;
						}
						if (cos > threshold) {
							// Above threshold, add
							ColColValue *CCV = new ColColValue;
							CCV->Col1 = col1;
							CCV->Col2 = col2;
							CCV->Value = cos;
							SimList->Add(CCV);
						}
					}
				}
				// Sort by similarity (descending)
				SimList->Sort(func_ivalueCmp);

				// Merge
				for (int i = 0; i < SimList->Count; i++) {
					ColColValue *CCV = (ColColValue*)SimList->Items[i];
					int lastindex = 0;
					for (int ii = 0; ii < ColList(CCV->Col1)->Count; ii++) {
						int cmp = 1;
						while (lastindex < ColList(CCV->Col2)->Count) {
							cmp = ColItem(CCV->Col1, ii).Row -
								ColItem(CCV->Col2, lastindex).Row;
							if (cmp > 0) {
								lastindex++;
							}
							else {
								break;
							}
						}
						if (cmp == 0) {
							// Same Row
							ColItem(CCV->Col2, lastindex).Value +=
								ColItem(CCV->Col1, ii).Value; // Add value
							ColItem(CCV->Col1, ii).Flag = true;
						}
						else {
							// No same Row

							// Move Col from Col1 to Col2
							ColItem(CCV->Col1, ii).Col = CCV->Col2;
							ColList(CCV->Col2)->Insert(lastindex++,
								&ColItem(CCV->Col1, ii));
						}
					}
					ColList(CCV->Col1)->Clear();

					// Check for duplicates
					for (int ix = i + 1; ix < SimList->Count;) {
						ColColValue *CCV2 = (ColColValue*)SimList->Items[ix];
						if ((CCV->Col1 == CCV2->Col1) ||
							(CCV->Col2 == CCV2->Col1) ||
							(CCV->Col2 == CCV2->Col2) ||
							(CCV->Col2 == CCV2->Col2)) {
							// Remove duplicate
							delete(ColColValue*)SimList->Items[ix];
							SimList->Delete(ix);
						}
						else {
							ix++;
						}
					}
				}
				processed += SimList->Count;

				// Discard similar pair list
				for (int i = 0; i < SimList->Count; i++) {
					delete(ColColValue *)SimList->Items[i];
				}
				delete SimList;
			}
		}

		for (int i = 0; i < IDVList->Count; i++) {
			delete(IDValue*)IDVList->Items[i];
		}
		delete IDVList;
	}

	// Remove from RowList
	for (int i = 0; i < m_Rows->Count; i++) {
		TFListforTP *list = RowList(i);
		int listcount = list->Count;
		for (int i2 = 0; i2 < listcount;) {
			if (RowItem(i, i2).Flag) {
				list->Delete(i2);
				listcount--;
			}
			else {
				i2++;
			}
		}
	}

	// Remove from Items
	{
		int listcount = m_Items->Count;
		for (int i2 = 0; i2 < listcount;) {
			SMatrixItem *item = &Item(i2);
			if (item->Flag) {
				delete item;
				m_Items->Delete(i2);
				listcount--;
			}
			else {
				i2++;
			}
		}
	}
}
/*
 //---------------------------------------------------------------------------
 TTextToFeature::TTextToFeature(TWideStringList *SL, int maxcombi, int maxcount)
 {
 // Extract words
 TTextDecomposer *TD = new TTextDecomposer(SL, maxcombi, maxcount);

 // Doc-word occurrence probability storage
 TSMatrix *SM = new TSMatrix();

 // Fill SM from TD result
 // Document loop
 for (int id = 0 ; id < SL->Count ; id++){
 // Character loop
 WideString doc = SL->Strings(id);
 int doclen = doc.Length();
 for (int ic = 0 ; ic < doclen ; ic++){
 int len = 1;
 int start = 0;
 int end = TD->m_Gram[0]->Count - 1;
 while (start >= 0){
 TWSandValueList *WSVL = TD->m_Gram[len - 1];
 WideString c = doc.SubString(ic + 1, len);
 // Search for this char in 1-char words
 int idx = WSVL->Search(start, end, c);
 if (idx >= 0){
 if (WSVL->Enabled(idx)){
 SM->Add(WSVL->SN(idx), id, 1.0f);
 }
 start = WSVL->From(idx);
 end = WSVL->To(idx);
 }else{
 break;
 }
 }
 }
 }

 // Remove duplicates
 SM->Finalize(true);

 delete SM;
 delete TD;
 }
 //---------------------------------------------------------------------------
 TTextToFeature::~TTextToFeature()
 {
 }
 //---------------------------------------------------------------------------
 */
String NormalizeLineBreaks(const String input, const String& lineBreak)
{
	String result = input;

	// CR+LF -> LF
	result = StringReplace(result, L"\r\n", L"\n", TReplaceFlags() << rfReplaceAll);

	// CR -> LF
	result = StringReplace(result, L"\r", L"\n", TReplaceFlags() << rfReplaceAll);

	// LF -> CR + LF
	result = StringReplace(result, L"\n", lineBreak, TReplaceFlags() << rfReplaceAll);


	return result;
}
//---------------------------------------------------------------------------
#pragma package(smart_init)
