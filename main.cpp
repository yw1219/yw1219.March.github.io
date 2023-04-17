//DXライブラリの関数を使えるようにする
#include "DxLib.h"

//メッセージのフラグ管理用に定義
#define NORMAL 0
#define ERROR1 1
#define ERROR2 2
#define WINPL1 3
#define WINPL2 4
#define DRAWPL 5

//出力するメッセージの状態判別用の変数
int state = NORMAL;

//マウスのフラグ管理用に定義
#define HOLD 6
#define RELEASE 7

//マウスのクリック状態判別用の変数
int Mstate = RELEASE;

// キーが押されているフレーム数を格納する
int Key[256];

//マウスの入力状態と座標用変数
int Mouse, xMouse, yMouse;

//画像用変数
int Board, PieceW, PieceB, P1, P2, Pass;

//盤面の配列、0なら空、1なら黒、2なら白
int BoardData[8][8] = { 
{0, 0, 0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0, 0, 0},
{0, 0, 0, 2, 1, 0, 0, 0},
{0, 0, 0, 1, 2, 0, 0, 0},
{0, 0, 0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0, 0, 0} };

//手番、1なら黒、2なら白
int Player = 1;

//プレイヤーの得点
int Point1, Point2;

//結果出力用の得点
int result1, result2;


//操作中の座標
int x = 50, y = 150;

//座標の配列
int XData[8] = { 0, 100, 200, 300, 400, 500, 600, 700 };
int YData[8] = { 100, 200, 300, 400, 500, 600, 700, 800 };



//マウスをクリックした時の処理
void Click() {
	bool cpx = false;
	bool cpy = false;
	//マウスの入力状態取得
	Mouse = GetMouseInput();
	//マウスの座標取得
	GetMousePoint(&xMouse, &yMouse);

	//マウスの左ボタンが押されていたら
	if ((Mouse & MOUSE_INPUT_LEFT) != 0 && Mstate == RELEASE) {

		//マウスのクリックした座標を取得
		if (500 <= xMouse && xMouse <= 757) {
			cpx = true;
		}
		if (10 <= yMouse && yMouse <= 84) {
			cpy = true;
		}
		//クリックした座標がパスボタン内だったら手番を変える
		if (cpx && cpy) {
			switch (Player) {
			case 1:
				Player = 2;
				Mstate = HOLD;
				break;
			case 2:
				Player = 1;
				Mstate = HOLD;
				break;
			default:
				break;
			}
		}
	}

	//プレイヤーを変えた後にマウスが離されたら状態を戻す
	if ((Mouse & MOUSE_INPUT_LEFT) == 0 && Mstate == HOLD) {
		Mstate = RELEASE;
	}

	
	
	
}



// キーの入力状態を更新する処理
int gpUpdateKey() {
	// 現在のキーの入力状態を格納する
	char tmpKey[256];
	// 全てのキーの入力状態を得る
	GetHitKeyStateAll(tmpKey);
	// i番のキーコードに対応するキーが押されていたら加算、押されていなければ0にする
	for (int i = 0; i < 256; i++) {
		if (tmpKey[i] != 0) {
			Key[i]++;
		}
		else {
			Key[i] = 0;
		}
	}
	return 0;
}

//キーボード入力の判定の処理
void Keybord() {
	//下キーが押された時に一番下なら一番上まで戻す、下キーが押されたら一つ下のマスへ
	if (y == 850 && Key[KEY_INPUT_DOWN] == 1) {
		y = 150;
	}
	else if (Key[KEY_INPUT_DOWN] == 1) {
		y = y + 100; 
	}
	//上キーが押された時に一番上なら一番下まで戻す、上キーが押されたら一つ上のマスへ
	if (y == 150 && Key[KEY_INPUT_UP] == 1) {
		y = 850;
	}
	else if (Key[KEY_INPUT_UP] == 1) {
		y = y - 100;
	}
	//右キーが押された時に一番右なら一番左まで戻す、右キーが押されたら一つ右のマスへ
	if (x == 750 && Key[KEY_INPUT_RIGHT] == 1) {
		x = 50;
	}
	else if (Key[KEY_INPUT_RIGHT] == 1) {
		x = x + 100;
	}
	//左キーが押された時に一番右なら一番左まで戻す、左キーが押されたら一つ左のマスへ
	if (x == 50 && Key[KEY_INPUT_LEFT] == 1) {
		x = 750;
	}
	else if (Key[KEY_INPUT_LEFT] == 1) {

		x = x - 100;
	}

}

