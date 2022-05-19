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
    int turn;
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
            cout << " ";
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
bool finch(STATUS status)
{
    for (int i = 0; i < 24; i=i+3)
    {
        if (status.turn == 0)
        {
            if ((status.s2 & WIN_PATTERN[i]) && (status.s2 & WIN_PATTERN[i+1]) && (status.s2 & WIN_PATTERN[i+2]))
            {
                //cout << "負け:";
                //cout << bitset<9>(status.s2) << "：" << i << ":" << bitset<9>(WIN_PATTERN[i]) << ", " << i + 1 << ":" << bitset<9>(WIN_PATTERN[i + 1]) << ", " << i + 2 << ":" << bitset<9>(WIN_PATTERN[i + 2]) << endl;
                //cout << endl;
                return true;
            }
        }
        else if(status.turn == 1)
        {
            if ((status.s1 & WIN_PATTERN[i]) && (status.s1 & WIN_PATTERN[i+1]) && (status.s1 & WIN_PATTERN[i+2]))
            {
                //cout << "勝ち:";
                //cout << bitset<9>(status.s1) << "：" << i << ":" << bitset<9>(WIN_PATTERN[i]) << ", " << i + 1 << ":" << bitset<9>(WIN_PATTERN[i + 1]) << ", " << i + 2 << ":" << bitset<9>(WIN_PATTERN[i + 2]) << endl;
                //cout << endl;
                return true;
            }
        }
    }

    if ((status.s1 | status.s2) == 0b111111111)
    {
        //引き分け
        //cout << "引き分け" << endl;;
        status.turn = -1;
        return true;
    }

    //終了条件を満たさない
    //cout << "続行" << endl;
    return false;

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
    myturn = status.turn;
    //printstatus(status);

    for (int k = 0; k < numsim; k++)
    {
        //勝利条件を満たすまで着手し続ける
        while (!(finch(status)))
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
        if (status.turn == 0)
        {
            //負け
            //cout << "lose";
        }
        else if (status.turn == 1)
        {
            //勝ち
            //cout << "win";
            numwin += 1.0;
        }
        else
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
//返り値は各着手の勝率
vector<float> montecarlo(STATUS status, int numsim)
{
    vector<float> winrate;
    int nxmvs;
    float numwin = 0.0;
    STATUS nows;

    //初期化
    for (int i = 0; i < winrate.size(); i++)
        winrate[i] = 0.0;

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
            winrate.insert(winrate.begin() + i, numwin / float(numsim));
        }
    }

    return winrate;
}

int main()
{
    STATUS status;
    vector<float> winrate(9);

    //初期化
    status.s1 = 0;
    status.s2 = 0;
    status.turn = 0;

    //printstatus(status);
    
    //ひとまず10000回シミュレート見てもらう
    winrate = montecarlo(status, 10000);
    
    cout << "winrate:" << endl;
    printf("%f, ", winrate[0]);
    for (int i = 1; i < winrate.size(); i++)
        printf(", %f", winrate[i]);
    
    cout << endl;
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
