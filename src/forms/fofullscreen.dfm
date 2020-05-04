object Fo_FullScreen: TFo_FullScreen
  Left = 198
  Top = 267
  BorderStyle = bsNone
  Caption = 'Full Screen Window'
  ClientHeight = 99
  ClientWidth = 180
  Color = clBtnFace
  Font.Charset = SHIFTJIS_CHARSET
  Font.Color = clWindowText
  Font.Height = -12
  Font.Name = #65325#65331' '#65328#12468#12471#12483#12463
  Font.Style = []
  FormStyle = fsStayOnTop
  KeyPreview = True
  OldCreateOrder = False
  Position = poDefault
  OnResize = FormResize
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 12
  object Bu_Enter: TButton
    Left = 9
    Top = 10
    Width = 75
    Height = 25
    Caption = 'Enter'
    Default = True
    TabOrder = 0
  end
  object Bu_Dummy: TButton
    Left = 8
    Top = 48
    Width = 75
    Height = 25
    Caption = 'Dummy'
    TabOrder = 1
  end
end
