//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit6.h"
#include "Unit1.h"

#define  CLIENT_MAX 4 //�w�q�s�u�H��
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm6 *Form6;

//�������������������������������������������������������������������������������������������������������������������������������������������������������������u�ܼơv
String strHost;
//bool blnServer;  			//��server or client

int ssd = 0;//�üƺؤl��s
String ClientHostName = "";
String ServerName = "";
String newClient = "";

//String* CHost[4];
int user_num = 0;
String user[4];

player player_[4];




bool card[4][13];
String poke_hand[4][5][2];
//--
bool Game = false;
int total_num = 0;
int who_turn=0;//�����
bool isClockwise = true;//T = ��, F = �f

//�������ݭn���m���ܼ�
int card_num = 0;//���J�P[�b�~]���ƶq
int h=0;

//�������������������������������������������������������������������������������������������������������������������������������������������������������������uload�v
__fastcall TForm6::TForm6(TComponent* Owner) //��l��
	: TForm(Owner)
{
	ListBox1->Clear();
	Memo3->Lines->Clear();
	ComboBox2->Clear();
	//ComboBox2->Items->Add("POKER127.0.0.1#11$0000@");
	ComboBox2->Items->Add("POKER127.0.0.1#12$0000@");
	ComboBox2->Items->Add("POKER127.0.0.1#21$0000@");
	ComboBox2->Items->Add("POKER127.0.0.1#22$0000@");

	try
	{
		ServerSocket1->Active = true;//�}�� Server
	}
	catch(...)
	{
		//ShowMessage("Be a server failure.");
		Delay(500);
		Form6->Close();//�۰�����
		return;
	}
}
void __fastcall TForm6::FormCreate(TObject *Sender)
{


	Form6 -> Caption = "Local host:"+IdIPWatch1->LocalIP();

	

}
void __fastcall TForm6::FormClose(TObject *Sender, TCloseAction &Action)
{;
	Send(true,"Server",1,2,0,0,0);
}

//�������������������������������������������������������������������������������������������������������������������������������������������������������������uServer&Client�v
//�����������������������������������������������������������������������������uServer�v
void __fastcall TForm6::ServerSocket1ClientConnect(TObject *Sender, TCustomWinSocket *Socket)
{;
	PACKET pkg;
	String strPkg;

	//ShowMessage("S:Client�s�u���\");//------------------------------------------------------------------------------
	user_num = ServerSocket1->Socket->ActiveConnections;//1234



	if(user_num > CLIENT_MAX||Game)
	{
	  Form2->Memo1->Lines->Add("�w�ڵ��s�s�u");
	  actSend(false,0,"",1,2,0,0,0);//�_�u�q��
	  Socket->Close();
	  return;
	}



	String H = Socket->RemoteAddress + "-" + IntToStr(Socket->SocketHandle);
	Log("�h�h�h�h�h�s���a" + H);

	user[ServerSocket1->Socket->ActiveConnections-1] = H;
	for (int i = ServerSocket1->Socket->ActiveConnections; i < 4; i++)
	{
        user[i] = NULL;
	}
	reflash_client();

	actSend(false,0,H,1,1,user_num,0,0);
	Log("11:����Host");
	Status();
}
void __fastcall TForm6::ServerSocket1Accept(TObject *Sender, TCustomWinSocket *Socket)
{


	//ShowMessage("1");//------------------------------------------------------------------------------
	;
}

