object Fo_Information: TFo_Information
  Left = 189
  Top = 121
  Caption = 'Information'
  ClientHeight = 310
  ClientWidth = 367
  Color = clBtnFace
  Font.Charset = SHIFTJIS_CHARSET
  Font.Color = clWindowText
  Font.Height = -12
  Font.Name = #65325#65331' '#65328#12468#12471#12483#12463
  Font.Style = []
  FormStyle = fsStayOnTop
  OnClose = FormClose
  OnShow = FormShow
  TextHeight = 12
  object WB: TCppWebBrowser
    Left = 0
    Top = 0
    Width = 225
    Height = 193
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
    Top = 286
    Width = 367
    Height = 24
    Align = alBottom
    TabOrder = 1
    object Ch_DontCheckNew: TCheckBox
      Left = 8
      Top = 4
      Width = 265
      Height = 17
      Caption = 'Do not check latest information automatically'
      TabOrder = 0
      OnClick = Ch_DontCheckNewClick
    end
  end
end
