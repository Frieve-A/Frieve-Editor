//---------------------------------------------------------------------------

#include <vcl.h>

#pragma hdrstop

#include <math.h>

#include "fomain_iterscore.h"
#include "fomain.h"
#include "setting.h"

//---------------------------------------------------------------------------
void TFo_Main::IterScore()
{
    switch (SettingView.m_ScoreType){
    case 0:
        IterScore_Authenticity();
        break;
    case 1:
        IterScore_StartingPoint();
        break;
    case 2:
        IterScore_Destination();
        break;
    case 3:
        IterScore_Links_Out();
        break;
    case 4:
        IterScore_Links_In();
        break;
    case 5:
        IterScore_Links_Total();
        break;
    case 6:
        IterScore_Links_InOut();
        break;
    case 7:
        IterScore_TextLength();
        break;
    }
}
//---------------------------------------------------------------------------
void TFo_Main::IterScore_Authenticity()
{
    const float allratio = 0.1f;

    int *index = new int[m_Document->m_Cards->Count];
    float *score = new float[m_Document->m_Cards->Count];
    memset(score, 0, sizeof(float) * m_Document->m_Cards->Count);

    //スコアの移動
    float forall = 0.0f;//全てのカードに足されるスコア
    double sum = 0.0f;
    int cardcount = 0;
    //カードループ
    for (int i = 0 ; i < m_Document->m_Cards->Count ; i++){
        TCard *Card = m_Document->GetCardByIndex(i);
        if (Card->m_bVisible){
            cardcount++;
            sum += Card->m_fScore;

            //リンクを数え、リンク先を列挙
            //リンクループ
            int count = 0;
            for (int il = 0 ; il < m_Document->m_Links->Count ; il++) if (m_LinkVisible[il]){
                TLink *Link = m_Document->GetLinkByIndex(il);
                if (Link->m_nFromID == Card->m_nID){
                    index[count++] = m_Document->SearchCardIndex(Link->m_nDestID);
                }
            }

            if (count == 0){
                //リンクが無い場合、スコアはすべて適当なカードに移動
                forall += Card->m_fScore;
            }else{
                forall += Card->m_fScore  * allratio;
                float addscore = (Card->m_fScore * (1.0f - allratio)) / count;
                for (int il = 0 ; il < count ; il++){
                    //リンク先にスコアを加算
                    score[index[il]] += addscore;
                }
            }
        }
    }

    //forallを加算し、正規化
    m_fMinScore = 10000000.0f;
    m_fMaxScore = 0.0f;
    if (cardcount){
        forall /= cardcount;
        float normcoef = cardcount / sum;//1カードあたり平均1になるように正規化
        for (int i = 0 ; i < m_Document->m_Cards->Count ; i++){
            TCard *Card = m_Document->GetCardByIndex(i);
            if (Card->m_bVisible){
                float s = (score[i] + forall) * normcoef;
                Card->m_fScore = s;
                if (m_fMinScore > Card->m_fScore){
                    m_fMinScore = Card->m_fScore;
                }
                if (m_fMaxScore < Card->m_fScore){
                    m_fMaxScore = Card->m_fScore;
                }
            }
        }
    }
    if (m_fMinScore > 0){
        m_fMinScore = log(m_fMinScore);
    }
    if (m_fMaxScore > 0){
        m_fMaxScore = log(m_fMaxScore);
    }

    delete[] score;
    delete[] index;
}
//---------------------------------------------------------------------------
void TFo_Main::IterScore_StartingPoint()
{
    int *index = new int[m_Document->m_Cards->Count];
    float *score = new float[m_Document->m_Cards->Count];
    memset(score, 0, sizeof(float) * m_Document->m_Cards->Count);

    //スコアの移動
    double sum = 0.0;
    int cardcount = 0;
    //カードループ
    for (int i = 0 ; i < m_Document->m_Cards->Count ; i++){
        TCard *Card = m_Document->GetCardByIndex(i);
        if (Card->m_bVisible){
            cardcount++;

            //リンクを数え、リンク元を列挙
            //リンクループ
            int count = 0;
            for (int il = 0 ; il < m_Document->m_Links->Count ; il++) if (m_LinkVisible[il]){
                TLink *Link = m_Document->GetLinkByIndex(il);
                if (Link->m_nDestID == Card->m_nID){
                    index[count++] = m_Document->SearchCardIndex(Link->m_nFromID);
                }
            }

            if (count == 0){
                //リンクが無い
                //forall += Card->m_fScore;
            }else{
                for (int il = 0 ; il < count ; il++){
                    //リンク元にスコアを加算
                    score[index[il]] += Card->m_fScore;
                    sum += Card->m_fScore;
                }
            }

            //自分自身に1追加
            score[i] += 1.0f;
            sum += 1.0;
        }
    }

    //forallを加算し、正規化
    m_fMinScore = 10000000.0f;
    m_fMaxScore = 0.0f;
    if (cardcount){
        float normcoef = cardcount / sum;//1カードあたり平均1になるように正規化
        for (int i = 0 ; i < m_Document->m_Cards->Count ; i++){
            TCard *Card = m_Document->GetCardByIndex(i);
            if (Card->m_bVisible){
                float s = score[i] * normcoef;
                Card->m_fScore = s;
                if (m_fMinScore > Card->m_fScore){
                    m_fMinScore = Card->m_fScore;
                }
                if (m_fMaxScore < Card->m_fScore){
                    m_fMaxScore = Card->m_fScore;
                }
            }
        }
    }
    if (m_fMinScore > 0){
        m_fMinScore = log(m_fMinScore);
    }
    if (m_fMaxScore > 0){
        m_fMaxScore = log(m_fMaxScore);
    }

    delete[] score;
    delete[] index;
}
//---------------------------------------------------------------------------
void TFo_Main::IterScore_Destination()
{
    int *index = new int[m_Document->m_Cards->Count];
    float *score = new float[m_Document->m_Cards->Count];
    memset(score, 0, sizeof(float) * m_Document->m_Cards->Count);

    //スコアの移動
    double sum = 0.0;
    int cardcount = 0;
    //カードループ
    for (int i = 0 ; i < m_Document->m_Cards->Count ; i++){
        TCard *Card = m_Document->GetCardByIndex(i);
        if (Card->m_bVisible){
            cardcount++;

            //リンクを数え、リンク先を列挙
            //リンクループ
            int count = 0;
            for (int il = 0 ; il < m_Document->m_Links->Count ; il++) if (m_LinkVisible[il]){
                TLink *Link = m_Document->GetLinkByIndex(il);
                if (Link->m_nFromID == Card->m_nID){
                    index[count++] = m_Document->SearchCardIndex(Link->m_nDestID);
                }
            }

            if (count == 0){
                //リンクが無い
                //forall += Card->m_fScore;
            }else{
                for (int il = 0 ; il < count ; il++){
                    //リンク先にスコアを加算
                    score[index[il]] += Card->m_fScore;
                    sum += Card->m_fScore;
                }
            }

            //自分自身に1追加
            score[i] += 1.0f;
            sum += 1.0;
        }
    }

    //forallを加算し、正規化
    m_fMinScore = 10000000.0f;
    m_fMaxScore = 0.0f;
    if (cardcount){
        float normcoef = cardcount / sum;//1カードあたり平均1になるように正規化
        for (int i = 0 ; i < m_Document->m_Cards->Count ; i++){
            TCard *Card = m_Document->GetCardByIndex(i);
            if (Card->m_bVisible){
                float s = score[i] * normcoef;
                Card->m_fScore = s;
                if (m_fMinScore > Card->m_fScore){
                    m_fMinScore = Card->m_fScore;
                }
                if (m_fMaxScore < Card->m_fScore){
                    m_fMaxScore = Card->m_fScore;
                }
            }
        }
    }
    if (m_fMinScore > 0){
        m_fMinScore = log(m_fMinScore);
    }
    if (m_fMaxScore > 0){
        m_fMaxScore = log(m_fMaxScore);
    }

    delete[] score;
    delete[] index;
}
//---------------------------------------------------------------------------
void TFo_Main::IterScore_Links_Out()
{
    float *score = new float[m_Document->m_Cards->Count];
    memset(score, 0, sizeof(float) * m_Document->m_Cards->Count);

    //スコアの移動
    double sum = 0.0;
    int cardcount = 0;
    //カードループ
    for (int i = 0 ; i < m_Document->m_Cards->Count ; i++){
        TCard *Card = m_Document->GetCardByIndex(i);
        if (Card->m_bVisible){
            cardcount++;

            //リンクを数える
            //リンクループ
            int count = 0;
            for (int il = 0 ; il < m_Document->m_Links->Count ; il++) if (m_LinkVisible[il]){
                TLink *Link = m_Document->GetLinkByIndex(il);
                if (Link->m_nFromID == Card->m_nID){
                    count++;
                }
            }
            score[i] = count + 1;
            sum += score[i];
        }
    }

    //forallを加算し、正規化
    m_fMinScore = 10000000.0f;
    m_fMaxScore = 0.0f;
    if (cardcount){
        float normcoef = cardcount / sum;//1カードあたり平均1になるように正規化
        for (int i = 0 ; i < m_Document->m_Cards->Count ; i++){
            TCard *Card = m_Document->GetCardByIndex(i);
            if (Card->m_bVisible){
                float s = score[i] * normcoef;
                Card->m_fScore = s;
                if (m_fMinScore > Card->m_fScore){
                    m_fMinScore = Card->m_fScore;
                }
                if (m_fMaxScore < Card->m_fScore){
                    m_fMaxScore = Card->m_fScore;
                }
            }
        }
    }
    if (m_fMinScore > 0){
        m_fMinScore = log(m_fMinScore);
    }
    if (m_fMaxScore > 0){
        m_fMaxScore = log(m_fMaxScore);
    }

    delete[] score;
}
//---------------------------------------------------------------------------
void TFo_Main::IterScore_Links_In()
{
    float *score = new float[m_Document->m_Cards->Count];
    memset(score, 0, sizeof(float) * m_Document->m_Cards->Count);

    //スコアの移動
    double sum = 0.0;
    int cardcount = 0;
    //カードループ
    for (int i = 0 ; i < m_Document->m_Cards->Count ; i++){
        TCard *Card = m_Document->GetCardByIndex(i);
        if (Card->m_bVisible){
            cardcount++;

            //リンクを数える
            //リンクループ
            int count = 0;
            for (int il = 0 ; il < m_Document->m_Links->Count ; il++) if (m_LinkVisible[il]){
                TLink *Link = m_Document->GetLinkByIndex(il);
                if (Link->m_nDestID == Card->m_nID){
                    count++;
                }
            }
            score[i] = count + 1;
            sum += score[i];
        }
    }

    //forallを加算し、正規化
    m_fMinScore = 10000000.0f;
    m_fMaxScore = 0.0f;
    if (cardcount){
        float normcoef = cardcount / sum;//1カードあたり平均1になるように正規化
        for (int i = 0 ; i < m_Document->m_Cards->Count ; i++){
            TCard *Card = m_Document->GetCardByIndex(i);
            if (Card->m_bVisible){
                float s = score[i] * normcoef;
                Card->m_fScore = s;
                if (m_fMinScore > Card->m_fScore){
                    m_fMinScore = Card->m_fScore;
                }
                if (m_fMaxScore < Card->m_fScore){
                    m_fMaxScore = Card->m_fScore;
                }
            }
        }
    }
    if (m_fMinScore > 0){
        m_fMinScore = log(m_fMinScore);
    }
    if (m_fMaxScore > 0){
        m_fMaxScore = log(m_fMaxScore);
    }

    delete[] score;
}
//---------------------------------------------------------------------------
void TFo_Main::IterScore_Links_Total()
{
    float *score = new float[m_Document->m_Cards->Count];
    memset(score, 0, sizeof(float) * m_Document->m_Cards->Count);

    //スコアの移動
    double sum = 0.0;
    int cardcount = 0;
    //カードループ
    for (int i = 0 ; i < m_Document->m_Cards->Count ; i++){
        TCard *Card = m_Document->GetCardByIndex(i);
        if (Card->m_bVisible){
            cardcount++;

            //リンクを数える
            //リンクループ
            int count = 0;
            for (int il = 0 ; il < m_Document->m_Links->Count ; il++) if (m_LinkVisible[il]){
                TLink *Link = m_Document->GetLinkByIndex(il);
                if (Link->m_nFromID == Card->m_nID || Link->m_nDestID == Card->m_nID){
                    count++;
                }
            }
            score[i] = count + 1;
            sum += score[i];
        }
    }

    //forallを加算し、正規化
    m_fMinScore = 10000000.0f;
    m_fMaxScore = 0.0f;
    if (cardcount){
        float normcoef = cardcount / sum;//1カードあたり平均1になるように正規化
        for (int i = 0 ; i < m_Document->m_Cards->Count ; i++){
            TCard *Card = m_Document->GetCardByIndex(i);
            if (Card->m_bVisible){
                float s = score[i] * normcoef;
                Card->m_fScore = s;
                if (m_fMinScore > Card->m_fScore){
                    m_fMinScore = Card->m_fScore;
                }
                if (m_fMaxScore < Card->m_fScore){
                    m_fMaxScore = Card->m_fScore;
                }
            }
        }
    }
    if (m_fMinScore > 0){
        m_fMinScore = log(m_fMinScore);
    }
    if (m_fMaxScore > 0){
        m_fMaxScore = log(m_fMaxScore);
    }

    delete[] score;
}
//---------------------------------------------------------------------------
void TFo_Main::IterScore_Links_InOut()
{
    float *score = new float[m_Document->m_Cards->Count];
    memset(score, 0, sizeof(float) * m_Document->m_Cards->Count);

    //スコアの移動
    double sum = 0.0;
    int cardcount = 0;
    int min = 0;
    //カードループ
    for (int i = 0 ; i < m_Document->m_Cards->Count ; i++){
        TCard *Card = m_Document->GetCardByIndex(i);
        if (Card->m_bVisible){
            cardcount++;

            //リンクを数える
            //リンクループ
            int count = 0;
            for (int il = 0 ; il < m_Document->m_Links->Count ; il++) if (m_LinkVisible[il]){
                TLink *Link = m_Document->GetLinkByIndex(il);
                if (Link->m_nFromID == Card->m_nID){
                    count--;
                }else if (Link->m_nDestID == Card->m_nID){
                    count++;
                }
            }
            if (min > count){
                min = count;
            }
            score[i] = count;
            sum += score[i];
        }
    }

    //forallを加算し、正規化
    m_fMinScore = 10000000.0f;
    m_fMaxScore = 0.0f;
    if (cardcount){
        sum += (1 - min) * cardcount;//マイナスにならないように、最小値+1を足すため
        float normcoef = cardcount / sum;//1カードあたり平均1になるように正規化
        for (int i = 0 ; i < m_Document->m_Cards->Count ; i++){
            TCard *Card = m_Document->GetCardByIndex(i);
            if (Card->m_bVisible){
                float s = (score[i] + (1 - min)) * normcoef;
                Card->m_fScore = s;
                if (m_fMinScore > Card->m_fScore){
                    m_fMinScore = Card->m_fScore;
                }
                if (m_fMaxScore < Card->m_fScore){
                    m_fMaxScore = Card->m_fScore;
                }
            }
        }
    }
    if (m_fMinScore > 0){
        m_fMinScore = log(m_fMinScore);
    }
    if (m_fMaxScore > 0){
        m_fMaxScore = log(m_fMaxScore);
    }

    delete[] score;
}
//---------------------------------------------------------------------------
void TFo_Main::IterScore_TextLength()
{
    float *score = new float[m_Document->m_Cards->Count];
    memset(score, 0, sizeof(float) * m_Document->m_Cards->Count);

    //スコアの移動
    double sum = 0.0;
    int cardcount = 0;
    int min = 0;
    //カードループ
    for (int i = 0 ; i < m_Document->m_Cards->Count ; i++){
        TCard *Card = m_Document->GetCardByIndex(i);
        if (Card->m_bVisible){
            cardcount++;
            score[i] = Card->m_Lines->Text.Length();
            sum += score[i];
        }
    }

    //forallを加算し、正規化
    m_fMinScore = 10000000.0f;
    m_fMaxScore = 0.0f;
    if (cardcount){
        sum += (1 - min) * cardcount;//マイナスにならないように、最小値+1を足すため
        float normcoef = cardcount / sum;//1カードあたり平均1になるように正規化
        for (int i = 0 ; i < m_Document->m_Cards->Count ; i++){
            TCard *Card = m_Document->GetCardByIndex(i);
            if (Card->m_bVisible){
                float s = (score[i] + (1 - min)) * normcoef;
                Card->m_fScore = s;
                if (m_fMinScore > Card->m_fScore){
                    m_fMinScore = Card->m_fScore;
                }
                if (m_fMaxScore < Card->m_fScore){
                    m_fMaxScore = Card->m_fScore;
                }
            }
        }
    }
    if (m_fMinScore > 0){
        m_fMinScore = log(m_fMinScore);
    }
    if (m_fMaxScore > 0){
        m_fMaxScore = log(m_fMaxScore);
    }

    delete[] score;
}
//---------------------------------------------------------------------------

#pragma package(smart_init)
