//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit6.h"
#include "Unit1.h"

#define  CLIENT_MAX 4 //定義連線人數
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm6 *Form6;

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■「變數」
String strHost;
//bool blnServer;  			//當成server or client

int ssd = 0;//亂數種子更新
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
int who_turn=0;//輪到誰
bool isClockwise = true;//T = 順, F = 逆

//■■■需要重置的變數
int card_num = 0;//撲克牌[在外]的數量
int h=0;

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■「load」
__fastcall TForm6::TForm6(TComponent* Owner) //初始化
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
		ServerSocket1->Active = true;//開啟 Server
	}
	catch(...)
	{
		//ShowMessage("Be a server failure.");
		Delay(500);
		Form6->Close();//自動關閉
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

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■「Server&Client」
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■「Server」
void __fastcall TForm6::ServerSocket1ClientConnect(TObject *Sender, TCustomWinSocket *Socket)
{;
	PACKET pkg;
	String strPkg;

	//ShowMessage("S:Client連線成功");//------------------------------------------------------------------------------
	user_num = ServerSocket1->Socket->ActiveConnections;//1234



	if(user_num > CLIENT_MAX||Game)
	{
	  Form2->Memo1->Lines->Add("已拒絕新連線");
	  actSend(false,0,"",1,2,0,0,0);//斷線通知
	  Socket->Close();
	  return;
	}



	String H = Socket->RemoteAddress + "-" + IntToStr(Socket->SocketHandle);
	Log("▇▇▇▇▇新玩家" + H);

	user[ServerSocket1->Socket->ActiveConnections-1] = H;
	for (int i = ServerSocket1->Socket->ActiveConnections; i < 4; i++)
	{
        user[i] = NULL;
	}
	reflash_client();

	actSend(false,0,H,1,1,user_num,0,0);
	Log("11:給予Host");
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
	//ShowMessage("S收到:"+strPacket);


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
		//ShowMessage("Some 錯誤");


	}



}
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■「Packet」
//■■■■■■■■■■■■■■■■■組封包
String TForm6::AssemblePacket(PACKET pkg)
{
	String str="",tmp;
   //檔頭
	if(pkg.ID != "POKER")//如不是賓果，則是「特殊指令」直接回傳
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
//■■■■■■■■■■■■■■■■■拆封包
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
   //宣告新的記憶體
   pPkg=new PACKET;//指標型態的要用「->」不是「=」

   pPkg->ID 		= "POKER";
   pPkg->Host 		= tmpPkg.Host;

   pPkg->MainFunc 	= tmpPkg.MainFunc;
   pPkg->SubFunc 	= tmpPkg.SubFunc;

   pPkg->Poke_color = tmpPkg.Poke_color;
   pPkg->Poke 		= tmpPkg.Poke;
   pPkg->PokeFunc 	= tmpPkg.PokeFunc;

   return pPkg;//回傳，之後再自行判斷是否要刪掉記憶體
}
//■■■■■■■■■■■■■■■■■解析封包■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
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

	h = find_user(HOST);//判斷這是幾號玩家
	//ShowMessage(pPkg->MainFunc+pPkg->SubFunc);
	Log("▇▇▇▇▇ParsePacket");
	Log("M:"+IntToStr(mFunc)+",S:"+IntToStr(sFunc));
	Log("{");
	switch(mFunc)
	{
		case 1: //Link status (Connect or Disconnect)

			switch(sFunc)
			{
				case 1: //1,1： 玩家連線
					//Status = "new connection";
					break;

				case 2: //1,2： 玩家斷線 = 一有斷線，重新連接順序
					int ii = h;
					user[h] = NULL;



					Log("玩家斷線:"+HOST);
					for (int i = ii; i < 4-1; i++)//將沒有host的user調到最後
					{
						String sw = user[i];
						user[i] = user[i+1];
						user[i+1] = sw;
					}

					if(Game)//如果在遊戲中，串順序
					{
						//Delay(100);
						player_[h].boom = true;
						if(who_turn == h)//還剛好是當前出牌玩家，就換下一位
						{
							Log("發現為當前出牌玩家");
							send_to_next();
							Log("順序遞補");
						}
						Delay(100);
						make_trun();//串順序

                        for (int i = ii; i < 4-1; i++)//將沒有host的boom調到最後
						{
							bool sw = player_[i].boom;
							player_[i].boom = player_[i+1].boom;
							player_[i+1].boom = sw;
						}



						if(user_num==1 && bow())
						{
                            Log("發現為僅存玩家");
							actSend(true,0,user[0],2,3,user_num,total_num,0);
							Game=!Game;
							//return;
						}

					}
					Delay(100);
					for (int i = 0; i < user_num; i++)
					{
						actSend(true,i,user[i],2,0,i,user_num,0);
						Log("告知第"+IntToStr(i+1)+"位："+IntToStr(i)+IntToStr(user_num));
					}
					Delay(100);

					reflash_client();
					Log("玩家斷線結束");
					break;

            }
			break;

		case 2: //in Game
			switch(sFunc)
			{
				case 1: //2,1遊戲開始 洗牌 一開始每人發5張
					//Status = "Normal Packet";
					//ShowMessage("");
					if(user_num<=1)//人數一定要大於二
					{Log("}");return;}


					re_start();
					make_trun();
					Log("發牌中...");
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
					Log("起始發牌結束");
					break;

				case 2: //2,2：接受玩家的牌 再傳一張牌給他抽(玩家出牌+抽牌)
					Log("收到 P"+IntToStr(h+1)+" 的牌");

					Log("▼判斷卡牌效果");
					Game_Rule(HOST ,COLOR, POKE, POKEFunc);	//將玩家的牌進入遊戲判斷，然後公告給所有人
					Log("▲卡牌判斷完畢");

					Log("▼判斷是否洗牌");
					Shuffle();
					Log("▲判斷完成");								//檢查是否要洗牌

					Log("▼準備21");
					Draw(HOST); 							//玩家出玩牌要丟一張給他抽:21
					Log("▲21:還牌");

					Delay(100);

					//單獨寄給下一個人
					Log("▼準備22");
					send_to_next();                         //22
					Log("▲22:Next");

					Delay(100);

					//全體廣播數字
					Log("24:廣播現況");//先LOG，不然send太快，C回傳之後，這行會跑調
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
								Log("判斷勝負");
								Send(true,user[i],2,3,user_num,total_num,0);
								Game = false;
								Log("遊戲結束");
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
//■■■■■■■■■■■■■■■■■送出封包*******************************************************************
void TForm6::Send(bool SW,String Host,int MainFunc = 0,int SubFunc = 0,
					int Poke_color = 0,int Poke = 0,int PokeFunc = 0)//SW = true  廣播
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


	if(!SW)	//false = 傳給最新連線的
	{
		ServerSocket1->Socket->Connections[ServerSocket1->Socket->ActiveConnections-1]->SendText(strPkg);
	}
	else    //指定傳送
	{
		ServerSocket1->Socket->Connections[p]->SendText(strPkg);
	}

}
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■「Button_click」
//■■■■■■■■■■■■■■■■■按鈕事件


//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■「else」
//■■■■■■■■■■■■■■■■■洗牌
void TForm6::Shuffle()
{
	//檢查Server是否沒牌
	if(card_num!=25)//52
	{
		Log("□不可洗牌");
		return;
	}
	Log("■可洗牌");

	//bool all = false;

	Log("開始重置");

	//洗牌 = 全部回收 + 根據玩家手牌 把手牌權限還給它們
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 10; j++)
		{
			card[i][j] = true;
		}
	}
	Log("重置結束，開始返還");
	card_num = 0;

	for (int i = 0; i < user_num ; i++)
	{
		for (int j = 0; j < 5; j++)
		{
			if(!player_[i].boom)//沒爆的才需要還
			{
				Log("返還第"+IntToStr(i)+"位");
				card[StrToInt(poke_hand[i][j][0])-1][StrToInt(poke_hand[i][j][1])-1] = false;
                card_num++;
			}

		}
	}
   Log("返還結束");
}
//■■■■■■■■■■■■■■■■■串順序
void TForm6::make_trun()
{;

	Log("usernum 為"+IntToStr(user_num));
	for (int i = 0; i < user_num; i++)// 3 = 012
	{
		player_[i].next = i+1;
		player_[i].back = i-1;

	}
	player_[user_num-1].next = 0;
	player_[0].back = user_num-1;


}
//■■■■■■■■■■■■■■■■■下一場
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

	//T = 全發，F = 單發
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
		case 1://歸零
			if(COLOR==4)
			{
				total_num = 0;//黑桃A = 歸零;
			}else
			{
				total_num += POKE;
			}
		break;

		case 4://迴轉
            isClockwise = !isClockwise;

		break;

		case 5://指定
			for (int i = 0; i < POKEFunc; i++)
			{
				//ShowMessage("下面一位");
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

	//如果超出99，就減回去
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
	}//如果少於0，就等於0
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
//■■■■■■■■■■■■■■■■■找順序
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
//■■■■■■■■■■■■■■■■■爆
bool TForm6::bow()
{
	int count = 0;
	for (int i = 0; i < user_num; i++)//檢查有幾個沒爆
	{
		if(!player_[i].boom)
			count++;
	}

	if(count==1)
	{
		return true;
	}else
		return false;//還有人在
}
//■■■■■■■■■■■■■■■■■根據[順/逆]時針，送封包給下一個
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
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■工具程式
//■■■■■■■■■■■■■■■■■ find
bool TForm6::find(String s, String finds)//從 s 找 finds
{
    String tmp; //暫存擷取的字串
	int Start = 0; //重第 Start 的字開始取
	bool isFind = false;//是否找到對應字串

    Start = s.Pos(finds.SubString(1,1));
    //找出要找的字串的第一個字的位置

    tmp = s.SubString(Start, finds.Length());
    //取跟要找的數字依樣長度 將同樣長度的字串儲存

    if(tmp == finds)
		isFind = true;

    return isFind;
}
//■■■■■■■■■■■■■■■■■ Delay
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
//■■■■■■■■■■■■■■■■■更新玩家[上線]清單
void TForm6::reflash_client()
{
	ListBox1->Clear();
	for (int i = 0; i < 4; i++)
	{
		if(user[i]!=NULL)
			ListBox1->Items->Add(user[i]);
	}

}

