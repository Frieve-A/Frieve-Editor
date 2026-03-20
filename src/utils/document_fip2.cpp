// ---------------------------------------------------------------------------
// FIP2 plain-text document format (see FIP2_FORMAT_SPEC.md).
//
// Decoding: TDocument::Load forces UTF-8 for .fip2 and for UTF-8 FIP2 magic.
//
// Card body delimiters use <!--BODY token=...--> / <!--END_BODY token=...-->.
// The token is a per-card delimiter label only: the loader finds the end line
// by exact text match; it does not check that the token still "matches" the
// body. Manual or LLM edits may change body lines freely while keeping the
// same token on both marker lines; no re-hash is required unless a body line
// duplicates the end marker (then re-save from the app picks a new token).

#pragma hdrstop

#include <System.Classes.hpp>
#include <System.SysUtils.hpp>
#include <System.Math.hpp>

#include "document.h"
#include "drawing.h"
#include "label.h"
#include "link.h"
#include "card.h"

#include <vector>
#include <string.h>

namespace {

UnicodeString TrimLine(const UnicodeString &S) { return Trim(S); }

int IndexFirstNonEmptyLine(TStringList *SL) {
  for (int i = 0; i < SL->Count; i++) {
    if (!TrimLine(SL->Strings[i]).IsEmpty()) {
      return i;
    }
  }
  return -1;
}

bool ParseKeyValue(const UnicodeString &line, UnicodeString &key,
                   UnicodeString &value) {
  int p = line.Pos(L"=");
  if (p < 1) {
    return false;
  }
  key = TrimLine(line.SubString(1, p - 1));
  value = line.SubString(p + 1, line.Length() - p);
  return !key.IsEmpty();
}

int ParseIntDef(const UnicodeString &s, int def) {
  return StrToIntDef(TrimLine(s), def);
}

double ParseFloatDefU(const UnicodeString &s, double def) {
  UnicodeString t = StringReplace(TrimLine(s), L",", L".",
                                    TReplaceFlags() << rfReplaceAll);
  return StrToFloatDef(t, def);
}

int ParseBool01(const UnicodeString &s, int def) {
  return ParseIntDef(s, def) != 0 ? 1 : 0;
}

// ---------------------------------------------------------------------------
// SHA-256 (FIPS 180-4): used only to derive a 16-hex delimiter token on save
// (spec alignment + low collision rate). Not a body checksum; readers ignore
// whether the token equals any hash of the stored body.

typedef unsigned char u8;
typedef unsigned int u32;
typedef unsigned __int64 u64;

static u32 rotr(u32 x, int n) { return (x >> n) | (x << (32 - n)); }

static void sha256_compress(u32 *state, const u8 *block) {
  static const u32 K[64] = {
      0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1,
      0x923f82a4, 0xab1c5ed5, 0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
      0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174, 0xe49b69c1, 0xefbe4786,
      0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
      0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147,
      0x06ca6351, 0x14292967, 0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
      0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85, 0xa2bfe8a1, 0xa81a664b,
      0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
      0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a,
      0x5b9cca4f, 0x682e6ff3, 0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
      0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};
  u32 w[64];
  int i;
  for (i = 0; i < 16; i++) {
    w[i] = ((u32)block[i * 4] << 24) | ((u32)block[i * 4 + 1] << 16) |
           ((u32)block[i * 4 + 2] << 8) | (u32)block[i * 4 + 3];
  }
  for (; i < 64; i++) {
    u32 s0 = rotr(w[i - 15], 7) ^ rotr(w[i - 15], 18) ^ (w[i - 15] >> 3);
    u32 s1 = rotr(w[i - 2], 17) ^ rotr(w[i - 2], 19) ^ (w[i - 2] >> 10);
    w[i] = w[i - 16] + s0 + w[i - 7] + s1;
  }
  u32 a = state[0], b = state[1], c = state[2], d = state[3];
  u32 e = state[4], f = state[5], g = state[6], h = state[7];
  for (i = 0; i < 64; i++) {
    u32 S1 = rotr(e, 6) ^ rotr(e, 11) ^ rotr(e, 25);
    u32 ch = (e & f) ^ ((~e) & g);
    u32 t1 = h + S1 + ch + K[i] + w[i];
    u32 S0 = rotr(a, 2) ^ rotr(a, 13) ^ rotr(a, 22);
    u32 maj = (a & b) ^ (a & c) ^ (b & c);
    u32 t2 = S0 + maj;
    h = g;
    g = f;
    f = e;
    e = d + t1;
    d = c;
    c = b;
    b = a;
    a = t1 + t2;
  }
  state[0] += a;
  state[1] += b;
  state[2] += c;
  state[3] += d;
  state[4] += e;
  state[5] += f;
  state[6] += g;
  state[7] += h;
}

static void sha256_raw(const u8 *data, unsigned len, u8 out[32]) {
  u32 state[8] = {0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
                  0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19};
  unsigned pos = 0;
  while (pos + 64 <= len) {
    sha256_compress(state, data + pos);
    pos += 64;
  }
  u8 tail[128];
  unsigned r = len - pos;
  memcpy(tail, data + pos, r);
  tail[r++] = 0x80;
  if (r > 56) {
    memset(tail + r, 0, 64 - r);
    sha256_compress(state, tail);
    memset(tail, 0, 56);
  } else {
    memset(tail + r, 0, 56 - r);
  }
  u64 bitlen = (u64)len * 8ULL;
  for (int j = 0; j < 8; j++) {
    tail[56 + j] = (u8)((bitlen >> (56 - 8 * j)) & 0xff);
  }
  sha256_compress(state, tail);
  for (int i = 0; i < 8; i++) {
    out[i * 4] = (u8)(state[i] >> 24);
    out[i * 4 + 1] = (u8)(state[i] >> 16);
    out[i * 4 + 2] = (u8)(state[i] >> 8);
    out[i * 4 + 3] = (u8)(state[i]);
  }
}

static bool Sha256First8HexUpper(const u8 *data, unsigned len,
                                 UnicodeString &out16) {
  u8 hash[32];
  sha256_raw(data, len, hash);
  UnicodeString hex;
  for (unsigned i = 0; i < 8; i++) {
    hex += IntToHex(hash[i], 2);
  }
  out16 = hex.UpperCase();
  return true;
}

UnicodeString BuildBodyHashInput(int cardId, int lineCount,
                                 const UnicodeString &bodyJoined) {
  return IntToStr(cardId) + L":" + IntToStr(lineCount) + L":" + bodyJoined;
}

// Pick a token so BODY/END_BODY lines are unlikely to appear inside the body.
// Editors do not need to recompute SHA when editing body text between markers.
bool Fip2MakeTokenForCard(const TCard *card, UnicodeString &tokenOut) {
  UnicodeString bodyJoined;
  for (int i = 0; i < card->m_Lines->Count; i++) {
    if (i > 0) {
      bodyJoined += L"\n";
    }
    bodyJoined += card->m_Lines->Strings[i];
  }
  UnicodeString salt;
  for (int attempt = 0; attempt < 64; attempt++) {
    UnicodeString payload =
        BuildBodyHashInput(card->m_nID, card->m_Lines->Count, bodyJoined) +
        salt;
    TBytes pbytes = TEncoding::UTF8->GetBytes(payload);
    const u8 *pb =
        pbytes.Length > 0 ? (const u8 *)&pbytes[0] : (const u8 *)"";
    unsigned plen = (unsigned)pbytes.Length;
    if (!Sha256First8HexUpper(pb, plen, tokenOut)) {
      return false;
    }
    UnicodeString endMark =
        UnicodeString(L"<!--END_BODY token=") + tokenOut + L"-->";
    bool collision = false;
    for (int li = 0; li < card->m_Lines->Count; li++) {
      if (TrimLine(card->m_Lines->Strings[li]) == TrimLine(endMark)) {
        collision = true;
        break;
      }
    }
    UnicodeString startMark =
        UnicodeString(L"<!--BODY token=") + tokenOut + L"-->";
    if (!collision) {
      for (int li = 0; li < card->m_Lines->Count; li++) {
        if (TrimLine(card->m_Lines->Strings[li]) == TrimLine(startMark)) {
          collision = true;
          break;
        }
      }
    }
    if (!collision) {
      return true;
    }
    salt += L"\n";
  }
  return false;
}

struct Fip2ParsedLabel {
  int id;
  UnicodeString name;
  int color;
  int enabled;
  int show;
  int hide;
  int fold;
  int size;
};

struct Fip2ParsedLink {
  int fromId;
  int toId;
  int directionVisible;
  int shape;
  std::vector<int> labelIds;
  UnicodeString name;
};

int FindLineStartsWith(TStringList *SL, int from, const UnicodeString &prefix) {
  for (int i = from; i < SL->Count; i++) {
    if (SL->Strings[i].Pos(prefix) == 1) {
      return i;
    }
  }
  return -1;
}

void ApplyFip2LabelsFromParsed(TDocument *doc, int ltype,
                               std::vector<Fip2ParsedLabel> &items) {
  doc->ClearLabels(ltype);
  int maxId = 0;
  for (size_t i = 0; i < items.size(); i++) {
    if (items[i].id > maxId) {
      maxId = items[i].id;
    }
  }
  if (maxId <= 0) {
    if (ltype == 0) {
      doc->InitLabel(0);
    } else {
      doc->InitLabel(1);
    }
    return;
  }
  for (int id = 1; id <= maxId; id++) {
    Fip2ParsedLabel *found = NULL;
    for (size_t j = 0; j < items.size(); j++) {
      if (items[j].id == id) {
        found = &items[j];
        break;
      }
    }
    TCardLabel *L = doc->NewLabel(ltype);
    if (found) {
      L->m_Name = found->name;
      L->m_nColor = found->color;
      L->m_bEnable = found->enabled;
      L->m_bShow = found->show;
      L->m_bHide = found->hide;
      L->m_bFold = found->fold;
      L->m_nSize = found->size;
    }
  }
}

void ParseFip2LabelsSection(TDocument *Doc, TStringList *SL, int ltype) {
  std::vector<Fip2ParsedLabel> out;
  UnicodeString startTok =
      UnicodeString(L"[[LABELS type=") + IntToStr(ltype) + L"]]";
  int s = FindLineStartsWith(SL, 0, startTok);
  if (s < 0) {
    ApplyFip2LabelsFromParsed(Doc, ltype, out);
    return;
  }
  int e = -1;
  for (int j = s + 1; j < SL->Count; j++) {
    if (TrimLine(SL->Strings[j]) == L"[[END:LABELS]]") {
      e = j;
      break;
    }
  }
  if (e < 0) {
    ApplyFip2LabelsFromParsed(Doc, ltype, out);
    return;
  }
  int j = s + 1;
  while (j < e) {
    if (TrimLine(SL->Strings[j]) == L"[[LABEL]]") {
      j++;
      Fip2ParsedLabel Lbl;
      Lbl.id = 0;
      Lbl.name = L"";
      Lbl.color = 0;
      Lbl.enabled = 0;
      Lbl.show = 0;
      Lbl.hide = 0;
      Lbl.fold = 0;
      Lbl.size = 0;
      while (j < e && TrimLine(SL->Strings[j]) != L"[[END:LABEL]]") {
        UnicodeString k2, v2;
        if (ParseKeyValue(SL->Strings[j], k2, v2)) {
          if (k2 == L"id") {
            Lbl.id = ParseIntDef(v2, 0);
          } else if (k2 == L"name") {
            Lbl.name = v2;
          } else if (k2 == L"color") {
            Lbl.color = ParseIntDef(v2, 0);
          } else if (k2 == L"enabled") {
            Lbl.enabled = ParseBool01(v2, 0);
          } else if (k2 == L"show") {
            Lbl.show = ParseBool01(v2, 0);
          } else if (k2 == L"hide") {
            Lbl.hide = ParseBool01(v2, 0);
          } else if (k2 == L"fold") {
            Lbl.fold = ParseBool01(v2, 0);
          } else if (k2 == L"size") {
            Lbl.size = ParseIntDef(v2, 0);
          }
        }
        j++;
      }
      if (j < e && TrimLine(SL->Strings[j]) == L"[[END:LABEL]]") {
        j++;
      }
      out.push_back(Lbl);
    } else {
      j++;
    }
  }
  ApplyFip2LabelsFromParsed(Doc, ltype, out);
}

} // namespace

