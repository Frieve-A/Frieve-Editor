object Fo_Information: TFo_Information
  Left = 189
  Top = 121
  Caption = 'Information'
  ClientHeight = 388
  ClientWidth = 459
  Color = clBtnFace
  Font.Charset = SHIFTJIS_CHARSET
  Font.Color = clWindowText
  Font.Height = -15
  Font.Name = #65325#65331' '#65328#12468#12471#12483#12463
  Font.Style = []
  FormStyle = fsStayOnTop
  OldCreateOrder = False
  OnClose = FormClose
  OnShow = FormShow
  PixelsPerInch = 120
  TextHeight = 15
  object WB: TCppWebBrowser
    Left = 0
    Top = 0
    Width = 281
    Height = 241
    Margins.Left = 4
    Margins.Top = 4
    Margins.Right = 4
    Margins.Bottom = 4
    TabOrder = 0
    ControlData = {
      4C00000041170000F21300000000000000000000000000000000000000000000
      000000004C000000000000000000000001000000E0D057007335CF11AE690800
      2B2E126208000000000000004C0000000114020000000000C000000000000046
      8000000000000000000000000000000000000000000000000000000000000000
      00000000000000000100000000000000000000000000000000000000}
  end
  object Pa_Top_: TPanel
    Left = 0
    Top = 358
    Width = 459
    Height = 30
    Margins.Left = 4
    Margins.Top = 4
    Margins.Right = 4
    Margins.Bottom = 4
    Align = alBottom
    TabOrder = 1
    object Ch_DontCheckNew: TCheckBox
      Left = 10
      Top = 5
      Width = 331
      Height = 21
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      Caption = 'Do not check latest information automatically'
      TabOrder = 0
      OnClick = Ch_DontCheckNewClick
    end
  end
end
