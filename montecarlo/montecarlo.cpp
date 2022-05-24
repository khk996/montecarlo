// montecarlo.cpp : このファイルには 'main' 関数が含まれています。プログラム実行の開始と終了がそこで行われます。
//

#include <iostream>
#include <vector>
#include <bitset>
#include <random>

using namespace std;

typedef struct {
    unsigned int s1;
    unsigned int s2;
    int turn, winner;
}STATUS;

//勝利パターン。8通りある。
const unsigned int WIN_PATTERN[24] =
{ (1 << 2) , (1 << 5) , (1 << 8), (1 << 1) , (1 << 4) , (1 << 7), (1 << 0) , (1 << 3) , (1 << 6),
  (1 << 0) , (1 << 1) , (1 << 2), (1 << 3) , (1 << 4) , (1 << 5), (1 << 6) , (1 << 7) , (1 << 8),
  (1 << 0) , (1 << 4) , (1 << 8), (1 << 2) , (1 << 4) , (1 << 6) };


//デバッグ用盤面の出力
int printstatus(STATUS status)
{
    for (int i = 0; i < 9; i++)
    {
        if (status.s1 & (1 << i))
        {
            cout << "○";
        }
        else if (status.s2 & (1 << i))
        {
            cout << "×";
        }
        else
        {
            cout << "  ";
        }
        if ((i % 3) == 2)
        {
            cout << endl;
        }
    }
    cout << endl;

    return 0;
}

//現在の盤面から着手可能手を求める
//返り値は、着手可能なら1、不可能なら0とした時の2進数表記を10進数にしたもの
int searchmv(STATUS status)
{
    int nxmv;
    nxmv = ~(status.s1 | status.s2);

    return nxmv;
}

//勝利条件を見たいているかの判定。引き分けの判定も行う。
//引き分けの場合、status.turnを-1にする。
int finch(STATUS status)
{
    for (int i = 0; i < 24; i=i+3)
    {
        if (status.turn == 0)
        {
            if ((status.s2 & WIN_PATTERN[i]) && (status.s2 & WIN_PATTERN[i+1]) && (status.s2 & WIN_PATTERN[i+2]))
            {
                //cout << "後攻の勝ち:";
                //cout << bitset<9>(status.s2) << "：" << i << ":" << bitset<9>(WIN_PATTERN[i]) << ", " << i + 1 << ":" << bitset<9>(WIN_PATTERN[i + 1]) << ", " << i + 2 << ":" << bitset<9>(WIN_PATTERN[i + 2]) << endl;
                //cout << endl;
                return 2;
            }
        }
        else if(status.turn == 1)
        {
            if ((status.s1 & WIN_PATTERN[i]) && (status.s1 & WIN_PATTERN[i+1]) && (status.s1 & WIN_PATTERN[i+2]))
            {
                //cout << "先攻の勝ち:";
                //cout << bitset<9>(status.s1) << "：" << i << ":" << bitset<9>(WIN_PATTERN[i]) << ", " << i + 1 << ":" << bitset<9>(WIN_PATTERN[i + 1]) << ", " << i + 2 << ":" << bitset<9>(WIN_PATTERN[i + 2]) << endl;
                //cout << endl;
                return 1;
            }
        }
    }

    if ((status.s1 | status.s2) == 0b111111111)
    {
        //引き分け
        //cout << "引き分け" << endl;;
        status.turn = -1;
        return 3;
    }

    //終了条件を満たさない
    //cout << "続行" << endl;
    return 0;

}