void __fastcall TForm6::ServerSocket1ClientDisconnect(TObject *Sender, TCustomWinSocket *Socket)
{;
	Form2->Memo1 -> Lines -> Add("'" + Socket->RemoteAddress + "-" + IntToStr(Socket->SocketHandle) + "'disconnection comfirmed");
	//StatusBar1 -> SimpleText = "Status: Listening...";
	//Game = false;
	Delay(100);
	user_num = ServerSocket1->Socket->ActiveConnections-1;
	if( ServerSocket1 -> Socket -> ActiveConnections == 1 )
	{
	   Game = false;

	   TN->Caption = "";
	}
	reflash_client();
	Status();

}
void __fastcall TForm6::ServerSocket1ClientError(TObject *Sender, TCustomWinSocket *Socket,
		  TErrorEvent ErrorEvent, int &ErrorCode)
{
	user_num = ServerSocket1->Socket->ActiveConnections;
	Status();
	//StatusBar1->SimpleText = "Error from Client socket.";
	;
}
void __fastcall TForm6::ServerSocket1ClientRead(TObject *Sender, TCustomWinSocket *Socket)
{
	pPACKET pPkg;
	String strPacket;
	String A = Time();

	strPacket = Socket->ReceiveText();
	//ShowMessage("S����:"+strPacket);


	try
	{

		String many = strPacket;
		int the_way = 0;
		while (many.Length()!=0)
		{
			String now_it;
			the_way =  many.Pos("@");

			Memo3->Lines->Add(strPacket);

			now_it = many.SubString(0,the_way);

			pPkg=DisassemblePacket(now_it);

			ParsePacket(pPkg);

			many = StringReplace(many,now_it, "", TReplaceFlags());
		}




		delete pPkg;

	}catch(...)
	{
		//ShowMessage("Some ���~");


	}



}
//�������������������������������������������������������������������������������������������������������������������������������������������������������������uPacket�v
//�����������������������������������իʥ]
String TForm6::AssemblePacket(PACKET pkg)
{
	String str="",tmp;
   //���Y
	if(pkg.ID != "POKER")//�p���O���G�A�h�O�u�S����O�v�����^��
	{
		 str = pkg.ID;
		 return str;
	}

	str += pkg.ID;
	str+=pkg.Host;
	str += "#";

	str += pkg.MainFunc;
	str += pkg.SubFunc;
	str +=  "$";

	str += pkg.Poke_color;
	str += pkg.Poke;
	str += pkg.PokeFunc;
	str +=  "@";

   return str;
}
//������������������������������������ʥ]
pPACKET TForm6::DisassemblePacket(String strPkg)
{
	PACKET tmpPkg;
	pPACKET pPkg;
	int separator;
	String strTmp;

	//--------------------------------------------------------
	try
	{
		separator = strPkg.Pos("#");
		tmpPkg.Host = strPkg.SubString(6,separator-6);

        //ShowMessage(tmpPkg.Host);
		tmpPkg.MainFunc = strPkg.SubString(separator+1,1);//1
		tmpPkg.SubFunc = strPkg.SubString(separator+2,1);//2


		separator 			= strPkg.Pos("$");
		tmpPkg.Poke_color 	= strPkg.SubString(separator+1,1);//1
		tmpPkg.Poke 		= strPkg.SubString(separator+2,2);//23
		tmpPkg.PokeFunc 	= strPkg.SubString(separator+4,1);//4


	}
	catch(...)
	{

	}

//--------------------------------------------------------
   //�ŧi�s���O����
   pPkg=new PACKET;//���Ы��A���n�Ρu->�v���O�u=�v

   pPkg->ID 		= "POKER";
   pPkg->Host 		= tmpPkg.Host;

   pPkg->MainFunc 	= tmpPkg.MainFunc;
   pPkg->SubFunc 	= tmpPkg.SubFunc;

   pPkg->Poke_color = tmpPkg.Poke_color;
   pPkg->Poke 		= tmpPkg.Poke;
   pPkg->PokeFunc 	= tmpPkg.PokeFunc;

   return pPkg;//�^�ǡA����A�ۦ�P�_�O�_�n�R���O����
}
//�����������������������������������ѪR�ʥ]������������������������������������������������������������������������������������������������������������������������������������������������������
void TForm6::ParsePacket(pPACKET pPkg)
{
	int mFunc, sFunc, COLOR, POKE,POKEFunc;
	String HOST;


	HOST 		= pPkg->Host;
	mFunc 		= StrToInt(pPkg->MainFunc);
	sFunc 		= StrToInt(pPkg->SubFunc);
	COLOR 		= StrToInt(pPkg->Poke_color);
	POKE 		= StrToInt(pPkg->Poke);
	POKEFunc 	= StrToInt(pPkg->PokeFunc);

	h = find_user(HOST);//�P�_�o�O�X�����a
	//ShowMessage(pPkg->MainFunc+pPkg->SubFunc);
	Log("�h�h�h�h�hParsePacket");
	Log("M:"+IntToStr(mFunc)+",S:"+IntToStr(sFunc));
	Log("{");
	switch(mFunc)
	{
		case 1: //Link status (Connect or Disconnect)

			switch(sFunc)
			{
				case 1: //1,1�G ���a�s�u
					//Status = "new connection";
					break;

				case 2: //1,2�G ���a�_�u = �@���_�u�A���s�s������
					int ii = h;
					user[h] = NULL;



					Log("���a�_�u:"+HOST);
					for (int i = ii; i < 4-1; i++)//�N�S��host��user�ը�̫�
					{
						String sw = user[i];
						user[i] = user[i+1];
						user[i+1] = sw;
					}

					if(Game)//�p�G�b�C�����A�궶��
					{
						//Delay(100);
						player_[h].boom = true;
						if(who_turn == h)//�٭�n�O��e�X�P���a�A�N���U�@��
						{
							Log("�o�{����e�X�P���a");
							send_to_next();
							Log("���ǻ���");
						}
						Delay(100);
						make_trun();//�궶��

                        for (int i = ii; i < 4-1; i++)//�N�S��host��boom�ը�̫�
						{
							bool sw = player_[i].boom;
							player_[i].boom = player_[i+1].boom;
							player_[i+1].boom = sw;
						}



						if(user_num==1 && bow())
						{
                            Log("�o�{���Ȧs���a");
							actSend(true,0,user[0],2,3,user_num,total_num,0);
							Game=!Game;
							//return;
						}

					}
					Delay(100);
					for (int i = 0; i < user_num; i++)
					{
						actSend(true,i,user[i],2,0,i,user_num,0);
						Log("�i����"+IntToStr(i+1)+"��G"+IntToStr(i)+IntToStr(user_num));
					}
					Delay(100);

					reflash_client();
					Log("���a�_�u����");
					break;

            }
			break;

		case 2: //in Game
			switch(sFunc)
			{
				case 1: //2,1�C���}�l �~�P �@�}�l�C�H�o5�i
					//Status = "Normal Packet";
					//ShowMessage("");
					if(user_num<=1)//�H�Ƥ@�w�n�j��G
					{Log("}");return;}


					re_start();
					make_trun();
					Log("�o�P��...");
					for (int i = 0; i < ServerSocket1->Socket->ActiveConnections; i++)
					{
						for (int j = 0; j < 5; j++)
						{
							deal_card(true,i,user[i]);

							Delay(100);
						}
					}
					Delay(100);
					actSend(true,0,"",2,2,user_num,total_num,0);
					Log("�_�l�o�P����");
					break;

				case 2: //2,2�G�������a���P �A�Ǥ@�i�P���L��(���a�X�P+��P)
					Log("���� P"+IntToStr(h+1)+" ���P");

					Log("���P�_�d�P�ĪG");
					Game_Rule(HOST ,COLOR, POKE, POKEFunc);	//�N���a���P�i�J�C���P�_�A�M�᤽�i���Ҧ��H
					Log("���d�P�P�_����");

					Log("���P�_�O�_�~�P");
					Shuffle();
					Log("���P�_����");								//�ˬd�O�_�n�~�P

					Log("���ǳ�21");
					Draw(HOST); 							//���a�X���P�n��@�i���L��:21
					Log("��21:�ٵP");

					Delay(100);

					//��W�H���U�@�ӤH
					Log("���ǳ�22");
					send_to_next();                         //22
					Log("��22:Next");

					Delay(100);

					//����s���Ʀr
					Log("24:�s���{�p");//��LOG�A���Msend�ӧ֡AC�^�Ǥ���A�o��|�]��
					Send(true,"-Server",2,4,who_turn,total_num,0);


					break;

				case 3:
					player_[h].boom = true;

					if(bow()&&Game)
					{
						for (int i = 0; i < user_num; i++)
						{
							if (!player_[i].boom)
							{
								Log("�P�_�ӭt");
								Send(true,user[i],2,3,user_num,total_num,0);
								Game = false;
								Log("�C������");
                                return;
							}
						}

					}
					else
					{
						send_to_next();
						Log("22:Next");
					}



					break;


            }
			break;

		default:
			break;

	}

	Log("}");
    Delay(100);
    Status();
}
//�����������������������������������e�X�ʥ]*******************************************************************
void TForm6::Send(bool SW,String Host,int MainFunc = 0,int SubFunc = 0,
					int Poke_color = 0,int Poke = 0,int PokeFunc = 0)//SW = true  �s��
{
	if(!SW)
	{
		for (int i = 0; i < ServerSocket1->Socket->ActiveConnections; i++)
		{
			ServerSocket1->Socket->Connections[i]->SendText(Host);
		}
		return;
	}

	String strPkg ="";

	strPkg += "POKER";
	strPkg += Host;
	strPkg += "#";

	strPkg += MainFunc;
	strPkg += SubFunc;
	strPkg +=  "$";

	strPkg += Poke_color;


	strPkg += StringOfChar('0',2 - IntToStr(Poke).Length()  ) + Poke;
	//strPkg += PokeFunc;
	strPkg +=  "@";

	for (int i = 0; i < ServerSocket1->Socket->ActiveConnections; i++)
	{
		ServerSocket1->Socket->Connections[i]->SendText(strPkg);
	}
}
void TForm6::actSend(bool SW,int p,String Host,int MainFunc,int SubFunc,
						int Poke_color = 0,int Poke = 0,int PokeFunc = 0)
{


	String strPkg ="";

	strPkg += "POKER";
	strPkg += Host;
	strPkg += "#";

	strPkg += MainFunc;
	strPkg += SubFunc;
	strPkg +=  "$";

	strPkg += Poke_color;


	strPkg += StringOfChar('0',2 - IntToStr(Poke).Length() ) + Poke;
	//strPkg += PokeFunc;
	strPkg +=  "@";


	if(!SW)	//false = �ǵ��̷s�s�u��
	{
		ServerSocket1->Socket->Connections[ServerSocket1->Socket->ActiveConnections-1]->SendText(strPkg);
	}
	else    //���w�ǰe
	{
		ServerSocket1->Socket->Connections[p]->SendText(strPkg);
	}

}
//�������������������������������������������������������������������������������������������������������������������������������������������������������������uButton_click�v
//�������������������������������������s�ƥ�


