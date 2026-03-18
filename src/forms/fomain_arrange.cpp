// ---------------------------------------------------------------------------

#include <vcl.h>

#pragma hdrstop

#include <math.h>

#include "fomain.h"
#include "fomain_arrange.h"

// ---------------------------------------------------------------------------
void TFo_Main::BrowserArrange_Initialize(float *CardX, float *CardY,
                                         bool *Norm) {
  // Initialize CardX, CardY, Norm

  // Initialize CardX, CardY, Norm; get min/max coordinates
  float maxx = 0.5f, minx = 0.5f, maxy = 0.5f, miny = 0.5f;
  for (int i = 0; i < m_Document->m_Cards->Count; i++) {
    Norm[i] = false;
    TCard *Card = m_Document->GetCardByIndex(i);
    CardX[i] = Card->m_fX;
    CardY[i] = Card->m_fY;

    if (CardX[i] < minx) {
      minx = CardX[i];
    }
    if (CardX[i] > maxx) {
      maxx = CardX[i];
    }
    if (CardY[i] < miny) {
      miny = CardY[i];
    }
    if (CardY[i] > maxy) {
      maxy = CardY[i];
    }

    /*
     if (Me_Debug->Tag == 1){
     Me_Debug->Lines->Add(FormatFloat("0.00", CardX[i]) + "," +
     FormatFloat("0.00", CardY[i]));
     }
     */
  }

  if ((maxx == minx || maxy == miny) && m_Document->m_Cards->Count > 1) {
    // Scatter (no coordinates set)
    for (int i = 0; i < m_Document->m_Cards->Count; i++) {
      TCard *Card = m_Document->GetCardByIndex(i);
      if (!Card->m_bFixed) {
        Card->m_fX = CardX[i] = (rand() % 10000) / 10000.0f;
        Card->m_fY = CardY[i] = (rand() % 10000) / 10000.0f;
      }
    }
  }
}

