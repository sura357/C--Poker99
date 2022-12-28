//---------------------------------------------------------------------------

#ifndef Unit6H
#define Unit6H
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.Menus.hpp>
#include <Vcl.ComCtrls.hpp>
#include <System.Win.ScktComp.hpp>
#include <IdBaseComponent.hpp>
#include <IdComponent.hpp>
#include <IdIPWatch.hpp>
#include <Vcl.ExtCtrls.hpp>
//--------------------------------------
/*
Server 封包

Id ：POKER
Host：自身host
Separation ：#
Main ：待定
Sub ：待定
Separation2 ：$
Poke_color：卡牌花色  1byte
(1::clubs:，2::diamonds:，3::hearts:，4::spades:）
Poke ：卡牌數字(2byte)1-13
PokeFunc ：數字功能(1byte)
如果數字為5 =>case1 , 數字為10 or Q =>case2
case1 : PokeFunc = ( 0 代表指定下一位 ,  1代表指定下兩位 , 2代表指定下三位 )
case2: PokeFunc = ( 0 = add  ,  1 = sub  )
Endosymbol ：@

POKER127.0.0.1#21$1052@

Main ：1; Sub ：1 新玩家連線
Main ：1; Sub ：2 玩家斷線

Main ：2; Sub ：1 遊戲開始 洗牌 一開始每人發5張
Main ：2; Sub ：2 接受玩家的牌 再傳一張牌給他抽(玩家出牌+抽牌)
*/
//--------------------------------------
// 「POKER127.0.0.1#21$1052@」
typedef struct _PACKET
{
  String ID;  			//POKER
  String Host; 			//172.0.0.1

  String s1;  			// "#"

  String MainFunc;  	//
  String SubFunc;  		//

  String s2; 			// "$"

  String Poke_color; 	//長度1：
  String Poke;  		//長度2：
  String PokeFunc;      //

  String s3; 			// "@"
}PACKET, *pPACKET;		//普通,指標

typedef struct _CLIENT_SOCKET
{
  bool active;
  TCustomWinSocket *pSocket;
  String ClientID;
}CLIENTSOCKET, *pCLIENTSOCKET;//普通,指標

//---------------------玩家順序
typedef struct _player
{
    bool boom;//false=沒爆 ,true= 爆
	int back;//前一個
	int next;//後一個
	String host;

	
}player,*pplayer;

//---------------------------------------------------------------------------
class TForm6 : public TForm
{
__published:	// IDE-managed Components
	TServerSocket *ServerSocket1;
	TIdIPWatch *IdIPWatch1;
	TLabel *Label2;
	TLabel *Label3;
	TGroupBox *GroupBox3;
	TMemo *Memo3;
	TComboBox *ComboBox2;

	TLabel *TN;
	TListBox *ListBox1;
	TListBox *T_LOG;
	TListBox *T_All_Area;
	TLabel *Label4;
	TLabel *Label5;
	void __fastcall FormCreate(TObject *Sender);

	void __fastcall ServerSocket1ClientConnect(TObject *Sender, TCustomWinSocket *Socket);
	void __fastcall ServerSocket1Accept(TObject *Sender, TCustomWinSocket *Socket);
	void __fastcall ServerSocket1ClientDisconnect(TObject *Sender, TCustomWinSocket *Socket);
	void __fastcall ServerSocket1ClientError(TObject *Sender, TCustomWinSocket *Socket,
          TErrorEvent ErrorEvent, int &ErrorCode);
	void __fastcall ServerSocket1ClientRead(TObject *Sender, TCustomWinSocket *Socket);

	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);





private:	// User declarations((((((代表「不想被別人使用」
public:		// User declarations((((((代表「想被別人使用」
	__fastcall TForm6(TComponent* Owner);

	String AssemblePacket(PACKET );
	pPACKET DisassemblePacket(String );
	//void ShowPacket(PACKET);
	//void ShowPacket(pPACKET);
	void ParsePacket(pPACKET);

	bool find(String,String);
	void Send(bool,String ,int ,int ,int,int ,int );
    void actSend(bool,int,String ,int ,int ,int,int ,int );
	void Delay(int);
	void deal_card( bool,int,String);
	void re_start();
	void Game_Rule(String ,int ,int ,int );
	void Draw(String);
	int find_user(String);
	bool bow();
	void Shuffle();
	void make_trun();
	void reflash_client();
	void send_to_next();
	void Log(String);
	void Status();
};
//---------------------------------------------------------------------------
extern PACKAGE TForm6 *Form6;
//---------------------------------------------------------------------------
#endif
