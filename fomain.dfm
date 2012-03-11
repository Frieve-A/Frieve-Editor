object Fo_Main: TFo_Main
  Left = 0
  Top = 325
  ClientHeight = 448
  ClientWidth = 1370
  Color = clBtnFace
  Font.Charset = SHIFTJIS_CHARSET
  Font.Color = clWindowText
  Font.Height = -12
  Font.Name = #65325#65331' '#65328#12468#12471#12483#12463
  Font.Style = []
  KeyPreview = True
  Menu = MM_Menu
  OldCreateOrder = False
  Position = poDesigned
  WindowState = wsMaximized
  OnClose = FormClose
  OnCloseQuery = FormCloseQuery
  OnCreate = FormCreate
  OnDestroy = FormDestroy
  OnKeyDown = FormKeyDown
  OnKeyUp = FormKeyUp
  OnResize = FormResize
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 12
  object Sp_Left: TSplitter
    Left = 25
    Top = 33
    Height = 415
    AutoSnap = False
    Visible = False
    ExplicitHeight = 295
  end
  object Sp_Left2: TSplitter
    Left = 89
    Top = 33
    Height = 415
    AutoSnap = False
    ExplicitHeight = 295
  end
  object Sp_GlobalSearch: TSplitter
    Left = 1160
    Top = 33
    Height = 415
    Align = alRight
    AutoSnap = False
    Visible = False
    ExplicitLeft = 986
    ExplicitHeight = 295
  end
  object Pa_List: TPanel
    Left = 28
    Top = 33
    Width = 61
    Height = 415
    Align = alLeft
    BevelOuter = bvNone
    Constraints.MinWidth = 16
    TabOrder = 0
    object LB_List: TListBox
      Left = 0
      Top = 33
      Width = 89
      Height = 80
      ItemHeight = 12
      MultiSelect = True
      PopupMenu = PM_List
      TabOrder = 0
      OnClick = LB_ListClick
      OnDblClick = LB_ListDblClick
      OnMouseDown = LB_ListMouseDown
      OnMouseUp = LB_ListMouseUp
    end
    object Pa_ListBottom: TPanel
      Left = 0
      Top = 395
      Width = 61
      Height = 20
      Align = alBottom
      TabOrder = 2
      object La_Count: TLabel
        Left = 8
        Top = 4
        Width = 83
        Height = 12
        Caption = '1 Cards, 0 Links'
      end
    end
    object Pa_ListTop: TPanel
      Left = 0
      Top = 0
      Width = 61
      Height = 33
      Align = alTop
      TabOrder = 1
      OnResize = Pa_ListTopResize
      object La_Cards: TLabel
        Left = 8
        Top = 12
        Width = 32
        Height = 12
        Caption = 'Cards:'
      end
      object UD_Sort: TUpDown
        Left = 36
        Top = 4
        Width = 17
        Height = 25
        Min = -32768
        Max = 32767
        TabOrder = 0
        OnClick = UD_SortClick
        OnExit = UD_SortExit
      end
    end
  end
  object Pa_Client: TPanel
    Left = 96
    Top = 40
    Width = 1041
    Height = 265
    BevelOuter = bvNone
    TabOrder = 1
    object PC_Client: TPageControl
      Left = -1
      Top = 40
      Width = 1014
      Height = 209
      ActivePage = TS_Editor
      TabOrder = 0
      OnChange = PC_ClientChange
      object TS_Browser: TTabSheet
        Caption = 'Browser'
        ImageIndex = 1
        OnShow = TS_BrowserShow
        ExplicitLeft = 0
        ExplicitTop = 0
        ExplicitWidth = 0
        ExplicitHeight = 0
        object Im_Wall: TImage
          Left = 120
          Top = 47
          Width = 105
          Height = 105
          AutoSize = True
          Visible = False
        end
        object Sp_BrowserRight: TSplitter
          Left = 1003
          Top = 41
          Height = 120
          Align = alRight
          AutoSnap = False
          Visible = False
          OnMoved = Sp_BrowserRightMoved
          ExplicitLeft = 846
          ExplicitHeight = 121
        end
        object Sp_BrowserBottom: TSplitter
          Left = 0
          Top = 161
          Width = 1006
          Height = 3
          Cursor = crVSplit
          Align = alBottom
          AutoSnap = False
          Visible = False
          OnMoved = Sp_BrowserBottomMoved
          ExplicitTop = 162
          ExplicitWidth = 849
        end
        object Pa_BrowserBottom: TPanel
          Left = 0
          Top = 164
          Width = 1006
          Height = 17
          Align = alBottom
          BevelOuter = bvNone
          TabOrder = 0
          object Sc_X: TScrollBar
            Left = 0
            Top = 0
            Width = 989
            Height = 17
            Align = alClient
            Max = 15000
            Min = -5000
            PageSize = 0
            Position = 5000
            TabOrder = 0
            TabStop = False
            OnChange = Sc_YChange
          end
          object Pa_BrowserBottomRight: TPanel
            Left = 989
            Top = 0
            Width = 17
            Height = 17
            Align = alRight
            BevelOuter = bvNone
            TabOrder = 1
          end
        end
        object Sc_Y: TScrollBar
          Left = 986
          Top = 41
          Width = 17
          Height = 120
          Align = alRight
          Kind = sbVertical
          Max = 15000
          Min = -5000
          PageSize = 0
          Position = 5000
          TabOrder = 1
          TabStop = False
          OnChange = Sc_YChange
        end
        object Bu_Enter: TButton
          Left = 416
          Top = 120
          Width = 75
          Height = 25
          Caption = 'Enter'
          Default = True
          TabOrder = 2
          OnClick = Bu_EnterClick
        end
        object Bu_BrowserTest: TButton
          Left = 552
          Top = 80
          Width = 75
          Height = 25
          Caption = 'Test'
          TabOrder = 3
          Visible = False
          OnClick = Bu_BrowserTestClick
        end
        object Pa_BrowserTop: TPanel
          Left = 0
          Top = 0
          Width = 1006
          Height = 41
          Align = alTop
          TabOrder = 4
          object La_Zoom: TLabel
            Left = 352
            Top = 16
            Width = 30
            Height = 12
            Caption = 'Zoom:'
          end
          object SB_Cursor: TSpeedButton
            Left = 8
            Top = 8
            Width = 25
            Height = 25
            GroupIndex = 1
            Down = True
            Glyph.Data = {
              72010000424D7201000000000000760000002800000015000000150000000100
              040000000000FC00000000000000000000001000000000000000000000000000
              8000008000000080800080000000800080008080000080808000C0C0C0000000
              FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00888888888888
              8008888880008888888888880FF0888880008888888888880FF0888880008888
              88888880FF0888888000888880888880FF088888800088888008880FF0888888
              8000888880F0880FF08888888000888880FF00FF088888888000888880FFF0FF
              088888888000888880FFFFFF000008888000888880FFFFFFFFF0888880008888
              80FFFFFFFF0888888000888880FFFFFFF08888888000888880FFFFFF08888888
              8000888880FFFFF0888888888000888880FFFF08888888888000888880FFF088
              888888888000888880FF0888888888888000888880F088888888888880008888
              80088888888888888000888880888888888888888000}
          end
          object SB_Line: TSpeedButton
            Left = 32
            Top = 8
            Width = 25
            Height = 25
            GroupIndex = 1
            Glyph.Data = {
              72010000424D7201000000000000760000002800000015000000150000000100
              040000000000FC00000000000000000000001000000000000000000000000000
              8000008000000080800080000000800080008080000080808000C0C0C0000000
              FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00888888888888
              8888888880008888888888888888888880008888888888888888888880008888
              8888888888888088800088888888888888880888800088888888888888808888
              8000888888888888880888888000888888888888808888888000888888888888
              0888888880008888888888808888888880008888888888088888888880008888
              8888808888888888800088888888088888888888800088888880888888888888
              8000888888088888888888888000888880888888888888888000888808888888
              8888888880008880888888888888888880008888888888888888888880008888
              88888888888888888000888888888888888888888000}
          end
          object SB_View: TSpeedButton
            Left = 111
            Top = 8
            Width = 25
            Height = 25
            AllowAllUp = True
            Glyph.Data = {
              72010000424D7201000000000000760000002800000015000000150000000100
              040000000000FC00000000000000000000001000000010000000000000000000
              80000080000000808000800000008000800080800000C0C0C000808080000000
              FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00333333333333
              3333333330003333333333333333333330003333333333333333333330003333
              3333333333333333300033333333333333333333300033333333333333333333
              3000333333333333333333333000333333333333333333333000333333333333
              3333333330003333333333333333333330003333333333333333333330003333
              3333333333333333300033333333333333333333300033333333333333333333
              3000333033303000033030333000330303303033330303033000303330303000
              3303030330003033303030333303030330003033303030000303330330003333
              33333333333333333000333333333333333333333000}
            OnClick = SB_ViewClick
          end
          object SB_ToolLabel: TSpeedButton
            Left = 56
            Top = 8
            Width = 25
            Height = 25
            GroupIndex = 1
            Glyph.Data = {
              72010000424D7201000000000000760000002800000015000000150000000100
              040000000000FC00000000000000000000001000000000000000000000000000
              8000008000000080800080000000800080008080000080808000C0C0C0000000
              FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00888888888888
              8888888880008888888888888888888880008888888888888888888880008888
              8888777777777888800088888887FFFFFFFFF788800088888887FFFFFFFFF788
              800088888887FFFFFFFFF788800088888887FFFFFFFFF7888000888888887777
              7777788880008888888888888888888880008888888888888888888880008880
              0000088888888888800088800888888888888888800088800888888888888888
              8000888008888888888888888000888008888888888888888000888008888888
              8888888880008880088888888888888880008888888888888888888880008888
              88888888888888888000888888888888888888888000}
            OnClick = SB_ToolLabelClick
          end
          object SB_ToolLinkLabel: TSpeedButton
            Left = 80
            Top = 8
            Width = 25
            Height = 25
            GroupIndex = 1
            Glyph.Data = {
              72010000424D7201000000000000760000002800000015000000150000000100
              040000000000FC00000000000000000000001000000000000000000000000000
              8000008000000080800080000000800080008080000080808000C0C0C0000000
              FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00888888888888
              8888888880008888888888888888888880008888888888888888888880008888
              8888888888888088800088888888888888880888800088888888888888808888
              8000888888888888880888888000888888888888808888888000888888888888
              0888888880008888888888808888888880008888888888888888888880008880
              0000088888888888800088800888888888888888800088800888888888888888
              8000888008888888888888888000888008888888888888888000888008888888
              8888888880008880088888888888888880008888888888888888888880008888
              88888888888888888000888888888888888888888000}
            OnClick = SB_ToolLinkLabelClick
          end
          object SB_Arrange: TSpeedButton
            Left = 193
            Top = 10
            Width = 25
            Height = 21
            AllowAllUp = True
            GroupIndex = 2
            Caption = 'ON'
            OnClick = SB_ArrangeClick
          end
          object SB_AutoScroll: TSpeedButton
            Left = 578
            Top = 10
            Width = 90
            Height = 21
            AllowAllUp = True
            GroupIndex = 3
            Down = True
            Caption = 'Auto Scroll'
            OnClick = SB_AutoScrollClick
          end
          object SB_AutoZoom: TSpeedButton
            Left = 674
            Top = 10
            Width = 90
            Height = 21
            AllowAllUp = True
            GroupIndex = 4
            Caption = 'Auto Zoom'
          end
          object SB_ArrangeRefresh: TSpeedButton
            Left = 312
            Top = 10
            Width = 25
            Height = 21
            AllowAllUp = True
            Enabled = False
            Glyph.Data = {
              86000000424D86000000000000003E0000002800000013000000120000000100
              010000000000480000000000000000000000020000000000000000000000FFFF
              FF00FFFFE000FFFFE000FFFFE000FFDFE000FFEFE000FFF7E000FFF7E000FBF7
              E000F180E000E0C1E000C063E000FBF7E000FBFFE000FBFFE000FDFFE000FEFF
              E000FFFFE000FFFFE000}
            OnClick = SB_ArrangeRefreshClick
          end
          object La_Arrange: TLabel
            Left = 142
            Top = 16
            Width = 42
            Height = 12
            Caption = 'Arrange:'
          end
          object Bu_ArrangeType: TSpeedButton
            Left = 221
            Top = 10
            Width = 88
            Height = 21
            OnClick = Bu_ArrangeTypeClick
          end
          object TB_Zoom: TTrackBar
            Left = 391
            Top = 10
            Width = 105
            Height = 23
            LineSize = 1000
            Max = 10000
            Min = -4000
            Position = -2000
            TabOrder = 0
            OnChange = TB_ZoomChange
            OnKeyDown = TB_ZoomKeyDown
          end
          object Co_Arrange: TComboBox
            Left = 248
            Top = 18
            Width = 73
            Height = 20
            Style = csDropDownList
            ItemHeight = 0
            TabOrder = 1
            Visible = False
            Items.Strings = (
              'Normalize'
              'Repulsion'
              'Link'
              'Label'
              'Index')
          end
          object Bu_Shuffle: TButton
            Left = 499
            Top = 8
            Width = 71
            Height = 25
            Caption = 'Shuffle'
            TabOrder = 2
            TabStop = False
            OnClick = Bu_ShuffleClick
          end
          object Ed_FindCard: TEdit
            Left = 770
            Top = 10
            Width = 90
            Height = 20
            TabOrder = 3
            OnChange = Ed_FindCardChange
            OnEnter = Ed_FindCardEnter
            OnExit = Ed_FindCardExit
          end
          object Bu_FindCard: TButton
            Left = 866
            Top = 8
            Width = 49
            Height = 25
            Caption = 'Find'
            TabOrder = 4
            OnClick = Bu_FindCardClick
          end
        end
        object Pa_Browser: TPanel
          Left = -2
          Top = 39
          Width = 116
          Height = 109
          BevelOuter = bvNone
          TabOrder = 5
          object PB_Browser: TPaintBox
            Left = 1
            Top = 8
            Width = 101
            Height = 101
            PopupMenu = PM_BNoSelect
            OnDblClick = PB_BrowserDblClick
            OnMouseDown = PB_BrowserMouseDown
            OnMouseMove = PB_BrowserMouseMove
            OnMouseUp = PB_BrowserMouseUp
            OnPaint = PB_BrowserPaint
          end
        end
      end
      object TS_Editor: TTabSheet
        Caption = 'Editor'
        object Sp_EditorRelated: TSplitter
          Left = 818
          Top = 41
          Height = 140
          Align = alRight
          Visible = False
          ExplicitLeft = 661
          ExplicitHeight = 141
        end
        object Pa_Editor: TPanel
          Left = 0
          Top = 8
          Width = 641
          Height = 169
          BevelOuter = bvNone
          TabOrder = 0
          object Splitter2: TSplitter
            Left = 0
            Top = 61
            Width = 641
            Height = 3
            Cursor = crVSplit
            Align = alBottom
          end
          object LB_Link: TListBox
            Left = 0
            Top = 64
            Width = 641
            Height = 105
            Align = alBottom
            ItemHeight = 12
            PopupMenu = PM_Link
            TabOrder = 1
            OnClick = LB_LinkClick
            OnDblClick = LB_LinkDblClick
          end
          object RE_Edit: TMemo
            Left = 0
            Top = 32
            Width = 185
            Height = 89
            Font.Charset = SHIFTJIS_CHARSET
            Font.Color = clWindowText
            Font.Height = -12
            Font.Name = #65325#65331' '#12468#12471#12483#12463
            Font.Pitch = fpFixed
            Font.Style = []
            ParentFont = False
            PopupMenu = PM_Editor
            ScrollBars = ssBoth
            TabOrder = 0
            WantTabs = True
            WordWrap = False
            OnChange = RE_EditChange
            OnEnter = RE_EditEnter
            OnExit = RE_EditExit
            OnKeyDown = RE_EditKeyDown
            OnKeyUp = RE_EditKeyUp
            OnMouseDown = RE_EditMouseDown
            OnMouseUp = RE_EditMouseUp
          end
          object Bu_Test: TButton
            Left = 552
            Top = 40
            Width = 75
            Height = 25
            Caption = 'Test'
            TabOrder = 2
            Visible = False
            OnClick = Bu_TestClick
          end
        end
        object Pa_EditorRelated: TPanel
          Left = 821
          Top = 41
          Width = 185
          Height = 140
          Align = alRight
          BevelOuter = bvNone
          TabOrder = 2
          Visible = False
          object Me_EditorRelated: TMemo
            Left = 0
            Top = 24
            Width = 185
            Height = 116
            Align = alClient
            ReadOnly = True
            ScrollBars = ssVertical
            TabOrder = 0
          end
          object Pa_EditorRelatedTop: TPanel
            Left = 0
            Top = 0
            Width = 185
            Height = 24
            Align = alTop
            Caption = 'Related Text'
            TabOrder = 1
            Visible = False
          end
        end
        object Pa_EditorTop: TPanel
          Left = 0
          Top = 0
          Width = 1006
          Height = 41
          Align = alTop
          TabOrder = 1
          object SB_EditorRelated: TSpeedButton
            Left = 6
            Top = 10
            Width = 81
            Height = 21
            AllowAllUp = True
            GroupIndex = 3
            Caption = 'Related Text'
            OnClick = SB_EditorRelatedClick
          end
        end
      end
      object TS_Drawing: TTabSheet
        Caption = 'Drawing'
        ImageIndex = 3
        ExplicitLeft = 0
        ExplicitTop = 0
        ExplicitWidth = 0
        ExplicitHeight = 0
        object PB_Drawing: TPaintBox
          Left = 3
          Top = 49
          Width = 81
          Height = 80
          PopupMenu = PM_Drawing
          OnMouseDown = PB_DrawingMouseDown
          OnMouseMove = PB_DrawingMouseMove
          OnMouseUp = PB_DrawingMouseUp
          OnPaint = PB_DrawingPaint
        end
        object Pa_DrawingTop: TPanel
          Left = 0
          Top = 0
          Width = 1006
          Height = 41
          Align = alTop
          TabOrder = 0
          object La_DZoom: TLabel
            Left = 326
            Top = 16
            Width = 30
            Height = 12
            Caption = 'Zoom:'
          end
          object SB_DCursor: TSpeedButton
            Left = 8
            Top = 8
            Width = 25
            Height = 25
            GroupIndex = 1
            Down = True
            Glyph.Data = {
              72010000424D7201000000000000760000002800000015000000150000000100
              040000000000FC00000000000000000000001000000000000000000000000000
              8000008000000080800080000000800080008080000080808000C0C0C0000000
              FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00888888888888
              8008888880008888888888880FF0888880008888888888880FF0888880008888
              88888880FF0888888000888880888880FF088888800088888008880FF0888888
              8000888880F0880FF08888888000888880FF00FF088888888000888880FFF0FF
              088888888000888880FFFFFF000008888000888880FFFFFFFFF0888880008888
              80FFFFFFFF0888888000888880FFFFFFF08888888000888880FFFFFF08888888
              8000888880FFFFF0888888888000888880FFFF08888888888000888880FFF088
              888888888000888880FF0888888888888000888880F088888888888880008888
              80088888888888888000888880888888888888888000}
            OnClick = SB_DrawingToolClick
          end
          object SB_FreeHand: TSpeedButton
            Tag = 1
            Left = 32
            Top = 8
            Width = 25
            Height = 25
            GroupIndex = 1
            Glyph.Data = {
              72010000424D7201000000000000760000002800000015000000150000000100
              040000000000FC00000000000000000000001000000000000000000000000000
              8000008000000080800080000000800080008080000080808000C0C0C0000000
              FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00888888888888
              8888888880008888888888888888888880008888888888888888888880008888
              8888888888888088800088888888888888888088800088888888888888888088
              8000888888888888888808888000888888888888888088888000888888888888
              8808888880008888888888880088888880008888888880008888888880008888
              8880088888888888800088888808888888888888800088888088888888888888
              8000888808888888888888888000888088888888888888888000888088888888
              8888888880008880888888888888888880008888888888888888888880008888
              88888888888888888000888888888888888888888000}
            OnClick = SB_DrawingToolClick
          end
          object SB_DLine: TSpeedButton
            Tag = 2
            Left = 56
            Top = 8
            Width = 25
            Height = 25
            GroupIndex = 1
            Glyph.Data = {
              72010000424D7201000000000000760000002800000015000000150000000100
              040000000000FC00000000000000000000001000000000000000000000000000
              8000008000000080800080000000800080008080000080808000C0C0C0000000
              FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00888888888888
              8888888880008888888888888888888880008888888888888888888880008888
              8888888888888088800088888888888888880888800088888888888888808888
              8000888888888888880888888000888888888888808888888000888888888888
              0888888880008888888888808888888880008888888888088888888880008888
              8888808888888888800088888888088888888888800088888880888888888888
              8000888888088888888888888000888880888888888888888000888808888888
              8888888880008880888888888888888880008888888888888888888880008888
              88888888888888888000888888888888888888888000}
            OnClick = SB_DrawingToolClick
          end
          object SB_Box: TSpeedButton
            Tag = 3
            Left = 80
            Top = 8
            Width = 25
            Height = 25
            GroupIndex = 1
            Glyph.Data = {
              72010000424D7201000000000000760000002800000015000000150000000100
              040000000000FC00000000000000000000001000000000000000000000000000
              8000008000000080800080000000800080008080000080808000C0C0C0000000
              FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00888888888888
              8888888880008888888888888888888880008888888888888888888880008888
              8888888888888888800088880000000000000888800088880888888888880888
              8000888808888888888808888000888808888888888808888000888808888888
              8888088880008888088888888888088880008888088888888888088880008888
              0888888888880888800088880888888888880888800088880888888888880888
              8000888808888888888808888000888808888888888808888000888800000000
              0000088880008888888888888888888880008888888888888888888880008888
              88888888888888888000888888888888888888888000}
            OnClick = SB_DrawingToolClick
          end
          object SB_Circle: TSpeedButton
            Tag = 4
            Left = 104
            Top = 8
            Width = 25
            Height = 25
            GroupIndex = 1
            Glyph.Data = {
              72010000424D7201000000000000760000002800000015000000150000000100
              040000000000FC00000000000000000000001000000000000000000000000000
              8000008000000080800080000000800080008080000080808000C0C0C0000000
              FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00888888888888
              8888888880008888888888888888888880008888888888888888888880008888
              8888000008888888800088888800888880088888800088888088888888808888
              8000888808888888888808888000888808888888888808888000888088888888
              8888808880008880888888888888808880008880888888888888808880008880
              8888888888888088800088808888888888888088800088880888888888880888
              8000888808888888888808888000888880888888888088888000888888008888
              8008888880008888888800000888888880008888888888888888888880008888
              88888888888888888000888888888888888888888000}
            OnClick = SB_DrawingToolClick
          end
          object SB_Text: TSpeedButton
            Tag = 5
            Left = 128
            Top = 8
            Width = 25
            Height = 25
            GroupIndex = 1
            Glyph.Data = {
              72010000424D7201000000000000760000002800000015000000150000000100
              040000000000FC00000000000000000000001000000000000000000000000000
              8000008000000080800080000000800080008080000080808000C0C0C0000000
              FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00888888888888
              8888888880008888888888888888888880008888888888888888888880008888
              8888888888888888800088880000888880000888800088888008888888008888
              8000888888008888880088888000888888008888880088888000888888800888
              8800888880008888888000000000888880008888888800888000888880008888
              8888008880088888800088888888800880088888800088888888800880088888
              8000888888888800800888888000888888888800000888888000888888888880
              0008888880008888888888888888888880008888888888888888888880008888
              88888888888888888000888888888888888888888000}
            Visible = False
            OnClick = SB_DrawingToolClick
          end
          object La_PenColor: TLabel
            Left = 165
            Top = 16
            Width = 29
            Height = 12
            Caption = 'Color:'
          end
          object Sh_DColorDefault: TShape
            Left = 200
            Top = 8
            Width = 49
            Height = 25
            Pen.Width = 3
            OnMouseDown = Sh_DColorDefaultMouseDown
          end
          object La_PenColorDefault: TLabel
            Left = 200
            Top = 15
            Width = 49
            Height = 12
            Alignment = taCenter
            AutoSize = False
            Caption = 'Default'
            OnMouseDown = Sh_DColorDefaultMouseDown
          end
          object Sh_DColor: TShape
            Left = 255
            Top = 8
            Width = 25
            Height = 25
            Brush.Color = clGray
            OnMouseDown = Sh_DColorMouseDown
          end
          object TB_DZoom: TTrackBar
            Left = 364
            Top = 10
            Width = 105
            Height = 23
            LineSize = 1000
            Max = 15000
            Min = 1000
            Position = 10000
            TabOrder = 0
            OnChange = PB_DrawingPaint
          end
          object Bu_DColor: TBitBtn
            Left = 280
            Top = 8
            Width = 15
            Height = 25
            TabOrder = 1
            OnClick = Bu_DColorClick
            Glyph.Data = {
              8A000000424D8A00000000000000760000002800000005000000050000000100
              0400000000001400000000000000000000001000000000000000000000000000
              8000008000000080800080000000800080008080000080808000C0C0C0000000
              FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00888880008808
              8000800080000000000088888000}
          end
        end
      end
      object TS_Statistics: TTabSheet
        Caption = 'Statistics'
        ImageIndex = 2
        ExplicitLeft = 0
        ExplicitTop = 0
        ExplicitWidth = 0
        ExplicitHeight = 0
        object PB_Statistics: TPaintBox
          Left = 0
          Top = 41
          Width = 81
          Height = 80
          OnMouseDown = PB_StatisticsMouseDown
          OnPaint = PB_StatisticsPaint
        end
        object Sc_StatisticsY: TScrollBar
          Left = 989
          Top = 41
          Width = 17
          Height = 140
          Align = alRight
          Kind = sbVertical
          Max = 10000
          PageSize = 0
          TabOrder = 1
          TabStop = False
          OnChange = Sc_StatisticsYChange
        end
        object Pa_StatisticsTop: TPanel
          Left = 0
          Top = 0
          Width = 1006
          Height = 41
          Align = alTop
          TabOrder = 0
          object SB_StatisticsSort: TSpeedButton
            Left = 191
            Top = 10
            Width = 51
            Height = 21
            AllowAllUp = True
            GroupIndex = 3
            Caption = 'Sort'
            OnClick = SB_StatisticsSortClick
          end
          object Bu_StatisticsKey: TSpeedButton
            Left = 40
            Top = 10
            Width = 145
            Height = 21
            Caption = 'Label'
            OnClick = Bu_StatisticsKeyClick
          end
          object La_StatisticsKey: TLabel
            Left = 8
            Top = 16
            Width = 21
            Height = 12
            Caption = 'Key:'
          end
        end
      end
    end
    object SB_Status: TStatusBar
      Left = 0
      Top = 246
      Width = 1041
      Height = 19
      Panels = <
        item
          Width = 450
        end
        item
          Width = 1000
        end>
    end
    object Pa_Card: TPanel
      Left = 0
      Top = 0
      Width = 1041
      Height = 33
      Align = alTop
      TabOrder = 1
      object La_Title: TLabel
        Left = 48
        Top = 12
        Width = 25
        Height = 12
        Caption = 'Title:'
      end
      object La_Label: TLabel
        Left = 432
        Top = 12
        Width = 29
        Height = 12
        Caption = 'Label:'
      end
      object La_Card: TLabel
        Left = 8
        Top = 12
        Width = 26
        Height = 12
        Caption = 'Card:'
      end
      object Label5: TLabel
        Left = 48
        Top = 36
        Width = 42
        Height = 12
        Caption = 'Created:'
        Visible = False
      end
      object Label6: TLabel
        Left = 248
        Top = 36
        Width = 34
        Height = 12
        Caption = 'Edited:'
        Visible = False
      end
      object Label7: TLabel
        Left = 456
        Top = 36
        Width = 39
        Height = 12
        Caption = 'Viewed:'
        Visible = False
      end
      object La_Created: TLabel
        Left = 96
        Top = 36
        Width = 6
        Height = 12
        Caption = '-'
        Visible = False
      end
      object La_Edited: TLabel
        Left = 296
        Top = 36
        Width = 6
        Height = 12
        Caption = '-'
        Visible = False
      end
      object La_Viewed: TLabel
        Left = 504
        Top = 36
        Width = 6
        Height = 12
        Caption = '-'
        Visible = False
      end
      object SB_Shape: TSpeedButton
        Left = 327
        Top = 6
        Width = 41
        Height = 21
        OnClick = SB_ShapeClick
      end
      object La_Size: TLabel
        Left = 242
        Top = 12
        Width = 23
        Height = 12
        Caption = 'Size:'
      end
      object SB_Fix: TSpeedButton
        Left = 376
        Top = 6
        Width = 21
        Height = 21
        AllowAllUp = True
        GroupIndex = 1
        Glyph.Data = {
          42010000424D4201000000000000760000002800000011000000110000000100
          040000000000CC00000000000000000000001000000000000000000000000000
          8000008000000080800080000000800080008080000080808000C0C0C0000000
          FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00888888888888
          8888800000008088888888008888800000008808888800FF0888800000008880
          8880FFFF0888800000008888080FFFF0888880000000888880FFFFF088888000
          000088880FFFFF088888800000008880FFF0F000088880000000880FFFFF0099
          900880000000880FFFF0099999088000000080FFFF00999999908000000080FF
          008099999F90800000008800888099999F9080000000888888880999F9088000
          0000888888880099900880000000888888888800088880000000888888888888
          888880000000}
        OnClick = SB_FixClick
      end
      object SB_Top: TSpeedButton
        Left = 404
        Top = 6
        Width = 21
        Height = 21
        AllowAllUp = True
        GroupIndex = 2
        Glyph.Data = {
          42010000424D4201000000000000760000002800000011000000110000000100
          040000000000CC00000000000000000000001000000000000000000000000000
          8000008000000080800080000000800080008080000080808000C0C0C0000000
          FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00888888888888
          8888800000008888888888888888800000008888800000000008800000008888
          8088888888888000000088888088888888888000000088888088888888888000
          0000888880000000000880000000888880888888888880000000888880888888
          8888800000008888808888888888800000008880888088888888800000008880
          000088888888800000008880BBB0888888888000000088000B00088888888000
          000088880B088888888880000000888880888888888880000000888888888888
          888880000000}
        OnClick = SB_TopClick
      end
      object Ed_Title: TEdit
        Left = 80
        Top = 6
        Width = 153
        Height = 20
        TabOrder = 0
        OnChange = Ed_TitleChange
      end
      object Bu_Label0: TButton
        Left = 472
        Top = 6
        Width = 81
        Height = 21
        TabOrder = 1
        Visible = False
        OnClick = Bu_Label0Click
      end
      object Ed_CardSize: TEdit
        Left = 281
        Top = 6
        Width = 21
        Height = 20
        TabOrder = 3
        Text = '0'
        OnChange = Ed_CardSizeChange
      end
      object UD_CardSize: TUpDown
        Left = 302
        Top = 6
        Width = 16
        Height = 20
        Associate = Ed_CardSize
        Min = -8
        Max = 8
        TabOrder = 4
        Thousands = False
      end
      object Pa_Link: TPanel
        Left = 516
        Top = 3
        Width = 927
        Height = 31
        BevelOuter = bvNone
        TabOrder = 2
        Visible = False
        object La_LinkTitle: TLabel
          Left = 47
          Top = 11
          Width = 25
          Height = 12
          Caption = 'Title:'
        end
        object La_LinkLabel: TLabel
          Left = 431
          Top = 11
          Width = 29
          Height = 12
          Caption = 'Label:'
        end
        object La_Link: TLabel
          Left = 7
          Top = 11
          Width = 23
          Height = 12
          Caption = 'Link:'
        end
        object Label11: TLabel
          Left = 48
          Top = 36
          Width = 42
          Height = 12
          Caption = 'Created:'
          Visible = False
        end
        object Label12: TLabel
          Left = 248
          Top = 36
          Width = 34
          Height = 12
          Caption = 'Edited:'
          Visible = False
        end
        object Label13: TLabel
          Left = 456
          Top = 36
          Width = 39
          Height = 12
          Caption = 'Viewed:'
          Visible = False
        end
        object Label14: TLabel
          Left = 96
          Top = 36
          Width = 6
          Height = 12
          Caption = '-'
          Visible = False
        end
        object Label15: TLabel
          Left = 296
          Top = 36
          Width = 6
          Height = 12
          Caption = '-'
          Visible = False
        end
        object Label16: TLabel
          Left = 504
          Top = 36
          Width = 6
          Height = 12
          Caption = '-'
          Visible = False
        end
        object SB_LinkShape: TSpeedButton
          Left = 383
          Top = 5
          Width = 41
          Height = 21
          OnClick = SB_LinkShapeClick
        end
        object Ed_LinkTitle: TEdit
          Left = 79
          Top = 5
          Width = 153
          Height = 20
          TabOrder = 0
          OnChange = Ed_LinkTitleChange
        end
        object Bu_LinkLabel0: TButton
          Left = 471
          Top = 5
          Width = 81
          Height = 21
          TabOrder = 1
          Visible = False
          OnClick = Bu_LinkLabel0Click
        end
        object Ch_LinkDirection: TCheckBox
          Left = 240
          Top = 8
          Width = 65
          Height = 17
          Caption = 'Direction'
          TabOrder = 2
          OnClick = Ch_LinkDirectionClick
        end
        object Bu_LinkDirectionInverse: TButton
          Left = 315
          Top = 5
          Width = 51
          Height = 21
          Caption = 'Inverse'
          TabOrder = 3
          OnClick = Bu_LinkDirectionInverseClick
        end
      end
    end
  end
  object Pa_Files: TPanel
    Left = 0
    Top = 33
    Width = 25
    Height = 415
    Align = alLeft
    BevelOuter = bvNone
    Constraints.MinWidth = 16
    TabOrder = 3
    Visible = False
    object LB_FileList: TListBox
      Left = 0
      Top = 33
      Width = 89
      Height = 80
      ItemHeight = 12
      TabOrder = 0
      OnClick = LB_FileListClick
      OnDblClick = LB_FileListDblClick
      OnMouseDown = LB_FileListMouseDown
      OnMouseUp = LB_FileListMouseUp
    end
    object Pa_FilesBottom: TPanel
      Left = 0
      Top = 395
      Width = 25
      Height = 20
      Align = alBottom
      TabOrder = 2
    end
    object Pa_FilesTop: TPanel
      Left = 0
      Top = 0
      Width = 25
      Height = 33
      Align = alTop
      TabOrder = 1
      OnResize = Pa_ListTopResize
      object La_Files: TLabel
        Left = 8
        Top = 12
        Width = 27
        Height = 12
        Caption = 'Files:'
      end
    end
  end
  object Pa_GlobalSearch: TPanel
    Left = 1163
    Top = 33
    Width = 207
    Height = 415
    Align = alRight
    BevelOuter = bvNone
    Constraints.MinWidth = 16
    TabOrder = 4
    Visible = False
    object PB_GlobalSearch: TPaintBox
      Left = 8
      Top = 40
      Width = 105
      Height = 105
      OnMouseDown = PB_GlobalSearchMouseDown
      OnPaint = PB_GlobalSearchPaint
    end
    object Pa_GlobalSearchTop: TPanel
      Left = 0
      Top = 0
      Width = 207
      Height = 33
      Align = alTop
      TabOrder = 0
      OnResize = Pa_ListTopResize
      object La_GlobalSearchTop: TLabel
        Left = 40
        Top = 12
        Width = 80
        Height = 12
        Caption = 'Search Results:'
      end
      object SB_CloseGlobalSearch: TSpeedButton
        Left = 6
        Top = 6
        Width = 21
        Height = 21
        AllowAllUp = True
        Glyph.Data = {
          42010000424D4201000000000000760000002800000011000000110000000100
          040000000000CC00000000000000000000001000000000000000000000000000
          8000008000000080800080000000800080008080000080808000C0C0C0000000
          FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00888888888888
          8888800000008888888888888888800000008888888888888888800000008888
          8888888888888000000088887088888078888000000088880008880008888000
          0000888880008000888880000000888888000008888880000000888888800088
          8888800000008888880000088888800000008888800080008888800000008888
          0008880008888000000088887088888078888000000088888888888888888000
          0000888888888888888880000000888888888888888880000000888888888888
          888880000000}
        OnClick = SB_CloseGlobalSearchClick
      end
      object La_SearchResultKeyword: TLabel
        Left = 128
        Top = 12
        Width = 12
        Height = 12
        Caption = '""'
      end
    end
    object Pa_SearchResultBottom: TPanel
      Left = 0
      Top = 395
      Width = 207
      Height = 20
      Align = alBottom
      TabOrder = 1
      object La_GlobalSearchCount: TLabel
        Left = 8
        Top = 4
        Width = 31
        Height = 12
        Caption = '0 Hits'
      end
    end
    object Sc_GlobalSearch: TScrollBar
      Left = 190
      Top = 33
      Width = 17
      Height = 362
      Align = alRight
      Kind = sbVertical
      PageSize = 0
      TabOrder = 2
      OnChange = Sc_GlobalSearchChange
      OnKeyDown = Sc_GlobalSearchKeyDown
    end
  end
  object Pa_Top_: TPanel
    Left = 0
    Top = 0
    Width = 1370
    Height = 33
    Align = alTop
    TabOrder = 2
    Visible = False
    object Label2: TLabel
      Left = 8
      Top = 11
      Width = 23
      Height = 12
      Caption = 'Size:'
    end
    object Ed_Size: TEdit
      Left = 40
      Top = 5
      Width = 41
      Height = 20
      TabOrder = 0
      Text = '9'
      OnChange = Ed_SizeChange
    end
    object UD_Size: TUpDown
      Left = 81
      Top = 5
      Width = 16
      Height = 20
      Associate = Ed_Size
      Min = 5
      Position = 9
      TabOrder = 1
    end
  end
  object Ti_Check: TTimer
    Enabled = False
    Interval = 30
    OnTimer = Ti_CheckTimer
    Left = 16
    Top = 224
  end
  object PM_List: TPopupMenu
    Left = 16
    Top = 80
    object PL_NewCard: TMenuItem
      Caption = '&New Card'
      OnClick = MI_NewCardClick
    end
    object PL_DeleteCard: TMenuItem
      Caption = '&Delete Card'
      OnClick = PL_DeleteCardClick
    end
    object N10: TMenuItem
      Caption = '-'
    end
    object PL_CardProperty: TMenuItem
      Caption = '&Property...'
      OnClick = PL_CardPropertyClick
    end
  end
  object MM_Menu: TMainMenu
    AutoHotkeys = maManual
    Left = 224
    Top = 192
    object M_File: TMenuItem
      Caption = '&File'
      object MF_New: TMenuItem
        Caption = '&New'
        OnClick = MF_NewClick
      end
      object MF_Open: TMenuItem
        Caption = '&Open...'
        ShortCut = 16463
        OnClick = MF_OpenClick
      end
      object MF_Save: TMenuItem
        Caption = '&Save'
        ShortCut = 16467
        OnClick = MF_SaveClick
      end
      object MF_SaveAs: TMenuItem
        Caption = 'Save &As...'
        OnClick = MF_SaveAsClick
      end
      object N1: TMenuItem
        Caption = '-'
      end
      object MF_RecentFiles: TMenuItem
        Caption = '&Recent Files'
      end
      object MF_RecentFolders: TMenuItem
        Caption = 'Re&cent Folders'
      end
      object N7: TMenuItem
        Caption = '-'
      end
      object MF_Import: TMenuItem
        Caption = '&Import'
        object MFI_TxtFile: TMenuItem
          Caption = '&Txt File(s)...'
          OnClick = MFI_TxtFileClick
        end
        object MFI_HierarchicalTextFile: TMenuItem
          Caption = '&Hierarchical Text File...'
          OnClick = MFI_HierarchicalTextFileClick
        end
        object MFI_HierarchicalTextFile2: TMenuItem
          Caption = 'Hierarchical Text File &2...'
          OnClick = MFI_HierarchicalTextFile2Click
        end
        object MFI_TxtFilesinaFolder: TMenuItem
          Caption = 'Txt Files in a &Folder...'
          OnClick = MFI_TxtFilesinaFolderClick
        end
      end
      object MF_Export: TMenuItem
        Caption = '&Export'
        object MFE_TxtFileCardTitle: TMenuItem
          Caption = 'Txt File(&Card Title)...'
          OnClick = MFE_TxtFileCardTitleClick
        end
        object MFE_TxtFile: TMenuItem
          Caption = '&Txt File(Text)...'
          OnClick = MFE_TxtFileClick
        end
        object MFE_TxtFiles: TMenuItem
          Caption = 'T&xt Files...'
          OnClick = MFE_TxtFilesClick
        end
        object MFE_HierarchicalTextFile: TMenuItem
          Caption = '&Hierarchical Text File...'
          OnClick = MFE_HierarchicalTextFileClick
        end
        object N30: TMenuItem
          Caption = '-'
        end
        object MFE_HtmlFiles: TMenuItem
          Caption = '&Html Files...'
          OnClick = MFE_HtmlFilesClick
        end
        object N32: TMenuItem
          Caption = '-'
        end
        object MFE_BMPFile: TMenuItem
          Caption = '&BMP file...'
          OnClick = MFE_BMPFileClick
        end
        object MFE_JPEGFile: TMenuItem
          Caption = '&JPEG file...'
          OnClick = MFE_JPEGFileClick
        end
        object N31: TMenuItem
          Caption = '-'
        end
        object MFE_ClipboardCardTitle: TMenuItem
          Caption = 'C&lipboard(Card Title)'
          OnClick = MFE_ClipboardCardTitleClick
        end
        object MFE_ClipboardCardText: TMenuItem
          Caption = 'Cl&ipboard(Text)'
          OnClick = MFE_ClipboardCardTextClick
        end
        object MFE_ClipboardBMP: TMenuItem
          Caption = 'Cli&pboard(BMP)'
          OnClick = MFE_ClipboardBMPClick
        end
      end
      object N15: TMenuItem
        Caption = '-'
      end
      object MF_PrintImage: TMenuItem
        Caption = '&Print Image...'
        OnClick = MF_PrintImageClick
      end
      object MF_PrintText: TMenuItem
        Caption = 'Print &Text...'
        Visible = False
        OnClick = MF_PrintTextClick
      end
      object N13: TMenuItem
        Caption = '-'
      end
      object MF_Exit: TMenuItem
        Caption = 'E&xit'
        ShortCut = 32883
        OnClick = MF_ExitClick
      end
    end
    object M_Edit: TMenuItem
      Caption = '&Edit'
      object ME_Undo: TMenuItem
        Caption = '&Undo'
        ShortCut = 16474
        OnClick = ME_UndoClick
      end
      object ME_Redo: TMenuItem
        Caption = '&Redo'
        ShortCut = 24666
        OnClick = ME_RedoClick
      end
      object N2: TMenuItem
        Caption = '-'
      end
      object ME_Cut: TMenuItem
        Caption = 'Cu&t'
        ShortCut = 16472
        OnClick = ME_CutClick
      end
      object ME_Copy: TMenuItem
        Caption = '&Copy'
        ShortCut = 16451
        OnClick = ME_CopyClick
      end
      object ME_Paste: TMenuItem
        Caption = '&Paste'
        ShortCut = 16470
        OnClick = ME_PasteClick
      end
      object ME_Delete: TMenuItem
        Caption = '&Delete'
        ShortCut = 16430
        OnClick = ME_DeleteClick
      end
      object ME_SelectAll: TMenuItem
        Caption = 'Select &All'
        ShortCut = 16449
        OnClick = ME_SelectAllClick
      end
      object N3: TMenuItem
        Caption = '-'
      end
      object ME_Find: TMenuItem
        Caption = '&Find...'
        ShortCut = 16454
        OnClick = ME_FindClick
      end
      object ME_Replace: TMenuItem
        Caption = 'R&eplace'
        ShortCut = 16466
        OnClick = ME_ReplaceClick
      end
      object ME_FindNext: TMenuItem
        Caption = 'Find &Next'
        ShortCut = 114
        OnClick = ME_FindNextClick
      end
      object ME_FindPrevious: TMenuItem
        Caption = 'Find Pre&vious'
        ShortCut = 8306
        OnClick = ME_FindPreviousClick
      end
      object ME_WebSearch: TMenuItem
        Caption = '&Web Search'
      end
      object N5: TMenuItem
        Caption = '-'
      end
      object MD_SortCard: TMenuItem
        Caption = '&Sort Card'
        object MDS_Title: TMenuItem
          Caption = '&Title'
          OnClick = MDS_TitleClick
        end
        object MDS_Title_I: TMenuItem
          Tag = 1
          Caption = 'T&itle (Inverse)'
          OnClick = MDS_TitleClick
        end
        object MDS_Date_Created: TMenuItem
          Caption = 'Date &Created'
          OnClick = MDS_Date_CreatedClick
        end
        object MDS_Date_Created_I: TMenuItem
          Tag = 1
          Caption = 'Date C&reated (Inverse)'
          OnClick = MDS_Date_CreatedClick
        end
        object MDS_Date_Edited: TMenuItem
          Caption = 'Date &Edited'
          OnClick = MDS_Date_EditedClick
        end
        object MDS_Date_Edited_I: TMenuItem
          Tag = 1
          Caption = 'Date E&dited (Inverse)'
          OnClick = MDS_Date_EditedClick
        end
        object MDS_Date_Viewed: TMenuItem
          Caption = 'Date &Viewed'
          OnClick = MDS_Date_ViewedClick
        end
        object MDS_Date_Viewed_I: TMenuItem
          Tag = 1
          Caption = 'Date Vie&wed (Inverse)'
          OnClick = MDS_Date_ViewedClick
        end
        object MDS_Score: TMenuItem
          Caption = '&Score'
          OnClick = MDS_ScoreClick
        end
        object MDS_Score_I: TMenuItem
          Tag = 1
          Caption = 'Sc&ore (Inverse)'
          OnClick = MDS_ScoreClick
        end
        object MDS_Shuffle: TMenuItem
          Caption = 'S&huffle'
          OnClick = MDS_ShuffleClick
        end
      end
      object ME_BatchConversion: TMenuItem
        Caption = '&Batch Conversion'
        object MEC_AllCards: TMenuItem
          Caption = 'All &Cards Shape...'
          OnClick = MEC_AllCardsClick
        end
        object MEC_AllLinks: TMenuItem
          Caption = 'All &Links Shape...'
          OnClick = MEC_AllLinksClick
        end
        object MEC_AllLinksDirection: TMenuItem
          Caption = 'All Links &Direction'
          OnClick = MEC_AllLinksDirectionClick
        end
      end
      object N8: TMenuItem
        Caption = '-'
      end
      object ME_Label: TMenuItem
        Caption = '&Label...'
        OnClick = ME_LabelClick
      end
      object ME_LinkLabel: TMenuItem
        Caption = 'L&ink Label...'
        OnClick = ME_LinkLabelClick
      end
    end
    object M_Insert: TMenuItem
      Caption = '&Insert'
      object MI_NewCard: TMenuItem
        Caption = 'New &Card'
        ShortCut = 16461
        OnClick = MI_NewCardClick
      end
      object MI_NewLink: TMenuItem
        Caption = 'New &Link...'
        ShortCut = 16462
        OnClick = MI_NewLinkClick
      end
      object MI_NewExtLink: TMenuItem
        Caption = 'New &Ext Link...'
        OnClick = MI_NewExtLinkClick
      end
      object MI_NewLabel: TMenuItem
        Caption = 'New L&abel...'
        OnClick = MI_NewLabelClick
      end
      object MI_NewLinkLabel: TMenuItem
        Caption = 'New Li&nk Label...'
        OnClick = MI_NewLinkLabelClick
      end
      object N48: TMenuItem
        Caption = '-'
      end
      object MI_LinktoAllCardswithDesignatedLabel: TMenuItem
        Caption = 'L&ink to All Cards with Designated Label...'
        OnClick = MI_LinktoAllCardswithDesignatedLabelClick
      end
      object MI_NewCardLinkstoAllCardswithDesignatedLabel: TMenuItem
        Caption = 'New Ca&rd Links to All Cards with Designated Label...'
        OnClick = MI_NewCardLinkstoAllCardswithDesignatedLabelClick
      end
      object MI_AddDesignatedLabeltoAllDestinationCards: TMenuItem
        Caption = 'A&dd Designated Label to All Destination Cards...'
        OnClick = MI_AddDesignatedLabeltoAllDestinationCardsClick
      end
    end
    object M_Animation: TMenuItem
      Caption = '&Animation'
      object MA_RandomFlash: TMenuItem
        Caption = 'Random &Flash'
        OnClick = MA_RandomFlashClick
      end
      object MA_RandomMap: TMenuItem
        Caption = 'Random &Map'
        OnClick = MA_RandomMapClick
      end
      object MA_RandomScroll: TMenuItem
        Caption = 'Random &Scroll'
        OnClick = MA_RandomScrollClick
      end
      object MA_RandomJump: TMenuItem
        Caption = 'Random &Jump'
        OnClick = MA_RandomJumpClick
      end
      object MA_RandomTrace: TMenuItem
        Caption = 'Random &Trace'
        OnClick = MA_RandomTraceClick
      end
    end
    object M_View: TMenuItem
      Caption = '&View'
      object MV_Font: TMenuItem
        Caption = '&Font'
        object MVF_DefaultSize: TMenuItem
          Caption = '&Default Size'
          OnClick = MVF_DefaultSizeClick
        end
        object MVF_Magnify: TMenuItem
          Caption = '&Magnify'
          ShortCut = 16417
          OnClick = MVF_MagnifyClick
        end
        object MVF_Reduce: TMenuItem
          Caption = '&Reduce'
          ShortCut = 16418
          OnClick = MVF_ReduceClick
        end
        object N21: TMenuItem
          Caption = '-'
        end
        object MVF_ChangeFont: TMenuItem
          Caption = '&Change Font...'
          OnClick = MVF_ChangeFontClick
        end
      end
      object MV_Card: TMenuItem
        Caption = '&Card'
        object MVC_CardShadow: TMenuItem
          Caption = 'Shadow'
          OnClick = MVC_CardShadowClick
        end
        object MVC_CardGradation: TMenuItem
          Caption = 'Gradation'
          OnClick = MVC_CardGradationClick
        end
        object N25: TMenuItem
          Caption = '-'
        end
        object MVC_TickerVisible: TMenuItem
          Caption = '&Ticker'
          OnClick = MVC_TickerVisibleClick
        end
        object MVC_Ticker1Line: TMenuItem
          Tag = 1
          Caption = '&1 Line'
          GroupIndex = 1
          RadioItem = True
          OnClick = MVC_TickerLineClick
        end
        object MVC_Ticker2Lines: TMenuItem
          Tag = 2
          Caption = '&2 Lines'
          GroupIndex = 1
          RadioItem = True
          OnClick = MVC_TickerLineClick
        end
        object N24: TMenuItem
          Caption = '-'
          GroupIndex = 1
        end
        object MVC_Image: TMenuItem
          Caption = '&Image'
          GroupIndex = 1
          OnClick = MVC_ImageClick
        end
        object MVC_Video: TMenuItem
          Caption = '&Video'
          GroupIndex = 1
          OnClick = MVC_VideoClick
        end
        object MVC_Drawing: TMenuItem
          Caption = '&Drawing'
          GroupIndex = 1
          OnClick = MVC_DrawingClick
        end
        object MVI_32: TMenuItem
          Tag = 32
          Caption = '32 x 32'
          GroupIndex = 2
          RadioItem = True
          OnClick = MVI_ImageLimitationClick
        end
        object MVI_40: TMenuItem
          Tag = 40
          Caption = '40 x 40'
          GroupIndex = 2
          RadioItem = True
          OnClick = MVI_ImageLimitationClick
        end
        object MVI_64: TMenuItem
          Tag = 64
          Caption = '64 x 64'
          GroupIndex = 2
          RadioItem = True
          OnClick = MVI_ImageLimitationClick
        end
        object MVI_80: TMenuItem
          Tag = 80
          Caption = '80 x 80'
          Checked = True
          GroupIndex = 2
          RadioItem = True
          OnClick = MVI_ImageLimitationClick
        end
        object MVI_120: TMenuItem
          Tag = 120
          Caption = '120 x 120'
          GroupIndex = 2
          RadioItem = True
          OnClick = MVI_ImageLimitationClick
        end
        object MVI_160: TMenuItem
          Tag = 160
          Caption = '160 x 160'
          GroupIndex = 2
          RadioItem = True
          OnClick = MVI_ImageLimitationClick
        end
        object MVI_240: TMenuItem
          Tag = 240
          Caption = '240 x 240'
          GroupIndex = 2
          RadioItem = True
          OnClick = MVI_ImageLimitationClick
        end
        object MVI_320: TMenuItem
          Tag = 320
          Caption = '320 x 320'
          GroupIndex = 2
          RadioItem = True
          OnClick = MVI_ImageLimitationClick
        end
      end
      object MV_Link: TMenuItem
        Caption = '&Link'
        object MVL_Link: TMenuItem
          Caption = '&Link'
          GroupIndex = 1
          OnClick = MVL_LinkClick
        end
        object MVL_LinkHemming: TMenuItem
          Caption = '&Hemming'
          GroupIndex = 1
          OnClick = MVL_LinkHemmingClick
        end
        object MVL_LinkShadow: TMenuItem
          Caption = '&Shadow'
          GroupIndex = 1
          Visible = False
          OnClick = MVL_LinkShadowClick
        end
        object MVL_LinkDirection: TMenuItem
          Caption = '&Direction'
          GroupIndex = 2
          OnClick = MVL_LinkDirectionClick
        end
        object MVL_LinkName: TMenuItem
          Caption = '&Name'
          GroupIndex = 3
          OnClick = MVL_LinkNameClick
        end
      end
      object MV_Label: TMenuItem
        Caption = 'L&abel'
        object MVL_LabelCircle: TMenuItem
          Caption = '&Circle'
          OnClick = MVL_LabelCircleClick
        end
        object MVL_LabelRectangle: TMenuItem
          Caption = '&Rectangle'
          OnClick = MVL_LabelRectangleClick
        end
        object MVL_LabelName: TMenuItem
          Caption = '&Name'
          OnClick = MVL_LabelNameClick
        end
      end
      object MV_Text: TMenuItem
        Caption = '&Text'
        object MVT_Text: TMenuItem
          Caption = '&Text'
          OnClick = MVT_TextClick
        end
        object MVT_Centering: TMenuItem
          Caption = '&Centering'
          OnClick = MVT_CenteringClick
        end
        object MVT_WordWrap: TMenuItem
          Caption = '&Word Wrap'
          OnClick = MVT_WordWrapClick
        end
        object N35: TMenuItem
          Caption = '-'
        end
        object MVT_EditInBrowser: TMenuItem
          Caption = '&Edit in Browser'
          OnClick = MVT_EditInBrowserClick
        end
        object MVT_AlwaysShowEditor: TMenuItem
          Caption = '&Always Show Editor'
          OnClick = MVT_AlwaysShowEditorClick
        end
        object MVT_UnderTheCard: TMenuItem
          Caption = '&Under the Card'
          GroupIndex = 1
          RadioItem = True
          OnClick = MVT_BrowserPosClick
        end
        object MVT_BrowserRight: TMenuItem
          Tag = 1
          Caption = 'Browser &Right'
          GroupIndex = 1
          RadioItem = True
          OnClick = MVT_BrowserPosClick
        end
        object MVT_BrowserBottom: TMenuItem
          Tag = 2
          Caption = 'Browser &Bottom'
          GroupIndex = 1
          RadioItem = True
          OnClick = MVT_BrowserPosClick
        end
      end
      object MV_Score: TMenuItem
        Caption = '&Score'
        object MVS_Score: TMenuItem
          Caption = '&Score'
          OnClick = MVS_ScoreClick
        end
        object N17: TMenuItem
          Caption = '-'
        end
        object MVS_Authenticity: TMenuItem
          Caption = '&Authenticity'
          GroupIndex = 1
          RadioItem = True
          OnClick = MVS_ScoreClickType
        end
        object MVS_StartingPoint: TMenuItem
          Tag = 1
          Caption = 'S&tarting Point'
          GroupIndex = 1
          RadioItem = True
          OnClick = MVS_ScoreClickType
        end
        object MVS_Destination: TMenuItem
          Tag = 2
          Caption = '&Destination'
          GroupIndex = 1
          RadioItem = True
          OnClick = MVS_ScoreClickType
        end
        object MVS_Links_Out: TMenuItem
          Tag = 3
          Caption = 'Links(&Out)'
          GroupIndex = 1
          RadioItem = True
          OnClick = MVS_ScoreClickType
        end
        object MVS_Links_In: TMenuItem
          Tag = 4
          Caption = 'Links(&In)'
          GroupIndex = 1
          RadioItem = True
          OnClick = MVS_ScoreClickType
        end
        object MVS_Links_Total: TMenuItem
          Tag = 5
          Caption = 'Links(&Total)'
          GroupIndex = 1
          RadioItem = True
          OnClick = MVS_ScoreClickType
        end
        object MVS_Links_InOut: TMenuItem
          Tag = 6
          Caption = '&Links(In-Out)'
          GroupIndex = 1
          RadioItem = True
          OnClick = MVS_ScoreClickType
        end
        object MVS_TextLength: TMenuItem
          Tag = 7
          Caption = 'Te&xt Length'
          GroupIndex = 1
          RadioItem = True
          OnClick = MVS_ScoreClickType
        end
      end
      object MV_Read: TMenuItem
        Caption = '&Read Aloud'
        object MVR_Read: TMenuItem
          Caption = '&Read Aroud'
          OnClick = MVR_ReadClick
        end
        object MVR_ChangeAgent: TMenuItem
          Caption = 'Change &Agent...'
          Visible = False
          OnClick = MVR_ChangeAgentClick
        end
        object MVR_ReadSetting: TMenuItem
          Caption = '&Setting...'
          Visible = False
          OnClick = MVR_ReadSettingClick
        end
      end
      object MV_Others: TMenuItem
        Caption = '&Others'
        object MVO_CardList: TMenuItem
          Caption = 'Card List'
          OnClick = MVO_CardListClick
        end
        object MVO_FileList: TMenuItem
          Caption = 'File List'
          OnClick = MVO_FileListClick
        end
        object N45: TMenuItem
          Caption = '-'
        end
        object MVO_Overview: TMenuItem
          Caption = '&Overview'
          OnClick = MVO_OverviewClick
        end
        object N22: TMenuItem
          Caption = '-'
        end
        object MVO_ChangeFourgroundColor: TMenuItem
          Caption = 'Change &Fourground Color...'
          OnClick = MVO_ChangeFourgroundColorClick
        end
        object MVO_ChangeBackgroundColor: TMenuItem
          Caption = 'Change &Background Color...'
          OnClick = MVO_ChangeBackgroundColorClick
        end
        object N20: TMenuItem
          Caption = '-'
        end
        object MVO_WallPaper: TMenuItem
          Caption = '&Wall Paper...'
          OnClick = MVO_WallPaperClick
        end
        object MVO_FixWallPaper: TMenuItem
          Caption = 'F&ix Wall Paper'
          OnClick = MVO_FixWallPaperClick
        end
        object MVO_TileWallPaper: TMenuItem
          Caption = '&Tile Wall Paper'
          OnClick = MVO_TileWallPaperClick
        end
        object N33: TMenuItem
          Caption = '-'
        end
        object MVO_BGAnimation: TMenuItem
          Caption = 'Ba&ckground Animation'
          OnClick = MVO_BGAnimationClick
        end
        object MVO_BGAnimationType: TMenuItem
          Caption = 'Bac&kground Animation Type'
          object MVOB_MovingLine: TMenuItem
            Caption = '&Moving Line'
            GroupIndex = 1
            RadioItem = True
            OnClick = MVOB_BGAnimationTypeClick
          end
          object MVOB_Bubble: TMenuItem
            Tag = 1
            Caption = '&Bubble'
            GroupIndex = 1
            RadioItem = True
            OnClick = MVOB_BGAnimationTypeClick
          end
          object MVOB_Snow: TMenuItem
            Tag = 2
            Caption = '&Snow'
            GroupIndex = 1
            RadioItem = True
            OnClick = MVOB_BGAnimationTypeClick
          end
          object MVOB_CherryBlossom: TMenuItem
            Tag = 3
            Caption = '&Cherry Blossom'
            GroupIndex = 1
            RadioItem = True
            OnClick = MVOB_BGAnimationTypeClick
          end
        end
        object MVO_CursorAnimation: TMenuItem
          Caption = 'C&ursor Animation'
          OnClick = MVO_CursorAnimationClick
        end
        object MVO_NoScrollLag: TMenuItem
          Caption = '&No Scroll Lag'
          OnClick = MVO_NoScrollLagClick
        end
        object N34: TMenuItem
          Caption = '-'
        end
        object MVO_AntiAliasing: TMenuItem
          Caption = '&Anti-Aliasing'
          OnClick = MVO_AntiAliasingClick
        end
        object MVO_AA2x: TMenuItem
          Tag = 2
          Caption = '&2x'
          GroupIndex = 1
          RadioItem = True
          OnClick = MVO_AAClick
        end
        object MVO_AA4x: TMenuItem
          Tag = 4
          Caption = '&4x'
          GroupIndex = 1
          RadioItem = True
          OnClick = MVO_AAClick
        end
      end
      object MV_ChangeLanguage: TMenuItem
        Caption = 'C&hange Language...'
        object MVC_English: TMenuItem
          Caption = '&English'
          Checked = True
          GroupIndex = 1
          RadioItem = True
          OnClick = MVC_EnglishClick
        end
      end
      object N9: TMenuItem
        Caption = '-'
      end
      object MV_FullScreen: TMenuItem
        Caption = 'F&ull Screen'
        ShortCut = 121
        OnClick = MV_FullScreenClick
      end
      object N14: TMenuItem
        Caption = '-'
      end
      object MV_StatusBar: TMenuItem
        Caption = 'Status &Bar'
        OnClick = MV_StatusBarClick
      end
      object MV_Browser: TMenuItem
        Caption = 'Browser'
        ShortCut = 16450
        Visible = False
        OnClick = MV_BrowserClick
      end
      object MV_Editor: TMenuItem
        Caption = 'Editor'
        ShortCut = 16453
        Visible = False
        OnClick = MV_EditorClick
      end
      object MV_EditInBrowser: TMenuItem
        Caption = 'Edit In Browser'
        ShortCut = 24645
        Visible = False
        OnClick = MV_EditInBrowserClick
      end
      object MV_EditTitle: TMenuItem
        Caption = 'Edit Title'
        ShortCut = 113
        Visible = False
        OnClick = MV_EditTitleClick
      end
    end
    object M_Help: TMenuItem
      Caption = '&Help'
      object MH_Contents: TMenuItem
        Caption = '&Contents...'
        ShortCut = 112
        OnClick = MH_ContentsClick
      end
      object MH_FrieveSite: TMenuItem
        Caption = 'Open &Frieve Web Site...'
        OnClick = MH_FrieveSiteClick
      end
      object MH_FIPSite: TMenuItem
        Caption = '&Open Frieve Editor Web Site...'
        OnClick = MH_FIPSiteClick
      end
      object MH_CheckLatest: TMenuItem
        Caption = 'Check &Latest Release...'
        OnClick = MH_CheckLatestClick
      end
      object N6: TMenuItem
        Caption = '-'
      end
      object MH_About: TMenuItem
        Caption = '&About...'
        OnClick = MH_AboutClick
      end
      object MH_EnableSpecialMenu: TMenuItem
        Caption = '&Enable Special Menu'
        Default = True
        ShortCut = 57464
        Visible = False
        OnClick = MH_EnableSpecialMenuClick
      end
    end
    object M_Special: TMenuItem
      Caption = '&Special'
      Visible = False
      object MS_ResetAllDates: TMenuItem
        Caption = '&Reset All Dates'
        OnClick = MS_ResetAllDatesClick
      end
      object MS_TextAnalysisTest: TMenuItem
        Caption = 'Text Analysis Test'
        OnClick = MS_TextAnalysisTestClick
      end
      object MS_OutputWordNgram: TMenuItem
        Caption = '&Output Word N-gram...'
        OnClick = MS_OutputWordNgramClick
      end
      object MS_ImportCSV: TMenuItem
        Caption = 'Import CSV...'
        OnClick = MS_ImportCSVClick
      end
      object MS_Demo: TMenuItem
        Caption = '&Demo'
        Enabled = False
        ShortCut = 120
        Visible = False
      end
    end
  end
  object OD: TOpenDialog
    DefaultExt = 'fip'
    FileName = '*.fip'
    Filter = '*.fip|*.fip|*.*|*.*'
    Left = 256
    Top = 192
  end
  object SD: TSaveDialog
    DefaultExt = 'fip'
    FileName = '*.fip'
    Filter = '*.fip|*.fip|*.*|*.*'
    Options = [ofOverwritePrompt, ofHideReadOnly, ofEnableSizing]
    Left = 288
    Top = 192
  end
  object PM_Link: TPopupMenu
    Left = 224
    Top = 160
    object PI_NewLink: TMenuItem
      Caption = '&New Link...'
      OnClick = MI_NewLinkClick
    end
    object PI_DeleteLink: TMenuItem
      Caption = '&Delete Link'
      OnClick = PI_DeleteLinkClick
    end
  end
  object Po_Label: TPopupMenu
    Left = 576
    Top = 56
    object PL_NoLabel: TMenuItem
      Caption = 'No Label'
      GroupIndex = 1
      RadioItem = True
      OnClick = PL_Click
    end
    object N4: TMenuItem
      Caption = '-'
      GroupIndex = 1
    end
  end
  object Po_LinkLabel: TPopupMenu
    Left = 616
    Top = 56
    object PL_NoLinkLabel: TMenuItem
      Caption = 'No Label'
      GroupIndex = 1
      RadioItem = True
      OnClick = PL_NoLinkLabelClick
    end
    object MenuItem2: TMenuItem
      Caption = '-'
      GroupIndex = 1
    end
  end
  object Po_ExtLink: TPopupMenu
    AutoHotkeys = maManual
    Left = 224
    Top = 224
  end
  object OD_Ext: TOpenDialog
    FileName = '*.*'
    Filter = '*.*|*.*'
    Left = 320
    Top = 192
  end
  object PM_Editor: TPopupMenu
    Left = 568
    Top = 224
    object PE_Undo: TMenuItem
      Caption = '&Undo'
      OnClick = ME_UndoClick
    end
    object PE_Redo: TMenuItem
      Caption = '&Redo'
      OnClick = ME_RedoClick
    end
    object N11: TMenuItem
      Caption = '-'
    end
    object PE_Cut: TMenuItem
      Caption = 'Cu&t'
      OnClick = ME_CutClick
    end
    object PE_Copy: TMenuItem
      Caption = '&Copy'
      OnClick = ME_CopyClick
    end
    object PE_Paste: TMenuItem
      Caption = '&Paste'
      OnClick = ME_PasteClick
    end
    object PE_Delete: TMenuItem
      Caption = '&Delete'
      OnClick = ME_DeleteClick
    end
    object PE_SelectAll: TMenuItem
      Caption = 'Select &All'
      OnClick = ME_SelectAllClick
    end
    object PE_Sep1: TMenuItem
      Caption = '-'
    end
    object N46: TMenuItem
      Caption = '-'
    end
    object PE_CutToNewCard: TMenuItem
      Caption = 'Cut to &New Card'
      OnClick = PE_CutToNewCardClick
    end
    object PE_CutToNewCardTitle: TMenuItem
      Caption = 'Cut to N&ew Card Title'
      OnClick = PE_CutToNewCardTitleClick
    end
    object PE_CutToNewCardTitleWithLink: TMenuItem
      Caption = 'Cut to New Card Title with &Link'
      OnClick = PE_CutToNewCardTitleWithLinkClick
    end
  end
  object OD_Txt: TOpenDialog
    DefaultExt = 'txt'
    FileName = '*.txt'
    Filter = '*.txt|*.txt|*.*|*.*'
    Options = [ofHideReadOnly, ofAllowMultiSelect, ofEnableSizing]
    Left = 256
    Top = 224
  end
  object SD_Txt: TSaveDialog
    DefaultExt = 'txt'
    FileName = '*.txt'
    Filter = '*.txt|*.txt|*.*|*.*'
    Options = [ofOverwritePrompt, ofHideReadOnly, ofEnableSizing]
    Left = 288
    Top = 224
  end
  object SD_Html: TSaveDialog
    DefaultExt = 'html'
    FileName = '*.html'
    Filter = '*.html|*.html|*.*|*.*'
    Options = [ofOverwritePrompt, ofHideReadOnly, ofEnableSizing]
    Left = 320
    Top = 224
  end
  object IL_Shape: TImageList
    Height = 13
    Width = 31
    Left = 384
    Top = 64
    Bitmap = {
      494C0101100013000C001F000D00FFFFFFFFFF10FFFFFFFFFFFFFFFF424D3600
      00000000000036000000280000007C000000410000000100200000000000F07D
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000424D3E000000000000003E000000280000007C0000004100000001000100
      00000000100400000000000000000000000000000000000000000000FFFFFF00
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF0
      FFFFFFFFFFFFFFFFFFFFFFFFFFBEFFF0FFFEFFFFFFFDFFFFFFC0FFFFFF9CFFF0
      FFFD7FFFFFFAFFFFFFDEFFFFFFAAFFF0FFFD7FFFFFF77FFFFFBF7FFFFFD5FFF0
      FFFBBFFFFFEFBFFFFFBF7FFFFFDDFFF0FFF7DFFFFFDFDFFFFF7FBFFFFF3E7FF0
      FFF7DFFFFFEFBFFFFFBF7FFFFE083FF0FFEFEFFFFFF77FFFFFBF7FFFFFEBFFF0
      FFE00FFFFFFAFFFFFFDEFFFFFFEBFFF0FFFFFFFFFFFDFFFFFFC0FFFFFFF7FFF0
      FFFFFFFFFFFFFFFFFFFFFFFFFFF7FFF0FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF0
      FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF0FC0000FFFFFFFFFFFFFFFFFFFFFFFFF0
      FBFFFF7FFFFFFFFFFFFFFFFFFFFFFFF0F7FFFFBFFFC03FFFFFE1FFFFFFFFFFF0
      F7FFFFBFFFDFBFFFFFDEFFFFFF007FF0EFFFFFDFFFDFBFFFFFBF7FFFFF7F7FF0
      EFFFFFDFFFDFBFFFFFBF7FFFFFBEFFF0DFFFFFEFFFDFBFFFFFBF7FFFFFBEFFF0
      DFFFFFEFFFDFBFFFFFBF7FFFFFDDFFF0BFFFFFF7FFDFBFFFFFDEFFFFFFEBFFF0
      BFFFFFF7FFC03FFFFFE1FFFFFFEBFFF07FFFFFFBFFFFFFFFFFFFFFFFFFF7FFF0
      00000003FFFFFFFFFFFFFFFFFFFFFFF0FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF0
      FF8007FFFFF0FFFFC00000F800000010F87FF87FFF8F1FFFBFFFFF7BFFFFFFD0
      E7FFFF9FFC7FE3FF7FFFFFBDFFFFFFB0DFFFFFEFE3FFFC7F7FFFFFBDFFFFFFB0
      BFFFFFF71FFFFF8EFFFFFFDEFFFFFF707FFFFFFAFFFFFFF5FFFFFFEEFFFFFF70
      7FFFFFFAFFFFFFF5FFFFFFEF7FFFFEF0BFFFFFF71FFFFF8EFFFFFFDF7FFFFEF0
      DFFFFFEFE3FFFC7F7FFFFFBFBFFFFDF0E7FFFF9FFC7FE3FF7FFFFFBFBFFFFDF0
      F87FF87FFF8F1FFFBFFFFF7FDFFFFBF0FF8007FFFFF0FFFFC00000FFE00007F0
      FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF0FFFFFFFE000000070000003F800001F0
      FFFFFFFEFFFFFFF6FFFFFFDE7FFFFE70FFFFFFFEFFFFFFF5FFFFFFEDFFFFFFB0
      FFFFFFFEFFFFFFF5FFFFFFEDFFFFFFB0FFFFFFFEFFFFFFF5FFFFFFEBFFFFFFD0
      FFFFFFFEFFFFFFF5FFFFFFEBFFFFFFD0FFFFFFFEFFFFFFF5FFFFFFEBFFFFFFD0
      FFFFFFFEFFFFFFF5FFFFFFEBFFFFFFD0FFFFFFFEFFFFFFF5FFFFFFEDFFFFFFB0
      FFFFFFFEFFFFFFF5FFFFFFEDFFFFFFB0FFFFFFFEFFFFFFF6FFFFFFDE7FFFFE70
      FFFFFFFE000000070000003F800001F000000000000000000000000000000000
      000000000000}
  end
  object PD_Text: TPrintDialog
    Options = [poPageNums, poDisablePrintToFile]
    Left = 16
    Top = 288
  end
  object PD_Image: TPrintDialog
    Options = [poDisablePrintToFile]
    Left = 48
    Top = 288
  end
  object CD: TColorDialog
    Left = 80
    Top = 288
  end
  object OD_ACS: TOpenDialog
    FileName = '*.acs'
    Filter = '*.acs|*.acs'
    Left = 352
    Top = 192
  end
  object PM_ArrangeType: TPopupMenu
    AutoHotkeys = maManual
    Left = 360
    Top = 128
    object PAT_Normalize: TMenuItem
      Caption = '&Normalize'
      OnClick = PAT_ArrangeTypeClick
    end
    object PAT_Repulsion: TMenuItem
      Tag = 1
      Caption = '&Repulsion'
      OnClick = PAT_ArrangeTypeClick
    end
    object PAT_Link: TMenuItem
      Tag = 2
      Caption = '&Link'
      OnClick = PAT_ArrangeTypeClick
    end
    object PAT_Label: TMenuItem
      Tag = 3
      Caption = 'L&abel'
      OnClick = PAT_ArrangeTypeClick
    end
    object PAT_Index: TMenuItem
      Tag = 4
      Caption = '&Index'
      OnClick = PAT_ArrangeTypeClick
    end
    object N18: TMenuItem
      Caption = '-'
    end
    object PAT_SoftLink: TMenuItem
      Tag = 102
      Caption = 'Lin&k(Soft)'
      OnClick = PAT_ArrangeTypeClick
    end
    object PAT_SoftLabel: TMenuItem
      Tag = 103
      Caption = 'La&bel(Soft)'
      OnClick = PAT_ArrangeTypeClick
    end
    object PAT_SoftIndex: TMenuItem
      Tag = 104
      Caption = 'In&dex(Soft)'
      OnClick = PAT_ArrangeTypeClick
    end
    object N19: TMenuItem
      Caption = '-'
      OnClick = PAT_ArrangeTypeClick
    end
    object PAT_Matrix: TMenuItem
      Tag = 200
      Caption = '&Matrix'
      OnClick = PAT_ArrangeTypeClick
    end
    object PAT_LinkMatrix: TMenuItem
      Tag = 202
      Caption = 'Link(Matrix)'
      Visible = False
      OnClick = PAT_ArrangeTypeClick
    end
    object PAT_LabelMatrix: TMenuItem
      Tag = 203
      Caption = 'Label(Matrix)'
      Visible = False
      OnClick = PAT_ArrangeTypeClick
    end
    object PAT_MatrixIndex: TMenuItem
      Tag = 204
      Caption = 'Ind&ex(Matrix)'
      OnClick = PAT_ArrangeTypeClick
    end
    object N23: TMenuItem
      Caption = '-'
    end
    object PAT_Similarity: TMenuItem
      Tag = 500
      Caption = '&Similarity'
      OnClick = PAT_ArrangeTypeClick
    end
    object PAT_SimilaritySoft: TMenuItem
      Tag = 600
      Caption = 'Similarity(Soft)'
      OnClick = PAT_ArrangeTypeClick
    end
    object PAT_SimilarityMatrix: TMenuItem
      Tag = 700
      Caption = 'Similarity(Matrix)'
      Visible = False
      OnClick = PAT_ArrangeTypeClick
    end
    object N44: TMenuItem
      Caption = '-'
    end
    object PAT_TreeRadial: TMenuItem
      Tag = 1000
      Caption = '&Tree (Radial)'
      OnClick = PAT_ArrangeTypeClick
    end
  end
  object OD_WallPaper: TOpenDialog
    DefaultExt = 'jpg'
    FileName = '*.bmp;*.jpg;*.jpeg'
    Filter = '*.bmp;*.jpg;*.jpeg|*.bmp;*.jpg;*.jpeg|*.*|*.*'
    Left = 200
    Top = 216
  end
  object FD_Font: TFontDialog
    Font.Charset = SHIFTJIS_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = #65325#65331' '#65328#12468#12471#12483#12463
    Font.Style = []
    Left = 384
    Top = 192
  end
  object PM_BNoSelect: TPopupMenu
    Left = 24
    Top = 208
    object PBN_Select: TMenuItem
      Caption = '&Select'
      GroupIndex = 1
      RadioItem = True
      OnClick = PBN_SelectClick
    end
    object PBN_Link: TMenuItem
      Tag = 1
      Caption = '&Link'
      GroupIndex = 1
      RadioItem = True
      OnClick = PBN_LinkClick
    end
    object N38: TMenuItem
      Caption = '-'
      GroupIndex = 1
    end
    object PBN_Undo: TMenuItem
      Caption = '&Undo'
      GroupIndex = 1
      OnClick = ME_UndoClick
    end
    object PBN_Redo: TMenuItem
      Caption = '&Redo'
      GroupIndex = 1
      OnClick = ME_RedoClick
    end
    object N36: TMenuItem
      Caption = '-'
      GroupIndex = 1
    end
    object PBN_Paste: TMenuItem
      Caption = '&Paste'
      GroupIndex = 1
      OnClick = ME_PasteClick
    end
    object N37: TMenuItem
      Caption = '-'
      GroupIndex = 1
    end
    object PBN_NewCard: TMenuItem
      Caption = 'New &Card'
      GroupIndex = 1
      OnClick = PBN_NewCardClick
    end
  end
  object PM_BCardSelect: TPopupMenu
    Left = 56
    Top = 208
    object PBC_Select: TMenuItem
      Caption = '&Select'
      GroupIndex = 1
      RadioItem = True
      OnClick = PBN_SelectClick
    end
    object PBC_Link: TMenuItem
      Tag = 1
      Caption = '&Link'
      GroupIndex = 1
      RadioItem = True
      OnClick = PBN_LinkClick
    end
    object N39: TMenuItem
      Caption = '-'
      GroupIndex = 1
    end
    object PBC_Undo: TMenuItem
      Caption = '&Undo'
      GroupIndex = 1
      OnClick = ME_UndoClick
    end
    object PBC_Redo: TMenuItem
      Caption = '&Redo'
      GroupIndex = 1
      OnClick = ME_RedoClick
    end
    object N40: TMenuItem
      Caption = '-'
      GroupIndex = 1
    end
    object PBC_Cut: TMenuItem
      Caption = 'Cu&t'
      GroupIndex = 1
      OnClick = ME_CutClick
    end
    object PBC_Copy: TMenuItem
      Caption = '&Copy'
      GroupIndex = 1
      OnClick = ME_CopyClick
    end
    object PBC_Paste: TMenuItem
      Caption = '&Paste'
      GroupIndex = 1
      OnClick = ME_PasteClick
    end
    object N41: TMenuItem
      Caption = '-'
      GroupIndex = 1
    end
    object PBC_WebSearch: TMenuItem
      Caption = '&Web Search'
      GroupIndex = 1
    end
    object N12: TMenuItem
      Caption = '-'
      GroupIndex = 1
    end
    object PBC_NewCard: TMenuItem
      Caption = 'New &Card'
      GroupIndex = 1
      OnClick = PBN_NewCardClick
    end
    object PBC_NewChildCard: TMenuItem
      Caption = 'New C&hild'
      GroupIndex = 1
      OnClick = PBC_NewChildCardClick
    end
    object PBC_NewBrotherCard: TMenuItem
      Caption = 'New &Brother'
      GroupIndex = 1
      OnClick = PBC_NewBrotherCardClick
    end
    object PBC_NewLink: TMenuItem
      Caption = 'New &Link...'
      GroupIndex = 1
      OnClick = MI_NewLinkClick
    end
    object PBC_NewExtLink: TMenuItem
      Caption = 'New &Ext Link...'
      GroupIndex = 1
      OnClick = MI_NewExtLinkClick
    end
    object PBC_NewLabel: TMenuItem
      Caption = 'New L&abel...'
      GroupIndex = 1
      OnClick = MI_NewLabelClick
    end
    object N16: TMenuItem
      Caption = '-'
      GroupIndex = 1
    end
    object PBC_SetAsDefault: TMenuItem
      Caption = '&Set as Default'
      GroupIndex = 1
      OnClick = PBC_SetAsDefaultClick
    end
    object N26: TMenuItem
      Caption = '-'
      GroupIndex = 1
    end
    object PBC_DeleteCard: TMenuItem
      Caption = '&Delete Card'
      GroupIndex = 1
      OnClick = PL_DeleteCardClick
    end
    object N27: TMenuItem
      Caption = '-'
      GroupIndex = 1
    end
    object PBC_Property: TMenuItem
      Caption = '&Property...'
      GroupIndex = 1
      OnClick = PL_CardPropertyClick
    end
  end
  object PM_BLinkSelect: TPopupMenu
    Left = 88
    Top = 208
    object PBL_Select: TMenuItem
      Caption = '&Select'
      GroupIndex = 1
      RadioItem = True
      OnClick = PBN_SelectClick
    end
    object PBL_Link: TMenuItem
      Tag = 1
      Caption = '&Link'
      GroupIndex = 1
      RadioItem = True
      OnClick = PBN_LinkClick
    end
    object N42: TMenuItem
      Caption = '-'
      GroupIndex = 1
    end
    object PBL_Undo: TMenuItem
      Caption = '&Undo'
      GroupIndex = 1
      OnClick = ME_UndoClick
    end
    object PBL_Redo: TMenuItem
      Caption = '&Redo'
      GroupIndex = 1
      OnClick = ME_RedoClick
    end
    object N47: TMenuItem
      Caption = '-'
      GroupIndex = 1
    end
    object PBL_WebSearch: TMenuItem
      Caption = '&Web Search'
      GroupIndex = 1
    end
    object N43: TMenuItem
      Caption = '-'
      GroupIndex = 1
    end
    object PBL_NewCard: TMenuItem
      Caption = 'New &Card'
      GroupIndex = 1
      OnClick = PBN_NewCardClick
    end
    object PBL_InsertCard: TMenuItem
      Caption = '&Insert Card'
      GroupIndex = 1
      OnClick = PBL_InsertCardClick
    end
    object PBL_NewLinkLabel: TMenuItem
      Caption = 'New Li&nk Label'
      GroupIndex = 1
      OnClick = MI_NewLinkLabelClick
    end
    object N28: TMenuItem
      Caption = '-'
      GroupIndex = 1
    end
    object PBL_SetAsDefault: TMenuItem
      Caption = '&Set as Default'
      GroupIndex = 1
      OnClick = PBL_SetAsDefaultClick
    end
    object N29: TMenuItem
      Caption = '-'
      GroupIndex = 1
    end
    object PBL_DeleteLink: TMenuItem
      Caption = '&Delete Link'
      GroupIndex = 1
      OnClick = PBL_DeleteLinkClick
    end
  end
  object IL_LinkShape: TImageList
    Height = 13
    Width = 31
    Left = 904
    Top = 64
    Bitmap = {
      494C01010C000E000C001F000D00FFFFFFFFFF10FFFFFFFFFFFFFFFF424D3600
      00000000000036000000280000007C000000340000000100200000000000C064
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000C0C0C000C0C0C00080808000808080008080800080808000C0C0
      C000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000C0C0C000C0C0C000808080008080800080808000808080000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000808080008080800080808000808080000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000808080008080
      8000C0C0C0000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000008080800080808000C0C0C0000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000008080800080808000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000C0C0C00000000000000000008080
      8000808080000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000C0C0C000C0C0C000000000000000
      0000000000008080800080808000808080000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000080808000808080008080800080808000808080000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000080808000808080008080800080808000808080008080
      8000808080000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000008080800080808000808080008080800080808000808080008080
      80008080800080808000C0C0C000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000080808000808080008080
      800080808000808080008080800080808000C0C0C00000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000008080800080808000C0C0C000C0C0C00000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000008080800080808000C0C0C000C0C0C00000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000808080008080800080808000808080008080
      8000808080008080800080808000808080008080800080808000808080008080
      8000808080008080800080808000808080008080800080808000808080008080
      8000808080008080800080808000808080008080800080808000808080008080
      8000808080000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000008080
      8000808080008080800080808000808080008080800080808000808080008080
      8000808080008080800080808000808080008080800080808000808080008080
      80008080800080808000C0C0C000C0C0C000C0C0C00000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000808080008080800080808000808080008080
      80008080800080808000C0C0C000C0C0C000C0C0C00000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000424D3E000000000000003E000000
      280000007C000000340000000100010000000000400300000000000000000000
      000000000000000000000000FFFFFF0000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF0FFFFFFC1FFFFFFF3FFFFFF07FFFFF800
      FFFFFE3FFFFFF87FFFFFF8FFFFFF81F0FFFFEFFFFFFFC7FFFFFFBFFFFFFE1FF0
      FFC01FFFFF803FFFFF007FFFFFF87FF0FFFF3FFFFFFF7FFFFFFEFFFFFFF0FFF0
      FFFEBFFFFFFF7FFFFFFEFFFFFFE3FFF0FFFDBFFFFFFF7FFFFFFEFFFFFF07FFF0
      FFF3BFFFFFE77FFFFFCEFFFFF00FFFF0FFEFBFFFFFDF7FFFFFBEFFF8001FFFF0
      F8FFBFFFF1FF7FFFE3FEFFF8007FFFF007FFBFFE0FFF7FFC1FFEFFF801FFFFF0
      FFFFFFFFFFFFFFFFFFFFFFF807FFFFF0FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF0
      FFFFFFFFFFFFFFFFFFFFFF07FFFFFFC0FFFFFFFFFFFFFFFFFFFFF0FFFFFFF1F0
      FFFBFFFFFFFFFFFFFFFF8FFFFFFF7FF0FFFCFFFE007FFFFFFF007FFFFE00FFF0
      FFFF3FFE000007FFFFFCFFFFFFFDFFF000E38E0200000007FFFAFFFFFFFDFFF0
      FFFF3FFE000007FFFFF6FFFFFFFDFFF0FFFCFFFE007FFFFFFFCEFFFFFF9DFFF0
      FFFBFFFFFFFFFFFFFE3EFFFFFF7DFFF0FFFFFFFFFFFFFFFFE1FEFFFFC7FDFFF0
      FFFFFFFFFFFFFFFC1FFEFFF9FFFDFFF0FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF0
      FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF0FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF0
      FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF0FFFBFFFFFFF7FFFFFFEFFFFFFFDFFFF0
      FFFCFFFFFFF9FFFFFFF3FFFFFFE7FFF0FFFF3FFFFFFE7FFFFFFCFFFFFFF9FFF0
      0000000271C71C7403803808071C01D0FFFF3FFFFFFE7FFFFFFCFFFFFFF9FFF0
      FFFCFFFFFFF9FFFFFFF3FFFFFFE7FFF0FFFBFFFFFFF7FFFFFFEFFFFFFFDFFFF0
      FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF0FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF0
      FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF000000000000000000000000000000000
      000000000000}
  end
  object SD_BMP: TSaveDialog
    DefaultExt = 'bmp'
    FileName = '*.bmp'
    Filter = '*.bmp|*.bmp|*.*|*.*'
    Options = [ofOverwritePrompt, ofHideReadOnly, ofEnableSizing]
    Left = 352
    Top = 224
  end
  object SD_JPEG: TSaveDialog
    DefaultExt = 'jpg'
    FileName = '*.jpg'
    Filter = '*.jpg|*.jpg|*.*|*.*'
    Options = [ofOverwritePrompt, ofHideReadOnly, ofEnableSizing]
    Left = 384
    Top = 224
  end
  object PM_StatisticsKey: TPopupMenu
    AutoHotkeys = maManual
    Left = 160
    Top = 136
    object PMSK_Label: TMenuItem
      Caption = '&Label'
      OnClick = PM_StatisticKeyClick
    end
    object PMSK_NumberOfLink: TMenuItem
      Tag = 10000
      Caption = '&Number of Link'
      OnClick = Bu_ArrangeTypeClick
      object PMSKL_Total: TMenuItem
        Tag = 10000
        Caption = '&Total'
        OnClick = PM_StatisticKeyClick
      end
      object PMSKL_Source: TMenuItem
        Tag = 10100
        Caption = '&Source'
        OnClick = PM_StatisticKeyClick
      end
      object PMSKL_Destination: TMenuItem
        Tag = 10200
        Caption = '&Destination'
        OnClick = PM_StatisticKeyClick
      end
    end
    object PMSK_CreatedDate: TMenuItem
      Caption = '&Created Date'
      object PMSKC_Year: TMenuItem
        Tag = 20000
        Caption = '&Year'
        OnClick = PM_StatisticKeyClick
      end
      object PMSKC_Month: TMenuItem
        Tag = 20100
        Caption = '&Month'
        OnClick = PM_StatisticKeyClick
      end
      object PMSKC_Day: TMenuItem
        Tag = 20200
        Caption = '&Day'
        OnClick = PM_StatisticKeyClick
      end
      object PMSKC_Week: TMenuItem
        Tag = 20300
        Caption = '&Week'
        OnClick = PM_StatisticKeyClick
      end
      object PMSKC_Hour: TMenuItem
        Tag = 20400
        Caption = '&Hour'
        OnClick = PM_StatisticKeyClick
      end
    end
    object PMSK_EditedDate: TMenuItem
      Caption = '&Edited Date'
      object PMSKE_Year: TMenuItem
        Tag = 30000
        Caption = '&Year'
        OnClick = PM_StatisticKeyClick
      end
      object PMSKE_Month: TMenuItem
        Tag = 30100
        Caption = '&Month'
        OnClick = PM_StatisticKeyClick
      end
      object PMSKE_Day: TMenuItem
        Tag = 30200
        Caption = '&Day'
        OnClick = PM_StatisticKeyClick
      end
      object PMSKE_Week: TMenuItem
        Tag = 30300
        Caption = '&Week'
        OnClick = PM_StatisticKeyClick
      end
      object PMSKE_Hour: TMenuItem
        Tag = 30400
        Caption = '&Hour'
        OnClick = PM_StatisticKeyClick
      end
    end
    object PMSK_ViewedDate: TMenuItem
      Caption = '&Viewed Date'
      object PMSKV_Year: TMenuItem
        Tag = 40000
        Caption = '&Year'
        OnClick = PM_StatisticKeyClick
      end
      object PMSKV_Month: TMenuItem
        Tag = 40100
        Caption = '&Month'
        OnClick = PM_StatisticKeyClick
      end
      object PMSKV_Day: TMenuItem
        Tag = 40200
        Caption = '&Day'
        OnClick = PM_StatisticKeyClick
      end
      object PMSKV_Week: TMenuItem
        Tag = 40300
        Caption = '&Week'
        OnClick = PM_StatisticKeyClick
      end
      object PMSKV_Hour: TMenuItem
        Tag = 40400
        Caption = '&Hour'
        OnClick = PM_StatisticKeyClick
      end
    end
  end
  object XPMan: TXPManifest
    Left = 560
    Top = 328
  end
  object CD_Color: TColorDialog
    Left = 392
    Top = 128
  end
  object PM_Drawing: TPopupMenu
    Left = 600
    Top = 224
    object PD_Undo: TMenuItem
      Caption = '&Undo'
      OnClick = ME_UndoClick
    end
    object PD_Redo: TMenuItem
      Caption = '&Redo'
      OnClick = ME_RedoClick
    end
    object MenuItem4: TMenuItem
      Caption = '-'
    end
    object PD_Cut: TMenuItem
      Caption = 'Cu&t'
      OnClick = ME_CutClick
    end
    object PD_Copy: TMenuItem
      Caption = '&Copy'
      OnClick = ME_CopyClick
    end
    object PD_Paste: TMenuItem
      Caption = '&Paste'
      OnClick = ME_PasteClick
    end
    object PD_Delete: TMenuItem
      Caption = '&Delete'
      OnClick = ME_DeleteClick
    end
    object PD_SelectAll: TMenuItem
      Caption = 'Select &All'
      OnClick = ME_SelectAllClick
    end
  end
  object SD_CSV: TSaveDialog
    DefaultExt = 'csv'
    FileName = '*.csv'
    Filter = '*.csv|*.csv|*.*|*.*'
    Options = [ofOverwritePrompt, ofHideReadOnly, ofEnableSizing]
    Left = 416
    Top = 224
  end
  object OD_CSV: TOpenDialog
    FileName = '*.csv'
    Filter = '*.csv|*.csv'
    Left = 416
    Top = 192
  end
end