// ---------------------------------------------------------------------------
bool TDocument::ShouldLoadAsFip2(const UnicodeString &FN, TStringList *SL) {
  if (LowerCase(ExtractFileExt(FN)) == L".fip2") {
    return true;
  }
  int i = IndexFirstNonEmptyLine(SL);
  if (i < 0) {
    return false;
  }
  UnicodeString t = TrimLine(SL->Strings[i]);
  return (t == L"FIP2/1" || t == L"FIPMD/1");
}

// ---------------------------------------------------------------------------
bool TDocument::LoadFromStringFip2(TStringList *SL, UnicodeString FN) {
  (void)FN;
  ClearCards();
  ClearLinks();
  ClearLabels(0);
  ClearLabels(1);

  int magicIdx = IndexFirstNonEmptyLine(SL);
  if (magicIdx < 0) {
    return false;
  }
  UnicodeString magic = TrimLine(SL->Strings[magicIdx]);
  if (magic != L"FIP2/1" && magic != L"FIPMD/1") {
    return false;
  }

  int globalStart = -1;
  for (int z = 0; z < SL->Count; z++) {
    if (TrimLine(SL->Strings[z]) == L"[Global]") {
      globalStart = z;
      break;
    }
  }
  if (globalStart < 0) {
    return false;
  }

  int version = 0;
  m_bReadOnly = false;
  m_nDefaultView = -1;
  m_nAutoSave = 0;
  m_nAutoReload = 0;

  int passSizeLim = 0;
  int passLinkLim = 0;
  int passDateLim = 0;

  int i = globalStart + 1;
  while (i < SL->Count) {
    UnicodeString raw = SL->Strings[i];
    UnicodeString ln = TrimLine(raw);
    if (ln.IsEmpty()) {
      i++;
      continue;
    }
    if (ln.Pos(L"[[") == 1) {
      break;
    }
    if (ln.Pos(L"[") == 1 && ln.Pos(L"[[") != 1) {
      break;
    }
    UnicodeString k, v;
    if (ParseKeyValue(raw, k, v)) {
      if (k == L"Version") {
        version = ParseIntDef(v, 0);
      } else if (k == L"ReadOnly") {
        m_bReadOnly = ParseBool01(v, 0) != 0;
      } else if (k == L"DefaultView") {
        m_nDefaultView = ParseIntDef(v, -1);
      } else if (k == L"AutoSave") {
        m_nAutoSave = ParseBool01(v, 0);
      } else if (k == L"AutoReload") {
        m_nAutoReload = ParseBool01(v, 0);
      } else if (k == L"Arrange") {
        bReqArrange = ParseIntDef(v, bReqArrange);
      } else if (k == L"ArrangeMode") {
        nReqArrangeMode = ParseIntDef(v, nReqArrangeMode);
      } else if (k == L"AutoScroll") {
        bReqAutoScroll = ParseIntDef(v, bReqAutoScroll);
      } else if (k == L"AutoZoom") {
        bReqAutoZoom = ParseIntDef(v, bReqAutoZoom);
      } else if (k == L"FullScreen") {
        bReqFullScreen = ParseIntDef(v, bReqFullScreen);
      } else if (k == L"Exit") {
        bReqExit = ParseIntDef(v, bReqExit);
      } else if (k == L"Zoom") {
        fReqZoom = (float)ParseFloatDefU(v, fReqZoom);
      } else if (k == L"X") {
        fReqX = (float)ParseFloatDefU(v, fReqX);
      } else if (k == L"Y") {
        fReqY = (float)ParseFloatDefU(v, fReqY);
      } else if (k == L"TargetCard") {
        nReqTargetCard = ParseIntDef(v, nReqTargetCard);
      } else if (k == L"SizeLimitation") {
        if (passSizeLim == 0) {
          bReqSizeLimitation = ParseIntDef(v, bReqSizeLimitation);
        } else {
          nReqSizeLimitation = ParseIntDef(v, nReqSizeLimitation);
        }
        passSizeLim++;
      } else if (k == L"LinkLimitation") {
        if (passLinkLim == 0) {
          bReqLinkLimitation = ParseIntDef(v, bReqLinkLimitation);
        } else {
          nReqLinkLimitation = ParseIntDef(v, nReqLinkLimitation);
        }
        passLinkLim++;
      } else if (k == L"DateLimitation") {
        if (passDateLim == 0) {
          bReqDateLimitation = ParseIntDef(v, bReqDateLimitation);
        } else {
          nReqDateLimitation = ParseIntDef(v, nReqDateLimitation);
        }
        passDateLim++;
      } else if (k == L"LinkDirection") {
        bReqLinkDirection = ParseIntDef(v, bReqLinkDirection);
      } else if (k == L"LinkBackward") {
        bReqLinkBackward = ParseIntDef(v, bReqLinkBackward);
      } else if (k == L"LinkTarget") {
        nReqLinkTarget = ParseIntDef(v, nReqLinkTarget);
      } else if (k == L"DateLimitationDateType") {
        ReqDateLimitationDateType =
            ParseIntDef(v, ReqDateLimitationDateType);
      } else if (k == L"DateLimitationType") {
        ReqDateLimitationType = ParseIntDef(v, ReqDateLimitationType);
      }
    }
    i++;
  }

  ParseFip2LabelsSection(this, SL, 0);
  ParseFip2LabelsSection(this, SL, 1);

  std::vector<Fip2ParsedLink> linksParsed;
  int linksStart = FindLineStartsWith(SL, 0, L"[[LINKS]]");
  if (linksStart >= 0) {
    int j = linksStart + 1;
    while (j < SL->Count && TrimLine(SL->Strings[j]) != L"[[END:LINKS]]") {
      if (TrimLine(SL->Strings[j]) == L"[[LINK]]") {
        j++;
        Fip2ParsedLink L;
        L.fromId = 0;
        L.toId = 0;
        L.directionVisible = 1;
        L.shape = 0;
        L.name = L"";
        while (j < SL->Count && TrimLine(SL->Strings[j]) != L"[[END:LINK]]") {
          UnicodeString k2, v2;
          if (ParseKeyValue(SL->Strings[j], k2, v2)) {
            if (k2 == L"from") {
              L.fromId = ParseIntDef(v2, 0);
            } else if (k2 == L"to") {
              L.toId = ParseIntDef(v2, 0);
            } else if (k2 == L"directionVisible") {
              L.directionVisible = ParseBool01(v2, 1);
            } else if (k2 == L"shape") {
              L.shape = ParseIntDef(v2, 0);
            } else if (k2 == L"name") {
              L.name = v2;
            } else if (k2 == L"labels") {
              TStringList *parts = new TStringList();
              parts->Delimiter = L',';
              parts->DelimitedText = v2;
              for (int p = 0; p < parts->Count; p++) {
                UnicodeString t = TrimLine(parts->Strings[p]);
                if (!t.IsEmpty()) {
                  L.labelIds.push_back(ParseIntDef(t, 0));
                }
              }
              delete parts;
            }
          }
          j++;
        }
        if (j < SL->Count && TrimLine(SL->Strings[j]) == L"[[END:LINK]]") {
          j++;
        }
        linksParsed.push_back(L);
      } else {
        j++;
      }
    }
  }

  for (size_t li = 0; li < linksParsed.size(); li++) {
    TLink *Link = NewLink();
    Link->m_nFromID = linksParsed[li].fromId;
    Link->m_nDestID = linksParsed[li].toId;
    Link->m_bDirection = linksParsed[li].directionVisible;
    Link->m_nShape = linksParsed[li].shape;
    Link->m_Name = linksParsed[li].name;
    for (size_t k = 0; k < linksParsed[li].labelIds.size(); k++) {
      Link->m_Labels->AddLabel(linksParsed[li].labelIds[k]);
    }
  }

  int cardsStart = -1;
  for (int z = 0; z < SL->Count; z++) {
    if (TrimLine(SL->Strings[z]).Pos(L"[[CARDS focus=") == 1) {
      cardsStart = z;
      break;
    }
  }

  if (cardsStart < 0) {
    return false;
  }

  int j = cardsStart + 1;
  while (j < SL->Count && TrimLine(SL->Strings[j]) != L"[[END:CARDS]]") {
    UnicodeString lineJ = TrimLine(SL->Strings[j]);
    if (lineJ.Pos(L"[[CARD id=") == 1) {
      int idClose = lineJ.Pos(L"]]");
      int eq = lineJ.Pos(L"=");
      int cid = 0;
      if (eq > 0 && idClose > eq) {
        cid = ParseIntDef(lineJ.SubString(eq + 1, idClose - eq - 1), 0);
      }
      j++;

      UnicodeString kvTitle, kvVisible, kvShape, kvSize, kvTop, kvFixed,
          kvFold, kvX, kvY, kvCreated, kvUpdated, kvViewed, kvLabelList,
          kvDrawing;
      std::vector<UnicodeString> bodyLines;
      UnicodeString token;

      while (j < SL->Count) {
        UnicodeString cur = SL->Strings[j];
        UnicodeString curT = TrimLine(cur);
        if (curT == L"[[END:CARD]]") {
          j++;
          break;
        }
        // Body ends at the END_BODY line matching this token; no hash check.
        if (curT.Pos(L"<!--BODY token=") == 1) {
          int tp = curT.Pos(L"token=");
          int tq = curT.Pos(L"-->");
          if (tp > 0 && tq > tp) {
            token = curT.SubString(tp + 6, tq - tp - 6);
          }
          j++;
          UnicodeString endExpected =
              UnicodeString(L"<!--END_BODY token=") + token + L"-->";
          while (j < SL->Count) {
            if (TrimLine(SL->Strings[j]) == TrimLine(endExpected)) {
              j++;
              break;
            }
            bodyLines.push_back(SL->Strings[j]);
            j++;
          }
          continue;
        }
        UnicodeString k3, v3;
        if (ParseKeyValue(cur, k3, v3)) {
          if (k3 == L"title") {
            kvTitle = v3;
          } else if (k3 == L"visible") {
            kvVisible = v3;
          } else if (k3 == L"shape") {
            kvShape = v3;
          } else if (k3 == L"size") {
            kvSize = v3;
          } else if (k3 == L"top") {
            kvTop = v3;
          } else if (k3 == L"fixed") {
            kvFixed = v3;
          } else if (k3 == L"fold") {
            kvFold = v3;
          } else if (k3 == L"x") {
            kvX = v3;
          } else if (k3 == L"y") {
            kvY = v3;
          } else if (k3 == L"created") {
            kvCreated = v3;
          } else if (k3 == L"updated") {
            kvUpdated = v3;
          } else if (k3 == L"viewed") {
            kvViewed = v3;
          } else if (k3 == L"labelList") {
            kvLabelList = v3;
          } else if (k3 == L"drawing") {
            kvDrawing = v3;
          }
        }
        j++;
      }

      TCard *Card = new TCard();
      m_Cards->Add(Card);
      Card->m_nID = cid;
      Card->m_Title = kvTitle;
      Card->m_bVisible = kvVisible.IsEmpty() ? true
                                             : (ParseBool01(kvVisible, 1) != 0);
      if (!kvShape.IsEmpty()) {
        Card->m_nShape = ParseIntDef(kvShape, Card->m_nShape);
      }
      if (!kvSize.IsEmpty()) {
        Card->m_nSize = ParseIntDef(kvSize, Card->m_nSize);
      }
      if (!kvTop.IsEmpty()) {
        Card->m_bTop = ParseBool01(kvTop, 0) != 0;
      }
      if (!kvFixed.IsEmpty()) {
        Card->m_bFixed = ParseBool01(kvFixed, 0) != 0;
      }
      if (!kvFold.IsEmpty()) {
        Card->m_bFold = ParseBool01(kvFold, 0) != 0;
      }
      if (!kvX.IsEmpty()) {
        Card->m_fX = (float)ParseFloatDefU(kvX, Card->m_fX);
      }
      if (!kvY.IsEmpty()) {
        Card->m_fY = (float)ParseFloatDefU(kvY, Card->m_fY);
      }
      if (!kvCreated.IsEmpty()) {
        Card->m_fCreated = StrToDateTimeDef(kvCreated, Card->m_fCreated);
      }
      if (!kvUpdated.IsEmpty()) {
        Card->m_fUpdated = StrToDateTimeDef(kvUpdated, Card->m_fUpdated);
      }
      if (!kvViewed.IsEmpty()) {
        Card->m_fViewed = StrToDateTimeDef(kvViewed, Card->m_fViewed);
      }

      Card->m_Labels->Clear();
      TStringList *lp = new TStringList();
      lp->Delimiter = L',';
      lp->DelimitedText = kvLabelList;
      for (int p = 0; p < lp->Count; p++) {
        UnicodeString t = TrimLine(lp->Strings[p]);
        if (!t.IsEmpty()) {
          Card->m_Labels->AddLabel(ParseIntDef(t, 0));
        }
      }
      delete lp;

      delete Card->m_Drawing;
      Card->m_Drawing = new TDrawing(kvDrawing);
      Card->m_Drawing->ClearSelection();

      Card->m_Lines->Clear();
      for (size_t bi = 0; bi < bodyLines.size(); bi++) {
        Card->m_Lines->Add(bodyLines[bi]);
      }
      if (version <= 4) {
        Card->m_Title = StringReplace(Card->m_Title, L"\\", L"\\\\",
                                      TReplaceFlags() << rfReplaceAll);
      }
      Card->CheckImageFN();
    } else {
      j++;
    }
  }

  int maxCardId = 0;
  for (int ci = 0; ci < m_Cards->Count; ci++) {
    TCard *C = GetCardByIndex(ci);
    if (C->m_nID > maxCardId) {
      maxCardId = C->m_nID;
    }
  }
  m_nMaxCardID = maxCardId + 1;
  if (m_nMaxCardID < 1) {
    m_nMaxCardID = 1;
  }

  m_nCardID = nReqTargetCard;
  if (SearchCardIndex(m_nCardID) < 0) {
    m_nCardID = -1;
  }

  return m_Cards->Count > 0;
}