//手番ごとに表示するコマを変える処理
void Change() {
	if (Player == 1) {
		DrawRotaGraph(x, y, 1.0, 0.0, P1, TRUE);
	}
	else if (Player == 2) {
		DrawRotaGraph(x, y, 1.0, 0.0, P2, TRUE);
	}
}

//エンターを押した時の処理
void Enter() {
	if (Key[KEY_INPUT_RETURN] == 1) { 
		//多重ループを抜ける用の変数
		bool finished = false;
			for (int i = 0; i < 8; i++) {
				for (int j = 0; j < 8; j++) {

					//ループして自分が操作してるマスの座標に対して処理を行う
					if (XData[i] + 50 == x && YData[j] + 50 == y) {

						//全部でめくれるコマが何枚あるかチェックする用の変数-1枚もめくれない時のメッセージ用
						int check = 0;

						//既に置かれてるマスには置けないというメッセージを出す
						if (BoardData[i][j] != 0) {
							state = ERROR1;
							finished = true;
							break;
						}
						else {

							/*自分が置いた場所を(0,0)という風に考えてめくれるかどうかを判別する処理を8方向分していく
							 (-1,-1)(0, -1)(+1,-1)
							 (-1, 0)(0,  0)(+1, 0)
							 (-1,+1)(0, +1)(+1,+1)
							*/

							//左上(-1, -1);
							for (int k = 1; i - k > -1 && j - k > -1; k++) {
								//めくれる枚数用の変数
								int count = 0;
								//一番端までいってないかチェックする用の変数
								bool last = false;
								//隣接してるマスが空もしくは自分と同じ色なら何もしない
								if (BoardData[i - k][j - k] == 0 || BoardData[i - k][j - k] == Player) {
									break;
								}
								//隣接してるマスが自分と違う色なら次のループへ
								else if (BoardData[i - k][j - k] != Player) {
									for (int l = k + 1; i - l > -1 && j - l > -1; l++) {
										//隣からL番目のマスが自分と違う色かつ空でなければcountを増やす　例）自分と別の色が続いてる場合カウントを増やしていく
										if (BoardData[i - l][j - l] != Player && BoardData[i - l][j - l] != 0) {
											count++;
										}
										//隣からL番目のマスが自分と同じ色かつ一度もカウントされてなかったらcountを増やす　例）挟んでるのが一個だけの時に使う
										else if (BoardData[i - l][j - l] == Player && count == 0) {
											count++;
											last = true;
											break;
										}
										//隣からL番目のマスが自分と同じ色かつ一度以上カウントされてたららループを抜ける　例）めくっていく方向に自分の色があったらそこでカウントをやめる
										else if (BoardData[i - l][j - l] == Player && count >= 1) {
											last = true;
											break;
										}
										//隣からL番目のマスが空だったらカウントをリセットする　例）めくっていく方向が空だったらカウントをリセットする
										else if (BoardData[i - l][j - l] == 0) {
											count = 0;
											break;
										}
									}

									//もしめくれる枚数が1枚以上かつ盤面の外までいってなければ一致した箇所の色を変える
									if (count >= 1 && last) {
										for (int m = count; 0 <= m; m--) {
											BoardData[i - k][j - k] = Player;
											k++;
											check++;
										}
									}break;
								}
								else {
									break;
								}

							}

							//上(0, -1);
							for (int k = 1; j - k > -1; k++) {
								int count = 0;
								bool last = false;
								if (BoardData[i][j - k] == 0 || BoardData[i][j - k] == Player) {
									break;
								}
								else if (BoardData[i][j - k] != Player) {
									for (int l = k + 1; j - l > -1; l++) {
										if (BoardData[i][j - l] != Player && BoardData[i][j - l] != 0) {
											count++;
										}
										else if (BoardData[i][j - l] == Player && count == 0) {
											count++;
											last = true;
											break;
										}
										else if (BoardData[i][j - l] == Player && count >= 1) {
											last = true;
											break;
										}
										else if (BoardData[i][j - l] == 0) {
											count = 0;;
											break;
										}
									}

									if (count >= 1 && last) {
										for (int m = count; 0 <= m; m--) {
											BoardData[i][j - k] = Player;
											k++;
											check++;
										}
									}break;
								}
								else {
									break;
								}

							}

							//右上(+1,-1);
							for (int k = 1; i + k < 8 && j - k > -1; k++) {
								int count = 0;
								bool last = false;
								if (BoardData[i + k][j - k] == 0 || BoardData[i + k][j - k] == Player) {
									break;
								}
								else if (BoardData[i + k][j - k] != Player) {
									for (int l = k + 1; i + l < 8 && j - l > -1; l++) {
										if (BoardData[i + l][j - l] != Player && BoardData[i + l][j - l] != 0) {
											count++;
										}
										else if (BoardData[i + l][j - l] == Player && count == 0) {
											count++;
											last = true;
											break;
										}
										else if (BoardData[i + l][j - l] == Player && count >= 1) {
											last = true;
											break;
										}
										else if (BoardData[i + l][j - l] == 0) {
											count = 0;
											break;
										}
									}

									if (count >= 1 && last) {
										for (int m = count; 0 <= m; m--) {
											BoardData[i + k][j - k] = Player;
											k++;
											check++;
										}
									}break;
								}
								else {
									break;
								}

							}

							//左(-1, 0);
							for (int k = 1; i - k > -1; k++) {
								int count = 0;
								bool last = false;
								if (BoardData[i - k][j] == 0 || BoardData[i - k][j] == Player) {
									break;
								}
								else if (BoardData[i - k][j] != Player) {
									for (int l = k + 1; i - l > -1; l++) {
										if (BoardData[i - l][j] != Player && BoardData[i - l][j] != 0) {
											count++;
										}
										else if (BoardData[i - l][j] == Player && count == 0) {
											count++;
											last = true;
											break;
										}
										else if (BoardData[i - l][j] == Player && count >= 1) {
											last = true;
											break;
										}
										else if (BoardData[i - l][j] == 0) {
											count = 0;
											break;
										}
									}

									if (count >= 1 && last) {
										for (int m = count; 0 <= m; m--) {
											BoardData[i - k][j] = Player;
											k++;
											check++;
										}
									}
									break;
								}
								else {
									break;
								}

							}

							//右(+1, 0);
							for (int k = 1; i + k < 8; k++) {
								int count = 0;
								bool last = false;
								if (BoardData[i + k][j] == 0 || BoardData[i + k][j] == Player) {
									break;
								}
								else if (BoardData[i + k][j] != Player) {
									for (int l = k + 1; i + l < 8; l++) {
										if (BoardData[i + l][j] != Player && BoardData[i + l][j] != 0) {
											count++;
										}
										else if (BoardData[i + l][j] == Player && count == 0) {
											count++;
											last = true;
											break;
										}
										else if (BoardData[i + l][j] == Player && count >= 1) {
											last = true;
											break;
										}
										else if (BoardData[i + l][j] == 0) {
											count = 0;
											break;
										}
									}

									if (count >= 1 && last) {
										for (int m = count; 0 <= m; m--) {
											BoardData[i + k][j] = Player;
											k++;
											check++;
										}
									}break;
								}
								else {
									break;
								}

							}

							//左下(-1,+1);
							for (int k = 1; i - k > -1 && j + k < 8; k++) {
								int count = 0;
								bool last = false;
								if (BoardData[i - k][j + k] == 0 || BoardData[i - k][j + k] == Player) {
									break;
								}
								else if (BoardData[i - k][j + k] != Player) {
									for (int l = k + 1; i - l > -1 && j + l < 8; l++) {
										if (BoardData[i - l][j + l] != Player && BoardData[i - l][j + l] != 0) {
											count++;
										}
										else if (BoardData[i - l][j + l] == Player && count == 0) {
											count++;
											last = true;
											break;
										}
										else if (BoardData[i - l][j + l] == Player && count >= 1) {
											last = true;
											break;
										}
										else if (BoardData[i - l][j + l] == 0) {
											count = 0;
											break;
										}
									}

									if (count >= 1 && last) {
										for (int m = count; 0 <= m; m--) {
											BoardData[i - k][j + k] = Player;
											k++;
											check++;
										}
									}break;
								}
								else {
									break;
								}

							}

							//下(0, +1);
							for (int k = 1; j + k < 8; k++) {
								int count = 0;
								bool last = false;
								if (BoardData[i][j + k] == 0 || BoardData[i][j + k] == Player) {
									break;
								}
								else if (BoardData[i][j + k] != Player) {
									for (int l = k + 1; j + l < 8; l++) {
										if (BoardData[i][j + l] != Player && BoardData[i][j + l] != 0) {
											count++;
										}
										else if (BoardData[i][j + l] == Player && count == 0) {
											count++;
											last = true;
											break;
										}
										else if (BoardData[i][j + l] == Player && count >= 1) {
											last = true;
											break;
										}
										else if (BoardData[i][j + l] == 0) {
											count = 0;
											break;
										}
									}

									if (count >= 1 && last) {
										for (int m = count; 0 <= m; m--) {
											BoardData[i][j + k] = Player;
											k++;
											check++;
										}
									}break;
								}
								else {
									break;
								}

							}

							//右下(+1,+1);
							for (int k = 1; i + k < 8 && j + k < 8; k++) {
								int count = 0;
								bool last = false;
								if (BoardData[i + k][j + k] == 0 || BoardData[i + k][j + k] == Player) {
									break;
								}
								else if (BoardData[i + k][j + k] != Player) {
									for (int l = k + 1; i + l < 8 && j + l < 8; l++) {
										if (BoardData[i + l][j + l] != Player && BoardData[i + l][j + l] != 0) {
											count++;
										}
										else if (BoardData[i + l][j + l] == Player && count == 0) {
											count++;
											last = true;
											break;
										}
										else if (BoardData[i + l][j + l] == Player && count >= 1) {
											last = true;
											break;
										}
										else if (BoardData[i + l][j + l] == 0) {
											count = 0;
											break;
										}
									}

									if (count >= 1 && last) {
										for (int m = count; 0 <= m; m--) {
											BoardData[i + k][j + k] = Player;
											k++;
											check++;
										}
									}break;
								}
								else {
									break;
								}

							}

							//めくれる枚数が0ならメッセージを表示
							if (check == 0) {
								state = ERROR2;
								finished = true;
								break;
							}
							
							//プレイヤーが黒なら指定のコマに黒を置いてプレイヤーを白に切り替え
							if (Player == 1) {
								BoardData[i][j] = Player;
								state = NORMAL;
								Player = 2;
								finished = true;
								break;
							}
							//プレイヤーが白なら指定のコマに白を置いてプレイヤーを黒に切り替え
							else {
								BoardData[i][j] = Player;
								state = NORMAL;
								Player = 1;
								finished = true;
								break;
							}

						}
					}
					
				}
				//1番目のループを抜けるための処理
				if (finished) {
					break;
				}
			}
		}
	}

