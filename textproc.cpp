//---------------------------------------------------------------------------

#include <math.h>

#include "textproc.h"

//---------------------------------------------------------------------------
TDMatrix::TDMatrix(int num) :
    m_nDim(num)
{
    m_IDs = new int[m_nDim];
    m_Data = new float[m_nDim * m_nDim];
    m_IDValue = new TList();

    //inline対策
    ID(0);
    S(0, 0);
}
//---------------------------------------------------------------------------
TDMatrix::~TDMatrix()
{
    FreeIDValue();
    delete m_IDValue;
    delete[] m_Data;
    delete[] m_IDs;
}
//---------------------------------------------------------------------------
int &TDMatrix::ID(int idx)
{
    return m_IDs[idx];
}
//---------------------------------------------------------------------------
float &TDMatrix::S(int source, int target)
{
    return m_Data[source * m_nDim + target];
}
//---------------------------------------------------------------------------
void TDMatrix::FreeIDValue()
{
    for (int i = 0 ; i < m_IDValue->Count ; i++){
        delete (IDValue*)m_IDValue->Items[i];
    }
    m_IDValue->Clear();
}
//---------------------------------------------------------------------------
int __fastcall func_IDCmp(void * Item1, void * Item2)
{
    return ((IDValue *)Item1)->ID - ((IDValue *)Item2)->ID;
}
//---------------------------------------------------------------------------
void TDMatrix::Finalize()
{
    FreeIDValue();

    //ソートされたIDとIndexのデータを作る
    for (int i = 0 ; i < m_nDim ; i++){
        IDValue *IDV = new IDValue;
        IDV->ID = ID(i);
        IDV->Value = i;
        m_IDValue->Add(IDV);
    }
    m_IDValue->Sort(func_IDCmp);
}
//---------------------------------------------------------------------------
int TDMatrix::SearchIndex(int ID)
{
    //IDのIndexを検索
    register int step = m_IDValue->Count >> 1;
    register int pos = step;
    register int count = 0;//1ずつシフトしても見つからなかった回数
    while (true){
        int Cmp = ID - ((IDValue*)m_IDValue->Items[pos])->ID;
        if (Cmp == 0){
            return ((IDValue*)m_IDValue->Items[pos])->Value;
        }else{
            int laststep = step;
            step >>= 1;
            if (step < 1){
                step = 1;
                count++;
                if (count >= 2){
                    return -1;
                }
            }else if (step << 1 != laststep){
                count--;
            }
            if (Cmp < 0){
                pos -= step;
                if (pos < 0){
                    return -1;
                }
            }else{
                pos += step;
                if (pos >= m_IDValue->Count){
                    return -1;
                }
            }
        }
    }
    /*
    //IDのIndexを検索
    int step = m_IDValue->Count >> 1;
    int pos = step;
    int count = 0;//1ずつシフトしても見つからなかった回数
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
//---------------------------------------------------------------------------
//コンストラクタ
TFList::TFList() :
    m_nSize(0),
    m_nCount(0),
    m_Data(NULL)
{
}
//---------------------------------------------------------------------------
//デストラクタ
__fastcall TFList::~TFList()
{
    Clear();
}
//---------------------------------------------------------------------------
//ポインタリストを倍にする
void TFList::Twice()
{
    //次のサイズを決める
    void **newdata;
    int lastsize = m_nSize;
    if (m_nSize){
        m_nSize *= 2;
    }else{
        m_nSize = MINFLISTBUFSIZE;
    }

    //新しい格納場所を作る
    newdata = new void*[m_nSize];
    if (lastsize){
        //これまでの結果をコピー
        memcpy(newdata, m_Data, sizeof(void*) * m_nCount);
        delete[] m_Data;
    }

    //新しい格納場所を代入
    m_Data = newdata;
}
//---------------------------------------------------------------------------
//ポインタリストを半分にする
void TFList::Half()
{
    //次のサイズを決める
    void **newdata;
    m_nSize /= 2;

    //新しい格納場所を作る
    if (m_nSize){
        //サイズがある
        newdata = new void*[m_nSize];
        //これまでの結果をコピー
        memcpy(newdata, m_Data, sizeof(void*) * m_nCount);
        delete[] m_Data;

        //新しい格納場所を代入
        m_Data = newdata;
    }else{
        delete[] m_Data;
        m_Data = NULL;
    }
}

void __fastcall QuickSort(void **SortList, int L, int R, TFListSortCompare SCompare)
{
    register int I = L;
    while (I < R){
        I = L;
        register int J = R;
        register void *P = SortList[(L + R) >> 1];
        while (I <= J){
            while (SCompare(SortList[I], P) < 0){
                I++;
            }
            while (SCompare(SortList[J], P) > 0){
                J--;
            }
            if (I <= J){
                register void *T = SortList[I];
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
//---------------------------------------------------------------------------
//クイックソート
void TFList::QuickSort(TFListSortCompare CompFunc, int nLeft, int nRight)
{
    if (m_nCount <= 1){
        return;
    }

    //ピポット選択
    void *p = m_Data[(nLeft + nRight + 1) / 2];
    int Left = nLeft;
    int Right = nRight;

    while (true){
        //左からp以上を探す
        int lefti = Left;
        while (CompFunc(m_Data[lefti], p) > 0){//pより小さい
            lefti++;
        }

        //右からp以下を探す
        int righti = Right;
        while (CompFunc(m_Data[righti], p) < 0){//pより大きい
            righti--;
        }

        if (lefti < righti){
            //並び替え発生
            void *tmp = m_Data[righti];
            m_Data[righti] = m_Data[lefti];
            m_Data[lefti] = tmp;
            Left = lefti + 1;
            Right = righti - 1;
        }else{
            //ピポットの左はピポット以下、右はピポット以上になった

            //左右に分割してソート
            if (nLeft < lefti - 1){
                //左をソート
                QuickSort(CompFunc, nLeft, lefti - 1);
            }
            if (lefti < nRight){
                //右をソート
                QuickSort(CompFunc, lefti, nRight);
            }
            break;
        }
    }
}
//---------------------------------------------------------------------------
//数を得る
int TFList::FCount()
{
    return m_nCount;
}
//---------------------------------------------------------------------------
//追加
void *TFList::Add(void *p)
{
    if (m_nCount >= m_nSize){
        //格納場所が足りなければ拡張
        Twice();
    }
    m_Data[m_nCount++] = p;
    return p;
}
//---------------------------------------------------------------------------
//変更
void *TFList::Set(int nIndex, void *p)
{
    m_Data[nIndex] = p;
    return p;
}
//---------------------------------------------------------------------------
//挿入
void *TFList::Insert(int nIndex, void *p)
{
    if (nIndex < m_nCount){
        if (m_nCount >= m_nSize){
            //格納場所が足りなければ拡張
            Twice();
        }

        //挿入箇所以降を1つ後ろにずらす
        for (int i = m_nCount ; i > nIndex ; i--){
            m_Data[i] = m_Data[i - 1];
        }
        m_Data[nIndex] = p;
        m_nCount++;

        return p;
    }else{
        //最後に挿入（Addと同じ動作）
        return Add(p);
    }
}
//---------------------------------------------------------------------------
//削除
void TFList::Delete(int nindex)
{
    //nindex番目のデータを削除

    //nindex以降のデータを1つ手前にシフト
    for (int i = nindex ; i < m_nCount - 1 ; i++){
        m_Data[i] = m_Data[i + 1];
    }

    //数を減らす
    m_nCount--;

    //格納場所が大きすぎたら半分に
    if (m_nCount <= m_nSize / 2 && m_nCount > MINFLISTBUFSIZE){
        Half();
    }
}
//---------------------------------------------------------------------------
//参照
void *TFList::FItems(int nindex)
{
    return m_Data[nindex];
}
void TFList::FWrite(int nindex, void *P)
{
    m_Data[nindex] = P;
}
//---------------------------------------------------------------------------
//全削除
void TFList::Clear()
{
    if (m_nSize){
        delete[] m_Data;
        m_nSize = 0;
        m_nCount = 0;
        m_Data = NULL;
    }
}
//---------------------------------------------------------------------------
//ソート
void TFList::Sort(TFListSortCompare CompFunc)
{
//    QuickSort(CompFunc, 0, Count - 1);
    if (m_nCount <= 1){
        return;
    }
    ::QuickSort(m_Data, 0, Count - 1, CompFunc);
}
//---------------------------------------------------------------------------
//シャッフル
void TFList::Shuffle()
{
    for (int i = 0 ; i < m_nCount ; i++){
        int idx = rand() % m_nCount;
        void *tmp = m_Data[i];
        m_Data[i] = m_Data[idx];
        m_Data[idx] = tmp;
    }
}
//---------------------------------------------------------------------------
/*
int __fastcall func_pwsCmp(void * Item1, void * Item2)
{
    WideString *WS1 = (WideString *)Item1;
    WideString *WS2 = (WideString *)Item2;
    return WideCompareText(*WS1, *WS2);
}
*/
//#define DEBUG_SPEEDTUNE
#ifdef DEBUG_SPEEDTUNE
#include <mmsystem.hpp>
#endif
//---------------------------------------------------------------------------
TTextDecomposer::TTextDecomposer(TWideStringList *SL, int maxcombi, int maxcount, float pgmin, float pgmax, float &pgpos, bool &terminated, int option) :
    m_nMaxCombi(maxcombi)
{
    #ifdef DEBUG_SPEEDTUNE
    //デバッグ用変数
    int debug_orgtotal = 0;//もともとの組み合わせ総数

    int debug_fewdeleted = 0;//より短い組み合わせが1回以下しか出現していなかったため削除
    int debug_dplxdeleted = 0;//重複で削除された組み合わせの総数
    int debug_shortdeleted = 0;//より短い組み合わせがより長い組み合わせと同数だったため削除

    int debug_totalremain = 0;//抽出された組み合わせ総数

    unsigned int time_bunkai = 0, time_sort = 0, time_rekkyo = 0, t;
    #endif
    WideString Ret = "\n";

    m_Gram = new TWSandValueList*[maxcombi];
    //N-gramループ
    for (int i = 0 ; i < m_nMaxCombi ; i++){
        m_Gram[i] = new TWSandValueList();
    }
    TWSandValueList *lastgram = NULL;
    for (int i = 0 ; i < m_nMaxCombi && !terminated ; i++){
        TWideStringList *Tmp = new TWideStringList();//各カードの重複なし組み合わせ格納用
        if (i > 0){
            if (lastgram->Count == 0){
                //もう組み合わせが無い
                break;
            }
        }

        TWSandValueList *gram = m_Gram[i];

        //カードループ
        for (int ic = 0 ; ic < SL->Count ; ic++){
            //小文字化
            //WideString WS = SL->Strings(ic);
			WideString WS;
			if (option & 0x1) {
				//小文字化禁止
				WS = SL->Strings(ic);
			}else{
            	//通常はロバストネスのため小文字に統一
				WS = WideLowerCase(SL->Strings(ic));
			}

            //if (i > 0){
                #ifdef DEBUG_SPEEDTUNE
                t = timeGetTime();
                #endif
                //組み合わせ分解
                int len = i + 1;
                int wslen = WS.Length();
                TWideStringList *Tmp2 = new TWideStringList();//カード内の全組み合わせ格納用
                /*
                for (int i2 = 0 ; i2 < wslen - i ; i2++){
                    WideString W1 = WS.SubString(i2 + 1, len);
                    int p = W1.Pos("\n");
                    if (p == 0){
                        Tmp2->Add(W1);
                    }else{
                        i2 += p - 1;
                    }
                }
                //*/
                //*
                int count = 0;
                wchar_t *w1 = &WS[1];
                for (int i2 = 0 ; i2 < wslen ; i2++){
                    if (*w1++ == '\n'){
                        count = 0;
                    }else{
                        if (count < len){
                            count++;
                        }
                        if (count == len){
                            Tmp2->Add(WS.SubString(i2 - i + 1, len));
                        }
                    }
                }
                //*/
                #ifdef DEBUG_SPEEDTUNE
                time_bunkai += timeGetTime() - t;

                t = timeGetTime();
                #endif
                Tmp2->Sort();
                #ifdef DEBUG_SPEEDTUNE
                debug_orgtotal += Tmp2->Count;
                time_sort += timeGetTime() - t;
                #endif

                //重複削除しながらこのカードの組み合わせに追加
                //WS = "";
                #ifdef DEBUG_SPEEDTUNE
                t = timeGetTime();
                #endif
                if (Tmp2->Count > 0){
                    if (i > 0){
                        //2回以上出現している組み合わせを含むより長い組み合わせのみ残す
                        register int lastindex = 0;
                        int lastcount = lastgram->Count;

                        WideString *TSL;//直前の文字列保存用

                        int tmp2count = Tmp2->Count;
                        for (int i3 = 0 ; i3 < tmp2count ; i3++){
                            WideString *TS1 = Tmp2->StringsP(i3);//追加すべきテキスト
                            //WideString *TS1 = (WideString*)&Tmp2->Strings(i3);//追加すべきテキスト
                            //bool add = true;
                            bool add = i3 == 0 ? true : (WideCompareStr(*TSL, *TS1) != 0);
                            //bool add = i3 == 0 ? true : (wcscmp(TSL, TS1) != 0);
                            if (add){//Tmp2->Strings(i3)を追加すべき
                                WideString TSC = TS1->SubString(1, i);//1文字短い追加対象
                                //WideString TSC = ((WideString)TS1).SubString(1, i);//1文字短い追加対象
                                int cmp = 1;
                                while (lastindex < lastcount){
                                    cmp = WideCompareStr(lastgram->Strings(lastindex), TSC);
                                    //cmp = wcscmp(lastgram->Strings(lastindex), TSC);
                                    if (cmp < 0){
                                        lastindex++;
                                    }else{
                                        break;
                                    }
                                }
                                if (cmp == 0){
                                    //WS += TS;
                                    //*
                                    Tmp->AddP(TS1);
                                    Tmp2->SetNULL(i3);
                                    //*/
                                    //Tmp->AddA(*TS1);
                                }
                                #ifdef DEBUG_SPEEDTUNE
                                else{
                                    debug_fewdeleted++;
                                }
                                #endif
                            }
                            #ifdef DEBUG_SPEEDTUNE
                            else{
                                debug_fewdeleted++;
                            }
                            #endif
                            TSL = TS1;
                        }
                    }else{

                        //全ての組み合わせを残す
                        WideString *WSL;

                        int tmp2count = Tmp2->Count;
                        for (int i3 = 0 ; i3 < tmp2count ; i3++){
                            register WideString *TS1 = Tmp2->StringsP(i3);
                            //register WideString *TS1 = (WideString*)&Tmp2->Strings(i3);

                            //bool add = true;
                            bool add = i3 == 0 ? true : *TS1 != Ret && WideCompareStr(*WSL, *TS1) != 0;
                            if (add){
                                //Tmp->AddA(*TS1);
                                Tmp->AddP(TS1);
                                Tmp2->SetNULL(i3);
                            }
                            #ifdef DEBUG_SPEEDTUNE
                            else{
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

            //Tmp += WS;
            /*
            int len2 = Tmp.Length();
            if (len2 % (i + 1) != 0){
                Tmp = "";
            }
            //*/
        }//カードループ

        //全カードの結果合成
        //組み合わせ分解
        /*
        int len = i + 1;
        int count = Tmp.Length() / len;
        TWideStringList *Tmp2 = new TWideStringList();//カード内の全組み合わせ格納用
        for (int i2 = 0 ; i2 < count ; i2++){
            Tmp2->Add(Tmp.SubString(i2 * len + 1, len));
        }
        Tmp2->Sort();
        */
        Tmp->Sort();

        //重複削除かつ2回出現したもののみこのカードの組み合わせに追加
        int count = 0;
        int tmpcount = Tmp->Count;
        if (tmpcount > 0){
            WideString *WSL = Tmp->StringsP(0);
            //WideString *WSL = (WideString*)&Tmp->Strings(0);

            for (int i3 = 1 ; i3 < tmpcount ; i3++){
                WideString *TS = Tmp->StringsP(i3);
                //WideString *TS = (WideString*)&Tmp->Strings(i3);
                if (WideCompareStr(*WSL, *TS) != 0){
                    //異なる
                    if (count > 0){
                        count++;
                        gram->Values(gram->Count - 1) = count;
                        if (maxcount > 0 && count >= maxcount){
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
                }else{
                    //同じ
                    count ++;
                    if (count == 1){
                        //gram->AddA(*TS);
                        gram->AddP(TS);
                        Tmp->SetNULL(i3);
                    }
                    #ifdef DEBUG_SPEEDTUNE
                    else{
                        debug_dplxdeleted++;
                    }
                    #endif
                }
                WSL = TS;
            }
            if (count > 0){
                count++;
                gram->Values(gram->Count - 1) = count;
                if (maxcount > 0 && count >= maxcount){
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
        //m_Gramとm_GramCountの数が同じかチェック（デバッグ用）
        if (gram->Count != m_GramCount[i]->Count){
            m_GramCount[i]->Add(NULL);
        }
        */

        //(N-1)gramと出現回数が同じなら(N-1)gramを削除。同時にindex対応付け
        if (i > 0){
            int lastindex = 0;
            int lastcount = lastgram->Count;
            int count = gram->Count;
            for (int i3 = 0 ; i3 < count ; i3++){
                WideString TSC = gram->Strings(i3).SubString(1, i);//1文字短いテキスト
                int cmp = 1;
                while (lastindex < lastcount){
                    if ((cmp = WideCompareStr(lastgram->Strings(lastindex), TSC)) < 0){
                        lastindex++;
                    }else{
                        break;
                    }
                }
                if (cmp == 0){
                    if (lastgram->From(lastindex) == -1){
                        lastgram->From(lastindex) = i3;
                    }
                    if (lastgram->Values(lastindex) == gram->Values(i3)){
                        if (lastgram->Enabled(lastindex)){
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

        //2文字目以降でソート
        gram->Sort2();

        //2文字目以降で(N-1)gramと出現回数が同じなら(N-1)gramを削除
        if (i > 0){
            int lastindex = 0;
            int lastcount = lastgram->Count;
            int count = gram->Count;
            for (int i3 = 0 ; i3 < count ; i3++){
                WideString TSC = gram->Strings(i3).SubString(2, i);//1文字短いテキスト
                int cmp = 1;
                while (lastindex < lastcount){
                    if ((cmp = WideCompareStr(lastgram->Strings(lastindex), TSC)) < 0){
                        lastindex++;
                    }else{
                        break;
                    }
                }
                if (cmp == 0){
                    if (lastgram->Values(lastindex) == gram->Values(i3)){
                        if (lastgram->Enabled(lastindex)){
                            lastgram->Enabled(lastindex) = false;
                            #ifdef DEBUG_SPEEDTUNE
                            debug_shortdeleted++;
                            #endif
                        }
                    }
                }
            }
        }

        //順序を元に（1文字目からのソートに）戻す
        gram->Sort();

        delete Tmp;

        lastgram = gram;

        //Progress
        if (pgmax > pgmin){
            pgpos = ((pgmax - pgmin) * (i + 1)) / m_nMaxCombi + pgmin;
            Application->ProcessMessages();
        }
    }//N-gramループ

    //合計計算、シリアル振り
    m_nMaxSN = 0;
    for (int i = 0 ; i < m_nMaxCombi ; i++){
        TWSandValueList *gram = m_Gram[i];
        int gramcount = gram->Count;
		for (int i3 = 0 ; i3 < gramcount ; i3++) if (gram->Enabled(i3)){
            #ifdef DEBUG_SPEEDTUNE
            debug_totalremain++;
            #endif
            gram->SN(i3) = m_nMaxSN++;
        }
    }
}
//---------------------------------------------------------------------------
TTextDecomposer::~TTextDecomposer()
{
    for (int i = 0 ; i < m_nMaxCombi ; i++){
        delete m_Gram[i];
    }
    delete[] m_Gram;
}
//---------------------------------------------------------------------------
/*
WideString *TTextDecomposer::GetStr(int list, int index)
{
    return (WideString *)m_Gram[list]->Items[index];
}
*/
//---------------------------------------------------------------------------
//TWideStringList
//---------------------------------------------------------------------------
_fastcall TWideStringList::~TWideStringList()
{
    Clear();
}
//---------------------------------------------------------------------------
void _fastcall TWideStringList::Clear()
{
    /*
    for (int i = Count - 1 ; i >= 0 ; i--){
        delete (WideString*)Items[i];
    }
    */
    for (int i = 0 ; i < Count ; i++){
        if (Items[i]){
            delete (WideString*)Items[i];
        }
    }
    TFListforTP::Clear();
}
//---------------------------------------------------------------------------
//追加
void TWideStringList::Add(WideString S)
{
    WideString *S1 = new WideString;
    *S1 = S;
    TFListforTP::Add(S1);
}
//---------------------------------------------------------------------------
//追加
void TWideStringList::AddA(WideString &S)
{
    WideString *S1 = new WideString;
    *S1 = S;
    TFListforTP::Add(S1);
}
//---------------------------------------------------------------------------
void TWideStringList::AddP(WideString *S)
{
    TFListforTP::Add(S);
}
//---------------------------------------------------------------------------
void TWideStringList::SetNULL(int nindex)
{
    Items[nindex] = NULL;
}
//---------------------------------------------------------------------------
//削除
void TWideStringList::Delete(int nindex)
{
    if (Items[nindex]){
        delete (WideString*)Items[nindex];
    }
    TFListforTP::Delete(nindex);
}
//---------------------------------------------------------------------------
//参照
WideString &TWideStringList::Strings(int nindex)
{
    return *(WideString*)Items[nindex];
}
//---------------------------------------------------------------------------
//参照
WideString *TWideStringList::StringsP(int nindex)
{
    return (WideString*)Items[nindex];
}
//---------------------------------------------------------------------------
int __fastcall WideStringSortCompare(void * Item1, void * Item2)
{
    return WideCompareStr(*(WideString*)Item1, *(WideString*)Item2);
}
//---------------------------------------------------------------------------
//昇順ソート
void TWideStringList::Sort()
{
    TFListforTP::Sort(WideStringSortCompare);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//TWSandValueList
//---------------------------------------------------------------------------
_fastcall TWSandValueList::~TWSandValueList()
{
    Clear();
}
//---------------------------------------------------------------------------
void _fastcall TWSandValueList::Clear()
{
    for (int i = 0 ; i < Count ; i++){
        delete ((WSandValue*)Items[i])->WS;
        delete (WSandValue*)Items[i];
    }
    TFListforTP::Clear();
}
//---------------------------------------------------------------------------
//追加
void TWSandValueList::Add(WideString S)
{
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
//---------------------------------------------------------------------------
//追加
void TWSandValueList::AddA(WideString &S)
{
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
//---------------------------------------------------------------------------
//追加
void TWSandValueList::AddP(WideString *S)
{
    WSandValue *S1 = new WSandValue;
    (*S1).WS = S;
    (*S1).Value = 0;
    (*S1).Enabled = true;
    (*S1).From = -1;
    (*S1).To = -1;
    (*S1).SN = -1;
    TFListforTP::Add(S1);
}
//---------------------------------------------------------------------------
//削除
void TWSandValueList::Delete(int nindex)
{
    delete ((WSandValue*)Items[nindex])->WS;
    delete (WSandValue*)Items[nindex];
    TFListforTP::Delete(nindex);
}
//---------------------------------------------------------------------------
//参照
WideString &TWSandValueList::Strings(int nindex)
{
    return *(*(WSandValue*)Items[nindex]).WS;
}
//---------------------------------------------------------------------------
//参照
int &TWSandValueList::Values(int nindex)
{
    return (*(WSandValue*)Items[nindex]).Value;
}
//---------------------------------------------------------------------------
//参照
bool &TWSandValueList::Enabled(int nindex)
{
    return (*(WSandValue*)Items[nindex]).Enabled;
}
//---------------------------------------------------------------------------
//参照
int &TWSandValueList::From(int nindex)
{
    return (*(WSandValue*)Items[nindex]).From;
}
//---------------------------------------------------------------------------
//参照
int &TWSandValueList::To(int nindex)
{
    return (*(WSandValue*)Items[nindex]).To;
}
//---------------------------------------------------------------------------
//参照
int &TWSandValueList::SN(int nindex)
{
    return (*(WSandValue*)Items[nindex]).SN;
}
//---------------------------------------------------------------------------
int _fastcall WSandValueSortCompare(void * Item1, void * Item2)
{
    return WideCompareStr(*(*(WSandValue*)Item1).WS, *(*(WSandValue*)Item2).WS);
}
//---------------------------------------------------------------------------
int _fastcall WSandValueSortCompare2(void * Item1, void * Item2)
{
    //2文字目以降
    return WideCompareStr((*(WSandValue*)Item1).WS->SubString(2, 32767), (*(WSandValue*)Item2).WS->SubString(2, 32767));
}
//---------------------------------------------------------------------------
int _fastcall WSandValueSortCompareValue(void * Item1, void * Item2)
{
    //2文字目以降
    return (*(WSandValue*)Item1).Value - (*(WSandValue*)Item2).Value;
}
//---------------------------------------------------------------------------
//昇順ソート
void TWSandValueList::Sort()
{
    TFListforTP::Sort(WSandValueSortCompare);
}
//---------------------------------------------------------------------------
//昇順ソート
void TWSandValueList::Sort2()
{
    TFListforTP::Sort(WSandValueSortCompare2);
}
//---------------------------------------------------------------------------
//値でソート
void TWSandValueList::SortValue()
{
	TFListforTP::Sort(WSandValueSortCompareValue);
}
//---------------------------------------------------------------------------
int TWSandValueList::Search(int from, int to, WideString &Key)
{
    //ソートされている前提で、数当て方式でfrom～toの間でKey（完全一致）を検索
    register int step = (to + 1 - from) >> 1;
    register int pos = from + step;
    register int count = 0;//1ずつシフトしても見つからなかった回数
    while (true){
        int CmpText = WideCompareStr(Strings(pos), Key);
        if (CmpText == 0){
            return pos;
        }else{
            int laststep = step;
            step >>= 1;
            if (step < 1){
                step = 1;
                count++;
                if (count >= 2){
                    return -1;
                }
            }else if (step << 1 != laststep){
                count--;
            }
            if (CmpText > 0){//Key < pos
                pos -= step;
                if (pos < 0){
                    return -1;
                }
            }else{
                pos += step;
                if (pos > to){
                    return -1;
                }
            }
        }
    }
    /*
    int step = (to + 1 - from) / 2;
    int pos = from + step;
    int count = 0;//1ずつシフトしても見つからなかった回数
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
            if (CmpText > 0){//Key < pos
                pos -= step;
            }else{
                pos += step;
            }
        }
    }
    return -1;
    */
}
//---------------------------------------------------------------------------
TSMatrix::TSMatrix()
{
    m_Items = new TFListforTP();
    m_Rows = new TFListforTP();
    m_Cols = new TFListforTP();
    RowNorm = NULL;
    ColNorm = NULL;
}
//---------------------------------------------------------------------------
void TSMatrix::Dummy()
{
    Add(0, 0, 0.0f);
}
//---------------------------------------------------------------------------
TSMatrix::~TSMatrix()
{
    if (RowNorm){
        delete[] RowNorm;
    }
    if (ColNorm){
        delete[] ColNorm;
    }
    for (int i = 0 ; i < m_Cols->Count ; i++){
        delete ColList(i);
    }
    delete m_Cols;

    for (int i = 0 ; i < m_Rows->Count ; i++){
        delete RowList(i);
    }
    delete m_Rows;

    for (int i = 0 ; i < m_Items->Count ; i++){
        delete &Item(i);
    }
    delete m_Items;
}
//---------------------------------------------------------------------------
void TSMatrix::Add(int col, int row, float value)
{
    if (m_Cols->Count < col + 1){
        for (int i = m_Cols->Count ; i <= col ; i++){
            m_Cols->Add(new TFListforTP());
        }
    }

    if (m_Rows->Count < row + 1){
        for (int i = m_Rows->Count ; i <= row ; i++){
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
//---------------------------------------------------------------------------
void TSMatrix::PrepareCosRow()
{
    int rowcount = m_Rows->Count;
    if (RowNorm){
        delete[] RowNorm;
    }
    RowNorm = new float[rowcount];

    //行ループ
    for (int i = 0 ; i < rowcount ; i ++){
        TFListforTP *list = RowList(i);
        int count1 = list->Count;
        float norm = 0.0f;
        for (int ic = 0 ; ic < count1 ; ic++){
            float f = ((SMatrixItem *)list->Items[ic])->Value;//RowItem(i, ic).Value;
            norm += f * f;
        }
        if (norm > 0.0f){
            RowNorm[i] = 1.0f / sqrt(norm);
        }else{
            RowNorm[i] = 0.0f;
        }
    }
}
//---------------------------------------------------------------------------
float TSMatrix::CosCol(int col1, int col2)
{
    if (col1 >= m_Cols->Count || col2 >= m_Cols->Count){
        return 0.0f;
    }

    //Cos距離
    int count1 = ColList(col1)->Count;
    int count2 = ColList(col2)->Count;
    if (count1 > 0 && count2 > 0){
        float cov = 0.0f;
        float norm1 = 0.0f, norm2 = 0.0f;
        int index = 0;
        for (int i = 0 ; i < count1 ; i++){
            while (index < count2){
                if (ColItem(col2, index).Row < ColItem(col1, i).Row){
                    norm2 += ColItem(col2, index).Value * ColItem(col2, index).Value;
                    index++;
                }else{
                    break;
                }
            }
            if (index < count2){
                if (ColItem(col2, index).Row == ColItem(col1, i).Row){
                    cov += ColItem(col2, index).Value * ColItem(col1, i).Value;
                }
            }
            norm1 += ColItem(col1, i).Value * ColItem(col1, i).Value;
        }
        for (int i = index ; i < count2 ; i++){
            norm2 += ColItem(col2, i).Value * ColItem(col2, i).Value;
        }

        if (norm1 > 0.0f && norm2 > 0.0f){
            float cos = (float)(cov / (sqrt(norm1) * sqrt(norm2)));
            return cos;
        }else{
            return 0.0f;
        }
    }else{
        return 0.0f;
    }
}
//---------------------------------------------------------------------------
float TSMatrix::CosRow(int row1, int row2)
{
    if (row1 >= m_Rows->Count || row2 >= m_Rows->Count){
        return 0.0f;
    }
    
    if (RowNorm){
        //Norm計算済み
        //Cos距離
        if (RowNorm[row1] > 0.0f && RowNorm[row2] > 0.0f){
            TFListforTP * list1 = RowList(row1);
            TFListforTP * list2 = RowList(row2);
            int count1 = list1->Count;
            int count2 = list2->Count;

            float cov = 0.0f;
            int index = 0;
            for (int i = 0 ; i < count1 ; i++){
                int cmp = 1;
                SMatrixItem *SMI1 = (SMatrixItem *)list1->Items[i];//&RowItem(row1, i);
                SMatrixItem *SMI2 = NULL;
                while (index < count2){
                    SMI2 = (SMatrixItem *)list2->Items[index];//&RowItem(row2, index);
                    cmp = SMI2->Col - SMI1->Col;
                    if (cmp < 0){
                        index++;
                    }else{
                        break;
                    }
                }
                if (cmp == 0){
                    cov += SMI2->Value * SMI1->Value;
                }
            }

            return cov * RowNorm[row1] * RowNorm[row2];
        }else{
            return 0.0f;
        }
    }else{
        //Cos距離
        int count1 = RowList(row1)->Count;
        int count2 = RowList(row2)->Count;
        if (count1 > 0 && count2 > 0){
            float cov = 0.0f;
            float norm1 = 0.0f, norm2 = 0.0f;
            int index = 0;
            for (int i = 0 ; i < count1 ; i++){
                int cmp = 1;
                SMatrixItem *SMI1 = &RowItem(row1, i);
                SMatrixItem *SMI2 = NULL;
                while (index < count2){
                    SMI2 = &RowItem(row2, index);
                    cmp = SMI2->Col - SMI1->Col;
                    if (cmp < 0){
                        norm2 += SMI2->Value * SMI2->Value;
                        index++;
                    }else{
                        break;
                    }
                }
                if (cmp == 0){
                    cov += SMI2->Value * SMI1->Value;
                }
                norm1 += SMI1->Value * SMI1->Value;
            }
            for (int i = index ; i < count2 ; i++){
                SMatrixItem *SMI2 = &RowItem(row2, i);
                norm2 += SMI2->Value * SMI2->Value;
            }

            if (norm1 > 0.0f && norm2 > 0.0f){
                float cos = (float)(cov / (sqrt(norm1) * sqrt(norm2)));
                return cos;
            }else{
                return 0.0f;
            }
        }else{
            return 0.0f;
        }
    }
}
//---------------------------------------------------------------------------
int __fastcall func_SMIrowCmp(void * Item1, void * Item2)
{
    return ((SMatrixItem*)Item1)->Row - ((SMatrixItem*)Item2)->Row;
}
//---------------------------------------------------------------------------
int __fastcall func_SMIcolCmp(void * Item1, void * Item2)
{
    return ((SMatrixItem*)Item1)->Col - ((SMatrixItem*)Item2)->Col;
}
//---------------------------------------------------------------------------
void TSMatrix::Finalize(bool bAdd, float *pgpos, bool *terminated, float pgstep)
{
    bool term = false;
    if (terminated == NULL){
        terminated = &term;
    }
    //各ColをRow順ソートしてDuplexを見つける
    for (int i = 0 ; i < m_Cols->Count ; i++){
        TFListforTP *list = ColList(i);
        int listcount = list->Count;
        if (listcount){
            list->Sort(func_SMIrowCmp);
            SMatrixItem *last = (SMatrixItem *)list->Items[0];//&ColItem(i, 0);
            for (int i2 = 1 ; i2 < listcount ; ){
                SMatrixItem *smi = (SMatrixItem *)list->Items[i2];//&ColItem(i, i2);
                if (last->Row == smi->Row){
                    last->Flag = true;//削除するアイテムのフラグを立てる
                    if (bAdd){
                        //値の足し合わせ
                        smi->Value += last->Value;
                        last->Value = 0.0f;
                    }
                    list->Delete(i2 - 1);//ColListから削除
                    listcount--;
                }else{
                    i2++;
                    last = smi;
                }
            }
        }
    }
    if (pgpos){
        *pgpos += pgstep;
        Application->ProcessMessages();
    }

    if (!*terminated){
        //RowListから削除
        for (int i = 0 ; i < m_Rows->Count ; i++){
            TFListforTP *list = RowList(i);
            int listcount = list->Count;
            list->Sort(func_SMIcolCmp);
            for (int i2 = 0 ; i2 < listcount ; ){
                if (((SMatrixItem *)list->Items[i2])->Flag){//if (RowItem(i, i2).Flag){
                    list->Delete(i2);//RowListから削除
                    listcount--;
                }else{
                    i2++;
                }
            }
        }
        if (pgpos){
            *pgpos += pgstep;
            Application->ProcessMessages();
        }
    }

    //Itemsから削除
    if (!*terminated){
        int listcount = m_Items->Count;
        for (int i2 = 0 ; i2 < listcount ; ){
            SMatrixItem *item = &Item(i2);
            if (item->Flag){
                delete item;//Item自体削除
                m_Items->Delete(i2);//m_Itemから削除
                listcount--;
            }else{
                i2++;
            }
        }

        if (pgpos){
            *pgpos += pgstep;
            Application->ProcessMessages();
        }
    }
}
//---------------------------------------------------------------------------
int __fastcall func_valueCmp(void * Item1, void * Item2)
{
    return ((IDValue *)Item1)->Value - ((IDValue *)Item2)->Value;
}
//---------------------------------------------------------------------------
void TSMatrix::DeleteSameCol(bool bAdd)
{
    //同じところに値が入っているColを削除（ただしColのIndexは変わらない）

    //同じColを見つけるために、各ColのIndexを各Colの数でソート
    int docnum = m_Rows->Count;
    TFListforTP *IDVList = new TFListforTP();
    for (int i = 0 ; i < m_Cols->Count ; i++){
        IDValue *IDV = new IDValue;
        IDV->ID = i;
        int count = ColList(i)->Count;
        if (count){
            IDV->Value = count * docnum + ColItem(i, 0).Row;
        }else{
            IDV->Value = 0;
        }
        IDVList->Add(IDV);
    }
    IDVList->Sort(func_valueCmp);

    if (IDVList->Count){
        for (int il = 0 ; il < IDVList->Count ; il++){
            int value = ((IDValue*)IDVList->Items[il])->Value;
            int col1 = ((IDValue*)IDVList->Items[il])->ID;
            TFListforTP *list1 = ColList(col1);
            int count = list1->Count;
            for (int il2 = il + 1 ; il2 < IDVList->Count ; il2++){
                int value2 = ((IDValue*)IDVList->Items[il2])->Value;
                if (value2 == value){
                    //数が同じ
                    bool same = true;

                    //一致判定
                    int col2 = ((IDValue*)IDVList->Items[il2])->ID;
                    TFListforTP *list2 = ColList(col2);
                    for (int i = 0 ; i < count && same ; i++){
                        //same &= (ColItem(col1, i).Row == ColItem(col2, i).Row);
                        same &= ((SMatrixItem *)list1->Items[i])->Row == ((SMatrixItem *)list2->Items[i])->Row;
                    }
                    if (same){
                        //一致
                        for (int i = 0 ; i < count ; i++){
                            SMatrixItem * item1 = (SMatrixItem *)list1->Items[i];
                            if (bAdd){
                                ((SMatrixItem *)list2->Items[i])->Value += item1->Value;
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
                }else{
                    break;
                }
            }
        }
    }
    for (int i = 0 ; i < IDVList->Count ; i++){
        delete (IDValue*)IDVList->Items[i];
    }
    delete IDVList;

    //RowListから削除
    for (int i = 0 ; i < m_Rows->Count ; i++){
        TFListforTP *list = RowList(i);
        int listcount = list->Count;
        for (int i2 = 0 ; i2 < listcount ; ){
            if (((SMatrixItem *)list->Items[i2])->Flag){//if (RowItem(i, i2).Flag){
                list->Delete(i2);//RowListから削除
                listcount--;
            }else{
                i2++;
            }
        }
    }

    //Itemsから削除
    {
        int listcount = m_Items->Count;
        for (int i2 = 0 ; i2 < listcount ; ){
            SMatrixItem *item = &Item(i2);
            if (item->Flag){
                delete item;//Item自体削除
                m_Items->Delete(i2);//m_Itemから削除
                listcount--;
            }else{
                i2++;
            }
        }
    }
}
//---------------------------------------------------------------------------
typedef struct ColColValue_{
    int Col1, Col2;
    float Value;
} ColColValue;
//---------------------------------------------------------------------------
int __fastcall func_ivalueCmp(void * Item1, void * Item2)
{
    float f1 = ((IDValue *)Item1)->Value;
    float f2 = ((IDValue *)Item2)->Value;
    if (f1 > f2){
        return -1;
    }else if (f1 < f2){
        return 1;
    }else{
        return 0;
    }
}
//---------------------------------------------------------------------------
void TSMatrix::MergeCol(int block, float threshold, int target)
{
    int processed = 1;//処理されたかペア数
    int wcount;
    while (processed){
        //同じColを見つけるために、各ColのIndexを各Colの数でソート
        wcount = 0;//有効な単語数
        processed = 0;
        TFListforTP *IDVList = new TFListforTP();
        for (int i = 0 ; i < m_Cols->Count ; i++){
            IDValue *IDV = new IDValue;
            IDV->ID = i;
            IDV->Value = ColList(i)->Count;
            if (IDV->Value){
                wcount++;
            }
            IDVList->Add(IDV);
        }

        if (wcount > target){
            //所望の単語数に達していない

            IDVList->Sort(func_valueCmp);

            //blockずつループ
            for (int ib = 0 ; ib <= wcount / block ; ib++){
                int boffset = ib * block;
                int bcount = wcount - boffset;//このblockで処理する数
                if (bcount > block){
                    bcount = block;
                }
                if (ib > 0 && bcount < block / 2){
                    //blockが半数に満たないのでMergeしない
                    break;
                }

                //類似ペアリスト生成
                TFListforTP *SimList = new TFListforTP();
                float debug_maxcos = 0.0f;
                for (int i = 0 ; i < bcount ; i++){
                    int col1 = ((IDValue*)IDVList->Items[i + boffset])->ID;
                    for (int i2 = i + 1 ; i2 < bcount ; i2++){
                        int col2 = ((IDValue*)IDVList->Items[i2 + boffset])->ID;
                        float cos = CosCol(col1, col2);
                        if (cos > debug_maxcos){
                            debug_maxcos = cos;
                        }
                        if (cos > threshold){
                            //Thresholdを超えたので追加
                            //重複していない
                            ColColValue *CCV = new ColColValue;
                            CCV->Col1 = col1;
                            CCV->Col2 = col2;
                            CCV->Value = cos;
                            SimList->Add(CCV);
                        }
                    }
                }
                //類似度（降順）でソート
                SimList->Sort(func_ivalueCmp);

                //合成
                for (int i = 0 ; i < SimList->Count ; i++){
                    ColColValue *CCV = (ColColValue *)SimList->Items[i];
                    int lastindex = 0;
                    for (int ii = 0 ; ii < ColList(CCV->Col1)->Count ; ii++){
                        int cmp = 1;
                        while (lastindex < ColList(CCV->Col2)->Count){
                            cmp = ColItem(CCV->Col1, ii).Row - ColItem(CCV->Col2, lastindex).Row;
                            if (cmp > 0){
                                lastindex++;
                            }else{
                                break;
                            }
                        }
                        if (cmp == 0){
                            //同じRow
                            ColItem(CCV->Col2, lastindex).Value += ColItem(CCV->Col1, ii).Value;//値足しこみ
                            ColItem(CCV->Col1, ii).Flag = true;
                        }else{
                            //同じRowが無い

                            //ColをCol1からCol2に移動
                            ColItem(CCV->Col1, ii).Col = CCV->Col2;
                            ColList(CCV->Col2)->Insert(lastindex++, &ColItem(CCV->Col1, ii));
                        }
                    }
                    ColList(CCV->Col1)->Clear();

                    //重複チェック
                    for (int ix = i + 1 ; ix < SimList->Count ; ){
                        ColColValue *CCV2 = (ColColValue *)SimList->Items[ix];
                        if ((CCV->Col1 == CCV2->Col1) || (CCV->Col2 == CCV2->Col1) || (CCV->Col2 == CCV2->Col2) || (CCV->Col2 == CCV2->Col2)){
                            //重複削除
                            delete (ColColValue*)SimList->Items[ix];
                            SimList->Delete(ix);
                        }else{
                            ix++;
                        }
                    }
                }
                processed += SimList->Count;

                //類似ペアリスト破棄
                for (int i = 0 ; i < SimList->Count ; i++){
                    delete (ColColValue *)SimList->Items[i];
                }
                delete SimList;
            }
        }

        for (int i = 0 ; i < IDVList->Count ; i++){
            delete (IDValue*)IDVList->Items[i];
        }
        delete IDVList;
    }

    //RowListから削除
    for (int i = 0 ; i < m_Rows->Count ; i++){
        TFListforTP *list = RowList(i);
        int listcount = list->Count;
        for (int i2 = 0 ; i2 < listcount ; ){
            if (RowItem(i, i2).Flag){
                list->Delete(i2);//RowListから削除
                listcount--;
            }else{
                i2++;
            }
        }
    }

    //Itemsから削除
    {
        int listcount = m_Items->Count;
        for (int i2 = 0 ; i2 < listcount ; ){
            SMatrixItem *item = &Item(i2);
            if (item->Flag){
                delete item;//Item自体削除
                m_Items->Delete(i2);//m_Itemから削除
                listcount--;
            }else{
                i2++;
            }
        }
    }
}
/*
//---------------------------------------------------------------------------
TTextToFeature::TTextToFeature(TWideStringList *SL, int maxcombi, int maxcount)
{
    //単語抽出
    TTextDecomposer *TD = new TTextDecomposer(SL, maxcombi, maxcount);

    //Doc-単語出現確率格納用
    TSMatrix *SM = new TSMatrix();

    //TDの結果からSMを埋める
    //ドキュメントループ
    for (int id = 0 ; id < SL->Count ; id++){
        //文字ループ
        WideString doc = SL->Strings(id);
        int doclen = doc.Length();
        for (int ic = 0 ; ic < doclen ; ic++){
            int len = 1;
            int start = 0;
            int end = TD->m_Gram[0]->Count - 1;
            while (start >= 0){
                TWSandValueList *WSVL = TD->m_Gram[len - 1];
                WideString c = doc.SubString(ic + 1, len);
                //1文字単語からこの文字を検索
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

    //重複削除
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

#pragma package(smart_init)