// ---------------------------------------------------------------------------
bool TDocument::SaveToStringFip2(TStringList *SL) {
  SL->Clear();
  SL->Add(L"FIP2/1");
  SL->Add(L"This is a UTF-8 text file in Frieve Editor's fip2 format.");
  SL->Add(L"");

  SL->Add(L"[Global]");
  SL->Add(UnicodeString(L"Version=") + IntToStr(FileVersion));
  SL->Add(UnicodeString(L"ReadOnly=") + IntToStr(m_bReadOnly ? 1 : 0));
  SL->Add(UnicodeString(L"DefaultView=") + IntToStr(m_nDefaultView));
  SL->Add(UnicodeString(L"AutoSave=") + IntToStr(m_nAutoSave));
  SL->Add(UnicodeString(L"AutoReload=") + IntToStr(m_nAutoReload));
  SL->Add(UnicodeString(L"Arrange=") + IntToStr(bReqArrange));
  SL->Add(UnicodeString(L"ArrangeMode=") + IntToStr(nReqArrangeMode));
  SL->Add(UnicodeString(L"AutoScroll=") + IntToStr(bReqAutoScroll));
  SL->Add(UnicodeString(L"AutoZoom=") + IntToStr(bReqAutoZoom));
  SL->Add(UnicodeString(L"FullScreen=") + IntToStr(bReqFullScreen));
  SL->Add(UnicodeString(L"Exit=") + IntToStr(bReqExit));
  SL->Add(UnicodeString(L"Zoom=") + FloatToStr(fReqZoom));
  SL->Add(UnicodeString(L"X=") + FloatToStr(fReqX));
  SL->Add(UnicodeString(L"Y=") + FloatToStr(fReqY));
  SL->Add(UnicodeString(L"TargetCard=") + IntToStr(nReqTargetCard));
  SL->Add(UnicodeString(L"SizeLimitation=") + IntToStr(bReqSizeLimitation));
  SL->Add(UnicodeString(L"LinkLimitation=") + IntToStr(bReqLinkLimitation));
  SL->Add(UnicodeString(L"DateLimitation=") + IntToStr(bReqDateLimitation));
  SL->Add(UnicodeString(L"SizeLimitation=") + IntToStr(nReqSizeLimitation));
  SL->Add(UnicodeString(L"LinkLimitation=") + IntToStr(nReqLinkLimitation));
  SL->Add(UnicodeString(L"DateLimitation=") + IntToStr(nReqDateLimitation));
  SL->Add(UnicodeString(L"LinkDirection=") + IntToStr(bReqLinkDirection));
  SL->Add(UnicodeString(L"LinkBackward=") + IntToStr(bReqLinkBackward));
  SL->Add(UnicodeString(L"LinkTarget=") + IntToStr(nReqLinkTarget));
  SL->Add(UnicodeString(L"DateLimitationDateType=") +
          IntToStr(ReqDateLimitationDateType));
  SL->Add(UnicodeString(L"DateLimitationType=") +
          IntToStr(ReqDateLimitationType));
  SL->Add(L"");

  for (int ltype = 0; ltype <= 1; ltype++) {
    SL->Add(UnicodeString(L"[[LABELS type=") + IntToStr(ltype) + L"]]");
    for (int li = 0; li < m_Labels[ltype]->Count; li++) {
      TCardLabel *Lb = GetLabelByIndex(ltype, li);
      SL->Add(L"[[LABEL]]");
      SL->Add(UnicodeString(L"id=") + IntToStr(li + 1));
      SL->Add(UnicodeString(L"name=") + Lb->m_Name);
      SL->Add(UnicodeString(L"color=") + IntToStr(Lb->m_nColor));
      SL->Add(UnicodeString(L"enabled=") + IntToStr(Lb->m_bEnable));
      SL->Add(UnicodeString(L"show=") + IntToStr(Lb->m_bShow));
      SL->Add(UnicodeString(L"hide=") + IntToStr(Lb->m_bHide));
      SL->Add(UnicodeString(L"fold=") + IntToStr(Lb->m_bFold));
      SL->Add(UnicodeString(L"size=") + IntToStr(Lb->m_nSize));
      SL->Add(L"[[END:LABEL]]");
    }
    SL->Add(L"[[END:LABELS]]");
    SL->Add(L"");
  }

  SL->Add(L"[[LINKS]]");
  for (int li = 0; li < m_Links->Count; li++) {
    TLink *Lk = GetLinkByIndex(li);
    SL->Add(L"[[LINK]]");
    SL->Add(UnicodeString(L"from=") + IntToStr(Lk->m_nFromID));
    SL->Add(UnicodeString(L"to=") + IntToStr(Lk->m_nDestID));
    SL->Add(UnicodeString(L"directionVisible=") + IntToStr(Lk->m_bDirection));
    SL->Add(UnicodeString(L"shape=") + IntToStr(Lk->m_nShape));
    UnicodeString lab;
    for (int k = 0; k < Lk->m_Labels->Count; k++) {
      if (k > 0) {
        lab += L",";
      }
      lab += IntToStr(Lk->m_Labels->GetLabel(k));
    }
    SL->Add(UnicodeString(L"labels=") + lab);
    SL->Add(UnicodeString(L"name=") + Lk->m_Name);
    SL->Add(L"[[END:LINK]]");
  }
  SL->Add(L"[[END:LINKS]]");
  SL->Add(L"");

  SL->Add(UnicodeString(L"[[CARDS focus=") + IntToStr(m_nCardID) + L"]]");
  for (int ci = 0; ci < m_Cards->Count; ci++) {
    TCard *Card = GetCardByIndex(ci);
    SL->Add(UnicodeString(L"[[CARD id=") + IntToStr(Card->m_nID) + L"]]");
    SL->Add(UnicodeString(L"title=") + Card->m_Title);
    SL->Add(UnicodeString(L"visible=") + IntToStr(Card->m_bVisible ? 1 : 0));
    SL->Add(UnicodeString(L"shape=") + IntToStr(Card->m_nShape));
    SL->Add(UnicodeString(L"size=") + IntToStr(Card->m_nSize));
    SL->Add(UnicodeString(L"top=") + IntToStr(Card->m_bTop ? 1 : 0));
    SL->Add(UnicodeString(L"fixed=") + IntToStr(Card->m_bFixed ? 1 : 0));
    SL->Add(UnicodeString(L"fold=") + IntToStr(Card->m_bFold ? 1 : 0));
    SL->Add(UnicodeString(L"x=") + FloatToStr(Card->m_fX));
    SL->Add(UnicodeString(L"y=") + FloatToStr(Card->m_fY));
    SL->Add(UnicodeString(L"created=") + DateTimeToStr(Card->m_fCreated));
    SL->Add(UnicodeString(L"updated=") + DateTimeToStr(Card->m_fUpdated));
    SL->Add(UnicodeString(L"viewed=") + DateTimeToStr(Card->m_fViewed));
    UnicodeString clist;
    for (int k = 0; k < Card->m_Labels->Count; k++) {
      if (k > 0) {
        clist += L",";
      }
      clist += IntToStr(Card->m_Labels->GetLabel(k));
    }
    SL->Add(UnicodeString(L"labelList=") + clist);
    SL->Add(UnicodeString(L"drawing=") + Card->m_Drawing->Encode(true));

    // Token labels the pair of marker lines; same token after manual body edits.
    UnicodeString token;
    if (!Fip2MakeTokenForCard(Card, token)) {
      token = L"DEADBEEFCAFEBABE";
    }
    SL->Add(UnicodeString(L"<!--BODY token=") + token + L"-->");
    for (int li = 0; li < Card->m_Lines->Count; li++) {
      SL->Add(Card->m_Lines->Strings[li]);
    }
    SL->Add(UnicodeString(L"<!--END_BODY token=") + token + L"-->");
    SL->Add(L"[[END:CARD]]");
  }
  SL->Add(L"[[END:CARDS]]");
  SL->Add(L"");

  return true;
}
