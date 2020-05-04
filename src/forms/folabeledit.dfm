object Fo_LabelEdit: TFo_LabelEdit
  Left = 205
  Top = 121
  BorderStyle = bsDialog
  Caption = 'Label Edit'
  ClientHeight = 243
  ClientWidth = 257
  Color = clBtnFace
  Font.Charset = SHIFTJIS_CHARSET
  Font.Color = clWindowText
  Font.Height = -12
  Font.Name = #65325#65331' '#65328#12468#12471#12483#12463
  Font.Style = []
  KeyPreview = True
  OldCreateOrder = False
  Position = poOwnerFormCenter
  OnClose = FormClose
  OnCreate = FormCreate
  OnKeyDown = FormKeyDown
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 12
  object Bu_OK: TButton
    Left = 176
    Top = 208
    Width = 75
    Height = 25
    Caption = '&OK'
    Default = True
    ModalResult = 1
    TabOrder = 3
  end
  object SG_Labels: TStringGrid
    Left = 8
    Top = 8
    Width = 241
    Height = 193
    ColCount = 2
    DefaultRowHeight = 18
    FixedCols = 0
    FixedRows = 0
    Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goRangeSelect, goRowSelect, goThumbTracking]
    ScrollBars = ssVertical
    TabOrder = 0
    OnDblClick = SG_LabelsDblClick
    OnDrawCell = SG_LabelsDrawCell
    OnSelectCell = SG_LabelsSelectCell
    ColWidths = (
      171
      64)
  end
  object Bu_Delete: TButton
    Left = 88
    Top = 208
    Width = 75
    Height = 25
    Caption = '&Delete'
    Enabled = False
    TabOrder = 2
    OnClick = Bu_DeleteClick
  end
  object Bu_New: TButton
    Left = 8
    Top = 208
    Width = 75
    Height = 25
    Caption = '&New'
    TabOrder = 1
    OnClick = Bu_NewClick
  end
  object CD: TColorDialog
    Left = 88
    Top = 208
  end
  object Ti_Check: TTimer
    Enabled = False
    Interval = 50
    OnTimer = Ti_CheckTimer
    Left = 48
    Top = 208
  end
end
