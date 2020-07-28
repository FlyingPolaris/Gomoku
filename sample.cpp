#pragma GCC optimize(2)
#include "AIController.h"
#include <utility>
#include <cmath>
#include <cstring>
#include <climits>

typedef long long ll;
extern int ai_side; //0: black, 1: white
std::string ai_name = "polaris'_ai";

int turn = 0;
int board[15][15];
int positionweight[15][15] =
{
{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
{ 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1 },
{ 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1 },
{ 1, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 1 },
{ 1, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 1 },
{ 1, 2, 2, 3, 3, 4, 4, 4, 4, 4, 3, 3, 2, 2, 1 },
{ 1, 2, 2, 3, 3, 4, 4, 4, 4, 4, 3, 3, 2, 2, 1 },
{ 1, 2, 2, 3, 3, 4, 4, 5, 4, 4, 3, 3, 2, 2, 1 },
{ 1, 2, 2, 3, 3, 4, 4, 4, 4, 4, 3, 3, 2, 2, 1 },
{ 1, 2, 2, 3, 3, 4, 4, 4, 4, 4, 3, 3, 2, 2, 1 },
{ 1, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 1 },
{ 1, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 1 },
{ 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1 },
{ 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1 },
{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
};

ll judge_score(int n)
{
    if (n == 0) return 0;
    if (n == 1 || n == 2) return pow(10, n);
    if (n == 3) return pow(100, n);
    if (n == 4) return pow(1000, n);
    return 1e16;
}


int dir[4][2] =
{
{ 0, 1 },
{ 1, 0 },
{ 1, 1 },
{ 1, -1 },
};

const int DEPTH = 6;
int x_place = 0, y_place = 0;
ll score[2] = { 0,0 };
int range[4] = { 14,0,14,0 };

void range_update(int* range, int x, int y)
{
    if (x - 2 >= 0)
    {
        if (x - 2 < range[0]) range[0] = x - 2;
        else range[0] = 0;
    }
    if (x + 2 < 15)
    {
        if (x + 2 > range[1]) range[1] = x + 2;
        else range[1] = 14;
    }
    if (y - 2 >= 0)
    {
        if (y - 2 < range[2]) range[2] = y - 2;
        else range[2] = 0;
    }
    if (y + 2 < 15)
    {
        if (y + 2 > range[3]) range[3] = y + 2;
        else range[3] = 14;
    }
}


bool outofboard(int x, int y)
{
    if (x < 0 || x > 14 || y < 0 || y > 14)
        return true;
    else return false;
}


bool search_place(int x, int y)
{
    if (board[x][y] != -1) return false;
    if (outofboard(x, y)) return false;
    for (int i = x - 2;i <= x + 2;++i)
        for (int j = y - 2;j <= j + 2;++j)
        {
            if (!outofboard(i, j) && (board[i][j] == 0 || board[i][j] == 1))
                return true;
        }
    return false;
}

//int score_cal(int a[15][15], int color, bool flag)
//{
//    int score = 0;
//    int count = 0;
//
//    for (int i = 0;i < 15;++i)
//        for (int j = 0;j < 15;++j)
//        {
//            if (a[i][j] == color)
//            {
//                for (int m = i - 1;m <= i + 1;++m)
//                    for (int n = j - 1;n <= j + 1;++n)
//                    {
//                        if (!outofboard(m, n))
//                        {
//                            if (a[m][n] != -1) count++;
//                        }
//                    }
//                if (count == 1) score += 10 * positionweight[i][j];
//            }
//        }
//    count = 0;
//
//    for (int i = 0;i < 15;++i)
//        for (int j = 0;j < 15;++j)
//        {
//            if (a[i][j] == color) count++;
//            else if (count > 1)
//            {
//                int weight = 0;
//                for (int m = j - count;m <= j - 1;++m)
//                {
//                    weight += positionweight[i][m];
//                }
//                weight /= count;
//
//                if (a[i][j] == -1 && a[i][j - count - 1] == -1)
//                {
//                    if (count == 5) score += 1000000;
//                    else score += pow(10, count) * weight;
//                }
//                else if (a[i][j] != -1 && a[i][j - count - 1] != -1)
//                {
//                    if (count == 5) score += 1000000;
//                }
//                else
//                {
//                    if (count == 5) score += 1000000;
//                    else score += pow(10, count - 1) * weight;
//                }
//                count = 0;
//            }
//            else count = 0;
//        }
//    count = 0;
//
//    for (int j = 0;j < 15;++j)
//        for (int i = 0;i < 15;++i)
//        {
//            if (a[i][j] == color) count++;
//            else if (count > 1)
//            {
//                int weight = 0;
//                for (int m = i - count;m <= i - 1;++m)
//                {
//                    weight += positionweight[m][j];
//                }
//                weight /= count;
//                if (a[i - count - 1][j] == -1 && a[i][j] == -1)
//                {
//                    if (count == 5) score += 1000000;
//                    else score += pow(10, count) * weight;
//                }
//                else if (a[i][j] != -1 && a[i - count - 1][j] != -1)
//                {
//                    if (count == 5) score += 1000000;
//                }
//                else
//                {
//                    if (count == 5) score += 1000000;
//                    else score += pow(10, count - 1) * weight;
//                }
//                count = 0;
//            }
//            else count = 0;
//        }
//    count = 0;
//
//    for (int i = -14;i <= 14;++i)
//    {
//        int j = i;
//        int k = 14;
//        for (;j < 15;++j, --k)
//        {
//            if (!outofboard(j, k))
//            {
//                if (a[j][k] == color) count++;
//                else if (count > 1)
//                {
//                    int weight = 0;
//                    for (int m = 1;m <= count;++m)
//                    {
//                        weight += positionweight[j - m][k + m];
//                    }
//                    weight /= count;
//
//                    if (a[j - count - 1][k + count + 1] == -1 && a[j][k] == -1)
//                    {
//                        if (count == 5) score += 1000000;
//                        else score += pow(10, count) * weight;
//                    }
//                    else if (a[j - count - 1][k + count + 1] != -1 && a[j][k] != -1)
//                    {
//                        if (count == 5) score += 1000000;
//                    }
//                    else
//                    {
//                        if (count == 5) score += 1000000;
//                        else score += pow(10, count - 1) * weight;
//                    }
//                    count = 0;
//                }
//                else count = 0;
//            }
//        }
//    }
//    count = 0;
//
//    for (int i = -14;i <= 14;++i)
//    {
//        int j = i;
//        int k = 0;
//        for (;j < 15;++j, ++k)
//        {
//            if (!outofboard(j, k))
//            {
//                if (a[j][k] == color) count++;
//                else if (count > 1)
//                {
//                    int weight = 0;
//                    for (int m = 1;m <= count;++m)
//                    {
//                        weight += positionweight[j - m][k - m];
//                    }
//                    weight /= count;
//
//                    if (a[j - count - 1][k - count - 1] == -1 && a[j][k] == -1)
//                    {
//                        if (count == 5) score += 1000000;
//                        else score += pow(10, count) * weight;
//                    }
//                    else if (a[j - count - 1][k - count - 1] != -1 && a[j][k] != -1)
//                    {
//                        if (count == 5) score += 1000000;
//                    }
//                    else
//                    {
//                        if (count == 5) score += 1000000;
//                        else score += pow(10, count - 1) * weight;
//                    }
//                    count = 0;
//                }
//                else count = 0;
//            }
//        }
//    }
//
//    if (flag)
//    {
//        return score - score_cal(a, 1 - color, false);
//    }
//
//    return score;
//
//}


int score_update(int x, int y, int color, ll* score)
{
    bool left_blocked = false, right_blocked = false;
    int num1 = 0, num2 = 0, num = 0;
    //int weight1 = 0, weight2 = 0, weight = 0;
    for (int i = 0;i < 4;++i)
    {
        int det_x = dir[i][0], det_y = dir[i][1];
        while (!outofboard(x - det_x * (1 + num1), y - det_y * (1 + num1)))
        {
            if (board[x - det_x * (1 + num1)][y - det_y * (1 + num1)] != color) break;
            //weight1 += positionweight[x - det_x * (1 + num1)][y - det_y * (1 + num1)];
            num1++;
        }
       // weight += weight1;
       // if(num1) weight1 /= num1;
        while (!outofboard(x + det_x * (1 + num2), y + det_y * (1 + num2)))
        {
            if (board[x + det_x * (1 + num2)][y + det_y * (1 + num2)] != color) break;
           // weight2 += positionweight[x + det_x * (1 + num2)][y + det_y * (1 + num2)];
            num2++;
        }
      //  weight += weight2;
       // if (num2) weight2 /= num2;

      //  weight += positionweight[x][y];
        num = num1 + num2 + 1;
      //  weight /= num;

        if (outofboard(x - det_x * (1 + num1), y - det_y * (1 + num1)) || board[x - det_x * (1 + num1)][y - det_y * (1 + num1)] != -1) left_blocked = true;
        if (outofboard(x + det_x * (1 + num2), y + det_y * (1 + num2)) || board[x + det_x * (1 + num2)][y + det_y * (1 + num2)] != -1) right_blocked = true;

        score[color] -= judge_score(num1 - left_blocked);//* weight1;
        score[color] -= judge_score(num2 - right_blocked);// *weight2;

        if (num == 5) score[color] += judge_score(5);
        else if (left_blocked + right_blocked < 2) score[color] += judge_score(num + 1 - left_blocked - right_blocked);// *weight;

        //weight1 = weight2 = 0;
        if (!num1 && left_blocked)
        {
            while (!outofboard(x - det_x * (1 + num1), y - det_y * (1 + num1)))
            {
                if (board[x - det_x * (1 + num1)][y - det_y * (1 + num1)] != 1 - color) break;
               // weight1 += positionweight[x - det_x * (1 + num1)][y - det_y * (1 + num1)];
                num1++;
            }
           // if(num1) weight1 /= num1;

            if (outofboard(x - det_x * (1 + num1), y - det_y * (1 + num1)) || board[x - det_x * (1 + num1)][y - det_y * (1 + num1)] != -1) left_blocked = true;
            else left_blocked = false;

            score[1 - color] -= judge_score(num1 - left_blocked);// *weight1;
            if (!left_blocked) score[1 - color] += judge_score(num1 - 1);// *weight1;
        }

        if (!num2 && right_blocked)
        {
            while (!outofboard(x + det_x * (1 + num2), y + det_y * (1 + num2)))
            {
                if (board[x + det_x * (1 + num2)][y + det_y * (1 + num2)] != color) break;
             //   weight2 += positionweight[x + det_x * (1 + num2)][y + det_y * (1 + num2)];
                num2++;
            }
           // if(num2) weight2 /= num2;

            if (outofboard(x + det_x * (1 + num2), y + det_y * (1 + num2)) || board[x + det_x * (1 + num2)][y + det_y * (1 + num2)] != -1) right_blocked = true;
            else right_blocked = false;

            score[1 - color] -= judge_score(num2 - right_blocked);// *weight2;
            if (!right_blocked) score[1 - color] += judge_score(num2 - 1);// *weight2;
        }
    }

    return score[ai_side] - score[1 - ai_side];
}

struct tree
{
    int x, y, alpha, beta;
    int w;
};

ll min_max(int deep, int color, int ALPHA, int BETA, int _range[4], ll _score[2])
{
    tree t;
    //t.w = INT_MIN;
    t.alpha = ALPHA;
    t.beta = BETA;
    int minmax;
    bool flag = false;
    int range[4];
    ll score[2];



    if (deep % 2 == 0 && deep != 0)
    {
        for (int i = _range[0];i <= _range[1];++i)
        {
            bool breakflag = false;
            for (int j = _range[2];j <= _range[3];++j)
            {
                if (board[i][j] == -1)
                {
                    memcpy(range, _range, 4 * sizeof(int));
                    memcpy(score, _score, 2 * sizeof(ll));
                    board[i][j] = color;
                    range_update(range, i, j);

                    score_update(i, j, color, score);

                    minmax = min_max(deep - 1, 1 - color, t.alpha, t.beta, range, score);
                    board[i][j] = -1;

                    if (t.alpha < minmax)
                    {
                        t.alpha = minmax;
                        t.x = i;
                        t.y = j;
                    }

                    if (t.beta <= t.alpha) breakflag = true;
                }
                if (breakflag) break;
            }
        }
    }

    if (deep % 2 == 1)
    {
        for (int i = _range[0];i <= _range[1];++i)
        {
            bool breakflag = false;
            for (int j = _range[2];j <= _range[3];++j)
            {
                if (board[i][j] == -1)
                {
                    memcpy(range, _range, 4 * sizeof(int));
                    memcpy(score, _score, 2 * sizeof(ll));
                    board[i][j] = color;
                    range_update(range, i, j);

                    score_update(i, j, color, score);

                    minmax = min_max(deep - 1, 1 - color, t.alpha, t.beta, range, score);
                    board[i][j] = -1;

                    if (t.beta > minmax)
                    {
                        t.beta = minmax;
                        t.x = i;
                        t.y = j;
                    }

                    if (t.beta <= t.alpha) breakflag = true;
                }
                if (breakflag) break;
            }
        }
    }

    x_place = t.x;
    y_place = t.y;
    return score[ai_side] - score[1 - ai_side];

}

//init function is called once at the beginning
void init() 
{
    score[0] = score[1] = 0;
	memset(board, -1, sizeof(board));
}

// loc is the action of your opponent
// Initially, loc being (-1,-1) means it's your first move
// If this is the third step(with 2 black ), where you can use the swap rule, your output could be either (-1, -1) to indicate that you choose a swap, or a coordinate (x,y) as normal.

//std::pair<int, int> getRandom() 
//{
//    while (true) 
//    {
//        int x = rand() % 15;
//        int y = rand() % 15;
//        if (board[x][y] == -1) 
//        {
//            board[x][y] = ai_side;
//            return std::make_pair(x, y);
//        }
//    }
//}


bool swap_or_not()
{
    if (score[1 - ai_side] <= 80) return false;
    else return true;
}

std::pair<int, int> action(std::pair<int, int> loc) 
{
	/* TODO: Replace this by your code */
	/* This is now a random strategy */
    turn++;
    if (loc.first == -1 && loc.second == -1)
    {
        if (turn == 1)
        {
            board[7][7] = ai_side;
            range[0] = range[2] = 5;
            range[1] = range[3] = 9;
            score[1 - ai_side] += 50;
            return std::make_pair(7, 7);
        }
        else if (turn == 3)
        {
            for (int i = 0; i < 15; ++i)
                for (int j = 0; j < 15; ++j)
                    if (board[i][j] != -1)  board[i][j] = 1 - board[i][j];
            int tmp = score[0];
            score[0] = score[1];
            score[1] = tmp;

            min_max(DEPTH, ai_side, INT_MIN, INT_MAX, range, score);
            board[x_place][y_place] = ai_side;
            score_update(x_place, y_place, ai_side, score);
            range_update(range, x_place, y_place);
            return std::make_pair(x_place, y_place);
        }
    }

    board[loc.first][loc.second] = 1 - ai_side;
    score_update(loc.first, loc.second, 1 - ai_side, score);
    range_update(range, loc.first, loc.second);

    if (turn == 1)
    {
        if (board[7][7] == -1)
        {
            board[7][7] = ai_side;
            range[0] = range[2] = 5;
            range[1] = range[3] = 9;
            
            bool flag = false;
            for (int i = 6;i <= 8;++i)
                for (int j = 6;j <= 8;++j)
                {
                    if (board[i][j] == 1 - ai_side) flag = true;
                }
            if (flag) score[1 - ai_side] -= 40;
            else score[ai_side] += 50;

            return std::make_pair(7, 7);
        }
        else
        {
            if (rand() % 2)
            {
                board[7][6] = ai_side;
                range[0] = 5;
                range[1] = 9;
                range[2] = 4;
                range[3] = 8;
                score[1 - ai_side] -= 50;
                return std::make_pair(7, 6);
            }
            else
            {
                board[6][6] = ai_side;
                range[0] = range[2] = 4;
                range[1] = range[3] = 8;
                score[1 - ai_side] -= 50;
                return std::make_pair(6, 6);
            }
        }
    }

    if (turn == 2 && ai_side == 1 && swap_or_not())
    {
        for (int i = 0; i < 15; ++i)
            for (int j = 0; j < 15; ++j)
                if (board[i][j] != -1)  board[i][j] = 1 - board[i][j];

        int tmp = score[0];
        score[0] = score[1];
        score[1] = tmp;

        return std::make_pair(-1, -1);
    }
    
    min_max(DEPTH, ai_side, INT_MIN, INT_MAX, range, score);
    board[x_place][y_place] = ai_side;
    score_update(x_place, y_place, ai_side, score);
    range_update(range, x_place, y_place);
    return std::make_pair(x_place, y_place);
}
