#pragma GCC optimize(2)
#include "AIController.h"
#include <utility>
#include <cmath>
#include <cstring>
#include <climits>


extern int ai_side; //0: black, 1: white
std::string ai_name = "polaris'_ai";

int turn = 0;
int board[15][15];
int positionweight[15][15]  = 
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
const int DEPTH = 6;
int x_place, y_place;
int NW = 0, NE = 14, SW = 0, SE = 14;

bool outofboard(int x, int y)
{
    if (x < 0 || x > 14 || y < 0 || y > 14)
        return true;
    else return false;
}


bool search_place(int a[15][15], int x, int y)
{
    if (a[x][y] != -1) return false;
    for (int i = x - 2;i <= x + 2;++i)
        for (int j = y - 2;j <= j + 2;++j)
        {
            if (!outofboard(i, j) && (a[i][j] == 0 || a[i][j] == 1))
                return true;
        }
    return false;
}

int score_cal(int a[15][15], int color, bool flag)
{
    int score = 0;
    int count = 0;

    for (int i = 0;i < 15;++i)
        for (int j = 0;j < 15;++j)
        {
            if (a[i][j] == color)
            {
                for (int m = i - 1;m <= i + 1;++m)
                    for (int n = j - 1;n <= j + 1;++n)
                    {
                        if (!outofboard(m, n))
                        {
                            if (a[m][n] != -1) count++;
                        }
                    }
                if (count == 1) score += 10 * positionweight[i][j];
            }
        }
    count = 0;

    for (int i = 0;i < 15;++i)
        for (int j = 0;j < 15;++j)
        {
            if (a[i][j] == color) count++;
            else if (count > 1)
            {
                int weight = 0;
                for (int m = j - count;m <= j - 1;++m)
                {
                    weight += positionweight[i][m];
                }
                weight /= count;

                if (a[i][j] == -1 && a[i][j - count - 1] == -1)
                {
                    if (count == 5) score += 1000000;
                    else score += pow(10, count) * weight;
                }
                else if (a[i][j] != -1 && a[i][j - count - 1] != -1)
                {
                    if (count == 5) score += 1000000;
                }
                else
                {
                    if (count == 5) score += 1000000;
                    else score += pow(10, count - 1) * weight;
                }
                count = 0;
            }
            else count = 0;
        }
    count = 0;

    for (int j = 0;j < 15;++j)
        for (int i = 0;i < 15;++i)
        {
            if (a[i][j] == color) count++;
            else if (count > 1)
            {
                int weight = 0;
                for (int m = i - count;m <= i - 1;++m)
                {
                    weight += positionweight[m][j];
                }
                weight /= count;
                if (a[i - count - 1][j] == -1 && a[i][j] == -1)
                {
                    if (count == 5) score += 1000000;
                    else score += pow(10, count) * weight;
                }
                else if (a[i][j] != -1 && a[i - count - 1][j] != -1)
                {
                    if (count == 5) score += 1000000;
                }
                else
                {
                    if (count == 5) score += 1000000;
                    else score += pow(10, count - 1) * weight;
                }
                count = 0;
            }
            else count = 0;
        }
    count = 0;

    for (int i = -14;i <= 14;++i)
    {
        int j = i;
        int k = 14;
        for (;j < 15;++j, --k)
        {
            if (!outofboard(j, k))
            {
                if (a[j][k] == color) count++;
                else if (count > 1)
                {
                    int weight = 0;
                    for (int m = 1;m <= count;++m)
                    {
                        weight += positionweight[j - m][k + m];
                    }
                    weight /= count;

                    if (a[j - count - 1][k + count + 1] == -1 && a[j][k] == -1)
                    {
                        if (count == 5) score += 1000000;
                        else score += pow(10, count) * weight;
                    }
                    else if (a[j - count - 1][k + count + 1] != -1 && a[j][k] != -1)
                    {
                        if (count == 5) score += 1000000;
                    }
                    else
                    {
                        if (count == 5) score += 1000000;
                        else score += pow(10, count - 1) * weight;
                    }
                    count = 0;
                }
                else count = 0;
            }
        }
    }
    count = 0;

    for (int i = -14;i <= 14;++i)
    {
        int j = i;
        int k = 0;
        for (;j < 15;++j, ++k)
        {
            if (!outofboard(j, k))
            {
                if (a[j][k] == color) count++;
                else if (count > 1)
                {
                    int weight = 0;
                    for (int m = 1;m <= count;++m)
                    {
                        weight += positionweight[j - m][k - m];
                    }
                    weight /= count;

                    if (a[j - count - 1][k - count - 1] == -1 && a[j][k] == -1)
                    {
                        if (count == 5) score += 1000000;
                        else score += pow(10, count) * weight;
                    }
                    else if (a[j - count - 1][k - count - 1] != -1 && a[j][k] != -1)
                    {
                        if (count == 5) score += 1000000;
                    }
                    else
                    {
                        if (count == 5) score += 1000000;
                        else score += pow(10, count - 1) * weight;
                    }
                    count = 0;
                }
                else count = 0;
            }
        }
    }

    if (flag)
    {
        return score - score_cal(a, 1 - color, false);
    }

    return score;

}