//配列の中身を見て盤面に表示する処理
void Set() {
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			//配列の中身が1なら黒いコマを表示して黒側の得点を加算
			if (BoardData[i][j] == 1) {
				DrawGraph(XData[i], YData[j], PieceB, TRUE);
				Point1++;
			}
			//配列の中身が2なら白いコマを表示して白側の得点を加算
			else if (BoardData[i][j] == 2) {
				DrawGraph(XData[i], YData[j], PieceW, TRUE);
				Point2++;
			}
			//配列の中身が0なら何も表示しないので処理は書かない

		}
	}
	
}

//配列の中身を見て勝負の判定をする処理
void End() {
	//配列の中身が1か2だけになった後に、1が多いときは黒側の勝利
	if (Point1 + Point2 == 64 && Point1 > Point2) {
		//ループによる点滅表示を防ぐため得点をresult変数に代入
		result1 = Point1;
		result2 = Point2;
		state = WINPL1;

	}
	//配列の中身が1か2だけになった後に、2が多いときは白側の勝利
	else if (Point1 + Point2 == 64 && Point1 < Point2) {
		result1 = Point1;
		result2 = Point2;
		state = WINPL2;
		
	}
	//配列の中身が1か2だけになった後に、同数なら引き分け
	else if (Point1 + Point2 == 64 && Point1 == Point2) {
		result1 = Point1;
		result2 = Point2;
		state = DRAWPL;
		
	}
	//配列の中身に0が残ってる場合（盤面が全て埋まっていない時）ポイントを初期化
	else {
		Point1 = 0;
		Point2 = 0;
	}
}

