object Fo_Shape: TFo_Shape
  Left = 189
  Top = 115
  BorderStyle = bsNone
  ClientHeight = 446
  ClientWidth = 920
  Color = clBtnFace
  Font.Charset = SHIFTJIS_CHARSET
  Font.Color = clWindowText
  Font.Height = -12
  Font.Name = #65325#65331' '#65328#12468#12471#12483#12463
  Font.Style = []
  FormStyle = fsStayOnTop
  OldCreateOrder = False
  OnCreate = FormCreate
  OnDeactivate = FormDeactivate
  OnKeyDown = FormKeyDown
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 12
  object Pa: TPanel
    Left = 0
    Top = 0
    Width = 193
    Height = 121
    TabOrder = 0
    object PB: TPaintBox
      Left = 1
      Top = 1
      Width = 191
      Height = 119
      Align = alClient
      OnMouseDown = PBMouseDown
      OnMouseMove = PBMouseMove
      OnPaint = PBPaint
    end
  end
end