//■■■■■■■■■■■■■■■■■[Log]
void TForm6::Log(String print_)
{
	T_LOG->Items->Add(print_);
	T_LOG->ItemIndex = T_LOG->Items->Count-1;
}
//■■■■■■■■■■■■■■■■■[All_status]
void TForm6::Status()
{
	T_All_Area -> Clear();
	//T_LOG->Items->Add("");
	//IntToStr()
    if(Game)
		T_All_Area->Items->Add("遊戲狀態：遊戲中●");
	else
		T_All_Area->Items->Add("遊戲狀態：未開始○");

	T_All_Area->Items->Add("目前總數:"+IntToStr(total_num));
	T_All_Area->Items->Add("輪到P"+IntToStr(who_turn+1)+"("+player_[who_turn].host+")");

	if(isClockwise)
		T_All_Area->Items->Add("順序：順");
	else
		T_All_Area->Items->Add("順序：逆");


	T_All_Area->Items->Add("玩家人數:"+IntToStr(user_num));

	T_All_Area->Items->Add("現在玩家：");
	T_All_Area->Items->Add("{");
	for (int i = 0; i < user_num; i++)
	{
		T_All_Area->Items->Add("　P"+IntToStr(i+1)+":"+player_[i].host);

		if(player_[i].boom)
			T_All_Area->Items->Add("　Boom:true");
		else
			T_All_Area->Items->Add("　Boom:false");

		T_All_Area->Items->Add("　Next:P"+IntToStr(player_[i].next+1));
		T_All_Area->Items->Add("　Back:P"+IntToStr(player_[i].back+1));
	}
	T_All_Area->Items->Add("}");
	T_All_Area->Items->Add("Seed："+IntToStr(ssd));
	T_All_Area->Items->Add("card_num："+IntToStr(card_num));


}


//---------------------------------------------------------------------------

