object Fo_Search: TFo_Search
  Left = 660
  Top = 129
  AlphaBlendValue = 192
  BorderStyle = bsDialog
  Caption = 'Search'
  ClientHeight = 109
  ClientWidth = 521
  Color = clBtnFace
  Font.Charset = SHIFTJIS_CHARSET
  Font.Color = clWindowText
  Font.Height = -12
  Font.Name = #65325#65331' '#65328#12468#12471#12483#12463
  Font.Style = []
  FormStyle = fsStayOnTop
  KeyPreview = True
  OldCreateOrder = False
  Position = poOwnerFormCenter
  OnCreate = FormCreate
  OnKeyDown = FormKeyDown
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 12
  object La_Keyword: TLabel
    Left = 8
    Top = 16
    Width = 45
    Height = 12
    Caption = 'Keyword:'
  end
  object La_ReplaceTo: TLabel
    Left = 8
    Top = 39
    Width = 57
    Height = 12
    Caption = 'Replace to:'
    Visible = False
  end
  object Bu_FindNext: TButton
    Left = 438
    Top = 8
    Width = 75
    Height = 25
    Caption = 'Find &Next'
    Default = True
    TabOrder = 2
    OnClick = Bu_FindNextClick
  end
  object Bu_Cancel: TButton
    Left = 438
    Top = 40
    Width = 75
    Height = 25
    Cancel = True
    Caption = '&Cancel'
    ModalResult = 2
    TabOrder = 3
    OnClick = Bu_CancelClick
  end
  object Ch_CardTitle: TCheckBox
    Left = 80
    Top = 63
    Width = 97
    Height = 17
    Caption = 'Card Title'
    Checked = True
    State = cbChecked
    TabOrder = 4
  end
  object Ch_Text: TCheckBox
    Left = 184
    Top = 63
    Width = 97
    Height = 17
    Caption = 'Text'
    Checked = True
    State = cbChecked
    TabOrder = 5
  end
  object Ch_MatchCase: TCheckBox
    Left = 287
    Top = 63
    Width = 138
    Height = 17
    Caption = 'Match Case'
    TabOrder = 6
  end
  object Ch_GlobalSearch: TCheckBox
    Left = 184
    Top = 87
    Width = 97
    Height = 17
    Caption = 'Global Search'
    Checked = True
    State = cbChecked
    TabOrder = 8
  end
  object Ch_Backward: TCheckBox
    Left = 287
    Top = 86
    Width = 138
    Height = 17
    Caption = 'Backward'
    TabOrder = 9
  end
  object Co_Keyword: TComboBox
    Left = 80
    Top = 10
    Width = 345
    Height = 20
    AutoComplete = False
    ItemHeight = 12
    PopupMenu = Po_Search
    TabOrder = 0
    OnChange = Co_KeywordChange
  end
  object Ed_Keyword: TEdit
    Left = 87
    Top = 16
    Width = 345
    Height = 20
    TabStop = False
    PopupMenu = Po_Search
    TabOrder = 10
    Visible = False
    OnChange = Ed_KeywordChange
  end
  object Ch_InvisibleCard: TCheckBox
    Left = 80
    Top = 87
    Width = 97
    Height = 17
    Caption = 'Invisible Card'
    Checked = True
    State = cbChecked
    TabOrder = 7
  end
  object Ed_ReplaceTo: TEdit
    Left = 80
    Top = 36
    Width = 345
    Height = 20
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
