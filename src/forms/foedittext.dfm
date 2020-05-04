object Fo_EditText: TFo_EditText
  Left = 376
  Top = 633
  BorderStyle = bsDialog
  Caption = 'Edit Text'
  ClientHeight = 73
  ClientWidth = 466
  Color = clBtnFace
  Font.Charset = SHIFTJIS_CHARSET
  Font.Color = clWindowText
  Font.Height = -12
  Font.Name = #65325#65331' '#65328#12468#12471#12483#12463
  Font.Style = []
  OldCreateOrder = False
  Position = poOwnerFormCenter
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 12
  object La_Text: TLabel
    Left = 8
    Top = 16
    Width = 25
    Height = 12
    Caption = 'Text:'
  end
  object Ed_Text: TEdit
    Left = 80
    Top = 10
    Width = 297
    Height = 20
    TabOrder = 0
  end
  object Bu_OK: TButton
    Left = 384
    Top = 8
    Width = 75
    Height = 25
    Caption = '&OK'
    Default = True
    ModalResult = 1
    TabOrder = 1
  end
  object Bu_Cancel: TButton
    Left = 384
    Top = 40
    Width = 75
    Height = 25
    Cancel = True
    Caption = '&Cancel'
    ModalResult = 2
    TabOrder = 2
    OnClick = Bu_CancelClick
  end
end