int finch2(STATUS status)
{
    for (int i = 0; i < 24; i = i + 3)
    {
        if (status.turn == 0)
        {
            if ((status.s2 & WIN_PATTERN[i]) && (status.s2 & WIN_PATTERN[i + 1]) && (status.s2 & WIN_PATTERN[i + 2]))
            {
                //cout << "後攻の勝ち:";
                //cout << bitset<9>(status.s2) << "：" << i << ":" << bitset<9>(WIN_PATTERN[i]) << ", " << i + 1 << ":" << bitset<9>(WIN_PATTERN[i + 1]) << ", " << i + 2 << ":" << bitset<9>(WIN_PATTERN[i + 2]) << endl;
                //cout << endl;
                //status.winner = 0;
                return 2;
            }
        }
        else if (status.turn == 1)
        {
            if ((status.s1 & WIN_PATTERN[i]) && (status.s1 & WIN_PATTERN[i + 1]) && (status.s1 & WIN_PATTERN[i + 2]))
            {
                //cout << "先攻の勝ち:";
                //cout << bitset<9>(status.s1) << "：" << i << ":" << bitset<9>(WIN_PATTERN[i]) << ", " << i + 1 << ":" << bitset<9>(WIN_PATTERN[i + 1]) << ", " << i + 2 << ":" << bitset<9>(WIN_PATTERN[i + 2]) << endl;
                //cout << endl;
                //status.winner = 1;
                return 1;
            }
        }
    }

    if ((status.s1 | status.s2) == 0b111111111)
    {
        //引き分け
        //cout << "引き分け" << endl;;
        //status.turn = -1;
        status.winner = 2;
        return 3;
    }

    //終了条件を満たさない
    //cout << "続行" << endl;
    return 0;

}

float sim(STATUS status, int numsim)
{
    int nxmvs, nx, myturn;
    float numwin = 0.0;
    vector<int> mv;
    STATUS startst;

    //初期状態の保存
    startst = status;

    //自分の番の保存。0なら自分は○。1なら自分は×。
    if (status.turn == 0)
        myturn = 1;
    else if (status.turn == 1)
        myturn = 0;

    //printstatus(status);

    for (int k = 0; k < numsim; k++)
    {
        //勝利条件を満たすまで着手し続ける
        while (finch2(status) == 0)
        {
            //printstatus(status);

            //盤面の着手可能手を求める
            nxmvs = searchmv(status);

            for (int i = 0; i < 9; i++)
            {
                if (nxmvs & (1 << i))
                {
                    mv.push_back(i);
                }
            }
            /*
            cout << "〇：" << bitset<9>(status.s1) << endl;
            cout << "×：" << bitset<9>(status.s2) << endl;
            cout << "着手可能手：" << bitset<9>(nxmvs) << endl;
            for (int i = 0; i < mv.size(); i++)
                cout << mv[i] << ", ";
            cout << endl;
            */

            //着手可能手からランダムに着手し、盤面を更新する
            bitset<9>nxmv(nxmvs);
            nx = mv[rand() % nxmv.count()];
            //cout << "着手：" << nx << endl;

            if (status.turn == 0)
            {
                status.s1 |= (1 << nx);
                status.turn = 1;
            }
            else
            {
                status.s2 |= (1 << nx);
                status.turn = 0;
            }

            mv.erase(mv.begin(), mv.end());
        }

        //どちらが勝っているかに応じて勝利数にカウントする
        if (finch2(status) == 1)
        {
            //先攻の勝ち
            if (myturn == 0)
                numwin += 1.0;
        }
        else if (finch2(status) == 2)
        {
            //後攻の勝ち
            if (myturn == 1)
                numwin += 1.0;
        }
        else if(finch2(status) == 3)
        {
            //引き分け
            //cout << "drow";
            numwin += 0.5;
        }

        //初期化
        status = startst;
    }
    //cout <<"winrate:"<<bitset<9>(status.s1) <<":" << float(numwin) << endl;
    return numwin;
}

//現在の盤面からモンテカルロ法を行って勝率を求める
//返り値は最も勝率が高い手
int montecarlo(STATUS status, int numsim)
{
    vector<float> winrate(9);
    int nxmvs;
    int best = 0;
    float max = -0.5;
    float numwin = 0.0;
    STATUS nows;

    //初期化
    for (int i = 0; i < winrate.size(); i++)
        winrate[i] = -1.0;

    //現在の盤面から可能な着手を求める
    nxmvs = searchmv(status);

    //numsimの分シミュレーションを行い、可能な着手の勝率を求める
    for (int i = 0; i < 9; i++)
    {

        if (nxmvs & (1 << i))
        {
            nows = status;

            //選んだ着手を行い盤面を更新する
            if (nows.turn == 0)
            {
                nows.s1 |= (1 << i);
                nows.turn = 1;
            }
            else
            {
                nows.s2 |= (1 << i);
                nows.turn = 0;
            }

            //プレイアウトを行う
            numwin = sim(nows, numsim);

            //勝率をリストに格納する。
            winrate[i] = numwin / float(numsim);
            
        }
        //cout <<i<<": " << winrate[i] << endl;
    }

    
    cout << "winrate:" << endl;
    printf("%f, ", winrate[0]);
    for (int i = 1; i < winrate.size(); i++)
        printf(", %f", winrate[i]);
    cout << endl;
    
    

    //勝率が最も高い手を選択する
    for (int i = 0; i < winrate.size(); i++)
    {
        if (max < winrate[i])
        {
            max = winrate[i];
            best = i;
        }
    }

    return best;
}

