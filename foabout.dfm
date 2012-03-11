object Fo_About: TFo_About
  Left = 323
  Top = 541
  BorderIcons = [biSystemMenu]
  BorderStyle = bsDialog
  Caption = 'About'
  ClientHeight = 81
  ClientWidth = 313
  Color = clBtnFace
  Font.Charset = SHIFTJIS_CHARSET
  Font.Color = clWindowText
  Font.Height = -12
  Font.Name = #65325#65331' '#65328#12468#12471#12483#12463
  Font.Style = []
  OldCreateOrder = True
  Position = poScreenCenter
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 12
  object ProgramIcon: TImage
    Left = 8
    Top = 8
    Width = 33
    Height = 33
    Stretch = True
    IsControl = True
  end
  object ProductName: TLabel
    Left = 56
    Top = 8
    Width = 43
    Height = 12
    Caption = 'AppTitle'
    IsControl = True
  end
  object Copyright: TLabel
    Left = 56
    Top = 28
    Width = 163
    Height = 12
    Caption = 'Copyright (C) Frieve 2006-2008'
    IsControl = True
  end
  object Comments: TLabel
    Left = 56
    Top = 48
    Width = 4
    Height = 12
    WordWrap = True
    IsControl = True
  end
  object Label1: TLabel
    Left = 56
    Top = 44
    Width = 118
    Height = 12
    Caption = 'http://www.frieve.com/'
    IsControl = True
  end
  object Bu_OK: TButton
    Left = 231
    Top = 48
    Width = 75
    Height = 25
    Cancel = True
    Caption = '&OK'
    Default = True
    ModalResult = 1
    TabOrder = 0
  end
end
