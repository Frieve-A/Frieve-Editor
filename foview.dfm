object Fo_View: TFo_View
  Left = 405
  Top = 121
  AlphaBlendValue = 224
  BorderStyle = bsToolWindow
  Caption = 'View'
  ClientHeight = 224
  ClientWidth = 545
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
  OnCreate = FormCreate
  OnDeactivate = FormDeactivate
  OnKeyDown = FormKeyDown
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 12
  object PC_View: TPageControl
    Left = 0
    Top = 0
    Width = 545
    Height = 225
    ActivePage = TS_Limitation
    TabOrder = 0
    object TS_Label: TTabSheet
      Caption = 'Label'
      ExplicitLeft = 0
      ExplicitTop = 0
      ExplicitWidth = 0
      ExplicitHeight = 0
      object La_Label: TLabel
        Left = 8
        Top = 8
        Width = 29
        Height = 12
        Caption = 'Label:'
      end
      object La_LabelShow: TLabel
        Left = 144
        Top = 24
        Width = 29
        Height = 12
        Caption = 'Show:'
      end
      object La_LabelHide: TLabel
        Left = 240
        Top = 24
        Width = 25
        Height = 12
        Caption = 'Hide:'
      end
      object La_LabelFold: TLabel
        Left = 48
        Top = 24
        Width = 24
        Height = 12
        Caption = 'Fold:'
      end
      object La_LinkLabel: TLabel
        Left = 344
        Top = 8
        Width = 54
        Height = 12
        Caption = 'Link Label:'
      end
      object La_LinkLabelShow: TLabel
        Left = 344
        Top = 24
        Width = 29
        Height = 12
        Caption = 'Show:'
      end
      object La_LinkLabelHide: TLabel
        Left = 440
        Top = 24
        Width = 25
        Height = 12
        Caption = 'Hide:'
      end
      object Bevel1: TBevel
        Left = 336
        Top = 8
        Width = 17
        Height = 185
        Shape = bsLeftLine
      end
      object Bu_LabelShowAll: TButton
        Left = 176
        Top = 20
        Width = 25
        Height = 17
        Caption = 'All'
        TabOrder = 0
        OnClick = Bu_LabelShowAllClick
      end
      object Bu_LabelShowClear: TButton
        Tag = 1
        Left = 200
        Top = 20
        Width = 33
        Height = 17
        Caption = 'Clear'
        TabOrder = 1
        OnClick = Bu_LabelShowAllClick
      end
      object Bu_LabelHideAll: TButton
        Left = 272
        Top = 20
        Width = 25
        Height = 17
        Caption = 'All'
        TabOrder = 2
        OnClick = Bu_LabelHideAllClick
      end
      object Bu_LabelFoldAll: TButton
        Left = 80
        Top = 20
        Width = 25
        Height = 17
        Caption = 'All'
        TabOrder = 3
        OnClick = Bu_LabelFoldAllClick
      end
      object Bu_LabelClearAll: TButton
        Tag = 1
        Left = 104
        Top = 20
        Width = 33
        Height = 17
        Caption = 'Clear'
        TabOrder = 4
        OnClick = Bu_LabelFoldAllClick
      end
      object Bu_LabelHideClear: TButton
        Tag = 1
        Left = 296
        Top = 20
        Width = 33
        Height = 17
        Caption = 'Clear'
        TabOrder = 5
        OnClick = Bu_LabelHideAllClick
      end
      object SG_Labels: TStringGrid
        Left = 8
        Top = 40
        Width = 321
        Height = 153
        ColCount = 6
        DefaultColWidth = 40
        DefaultRowHeight = 16
        FixedCols = 0
        Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goRangeSelect, goThumbTracking]
        ScrollBars = ssVertical
        TabOrder = 6
        OnDrawCell = SG_LabelsDrawCell
        OnKeyDown = SG_LabelsKeyDown
        OnMouseDown = SG_LabelsMouseDown
        ColWidths = (
          93
          40
          40
          40
          40
          40)
      end
      object Bu_LinkLabelShowAll: TButton
        Tag = 2
        Left = 376
        Top = 20
        Width = 25
        Height = 17
        Caption = 'All'
        TabOrder = 7
        OnClick = Bu_LabelShowAllClick
      end
      object Bu_LinkLabelShowClear: TButton
        Tag = 3
        Left = 400
        Top = 20
        Width = 33
        Height = 17
        Caption = 'Clear'
        TabOrder = 8
        OnClick = Bu_LabelShowAllClick
      end
      object Bu_LinkLabelHideAll: TButton
        Tag = 2
        Left = 472
        Top = 20
        Width = 25
        Height = 17
        Caption = 'All'
        TabOrder = 9
        OnClick = Bu_LabelHideAllClick
      end
      object Bu_LinkLabelHideClear: TButton
        Tag = 3
        Left = 496
        Top = 20
        Width = 33
        Height = 17
        Caption = 'Clear'
        TabOrder = 10
        OnClick = Bu_LabelHideAllClick
      end
      object SG_LinkLabels: TStringGrid
        Tag = 1
        Left = 344
        Top = 39
        Width = 185
        Height = 154
        ColCount = 3
        DefaultColWidth = 40
        DefaultRowHeight = 16
        FixedCols = 0
        Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goRangeSelect, goThumbTracking]
        ScrollBars = ssVertical
        TabOrder = 11
        OnDrawCell = SG_LabelsDrawCell
        OnMouseDown = SG_LabelsMouseDown
        ColWidths = (
          80
          40
          40)
      end
    end
    object TS_Limitation: TTabSheet
      Caption = 'Limitation'
      ImageIndex = 1
      ExplicitLeft = 0
      ExplicitTop = 0
      ExplicitWidth = 0
      ExplicitHeight = 0
      object GB_SizeLimitation: TGroupBox
        Left = 8
        Top = 8
        Width = 521
        Height = 57
        TabOrder = 1
        object La_Size: TLabel
          Left = 96
          Top = 26
          Width = 21
          Height = 12
          Caption = 'Size'
        end
        object TB_SizeLimitation: TTrackBar
          Left = 8
          Top = 23
          Width = 81
          Height = 23
          Max = 8
          Min = -8
          PageSize = 1
          TabOrder = 0
          ThumbLength = 15
          OnChange = TB_SizeLimitationChange
        end
      end
      object Ch_SizeLimitation: TCheckBox
        Left = 16
        Top = 8
        Width = 115
        Height = 17
        Caption = 'Size Limitation'
        TabOrder = 0
        OnClick = Ch_SizeLimitationClick
      end
      object GB_LinkLimitation: TGroupBox
        Left = 8
        Top = 72
        Width = 521
        Height = 57
        TabOrder = 3
        object La_Links: TLabel
          Left = 96
          Top = 26
          Width = 27
          Height = 12
          Caption = 'Links'
        end
        object La_LinkTarget: TLabel
          Left = 367
          Top = 26
          Width = 35
          Height = 12
          Caption = 'Target:'
        end
        object TB_LinkLimitation: TTrackBar
          Left = 8
          Top = 23
          Width = 81
          Height = 23
          Max = 11
          Min = 1
          PageSize = 1
          Position = 1
          TabOrder = 0
          ThumbLength = 15
          OnChange = TB_LinkLimitationChange
        end
        object RB_LinkForward: TRadioButton
          Left = 217
          Top = 24
          Width = 65
          Height = 17
          Caption = 'Forward'
          Checked = True
          TabOrder = 2
          TabStop = True
          OnClick = RB_LinkForwardClick
        end
        object RB_LinkBackward: TRadioButton
          Left = 288
          Top = 24
          Width = 73
          Height = 17
          Caption = 'Backward'
          TabOrder = 3
          OnClick = RB_LinkForwardClick
        end
        object Ch_LinkDirection: TCheckBox
          Left = 145
          Top = 24
          Width = 65
          Height = 17
          Caption = 'Direction'
          TabOrder = 1
          OnClick = Ch_LinkDirectionClick
        end
        object Bu_LinkTarget: TButton
          Left = 416
          Top = 24
          Width = 97
          Height = 17
          Caption = 'No Assign'
          TabOrder = 4
          OnClick = Bu_LinkTargetClick
        end
      end
      object GB_DateLimitation: TGroupBox
        Left = 8
        Top = 136
        Width = 521
        Height = 57
        TabOrder = 5
        object La_DateType: TLabel
          Left = 104
          Top = 28
          Width = 55
          Height = 12
          Caption = 'Date Type:'
        end
        object La_DateLimitationType: TLabel
          Left = 256
          Top = 28
          Width = 109
          Height = 12
          Caption = 'Date Limitation Type:'
        end
        object TB_DateLimitation: TTrackBar
          Left = 8
          Top = 23
          Width = 81
          Height = 23
          Max = 100
          PageSize = 1
          Frequency = 10
          TabOrder = 0
          ThumbLength = 15
          OnChange = TB_DateLimitationChange
        end
        object Co_DateType: TComboBox
          Left = 168
          Top = 24
          Width = 73
          Height = 20
          Style = csDropDownList
          ItemHeight = 12
          TabOrder = 1
          OnChange = Co_DateTypeChange
          Items.Strings = (
            'Created'
            'Edited'
            'Viewed')
        end
        object Co_DateLimitationType: TComboBox
          Left = 368
          Top = 24
          Width = 81
          Height = 20
          Style = csDropDownList
          ItemHeight = 12
          TabOrder = 2
          OnChange = Co_DateLimitationTypeChange
          Items.Strings = (
            'Old'
            'New'
            'Near'
            'Older'
            'Newer')
        end
      end
      object Ch_DateLimitation: TCheckBox
        Left = 16
        Top = 136
        Width = 115
        Height = 17
        Caption = 'Date Limitation'
        TabOrder = 4
        OnClick = Ch_DateLimitationClick
      end
      object Ch_LinkLimitation: TCheckBox
        Left = 16
        Top = 72
        Width = 115
        Height = 17
        Caption = 'Link Limitation'
        TabOrder = 2
        OnClick = Ch_LinkLimitationClick
      end
    end
  end
  object PM_LinkTarget: TPopupMenu
    Left = 496
    Top = 128
    object PT_NoAssign: TMenuItem
      Caption = '&No Assign'
      OnClick = PT_NoAssignClick
    end
    object N1: TMenuItem
      Caption = '-'
    end
    object PT_Select: TMenuItem
      Caption = '&Select...'
      OnClick = PT_SelectClick
    end
  end
end
