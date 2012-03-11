object Fo_Progress: TFo_Progress
  Left = 192
  Top = 113
  BorderIcons = []
  BorderStyle = bsNone
  Caption = 'Progress'
  ClientHeight = 88
  ClientWidth = 425
  Color = clBtnFace
  Font.Charset = SHIFTJIS_CHARSET
  Font.Color = clWindowText
  Font.Height = -12
  Font.Name = #65325#65331' '#65328#12468#12471#12483#12463
  Font.Style = []
  KeyPreview = True
  OldCreateOrder = False
  Position = poDefault
  OnActivate = FormActivate
  OnCreate = FormCreate
  OnKeyDown = FormKeyDown
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 12
  object Be_Progress: TBevel
    Left = 0
    Top = 0
    Width = 153
    Height = 41
    Style = bsRaised
  end
  object La_Progress: TLabel
    Left = 8
    Top = 8
    Width = 401
    Height = 12
    Alignment = taCenter
    AutoSize = False
  end
  object Bu_Cancel: TButton
    Left = 168
    Top = 48
    Width = 81
    Height = 25
    Caption = '&Cancel'
    Default = True
    TabOrder = 0
    OnClick = Bu_CancelClick
  end
  object PG_Progress: TProgressBar
    Left = 8
    Top = 24
    Width = 401
    Height = 16
    Max = 10000
    TabOrder = 1
  end
  object Ti_Check: TTimer
    Interval = 20
    OnTimer = Ti_CheckTimer
    Left = 8
    Top = 56
  end
end
