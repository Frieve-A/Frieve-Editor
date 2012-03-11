//---------------------------------------------------------------------------

#include <vcl.h>

#pragma hdrstop

#include <math.h>

#include "fomain_arrange.h"
#include "fomain.h"

//---------------------------------------------------------------------------
void TFo_Main::BrowserArrange_Initialize(float *CardX, float *CardY, bool *Norm)
{
    //CardX, CardY, Norm������

    //CardX, CardY, Norm�������A�ő�ŏ����W�擾
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
        //���W�������Ă��Ȃ��̂ŁA�U�炷
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
        //��\�J�[�h�ł͂Ȃ����A�Œ肳��Ă���
        return;
    }

    ratio *= 0.3f;

    //Fold����Ă���i��\�J�[�h�ȊO����������N�����邩���j
    if (m_Document->LabelIsFold(Card)){
        //�S�Ă̓������x���̃J�[�h����̃����N���l��

        //�����N����Ă���J�[�h�ɋ߂Â�
        float count = 0;
        float X = 0.0f;
        float Y = 0.0f;
        bool *used = new bool[m_Document->m_Cards->Count];
        memset(used, 0, sizeof(bool) * m_Document->m_Cards->Count);
        for (int ic = 0 ; ic < m_Document->m_Cards->Count ; ic++) if (m_CardVisible[ic] && m_CardAssign[ic] == i){
            TCard *Card_ = m_Document->GetCardByIndex(ic);
            //�����N���[�v
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
                    //�֘A����J�[�h������
                    /*
                    float bonus;
                    if (m_nTargetCard == Link->m_nDestID || m_nTargetCard == Link->m_nFromID || m_nTargetLink == i2){
                        //�I�𒆂̃J�[�h�A�����N�̉e���͂��グ��
                        bonus = 2.0f;
                    }else{
                        //�ʏ�̃J�[�h
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
            //���W�X�V
            Card->m_fX = (float)((X / count) * ratio + CardX[i] * (1.0f - ratio));
            Card->m_fY = (float)((Y / count) * ratio + CardY[i] * (1.0f - ratio));
            Norm[i] = true;
        }
    }else{
        //�ʏ�i1���̃J�[�h����̃����N�̂ݍl���j

        //�����N����Ă���J�[�h�ɋ߂Â�
        float count = 0;
        float X = 0.0f;
        float Y = 0.0f;
        //�����N���[�v
        for (int i2 = 0 ; i2 < m_Document->m_Links->Count ; i2++) if (m_LinkVisible[i2]){
            TLink *Link = m_Document->GetLinkByIndex(i2);
            int card2index = -1;
            if (Link->m_nFromID == Card->m_nID){
                card2index = m_Document->SearchCardIndex(Link->m_nDestID);
            }else if (Link->m_nDestID == Card->m_nID){
                card2index = m_Document->SearchCardIndex(Link->m_nFromID);
            }
            if (card2index >= 0 && card2index != i) if (m_CardVisible[card2index]){
                //�֘A����J�[�h������
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
            //���W�X�V
            Card->m_fX = (float)((X / count) * ratio + CardX[i] * (1.0f - ratio));
            Card->m_fY = (float)((Y / count) * ratio + CardY[i] * (1.0f - ratio));
            Norm[i] = true;
        }

        /*
        //�ʏ�i1���̃J�[�h����̃����N�̂ݍl���j

        //�����N����Ă���J�[�h�ɋ߂Â�
        float count = 1;
        float X = Card->m_fX;
        float Y = Card->m_fY;
        //�����N���[�v
        for (int i2 = 0 ; i2 < m_Document->m_Links->Count ; i2++) if (m_LinkVisible[i2]){
            TLink *Link = m_Document->GetLinkByIndex(i2);
            int card2index = -1;
            if (Link->m_nFromID == Card->m_nID){
                card2index = m_Document->SearchCardIndex(Link->m_nDestID);
            }else if (Link->m_nDestID == Card->m_nID){
                card2index = m_Document->SearchCardIndex(Link->m_nFromID);
            }
            if (card2index >= 0 && card2index != i) if (m_CardVisible[card2index]){
                //�֘A����J�[�h������
                X += CardX[card2index];
                Y += CardY[card2index];
                count += 1.0f;
            }
        }

        if (count > 1.0f){
            //���W�X�V
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
    //���̃J�[�h�Ɣ�������
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
                //���ׂĂ�fold����Ă��Ȃ�

                //���W���d�Ȃ��Ă���ꍇ�̔���
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
        //��\�J�[�h�ł͂Ȃ����A�Œ肳��Ă���
        return;
    }

    //*
    //���̃J�[�h�Ɣ�������
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
                //���ׂĂ�fold����Ă��Ȃ�

                //���W���d�Ȃ��Ă���ꍇ�̔���
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
    //�e���x���̒��S���W�v�Z
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
        //��\�J�[�h�ł͂Ȃ����A�Œ肳��Ă���
        return;
    }

    //if (Card->m_Labels->Count == 0){
    if (m_Document->CountEnableLabel(Card) == 0){
        //���x�������Ă��Ȃ�
        return;
    }


    //float ratio2 = ratio * 0.3f;
    float ratio2 = ratio;
    //ratio *= 0.1f;

    int count = 1;
    float X = Card->m_fX;
    float Y = Card->m_fY;

    //���x���Ŏw�肳�ꂽ���W�ɋ߂Â�
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
    //�������x���������Ă���J�[�h�ɋ߂Â�
    for (int i2 = 0 ; i2 < m_Document->m_Cards->Count ; i2++) if (i != i2) if (m_CardVisible[i2] && m_CardAssign[i2] == i2){
        TCard *Card2 = m_Document->GetCardByIndex(i2);
        for (int il = 0 ; il < Card->m_Labels->Count ; il++){
            int labelindex = Card->m_Labels->GetLabel(il);
            TCardLabel *Label = m_Document->GetLabelByIndex(0, labelindex - 1);
            if (Label->m_bEnable){
                if (Card2->m_Labels->Contain(labelindex)){
                    //������̃J�[�h�ɂ����x��������
                    X += CardX[i2];
                    Y += CardY[i2];
                    count ++;
                }
            }
        }
    }
    */
    if (count > 1){
        //���W�X�V
        Card->m_fX = (X / count) * ratio2 + Card->m_fX * (1.0f - ratio2);
        Card->m_fY = (Y / count) * ratio2 + Card->m_fY * (1.0f - ratio2);
        Norm[i] = true;
    }

    /*
    //���x���������Ă��Ȃ����̂Ɣ�������
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
        //��\�J�[�h�ł͂Ȃ����A�Œ肳��Ă���
        return;
    }

    //�J�[�h�����߂����̂قǋ߂���

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
    //���t���߂����̂قǋ߂���

    //�i�������j

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
        //��\�J�[�h�ł͂Ȃ����A�Œ肳��Ă���
        return;
    }

    int source = idxtable[i];//m_SimMatrix->SearchIndex(Card->m_nID);
    if (source < 0){
        return;
    }

    //�ގ��J�[�h�قǋ߂���
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
    //��ʓ��Ɏ��܂�悤�Ƀm�[�}���C�Y
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
    //�����݂͂̂ɏ]�����W�X�V

    float *CardX = new float[m_Document->m_Cards->Count];//X���W�o�b�N�A�b�v
    float *CardY = new float[m_Document->m_Cards->Count];//Y���W�o�b�N�A�b�v
    bool *Norm = new bool[m_Document->m_Cards->Count];//�m�[�}���C�Y�Ώ�
    memset(Norm, 0, sizeof(bool) * m_Document->m_Cards->Count);

    BrowserArrange_Initialize(CardX, CardY, Norm);

    //��ʓ��Ɏ��܂�悤�Ƀm�[�}���C�Y
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
    //�����݂͂̂ɏ]�����W�X�V

    float *CardX = new float[m_Document->m_Cards->Count];//X���W�o�b�N�A�b�v
    float *CardY = new float[m_Document->m_Cards->Count];//Y���W�o�b�N�A�b�v
    bool *Norm = new bool[m_Document->m_Cards->Count];//�m�[�}���C�Y�Ώ�
    memset(Norm, 0, sizeof(bool) * m_Document->m_Cards->Count);

    BrowserArrange_Initialize(CardX, CardY, Norm);

    //���W�X�V

    //���̃J�[�h�Ɣ�������
    //�J�[�h���[�v
    for (int i = 0 ; i < m_Document->m_Cards->Count ; i++) if (m_CardVisible[i]){
        TCard *Card = m_Document->GetCardByIndex(i);

        //���̃J�[�h�Ɣ�������
        BrowserArrange_Repulsion(i, Card, CardX, CardY, Norm, ratio);
    }

    //��ʓ��Ɏ��܂�悤�Ƀm�[�}���C�Y
    BrowserArrange_Normalize(CardX, CardY, Norm);

    delete[] Norm;
    delete[] CardX;
    delete[] CardY;
}
//---------------------------------------------------------------------------
void TFo_Main::BrowserArrangeByLink(float ratio)
{
    //�����N�ɏ]�����W�X�V

    float *CardX = new float[m_Document->m_Cards->Count];//X���W�o�b�N�A�b�v
    float *CardY = new float[m_Document->m_Cards->Count];//Y���W�o�b�N�A�b�v
    bool *Norm = new bool[m_Document->m_Cards->Count];//�m�[�}���C�Y�Ώ�
    memset(Norm, 0, sizeof(bool) * m_Document->m_Cards->Count);

    BrowserArrange_Initialize(CardX, CardY, Norm);

    //���W�X�V

    //���̃J�[�h�Ɣ�������
    //�J�[�h���[�v
    for (int i = 0 ; i < m_Document->m_Cards->Count ; i++) if (m_CardVisible[i]){
        TCard *Card = m_Document->GetCardByIndex(i);

        //���̃J�[�h�Ɣ�������
        BrowserArrange_Repulsion(i, Card, CardX, CardY, Norm, 0.5f);
    }

    //��U���W�X�V
    BrowserArrange_Normalize(CardX, CardY, Norm);

    //�����N����Ă���J�[�h�ɋ߂Â�
    //�J�[�h���[�v
    for (int i = 0 ; i < m_Document->m_Cards->Count ; i++) if (m_CardVisible[i]){
        TCard *Card = m_Document->GetCardByIndex(i);

        //�����N����Ă���J�[�h�ɋ߂Â�
        BrowserArrange_Link(i, Card, CardX, CardY, Norm, ratio * 0.66);
    }

    //��ʓ��Ɏ��܂�悤�Ƀm�[�}���C�Y
    BrowserArrange_Normalize(CardX, CardY, Norm);

    delete[] Norm;
    delete[] CardX;
    delete[] CardY;
}
//---------------------------------------------------------------------------
void TFo_Main::BrowserArrangeByLabel(float ratio)
{
    //���x���ɏ]�����W�X�V

    float *CardX = new float[m_Document->m_Cards->Count];//X���W�o�b�N�A�b�v
    float *CardY = new float[m_Document->m_Cards->Count];//Y���W�o�b�N�A�b�v
    bool *Norm = new bool[m_Document->m_Cards->Count];//�m�[�}���C�Y�Ώ�
    memset(Norm, 0, sizeof(bool) * m_Document->m_Cards->Count);

    BrowserArrange_Initialize(CardX, CardY, Norm);

    //���W�X�V

    //�J�[�h���[�v
    //���̃J�[�h�Ɣ�������
    for (int i = 0 ; i < m_Document->m_Cards->Count ; i++) if (m_CardVisible[i]){
        TCard *Card = m_Document->GetCardByIndex(i);

        //���̃J�[�h�Ɣ�������
        BrowserArrange_Repulsion(i, Card, CardX, CardY, Norm, 0.5f);
    }

    //��U���W�X�V
    BrowserArrange_Normalize(CardX, CardY, Norm);

    //�����N����Ă���J�[�h�ɋ߂Â�
    //�J�[�h���[�v
    for (int i = 0 ; i < m_Document->m_Cards->Count ; i++) if (m_CardVisible[i]){
        TCard *Card = m_Document->GetCardByIndex(i);

        //�����N����Ă���J�[�h�ɋ߂Â�
        BrowserArrange_Link(i, Card, CardX, CardY, Norm, ratio / 4);
    }

    //��U���W�X�V
    BrowserArrange_Normalize(CardX, CardY, Norm);

    //�������x���͋߂��A�Ⴄ���x���͉���
    BrowserArrange_LabelPrepare();
    //�J�[�h���[�v
    for (int i = 0 ; i < m_Document->m_Cards->Count ; i++) if (m_CardVisible[i]){
        TCard *Card = m_Document->GetCardByIndex(i);

        //�������x���͋߂��A�Ⴄ���x���͉���
        BrowserArrange_Label(i, Card, CardX, CardY, Norm, ratio / 3);
    }

    //��ʓ��Ɏ��܂�悤�Ƀm�[�}���C�Y
    BrowserArrange_Normalize(CardX, CardY, Norm);

    delete[] Norm;
    delete[] CardX;
    delete[] CardY;
}
//---------------------------------------------------------------------------
void TFo_Main::BrowserArrangeByIndex(float ratio)
{
    //�J�[�h���ɏ]�����W�X�V

    float *CardX = new float[m_Document->m_Cards->Count];//X���W�o�b�N�A�b�v
    float *CardY = new float[m_Document->m_Cards->Count];//Y���W�o�b�N�A�b�v
    bool *Norm = new bool[m_Document->m_Cards->Count];//�m�[�}���C�Y�Ώ�
    memset(Norm, 0, sizeof(bool) * m_Document->m_Cards->Count);

    BrowserArrange_Initialize(CardX, CardY, Norm);

    //���W�X�V

    //���W�X�V

    //�J�[�h���[�v
    //���̃J�[�h�Ɣ�������
    for (int i = 0 ; i < m_Document->m_Cards->Count ; i++) if (m_CardVisible[i]){
        TCard *Card = m_Document->GetCardByIndex(i);

        //���̃J�[�h�Ɣ�������
        BrowserArrange_Repulsion(i, Card, CardX, CardY, Norm, 0.5f);
    }

    //��U���W�X�V
    BrowserArrange_Normalize(CardX, CardY, Norm);


    //�J�[�h���[�v
    for (int i = 0 ; i < m_Document->m_Cards->Count ; i++) if (m_CardVisible[i]){
        TCard *Card = m_Document->GetCardByIndex(i);

        //�J�[�h�����߂����̂قǋ߂�
        BrowserArrange_Index(i, Card, CardX, CardY, Norm, ratio * 0.61f);
    }

    //��ʓ��Ɏ��܂�悤�Ƀm�[�}���C�Y
    BrowserArrange_Normalize(CardX, CardY, Norm);

    delete[] Norm;
    delete[] CardX;
    delete[] CardY;
}
//---------------------------------------------------------------------------
void TFo_Main::BrowserArrangeByDate(int dateindex, float ratio)
{
    //���t�ɏ]�����W�X�V

    float *CardX = new float[m_Document->m_Cards->Count];//X���W�o�b�N�A�b�v
    float *CardY = new float[m_Document->m_Cards->Count];//Y���W�o�b�N�A�b�v
    bool *Norm = new bool[m_Document->m_Cards->Count];//�m�[�}���C�Y�Ώ�
    memset(Norm, 0, sizeof(bool) * m_Document->m_Cards->Count);

    BrowserArrange_Initialize(CardX, CardY, Norm);

    //���W�X�V

    //�J�[�h���[�v
    for (int i = 0 ; i < m_Document->m_Cards->Count ; i++){
        TCard *Card = m_Document->GetCardByIndex(i);

        if (Card->m_nID != m_nTargetCard){
            //��������J�[�h

            //���̃J�[�h�Ɣ�������
            BrowserArrange_Repulsion(i, Card, CardX, CardY, Norm, ratio / 3);

            //�����N����Ă���J�[�h�ɋ߂Â�
            BrowserArrange_Link(i, Card, CardX, CardY, Norm, ratio / 3);

            //���t���߂����̂قǋ߂�
            BrowserArrange_Date(i, Card, CardX, CardY, Norm, ratio / 3, dateindex);
        }
    }

    //��ʓ��Ɏ��܂�悤�Ƀm�[�}���C�Y
    BrowserArrange_Normalize(CardX, CardY, Norm);

    delete[] Norm;
    delete[] CardX;
    delete[] CardY;
}
//---------------------------------------------------------------------------
void TFo_Main::BrowserArrangeByFold(float ratio)
{
    //�܂肽���܂ꂽ���x���ɏ]�����W�X�V
    //�ʏ��Arrange�ł͂Ȃ��A���x����Fold���ύX���ꂽ�Ƃ��ɌĂ΂��

    float *CardX = new float[m_Document->m_Cards->Count];//X���W�o�b�N�A�b�v
    float *CardY = new float[m_Document->m_Cards->Count];//Y���W�o�b�N�A�b�v
    bool *Norm = new bool[m_Document->m_Cards->Count];//�m�[�}���C�Y�Ώ�
    memset(Norm, 0, sizeof(bool) * m_Document->m_Cards->Count);
    float *Sum = new float[m_Document->m_Cards->Count];//Y���W�o�b�N�A�b�v
    memset(Sum, 0, sizeof(float) * m_Document->m_Cards->Count);
    float *LabelX = new float[m_Document->m_Labels[0]->Count];//X���W
    memset(LabelX, 0, sizeof(float) * m_Document->m_Labels[0]->Count);
    float *LabelY = new float[m_Document->m_Labels[0]->Count];//Y���W
    memset(LabelY, 0, sizeof(float) * m_Document->m_Labels[0]->Count);
    float *LabelSum = new float[m_Document->m_Labels[0]->Count];//Y���W�o�b�N�A�b�v
    memset(LabelSum, 0, sizeof(float) * m_Document->m_Labels[0]->Count);

    BrowserArrange_Initialize(CardX, CardY, Norm);

    //���W�X�V

    //���x���̒��S���W�v�Z
    //���x�����[�v
    for (int il = 0 ; il < m_Document->m_Labels[0]->Count ; il++){
        TCardLabel *Label = m_Document->GetLabelByIndex(0, il);
        if (Label->m_bEnable && Label->m_bFold){
            //�܂肽���܂ꂽ���x��

            //�J�[�h���[�v
            for (int i = 0 ; i < m_Document->m_Cards->Count ; i++) if (m_CardVisible[i]){
                TCard *Card = m_Document->GetCardByIndex(i);

                if (Card->m_Labels->Contain(il + 1)){
                    //���̃��x�����܂�
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

    //�J�[�h���W�����Z�b�g
    for (int i = 0 ; i < m_Document->m_Cards->Count ; i++) if (m_CardVisible[i]){
        CardX[i] = 0.0f;
        CardY[i] = 0.0f;
    }

    //���x���̒��S���W�ɃJ�[�h���ړ�
    //���x�����[�v
    for (int il = 0 ; il < m_Document->m_Labels[0]->Count ; il++){
        TCardLabel *Label = m_Document->GetLabelByIndex(0, il);
        if (Label->m_bEnable && Label->m_bFold){
            //�܂肽���܂ꂽ���x��

            //�J�[�h���[�v
            for (int i = 0 ; i < m_Document->m_Cards->Count ; i++) if (m_CardVisible[i]){
                TCard *Card = m_Document->GetCardByIndex(i);

                if (Card->m_Labels->Contain(il + 1)){
                    //���̃��x�����܂�

                    if (m_Document->LabelIsFold(Card)){
                        //���ׂ�fold����Ă���
                        CardX[i] += LabelX[il] / Card->m_Labels->Count;
                        CardY[i] += LabelY[il] / Card->m_Labels->Count;
                        Sum[i] += 1.0f / Card->m_Labels->Count;
                    }
                }
            }
        }
    }

    //�J�[�h���[�v
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
    float *CardX = new float[m_Document->m_Cards->Count];//X���W�o�b�N�A�b�v
    float *CardY = new float[m_Document->m_Cards->Count];//Y���W�o�b�N�A�b�v
    bool *Norm = new bool[m_Document->m_Cards->Count];//�m�[�}���C�Y�Ώ�
    memset(Norm, 0, sizeof(bool) * m_Document->m_Cards->Count);

    BrowserArrange_Initialize(CardX, CardY, Norm);

    //���W�X�V

    //���̃J�[�h�Ɣ�������
    //�J�[�h���[�v
    for (int i = 0 ; i < m_Document->m_Cards->Count ; i++) if (m_CardVisible[i]){
        TCard *Card = m_Document->GetCardByIndex(i);

        //���̃J�[�h�Ɣ�������
        BrowserArrange_Repulsion(i, Card, CardX, CardY, Norm, 0.5f);
    }

    //��U���W�X�V
    BrowserArrange_Normalize(CardX, CardY, Norm);

    //�ގ��J�[�h�ɋ߂Â�
    if (m_SimMatrix != NULL){
        //�ގ��x������

        int *idxtable = new int[m_Document->m_Cards->Count];
        for (int i = 0 ; i < m_Document->m_Cards->Count ; i++) if (m_CardVisible[i]){
            TCard *Card = m_Document->GetCardByIndex(i);
            idxtable[i] = m_SimMatrix->SearchIndex(Card->m_nID);
        }
        //�J�[�h���[�v
        for (int i = 0 ; i < m_Document->m_Cards->Count ; i++) if (m_CardVisible[i]){
            TCard *Card = m_Document->GetCardByIndex(i);

            //�ގ��J�[�h�ɋ߂Â�
            BrowserArrange_Similarity(i, Card, CardX, CardY, Norm, ratio * 0.8, idxtable);
        }
        delete[] idxtable;
    }

    //��ʓ��Ɏ��܂�悤�Ƀm�[�}���C�Y
    BrowserArrange_Normalize(CardX, CardY, Norm);

    delete[] Norm;
    delete[] CardX;
    delete[] CardY;
}
//---------------------------------------------------------------------------
void TFo_Main::PrepareMatrixArrange_AssignToMatrix2(int i, int *tmatrix, float minx, float maxx, float miny, float maxy)
{
    //i�Ԗڂ̃J�[�h���Atmatrix�Ŏ����}�g���N�X�Ɋi�[

    TCard *Card = m_Document->GetCardByIndex(i);
    float normx = (Card->m_fX - minx) / (maxx - minx);
    float normy = (Card->m_fY - miny) / (maxy - miny);

    //0.5�𒆐S�̉~��0.5���S��Rect�̍��W�ɕϊ�
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


    //�󂢂Ă���i�q�̂����A��ԋ߂��i�q��T��
    int x = -1, y = -1;
    float mindistance = m_nMatrixWidth + m_nMatrixHeight;
    //�i�q���[�v
    for (int iy = 0 ; iy < m_nMatrixHeight ; iy++){
        for (int ix = 0 ; ix < m_nMatrixWidth ; ix++){
            if (tmatrix[ix + iy * m_nMatrixWidth] == -1){
                //�󂢂Ă���
                float distance = fabs(normx * (m_nMatrixWidth - 1) - ix) + fabs(normy * (m_nMatrixHeight - 1) - iy);
                if (distance < mindistance){
                    //���߂��i�q
                    mindistance = distance;
                    x = ix;
                    y = iy;
                }
            }
        }
    }

    //�i�q�Ɋi�[
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
    //i�Ԗڂ̃J�[�h���Atmatrix�Ŏ����}�g���N�X�Ɋi�[

    TCard *Card = m_Document->GetCardByIndex(i);
    float normx = (Card->m_fX - minx) / (maxx - minx);
    float normy = (Card->m_fY - miny) / (maxy - miny);

    //�󂢂Ă���i�q�̂����A��ԋ߂��i�q��T��
    int x = -1, y = -1;
    float mindistance = m_nMatrixWidth + m_nMatrixHeight;
    //�i�q���[�v
    for (int iy = 0 ; iy < m_nMatrixHeight ; iy++){
        for (int ix = 0 ; ix < m_nMatrixWidth ; ix++){
            if (tmatrix[ix + iy * m_nMatrixWidth] == -1){
                //�󂢂Ă���
                float distance = fabs(normx * (m_nMatrixWidth - 1) - ix) + fabs(normy * (m_nMatrixHeight - 1) - iy);
                if (distance < mindistance){
                    //���߂��i�q
                    mindistance = distance;
                    x = ix;
                    y = iy;
                }
            }
        }
    }

    //�i�q�Ɋi�[
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
    //�i�q�T�C�Y�����߁A�S�J�[�h���i�q�ɔz�u�BArrange��I�������u�Ԏ��s
    if (!m_CardVisible){
        return;
    }

    /*
    float *CardX, *CardY;
    bool *Norm;
    if (type % 100 != 0){
        //�J�[�h�ʒu���o�b�N�A�b�v
        CardX = new float[m_Document->m_Cards->Count];
        CardY = new float[m_Document->m_Cards->Count];
        Norm = new bool[m_Document->m_Cards->Count];//�m�[�}���C�Y�Ώ�
        memset(Norm, 0, sizeof(bool) * m_Document->m_Cards->Count);

        BrowserArrange_Initialize(CardX, CardY, Norm);

        //���W�X�V
        switch(type){
        case 202:
            {
                float *CardX2 = new float[m_Document->m_Cards->Count];
                float *CardY2 = new float[m_Document->m_Cards->Count];

                memcpy(CardX2, CardX, sizeof(float) * m_Document->m_Cards->Count);
                memcpy(CardY2, CardY, sizeof(float) * m_Document->m_Cards->Count);

                for (int il = 0 ; il < 5 ; il++){
                    //�����N����Ă���J�[�h�ɋ߂Â�
                    //�J�[�h���[�v
                    for (int i = 0 ; i < m_Document->m_Cards->Count ; i++) if (m_CardVisible[i]){
                        TCard *Card = m_Document->GetCardByIndex(i);

                        //�����N����Ă���J�[�h�ɋ߂Â�
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
                //�������x���̃J�[�h�ɋ߂Â�
                BrowserArrange_LabelPrepare();
                //�J�[�h���[�v
                for (int i = 0 ; i < m_Document->m_Cards->Count ; i++) if (m_CardVisible[i]){
                    TCard *Card = m_Document->GetCardByIndex(i);

                    //�������x���̃J�[�h�ɋ߂Â�
                    BrowserArrange_Label(i, Card, CardX, CardY, Norm, 1.0f);
                }
            }
            break;
        case 700:
            {
                //�ގ��J�[�h�ɋ߂Â�
                int *idxtable = new int[m_Document->m_Cards->Count];
                for (int i = 0 ; i < m_Document->m_Cards->Count ; i++) if (m_CardVisible[i]){
                    TCard *Card = m_Document->GetCardByIndex(i);
                    idxtable[i] = m_SimMatrix->SearchIndex(Card->m_nID);
                }
                //�J�[�h���[�v
                for (int i = 0 ; i < m_Document->m_Cards->Count ; i++) if (m_CardVisible[i]){
                    TCard *Card = m_Document->GetCardByIndex(i);

                    //�ގ��J�[�h�ɋ߂Â�
                    BrowserArrange_Similarity(i, Card, CardX, CardY, Norm, 1.0f, idxtable);
                }
                delete[] idxtable;
            }
            break;
        }
    }
    */

    //�\������J�[�h�̐������߂�
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

    //�i�q�̏c��������߂�
    float xyratio = 1.0f;//�傫���Ȃ�قǉ��ɍL����
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

    //�i�q�̃T�C�Y�����߂�
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
        //�ߐ�Cell�ɔz�u

        //�ꎞMatrix�����A������
        int *tmatrix = new int[m_nMatrixWidth * m_nMatrixHeight];
        for (int i = 0 ; i < m_nMatrixWidth * m_nMatrixHeight ; i++){
            tmatrix[i] = -1;
        }

        //�J�[�h��Matrix�ɔz�u�iFixed�J�[�h���I���J�[�h���֘A�J�[�h�����̑��̃J�[�h�j
        int *draworder = new int[m_Document->m_Cards->Count];
        memset(draworder, 0, sizeof(int) * m_Document->m_Cards->Count);
        for (int i = 0 ; i < m_Document->m_Cards->Count ; i++){
            int vo = m_Document->GetCardByIndex(i)->m_nViewedOrder;
            draworder[vo] = i;
        }
        int targetcardindex = m_Document->SearchCardIndex(m_nTargetCard);
        //Fixed�J�[�h
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
        //�I�𒆂̃J�[�h��z�u
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
        //�֘A�J�[�h
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
        //���̑��̃J�[�h
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
        //Index���ɐ��R�Ɣz�u
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
        //���W��߂�
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

    //�i�q��ɔz�u
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
    //�K�w�\��

    //�ʒu�Œ�p���W�o�b�N�A�b�v
    float *xbak = new float[m_Document->m_Cards->Count];
    float *ybak = new float[m_Document->m_Cards->Count];
    for (int i = 0 ; i < m_Document->m_Cards->Count ; i++) if (m_CardVisible[i]){
        TCard *Card = m_Document->GetCardByIndex(i);
        xbak[i] = Card->m_fX;
        ybak[i] = Card->m_fY;
    }

    int *nodegroup = new int[m_Document->m_Cards->Count];

    //�\�����̃J�[�h�J�E���g
    int vcount = 1;
    for (int i = 0 ; i < m_Document->m_Cards->Count ; i++) if (m_CardVisible[i]){
        vcount++;
    }
    float xspan = 1.0f / sqrt(vcount);
    float yspan = 1.0f / sqrt(vcount);

    ratio *= 0.25f;


    //�e�\�����̃��[�g�m�[�h����Tree�����

    int *nodeheight = new int[m_Document->m_Cards->Count];
    memset(nodeheight, 0, sizeof(int) * m_Document->m_Cards->Count);

    //�J�[�h���[�v�i���[�g�j
    int group = 0;
    for (int i = 0 ; i < m_Document->m_Cards->Count ; i++) if (m_CardVisible[i]){
        TCard *Root = m_Document->GetCardByIndex(i);
        if (Root->m_nLevel == 0){
            //���[�g�m�[�h
            nodegroup[i] = group;

            //���̖؂̍ő僌�x���𓾂�
            int maxlevel = 0;
            int endindex = i + 1;//���̖؂̏I��Index�𓯎��Ɏ擾
            while (endindex < m_Document->m_Cards->Count){
                TCard *Card = m_Document->GetCardByIndex(endindex);
                if (Card->m_nLevel == 0){
                    //��������Tree
                    break;
                }else if (Card->m_nLevel > maxlevel){
                    maxlevel = Card->m_nLevel;
                }
                nodegroup[endindex] = group;
                endindex++;
            }

            //���ʃm�[�h�̃T�C�Y�𓾂�
            //���x�����[�v
            for (int il = maxlevel ; il >= 0 ; il--){
                //���̖؂̃m�[�h���[�v
                for (int in = i ; in < endindex ; in++) if (m_CardVisible[in]){
                    TCard *Card = m_Document->GetCardByIndex(in);
                    if (Card->m_nLevel == il){
                        //���̃J�[�h�̃T�C�Y�����ʃm�[�h�̐��̍��v�Ƃ���
                        int index = in + 1;
                        if (in == i){
                            in = i;
                        }
                        while (index < m_Document->m_Cards->Count){
                            TCard *Card = m_Document->GetCardByIndex(index);
                            if (Card->m_nLevel < il + 1){
                                //�������̃m�[�h
                                break;
                            }else if (Card->m_nLevel == il + 1){
                                //���ʃm�[�h�̃T�C�Y�𑫂�
                                nodeheight[in] += nodeheight[index];
                            }
                            index++;
                        }
                        if (nodeheight[in] == 0){
                            //�ŏ��ł��T�C�Y1�Ƃ���
                            nodeheight[in] = 1;
                        }
                    }
                }
            }

            //���[�g�m�[�h�𒆐S�ɁA���W�����߂Ă���
            //���x�����[�v
            for (int il = 1 ; il <= maxlevel ; il++){
                int heightsum = 0;//���̃��x���̏����σT�C�Y���v

                //���̖؂̃m�[�h���[�v
                TCard *LastRoot = NULL;//���O�̃��[�g�m�[�h
                int lastrootindex = -1;
                int rightcount = 0;
                int rightheight = 0;
                int rightheightsum = 0;
                //���̖؂̃m�[�h���[�v
                for (int in = i ; in < endindex ; in++) if (m_CardVisible[in]){
                    TCard *Card = m_Document->GetCardByIndex(in);
                    if (Card->m_nLevel == il - 1){
                        LastRoot = Card;
                        lastrootindex = in;
                        heightsum = 0;
                        if (il == 1){
                            //���E�ɕ�����
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
                            //���[�g����̕���
                            if (rightcount-- > 0){
                                //�E���ɕ\��
                                Card->m_fX = LastRoot->m_fX + xspan;
                                Card->m_fY = LastRoot->m_fY +
                                    (-rightheight * 0.5f +
                                     rightheightsum +
                                     nodeheight[in] * 0.5f) * yspan;
                                rightheightsum += nodeheight[in];
                            }else{
                                //�����ɕ\��
                                Card->m_fX = LastRoot->m_fX - xspan;
                                Card->m_fY = LastRoot->m_fY +
                                    (-(nodeheight[lastrootindex] - rightheight) * 0.5f +
                                     heightsum +
                                     nodeheight[in] * 0.5f) * yspan;
                                heightsum += nodeheight[in];
                            }
                        }else{
                            //���[�g�ȊO����̕���i���ʁj
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


    //�eTree���d�Ȃ�Ȃ��悤�ɔ���������

    if (group > 0){
        float *groupx = new float[group];//���[�g�m�[�h�̍��W
        float *groupy = new float[group];
        float *groupsizex = new float[group];//�c���[�̃T�C�Y
        float *groupsizey = new float[group];
        float *groupnewx = new float[group];//�ړ���̃��[�ƃm�[�h�̍��W
        float *groupnewy = new float[group];

        //�e�O���[�v�i�؁j�̃T�C�Y�𓾂�
        //�O���[�v���[�v
        for (int ig = 0 ; ig < group ; ig++){
            float minx = 0.0f, maxx = 0.0f, miny = 0.0f, maxy = 0.0f;
            bool first = true;
            for (int i = 0 ; i < m_Document->m_Cards->Count ; i++) if (m_CardVisible[i] && nodegroup[i] == ig){
                TCard *Card = m_Document->GetCardByIndex(i);
                if (first){
                    //�ŏ���1��i���[�g�j
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
                //�O���[�v���J�[�h1�H
                groupsizex[ig] = xspan;
                groupsizey[ig] = yspan;
            }else{
                groupsizex[ig] = maxx - minx;
                groupsizey[ig] = maxy - miny;
            }
        }

        //����
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

        //�m�[�}���C�Y
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

    //�Œ�J�[�h�̈ʒu�����ɖ߂��A�X���[�Y�Ɉړ�
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

    //Fold���ꂽ�J�[�h�̏���
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