struct tree
{
    int x, y, alpha, beta;
    int w;
};

int min_max(int a[15][15], int deep, int color, int ALPHA, int BETA)
{
    int b[15][15];
    tree t;
    t.w = INT_MIN;
    t.alpha = ALPHA;
    t.beta = BETA;
    int minmax;
    bool flag = false;

    if (deep > 0)
    {
        for (int i = 0;i < 15;++i)
            for (int j = 0;j < 15;++j)
            {
                if (t.alpha < t.beta && search_place(a, i, j))
                {
                    memcpy(b, a, 15 * 15 * sizeof(int));
                    b[i][j] = color;
                    minmax = min_max(b, deep - 1, 1 - color, t.alpha, t.beta);
                    if (deep % 2 == 0)
                    {
                        if (!flag)
                        {
                            t.w = minmax;
                            flag = true;
                        }
                        else
                        {
                            if (t.alpha < minmax)
                            {
                                t.w = t.alpha = minmax;
                                t.x = i;
                                t.y = j;
                            }
                        }
                    }
                    else
                    {
                        if (!flag)
                        {
                            t.w = minmax;
                            flag = true;
                        }
                        else
                        {
                            if (t.beta > minmax)
                            {
                                t.w = t.beta = minmax;
                                t.x = i;
                                t.y = j;
                            }
                        }
                    }
                }

            }
        x_place = t.x;
        y_place = t.y;
        return t.w;
    }
    else
    {
        return score_cal(a, color, true);
    }
}

//init function is called once at the beginning
void init() 
{
	memset(board, -1, sizeof(board));
}

// loc is the action of your opponent
// Initially, loc being (-1,-1) means it's your first move
// If this is the third step(with 2 black ), where you can use the swap rule, your output could be either (-1, -1) to indicate that you choose a swap, or a coordinate (x,y) as normal.

std::pair<int, int> getRandom() 
{
    while (true) 
    {
        int x = rand() % 15;
        int y = rand() % 15;
        if (board[x][y] == -1) 
        {
            board[x][y] = ai_side;
            return std::make_pair(x, y);
        }
    }
}


std::pair<int, int> choose_next()
{
    static bool flag = false;
    if (!flag)
    {
        for (int i = 0;i < 15;++i)
        {
            for (int j = 0;j < 15;++j)
            {
                if (board[i][j] != -1)
                {
                    flag = true;
                    break;
                }
            }
            if (flag) break;
        }
        if (!flag)
        {
            flag = true;
            return std::make_pair(7, 7);
        }
    }

    min_max(board, DEPTH, ai_side, INT_MIN, INT_MAX);
    board[x_place][y_place] = ai_side;
    return std::make_pair(x_place, y_place);
}


std::pair<int, int> action(std::pair<int, int> loc) 
{
	/* TODO: Replace this by your code */
	/* This is now a random strategy */
    turn++;
    board[loc.first][loc.second] = 1 - ai_side;
    if (turn == 2 && ai_side == 1) 
    {
        return std::make_pair(-1, -1);
    }
    else 
    {
        return choose_next();
    }
}