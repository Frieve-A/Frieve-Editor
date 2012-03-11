object Fo_Select: TFo_Select
  Left = 930
  Top = 160
  BorderStyle = bsDialog
  ClientHeight = 265
  ClientWidth = 225
  Color = clBtnFace
  Font.Charset = SHIFTJIS_CHARSET
  Font.Color = clWindowText
  Font.Height = -12
  Font.Name = #65325#65331' '#65328#12468#12471#12483#12463
  Font.Style = []
  OldCreateOrder = False
  Position = poOwnerFormCenter
  OnCreate = FormCreate
  OnDestroy = FormDestroy
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 12
  object Bu_OK: TButton
    Left = 64
    Top = 232
    Width = 75
    Height = 25
    Caption = '&OK'
    Default = True
    Enabled = False
    ModalResult = 1
    TabOrder = 0
    OnClick = Bu_OKClick
  end
  object Bu_Cancel: TButton
    Left = 144
    Top = 232
    Width = 75
    Height = 25
    Cancel = True
    Caption = '&Cancel'
    ModalResult = 2
    TabOrder = 1
  end
  object LB_Items: TListBox
    Left = 8
    Top = 8
    Width = 209
    Height = 217
    ItemHeight = 12
    TabOrder = 2
    OnClick = LB_ItemsClick
    OnDblClick = LB_ItemsDblClick
  end
end
