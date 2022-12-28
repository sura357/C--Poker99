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
Server �ʥ]

Id �GPOKER
Host�G�ۨ�host
Separation �G#
Main �G�ݩw
Sub �G�ݩw
Separation2 �G$
Poke_color�G�d�P���  1byte
(1::clubs:�A2::diamonds:�A3::hearts:�A4::spades:�^
Poke �G�d�P�Ʀr(2byte)1-13
PokeFunc �G�Ʀr�\��(1byte)
�p�G�Ʀr��5 =>case1 , �Ʀr��10 or Q =>case2
case1 : PokeFunc = ( 0 �N�����w�U�@�� ,  1�N�����w�U��� , 2�N�����w�U�T�� )
case2: PokeFunc = ( 0 = add  ,  1 = sub  )
Endosymbol �G@

POKER127.0.0.1#21$1052@

Main �G1; Sub �G1 �s���a�s�u
Main �G1; Sub �G2 ���a�_�u

Main �G2; Sub �G1 �C���}�l �~�P �@�}�l�C�H�o5�i
Main �G2; Sub �G2 �������a���P �A�Ǥ@�i�P���L��(���a�X�P+��P)
*/
//--------------------------------------
// �uPOKER127.0.0.1#21$1052@�v
typedef struct _PACKET
{
  String ID;  			//POKER
  String Host; 			//172.0.0.1

  String s1;  			// "#"

  String MainFunc;  	//
  String SubFunc;  		//

  String s2; 			// "$"

  String Poke_color; 	//����1�G
  String Poke;  		//����2�G
  String PokeFunc;      //

  String s3; 			// "@"
}PACKET, *pPACKET;		//���q,����

typedef struct _CLIENT_SOCKET
{
  bool active;
  TCustomWinSocket *pSocket;
  String ClientID;
}CLIENTSOCKET, *pCLIENTSOCKET;//���q,����

//---------------------���a����
typedef struct _player
{
    bool boom;//false=�S�z ,true= �z
	int back;//�e�@��
	int next;//��@��
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





private:	// User declarations((((((�N���u���Q�Q�O�H�ϥΡv
public:		// User declarations((((((�N���u�Q�Q�O�H�ϥΡv
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