//状態によってメッセージを表示する処理
void Message() {
	if (state == ERROR1) {
		DrawFormatString(200, 50, GetColor(255, 255, 255), "そこには既に置かれています。");
	}
	else if (state == ERROR2) {
		DrawFormatString(200, 50, GetColor(255, 255, 255), "そこに置いてもめくれません。");
	}
	else if (state == WINPL1) {
		DrawFormatString(120, 40, GetColor(255, 255, 255), "黒が%d枚　白が%d枚で黒側の勝利です。", result1, result2);
	}
	else if (state == WINPL2) {
		DrawFormatString(120, 40, GetColor(255, 255, 255), "黒が%d枚　白が%d枚で白側の勝利です。", result1, result2);
	}
	else if (state == DRAWPL) {
		DrawFormatString(120, 40, GetColor(255, 255, 255), "黒が%d枚　白が%d枚で引き分けです。", result1, result2);
	}
}



//Windowsプログラム開始のメイン関数
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
    //ウィンドウモード変更,ウインドウサイズ変更,初期化,裏画面設定
    ChangeWindowMode(TRUE), SetGraphMode(800, 900, 32), DxLib_Init(), SetDrawScreen(DX_SCREEN_BACK); 

	//盤面の画像をロード
    Board = LoadGraph("Images/board.png");
	//黒いコマの画像をロード
    PieceB = LoadGraph("Images/piece_black.png");
	//白いコマの画像をロード
    PieceW = LoadGraph("Images/piece_white.png");
	//黒側を操作する時の画像をロード
	P1 = LoadGraph("Images/player1.png");
	//白側を操作する時の画像をロード
	P2 = LoadGraph("Images/player2.png");
	//パスボタンの画像をロード
	Pass = LoadGraph("Images/pass.png");

	

    // while( 裏画面を表画面に反映, メッセージ処理, 画面クリア, キーの状態更新 )
	while (!ScreenFlip() && !ProcessMessage() && !ClearDrawScreen() && gpUpdateKey() == 0) {

		//背景画像の描画
		DrawGraph(0, 95, Board, TRUE);
		//パスボタンの描画
		DrawGraph(500, 10, Pass, TRUE);

		Click();
		Keybord();
		Enter();
		Set();
		Change();
		End();
		Message();
			
		
	}
    DxLib_End();
    return 0;
}
