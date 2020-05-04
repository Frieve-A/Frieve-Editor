//---------------------------------------------------------------------------

#include <vcl.h>

#pragma hdrstop

#include <math.h>

#include "fomain_arrange.h"
#include "fomain.h"

//---------------------------------------------------------------------------
void TFo_Main::BrowserArrange_Initialize(float *CardX, float *CardY, bool *Norm)
{
    //CardX, CardY, Norm初期化

    //CardX, CardY, Norm初期化、最大最小座標取得
    float maxx = 0.5f, minx = 0.5f, maxy = 0.5f, miny = 0.5f;
    for (int i = 0 ; i < m_Document->m_Cards->Count ; i++){
        Norm[i] = false;
        TCard *Card = m_Document->GetCardByIndex(i);
        CardX[i] = Card->m_fX;
        CardY[i] = Card->m_fY;

        if (CardX[i] < minx){
            minx = CardX[i];
        }
        if (CardX[i] > maxx){
            maxx = CardX[i];
        }
        if (CardY[i] < miny){
            miny = CardY[i];
        }
        if (CardY[i] > maxy){
            maxy = CardY[i];
        }

        /*
        if (Me_Debug->Tag == 1){
            Me_Debug->Lines->Add(FormatFloat("0.00", CardX[i]) + "," + FormatFloat("0.00", CardY[i]));
        }
        */
    }

    if ((maxx == minx || maxy == miny) && m_Document->m_Cards->Count > 1){
        //座標が入っていないので、散らす
        for (int i = 0 ; i < m_Document->m_Cards->Count ; i++){
            TCard *Card = m_Document->GetCardByIndex(i);
            if (!Card->m_bFixed){
                Card->m_fX = CardX[i] = (rand() % 10000) / 10000.0f;
                Card->m_fY = CardY[i] = (rand() % 10000) / 10000.0f;
            }
        }
    }
}
//---------------------------------------------------------------------------
void TFo_Main::BrowserArrange_Link(int i, TCard *Card, float *CardX, float *CardY, bool *Norm, float ratio)
{
    //if (m_CardAssign[i] != i || Card->m_bFixed || (m_nTargetCard == Card->m_nID && m_bMDownBrowser)){
    if (m_CardAssign[i] != i || Card->m_bFixed || (Card->m_bSelected && m_bMDownBrowser)){
        //代表カードではないか、固定されている
        return;
    }

    ratio *= 0.3f;

    //Foldされている（代表カード以外からもリンクがあるかも）
    if (m_Document->LabelIsFold(Card)){
        //全ての同じラベルのカードからのリンクを考慮

        //リンクされているカードに近づく
        float count = 0;
        float X = 0.0f;
        float Y = 0.0f;
        bool *used = new bool[m_Document->m_Cards->Count];
        memset(used, 0, sizeof(bool) * m_Document->m_Cards->Count);
        for (int ic = 0 ; ic < m_Document->m_Cards->Count ; ic++) if (m_CardVisible[ic] && m_CardAssign[ic] == i){
            TCard *Card_ = m_Document->GetCardByIndex(ic);
            //リンクループ
            for (int i2 = 0 ; i2 < m_Document->m_Links->Count ; i2++) if (m_LinkVisible[i2]){
                TLink *Link = m_Document->GetLinkByIndex(i2);
                int card2index = -1;
                if (Link->m_nFromID == Card_->m_nID){
                    card2index = m_Document->SearchCardIndex(Link->m_nDestID);
				}else if (Link->m_nDestID == Card_->m_nID){
					card2index = m_Document->SearchCardIndex(Link->m_nFromID);
                }
                if (card2index >= 0 && card2index != i) if (m_CardVisible[card2index] && !used[card2index]){
                    used[card2index] = true;
                    //関連あるカードがある
                    /*
                    float bonus;
                    if (m_nTargetCard == Link->m_nDestID || m_nTargetCard == Link->m_nFromID || m_nTargetLink == i2){
                        //選択中のカード、リンクの影響力を上げる
                        bonus = 2.0f;
                    }else{
                        //通常のカード
                        bonus = 1.0f;
                    }
                    X += CardX[card2index] * bonus;
                    Y += CardY[card2index] * bonus;
                    count += bonus;
                    */
                    X += CardX[card2index];
                    Y += CardY[card2index];
                    count += 1.0f;
                }
            }
        }
        delete[] used;

        if (count > 0.0f){
            //座標更新
            Card->m_fX = (float)((X / count) * ratio + CardX[i] * (1.0f - ratio));
            Card->m_fY = (float)((Y / count) * ratio + CardY[i] * (1.0f - ratio));
            Norm[i] = true;
        }
    }else{
        //通常（1枚のカードからのリンクのみ考慮）

        //リンクされているカードに近づく
        float count = 0;
        float X = 0.0f;
        float Y = 0.0f;
        //リンクループ
        for (int i2 = 0 ; i2 < m_Document->m_Links->Count ; i2++) if (m_LinkVisible[i2]){
            TLink *Link = m_Document->GetLinkByIndex(i2);
            int card2index = -1;
            if (Link->m_nFromID == Card->m_nID){
                card2index = m_Document->SearchCardIndex(Link->m_nDestID);
            }else if (Link->m_nDestID == Card->m_nID){
                card2index = m_Document->SearchCardIndex(Link->m_nFromID);
            }
            if (card2index >= 0 && card2index != i) if (m_CardVisible[card2index]){
                //関連あるカードがある
                /*
                TCard *Card2 = m_Document->GetCardByIndex(card2index);
                if (Card2->m_bFixed){
                    float r = 2.0f;
                    X += CardX[i] + (CardX[card2index] - CardX[i]) * r;
                    Y += CardY[i] + (CardY[card2index] - CardY[i]) * r;
                    count += 1.0f;
                }else{
                    X += CardX[card2index];
                    Y += CardY[card2index];
                    count += 1.0f;
                }
                */
                X += CardX[card2index];
                Y += CardY[card2index];
                count += 1.0f;
            }
        }

        if (count > 0.0f){
            //座標更新
            Card->m_fX = (float)((X / count) * ratio + CardX[i] * (1.0f - ratio));
            Card->m_fY = (float)((Y / count) * ratio + CardY[i] * (1.0f - ratio));
            Norm[i] = true;
        }

        /*
        //通常（1枚のカードからのリンクのみ考慮）

        //リンクされているカードに近づく
        float count = 1;
        float X = Card->m_fX;
        float Y = Card->m_fY;
        //リンクループ
        for (int i2 = 0 ; i2 < m_Document->m_Links->Count ; i2++) if (m_LinkVisible[i2]){
            TLink *Link = m_Document->GetLinkByIndex(i2);
            int card2index = -1;
            if (Link->m_nFromID == Card->m_nID){
                card2index = m_Document->SearchCardIndex(Link->m_nDestID);
            }else if (Link->m_nDestID == Card->m_nID){
                card2index = m_Document->SearchCardIndex(Link->m_nFromID);
            }
            if (card2index >= 0 && card2index != i) if (m_CardVisible[card2index]){
                //関連あるカードがある
                X += CardX[card2index];
                Y += CardY[card2index];
                count += 1.0f;
            }
        }

        if (count > 1.0f){
            //座標更新
            Card->m_fX = (float)((X / count) * ratio + Card->m_fX * (1.0f - ratio));
            Card->m_fY = (float)((Y / count) * ratio + Card->m_fY * (1.0f - ratio));
            Norm[i] = true;
        }
        */
    }
}
//---------------------------------------------------------------------------
void TFo_Main::BrowserArrange_Repulsion(int i, TCard *Card, float *CardX, float *CardY, bool *Norm, float ratio)
{
    /*
    //他のカードと反発する
    float weight = 0.0f;
    float X = 0.0f;
    float Y = 0.0f;
    int count = 0;
    for (int i2 = 0 ; i2 < m_Document->m_Cards->Count ; i2++) if (i != i2) if (m_CardVisible[i2]){
        float w =
            (CardX[i] - CardX[i2]) * (CardX[i] - CardX[i2]) +
            (CardY[i] - CardY[i2]) * (CardY[i] - CardY[i2]);
        if (w > 0.0f){
            w = 1.0f / w;
            if (w > 1000.0f){
                w = 1000.0f;
            }
            float w2 = m_CardRepulsion[i] * m_CardRepulsion[i2];
            X += (CardX[i] - CardX[i2]) * w * w2;
            Y += (CardY[i] - CardY[i2]) * w * w2;
            weight += w2;
            count++;
        }else{
            if (!m_Document->LabelIsFold(Card)){
                //すべてはfoldされていない

                //座標が重なっている場合の反発
                X += (rand() % 101 - 50) * 0.0001f;
                Y += (rand() % 101 - 50) * 0.0001f;
                weight += 1.0f;
                count++;
            }
        }
    }
    //Card->m_fX += X / weight * ratio * 0.25f;
    //Card->m_fY += Y / weight * ratio;
    if (weight > 0.0f && count > 0){
        weight /= count * 100 / ratio;
        Card->m_fX += X * weight;
        Card->m_fY += Y * weight;
        Norm[i] = true;
    }
    //*/

    if (m_CardAssign[i] != i || Card->m_bFixed || (Card->m_bSelected && m_bMDownBrowser)){
    //if (m_CardAssign[i] != i || Card->m_bFixed || (m_nTargetCard == Card->m_nID && m_bMDownBrowser)){
        //代表カードではないか、固定されている
        return;
    }

    //*
    //他のカードと反発する
    float weight = 0.0f;
    float X = 0.0f;
    float Y = 0.0f;
    int count = 0;
    for (int i2 = 0 ; i2 < m_Document->m_Cards->Count ; i2++) if (i != i2) if (m_CardVisible[i2] && m_CardAssign[i2] == i2){
        float fx = CardX[i] - CardX[i2];
        float fy = CardY[i] - CardY[i2];
        float w = fx * fx + fy * fy;
        if (w > 0.0f){
            //w = sqrt(10000.0f / w);
            if (m_CardWidth[i2] * m_CardHeight[i2] > 0){
                w = sqrt((1.0 * m_CardWidth[i2] * m_CardHeight[i2]) / w);
            }else{
                w = sqrt(100 / w);
            }

            X += (CardX[i] - CardX[i2]) * w;
            Y += (CardY[i] - CardY[i2]) * w;
            weight += 1.0f;
            count++;
        }else{
            if (!m_Document->LabelIsFold(Card)){
                //すべてはfoldされていない

                //座標が重なっている場合の反発
                X += (rand() % 101 - 50) * 0.0001f;
                Y += (rand() % 101 - 50) * 0.0001f;
                weight += 1.0f;
                count++;
            }
        }
    }
    //Card->m_fX += X / weight * ratio * 0.25f;
    //Card->m_fY += Y / weight * ratio;
    if (weight > 0.0f && count > 0){
        weight /= count * count * 5 / ratio;
        Card->m_fX += (float)(X * weight);
        Card->m_fY += (float)(Y * weight);
        Norm[i] = true;
    }
    //*/
}
//---------------------------------------------------------------------------
void TFo_Main::BrowserArrange_LabelPrepare()
{
    //各ラベルの中心座標計算
    for (int il = 0 ; il < m_Document->m_Labels[0]->Count ; il++){
        TCardLabel *Label = m_Document->GetLabelByIndex(0, il);
        if (!Label->m_bFixed){
            Label->m_fX = 0.0f;
            Label->m_fY = 0.0f;
            int count = 0;
            for (int i = 0 ; i < m_Document->m_Cards->Count ; i++) if (m_CardVisible[i] && m_CardAssign[i] == i){
                TCard *Card = m_Document->GetCardByIndex(i);
                if (Card->m_Labels->Contain(il + 1)){
                    Label->m_fX += Card->m_fX;
                    Label->m_fY += Card->m_fY;
                    count++;
                }
            }

            if (count > 0){
                Label->m_fX /= count;
                Label->m_fY /= count;
            }else{
                Label->m_fX = 0.5f;
                Label->m_fY = 0.5f;
            }
        }
    }
}
//---------------------------------------------------------------------------
void TFo_Main::BrowserArrange_Label(int i, TCard *Card, float *CardX, float *CardY, bool *Norm, float ratio)
{
    if (m_CardAssign[i] != i || Card->m_bFixed || (Card->m_bSelected && m_bMDownBrowser)){
    //if (m_CardAssign[i] != i || Card->m_bFixed || (m_nTargetCard == Card->m_nID && m_bMDownBrowser)){
        //代表カードではないか、固定されている
        return;
    }

    //if (Card->m_Labels->Count == 0){
    if (m_Document->CountEnableLabel(Card) == 0){
        //ラベルがついていない
        return;
    }


    //float ratio2 = ratio * 0.3f;
    float ratio2 = ratio;
    //ratio *= 0.1f;

    int count = 1;
    float X = Card->m_fX;
    float Y = Card->m_fY;

    //ラベルで指定された座標に近づく
    for (int il = 0 ; il < Card->m_Labels->Count ; il++){
        int labelindex = Card->m_Labels->GetLabel(il);
        TCardLabel *Label = m_Document->GetLabelByIndex(0, labelindex - 1);
        if (Label->m_bEnable){
            X += Label->m_fX;
            Y += Label->m_fY;
            count++;
        }
    }

    /*
    //同じラベルを持っているカードに近づく
    for (int i2 = 0 ; i2 < m_Document->m_Cards->Count ; i2++) if (i != i2) if (m_CardVisible[i2] && m_CardAssign[i2] == i2){
        TCard *Card2 = m_Document->GetCardByIndex(i2);
        for (int il = 0 ; il < Card->m_Labels->Count ; il++){
            int labelindex = Card->m_Labels->GetLabel(il);
            TCardLabel *Label = m_Document->GetLabelByIndex(0, labelindex - 1);
            if (Label->m_bEnable){
                if (Card2->m_Labels->Contain(labelindex)){
                    //もう一つのカードにもラベルがある
                    X += CardX[i2];
                    Y += CardY[i2];
                    count ++;
                }
            }
        }
    }
    */
    if (count > 1){
        //座標更新
        Card->m_fX = (X / count) * ratio2 + Card->m_fX * (1.0f - ratio2);
        Card->m_fY = (Y / count) * ratio2 + Card->m_fY * (1.0f - ratio2);
        Norm[i] = true;
    }

    /*
    //ラベルを持っていないものと反発する
    int count2 = 0;
    X = 0.0f;
    Y = 0.0f;
    for (int i2 = 0 ; i2 < m_Document->m_Cards->Count ; i2++) if (i != i2) if (m_CardVisible[i2] && m_CardAssign[i2] == i2){
        TCard *Card2 = m_Document->GetCardByIndex(i2);
        for (int il = 0 ; il < Card->m_Labels->Count ; il++){
            bool donthave = true;
            for (int il2 = 0 ; il2 < Card2->m_Labels->Count ; il2++){
                if (Card->m_Labels->GetLabel(il) == Card2->m_Labels->GetLabel(il2)){
                    donthave = false;
                    break;
                }
            }

            if (donthave){
                float fx = CardX[i] - CardX[i2];
                float fy = CardY[i] - CardY[i2];
                float w = fx * fx + fy * fy;
                if (w > 0.0f){
                    w = sqrt(10000.0f / w);
                    X += (CardX[i] - CardX[i2]) * w;
                    Y += (CardY[i] - CardY[i2]) * w;
                    count2 ++;
                }
            }
        }
    }
    if (count2 > 0){
        Card->m_fX += X / m_Document->m_Cards->Count * ratio;
        Card->m_fY += Y / m_Document->m_Cards->Count * ratio;
        Norm[i] = true;
    }
    */
}
//---------------------------------------------------------------------------
void TFo_Main::BrowserArrange_Index(int i, TCard *Card, float *CardX, float *CardY, bool *Norm, float ratio)
{
    if (m_CardAssign[i] != i || Card->m_bFixed || (Card->m_bSelected && m_bMDownBrowser)){
    //if (m_CardAssign[i] != i || Card->m_bFixed || (m_nTargetCard == Card->m_nID && m_bMDownBrowser)){
        //代表カードではないか、固定されている
        return;
    }

    //カード順が近いものほど近くに

    float weight = 0.0f;
    float X = 0.0f;
    float Y = 0.0f;
    for (int i2 = 0 ; i2 < m_Document->m_Cards->Count ; i2++) if (i != i2) if (m_CardVisible[i2] && m_CardAssign[i2] == i2){
        float w = m_Document->m_Cards->Count - fabs(i - i2);
        w *= w;
        X += CardX[i2] * w;
        Y += CardY[i2] * w;
        weight += w;
    }
    if (weight > 0.0f){
        Card->m_fX = (X / weight) * ratio + Card->m_fX * (1.0f - ratio);
        Card->m_fY = (Y / weight) * ratio + Card->m_fY * (1.0f - ratio);
        /*
        Card->m_fX += X / m_Document->m_Cards->Count * ratio;
        Card->m_fY += Y / m_Document->m_Cards->Count * ratio;
        */
        Norm[i] = true;
    }
}
//---------------------------------------------------------------------------
void TFo_Main::BrowserArrange_Date(int i, TCard *Card, float *CardX, float *CardY, bool *Norm, float ratio, int dateindex)
{
    //日付が近いものほど近くに

    //（調整中）

    float weight = 0.0f;
    float X = 0.0f;
    float Y = 0.0f;
    for (int i2 = 0 ; i2 < m_Document->m_Cards->Count ; i2++) if (i != i2){
        TCard *Card2 = m_Document->GetCardByIndex(i2);
        float w;
        switch(dateindex){
        case 0:
            w = Card->m_fCreated - Card2->m_fCreated;
            break;
        case 1:
            w = Card->m_fUpdated - Card2->m_fUpdated;
            break;
        default:
            w = Card->m_fViewed - Card2->m_fViewed;
            break;
        }
        w *= w;

        if (w > 0.0f){
            w = 1.0f / w;
            X += (CardX[i] - CardX[i2]) * w;
            Y += (CardY[i] - CardY[i2]) * w;
            weight += w;
        }
    }
    if (weight > 0.0f){
        ratio /= 100;
        Card->m_fX += X / m_Document->m_Cards->Count * ratio * 0.25f;
        Card->m_fY += Y / m_Document->m_Cards->Count * ratio;
        Norm[i] = true;
    }
}
//---------------------------------------------------------------------------
void TFo_Main::BrowserArrange_Similarity(int i, TCard *Card, float *CardX, float *CardY, bool *Norm, float ratio, int *idxtable)
{
    if (m_CardAssign[i] != i || Card->m_bFixed || (Card->m_bSelected && m_bMDownBrowser)){
    //if (m_CardAssign[i] != i || Card->m_bFixed || (m_nTargetCard == Card->m_nID && m_bMDownBrowser)){
        //代表カードではないか、固定されている
        return;
    }

    int source = idxtable[i];//m_SimMatrix->SearchIndex(Card->m_nID);
    if (source < 0){
        return;
    }

    //類似カードほど近くに
    ratio *= 0.5f;

    float weight = 0.0f;
    float X = 0.0f;
    float Y = 0.0f;
    for (int i2 = 0 ; i2 < m_Document->m_Cards->Count ; i2++) if (i != i2) if (m_CardVisible[i2] && m_CardAssign[i2] == i2){
        //TCard *Card2 = m_Document->GetCardByIndex(i2);
        int target = idxtable[i2];//m_SimMatrix->SearchIndex(Card2->m_nID);
        if (target >= 0){
            float w = m_SimMatrix->S(source, target);
            if (w > 0.0f){
                X += CardX[i2] * w;
                Y += CardY[i2] * w;
                weight += w;
            }
        }
    }
    if (weight > 0.0f){
        Card->m_fX = (X / weight) * ratio + Card->m_fX * (1.0f - ratio);
        Card->m_fY = (Y / weight) * ratio + Card->m_fY * (1.0f - ratio);
        /*
        Card->m_fX += X / m_Document->m_Cards->Count * ratio;
        Card->m_fY += Y / m_Document->m_Cards->Count * ratio;
        */
        Norm[i] = true;
    }
}
//---------------------------------------------------------------------------
void TFo_Main::BrowserArrange_Memory(float *CardX, float *CardY, bool *Norm)
{
    for (int i = 0 ; i < m_Document->m_Cards->Count ; i++) if (Norm[m_CardAssign[i]]){
        TCard *Card = m_Document->GetCardByIndex(m_CardAssign[i]);
        CardX[i] = Card->m_fX;
        CardY[i] = Card->m_fY;
    }
}
//---------------------------------------------------------------------------
void TFo_Main::BrowserArrange_Normalize(float *CardX, float *CardY, bool *Norm)
{
    //画面内に収まるようにノーマライズ
    float maxx = 0.5f, minx = 0.5f, maxy = 0.5f, miny = 0.5f;
    for (int i = 0 ; i < m_Document->m_Cards->Count ; i++) if (Norm[m_CardAssign[i]]){
        TCard *Card = m_Document->GetCardByIndex(m_CardAssign[i]);
        CardX[i] = Card->m_fX;
        CardY[i] = Card->m_fY;

        if (CardX[i] < minx){
            minx = CardX[i];
        }
        if (CardX[i] > maxx){
            maxx = CardX[i];
        }
        if (CardY[i] < miny){
            miny = CardY[i];
        }
        if (CardY[i] > maxy){
            maxy = CardY[i];
        }
    }

    if (maxx != minx && maxy != miny){
        for (int i = 0 ; i < m_Document->m_Cards->Count ; i++) if (Norm[m_CardAssign[i]]){
            TCard *Card = m_Document->GetCardByIndex(i);
            CardX[i] = Card->m_fX = (CardX[i] - minx) / (maxx - minx);
            CardY[i] = Card->m_fY = (CardY[i] - miny) / (maxy - miny);
        }
    }
}
//---------------------------------------------------------------------------
void TFo_Main::BrowserArrangeByNormalize(float ratio)
{
    //反発力のみに従い座標更新

    float *CardX = new float[m_Document->m_Cards->Count];//X座標バックアップ
    float *CardY = new float[m_Document->m_Cards->Count];//Y座標バックアップ
    bool *Norm = new bool[m_Document->m_Cards->Count];//ノーマライズ対象
    memset(Norm, 0, sizeof(bool) * m_Document->m_Cards->Count);

    BrowserArrange_Initialize(CardX, CardY, Norm);

    //画面内に収まるようにノーマライズ
    for (int i = 0 ; i < m_Document->m_Cards->Count ; i++) if (m_CardVisible[i]){
        Norm[i] = true;
    }
    BrowserArrange_Normalize(CardX, CardY, Norm);

    delete[] Norm;
    delete[] CardX;
    delete[] CardY;
}
//---------------------------------------------------------------------------
void TFo_Main::BrowserArrangeByRepulsion(float ratio)
{
    //反発力のみに従い座標更新

    float *CardX = new float[m_Document->m_Cards->Count];//X座標バックアップ
    float *CardY = new float[m_Document->m_Cards->Count];//Y座標バックアップ
    bool *Norm = new bool[m_Document->m_Cards->Count];//ノーマライズ対象
    memset(Norm, 0, sizeof(bool) * m_Document->m_Cards->Count);

    BrowserArrange_Initialize(CardX, CardY, Norm);

    //座標更新

    //他のカードと反発する
    //カードループ
    for (int i = 0 ; i < m_Document->m_Cards->Count ; i++) if (m_CardVisible[i]){
        TCard *Card = m_Document->GetCardByIndex(i);

        //他のカードと反発する
        BrowserArrange_Repulsion(i, Card, CardX, CardY, Norm, ratio);
    }

    //画面内に収まるようにノーマライズ
    BrowserArrange_Normalize(CardX, CardY, Norm);

    delete[] Norm;
    delete[] CardX;
    delete[] CardY;
}
//---------------------------------------------------------------------------
void TFo_Main::BrowserArrangeByLink(float ratio)
{
    //リンクに従い座標更新

    float *CardX = new float[m_Document->m_Cards->Count];//X座標バックアップ
    float *CardY = new float[m_Document->m_Cards->Count];//Y座標バックアップ
    bool *Norm = new bool[m_Document->m_Cards->Count];//ノーマライズ対象
    memset(Norm, 0, sizeof(bool) * m_Document->m_Cards->Count);

    BrowserArrange_Initialize(CardX, CardY, Norm);

    //座標更新

    //他のカードと反発する
    //カードループ
    for (int i = 0 ; i < m_Document->m_Cards->Count ; i++) if (m_CardVisible[i]){
        TCard *Card = m_Document->GetCardByIndex(i);

        //他のカードと反発する
        BrowserArrange_Repulsion(i, Card, CardX, CardY, Norm, 0.5f);
    }

    //一旦座標更新
    BrowserArrange_Normalize(CardX, CardY, Norm);

    //リンクされているカードに近づく
    //カードループ
    for (int i = 0 ; i < m_Document->m_Cards->Count ; i++) if (m_CardVisible[i]){
        TCard *Card = m_Document->GetCardByIndex(i);

        //リンクされているカードに近づく
        BrowserArrange_Link(i, Card, CardX, CardY, Norm, ratio * 0.66);
    }

    //画面内に収まるようにノーマライズ
    BrowserArrange_Normalize(CardX, CardY, Norm);

    delete[] Norm;
    delete[] CardX;
    delete[] CardY;
}
//---------------------------------------------------------------------------
void TFo_Main::BrowserArrangeByLabel(float ratio)
{
    //ラベルに従い座標更新

    float *CardX = new float[m_Document->m_Cards->Count];//X座標バックアップ
    float *CardY = new float[m_Document->m_Cards->Count];//Y座標バックアップ
    bool *Norm = new bool[m_Document->m_Cards->Count];//ノーマライズ対象
    memset(Norm, 0, sizeof(bool) * m_Document->m_Cards->Count);

    BrowserArrange_Initialize(CardX, CardY, Norm);

    //座標更新

    //カードループ
    //他のカードと反発する
    for (int i = 0 ; i < m_Document->m_Cards->Count ; i++) if (m_CardVisible[i]){
        TCard *Card = m_Document->GetCardByIndex(i);

        //他のカードと反発する
        BrowserArrange_Repulsion(i, Card, CardX, CardY, Norm, 0.5f);
    }

    //一旦座標更新
    BrowserArrange_Normalize(CardX, CardY, Norm);

    //リンクされているカードに近づく
    //カードループ
    for (int i = 0 ; i < m_Document->m_Cards->Count ; i++) if (m_CardVisible[i]){
        TCard *Card = m_Document->GetCardByIndex(i);

        //リンクされているカードに近づく
        BrowserArrange_Link(i, Card, CardX, CardY, Norm, ratio / 4);
    }

    //一旦座標更新
    BrowserArrange_Normalize(CardX, CardY, Norm);

    //同じラベルは近く、違うラベルは遠く
    BrowserArrange_LabelPrepare();
    //カードループ
    for (int i = 0 ; i < m_Document->m_Cards->Count ; i++) if (m_CardVisible[i]){
        TCard *Card = m_Document->GetCardByIndex(i);

        //同じラベルは近く、違うラベルは遠く
        BrowserArrange_Label(i, Card, CardX, CardY, Norm, ratio / 3);
    }

    //画面内に収まるようにノーマライズ
    BrowserArrange_Normalize(CardX, CardY, Norm);

    delete[] Norm;
    delete[] CardX;
    delete[] CardY;
}
//---------------------------------------------------------------------------
void TFo_Main::BrowserArrangeByIndex(float ratio)
{
    //カード順に従い座標更新

    float *CardX = new float[m_Document->m_Cards->Count];//X座標バックアップ
    float *CardY = new float[m_Document->m_Cards->Count];//Y座標バックアップ
    bool *Norm = new bool[m_Document->m_Cards->Count];//ノーマライズ対象
    memset(Norm, 0, sizeof(bool) * m_Document->m_Cards->Count);

    BrowserArrange_Initialize(CardX, CardY, Norm);

    //座標更新

    //座標更新

    //カードループ
    //他のカードと反発する
    for (int i = 0 ; i < m_Document->m_Cards->Count ; i++) if (m_CardVisible[i]){
        TCard *Card = m_Document->GetCardByIndex(i);

        //他のカードと反発する
        BrowserArrange_Repulsion(i, Card, CardX, CardY, Norm, 0.5f);
    }

    //一旦座標更新
    BrowserArrange_Normalize(CardX, CardY, Norm);


    //カードループ
    for (int i = 0 ; i < m_Document->m_Cards->Count ; i++) if (m_CardVisible[i]){
        TCard *Card = m_Document->GetCardByIndex(i);

        //カード順が近いものほど近く
        BrowserArrange_Index(i, Card, CardX, CardY, Norm, ratio * 0.61f);
    }

    //画面内に収まるようにノーマライズ
    BrowserArrange_Normalize(CardX, CardY, Norm);

    delete[] Norm;
    delete[] CardX;
    delete[] CardY;
}
//---------------------------------------------------------------------------
void TFo_Main::BrowserArrangeByDate(int dateindex, float ratio)
{
    //日付に従い座標更新

    float *CardX = new float[m_Document->m_Cards->Count];//X座標バックアップ
    float *CardY = new float[m_Document->m_Cards->Count];//Y座標バックアップ
    bool *Norm = new bool[m_Document->m_Cards->Count];//ノーマライズ対象
    memset(Norm, 0, sizeof(bool) * m_Document->m_Cards->Count);

    BrowserArrange_Initialize(CardX, CardY, Norm);

    //座標更新

    //カードループ
    for (int i = 0 ; i < m_Document->m_Cards->Count ; i++){
        TCard *Card = m_Document->GetCardByIndex(i);

        if (Card->m_nID != m_nTargetCard){
            //動かせるカード

            //他のカードと反発する
            BrowserArrange_Repulsion(i, Card, CardX, CardY, Norm, ratio / 3);

            //リンクされているカードに近づく
            BrowserArrange_Link(i, Card, CardX, CardY, Norm, ratio / 3);

            //日付が近いものほど近く
            BrowserArrange_Date(i, Card, CardX, CardY, Norm, ratio / 3, dateindex);
        }
    }

    //画面内に収まるようにノーマライズ
    BrowserArrange_Normalize(CardX, CardY, Norm);

    delete[] Norm;
    delete[] CardX;
    delete[] CardY;
}
//---------------------------------------------------------------------------
void TFo_Main::BrowserArrangeByFold(float ratio)
{
    //折りたたまれたラベルに従い座標更新
    //通常のArrangeではなく、ラベルのFoldが変更されたときに呼ばれる

    float *CardX = new float[m_Document->m_Cards->Count];//X座標バックアップ
    float *CardY = new float[m_Document->m_Cards->Count];//Y座標バックアップ
    bool *Norm = new bool[m_Document->m_Cards->Count];//ノーマライズ対象
    memset(Norm, 0, sizeof(bool) * m_Document->m_Cards->Count);
    float *Sum = new float[m_Document->m_Cards->Count];//Y座標バックアップ
    memset(Sum, 0, sizeof(float) * m_Document->m_Cards->Count);
    float *LabelX = new float[m_Document->m_Labels[0]->Count];//X座標
    memset(LabelX, 0, sizeof(float) * m_Document->m_Labels[0]->Count);
    float *LabelY = new float[m_Document->m_Labels[0]->Count];//Y座標
    memset(LabelY, 0, sizeof(float) * m_Document->m_Labels[0]->Count);
    float *LabelSum = new float[m_Document->m_Labels[0]->Count];//Y座標バックアップ
    memset(LabelSum, 0, sizeof(float) * m_Document->m_Labels[0]->Count);

    BrowserArrange_Initialize(CardX, CardY, Norm);

    //座標更新

    //ラベルの中心座標計算
    //ラベルループ
    for (int il = 0 ; il < m_Document->m_Labels[0]->Count ; il++){
        TCardLabel *Label = m_Document->GetLabelByIndex(0, il);
        if (Label->m_bEnable && Label->m_bFold){
            //折りたたまれたラベル

            //カードループ
            for (int i = 0 ; i < m_Document->m_Cards->Count ; i++) if (m_CardVisible[i]){
                TCard *Card = m_Document->GetCardByIndex(i);

                if (Card->m_Labels->Contain(il + 1)){
                    //このラベルを含む
                    LabelX[il] += CardX[i] / Card->m_Labels->Count;
                    LabelY[il] += CardY[i] / Card->m_Labels->Count;
                    LabelSum[il] += 1.0f / Card->m_Labels->Count;
                }
            }
            if (LabelSum[il] > 0.0f){
                LabelX[il] /= LabelSum[il];
                LabelY[il] /= LabelSum[il];
            }
        }
    }

    //カード座標をリセット
    for (int i = 0 ; i < m_Document->m_Cards->Count ; i++) if (m_CardVisible[i]){
        CardX[i] = 0.0f;
        CardY[i] = 0.0f;
    }

    //ラベルの中心座標にカードを移動
    //ラベルループ
    for (int il = 0 ; il < m_Document->m_Labels[0]->Count ; il++){
        TCardLabel *Label = m_Document->GetLabelByIndex(0, il);
        if (Label->m_bEnable && Label->m_bFold){
            //折りたたまれたラベル

            //カードループ
            for (int i = 0 ; i < m_Document->m_Cards->Count ; i++) if (m_CardVisible[i]){
                TCard *Card = m_Document->GetCardByIndex(i);

                if (Card->m_Labels->Contain(il + 1)){
                    //このラベルを含む

                    if (m_Document->LabelIsFold(Card)){
                        //すべてfoldされている
                        CardX[i] += LabelX[il] / Card->m_Labels->Count;
                        CardY[i] += LabelY[il] / Card->m_Labels->Count;
                        Sum[i] += 1.0f / Card->m_Labels->Count;
                    }
                }
            }
        }
    }

    //カードループ
    for (int i = 0 ; i < m_Document->m_Cards->Count ; i++) if (m_CardVisible[i]){
        if (Sum[i] > 0.0f){
            TCard *Card = m_Document->GetCardByIndex(i);
            Card->m_fX = CardX[i] / Sum[i];
            Card->m_fY = CardY[i] / Sum[i];
        }
    }
    delete[] LabelSum;
    delete[] LabelY;
    delete[] LabelX;
    delete[] Sum;
    delete[] Norm;
    delete[] CardX;
    delete[] CardY;
}
//---------------------------------------------------------------------------
void TFo_Main::BrowserArrangeBySimilarity(float ratio)
{
    float *CardX = new float[m_Document->m_Cards->Count];//X座標バックアップ
    float *CardY = new float[m_Document->m_Cards->Count];//Y座標バックアップ
    bool *Norm = new bool[m_Document->m_Cards->Count];//ノーマライズ対象
    memset(Norm, 0, sizeof(bool) * m_Document->m_Cards->Count);

    BrowserArrange_Initialize(CardX, CardY, Norm);

    //座標更新

    //他のカードと反発する
    //カードループ
    for (int i = 0 ; i < m_Document->m_Cards->Count ; i++) if (m_CardVisible[i]){
        TCard *Card = m_Document->GetCardByIndex(i);

        //他のカードと反発する
        BrowserArrange_Repulsion(i, Card, CardX, CardY, Norm, 0.5f);
    }

    //一旦座標更新
    BrowserArrange_Normalize(CardX, CardY, Norm);

    //類似カードに近づく
    if (m_SimMatrix != NULL){
        //類似度がある

        int *idxtable = new int[m_Document->m_Cards->Count];
        for (int i = 0 ; i < m_Document->m_Cards->Count ; i++) if (m_CardVisible[i]){
            TCard *Card = m_Document->GetCardByIndex(i);
            idxtable[i] = m_SimMatrix->SearchIndex(Card->m_nID);
        }
        //カードループ
        for (int i = 0 ; i < m_Document->m_Cards->Count ; i++) if (m_CardVisible[i]){
            TCard *Card = m_Document->GetCardByIndex(i);

            //類似カードに近づく
            BrowserArrange_Similarity(i, Card, CardX, CardY, Norm, ratio * 0.8, idxtable);
        }
        delete[] idxtable;
    }

    //画面内に収まるようにノーマライズ
    BrowserArrange_Normalize(CardX, CardY, Norm);

    delete[] Norm;
    delete[] CardX;
    delete[] CardY;
}
//---------------------------------------------------------------------------
void TFo_Main::PrepareMatrixArrange_AssignToMatrix2(int i, int *tmatrix, float minx, float maxx, float miny, float maxy)
{
    //i番目のカードを、tmatrixで示すマトリクスに格納

    TCard *Card = m_Document->GetCardByIndex(i);
    float normx = (Card->m_fX - minx) / (maxx - minx);
    float normy = (Card->m_fY - miny) / (maxy - miny);

    //0.5を中心の円→0.5中心のRectの座標に変換
    {
        float x1 = normx - 0.5f;
        float y1 = normy - 0.5f;
        float rad = fmod(atan2(y1, x1) + 2 * M_PI, M_PI / 2);//0~PI/2
        if (rad > M_PI / 4){
            rad = M_PI / 2 - rad;
        }//0~PI/4
        float ncoef = 1.0 / cos(rad);
        normx = x1 * ncoef + 0.5f;
        normy = y1 * ncoef + 0.5f;
    }


    //空いている格子のうち、一番近い格子を探す
    int x = -1, y = -1;
    float mindistance = m_nMatrixWidth + m_nMatrixHeight;
    //格子ループ
    for (int iy = 0 ; iy < m_nMatrixHeight ; iy++){
        for (int ix = 0 ; ix < m_nMatrixWidth ; ix++){
            if (tmatrix[ix + iy * m_nMatrixWidth] == -1){
                //空いている
                float distance = fabs(normx * (m_nMatrixWidth - 1) - ix) + fabs(normy * (m_nMatrixHeight - 1) - iy);
                if (distance < mindistance){
                    //より近い格子
                    mindistance = distance;
                    x = ix;
                    y = iy;
                }
            }
        }
    }

    //格子に格納
    if (x >= 0 && y >= 0){
        tmatrix[x + y * m_nMatrixWidth] = i;
        if (Card->m_nMatrixX != x || Card->m_nMatrixY != y){
            Card->m_nMatrixX = x;
            Card->m_nMatrixY = y;
            Card->m_fMatrixSpeed = 0.0f;
        }
    }
}
//---------------------------------------------------------------------------
void TFo_Main::PrepareMatrixArrange_AssignToMatrix(int i, int *tmatrix, float minx, float maxx, float miny, float maxy)
{
    //i番目のカードを、tmatrixで示すマトリクスに格納

    TCard *Card = m_Document->GetCardByIndex(i);
    float normx = (Card->m_fX - minx) / (maxx - minx);
    float normy = (Card->m_fY - miny) / (maxy - miny);

    //空いている格子のうち、一番近い格子を探す
    int x = -1, y = -1;
    float mindistance = m_nMatrixWidth + m_nMatrixHeight;
    //格子ループ
    for (int iy = 0 ; iy < m_nMatrixHeight ; iy++){
        for (int ix = 0 ; ix < m_nMatrixWidth ; ix++){
            if (tmatrix[ix + iy * m_nMatrixWidth] == -1){
                //空いている
                float distance = fabs(normx * (m_nMatrixWidth - 1) - ix) + fabs(normy * (m_nMatrixHeight - 1) - iy);
                if (distance < mindistance){
                    //より近い格子
                    mindistance = distance;
                    x = ix;
                    y = iy;
                }
            }
        }
    }

    //格子に格納
    if (x >= 0 && y >= 0){
        tmatrix[x + y * m_nMatrixWidth] = i;
        if (Card->m_nMatrixX != x || Card->m_nMatrixY != y){
            Card->m_nMatrixX = x;
            Card->m_nMatrixY = y;
            Card->m_fMatrixSpeed = 0.0f;
        }
    }
}
//---------------------------------------------------------------------------
void TFo_Main::PrepareMatrixArrange(int type)
{
    //格子サイズを決め、全カードを格子に配置。Arrangeを選択した瞬間実行
    if (!m_CardVisible){
        return;
    }

    /*
    float *CardX, *CardY;
    bool *Norm;
    if (type % 100 != 0){
        //カード位置をバックアップ
        CardX = new float[m_Document->m_Cards->Count];
        CardY = new float[m_Document->m_Cards->Count];
        Norm = new bool[m_Document->m_Cards->Count];//ノーマライズ対象
        memset(Norm, 0, sizeof(bool) * m_Document->m_Cards->Count);

        BrowserArrange_Initialize(CardX, CardY, Norm);

        //座標更新
        switch(type){
        case 202:
            {
                float *CardX2 = new float[m_Document->m_Cards->Count];
                float *CardY2 = new float[m_Document->m_Cards->Count];

                memcpy(CardX2, CardX, sizeof(float) * m_Document->m_Cards->Count);
                memcpy(CardY2, CardY, sizeof(float) * m_Document->m_Cards->Count);

                for (int il = 0 ; il < 5 ; il++){
                    //リンクされているカードに近づく
                    //カードループ
                    for (int i = 0 ; i < m_Document->m_Cards->Count ; i++) if (m_CardVisible[i]){
                        TCard *Card = m_Document->GetCardByIndex(i);

                        //リンクされているカードに近づく
                        BrowserArrange_Link(i, Card, CardX2, CardY2, Norm, 0.5f);
                    }
                    BrowserArrange_Normalize(CardX2, CardY2, Norm);
                }

                delete[] CardY2;
                delete[] CardX2;
            }
            break;
        case 203:
            {
                //同じラベルのカードに近づく
                BrowserArrange_LabelPrepare();
                //カードループ
                for (int i = 0 ; i < m_Document->m_Cards->Count ; i++) if (m_CardVisible[i]){
                    TCard *Card = m_Document->GetCardByIndex(i);

                    //同じラベルのカードに近づく
                    BrowserArrange_Label(i, Card, CardX, CardY, Norm, 1.0f);
                }
            }
            break;
        case 700:
            {
                //類似カードに近づく
                int *idxtable = new int[m_Document->m_Cards->Count];
                for (int i = 0 ; i < m_Document->m_Cards->Count ; i++) if (m_CardVisible[i]){
                    TCard *Card = m_Document->GetCardByIndex(i);
                    idxtable[i] = m_SimMatrix->SearchIndex(Card->m_nID);
                }
                //カードループ
                for (int i = 0 ; i < m_Document->m_Cards->Count ; i++) if (m_CardVisible[i]){
                    TCard *Card = m_Document->GetCardByIndex(i);

                    //類似カードに近づく
                    BrowserArrange_Similarity(i, Card, CardX, CardY, Norm, 1.0f, idxtable);
                }
                delete[] idxtable;
            }
            break;
        }
    }
    */

    //表示するカードの数を決める
    int count = 0;
    float minx, maxx, miny, maxy;
    for (int i = 0 ; i < m_Document->m_Cards->Count ; i++) if (m_CardAssign[i] == i){
        TCard *Card = m_Document->GetCardByIndex(i);
        if (Card->m_bVisible){
            if (count == 0 || minx > Card->m_fX){
                minx = Card->m_fX;
            }
            if (count == 0 || maxx < Card->m_fX){
                maxx = Card->m_fX;
            }
            if (count == 0 || miny > Card->m_fY){
                miny = Card->m_fY;
            }
            if (count == 0 || maxy < Card->m_fY){
                maxy = Card->m_fY;
            }
            count++;
        }
    }
    if (minx == maxx){
        maxx += 1.0f;
    }
    if (miny == maxy){
        maxy += 1.0f;
    }

    //格子の縦横比を決める
    float xyratio = 1.0f;//大きくなるほど横に広がる
    float widthsum = 0.0f;
    float heightsum = 0.0f;
    for (int i = 0 ; i < m_Document->m_Cards->Count ; i++) if (m_CardVisible[i] && m_CardAssign[i] == i){
        widthsum += m_CardWidth[i];
        heightsum += m_CardHeight[i];
    }
    if (heightsum > 0 && widthsum > 0){
        widthsum /= m_nBrowserWidth;
        heightsum /= m_nBrowserHeight;
        xyratio = sqrt(heightsum * 1.0f / widthsum);
    }

    //格子のサイズを決める
    m_nMatrixWidth = 1;
    m_nMatrixHeight = 1;
    while (count > m_nMatrixWidth * m_nMatrixHeight){
        if (m_nMatrixWidth <= m_nMatrixHeight * xyratio){
            m_nMatrixWidth += 2;
        }else{
            m_nMatrixHeight += 2;
        }
    }

    if (type != 204){
        //近接Cellに配置

        //一時Matrixを作り、初期化
        int *tmatrix = new int[m_nMatrixWidth * m_nMatrixHeight];
        for (int i = 0 ; i < m_nMatrixWidth * m_nMatrixHeight ; i++){
            tmatrix[i] = -1;
        }

        //カードをMatrixに配置（Fixedカード→選択カード→関連カード→その他のカード）
        int *draworder = new int[m_Document->m_Cards->Count];
        memset(draworder, 0, sizeof(int) * m_Document->m_Cards->Count);
        for (int i = 0 ; i < m_Document->m_Cards->Count ; i++){
            int vo = m_Document->GetCardByIndex(i)->m_nViewedOrder;
            draworder[vo] = i;
        }
        int targetcardindex = m_Document->SearchCardIndex(m_nTargetCard);
        //Fixedカード
        for (int ic = m_Document->m_Cards->Count - 1 ; ic >= 0 ; ic--){
            int i = draworder[ic];
            TCard *Card = m_Document->GetCardByIndex(i);
            if (m_CardVisible[i] && m_CardAssign[i] == i && Card->m_bFixed && i != targetcardindex){
                if (type % 100 == 0){
                    PrepareMatrixArrange_AssignToMatrix(i, tmatrix, minx, maxx, miny, maxy);
                }else{
                    PrepareMatrixArrange_AssignToMatrix2(i, tmatrix, minx, maxx, miny, maxy);
                }
            }
        }
        //選択中のカードを配置
        if (m_nTargetCard >= 0){
            int i = targetcardindex;
            if (i >= 0) if (m_CardVisible[i] && m_CardAssign[i] == i){
                if (type % 100 == 0){
                    PrepareMatrixArrange_AssignToMatrix(i, tmatrix, minx, maxx, miny, maxy);
                }else{
                    PrepareMatrixArrange_AssignToMatrix2(i, tmatrix, minx, maxx, miny, maxy);
                }
            }
        }
        //関連カード
        for (int i = m_Document->m_Cards->Count - 1 ; i >= 0 ; i--){
            TCard *Card = m_Document->GetCardByIndex(i);
            if (m_CardVisible[i] && m_CardRelated[i] && m_CardAssign[i] == i && i != targetcardindex && !Card->m_bFixed){
                if (type % 100 == 0){
                    PrepareMatrixArrange_AssignToMatrix(i, tmatrix, minx, maxx, miny, maxy);
                }else{
                    PrepareMatrixArrange_AssignToMatrix2(i, tmatrix, minx, maxx, miny, maxy);
                }
            }
        }
        //その他のカード
        for (int ic = m_Document->m_Cards->Count - 1 ; ic >= 0 ; ic--){
            int i = draworder[ic];
            TCard *Card = m_Document->GetCardByIndex(i);
            if (m_CardVisible[i] && !m_CardRelated[i] && m_CardAssign[i] == i && i != targetcardindex && !Card->m_bFixed){
                if (type % 100 == 0){
                    PrepareMatrixArrange_AssignToMatrix(i, tmatrix, minx, maxx, miny, maxy);
                }else{
                    PrepareMatrixArrange_AssignToMatrix2(i, tmatrix, minx, maxx, miny, maxy);
                }
            }
        }
        delete[] draworder;

        delete[] tmatrix;
    }else{
        //Index順に整然と配置
        int index = 0;
        for (int i = 0 ; i < m_Document->m_Cards->Count ; i++){
            TCard *Card = m_Document->GetCardByIndex(i);
            if (m_CardVisible[i] && m_CardAssign[i] == i && !Card->m_bFixed){
                Card->m_nMatrixX = index / m_nMatrixHeight;
                Card->m_nMatrixY = index % m_nMatrixHeight;
                index++;
            }
        }
    }

    /*
    if (type % 100 != 0){
        //座標を戻す
        for (int i = 0 ; i < m_Document->m_Cards->Count ; i++) if (Norm[i]){
            TCard *Card = m_Document->GetCardByIndex(i);
            Card->m_fX = CardX[i];
            Card->m_fY = CardY[i];
        }
        delete[] CardX;
        delete[] CardY;
    }
    */
}
//---------------------------------------------------------------------------
void TFo_Main::BrowserArrangeByMatrix(int type, float ratio)
{
    switch (type){
    case 200:
        PrepareMatrixArrange(type);
        break;
    case 202:
        //Link
        PrepareMatrixArrange(type);
        break;
    case 203:
        //Label
        break;
    case 204:
        PrepareMatrixArrange(type);
        break;
    case 700:
        //Similarity
        break;
    }

    //格子状に配置
    for (int i = 0 ; i < m_Document->m_Cards->Count ; i++) if (m_CardVisible[i]){
        TCard *Card = m_Document->GetCardByIndex(i);

        //if (m_CardAssign[i] != i || Card->m_bFixed || (Card->m_bSelected && m_bMDownBrowser)){
        //if (!Card->m_bFixed && (m_nTargetCard != Card->m_nID || !m_bMDownBrowser)){
        if (!Card->m_bFixed && (!Card->m_bSelected || !m_bMDownBrowser)){
            Card->m_fMatrixSpeed += 0.1f;
            if (Card->m_fMatrixSpeed > 0.5f){
                Card->m_fMatrixSpeed = 0.5f;
            }

            float targetx = 0.5f;
            if (m_nMatrixWidth > 1){
                targetx = Card->m_nMatrixX * 1.0f / (m_nMatrixWidth - 1);
            }
            float targety = 0.5f;
            if (m_nMatrixHeight > 1){
                targety = Card->m_nMatrixY * 1.0f / (m_nMatrixHeight - 1);
            }

            Card->m_fX = Card->m_fX * (1.0f - Card->m_fMatrixSpeed) + targetx * Card->m_fMatrixSpeed;
            Card->m_fY = Card->m_fY * (1.0f - Card->m_fMatrixSpeed) + targety * Card->m_fMatrixSpeed;
        }
    }

    for (int i = 0 ; i < m_Document->m_Cards->Count ; i++) if (m_CardAssign[i] != i){
        TCard *Card = m_Document->GetCardByIndex(m_CardAssign[i]);
        TCard *Card2 = m_Document->GetCardByIndex(i);

        Card2->m_fX = Card->m_fX;
        Card2->m_fY = Card->m_fY;
    }
}
//---------------------------------------------------------------------------
void TFo_Main::BrowserArrangeByTree(int type, float ratio)
{
    //階層表示

    //位置固定用座標バックアップ
    float *xbak = new float[m_Document->m_Cards->Count];
    float *ybak = new float[m_Document->m_Cards->Count];
    for (int i = 0 ; i < m_Document->m_Cards->Count ; i++) if (m_CardVisible[i]){
        TCard *Card = m_Document->GetCardByIndex(i);
        xbak[i] = Card->m_fX;
        ybak[i] = Card->m_fY;
    }

    int *nodegroup = new int[m_Document->m_Cards->Count];

    //表示中のカードカウント
    int vcount = 1;
    for (int i = 0 ; i < m_Document->m_Cards->Count ; i++) if (m_CardVisible[i]){
        vcount++;
    }
    float xspan = 1.0f / sqrt(vcount);
    float yspan = 1.0f / sqrt(vcount);

    ratio *= 0.25f;


    //各表示中のルートノード毎にTreeを作る

    int *nodeheight = new int[m_Document->m_Cards->Count];
    memset(nodeheight, 0, sizeof(int) * m_Document->m_Cards->Count);

    //カードループ（ルート）
    int group = 0;
    for (int i = 0 ; i < m_Document->m_Cards->Count ; i++) if (m_CardVisible[i]){
        TCard *Root = m_Document->GetCardByIndex(i);
        if (Root->m_nLevel == 0){
            //ルートノード
            nodegroup[i] = group;

            //この木の最大レベルを得る
            int maxlevel = 0;
            int endindex = i + 1;//この木の終了Indexを同時に取得
            while (endindex < m_Document->m_Cards->Count){
                TCard *Card = m_Document->GetCardByIndex(endindex);
                if (Card->m_nLevel == 0){
                    //もう次のTree
                    break;
                }else if (Card->m_nLevel > maxlevel){
                    maxlevel = Card->m_nLevel;
                }
                nodegroup[endindex] = group;
                endindex++;
            }

            //下位ノードのサイズを得る
            //レベルループ
            for (int il = maxlevel ; il >= 0 ; il--){
                //この木のノードループ
                for (int in = i ; in < endindex ; in++) if (m_CardVisible[in]){
                    TCard *Card = m_Document->GetCardByIndex(in);
                    if (Card->m_nLevel == il){
                        //このカードのサイズを下位ノードの数の合計とする
                        int index = in + 1;
                        if (in == i){
                            in = i;
                        }
                        while (index < m_Document->m_Cards->Count){
                            TCard *Card = m_Document->GetCardByIndex(index);
                            if (Card->m_nLevel < il + 1){
                                //もう次のノード
                                break;
                            }else if (Card->m_nLevel == il + 1){
                                //下位ノードのサイズを足す
                                nodeheight[in] += nodeheight[index];
                            }
                            index++;
                        }
                        if (nodeheight[in] == 0){
                            //最小でもサイズ1とする
                            nodeheight[in] = 1;
                        }
                    }
                }
            }

            //ルートノードを中心に、座標を決めていく
            //レベルループ
            for (int il = 1 ; il <= maxlevel ; il++){
                int heightsum = 0;//このレベルの処理済サイズ合計

                //この木のノードループ
                TCard *LastRoot = NULL;//直前のルートノード
                int lastrootindex = -1;
                int rightcount = 0;
                int rightheight = 0;
                int rightheightsum = 0;
                //この木のノードループ
                for (int in = i ; in < endindex ; in++) if (m_CardVisible[in]){
                    TCard *Card = m_Document->GetCardByIndex(in);
                    if (Card->m_nLevel == il - 1){
                        LastRoot = Card;
                        lastrootindex = in;
                        heightsum = 0;
                        if (il == 1){
                            //左右に分ける
                            int sum = 0;
                            for (int in2 = i ; in2 < endindex ; in2++) if (m_CardVisible[in2]){
                                TCard *Card2 = m_Document->GetCardByIndex(in2);
                                if (Card2->m_nLevel == il){
                                    if (abs((sum + nodeheight[in2]) * 2 - nodeheight[lastrootindex]) <= abs(sum * 2 - nodeheight[lastrootindex])){
                                        rightcount++;
                                        sum += nodeheight[in2];
                                    }else{
                                        break;
                                    }
                                }
                            }
                            rightheight = sum;
                        }
                    }else if (Card->m_nLevel == il && LastRoot){
                        if (LastRoot == Root){
                            //ルートからの分岐
                            if (rightcount-- > 0){
                                //右側に表示
                                Card->m_fX = LastRoot->m_fX + xspan;
                                Card->m_fY = LastRoot->m_fY +
                                    (-rightheight * 0.5f +
                                     rightheightsum +
                                     nodeheight[in] * 0.5f) * yspan;
                                rightheightsum += nodeheight[in];
                            }else{
                                //左側に表示
                                Card->m_fX = LastRoot->m_fX - xspan;
                                Card->m_fY = LastRoot->m_fY +
                                    (-(nodeheight[lastrootindex] - rightheight) * 0.5f +
                                     heightsum +
                                     nodeheight[in] * 0.5f) * yspan;
                                heightsum += nodeheight[in];
                            }
                        }else{
                            //ルート以外からの分岐（下位）
                            if (LastRoot->m_fX > Root->m_fX){
                                Card->m_fX = LastRoot->m_fX + xspan;
                            }else{
                                Card->m_fX = LastRoot->m_fX - xspan;
                            }
                            Card->m_fY = LastRoot->m_fY +
                                (-nodeheight[lastrootindex] * 0.5f +
                                 heightsum +
                                 nodeheight[in] * 0.5f) * yspan;
                            heightsum += nodeheight[in];
                        }
                    }
                }
            }
            group++;
        }
    }

    delete[] nodeheight;


    //各Treeを重ならないように反発させる

    if (group > 0){
        float *groupx = new float[group];//ルートノードの座標
        float *groupy = new float[group];
        float *groupsizex = new float[group];//ツリーのサイズ
        float *groupsizey = new float[group];
        float *groupnewx = new float[group];//移動後のルーとノードの座標
        float *groupnewy = new float[group];

        //各グループ（木）のサイズを得る
        //グループループ
        for (int ig = 0 ; ig < group ; ig++){
            float minx = 0.0f, maxx = 0.0f, miny = 0.0f, maxy = 0.0f;
            bool first = true;
            for (int i = 0 ; i < m_Document->m_Cards->Count ; i++) if (m_CardVisible[i] && nodegroup[i] == ig){
                TCard *Card = m_Document->GetCardByIndex(i);
                if (first){
                    //最初の1回（ルート）
                    minx = maxx = Card->m_fX;
                    miny = maxy = Card->m_fY;
                    first = false;
                }else{
                    if (minx > Card->m_fX){
                        minx = Card->m_fX;
                    }
                    if (maxx < Card->m_fX){
                        maxx = Card->m_fX;
                    }
                    if (miny > Card->m_fY){
                        miny = Card->m_fY;
                    }
                    if (maxy < Card->m_fY){
                        maxy = Card->m_fY;
                    }
                }
            }
            groupx[ig] = (minx + maxx) * 0.5f;
            groupy[ig] = (miny + maxy) * 0.5f;
            if (fabs(minx - maxx) < 0.01f){
                //グループがカード1つ？
                groupsizex[ig] = xspan;
                groupsizey[ig] = yspan;
            }else{
                groupsizex[ig] = maxx - minx;
                groupsizey[ig] = maxy - miny;
            }
        }

        //反発
        bool first = true;
        for (int ig = 0 ; ig < group ; ig++){
            groupnewx[ig] = 0.0f;
            groupnewy[ig] = 0.0f;
            float weight = 0.0f;
            int count = 0;
            for (int ig2 = 0 ; ig2 < group ; ig2++) if (ig != ig2){
                float fx = groupx[ig] - groupx[ig2];
                float fy = groupy[ig] - groupy[ig2];
                float w = fx * fx + fy * fy;
                if (w > 0.0f){
                    /*
                    float wx =
                        sqrt(1.0 * groupsizex[ig] * groupsizex[ig2] / w);
                    float wy =
                        sqrt(1.0 * groupsizey[ig] * groupsizey[ig2] / w);
                    */
                    float wx =
                        (groupsizex[ig] + groupsizex[ig2]) / w;
                    float wy =
                        (groupsizey[ig] + groupsizey[ig2]) / w;

                    groupnewx[ig] += fx * wx;
                    groupnewy[ig] += fy * wy;
                    weight += 1.0f;
                    count++;
                }
            }
            if (weight > 0.0f && count > 0){
                weight /= count * count * 5 / ratio;
                groupnewx[ig] = (float)(groupnewx[ig] * weight);
                groupnewy[ig] = (float)(groupnewy[ig] * weight);
                for (int i = 0 ; i < m_Document->m_Cards->Count ; i++) if (m_CardVisible[i] && nodegroup[i] == ig){
                    TCard *Card = m_Document->GetCardByIndex(i);
                    Card->m_fX += groupnewx[ig] * 10;
                    Card->m_fY += groupnewy[ig] * 10;
                }
            }
        }

        //ノーマライズ
        float minx = 0.0f, maxx = 0.0f, miny = 0.0f, maxy = 0.0f;
        first = true;
        for (int i = 0 ; i < m_Document->m_Cards->Count ; i++) if (m_CardVisible[i]){
            TCard *Card = m_Document->GetCardByIndex(i);
            if (first){
                minx = maxx = Card->m_fX;
                miny = maxy = Card->m_fY;
                first = false;
            }else{
                if (minx > Card->m_fX){
                    minx = Card->m_fX;
                }
                if (maxx < Card->m_fX){
                    maxx = Card->m_fX;
                }
                if (miny > Card->m_fY){
                    miny = Card->m_fY;
                }
                if (maxy < Card->m_fY){
                    maxy = Card->m_fY;
                }
            }
        }
        if (fabs(minx - maxx) > 0.01f && fabs(miny - maxy) > 0.01f){
            for (int i = 0 ; i < m_Document->m_Cards->Count ; i++) if (m_CardVisible[i]){
                TCard *Card = m_Document->GetCardByIndex(i);
                Card->m_fX = (Card->m_fX - minx) / (maxx - minx);
                Card->m_fY = (Card->m_fY - miny) / (maxy - miny);
            }
        }

        delete[] groupnewy;
        delete[] groupnewx;
        delete[] groupsizey;
        delete[] groupsizex;
        delete[] groupy;
        delete[] groupx;
    }

    delete[] nodegroup;

    //固定カードの位置を元に戻す、スムーズに移動
    for (int i = 0 ; i < m_Document->m_Cards->Count ; i++) if (m_CardVisible[i]){
        TCard *Card = m_Document->GetCardByIndex(i);
        if (Card->m_bFixed || (m_bMDownBrowser > 0 && Card->m_nID == m_nTargetCard)){
            Card->m_fX = xbak[i];
            Card->m_fY = ybak[i];
        }else{
            Card->m_fX = (Card->m_fX + xbak[i]) * 0.5f;
            Card->m_fY = (Card->m_fY + ybak[i]) * 0.5f;
        }
    }

    //Foldされたカードの処理
    for (int i = 0 ; i < m_Document->m_Cards->Count ; i++) if (m_CardVisible[i]){
        if (m_CardAssign[i] != i){
            TCard *Card = m_Document->GetCardByIndex(i);
            TCard *Card2 = m_Document->GetCardByIndex(m_CardAssign[i]);
            Card->m_fX = Card2->m_fX;
            Card->m_fY = Card2->m_fY;
        }
    }

    delete[] xbak;
    delete[] ybak;
}
//---------------------------------------------------------------------------

#pragma package(smart_init)
