object Fo_About: TFo_About
  Left = 323
  Top = 541
  BorderIcons = [biSystemMenu]
  BorderStyle = bsDialog
  Caption = 'About'
  ClientHeight = 101
  ClientWidth = 391
  Color = clBtnFace
  Font.Charset = SHIFTJIS_CHARSET
  Font.Color = clWindowText
  Font.Height = -15
  Font.Name = #65325#65331' '#65328#12468#12471#12483#12463
  Font.Style = []
  OldCreateOrder = True
  Position = poScreenCenter
  OnShow = FormShow
  PixelsPerInch = 120
  TextHeight = 15
  object ProgramIcon: TImage
    Left = 10
    Top = 10
    Width = 41
    Height = 41
    Margins.Left = 4
    Margins.Top = 4
    Margins.Right = 4
    Margins.Bottom = 4
    Stretch = True
    IsControl = True
  end
  object ProductName: TLabel
    Left = 70
    Top = 10
    Width = 51
    Height = 15
    Margins.Left = 4
    Margins.Top = 4
    Margins.Right = 4
    Margins.Bottom = 4
    Caption = 'AppTitle'
    IsControl = True
  end
  object Copyright: TLabel
    Left = 70
    Top = 35
    Width = 206
    Height = 15
    Margins.Left = 4
    Margins.Top = 4
    Margins.Right = 4
    Margins.Bottom = 4
    Caption = 'Copyright (C) Frieve 2006-2020'
    IsControl = True
  end
  object Comments: TLabel
    Left = 70
    Top = 60
    Width = 5
    Height = 15
    Margins.Left = 4
    Margins.Top = 4
    Margins.Right = 4
    Margins.Bottom = 4
    WordWrap = True
    IsControl = True
  end
  object Label1: TLabel
    Left = 70
    Top = 55
    Width = 151
    Height = 15
    Margins.Left = 4
    Margins.Top = 4
    Margins.Right = 4
    Margins.Bottom = 4
    Caption = 'http://www.frieve.com/'
    IsControl = True
  end
  object Bu_OK: TButton
    Left = 289
    Top = 60
    Width = 94
    Height = 31
    Margins.Left = 4
    Margins.Top = 4
    Margins.Right = 4
    Margins.Bottom = 4
    Cancel = True
    Caption = '&OK'
    Default = True
    ModalResult = 1
    TabOrder = 0
  end
end
