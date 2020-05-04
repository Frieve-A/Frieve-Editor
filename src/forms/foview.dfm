object Fo_View: TFo_View
  Left = 405
  Top = 121
  AlphaBlendValue = 224
  BorderStyle = bsToolWindow
  Caption = 'View'
  ClientHeight = 280
  ClientWidth = 681
  Color = clBtnFace
  Font.Charset = SHIFTJIS_CHARSET
  Font.Color = clWindowText
  Font.Height = -15
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
  PixelsPerInch = 120
  TextHeight = 15
  object PC_View: TPageControl
    Left = 0
    Top = 0
    Width = 681
    Height = 281
    Margins.Left = 4
    Margins.Top = 4
    Margins.Right = 4
    Margins.Bottom = 4
    ActivePage = TS_Limitation
    TabOrder = 0
    object TS_Label: TTabSheet
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      Caption = 'Label'
      ExplicitLeft = 0
      ExplicitTop = 0
      ExplicitWidth = 0
      ExplicitHeight = 0
      object La_Label: TLabel
        Left = 10
        Top = 10
        Width = 36
        Height = 15
        Margins.Left = 4
        Margins.Top = 4
        Margins.Right = 4
        Margins.Bottom = 4
        Caption = 'Label:'
      end
      object La_LabelShow: TLabel
        Left = 180
        Top = 30
        Width = 38
        Height = 15
        Margins.Left = 4
        Margins.Top = 4
        Margins.Right = 4
        Margins.Bottom = 4
        Caption = 'Show:'
      end
      object La_LabelHide: TLabel
        Left = 300
        Top = 30
        Width = 31
        Height = 15
        Margins.Left = 4
        Margins.Top = 4
        Margins.Right = 4
        Margins.Bottom = 4
        Caption = 'Hide:'
      end
      object La_LabelFold: TLabel
        Left = 60
        Top = 30
        Width = 29
        Height = 15
        Margins.Left = 4
        Margins.Top = 4
        Margins.Right = 4
        Margins.Bottom = 4
        Caption = 'Fold:'
      end
      object La_LinkLabel: TLabel
        Left = 430
        Top = 10
        Width = 67
        Height = 15
        Margins.Left = 4
        Margins.Top = 4
        Margins.Right = 4
        Margins.Bottom = 4
        Caption = 'Link Label:'
      end
      object La_LinkLabelShow: TLabel
        Left = 430
        Top = 30
        Width = 38
        Height = 15
        Margins.Left = 4
        Margins.Top = 4
        Margins.Right = 4
        Margins.Bottom = 4
        Caption = 'Show:'
      end
      object La_LinkLabelHide: TLabel
        Left = 550
        Top = 30
        Width = 31
        Height = 15
        Margins.Left = 4
        Margins.Top = 4
        Margins.Right = 4
        Margins.Bottom = 4
        Caption = 'Hide:'
      end
      object Bevel1: TBevel
        Left = 420
        Top = 10
        Width = 21
        Height = 231
        Margins.Left = 4
        Margins.Top = 4
        Margins.Right = 4
        Margins.Bottom = 4
        Shape = bsLeftLine
      end
      object Bu_LabelShowAll: TButton
        Left = 220
        Top = 25
        Width = 31
        Height = 21
        Margins.Left = 4
        Margins.Top = 4
        Margins.Right = 4
        Margins.Bottom = 4
        Caption = 'All'
        TabOrder = 0
        OnClick = Bu_LabelShowAllClick
      end
      object Bu_LabelShowClear: TButton
        Tag = 1
        Left = 250
        Top = 25
        Width = 41
        Height = 21
        Margins.Left = 4
        Margins.Top = 4
        Margins.Right = 4
        Margins.Bottom = 4
        Caption = 'Clear'
        TabOrder = 1
        OnClick = Bu_LabelShowAllClick
      end
      object Bu_LabelHideAll: TButton
        Left = 340
        Top = 25
        Width = 31
        Height = 21
        Margins.Left = 4
        Margins.Top = 4
        Margins.Right = 4
        Margins.Bottom = 4
        Caption = 'All'
        TabOrder = 2
        OnClick = Bu_LabelHideAllClick
      end
      object Bu_LabelFoldAll: TButton
        Left = 100
        Top = 25
        Width = 31
        Height = 21
        Margins.Left = 4
        Margins.Top = 4
        Margins.Right = 4
        Margins.Bottom = 4
        Caption = 'All'
        TabOrder = 3
        OnClick = Bu_LabelFoldAllClick
      end
      object Bu_LabelClearAll: TButton
        Tag = 1
        Left = 130
        Top = 25
        Width = 41
        Height = 21
        Margins.Left = 4
        Margins.Top = 4
        Margins.Right = 4
        Margins.Bottom = 4
        Caption = 'Clear'
        TabOrder = 4
        OnClick = Bu_LabelFoldAllClick
      end
      object Bu_LabelHideClear: TButton
        Tag = 1
        Left = 370
        Top = 25
        Width = 41
        Height = 21
        Margins.Left = 4
        Margins.Top = 4
        Margins.Right = 4
        Margins.Bottom = 4
        Caption = 'Clear'
        TabOrder = 5
        OnClick = Bu_LabelHideAllClick
      end
      object SG_Labels: TStringGrid
        Left = 10
        Top = 50
        Width = 401
        Height = 191
        Margins.Left = 4
        Margins.Top = 4
        Margins.Right = 4
        Margins.Bottom = 4
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
        Left = 470
        Top = 25
        Width = 31
        Height = 21
        Margins.Left = 4
        Margins.Top = 4
        Margins.Right = 4
        Margins.Bottom = 4
        Caption = 'All'
        TabOrder = 7
        OnClick = Bu_LabelShowAllClick
      end
      object Bu_LinkLabelShowClear: TButton
        Tag = 3
        Left = 500
        Top = 25
        Width = 41
        Height = 21
        Margins.Left = 4
        Margins.Top = 4
        Margins.Right = 4
        Margins.Bottom = 4
        Caption = 'Clear'
        TabOrder = 8
        OnClick = Bu_LabelShowAllClick
      end
      object Bu_LinkLabelHideAll: TButton
        Tag = 2
        Left = 590
        Top = 25
        Width = 31
        Height = 21
        Margins.Left = 4
        Margins.Top = 4
        Margins.Right = 4
        Margins.Bottom = 4
        Caption = 'All'
        TabOrder = 9
        OnClick = Bu_LabelHideAllClick
      end
      object Bu_LinkLabelHideClear: TButton
        Tag = 3
        Left = 620
        Top = 25
        Width = 41
        Height = 21
        Margins.Left = 4
        Margins.Top = 4
        Margins.Right = 4
        Margins.Bottom = 4
        Caption = 'Clear'
        TabOrder = 10
        OnClick = Bu_LabelHideAllClick
      end
      object SG_LinkLabels: TStringGrid
        Tag = 1
        Left = 430
        Top = 49
        Width = 231
        Height = 192
        Margins.Left = 4
        Margins.Top = 4
        Margins.Right = 4
        Margins.Bottom = 4
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
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      Caption = 'Limitation'
      ImageIndex = 1
      ExplicitLeft = 0
      ExplicitTop = 0
      ExplicitWidth = 0
      ExplicitHeight = 0
      object GB_SizeLimitation: TGroupBox
        Left = 10
        Top = 10
        Width = 651
        Height = 71
        Margins.Left = 4
        Margins.Top = 4
        Margins.Right = 4
        Margins.Bottom = 4
        TabOrder = 1
        object La_Size: TLabel
          Left = 120
          Top = 33
          Width = 27
          Height = 15
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          Caption = 'Size'
        end
        object TB_SizeLimitation: TTrackBar
          Left = 10
          Top = 29
          Width = 101
          Height = 29
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          Max = 8
          Min = -8
          PageSize = 1
          TabOrder = 0
          ThumbLength = 15
          OnChange = TB_SizeLimitationChange
        end
      end
      object Ch_SizeLimitation: TCheckBox
        Left = 20
        Top = 10
        Width = 144
        Height = 21
        Margins.Left = 4
        Margins.Top = 4
        Margins.Right = 4
        Margins.Bottom = 4
        Caption = 'Size Limitation'
        TabOrder = 0
        OnClick = Ch_SizeLimitationClick
      end
      object GB_LinkLimitation: TGroupBox
        Left = 10
        Top = 90
        Width = 651
        Height = 71
        Margins.Left = 4
        Margins.Top = 4
        Margins.Right = 4
        Margins.Bottom = 4
        TabOrder = 3
        object La_Links: TLabel
          Left = 120
          Top = 33
          Width = 33
          Height = 15
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          Caption = 'Links'
        end
        object La_LinkTarget: TLabel
          Left = 459
          Top = 33
          Width = 44
          Height = 15
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          Caption = 'Target:'
        end
        object TB_LinkLimitation: TTrackBar
          Left = 10
          Top = 29
          Width = 101
          Height = 29
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          Max = 11
          Min = 1
          PageSize = 1
          Position = 1
          TabOrder = 0
          ThumbLength = 15
          OnChange = TB_LinkLimitationChange
        end
        object RB_LinkForward: TRadioButton
          Left = 271
          Top = 30
          Width = 82
          Height = 21
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          Caption = 'Forward'
          Checked = True
          TabOrder = 2
          TabStop = True
          OnClick = RB_LinkForwardClick
        end
        object RB_LinkBackward: TRadioButton
          Left = 360
          Top = 30
          Width = 91
          Height = 21
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          Caption = 'Backward'
          TabOrder = 3
          OnClick = RB_LinkForwardClick
        end
        object Ch_LinkDirection: TCheckBox
          Left = 181
          Top = 30
          Width = 82
          Height = 21
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          Caption = 'Direction'
          TabOrder = 1
          OnClick = Ch_LinkDirectionClick
        end
        object Bu_LinkTarget: TButton
          Left = 520
          Top = 30
          Width = 121
          Height = 21
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          Caption = 'No Assign'
          TabOrder = 4
          OnClick = Bu_LinkTargetClick
        end
      end
      object GB_DateLimitation: TGroupBox
        Left = 10
        Top = 170
        Width = 651
        Height = 71
        Margins.Left = 4
        Margins.Top = 4
        Margins.Right = 4
        Margins.Bottom = 4
        TabOrder = 5
        object La_DateType: TLabel
          Left = 130
          Top = 35
          Width = 69
          Height = 15
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          Caption = 'Date Type:'
        end
        object La_DateLimitationType: TLabel
          Left = 320
          Top = 35
          Width = 135
          Height = 15
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          Caption = 'Date Limitation Type:'
        end
        object TB_DateLimitation: TTrackBar
          Left = 10
          Top = 29
          Width = 101
          Height = 29
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          Max = 100
          PageSize = 1
          Frequency = 10
          TabOrder = 0
          ThumbLength = 15
          OnChange = TB_DateLimitationChange
        end
        object Co_DateType: TComboBox
          Left = 210
          Top = 30
          Width = 91
          Height = 23
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          Style = csDropDownList
          TabOrder = 1
          OnChange = Co_DateTypeChange
          Items.Strings = (
            'Created'
            'Edited'
            'Viewed')
        end
        object Co_DateLimitationType: TComboBox
          Left = 460
          Top = 30
          Width = 101
          Height = 23
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          Style = csDropDownList
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
        Left = 20
        Top = 170
        Width = 144
        Height = 21
        Margins.Left = 4
        Margins.Top = 4
        Margins.Right = 4
        Margins.Bottom = 4
        Caption = 'Date Limitation'
        TabOrder = 4
        OnClick = Ch_DateLimitationClick
      end
      object Ch_LinkLimitation: TCheckBox
        Left = 20
        Top = 90
        Width = 144
        Height = 21
        Margins.Left = 4
        Margins.Top = 4
        Margins.Right = 4
        Margins.Bottom = 4
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
