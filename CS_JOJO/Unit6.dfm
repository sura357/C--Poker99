object Form6: TForm6
  Left = 0
  Top = 0
  BorderStyle = bsSingle
  Caption = 'Form6'
  ClientHeight = 423
  ClientWidth = 566
  Color = clBtnFace
  Font.Charset = MAC_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  OnClose = FormClose
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 14
  object Label2: TLabel
    Left = 8
    Top = 8
    Width = 5
    Height = 22
    Font.Charset = MAC_CHARSET
    Font.Color = clWindowText
    Font.Height = -19
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
  end
  object Label3: TLabel
    Left = 8
    Top = 8
    Width = 94
    Height = 39
    Caption = 'Server'
    Font.Charset = MAC_CHARSET
    Font.Color = clWindowText
    Font.Height = -32
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
  end
  object TN: TLabel
    Left = 8
    Top = 53
    Width = 16
    Height = 81
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -64
    Font.Name = #24494#36575#27491#40657#39636
    Font.Style = [fsBold]
    ParentFont = False
    Visible = False
  end
  object Label4: TLabel
    Left = 135
    Top = 116
    Width = 35
    Height = 14
    Caption = 'T_LOG'
  end
  object Label5: TLabel
    Left = 328
    Top = 116
    Width = 88
    Height = 14
    Caption = #35722#25976#65288'T_All_Area'
  end
  object GroupBox3: TGroupBox
    Left = 277
    Top = -43
    Width = 306
    Height = 153
    Caption = #28204#35430#29992
    TabOrder = 0
    object Memo3: TMemo
      Left = 3
      Top = 49
      Width = 282
      Height = 97
      ScrollBars = ssBoth
      TabOrder = 0
    end
    object ComboBox2: TComboBox
      Left = 11
      Top = 21
      Width = 201
      Height = 22
      TabOrder = 1
      Text = 'ComboBox2'
    end
  end
  object ListBox1: TListBox
    Left = 135
    Top = 8
    Width = 120
    Height = 95
    ItemHeight = 14
    TabOrder = 1
  end
  object T_LOG: TListBox
    Left = 135
    Top = 136
    Width = 187
    Height = 282
    ItemHeight = 14
    TabOrder = 2
  end
  object T_All_Area: TListBox
    Left = 328
    Top = 136
    Width = 233
    Height = 282
    ItemHeight = 14
    TabOrder = 3
  end
  object ServerSocket1: TServerSocket
    Active = False
    Port = 1024
    ServerType = stNonBlocking
    OnAccept = ServerSocket1Accept
    OnClientConnect = ServerSocket1ClientConnect
    OnClientDisconnect = ServerSocket1ClientDisconnect
    OnClientRead = ServerSocket1ClientRead
    OnClientError = ServerSocket1ClientError
    Left = 32
    Top = 48
  end
  object IdIPWatch1: TIdIPWatch
    Active = False
    HistoryFilename = 'iphist.dat'
    Left = 104
    Top = 48
  end
end