//�������������������������������������������������������������������������������������������������������������������������������������������������������������uelse�v
//�����������������������������������~�P
void TForm6::Shuffle()
{
	//�ˬdServer�O�_�S�P
	if(card_num!=25)//52
	{
		Log("�����i�~�P");
		return;
	}
	Log("���i�~�P");

	//bool all = false;

	Log("�}�l���m");

	//�~�P = �����^�� + �ھڪ��a��P ���P�v���ٵ�����
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 10; j++)
		{
			card[i][j] = true;
		}
	}
	Log("���m�����A�}�l����");
	card_num = 0;

	for (int i = 0; i < user_num ; i++)
	{
		for (int j = 0; j < 5; j++)
		{
			if(!player_[i].boom)//�S�z���~�ݭn��
			{
				Log("���ٲ�"+IntToStr(i)+"��");
				card[StrToInt(poke_hand[i][j][0])-1][StrToInt(poke_hand[i][j][1])-1] = false;
                card_num++;
			}

		}
	}
   Log("���ٵ���");
}
//�����������������������������������궶��
void TForm6::make_trun()
{;

	Log("usernum ��"+IntToStr(user_num));
	for (int i = 0; i < user_num; i++)// 3 = 012
	{
		player_[i].next = i+1;
		player_[i].back = i-1;

	}
	player_[user_num-1].next = 0;
	player_[0].back = user_num-1;


}
//�����������������������������������U�@��
void  TForm6:: re_start()
{
	Game = true;
	isClockwise = true;

    who_turn = 0;

	card_num = 0;
	total_num = 0;
    TN ->Visible = true;
	TN -> Caption = total_num;
	for (int i = 0; i < 4; i++)
	{
        player_[i].boom = false;
		for (int j = 0; j < 13; j++)
		{

			card[i][j] = true;
		}

	}

	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 5; j++)
			for (int o = 0; o < 2; o++)
                poke_hand[i][j][o] = NULL;
}

