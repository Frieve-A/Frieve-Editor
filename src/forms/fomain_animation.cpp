// ---------------------------------------------------------------------------
// Random card animation (split from fomain.cpp)

#include <vcl.h>
#pragma hdrstop

#include "fomain.h"
#include "fomain_animation.h"
#include "fomain_filter.h"
#include "forandomanimation.h"
#include "setting.h"

#pragma package(smart_init)

// ---------------------------------------------------------------------------
void __fastcall TFo_Main::MA_RandomFlashClick(TObject *Sender) {
  m_nAnimation = 1;
  m_nAnimationCount = 0;
  PC_Client->ActivePage = TS_Browser;
  SettingView.m_bAnimationPaused = false;

  if (!m_DocBeforeAnimation) {
    m_DocBeforeAnimation = new TDocument(*m_Document);
  }
  m_nTargetCard = -1;
  m_nTargetLink = -1;
  m_Document->ClearCardSelection();

  m_nAnimationBak_ArrangeType = Bu_ArrangeType->Tag;
  m_bAnimationBak_Arrange = SB_Arrange->Down;
  m_bAnimationBak_AutoScroll = SB_AutoScroll->Down;
  m_bAnimationBak_AutoZoom = SB_AutoZoom->Down;

  SB_Arrange->Down = true;
  Bu_ArrangeType->Tag = 102;

  PB_Browser->Tag = 1;
  TB_Zoom->Position = -1000; // All display
  SB_AutoScroll->Down = false;
  Sc_X->Position = 5000;
  Sc_Y->Position = 5000;
  PB_Browser->Tag = 0;

  Fo_RandomAnimation->TB_Zoom->Position = TB_Zoom->Position;
  Fo_RandomAnimation->TB_Zoom->Enabled = true;
  Fo_RandomAnimation->Show();
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::MA_RandomMapClick(TObject *Sender) {
  m_nAnimation = 5;
  m_nAnimationCount = 0;
  PC_Client->ActivePage = TS_Browser;
  SettingView.m_bAnimationPaused = false;

  if (!m_DocBeforeAnimation) {
    m_DocBeforeAnimation = new TDocument(*m_Document);
  }
  m_nTargetCard = -1;
  m_nTargetLink = -1;
  m_Document->ClearCardSelection();

  m_nAnimationBak_ArrangeType = Bu_ArrangeType->Tag;
  m_bAnimationBak_Arrange = SB_Arrange->Down;
  m_bAnimationBak_AutoScroll = SB_AutoScroll->Down;
  m_bAnimationBak_AutoZoom = SB_AutoZoom->Down;

  SB_Arrange->Down = true;
  Bu_ArrangeType->Tag = 2;

  PB_Browser->Tag = 1;
  TB_Zoom->Position = -1000; // All display
  SB_AutoScroll->Down = false;
  Sc_X->Position = 5000;
  Sc_Y->Position = 5000;
  PB_Browser->Tag = 0;

  Fo_RandomAnimation->TB_Zoom->Position = TB_Zoom->Position;
  Fo_RandomAnimation->TB_Zoom->Enabled = true;
  Fo_RandomAnimation->Show();
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::MA_RandomJumpClick(TObject *Sender) {
  m_nAnimation = 4;
  m_nAnimationCount = 3000;
  PC_Client->ActivePage = TS_Browser;
  SettingView.m_bAnimationPaused = false;

  if (!m_DocBeforeAnimation) {
    m_DocBeforeAnimation = new TDocument(*m_Document);
  }
  m_nTargetCard = -1;
  m_nTargetLink = -1;
  m_Document->ClearCardSelection();

  m_nAnimationBak_ArrangeType = Bu_ArrangeType->Tag;
  m_bAnimationBak_Arrange = SB_Arrange->Down;
  m_bAnimationBak_AutoScroll = SB_AutoScroll->Down;
  m_bAnimationBak_AutoZoom = SB_AutoZoom->Down;

  SB_Arrange->Down = false;

  PB_Browser->Tag = 1;
  TB_Zoom->Position = -2000;   // All display
  SB_AutoScroll->Down = false; // No scroll
  SB_AutoZoom->Down = false;   // No auto zoom
  Sc_X->Position = 5000;
  Sc_Y->Position = 5000;
  PB_Browser->Tag = 0;

  for (int i = 0; i < MAXRANDOMCARDS; i++) {
    m_Animation_RC2Idxs[i] = -1;
  }

  Fo_RandomAnimation->TB_Zoom->Position = TB_Zoom->Position;
  Fo_RandomAnimation->TB_Zoom->Enabled = false;
  Fo_RandomAnimation->Show();
}
// ---------------------------------------------------------------------------

void TFo_Main::Animation_RandomCard() {
  if (!Fo_RandomAnimation->Visible) {
    // Animation end
    m_nAnimation = 0;
    if (m_DocBeforeAnimation) {
      m_Document->CopyFrom(m_DocBeforeAnimation);
      delete m_DocBeforeAnimation;
      m_DocBeforeAnimation = NULL;
      SB_Arrange->Down = m_bAnimationBak_Arrange;
    } else {
      SB_Arrange->Down = true;
    }
    Bu_ArrangeType->Tag = m_nAnimationBak_ArrangeType;
    SB_AutoScroll->Down = m_bAnimationBak_AutoScroll;
    SB_AutoZoom->Down = m_bAnimationBak_AutoZoom;
    // MM_Menu->Enabled = true;// Disable edit during Animation
    Redraw();
    return;
  }

  // Random Card
  if (m_nAnimationCount == 0) {
    m_Document->ClearCardSelection();

    SetCardAssign();

    // All display
    for (int i = 0; i < m_Document->m_Cards->Count; i++)
      if (m_CardAssign[i] == i) {
        m_Document->GetCardByIndex(i)->m_bVisible = true;
      }
    for (int i = 0; i < m_Document->m_Links->Count; i++) {
      m_Document->GetLinkByIndex(i)->m_bVisible = true;
    }

    FilteringCard();

    // Count displayed cards
    int count = 0;
    for (int i = 0; i < m_Document->m_Cards->Count; i++) {
      if (m_Document->GetCardByIndex(i)->m_bVisible) {
        count++;
      }
    }

    // Hide until count below specified
    while (count > SettingView.m_nAnimationRCCards) {
      int cardindex = rand() % m_Document->m_Cards->Count;
      TCard *Card = m_Document->GetCardByIndex(cardindex);
      if (Card->m_bVisible) {
        Card->m_bVisible = false;
        count--;
      }
    }

    // Set random card active (for read-aloud)
    // Count displayed cards
    if (count > 0) {
      int idx = rand() % count;
      for (int i = 0; i < m_Document->m_Cards->Count; i++) {
        if (m_Document->GetCardByIndex(i)->m_bVisible) {
          if (idx == 0) {
            TCard *Card = m_Document->GetCardByIndex(i);
            m_nTargetCard = Card->m_nID;
            Card->m_bSelected = true;
            break;
          }
          idx--;
        }
      }
    }
  }
  m_nAnimationCount += Ti_Check->Interval;

  if (!SettingView.m_bAnimationPaused &&
      m_nAnimationCount >=
          3000 * pow(2.0, -(SettingView.m_nAnimationRCSpeed - 30) / 25.0)) {
    m_nAnimationCount = 0;
  }
}

// ---------------------------------------------------------------------------
void TFo_Main::Animation_RandomMap() {
  if (!Fo_RandomAnimation->Visible) {
    // Animation end
    m_nAnimation = 0;
    if (m_DocBeforeAnimation) {
      m_Document->CopyFrom(m_DocBeforeAnimation);
      delete m_DocBeforeAnimation;
      m_DocBeforeAnimation = NULL;
      SB_Arrange->Down = m_bAnimationBak_Arrange;
    } else {
      SB_Arrange->Down = true;
    }
    Bu_ArrangeType->Tag = m_nAnimationBak_ArrangeType;
    SB_AutoScroll->Down = m_bAnimationBak_AutoScroll;
    SB_AutoZoom->Down = m_bAnimationBak_AutoZoom;
    Redraw();
    return;
  }

  // Random Map
  if (m_nAnimationCount == 0) {
    // Map update timing

    // Count displayed cards
    SetCardAssign();
    int count = 0;
    for (int i = 0; i < m_Document->m_Cards->Count; i++)
      if (m_CardVisible[i] && m_CardAssign[i] == i) {
        count++;
      }

    // Backup card/link visibility
    bool *CVBak = new bool[m_Document->m_Cards->Count];
    bool *CVNext = new bool[m_Document->m_Cards->Count];
    memcpy(CVBak, m_CardVisible, sizeof(bool) * m_Document->m_Cards->Count);
    memcpy(CVNext, m_CardVisible, sizeof(bool) * m_Document->m_Cards->Count);
    bool *LVBak = new bool[m_Document->m_Links->Count];
    bool *LVNext = new bool[m_Document->m_Links->Count];
    memcpy(LVBak, m_LinkVisible, sizeof(bool) * m_Document->m_Links->Count);
    memcpy(LVNext, m_LinkVisible, sizeof(bool) * m_Document->m_Links->Count);

    // All display
    for (int i = 0; i < m_Document->m_Cards->Count; i++) {
      m_Document->GetCardByIndex(i)->m_bVisible = true;
    }
    for (int i = 0; i < m_Document->m_Links->Count; i++) {
      m_Document->GetLinkByIndex(i)->m_bVisible = true;
    }

    // Filter cards/links to display
    FilteringCard();

    int inccardnum = 0;
    if (m_nTargetCard >= 0) {
      // If card displayed, extend links from there
      for (int i = 0; i < m_Document->m_Links->Count &&
                      count < SettingView.m_nAnimationRCCards;
           i++) {
        TLink *Link = m_Document->GetLinkByIndex(i);
        if (Link->m_bVisible) { // Link to display
          int fromidx = m_Document->SearchCardIndex(Link->m_nFromID);
          if (m_CardVisible[fromidx]) { // Displayed = display target
            int destidx = m_Document->SearchCardIndex(Link->m_nDestID);
            TCard *Dest = m_Document->GetCardByIndex(destidx);
            if (!m_CardVisible[destidx] &&
                Dest->m_bVisible) { // Not displayed but is display target
              CVNext[destidx] = true;
              LVNext[i] = true;
              TCard *From = m_Document->GetCardByIndex(fromidx);
              Dest->m_fX = From->m_fX;
              Dest->m_fY = From->m_fY;
              if (m_CardAssign[destidx] == destidx) {
                count++;
                inccardnum++;
              }
            }
          }
        }
      }
    }
    if (m_nTargetCard < 0 || inccardnum == 0) {
      // No card selected or no card added; select one new card
      m_Document->ClearCardSelection();

      /*
       // All display
       for (int i = 0 ; i < m_Document->m_Cards->Count ; i++) if
       (m_CardAssign[i] == i){ m_Document->GetCardByIndex(i)->m_bVisible = true;
       }

       FilteringCard();
       */

      // Count displayed cards
      int count = 0;
      for (int i = 0; i < m_Document->m_Cards->Count; i++) {
        if (m_Document->GetCardByIndex(i)->m_bVisible) {
          count++;
        }
      }

      // Determine display index
      int idx = rand() % count;
      for (int i = 0; i < m_Document->m_Cards->Count; i++) {
        TCard *Card = m_Document->GetCardByIndex(i);
        if (Card->m_bVisible) {
          if (idx == 0) {
            m_nTargetCard = Card->m_nID;
            Card->m_bSelected = true;
            Card->m_fX = 0.5f;
            Card->m_fY = 0.5f;
          } else {
            Card->m_bVisible = false;
          }
          idx--;
        }
      }
    } else {
      // Reflect new visibility
      for (int i = 0; i < m_Document->m_Cards->Count; i++) {
        m_Document->GetCardByIndex(i)->m_bVisible = CVNext[i];
      }
      for (int i = 0; i < m_Document->m_Links->Count; i++) {
        m_Document->GetLinkByIndex(i)->m_bVisible = LVNext[i];
      }
    }

    delete[] LVNext;
    delete[] LVBak;
    delete[] CVNext;
    delete[] CVBak;
  }
  m_nAnimationCount += Ti_Check->Interval;

  if (!SettingView.m_bAnimationPaused &&
      m_nAnimationCount >=
          2000 * pow(2.0, -(SettingView.m_nAnimationRCSpeed - 30) / 25.0)) {
    m_nAnimationCount = 0;
  }
}

// ---------------------------------------------------------------------------
void __fastcall TFo_Main::MA_RandomScrollClick(TObject *Sender) {
  m_nAnimation = 2;
  m_nAnimationCount = 3000;
  PC_Client->ActivePage = TS_Browser;
  SettingView.m_bAnimationPaused = false;

  if (!m_DocBeforeAnimation) {
    m_DocBeforeAnimation = new TDocument(*m_Document);
  }
  m_nTargetCard = -1;
  m_nTargetLink = -1;
  m_Document->ClearCardSelection();

  m_nAnimationBak_ArrangeType = Bu_ArrangeType->Tag;
  m_bAnimationBak_Arrange = SB_Arrange->Down;
  m_bAnimationBak_AutoScroll = SB_AutoScroll->Down;
  m_bAnimationBak_AutoZoom = SB_AutoZoom->Down;
  // MM_Menu->Enabled = false;// Disable edit during Animation

  // No Arrange
  SB_Arrange->Down = false;

  PB_Browser->Tag = 1;
  TB_Zoom->Position = -2000;   // All display
  SB_AutoScroll->Down = false; // No scroll
  SB_AutoZoom->Down = false;   // No auto zoom
  Sc_X->Position = 5000;
  Sc_Y->Position = 5000;
  /*
   if (!SB_AutoScroll->Down){
   Sc_X->Position = 5000;
   Sc_Y->Position = 5000;
   }
   */
  PB_Browser->Tag = 0;

  for (int i = 0; i < MAXRANDOMCARDS; i++) {
    m_Animation_RC2Idxs[i] = -1;
  }

  Fo_RandomAnimation->TB_Zoom->Position = TB_Zoom->Position;
  Fo_RandomAnimation->TB_Zoom->Enabled = false;
  Fo_RandomAnimation->Show();
}

// ---------------------------------------------------------------------------
void TFo_Main::Animation_RandomCard2() {
  if (!Fo_RandomAnimation->Visible) {
    // Animation end
    m_nAnimation = 0;
    if (m_DocBeforeAnimation) {
      m_Document->CopyFrom(m_DocBeforeAnimation);
      delete m_DocBeforeAnimation;
      m_DocBeforeAnimation = NULL;
      SB_Arrange->Down = m_bAnimationBak_Arrange;
    } else {
      SB_Arrange->Down = true;
    }
    Bu_ArrangeType->Tag = m_nAnimationBak_ArrangeType;
    SB_AutoScroll->Down = m_bAnimationBak_AutoScroll;
    SB_AutoZoom->Down = m_bAnimationBak_AutoZoom;
    // MM_Menu->Enabled = true;// Disable edit during Animation
    Redraw();
    // SetCardVisible();
    return;
  }

  if (SettingView.m_bAnimationPaused) {
    // Paused
    return;
  }

  // Random Card 2
  for (int i = 0; i < MAXRANDOMCARDS; i++) {
    if (i < SettingView.m_nAnimationRCCards) {
      // Within display count

      if (m_Animation_RC2Idxs[i] < 0) {
        // Card not specified

        SetCardAssign();

        // Filter displayable cards
        for (int i2 = 0; i2 < m_Document->m_Cards->Count; i2++)
          if (m_CardAssign[i2] == i2) {
            m_Document->GetCardByIndex(i2)->m_bVisible = true;
          }
        for (int i = 0; i < m_Document->m_Links->Count; i++) {
          m_Document->GetLinkByIndex(i)->m_bVisible = true;
        }

        FilteringCard();

        // Filter already displayed cards
        int count = 0;
        for (int i2 = 0; i2 < m_Document->m_Cards->Count; i2++) {
          TCard *Card = m_Document->GetCardByIndex(i2);
          if (Card->m_bVisible) {
            bool found = false;
            for (int i3 = 0; i3 < SettingView.m_nAnimationRCCards && !found;
                 i3++) {
              if (m_Animation_RC2Idxs[i3] == i2) {
                found = true;
              }
            }

            if (found) {
              Card->m_bVisible = false;
            } else {
              count++;
            }
          }
        }

        if (count > 0) {
          // Select one from displayable cards
          int idx = rand() % count;
          for (int i2 = 0; i2 < m_Document->m_Cards->Count; i2++) {
            if (m_Document->GetCardByIndex(i2)->m_bVisible) {
              if (idx-- == 0) {
                idx = i2;
                break;
              }
            }
          }

          if (idx >= 0) {
            m_Animation_RC2Idxs[i] = idx;
            TCard *Card = m_Document->GetCardByIndex(m_Animation_RC2Idxs[i]);

            /*
             // For read-aloud
             if (SettingView.m_bRead) if (m_AgentChar && m_TTS){
             if (m_nAnimationCount >= 3000){
             m_Document->ClearCardSelection();
             m_nTargetCard = Card->m_nID;
             Card->m_bSelected = true;
             m_nAnimationCount = 0;
             }
             }
             */

            // Determine initial position and movement direction
            int direction = rand() % 4; // Up/down/left/right
            switch (direction / 2) {
            case 0: // From top/bottom
              Card->m_fX = (rand() % 2001) / 1000.0f - 0.5f;
              m_Animation_RC2AXs[i] = -(Card->m_fX - 0.5f) + 0.5f +
                                      (rand() % 10001) / 10000.0f - 0.5f;
              if (direction == 0) {
                // Top
                Card->m_fY = 2.0f;
                m_Animation_RC2AYs[i] = -3.0f;
              } else {
                // Bottom
                Card->m_fY = -1.0f;
                m_Animation_RC2AYs[i] = 3.0f;
              }
              break;
            case 1: // From left/right
              Card->m_fY = (rand() % 2001) / 1000.0f - 0.5f;
              m_Animation_RC2AYs[i] = -(Card->m_fX - 0.5f) + 0.5f +
                                      (rand() % 10001) / 10000.0f - 0.5f;
              if (direction == 3) {
                // Left
                Card->m_fX = 2.0f;
                m_Animation_RC2AXs[i] = -3.0f;
              } else {
                // Right
                Card->m_fX = -1.0f;
                m_Animation_RC2AXs[i] = 3.0f;
              }
              break;
            };

            // Set movement speed (default 160~240 frames)
            int frames = (160 + rand() % 81);
            m_Animation_RC2AXs[i] /= frames;
            m_Animation_RC2AYs[i] /= frames;
          }
        }
      }
    } else {
      // Outside display count
      m_Animation_RC2Idxs[i] = -1;
    }
  }

  // (All hidden)
  for (int i = 0; i < m_Document->m_Cards->Count; i++) {
    m_Document->GetCardByIndex(i)->m_bVisible = false;
  }

  // Display only cards in motion
  float speed = 0.5f / pow(2.0, -(SettingView.m_nAnimationRCSpeed - 30) / 25.0);
  for (int i = 0; i < SettingView.m_nAnimationRCCards; i++) {
    if (m_Animation_RC2Idxs[i] >= 0) {
      // In motion display
      TCard *Card = m_Document->GetCardByIndex(m_Animation_RC2Idxs[i]);

      // Top
      Card->m_fX += m_Animation_RC2AXs[i] * speed;
      Card->m_fY += m_Animation_RC2AYs[i] * speed;

      if (Card->m_fX < -1.0f || Card->m_fY < -1.0f || Card->m_fX > 2.0f ||
          Card->m_fY > 2.0f) {
        // Bottom
        m_Animation_RC2Idxs[i] = -1;
      } else {
        Card->m_bVisible = true;
      }
    }
  }

  m_nAnimationCount += Ti_Check->Interval;
}

// ---------------------------------------------------------------------------
void TFo_Main::Animation_RandomJump() {
  if (!Fo_RandomAnimation->Visible) {
    // Animation end
    m_nAnimation = 0;
    if (m_DocBeforeAnimation) {
      m_Document->CopyFrom(m_DocBeforeAnimation);
      delete m_DocBeforeAnimation;
      m_DocBeforeAnimation = NULL;
      SB_Arrange->Down = m_bAnimationBak_Arrange;
    } else {
      SB_Arrange->Down = true;
    }
    Bu_ArrangeType->Tag = m_nAnimationBak_ArrangeType;
    SB_AutoScroll->Down = m_bAnimationBak_AutoScroll;
    SB_AutoZoom->Down = m_bAnimationBak_AutoZoom;
    // MM_Menu->Enabled = true;// Disable edit during Animation
    Redraw();
    // SetCardVisible();
    return;
  }

  if (SettingView.m_bAnimationPaused) {
    // Paused
    return;
  }

  // Random Card 2
  for (int i = 0; i < MAXRANDOMCARDS; i++) {
    if (i < SettingView.m_nAnimationRCCards) {
      // Within display count

      if (m_Animation_RC2Idxs[i] < 0) {
        // Card not specified

        SetCardAssign();

        // Filter displayable cards
        for (int i2 = 0; i2 < m_Document->m_Cards->Count; i2++)
          if (m_CardAssign[i2] == i2) {
            m_Document->GetCardByIndex(i2)->m_bVisible = true;
          }
        for (int i = 0; i < m_Document->m_Links->Count; i++) {
          m_Document->GetLinkByIndex(i)->m_bVisible = true;
        }

        FilteringCard();

        // Filter already displayed cards
        int count = 0;
        for (int i2 = 0; i2 < m_Document->m_Cards->Count; i2++) {
          if (m_Document->GetCardByIndex(i2)->m_bVisible) {
            bool found = false;
            for (int i3 = 0; i3 < SettingView.m_nAnimationRCCards && !found;
                 i3++) {
              if (m_Animation_RC2Idxs[i3] == i2) {
                found = true;
              }
            }

            if (found) {
              m_Document->GetCardByIndex(i2)->m_bVisible = false;
            } else {
              count++;
            }
          }
        }

        if (count > 0) {
          // Select one from displayable cards
          int idx = rand() % count;
          for (int i2 = 0; i2 < m_Document->m_Cards->Count; i2++) {
            if (m_Document->GetCardByIndex(i2)->m_bVisible) {
              if (idx-- == 0) {
                idx = i2;
                break;
              }
            }
          }

          if (idx >= 0) {
            m_Animation_RC2Idxs[i] = idx;
            TCard *Card = m_Document->GetCardByIndex(m_Animation_RC2Idxs[i]);

            /*
             // For read-aloud
             if (SettingView.m_bRead) if (m_AgentChar && m_TTS){
             if (m_nAnimationCount >= 3000){
             m_Document->ClearCardSelection();
             m_nTargetCard = Card->m_nID;
             Card->m_bSelected = true;
             m_nAnimationCount = 0;
             }
             }
             */

            // Determine initial position and movement direction
            Card->m_fY = 2.0f;
            Card->m_fX = (rand() % 2001) / 1000.0f - 0.5f;
            // Set movement speed (default 160~240 frames)
            int frames = (160 + rand() % 81);
            m_Animation_RC2AYs[i] = (-rand() % 15 - 27) * (1.0f / frames);
            m_Animation_RC2AXs[i] = (rand() % 61 - 30) * (0.1f / frames);
          }
        }
      }
    } else {
      // Outside display count
      m_Animation_RC2Idxs[i] = -1;
    }
  }

  // (All hidden)
  for (int i = 0; i < m_Document->m_Cards->Count; i++) {
    m_Document->GetCardByIndex(i)->m_bVisible = false;
  }

  // Display only cards in motion
  float speed = 0.3f / pow(2.0, -(SettingView.m_nAnimationRCSpeed - 30) / 25.0);
  for (int i = 0; i < SettingView.m_nAnimationRCCards; i++) {
    if (m_Animation_RC2Idxs[i] >= 0) {
      // In motion display
      TCard *Card = m_Document->GetCardByIndex(m_Animation_RC2Idxs[i]);

      // Top
      Card->m_fX += m_Animation_RC2AXs[i] * speed;
      Card->m_fY += m_Animation_RC2AYs[i] * speed;
      m_Animation_RC2AYs[i] += speed * 0.01f;

      if (Card->m_fY > 2.0f) {
        // Bottom
        m_Animation_RC2Idxs[i] = -1;
      } else {
        Card->m_bVisible = true;
      }
    }
  }

  m_nAnimationCount += Ti_Check->Interval;
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::MA_RandomTraceClick(TObject *Sender) {
  m_nAnimation = 3;
  m_nAnimationCount = 0;
  PC_Client->ActivePage = TS_Browser;
  SettingView.m_bAnimationPaused = false;

  m_nTargetLink = -1;
  m_nAnimation_LastCard = m_nTargetCard;

  if (!m_DocBeforeAnimation) {
    m_DocBeforeAnimation = new TDocument(*m_Document);
  }
  m_nAnimationBak_ArrangeType = Bu_ArrangeType->Tag;
  m_bAnimationBak_Arrange = SB_Arrange->Down;
  m_bAnimationBak_AutoScroll = SB_AutoScroll->Down;
  m_bAnimationBak_AutoZoom = SB_AutoZoom->Down;
  // MM_Menu->Enabled = false;// Disable edit during Animation

  SB_AutoScroll->Down = true;
  SB_AutoZoom->Down = true;

  Fo_RandomAnimation->TB_Zoom->Position = TB_Zoom->Position;
  Fo_RandomAnimation->TB_Zoom->Enabled = true;
  Fo_RandomAnimation->Show();
}

// ---------------------------------------------------------------------------
void TFo_Main::Animation_RandomTrace() {
  if (!Fo_RandomAnimation->Visible) {
    // Animation end
    m_nAnimation = 0;
    if (m_DocBeforeAnimation) {
      m_Document->CopyFrom(m_DocBeforeAnimation);
      delete m_DocBeforeAnimation;
      m_DocBeforeAnimation = NULL;
    }
    Bu_ArrangeType->Tag = m_nAnimationBak_ArrangeType;
    SB_Arrange->Down = m_bAnimationBak_Arrange;
    SB_AutoScroll->Down = m_bAnimationBak_AutoScroll;
    SB_AutoZoom->Down = m_bAnimationBak_AutoZoom;
    Redraw();
    // MM_Menu->Enabled = true;// Disable edit during Animation
    return;
  }

  // Random Trace
  if (m_nAnimationCount == 0) {

    // Traceable link count
    int count = 0;
    if (m_nTargetCard >= 0) {
      // Count traceable links from current card

      for (int il = 0; il < m_Document->m_Links->Count; il++)
        if (m_LinkVisible[il]) {
          TLink *Link = m_Document->GetLinkByIndex(il);
          if ((Link->m_nFromID == m_nTargetCard &&
               Link->m_nDestID != m_nAnimation_LastCard) ||
              (Link->m_nDestID == m_nTargetCard &&
               Link->m_nFromID != m_nAnimation_LastCard &&
               !Link->m_bDirection)) {
            // Traceable link
            count++;
          }
        }
    }

    int nextid = -1;
    if (count) {
      // Has traceable links

      int idx = rand() % count; // Decide which link to trace

      for (int il = 0; il < m_Document->m_Links->Count; il++)
        if (m_LinkVisible[il]) {
          TLink *Link = m_Document->GetLinkByIndex(il);
          if ((Link->m_nFromID == m_nTargetCard &&
               Link->m_nDestID != m_nAnimation_LastCard) ||
              (Link->m_nDestID == m_nTargetCard &&
               Link->m_nFromID != m_nAnimation_LastCard &&
               !Link->m_bDirection)) {
            // Traceable link

            if (idx-- == 0) {
              if (Link->m_nFromID == m_nTargetCard) {
                nextid = Link->m_nDestID;
              } else {
                nextid = Link->m_nFromID;
              }
            }
          }
        }
    }

    if (m_nTargetCard < 0) {
      // No card selected

      // Select random card from displayed

      count = 0; // Displayed card count
      for (int i = 0; i < m_Document->m_Cards->Count; i++) {
        if (m_Document->GetCardByIndex(i)->m_bVisible) {
          count++;
        }
      }

      int idx = rand() % count; // Decide display card
      for (int i = 0; i < m_Document->m_Cards->Count; i++) {
        if (m_Document->GetCardByIndex(i)->m_bVisible) {
          if (idx-- == 0) {
            idx = i;
            break;
          }
        }
      }

      if (idx >= 0) {
        nextid = m_Document->GetCardByIndex(idx)->m_nID;
      }
    }

    m_nAnimation_LastCard = m_nTargetCard;
    m_nTargetCard = nextid;
    m_Document->ClearCardSelection();
    TCard *Card = m_Document->GetCard(m_nTargetCard);
    if (Card) {
      Card->m_bSelected = true;
    }
  }

  m_nAnimationCount += Ti_Check->Interval;

  if (!SettingView.m_bAnimationPaused &&
      m_nAnimationCount >=
          1000 * pow(2.0, -(SettingView.m_nAnimationRCSpeed - 30) / 25.0)) {
    m_nAnimationCount = 0;
  }
}