int playgame(STATUS status, int first, int second)
{
    int winner, best;
    STATUS nowstatus;

    nowstatus = status;

    //勝敗がつくまで打ち続ける
    while (finch2(status)==0)
    {
        //盤面の更新
        if (status.turn == 0)
        {
            //先手            
            best = montecarlo(nowstatus, first);
            status.s1 |= (1 << best);
            status.turn = 1;
            //cout << best << endl;
            //cout <<bitset<9>(status.s1)<<" ";
            //cout << bitset<9>(status.s2) << endl;
        }
        else
        {
            //後手
            best = montecarlo(nowstatus, second);
            status.s2 |= (1 << best);
            status.turn = 0;
            //cout << best << endl;
            //cout <<bitset<9>(status.s1)<<" ";
            //cout << bitset<9>(status.s2) << endl;
        }
        nowstatus = status;
        printstatus(status);
    }

    cout <<"結果:" << finch2(status) << endl;
    winner = finch2(status);

    return winner;
}

int main()
{
    STATUS status, nowstatus;
    int best, winner[3], sub;
    
    //初期化
    status.s1 = 0;
    status.s2 = 0;
    status.turn = status.winner = 0;
    winner[0] = winner[1] = winner[2] = 0;
    nowstatus = status;

    //printstatus(status);
    
    /*
    //一手進めた状態
    status.s1 |= (1 << 4);
    status.turn = 1;
    status.s2 |= (1 << 8);
    status.turn = 0;
    status.s1 |= (1 << 5);
    status.turn = 1;
    */

    //ひとまず10000回シミュレート見てもらう
    //best = montecarlo(status, 10000);
    
    //cout << "best:" << best << endl;
    
    //実際に戦わせてみよう
    //一方はシミュレーション数30、他方はシミュレーション数1000
    //公平性のため、それぞれが先攻後攻の場合を10回ずつ、計20回プレイしてみる
    //勝利数をwinnerに格納していく。
    //0番目は引き分けの数、1番目はシミュ1000回の勝利数、2番目はシミュ30回の勝利数
    
    for (int i = 0; i < 10; i++)
    {
        status = nowstatus;
        winner[playgame(status, 30, 1000)-1]++;
    }
    cout << "勝敗結果" << endl;
    cout << "引き分け：" << winner[2] << endl;
    cout << "後攻シミュ1000回の勝利数：" << winner[1] << endl;
    cout << "先攻シミュ30回の勝利数：" << winner[0] << endl;

    winner[0] = 0;
    winner[1] = 0;
    winner[2] = 0;

    for (int i = 0; i < 10; i++)
    {
        status = nowstatus;
        winner[playgame(status, 1000, 30)-1]++;
    }

    cout << "勝敗結果" << endl;
    cout << "引き分け：" << winner[2] << endl;
    cout << "先攻シミュ1000回の勝利数：" << winner[0] << endl;
    cout << "後攻シミュ30回の勝利数：" << winner[1] << endl;
    
}


// プログラムの実行: Ctrl + F5 または [デバッグ] > [デバッグなしで開始] メニュー
// プログラムのデバッグ: F5 または [デバッグ] > [デバッグの開始] メニュー

// 作業を開始するためのヒント: 
//    1. ソリューション エクスプローラー ウィンドウを使用してファイルを追加/管理します 
//   2. チーム エクスプローラー ウィンドウを使用してソース管理に接続します
//   3. 出力ウィンドウを使用して、ビルド出力とその他のメッセージを表示します
//   4. エラー一覧ウィンドウを使用してエラーを表示します
//   5. [プロジェクト] > [新しい項目の追加] と移動して新しいコード ファイルを作成するか、[プロジェクト] > [既存の項目の追加] と移動して既存のコード ファイルをプロジェクトに追加します
//   6. 後ほどこのプロジェクトを再び開く場合、[ファイル] > [開く] > [プロジェクト] と移動して .sln ファイルを選択します