// ---------------------------------------------------------------------------
void TFo_Main::BrowserArrange_Link(int i, TCard *Card, float *CardX,
                                   float *CardY, bool *Norm, float ratio) {
  if (m_CardAssign[i] != i || Card->m_bFixed ||
      (Card->m_bSelected && m_bMDownBrowser)) {
    // Not representative card or fixed
    return;
  }

  ratio *= 0.3f;

  // Folded (links may exist from non-representative cards)
  if (m_Document->LabelIsFold(Card)) {
    // Consider links from all same-label cards

    // Move toward linked cards
    float count = 0;
    float X = 0.0f;
    float Y = 0.0f;
    bool *used = new bool[m_Document->m_Cards->Count];
    memset(used, 0, sizeof(bool) * m_Document->m_Cards->Count);
    for (int ic = 0; ic < m_Document->m_Cards->Count; ic++)
      if (m_CardVisible[ic] && m_CardAssign[ic] == i) {
        TCard *Card_ = m_Document->GetCardByIndex(ic);
        // Link loop
        for (int i2 = 0; i2 < m_Document->m_Links->Count; i2++)
          if (m_LinkVisible[i2]) {
            TLink *Link = m_Document->GetLinkByIndex(i2);
            int card2index = -1;
            if (Link->m_nFromID == Card_->m_nID) {
              card2index = m_Document->SearchCardIndex(Link->m_nDestID);
            } else if (Link->m_nDestID == Card_->m_nID) {
              card2index = m_Document->SearchCardIndex(Link->m_nFromID);
            }
            if (card2index >= 0 && card2index != i)
              if (m_CardVisible[card2index] && !used[card2index]) {
                used[card2index] = true;
                // Related card found
                /*
                 float bonus;
                 if (m_nTargetCard == Link->m_nDestID || m_nTargetCard ==
                 Link->m_nFromID || m_nTargetLink == i2){
                 // Boost influence for selected card/link
                 bonus = 2.0f;
                 }else{
                 // Normal card
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

    if (count > 0.0f) {
      // Update coordinates
      Card->m_fX = (float)((X / count) * ratio + CardX[i] * (1.0f - ratio));
      Card->m_fY = (float)((Y / count) * ratio + CardY[i] * (1.0f - ratio));
      Norm[i] = true;
    }
  } else {
    // Normal (consider links from single card only)

    // Move toward linked cards
    float count = 0;
    float X = 0.0f;
    float Y = 0.0f;
    // Link loop
    for (int i2 = 0; i2 < m_Document->m_Links->Count; i2++)
      if (m_LinkVisible[i2]) {
        TLink *Link = m_Document->GetLinkByIndex(i2);
        int card2index = -1;
        if (Link->m_nFromID == Card->m_nID) {
          card2index = m_Document->SearchCardIndex(Link->m_nDestID);
        } else if (Link->m_nDestID == Card->m_nID) {
          card2index = m_Document->SearchCardIndex(Link->m_nFromID);
        }
        if (card2index >= 0 && card2index != i)
          if (m_CardVisible[card2index]) {
            // Related card found
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

    if (count > 0.0f) {
      // Update coordinates
      Card->m_fX = (float)((X / count) * ratio + CardX[i] * (1.0f - ratio));
      Card->m_fY = (float)((Y / count) * ratio + CardY[i] * (1.0f - ratio));
      Norm[i] = true;
    }

    /*
     // Normal (consider links from single card only)

     // Move toward linked cards
     float count = 1;
     float X = Card->m_fX;
     float Y = Card->m_fY;
     // Link loop
     for (int i2 = 0 ; i2 < m_Document->m_Links->Count ; i2++) if
     (m_LinkVisible[i2]){ TLink *Link = m_Document->GetLinkByIndex(i2); int
     card2index = -1; if (Link->m_nFromID == Card->m_nID){ card2index =
     m_Document->SearchCardIndex(Link->m_nDestID); }else if (Link->m_nDestID ==
     Card->m_nID){ card2index = m_Document->SearchCardIndex(Link->m_nFromID);
     }
     if (card2index >= 0 && card2index != i) if (m_CardVisible[card2index]){
     // Related card found
     X += CardX[card2index];
     Y += CardY[card2index];
     count += 1.0f;
     }
     }

     if (count > 1.0f){
     // Update coordinates
     Card->m_fX = (float)((X / count) * ratio + Card->m_fX * (1.0f - ratio));
     Card->m_fY = (float)((Y / count) * ratio + Card->m_fY * (1.0f - ratio));
     Norm[i] = true;
     }
     */
  }
}

// ---------------------------------------------------------------------------
void TFo_Main::BrowserArrange_Repulsion(int i, TCard *Card, float *CardX,
                                        float *CardY, bool *Norm, float ratio) {
  if (m_CardAssign[i] != i || Card->m_bFixed ||
      (Card->m_bSelected && m_bMDownBrowser)) {
    // Not representative card or fixed
    return;
  }

  // Repel from other cards
  float weight = 0.0f;
  float X = 0.0f;
  float Y = 0.0f;
  int count = 0;
  for (int i2 = 0; i2 < m_Document->m_Cards->Count; i2++)
    if (i != i2)
      if (m_CardVisible[i2] && m_CardAssign[i2] == i2) {
        float fx = CardX[i] - CardX[i2];
        float fy = CardY[i] - CardY[i2];
        float w = fx * fx + fy * fy;
        if (w > 0.0f) {
          if (m_CardWidth[i2] * m_CardHeight[i2] > 0) {
            w = sqrt((1.0 * m_CardWidth[i2] * m_CardHeight[i2]) / w);
          } else {
            w = sqrt(100 / w);
          }

          X += (CardX[i] - CardX[i2]) * w;
          Y += (CardY[i] - CardY[i2]) * w;
          weight += 1.0f;
          count++;
        } else {
          if (!m_Document->LabelIsFold(Card)) {
            // Not all folded

            // Repel when coordinates overlap
            X += (rand() % 101 - 50) * 0.0001f;
            Y += (rand() % 101 - 50) * 0.0001f;
            weight += 1.0f;
            count++;
          }
        }
      }
  if (weight > 0.0f && count > 0) {
    weight /= count * count * 5 / ratio;
    Card->m_fX += (float)(X * weight);
    Card->m_fY += (float)(Y * weight);
    Norm[i] = true;
  }
}

// ---------------------------------------------------------------------------
void TFo_Main::BrowserArrange_LabelPrepare() {
  // Calculate center coordinates for each label
  for (int il = 0; il < m_Document->m_Labels[0]->Count; il++) {
    TCardLabel *Label = m_Document->GetLabelByIndex(0, il);
    if (!Label->m_bFixed) {
      Label->m_fX = 0.0f;
      Label->m_fY = 0.0f;
      int count = 0;
      for (int i = 0; i < m_Document->m_Cards->Count; i++)
        if (m_CardVisible[i] && m_CardAssign[i] == i) {
          TCard *Card = m_Document->GetCardByIndex(i);
          if (Card->m_Labels->Contain(il + 1)) {
            Label->m_fX += Card->m_fX;
            Label->m_fY += Card->m_fY;
            count++;
          }
        }

      if (count > 0) {
        Label->m_fX /= count;
        Label->m_fY /= count;
      } else {
        Label->m_fX = 0.5f;
        Label->m_fY = 0.5f;
      }
    }
  }
}

// ---------------------------------------------------------------------------
void TFo_Main::BrowserArrange_Label(int i, TCard *Card, float *CardX,
                                    float *CardY, bool *Norm, float ratio) {
  if (m_CardAssign[i] != i || Card->m_bFixed ||
      (Card->m_bSelected && m_bMDownBrowser)) {
    // Not representative card or fixed
    return;
  }

  if (m_Document->CountEnableLabel(Card) == 0) {
    // No labels
    return;
  }

  float ratio2 = ratio;

  int count = 1;
  float X = Card->m_fX;
  float Y = Card->m_fY;

  // Move toward label-specified coordinates
  for (int il = 0; il < Card->m_Labels->Count; il++) {
    int labelindex = Card->m_Labels->GetLabel(il);
    TCardLabel *Label = m_Document->GetLabelByIndex(0, labelindex - 1);
    if (Label->m_bEnable) {
      X += Label->m_fX;
      Y += Label->m_fY;
      count++;
    }
  }

  /*
   // Move toward cards with same label
   for (int i2 = 0 ; i2 < m_Document->m_Cards->Count ; i2++) if (i != i2) if
   (m_CardVisible[i2] && m_CardAssign[i2] == i2){ TCard *Card2 =
   m_Document->GetCardByIndex(i2); for (int il = 0 ; il < Card->m_Labels->Count
   ; il++){ int labelindex = Card->m_Labels->GetLabel(il); TCardLabel *Label =
   m_Document->GetLabelByIndex(0, labelindex - 1); if (Label->m_bEnable){ if
   (Card2->m_Labels->Contain(labelindex)){
   // Other card also has label
   X += CardX[i2];
   Y += CardY[i2];
   count ++;
   }
   }
   }
   }
   */
  if (count > 1) {
    // Update coordinates
    Card->m_fX = (X / count) * ratio2 + Card->m_fX * (1.0f - ratio2);
    Card->m_fY = (Y / count) * ratio2 + Card->m_fY * (1.0f - ratio2);
    Norm[i] = true;
  }

  /*
   // Repel from cards without label
   int count2 = 0;
   X = 0.0f;
   Y = 0.0f;
   for (int i2 = 0 ; i2 < m_Document->m_Cards->Count ; i2++) if (i != i2) if
   (m_CardVisible[i2] && m_CardAssign[i2] == i2){ TCard *Card2 =
   m_Document->GetCardByIndex(i2); for (int il = 0 ; il < Card->m_Labels->Count
   ; il++){ bool donthave = true; for (int il2 = 0 ; il2 <
   Card2->m_Labels->Count ; il2++){ if (Card->m_Labels->GetLabel(il) ==
   Card2->m_Labels->GetLabel(il2)){ donthave = false; break;
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

// ---------------------------------------------------------------------------
void TFo_Main::BrowserArrange_Index(int i, TCard *Card, float *CardX,
                                    float *CardY, bool *Norm, float ratio) {
  if (m_CardAssign[i] != i || Card->m_bFixed ||
      (Card->m_bSelected && m_bMDownBrowser)) {
    // Not representative card or fixed
    return;
  }

  // Closer cards by index are placed nearer

  float weight = 0.0f;
  float X = 0.0f;
  float Y = 0.0f;
  for (int i2 = 0; i2 < m_Document->m_Cards->Count; i2++)
    if (i != i2)
      if (m_CardVisible[i2] && m_CardAssign[i2] == i2) {
        float w = m_Document->m_Cards->Count - fabs(i - i2);
        w *= w;
        X += CardX[i2] * w;
        Y += CardY[i2] * w;
        weight += w;
      }
  if (weight > 0.0f) {
    Card->m_fX = (X / weight) * ratio + Card->m_fX * (1.0f - ratio);
    Card->m_fY = (Y / weight) * ratio + Card->m_fY * (1.0f - ratio);
    Norm[i] = true;
  }
}

// ---------------------------------------------------------------------------
void TFo_Main::BrowserArrange_Date(int i, TCard *Card, float *CardX,
                                   float *CardY, bool *Norm, float ratio,
                                   int dateindex) {
  // Closer dates are placed nearer

  // (Under adjustment)

  float weight = 0.0f;
  float X = 0.0f;
  float Y = 0.0f;
  for (int i2 = 0; i2 < m_Document->m_Cards->Count; i2++)
    if (i != i2) {
      TCard *Card2 = m_Document->GetCardByIndex(i2);
      float w;
      switch (dateindex) {
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

      if (w > 0.0f) {
        w = 1.0f / w;
        X += (CardX[i] - CardX[i2]) * w;
        Y += (CardY[i] - CardY[i2]) * w;
        weight += w;
      }
    }
  if (weight > 0.0f) {
    ratio /= 100;
    Card->m_fX += X / m_Document->m_Cards->Count * ratio * 0.25f;
    Card->m_fY += Y / m_Document->m_Cards->Count * ratio;
    Norm[i] = true;
  }
}

// ---------------------------------------------------------------------------
void TFo_Main::BrowserArrange_Similarity(int i, TCard *Card, float *CardX,
                                         float *CardY, bool *Norm, float ratio,
                                         int *idxtable) {
  if (m_CardAssign[i] != i || Card->m_bFixed ||
      (Card->m_bSelected && m_bMDownBrowser)) {
    // Not representative card or fixed
    return;
  }

  int source = idxtable[i];
  if (source < 0) {
    return;
  }

  // Similar cards placed nearer
  ratio *= 0.5f;

  float weight = 0.0f;
  float X = 0.0f;
  float Y = 0.0f;
  for (int i2 = 0; i2 < m_Document->m_Cards->Count; i2++)
    if (i != i2)
      if (m_CardVisible[i2] && m_CardAssign[i2] == i2) {
        // TCard *Card2 = m_Document->GetCardByIndex(i2);
        int target = idxtable[i2];
        // m_SimMatrix->SearchIndex(Card2->m_nID);
        if (target >= 0) {
          float w = m_SimMatrix->S(source, target);
          if (w > 0.0f) {
            X += CardX[i2] * w;
            Y += CardY[i2] * w;
            weight += w;
          }
        }
      }
  if (weight > 0.0f) {
    Card->m_fX = (X / weight) * ratio + Card->m_fX * (1.0f - ratio);
    Card->m_fY = (Y / weight) * ratio + Card->m_fY * (1.0f - ratio);
    /*
     Card->m_fX += X / m_Document->m_Cards->Count * ratio;
     Card->m_fY += Y / m_Document->m_Cards->Count * ratio;
     */
    Norm[i] = true;
  }
}

// ---------------------------------------------------------------------------
void TFo_Main::BrowserArrange_Memory(float *CardX, float *CardY, bool *Norm) {
  for (int i = 0; i < m_Document->m_Cards->Count; i++)
    if (Norm[m_CardAssign[i]]) {
      TCard *Card = m_Document->GetCardByIndex(m_CardAssign[i]);
      CardX[i] = Card->m_fX;
      CardY[i] = Card->m_fY;
    }
}

// ---------------------------------------------------------------------------
void TFo_Main::BrowserArrange_Normalize(float *CardX, float *CardY,
                                        bool *Norm) {
  // Normalize to fit within view
  float maxx = 0.5f, minx = 0.5f, maxy = 0.5f, miny = 0.5f;
  for (int i = 0; i < m_Document->m_Cards->Count; i++)
    if (Norm[m_CardAssign[i]]) {
      TCard *Card = m_Document->GetCardByIndex(m_CardAssign[i]);
      CardX[i] = Card->m_fX;
      CardY[i] = Card->m_fY;

      if (CardX[i] < minx) {
        minx = CardX[i];
      }
      if (CardX[i] > maxx) {
        maxx = CardX[i];
      }
      if (CardY[i] < miny) {
        miny = CardY[i];
      }
      if (CardY[i] > maxy) {
        maxy = CardY[i];
      }
    }

  if (maxx != minx && maxy != miny) {
    for (int i = 0; i < m_Document->m_Cards->Count; i++)
      if (Norm[m_CardAssign[i]]) {
        TCard *Card = m_Document->GetCardByIndex(i);
        CardX[i] = Card->m_fX = (CardX[i] - minx) / (maxx - minx);
        CardY[i] = Card->m_fY = (CardY[i] - miny) / (maxy - miny);
      }
  }
}

// ---------------------------------------------------------------------------
void TFo_Main::BrowserArrangeByNormalize(float ratio) {
  // Update coordinates by repulsion only

  float *CardX = new float[m_Document->m_Cards->Count]; // X coordinate backup
  float *CardY = new float[m_Document->m_Cards->Count]; // Y coordinate backup
  bool *Norm = new bool[m_Document->m_Cards->Count];    // Normalization target
  memset(Norm, 0, sizeof(bool) * m_Document->m_Cards->Count);

  BrowserArrange_Initialize(CardX, CardY, Norm);

  // Normalize to fit within view
  for (int i = 0; i < m_Document->m_Cards->Count; i++)
    if (m_CardVisible[i]) {
      Norm[i] = true;
    }
  BrowserArrange_Normalize(CardX, CardY, Norm);

  delete[] Norm;
  delete[] CardX;
  delete[] CardY;
}

// ---------------------------------------------------------------------------
void TFo_Main::BrowserArrangeByRepulsion(float ratio) {
  // Update coordinates by repulsion only

  float *CardX = new float[m_Document->m_Cards->Count]; // X coordinate backup
  float *CardY = new float[m_Document->m_Cards->Count]; // Y coordinate backup
  bool *Norm = new bool[m_Document->m_Cards->Count];    // Normalization target
  memset(Norm, 0, sizeof(bool) * m_Document->m_Cards->Count);

  BrowserArrange_Initialize(CardX, CardY, Norm);

  // Update coordinates

  // Repel from other cards
  // Card loop
  for (int i = 0; i < m_Document->m_Cards->Count; i++)
    if (m_CardVisible[i]) {
      TCard *Card = m_Document->GetCardByIndex(i);

      // Repel from other cards
      BrowserArrange_Repulsion(i, Card, CardX, CardY, Norm, ratio);
    }

  // Normalize to fit within view
  BrowserArrange_Normalize(CardX, CardY, Norm);

  delete[] Norm;
  delete[] CardX;
  delete[] CardY;
}

// ---------------------------------------------------------------------------
void TFo_Main::BrowserArrangeByLink(float ratio) {
  // Update coordinates by link

  float *CardX = new float[m_Document->m_Cards->Count]; // X coordinate backup
  float *CardY = new float[m_Document->m_Cards->Count]; // Y coordinate backup
  bool *Norm = new bool[m_Document->m_Cards->Count];    // Normalization target
  memset(Norm, 0, sizeof(bool) * m_Document->m_Cards->Count);

  BrowserArrange_Initialize(CardX, CardY, Norm);

  // Update coordinates

  // Repel from other cards
  // Card loop
  for (int i = 0; i < m_Document->m_Cards->Count; i++)
    if (m_CardVisible[i]) {
      TCard *Card = m_Document->GetCardByIndex(i);

      // Repel from other cards
      BrowserArrange_Repulsion(i, Card, CardX, CardY, Norm, 0.5f);
    }

  // Tentative coordinate update
  BrowserArrange_Normalize(CardX, CardY, Norm);

  // Move toward linked cards
  // Card loop
  for (int i = 0; i < m_Document->m_Cards->Count; i++)
    if (m_CardVisible[i]) {
      TCard *Card = m_Document->GetCardByIndex(i);

      // Move toward linked cards
      BrowserArrange_Link(i, Card, CardX, CardY, Norm, ratio * 0.66);
    }

  // Normalize to fit within view
  BrowserArrange_Normalize(CardX, CardY, Norm);

  delete[] Norm;
  delete[] CardX;
  delete[] CardY;
}

// ---------------------------------------------------------------------------
void TFo_Main::BrowserArrangeByLabel(float ratio) {
  // Update coordinates by label

  float *CardX = new float[m_Document->m_Cards->Count]; // X coordinate backup
  float *CardY = new float[m_Document->m_Cards->Count]; // Y coordinate backup
  bool *Norm = new bool[m_Document->m_Cards->Count];    // Normalization target
  memset(Norm, 0, sizeof(bool) * m_Document->m_Cards->Count);

  BrowserArrange_Initialize(CardX, CardY, Norm);

  // Update coordinates

  // Card loop
  // Repel from other cards
  for (int i = 0; i < m_Document->m_Cards->Count; i++)
    if (m_CardVisible[i]) {
      TCard *Card = m_Document->GetCardByIndex(i);

      // Repel from other cards
      BrowserArrange_Repulsion(i, Card, CardX, CardY, Norm, 0.5f);
    }

  // Tentative coordinate update
  BrowserArrange_Normalize(CardX, CardY, Norm);

  // Move toward linked cards
  // Card loop
  for (int i = 0; i < m_Document->m_Cards->Count; i++)
    if (m_CardVisible[i]) {
      TCard *Card = m_Document->GetCardByIndex(i);

      // Move toward linked cards
      BrowserArrange_Link(i, Card, CardX, CardY, Norm, ratio / 4);
    }

  // Tentative coordinate update
  BrowserArrange_Normalize(CardX, CardY, Norm);

  // Same labels near, different labels far
  BrowserArrange_LabelPrepare();
  // Card loop
  for (int i = 0; i < m_Document->m_Cards->Count; i++)
    if (m_CardVisible[i]) {
      TCard *Card = m_Document->GetCardByIndex(i);

      // Same labels near, different labels far
      BrowserArrange_Label(i, Card, CardX, CardY, Norm, ratio / 3);
    }

  // Normalize to fit within view
  BrowserArrange_Normalize(CardX, CardY, Norm);

  delete[] Norm;
  delete[] CardX;
  delete[] CardY;
}

// ---------------------------------------------------------------------------
void TFo_Main::BrowserArrangeByIndex(float ratio) {
  // Update coordinates by card order

  float *CardX = new float[m_Document->m_Cards->Count]; // X coordinate backup
  float *CardY = new float[m_Document->m_Cards->Count]; // Y coordinate backup
  bool *Norm = new bool[m_Document->m_Cards->Count];    // Normalization target
  memset(Norm, 0, sizeof(bool) * m_Document->m_Cards->Count);

  BrowserArrange_Initialize(CardX, CardY, Norm);

  // Update coordinates

  // Update coordinates

  // Card loop
  // Repel from other cards
  for (int i = 0; i < m_Document->m_Cards->Count; i++)
    if (m_CardVisible[i]) {
      TCard *Card = m_Document->GetCardByIndex(i);

      // Repel from other cards
      BrowserArrange_Repulsion(i, Card, CardX, CardY, Norm, 0.5f);
    }

  // Tentative coordinate update
  BrowserArrange_Normalize(CardX, CardY, Norm);

  // Card loop
  for (int i = 0; i < m_Document->m_Cards->Count; i++)
    if (m_CardVisible[i]) {
      TCard *Card = m_Document->GetCardByIndex(i);

      // Closer cards by index
      BrowserArrange_Index(i, Card, CardX, CardY, Norm, ratio * 0.61f);
    }

  // Normalize to fit within view
  BrowserArrange_Normalize(CardX, CardY, Norm);

  delete[] Norm;
  delete[] CardX;
  delete[] CardY;
}

// ---------------------------------------------------------------------------
void TFo_Main::BrowserArrangeByDate(int dateindex, float ratio) {
  // Update coordinates by date

  float *CardX = new float[m_Document->m_Cards->Count]; // X coordinate backup
  float *CardY = new float[m_Document->m_Cards->Count]; // Y coordinate backup
  bool *Norm = new bool[m_Document->m_Cards->Count];    // Normalization target
  memset(Norm, 0, sizeof(bool) * m_Document->m_Cards->Count);

  BrowserArrange_Initialize(CardX, CardY, Norm);

  // Update coordinates

  // Card loop
  for (int i = 0; i < m_Document->m_Cards->Count; i++) {
    TCard *Card = m_Document->GetCardByIndex(i);

    if (Card->m_nID != m_nTargetCard) {
      // Movable card

      // Repel from other cards
      BrowserArrange_Repulsion(i, Card, CardX, CardY, Norm, ratio / 3);

      // Move toward linked cards
      BrowserArrange_Link(i, Card, CardX, CardY, Norm, ratio / 3);

      // Closer dates
      BrowserArrange_Date(i, Card, CardX, CardY, Norm, ratio / 3, dateindex);
    }
  }

  // Normalize to fit within view
  BrowserArrange_Normalize(CardX, CardY, Norm);

  delete[] Norm;
  delete[] CardX;
  delete[] CardY;
}

// ---------------------------------------------------------------------------
void TFo_Main::BrowserArrangeByFold(float ratio) {
  // Arrange by folded labels
  // Called when label Fold changes, not normal Arrange

  float *CardX = new float[m_Document->m_Cards->Count]; // X coordinate backup
  float *CardY = new float[m_Document->m_Cards->Count]; // Y coordinate backup
  bool *Norm = new bool[m_Document->m_Cards->Count];    // Normalization target
  memset(Norm, 0, sizeof(bool) * m_Document->m_Cards->Count);
  float *Sum = new float[m_Document->m_Cards->Count]; // Y coordinate backup
  memset(Sum, 0, sizeof(float) * m_Document->m_Cards->Count);
  float *LabelX = new float[m_Document->m_Labels[0]->Count]; // X coordinate
  memset(LabelX, 0, sizeof(float) * m_Document->m_Labels[0]->Count);
  float *LabelY = new float[m_Document->m_Labels[0]->Count]; // Y coordinate
  memset(LabelY, 0, sizeof(float) * m_Document->m_Labels[0]->Count);
  float *LabelSum =
      new float[m_Document->m_Labels[0]->Count]; // Y coordinate backup
  memset(LabelSum, 0, sizeof(float) * m_Document->m_Labels[0]->Count);

  BrowserArrange_Initialize(CardX, CardY, Norm);

  // Update coordinates

  // Calculate label center coordinates
  // Label loop
  for (int il = 0; il < m_Document->m_Labels[0]->Count; il++) {
    TCardLabel *Label = m_Document->GetLabelByIndex(0, il);
    if (Label->m_bEnable && Label->m_bFold) {
      // Folded label

      // Card loop
      for (int i = 0; i < m_Document->m_Cards->Count; i++)
        if (m_CardVisible[i]) {
          TCard *Card = m_Document->GetCardByIndex(i);

          if (Card->m_Labels->Contain(il + 1)) {
            // Contains this label
            LabelX[il] += CardX[i] / Card->m_Labels->Count;
            LabelY[il] += CardY[i] / Card->m_Labels->Count;
            LabelSum[il] += 1.0f / Card->m_Labels->Count;
          }
        }
      if (LabelSum[il] > 0.0f) {
        LabelX[il] /= LabelSum[il];
        LabelY[il] /= LabelSum[il];
      }
    }
  }

  // Reset card coordinates
  for (int i = 0; i < m_Document->m_Cards->Count; i++)
    if (m_CardVisible[i]) {
      CardX[i] = 0.0f;
      CardY[i] = 0.0f;
    }

  // Move cards to label center coordinates
  // Label loop
  for (int il = 0; il < m_Document->m_Labels[0]->Count; il++) {
    TCardLabel *Label = m_Document->GetLabelByIndex(0, il);
    if (Label->m_bEnable && Label->m_bFold) {
      // Folded label

      // Card loop
      for (int i = 0; i < m_Document->m_Cards->Count; i++)
        if (m_CardVisible[i]) {
          TCard *Card = m_Document->GetCardByIndex(i);

          if (Card->m_Labels->Contain(il + 1)) {
            // Contains this label

            if (m_Document->LabelIsFold(Card)) {
              // All folded
              CardX[i] += LabelX[il] / Card->m_Labels->Count;
              CardY[i] += LabelY[il] / Card->m_Labels->Count;
              Sum[i] += 1.0f / Card->m_Labels->Count;
            }
          }
        }
    }
  }

  // Card loop
  for (int i = 0; i < m_Document->m_Cards->Count; i++)
    if (m_CardVisible[i]) {
      if (Sum[i] > 0.0f) {
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

// ---------------------------------------------------------------------------
void TFo_Main::BrowserArrangeBySimilarity(float ratio) {
  float *CardX = new float[m_Document->m_Cards->Count]; // X coordinate backup
  float *CardY = new float[m_Document->m_Cards->Count]; // Y coordinate backup
  bool *Norm = new bool[m_Document->m_Cards->Count];    // Normalization target
  memset(Norm, 0, sizeof(bool) * m_Document->m_Cards->Count);

  BrowserArrange_Initialize(CardX, CardY, Norm);

  // Update coordinates

  // Repel from other cards
  // Card loop
  for (int i = 0; i < m_Document->m_Cards->Count; i++)
    if (m_CardVisible[i]) {
      TCard *Card = m_Document->GetCardByIndex(i);

      // Repel from other cards
      BrowserArrange_Repulsion(i, Card, CardX, CardY, Norm, 0.5f);
    }

  // Tentative coordinate update
  BrowserArrange_Normalize(CardX, CardY, Norm);

  // Move toward similar cards
  if (m_SimMatrix != NULL) {
    // Similarity exists

    int *idxtable = new int[m_Document->m_Cards->Count];
    for (int i = 0; i < m_Document->m_Cards->Count; i++)
      if (m_CardVisible[i]) {
        TCard *Card = m_Document->GetCardByIndex(i);
        idxtable[i] = m_SimMatrix->SearchIndex(Card->m_nID);
      }
    // Card loop
    for (int i = 0; i < m_Document->m_Cards->Count; i++)
      if (m_CardVisible[i]) {
        TCard *Card = m_Document->GetCardByIndex(i);

        // Move toward similar cards
        BrowserArrange_Similarity(i, Card, CardX, CardY, Norm, ratio * 0.8,
                                  idxtable);
      }
    delete[] idxtable;
  }

  // Normalize to fit within view
  BrowserArrange_Normalize(CardX, CardY, Norm);

  delete[] Norm;
  delete[] CardX;
  delete[] CardY;
}

// ---------------------------------------------------------------------------
void TFo_Main::PrepareMatrixArrange_AssignToMatrix2(int i, int *tmatrix,
                                                    float minx, float maxx,
                                                    float miny, float maxy) {
  // Assign i-th card to matrix indicated by tmatrix

  TCard *Card = m_Document->GetCardByIndex(i);
  float normx = (Card->m_fX - minx) / (maxx - minx);
  float normy = (Card->m_fY - miny) / (maxy - miny);

  // Convert circle centered at 0.5 to rect coordinates
  {
    float x1 = normx - 0.5f;
    float y1 = normy - 0.5f;
    float rad = fmod(atan2(y1, x1) + 2 * M_PI, M_PI / 2); // 0~PI/2
    if (rad > M_PI / 4) {
      rad = M_PI / 2 - rad;
    } // 0~PI/4
    float ncoef = 1.0 / cos(rad);
    normx = x1 * ncoef + 0.5f;
    normy = y1 * ncoef + 0.5f;
  }

  // Find nearest empty cell in grid
  int x = -1, y = -1;
  float mindistance = m_nMatrixWidth + m_nMatrixHeight;
  // Grid loop
  for (int iy = 0; iy < m_nMatrixHeight; iy++) {
    for (int ix = 0; ix < m_nMatrixWidth; ix++) {
      if (tmatrix[ix + iy * m_nMatrixWidth] == -1) {
        // Empty
        float distance = fabs(normx * (m_nMatrixWidth - 1) - ix) +
                         fabs(normy * (m_nMatrixHeight - 1) - iy);
        if (distance < mindistance) {
          // Closer cell
          mindistance = distance;
          x = ix;
          y = iy;
        }
      }
    }
  }

  // Store in cell
  if (x >= 0 && y >= 0) {
    tmatrix[x + y * m_nMatrixWidth] = i;
    if (Card->m_nMatrixX != x || Card->m_nMatrixY != y) {
      Card->m_nMatrixX = x;
      Card->m_nMatrixY = y;
      Card->m_fMatrixSpeed = 0.0f;
    }
  }
}

// ---------------------------------------------------------------------------
void TFo_Main::PrepareMatrixArrange_AssignToMatrix(int i, int *tmatrix,
                                                   float minx, float maxx,
                                                   float miny, float maxy) {
  // Assign i-th card to matrix indicated by tmatrix

  TCard *Card = m_Document->GetCardByIndex(i);
  float normx = (Card->m_fX - minx) / (maxx - minx);
  float normy = (Card->m_fY - miny) / (maxy - miny);

  // Find nearest empty cell in grid
  int x = -1, y = -1;
  float mindistance = m_nMatrixWidth + m_nMatrixHeight;
  // Grid loop
  for (int iy = 0; iy < m_nMatrixHeight; iy++) {
    for (int ix = 0; ix < m_nMatrixWidth; ix++) {
      if (tmatrix[ix + iy * m_nMatrixWidth] == -1) {
        // Empty
        float distance = fabs(normx * (m_nMatrixWidth - 1) - ix) +
                         fabs(normy * (m_nMatrixHeight - 1) - iy);
        if (distance < mindistance) {
          // Closer cell
          mindistance = distance;
          x = ix;
          y = iy;
        }
      }
    }
  }

  // Store in cell
  if (x >= 0 && y >= 0) {
    tmatrix[x + y * m_nMatrixWidth] = i;
    if (Card->m_nMatrixX != x || Card->m_nMatrixY != y) {
      Card->m_nMatrixX = x;
      Card->m_nMatrixY = y;
      Card->m_fMatrixSpeed = 0.0f;
    }
  }
}

// ---------------------------------------------------------------------------
void TFo_Main::PrepareMatrixArrange(int type) {
  // Determine grid size, place all cards in grid. Runs when Arrange is selected
  if (!m_CardVisible) {
    return;
  }

  /*
   float *CardX, *CardY;
   bool *Norm;
   if (type % 100 != 0){
   // Backup card positions
   CardX = new float[m_Document->m_Cards->Count];
   CardY = new float[m_Document->m_Cards->Count];
   Norm = new bool[m_Document->m_Cards->Count];// Normalization target
   memset(Norm, 0, sizeof(bool) * m_Document->m_Cards->Count);

   BrowserArrange_Initialize(CardX, CardY, Norm);

   // Update coordinates
   switch(type){
   case 202:
   {
   float *CardX2 = new float[m_Document->m_Cards->Count];
   float *CardY2 = new float[m_Document->m_Cards->Count];

   memcpy(CardX2, CardX, sizeof(float) * m_Document->m_Cards->Count);
   memcpy(CardY2, CardY, sizeof(float) * m_Document->m_Cards->Count);

   for (int il = 0 ; il < 5 ; il++){
   // Move toward linked cards
   // Card loop
   for (int i = 0 ; i < m_Document->m_Cards->Count ; i++) if (m_CardVisible[i]){
   TCard *Card = m_Document->GetCardByIndex(i);

   // Move toward linked cards
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
   // Move toward cards with same label
   BrowserArrange_LabelPrepare();
   // Card loop
   for (int i = 0 ; i < m_Document->m_Cards->Count ; i++) if (m_CardVisible[i]){
   TCard *Card = m_Document->GetCardByIndex(i);

   // Move toward cards with same label
   BrowserArrange_Label(i, Card, CardX, CardY, Norm, 1.0f);
   }
   }
   break;
   case 700:
   {
   // Move toward similar cards
   int *idxtable = new int[m_Document->m_Cards->Count];
   for (int i = 0 ; i < m_Document->m_Cards->Count ; i++) if (m_CardVisible[i]){
   TCard *Card = m_Document->GetCardByIndex(i);
   idxtable[i] = m_SimMatrix->SearchIndex(Card->m_nID);
   }
   // Card loop
   for (int i = 0 ; i < m_Document->m_Cards->Count ; i++) if (m_CardVisible[i]){
   TCard *Card = m_Document->GetCardByIndex(i);

   // Move toward similar cards
   BrowserArrange_Similarity(i, Card, CardX, CardY, Norm, 1.0f, idxtable);
   }
   delete[] idxtable;
   }
   break;
   }
   }
   */

  // Count visible cards
  int count = 0;
  float minx = 0.0f, maxx = 1.0f, miny = 0.0f, maxy = 1.0f;
  for (int i = 0; i < m_Document->m_Cards->Count; i++)
    if (m_CardAssign[i] == i) {
      TCard *Card = m_Document->GetCardByIndex(i);
      if (Card->m_bVisible) {
        if (count == 0 || minx > Card->m_fX) {
          minx = Card->m_fX;
        }
        if (count == 0 || maxx < Card->m_fX) {
          maxx = Card->m_fX;
        }
        if (count == 0 || miny > Card->m_fY) {
          miny = Card->m_fY;
        }
        if (count == 0 || maxy < Card->m_fY) {
          maxy = Card->m_fY;
        }
        count++;
      }
    }
  if (minx == maxx) {
    maxx += 1.0f;
  }
  if (miny == maxy) {
    maxy += 1.0f;
  }

  // Determine grid aspect ratio
  float xyratio = 1.0f; // Larger = wider
  float widthsum = 0.0f;
  float heightsum = 0.0f;
  for (int i = 0; i < m_Document->m_Cards->Count; i++)
    if (m_CardVisible[i] && m_CardAssign[i] == i) {
      widthsum += m_CardWidth[i];
      heightsum += m_CardHeight[i];
    }
  if (heightsum > 0 && widthsum > 0) {
    widthsum /= m_nBrowserWidth;
    heightsum /= m_nBrowserHeight;
    xyratio = sqrt(heightsum * 1.0f / widthsum);
  }

  // Determine grid size
  m_nMatrixWidth = 1;
  m_nMatrixHeight = 1;
  while (count > m_nMatrixWidth * m_nMatrixHeight) {
    if (m_nMatrixWidth <= m_nMatrixHeight * xyratio) {
      m_nMatrixWidth += 2;
    } else {
      m_nMatrixHeight += 2;
    }
  }

  if (type != 204) {
    // Place in adjacent cells

    // Create and initialize temp matrix
    int *tmatrix = new int[m_nMatrixWidth * m_nMatrixHeight];
    for (int i = 0; i < m_nMatrixWidth * m_nMatrixHeight; i++) {
      tmatrix[i] = -1;
    }

    // Place cards in matrix (Fixed -> selected -> related -> others)
    int *draworder = new int[m_Document->m_Cards->Count];
    memset(draworder, 0, sizeof(int) * m_Document->m_Cards->Count);
    for (int i = 0; i < m_Document->m_Cards->Count; i++) {
      int vo = m_Document->GetCardByIndex(i)->m_nViewedOrder;
      draworder[vo] = i;
    }
    int targetcardindex = m_Document->SearchCardIndex(m_nTargetCard);
    // Fixed cards
    for (int ic = m_Document->m_Cards->Count - 1; ic >= 0; ic--) {
      int i = draworder[ic];
      TCard *Card = m_Document->GetCardByIndex(i);
      if (m_CardVisible[i] && m_CardAssign[i] == i && Card->m_bFixed &&
          i != targetcardindex) {
        if (type % 100 == 0) {
          PrepareMatrixArrange_AssignToMatrix(i, tmatrix, minx, maxx, miny,
                                              maxy);
        } else {
          PrepareMatrixArrange_AssignToMatrix2(i, tmatrix, minx, maxx, miny,
                                               maxy);
        }
      }
    }
    // Place selected card
    if (m_nTargetCard >= 0) {
      int i = targetcardindex;
      if (i >= 0)
        if (m_CardVisible[i] && m_CardAssign[i] == i) {
          if (type % 100 == 0) {
            PrepareMatrixArrange_AssignToMatrix(i, tmatrix, minx, maxx, miny,
                                                maxy);
          } else {
            PrepareMatrixArrange_AssignToMatrix2(i, tmatrix, minx, maxx, miny,
                                                 maxy);
          }
        }
    }
    // Related cards
    for (int i = m_Document->m_Cards->Count - 1; i >= 0; i--) {
      TCard *Card = m_Document->GetCardByIndex(i);
      if (m_CardVisible[i] && m_CardRelated[i] && m_CardAssign[i] == i &&
          i != targetcardindex && !Card->m_bFixed) {
        if (type % 100 == 0) {
          PrepareMatrixArrange_AssignToMatrix(i, tmatrix, minx, maxx, miny,
                                              maxy);
        } else {
          PrepareMatrixArrange_AssignToMatrix2(i, tmatrix, minx, maxx, miny,
                                               maxy);
        }
      }
    }
    // Other cards
    for (int ic = m_Document->m_Cards->Count - 1; ic >= 0; ic--) {
      int i = draworder[ic];
      TCard *Card = m_Document->GetCardByIndex(i);
      if (m_CardVisible[i] && !m_CardRelated[i] && m_CardAssign[i] == i &&
          i != targetcardindex && !Card->m_bFixed) {
        if (type % 100 == 0) {
          PrepareMatrixArrange_AssignToMatrix(i, tmatrix, minx, maxx, miny,
                                              maxy);
        } else {
          PrepareMatrixArrange_AssignToMatrix2(i, tmatrix, minx, maxx, miny,
                                               maxy);
        }
      }
    }
    delete[] draworder;

    delete[] tmatrix;
  } else {
    // Place in index order
    int index = 0;
    for (int i = 0; i < m_Document->m_Cards->Count; i++) {
      TCard *Card = m_Document->GetCardByIndex(i);
      if (m_CardVisible[i] && m_CardAssign[i] == i && !Card->m_bFixed) {
        Card->m_nMatrixX = index / m_nMatrixHeight;
        Card->m_nMatrixY = index % m_nMatrixHeight;
        index++;
      }
    }
  }

  /*
   if (type % 100 != 0){
   // Restore coordinates
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

// ---------------------------------------------------------------------------
void TFo_Main::BrowserArrangeByMatrix(int type, float ratio) {
  switch (type) {
  case 200:
    PrepareMatrixArrange(type);
    break;
  case 202:
    // Link
    PrepareMatrixArrange(type);
    break;
  case 203:
    // Label
    break;
  case 204:
    PrepareMatrixArrange(type);
    break;
  case 700:
    // Similarity
    break;
  }

  // Place in grid
  for (int i = 0; i < m_Document->m_Cards->Count; i++)
    if (m_CardVisible[i]) {
      TCard *Card = m_Document->GetCardByIndex(i);

      if (!Card->m_bFixed && (!Card->m_bSelected || !m_bMDownBrowser)) {
        Card->m_fMatrixSpeed += 0.1f;
        if (Card->m_fMatrixSpeed > 0.5f) {
          Card->m_fMatrixSpeed = 0.5f;
        }

        float targetx = 0.5f;
        if (m_nMatrixWidth > 1) {
          targetx = Card->m_nMatrixX * 1.0f / (m_nMatrixWidth - 1);
        }
        float targety = 0.5f;
        if (m_nMatrixHeight > 1) {
          targety = Card->m_nMatrixY * 1.0f / (m_nMatrixHeight - 1);
        }

        Card->m_fX = Card->m_fX * (1.0f - Card->m_fMatrixSpeed) +
                     targetx * Card->m_fMatrixSpeed;
        Card->m_fY = Card->m_fY * (1.0f - Card->m_fMatrixSpeed) +
                     targety * Card->m_fMatrixSpeed;
      }
    }

  for (int i = 0; i < m_Document->m_Cards->Count; i++)
    if (m_CardAssign[i] != i) {
      TCard *Card = m_Document->GetCardByIndex(m_CardAssign[i]);
      TCard *Card2 = m_Document->GetCardByIndex(i);

      Card2->m_fX = Card->m_fX;
      Card2->m_fY = Card->m_fY;
    }
}

// ---------------------------------------------------------------------------
void TFo_Main::BrowserArrangeByTree(int type, float ratio) {
  // Hierarchy view

  // Coordinate backup for fixed positions
  float *xbak = new float[m_Document->m_Cards->Count];
  float *ybak = new float[m_Document->m_Cards->Count];
  for (int i = 0; i < m_Document->m_Cards->Count; i++)
    if (m_CardVisible[i]) {
      TCard *Card = m_Document->GetCardByIndex(i);
      xbak[i] = Card->m_fX;
      ybak[i] = Card->m_fY;
    }

  int *nodegroup = new int[m_Document->m_Cards->Count];

  // Count visible cards
  int vcount = 1;
  for (int i = 0; i < m_Document->m_Cards->Count; i++)
    if (m_CardVisible[i]) {
      vcount++;
    }
  float xspan = 1.0f / sqrt(vcount);
  float yspan = 1.0f / sqrt(vcount);

  ratio *= 0.25f;

  // Build tree for each visible root node

  int *nodeheight = new int[m_Document->m_Cards->Count];
  memset(nodeheight, 0, sizeof(int) * m_Document->m_Cards->Count);

  // Card loop (root)
  int group = 0;
  for (int i = 0; i < m_Document->m_Cards->Count; i++)
    if (m_CardVisible[i]) {
      TCard *Root = m_Document->GetCardByIndex(i);
      if (Root->m_nLevel == 0) {
        // Root node
        nodegroup[i] = group;

        // Get max level of this tree
        int maxlevel = 0;
        int endindex = i + 1; // Also get end index of this tree
        while (endindex < m_Document->m_Cards->Count) {
          TCard *Card = m_Document->GetCardByIndex(endindex);
          if (Card->m_nLevel == 0) {
            // Next tree
            break;
          } else if (Card->m_nLevel > maxlevel) {
            maxlevel = Card->m_nLevel;
          }
          nodegroup[endindex] = group;
          endindex++;
        }

        // Get child node sizes
        // Level loop
        for (int il = maxlevel; il >= 0; il--) {
          // Node loop of this tree
          for (int in = i; in < endindex; in++)
            if (m_CardVisible[in]) {
              TCard *Card = m_Document->GetCardByIndex(in);
              if (Card->m_nLevel == il) {
                // Card size = sum of child node counts
                int index = in + 1;
                if (in == i) {
                  in = i;
                }
                while (index < m_Document->m_Cards->Count) {
                  TCard *Card = m_Document->GetCardByIndex(index);
                  if (Card->m_nLevel < il + 1) {
                    // Next node
                    break;
                  } else if (Card->m_nLevel == il + 1) {
                    // Add child node size
                    nodeheight[in] += nodeheight[index];
                  }
                  index++;
                }
                if (nodeheight[in] == 0) {
                  // Min size 1
                  nodeheight[in] = 1;
                }
              }
            }
        }

        // Set coordinates from root
        // Level loop
        for (int il = 1; il <= maxlevel; il++) {
          int heightsum = 0; // Processed size sum for this level

          // Node loop of this tree
          TCard *LastRoot = NULL; // Previous root node
          int lastrootindex = -1;
          int rightcount = 0;
          int rightheight = 0;
          int rightheightsum = 0;
          // Node loop of this tree
          for (int in = i; in < endindex; in++)
            if (m_CardVisible[in]) {
              TCard *Card = m_Document->GetCardByIndex(in);
              if (Card->m_nLevel == il - 1) {
                LastRoot = Card;
                lastrootindex = in;
                heightsum = 0;
                if (il == 1) {
                  // Split left/right
                  int sum = 0;
                  for (int in2 = i; in2 < endindex; in2++)
                    if (m_CardVisible[in2]) {
                      TCard *Card2 = m_Document->GetCardByIndex(in2);
                      if (Card2->m_nLevel == il) {
                        if (abs((sum + nodeheight[in2]) * 2 -
                                nodeheight[lastrootindex]) <=
                            abs(sum * 2 - nodeheight[lastrootindex])) {
                          rightcount++;
                          sum += nodeheight[in2];
                        } else {
                          break;
                        }
                      }
                    }
                  rightheight = sum;
                }
              } else if (Card->m_nLevel == il && LastRoot) {
                if (LastRoot == Root) {
                  // Branch from root
                  if (rightcount-- > 0) {
                    // Display on right
                    Card->m_fX = LastRoot->m_fX + xspan;
                    Card->m_fY =
                        LastRoot->m_fY + (-rightheight * 0.5f + rightheightsum +
                                          nodeheight[in] * 0.5f) *
                                             yspan;
                    rightheightsum += nodeheight[in];
                  } else {
                    // Display on left
                    Card->m_fX = LastRoot->m_fX - xspan;
                    Card->m_fY =
                        LastRoot->m_fY +
                        (-(nodeheight[lastrootindex] - rightheight) * 0.5f +
                         heightsum + nodeheight[in] * 0.5f) *
                            yspan;
                    heightsum += nodeheight[in];
                  }
                } else {
                  // Branch from non-root (child)
                  if (LastRoot->m_fX > Root->m_fX) {
                    Card->m_fX = LastRoot->m_fX + xspan;
                  } else {
                    Card->m_fX = LastRoot->m_fX - xspan;
                  }
                  Card->m_fY =
                      LastRoot->m_fY + (-nodeheight[lastrootindex] * 0.5f +
                                        heightsum + nodeheight[in] * 0.5f) *
                                           yspan;
                  heightsum += nodeheight[in];
                }
              }
            }
        }
        group++;
      }
    }

  delete[] nodeheight;

  // Repel trees to avoid overlap

  if (group > 0) {
    float *groupx = new float[group]; // Root node coordinates
    float *groupy = new float[group];
    float *groupsizex = new float[group]; // Tree size
    float *groupsizey = new float[group];
    float *groupnewx = new float[group]; // Root coords after move
    float *groupnewy = new float[group];

    // Get size of each group (tree)
    // Group loop
    for (int ig = 0; ig < group; ig++) {
      float minx = 0.0f, maxx = 0.0f, miny = 0.0f, maxy = 0.0f;
      bool first = true;
      for (int i = 0; i < m_Document->m_Cards->Count; i++)
        if (m_CardVisible[i] && nodegroup[i] == ig) {
          TCard *Card = m_Document->GetCardByIndex(i);
          if (first) {
            // First (root)
            minx = maxx = Card->m_fX;
            miny = maxy = Card->m_fY;
            first = false;
          } else {
            if (minx > Card->m_fX) {
              minx = Card->m_fX;
            }
            if (maxx < Card->m_fX) {
              maxx = Card->m_fX;
            }
            if (miny > Card->m_fY) {
              miny = Card->m_fY;
            }
            if (maxy < Card->m_fY) {
              maxy = Card->m_fY;
            }
          }
        }
      groupx[ig] = (minx + maxx) * 0.5f;
      groupy[ig] = (miny + maxy) * 0.5f;
      if (fabs(minx - maxx) < 0.01f) {
        // Single-card group?
        groupsizex[ig] = xspan;
        groupsizey[ig] = yspan;
      } else {
        groupsizex[ig] = maxx - minx;
        groupsizey[ig] = maxy - miny;
      }
    }

    // Repel
    bool first = true;
    for (int ig = 0; ig < group; ig++) {
      groupnewx[ig] = 0.0f;
      groupnewy[ig] = 0.0f;
      float weight = 0.0f;
      int count = 0;
      for (int ig2 = 0; ig2 < group; ig2++)
        if (ig != ig2) {
          float fx = groupx[ig] - groupx[ig2];
          float fy = groupy[ig] - groupy[ig2];
          float w = fx * fx + fy * fy;
          if (w > 0.0f) {
            /*
             float wx =
             sqrt(1.0 * groupsizex[ig] * groupsizex[ig2] / w);
             float wy =
             sqrt(1.0 * groupsizey[ig] * groupsizey[ig2] / w);
             */
            float wx = (groupsizex[ig] + groupsizex[ig2]) / w;
            float wy = (groupsizey[ig] + groupsizey[ig2]) / w;

            groupnewx[ig] += fx * wx;
            groupnewy[ig] += fy * wy;
            weight += 1.0f;
            count++;
          }
        }
      if (weight > 0.0f && count > 0) {
        weight /= count * count * 5 / ratio;
        groupnewx[ig] = (float)(groupnewx[ig] * weight);
        groupnewy[ig] = (float)(groupnewy[ig] * weight);
        for (int i = 0; i < m_Document->m_Cards->Count; i++)
          if (m_CardVisible[i] && nodegroup[i] == ig) {
            TCard *Card = m_Document->GetCardByIndex(i);
            Card->m_fX += groupnewx[ig] * 10;
            Card->m_fY += groupnewy[ig] * 10;
          }
      }
    }

    // Normalize
    float minx = 0.0f, maxx = 0.0f, miny = 0.0f, maxy = 0.0f;
    first = true;
    for (int i = 0; i < m_Document->m_Cards->Count; i++)
      if (m_CardVisible[i]) {
        TCard *Card = m_Document->GetCardByIndex(i);
        if (first) {
          minx = maxx = Card->m_fX;
          miny = maxy = Card->m_fY;
          first = false;
        } else {
          if (minx > Card->m_fX) {
            minx = Card->m_fX;
          }
          if (maxx < Card->m_fX) {
            maxx = Card->m_fX;
          }
          if (miny > Card->m_fY) {
            miny = Card->m_fY;
          }
          if (maxy < Card->m_fY) {
            maxy = Card->m_fY;
          }
        }
      }
    if (fabs(minx - maxx) > 0.01f && fabs(miny - maxy) > 0.01f) {
      for (int i = 0; i < m_Document->m_Cards->Count; i++)
        if (m_CardVisible[i]) {
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

  // Restore fixed card positions, smooth move
  for (int i = 0; i < m_Document->m_Cards->Count; i++)
    if (m_CardVisible[i]) {
      TCard *Card = m_Document->GetCardByIndex(i);
      if (Card->m_bFixed ||
          (m_bMDownBrowser > 0 && Card->m_nID == m_nTargetCard)) {
        Card->m_fX = xbak[i];
        Card->m_fY = ybak[i];
      } else {
        Card->m_fX = (Card->m_fX + xbak[i]) * 0.5f;
        Card->m_fY = (Card->m_fY + ybak[i]) * 0.5f;
      }
    }

  // Process folded cards
  for (int i = 0; i < m_Document->m_Cards->Count; i++)
    if (m_CardVisible[i]) {
      if (m_CardAssign[i] != i) {
        TCard *Card = m_Document->GetCardByIndex(i);
        TCard *Card2 = m_Document->GetCardByIndex(m_CardAssign[i]);
        Card->m_fX = Card2->m_fX;
        Card->m_fY = Card2->m_fY;
      }
    }

  delete[] xbak;
  delete[] ybak;
}
// ---------------------------------------------------------------------------

#pragma package(smart_init)