void TForm6::deal_card(bool SW,int i,String HOST = "")
{
	srand((unsigned)time(NULL)+ssd);
	ssd++;

	int p1,p2;
	p1 = rand() % 4;
	p2 = rand() % 13;
	while (!card[p1][p2])
	{
		p1 = rand() % 4 ;
		p2 = rand() % 13 ;
	}
	//ShowMessage("");

	card[p1][p2] = false;

	card_num++;

	//T = ���o�AF = ��o
	int a = find_user(HOST);

	for (int o=0; o < 5; o++)
	{
		if(poke_hand[a][o][0]==NULL)
		{
			poke_hand[a][o][0] = IntToStr(p1);
			poke_hand[a][o][1] = IntToStr(p2);

			break;
		}
	}

	actSend(SW,a,HOST,2,1,p1+1,p2+1,0);
}
void TForm6::Game_Rule(String HOST ,int COLOR ,int POKE ,int POKEFunc)
{
	h = find_user(HOST);
	switch (POKE)
	{
		case 1://�k�s
			if(COLOR==4)
			{
				total_num = 0;//�®�A = �k�s;
			}else
			{
				total_num += POKE;
			}
		break;

		case 4://�j��
            isClockwise = !isClockwise;

		break;

		case 5://���w
			for (int i = 0; i < POKEFunc; i++)
			{
				//ShowMessage("�U���@��");
				if(isClockwise)
				{
					h = player_[h].next;
				}
				else
				{
					h = player_[h].back;
				}
			}

		break;

		case 10://+10 or -10
			if(POKEFunc==0)
			{
				total_num += POKE;
			}
			else
			{
				total_num -= POKE;
			}
		break;

		case 11://pass


		break;

		case 12://+20 or -20
			if(POKEFunc==0)
			{
				total_num += 20;
			}
			else
			{
				total_num -= 20;
			}

		break;

		case 13://99
			total_num = 99;
		break;

		default:// else + num
			total_num += POKE;
		break;
	}

	//�p�G�W�X99�A�N��^�h
	if(total_num>99)
	{
		switch (POKE)
		{
			case 10:
				total_num -= 10;
			break;

			case 12:
				total_num -= 20;
			break;

			default:
			total_num -= POKE;
			break;
		}
		player_[h].boom = true;
		actSend(true,player_[h].next,"",2,3,user_num,total_num,0);
	}//�p�G�֩�0�A�N����0
	else if(total_num<0)
	{
		total_num = 0;
	}


	TN -> Caption = total_num;
}
void TForm6::Draw(String HOST)
{
	int num = find_user(HOST);
	deal_card(true ,num,HOST);

}
//�����������������������������������䶶��
int TForm6::find_user(String HOST)
{
	for (int i = 0; i < 4; i++)
	{
		if(user[i] == HOST)
		{
			return i;
		}
	}
	return -1;
}
//�����������������������������������z
bool TForm6::bow()
{
	int count = 0;
	for (int i = 0; i < user_num; i++)//�ˬd���X�ӨS�z
	{
		if(!player_[i].boom)
			count++;
	}

	if(count==1)
	{
		return true;
	}else
		return false;//�٦��H�b
}
//�����������������������������������ھ�[��/�f]�ɰw�A�e�ʥ]���U�@��
void TForm6::send_to_next()
{
    if(isClockwise)
	{
		actSend(true,player_[h].next,"",2,2,user_num,total_num,0);
		who_turn = player_[h].next;
	}
	else
	{
		actSend(true,player_[h].back,"",2,2,user_num,total_num,0);
		who_turn = player_[h].back;
	}
}
//�����������������������������������������������������������������������������������������������������������������������������������������������������������������������u��{��
//���������������������������������� find
bool TForm6::find(String s, String finds)//�q s �� finds
{
    String tmp; //�Ȧs�^�����r��
	int Start = 0; //���� Start ���r�}�l��
	bool isFind = false;//�O�_�������r��

    Start = s.Pos(finds.SubString(1,1));
    //��X�n�䪺�r�ꪺ�Ĥ@�Ӧr����m

    tmp = s.SubString(Start, finds.Length());
    //����n�䪺�Ʀr�̼˪��� �N�P�˪��ת��r���x�s

    if(tmp == finds)
		isFind = true;

    return isFind;
}
//���������������������������������� Delay
void TForm6::Delay(int iMilliSeconds)
{
  int iStart;

  iStart = GetTickCount();
  while (GetTickCount() - iStart <= iMilliSeconds)
  {
    Sleep(1);
	Application->ProcessMessages();
   }
}
//������������������������������������s���a[�W�u]�M��
void TForm6::reflash_client()
{
	ListBox1->Clear();
	for (int i = 0; i < 4; i++)
	{
		if(user[i]!=NULL)
			ListBox1->Items->Add(user[i]);
	}

}

