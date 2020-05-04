object Fo_Search: TFo_Search
  Left = 660
  Top = 129
  AlphaBlendValue = 192
  BorderStyle = bsDialog
  Caption = 'Search'
  ClientHeight = 136
  ClientWidth = 651
  Color = clBtnFace
  Font.Charset = SHIFTJIS_CHARSET
  Font.Color = clWindowText
  Font.Height = -15
  Font.Name = #65325#65331' '#65328#12468#12471#12483#12463
  Font.Style = []
  FormStyle = fsStayOnTop
  KeyPreview = True
  OldCreateOrder = False
  Position = poOwnerFormCenter
  OnCreate = FormCreate
  OnKeyDown = FormKeyDown
  OnShow = FormShow
  PixelsPerInch = 120
  TextHeight = 15
  object La_Keyword: TLabel
    Left = 10
    Top = 20
    Width = 57
    Height = 15
    Margins.Left = 4
    Margins.Top = 4
    Margins.Right = 4
    Margins.Bottom = 4
    Caption = 'Keyword:'
  end
  object La_ReplaceTo: TLabel
    Left = 10
    Top = 49
    Width = 71
    Height = 15
    Margins.Left = 4
    Margins.Top = 4
    Margins.Right = 4
    Margins.Bottom = 4
    Caption = 'Replace to:'
    Visible = False
  end
  object Bu_FindNext: TButton
    Left = 548
    Top = 10
    Width = 93
    Height = 31
    Margins.Left = 4
    Margins.Top = 4
    Margins.Right = 4
    Margins.Bottom = 4
    Caption = 'Find &Next'
    Default = True
    TabOrder = 2
    OnClick = Bu_FindNextClick
  end
  object Bu_Cancel: TButton
    Left = 548
    Top = 50
    Width = 93
    Height = 31
    Margins.Left = 4
    Margins.Top = 4
    Margins.Right = 4
    Margins.Bottom = 4
    Cancel = True
    Caption = '&Cancel'
    ModalResult = 2
    TabOrder = 3
    OnClick = Bu_CancelClick
  end
  object Ch_CardTitle: TCheckBox
    Left = 100
    Top = 79
    Width = 121
    Height = 21
    Margins.Left = 4
    Margins.Top = 4
    Margins.Right = 4
    Margins.Bottom = 4
    Caption = 'Card Title'
    Checked = True
    State = cbChecked
    TabOrder = 4
  end
  object Ch_Text: TCheckBox
    Left = 230
    Top = 79
    Width = 121
    Height = 21
    Margins.Left = 4
    Margins.Top = 4
    Margins.Right = 4
    Margins.Bottom = 4
    Caption = 'Text'
    Checked = True
    State = cbChecked
    TabOrder = 5
  end
  object Ch_MatchCase: TCheckBox
    Left = 359
    Top = 79
    Width = 172
    Height = 21
    Margins.Left = 4
    Margins.Top = 4
    Margins.Right = 4
    Margins.Bottom = 4
    Caption = 'Match Case'
    TabOrder = 6
  end
  object Ch_GlobalSearch: TCheckBox
    Left = 230
    Top = 109
    Width = 121
    Height = 21
    Margins.Left = 4
    Margins.Top = 4
    Margins.Right = 4
    Margins.Bottom = 4
    Caption = 'Global Search'
    Checked = True
    State = cbChecked
    TabOrder = 8
  end
  object Ch_Backward: TCheckBox
    Left = 359
    Top = 108
    Width = 172
    Height = 21
    Margins.Left = 4
    Margins.Top = 4
    Margins.Right = 4
    Margins.Bottom = 4
    Caption = 'Backward'
    TabOrder = 9
  end
  object Co_Keyword: TComboBox
    Left = 100
    Top = 13
    Width = 431
    Height = 20
    Margins.Left = 4
    Margins.Top = 4
    Margins.Right = 4
    Margins.Bottom = 4
    AutoComplete = False
    PopupMenu = Po_Search
    TabOrder = 0
    OnChange = Co_KeywordChange
  end
  object Ed_Keyword: TEdit
    Left = 109
    Top = 20
    Width = 431
    Height = 20
    Margins.Left = 4
    Margins.Top = 4
    Margins.Right = 4
    Margins.Bottom = 4
    TabStop = False
    PopupMenu = Po_Search
    TabOrder = 10
    Visible = False
    OnChange = Ed_KeywordChange
  end
  object Ch_InvisibleCard: TCheckBox
    Left = 100
    Top = 109
    Width = 121
    Height = 21
    Margins.Left = 4
    Margins.Top = 4
    Margins.Right = 4
    Margins.Bottom = 4
    Caption = 'Invisible Card'
    Checked = True
    State = cbChecked
    TabOrder = 7
  end
  object Ed_ReplaceTo: TEdit
    Left = 100
    Top = 45
    Width = 431
    Height = 20
    Margins.Left = 4
    Margins.Top = 4
    Margins.Right = 4
    Margins.Bottom = 4
    PopupMenu = Po_Search
    TabOrder = 1
    Visible = False
    OnChange = Ed_KeywordChange
  end
  object Po_Search: TPopupMenu
    AutoPopup = False
    Left = 88
    Top = 16
    object PM_Undo: TMenuItem
      Caption = '&Undo'
      ShortCut = 16474
      OnClick = PM_UndoClick
    end
    object PM_Cut: TMenuItem
      Caption = 'Cu&t'
      ShortCut = 16472
      OnClick = PM_CutClick
    end
    object PM_Copy: TMenuItem
      Caption = '&Copy'
      ShortCut = 16451
      OnClick = PM_CopyClick
    end
    object PM_Paste: TMenuItem
      Caption = '&Paste'
      ShortCut = 16470
      OnClick = PM_PasteClick
    end
    object PM_SelectAll: TMenuItem
      Caption = 'Select &All'
      ShortCut = 16449
      OnClick = PM_SelectAllClick
    end
  end
end