//����������������������������������[Log]
void TForm6::Log(String print_)
{
	T_LOG->Items->Add(print_);
	T_LOG->ItemIndex = T_LOG->Items->Count-1;
}
//����������������������������������[All_status]
void TForm6::Status()
{
	T_All_Area -> Clear();
	//T_LOG->Items->Add("");
	//IntToStr()
    if(Game)
		T_All_Area->Items->Add("�C�����A�G�C������");
	else
		T_All_Area->Items->Add("�C�����A�G���}�l��");

	T_All_Area->Items->Add("�ثe�`��:"+IntToStr(total_num));
	T_All_Area->Items->Add("����P"+IntToStr(who_turn+1)+"("+player_[who_turn].host+")");

	if(isClockwise)
		T_All_Area->Items->Add("���ǡG��");
	else
		T_All_Area->Items->Add("���ǡG�f");


	T_All_Area->Items->Add("���a�H��:"+IntToStr(user_num));

	T_All_Area->Items->Add("�{�b���a�G");
	T_All_Area->Items->Add("{");
	for (int i = 0; i < user_num; i++)
	{
		T_All_Area->Items->Add("�@P"+IntToStr(i+1)+":"+player_[i].host);

		if(player_[i].boom)
			T_All_Area->Items->Add("�@Boom:true");
		else
			T_All_Area->Items->Add("�@Boom:false");

		T_All_Area->Items->Add("�@Next:P"+IntToStr(player_[i].next+1));
		T_All_Area->Items->Add("�@Back:P"+IntToStr(player_[i].back+1));
	}
	T_All_Area->Items->Add("}");
	T_All_Area->Items->Add("Seed�G"+IntToStr(ssd));
	T_All_Area->Items->Add("card_num�G"+IntToStr(card_num));


}


//---------------------------------------------------------------------------

