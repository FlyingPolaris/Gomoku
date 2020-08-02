#include "AIController.h"
#include <utility>
#include <cmath>
#include <vector>
#include <cstring>
#include <climits>
#include <ctime>

typedef long long ll;
extern int ai_side; //0: black, 1: white
std::string ai_name = "polaris'_ai";

bool PRINT = 0;
int turn = 0;
int board[15][15];

void print_board()
{
    std::cout << '\n';

    for (int i = 0;i < 15;++i)
    {
        for (int j = 0;j < 15;++j)
        {
            if (board[i][j] > -1) std::cout << board[i][j] << ' ';
            else std::cout << '_' << ' ';

        }
        std::cout << '\n';
    }
    std::cout << '\n';
}

int dir[4][2] =
{
{ 0, 1 },
{ 1, 0 },
{ 1, 1 },
{ 1, -1 },
};

const int DEPTH = 4;

struct situation
{
    int live[2][6];
    int sleep[2][6];
    ll score[2];

    void init()
    {
        for (int i = 0;i <= 1;++i)
            for (int j = 0;j <= 5;++j)
                live[i][j] = sleep[i][j] = 0;
        score[0] = score[1] = 0;
    }

    void score_cal(int color)
    {
        for (int i = 0;i <= 1;++i)
        {
            score[i] = 0;
            int t = color == i ? 4 : 5;
            score[i] += live[i][1] * 10 * t;
            score[i] += live[i][2] * 100 * t;
            switch (live[i][3])
            {
            case 0: break;
            case 1: score[i] += 1000 * t; break;
            default:  score[i] += 10000 * t; break;
            }
            score[i] += live[i][4] * 200000 * t;
            score[i] += live[i][5] * 10000000 * t;
            score[i] += sleep[i][2] * 10 * t;
            score[i] += sleep[i][3] * 100 * t;
            switch (sleep[i][4])
            {
            case 0: break;
            case 1: score[i] += 2000 * t; break;
            default:  score[i] += 200000 * t; break;
            }
        }
    }
}situat;

void sit_swap()
{
    int tmp;
    for (int i = 0;i < 6;++i)
    {
        tmp = situat.live[0][i];
        situat.live[0][i] = situat.live[1][i];
        situat.live[1][i] = tmp;

        tmp = situat.sleep[0][i];
        situat.sleep[0][i] = situat.sleep[1][i];
        situat.sleep[1][i] = tmp;
    }
    tmp = situat.score[0];
    situat.score[0] = situat.score[1];
    situat.score[1] = tmp;
}


int range[4] = { 14,0,14,0 };

void range_update(int* range, int x, int y)
{
    if (x - 3 >= 0)
    {
        if (x - 3 < range[0]) range[0] = x - 3;
    }
    else range[0] = 0;

    if (x + 3 < 15)
    {
        if (x + 3 > range[1]) range[1] = x + 3;
    }
    else range[1] = 14;

    if (y - 3 >= 0)
    {
        if (y - 3 < range[2]) range[2] = y - 3;
    }
    else range[2] = 0;

    if (y + 3 < 15)
    {
        if (y + 3 > range[3]) range[3] = y + 3;
    }
    else range[3] = 14;
}


bool outofboard(int x, int y)
{
    if (x < 0 || x > 14 || y < 0 || y > 14)
        return true;
    else return false;
}

ll score_update(int x, int y, int color, situation& sit)
{
    for (int i = 0;i < 4;++i)
    {
        bool left_blocked = false, right_blocked = false;
        int num1 = 0, num2 = 0, num = 0;
        int det_x = dir[i][0], det_y = dir[i][1];
        while (!outofboard(x - det_x * (1 + num1), y - det_y * (1 + num1)))
        {
            if (board[x - det_x * (1 + num1)][y - det_y * (1 + num1)] != color) break;
            num1++;
        }

        while (!outofboard(x + det_x * (1 + num2), y + det_y * (1 + num2)))
        {
            if (board[x + det_x * (1 + num2)][y + det_y * (1 + num2)] != color) break;
            num2++;
        }
        num = num1 + num2 + 1;

        if (outofboard(x - det_x * (1 + num1), y - det_y * (1 + num1)) || board[x - det_x * (1 + num1)][y - det_y * (1 + num1)] != -1) left_blocked = true;
        if (outofboard(x + det_x * (1 + num2), y + det_y * (1 + num2)) || board[x + det_x * (1 + num2)][y + det_y * (1 + num2)] != -1) right_blocked = true;

        if (left_blocked) sit.sleep[color][num1]--;
        else sit.live[color][num1]--;
        if (right_blocked) sit.sleep[color][num2]--;
        else sit.live[color][num2]--;

        if (num >= 5) sit.live[color][5]++;
        else if (left_blocked + right_blocked == 0) sit.live[color][num]++;
        else if (left_blocked + right_blocked == 1) sit.sleep[color][num]++;

        if (!num1 && left_blocked)
        {
            while (!outofboard(x - det_x * (1 + num1), y - det_y * (1 + num1)))
            {
                if (board[x - det_x * (1 + num1)][y - det_y * (1 + num1)] != 1 - color) break;
                num1++;
            }

            if (outofboard(x - det_x * (1 + num1), y - det_y * (1 + num1)) || board[x - det_x * (1 + num1)][y - det_y * (1 + num1)] != -1) left_blocked = true;
            else left_blocked = false;

            if (!left_blocked)
            {
                sit.live[1 - color][num1]--;
                sit.sleep[1 - color][num1]++;
            }
            else
            {
                sit.sleep[1 - color][num1]--;
            }
        }

        if (!num2 && right_blocked)
        {
            while (!outofboard(x + det_x * (1 + num2), y + det_y * (1 + num2)))
            {
                if (board[x + det_x * (1 + num2)][y + det_y * (1 + num2)] != 1 - color) break;
                num2++;
            }

            if (outofboard(x + det_x * (1 + num2), y + det_y * (1 + num2)) || board[x + det_x * (1 + num2)][y + det_y * (1 + num2)] != -1) right_blocked = true;
            else right_blocked = false;

            if (!right_blocked)
            {
                sit.live[1 - color][num2]--;
                sit.sleep[1 - color][num2]++;
            }
            else
            {
                sit.sleep[1 - color][num2]--;
            }
        }
    }
    sit.score_cal(color);
    return sit.score[ai_side] - sit.score[1 - ai_side];
}

struct point
{
    int x;
    int y;
};

bool find_neibor(int x, int y, int dis)
{
    for (int i = x - dis;i <= x + dis;++i)
        for (int j = y - dis;j <= y + dis;++j)
        {
            if (!outofboard(i, j) && !(i == x && j == y))
            {
                if (board[i][j] != -1) return true;
            }
        }
    return false;
}


void gen(std::vector<point>& points, int deep, int color, int _range[4], situation& sit)
{
    point p;
    std::vector<point> enemy_five;
    std::vector<point> my_five;
    std::vector<point> enemy_four;
    std::vector<point> my_four;
    std::vector<point> enemy_doublesleepfour;
    std::vector<point> my_doublesleepfour;
    std::vector<point> enemy_threefour;
    std::vector<point> my_threefour;
    std::vector<point> enemy_doublethree;
    std::vector<point> my_doublethree;
    std::vector<point> points1;
    std::vector<point> points2;
    situation _sit;
    for (int i = _range[0];i <= _range[1];++i)
        for (int j = _range[2];j <= _range[3];++j)
        {
            if (board[i][j] == -1)
            {
                if (find_neibor(i, j, 1))
                {
                    _sit = sit;
                    score_update(i, j, 1 - color, _sit);
                    if (_sit.live[1 - color][5])
                    {
                        p.x = i;
                        p.y = j;
                        enemy_five.push_back(p);
                    }
                    else if (_sit.live[1 - color][4])
                    {
                        p.x = i;
                        p.y = j;
                        enemy_four.push_back(p);
                    }
                    else if (_sit.sleep[1 - color][4] >= 2)
                    {
                        p.x = i;
                        p.y = j;
                        enemy_doublesleepfour.push_back(p);
                    }
                    else if ( (_sit.live[1 - color][3] && _sit.sleep[1 - color][4]))
                    {
                        p.x = i;
                        p.y = j;
                        enemy_threefour.push_back(p);
                    }
                    else if ( _sit.live[1 - color][3] >= 2)
                    {
                        p.x = i;
                        p.y = j;
                        enemy_doublethree.push_back(p);
                    }

                    _sit = sit;
                    score_update(i, j, color, _sit);
                    if (_sit.live[color][5])
                    {
                        p.x = i;
                        p.y = j;
                        my_five.push_back(p);
                    }
                    else if (_sit.live[color][4])
                    {
                        p.x = i;
                        p.y = j;
                        my_four.push_back(p);
                    }
                    else if (_sit.sleep[color][4] >= 2)
                    {
                        p.x = i;
                        p.y = j;
                        my_doublesleepfour.push_back(p);
                    }
                    else if ((_sit.live[color][3] && _sit.sleep[color][4]))
                    {
                        p.x = i;
                        p.y = j;
                        my_threefour.push_back(p);
                    }
                    else if (_sit.live[color][3] >= 2)
                    {
                        p.x = i;
                        p.y = j;
                        my_doublethree.push_back(p);
                    }
                    else
                    {
                        p.x = i;
                        p.y = j;
                        points1.push_back(p);
                    }
                }
                else if (deep >= 2 && find_neibor(i, j, 2))
                {
                    p.x = i;
                    p.y = j;
                    points2.push_back(p);
                }
            }
        }

    if (!my_five.empty())
    {
        points.insert(points.end(), my_five.begin(), my_five.end());
        return;
    }
    else if (!enemy_five.empty())
    {
        points.insert(points.end(), enemy_five.begin(), enemy_five.end());
        return;
    }
    else if (!my_four.empty())
    {
        points.insert(points.end(), my_four.begin(), my_four.end());
        return;
    }
    else if (!enemy_four.empty())
    {
        points.insert(points.end(), enemy_four.begin(), enemy_four.end());
        return;
    }
    else if (!my_doublesleepfour.empty())
    {
        points.insert(points.end(), my_doublesleepfour.begin(), my_doublesleepfour.end());
        return;
    }
    else if (!enemy_doublesleepfour.empty())
    {
        points.insert(points.end(), enemy_doublesleepfour.begin(), enemy_doublesleepfour.end());
        return;
    }
    else if (!my_threefour.empty())
    {
        points.insert(points.end(), my_threefour.begin(), my_threefour.end());
        return;
    }

    else if (!enemy_threefour.empty())
    {
        points.insert(points.end(), enemy_threefour.begin(), enemy_threefour.end());
        return;
    }
    else if (!my_doublethree.empty())
    {
        points.insert(points.end(), my_doublethree.begin(), my_doublethree.end());
        return;
    }
    else if (!enemy_doublethree.empty())
    {
        points.insert(points.end(), enemy_doublethree.begin(), enemy_doublethree.end());
        return;
    }
    points.insert(points.end(), points1.begin(), points1.end());
    points.insert(points.end(), points2.begin(), points2.end());
    return;
}

ll Min(int deep, situation& _sit, ll alpha, ll beta, int _range[4]);
ll Max(int deep, situation& _sit, ll alpha, ll beta, int _range[4]);

ll Min(int deep, situation& _sit, ll alpha, ll beta,int _range[4])
{
    int rang[4];
    memcpy(rang, _range, 4 * sizeof(int));
    situation sit = _sit;
    if (deep <= 0 || sit.live[0][5] + sit.live[1][5] ) return sit.score[ai_side] - sit.score[1 - ai_side];
    ll best = INT_MAX;
    std::vector<point> points;
    gen(points, deep, 1 - ai_side, rang, sit);
    for (int i = 0;i < points.size();++i)
    {
        sit = _sit;
        point p = points[i];
        board[p.x][p.y] = 1 - ai_side;
        range_update(rang, p.x, p.y);
        score_update(p.x, p.y, 1 - ai_side, sit);
        ll v = Max(deep - 1, sit, best < alpha ? best : alpha, beta, rang);
        board[p.x][p.y] = -1;
        if (v < best) best = v;
        if (v < beta) break;
    }
    return best;
}

ll Max(int deep, situation& _sit, ll alpha, ll beta, int _range[4])
{
    int rang[4];
    memcpy(rang, _range, 4 * sizeof(int));
    situation sit = _sit;
    if (deep <= 0 || sit.live[0][5] + sit.live[1][5]) return sit.score[ai_side] - sit.score[1 - ai_side];
    ll best = INT_MIN;
    std::vector<point> points;
    gen(points, deep, ai_side, rang, sit);
    for (int i = 0;i < points.size();++i)
    {
        sit = _sit;
        point p = points[i];
        board[p.x][p.y] = ai_side;
        range_update(rang, p.x, p.y);
        score_update(p.x, p.y, ai_side, sit);
        ll v = Min(deep - 1, sit, alpha, best > beta ? best : beta, rang);
        board[p.x][p.y] = -1;
        if (v > best) best = v;
        if (alpha < v) break;
    }
    return best;
}


point maxmin(int deep)
{
    int rang[4];
    memcpy(rang, range, 4 * sizeof(int));
    situation sit = situat;
    ll best = INT_MIN;
    std::vector<point> points;
    std::vector<point> best_points;
    gen(points, deep, ai_side, range, sit);
    for (int i = 0;i < points.size();++i)
    {
        sit = situat;
        point p = points[i];
        board[p.x][p.y] = ai_side;
        range_update(rang, p.x, p.y);
        score_update(p.x, p.y, ai_side, sit);
        ll v = Min(deep - 1, sit, INT_MAX, INT_MIN, rang);
        if (v == best) best_points.push_back(p);
        else if (v > best)
        {
            best = v;
            best_points.clear();
            best_points.push_back(p);
        }
        board[p.x][p.y] = -1;
    }
    
    int t = 0;
    int r = 0;
    for (int i = 0;i < best_points.size();++i)
    {
        int p = best_points[i].x < best_points[i].y ? best_points[i].x : best_points[i].y;
        if (p > t)
        {
            t = p;
            r = i;
        }
    }
    return best_points[r];
}

void init()
{
    situat.init();
    memset(board, -1, sizeof(board));
}

bool swap_or_not()
{
   return false;
  // return true;
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
            board[0][0] = ai_side;
            range_update(range, 0, 0);
            score_update(0, 0, ai_side, situat);
            if (PRINT) print_board();
            return std::make_pair(0, 0);
        }
        else if (turn == 3)
        {
            for (int i = 0; i < 15; ++i)
                for (int j = 0; j < 15; ++j)
                    if (board[i][j] != -1)  board[i][j] = 1 - board[i][j];
            sit_swap();

            point p;
            p = maxmin(DEPTH);
            board[p.x][p.y] = ai_side;
            score_update(p.x, p.y, ai_side, situat);
            range_update(range, p.x, p.y);
            return std::make_pair(p.x, p.y);
        }
    }

    board[loc.first][loc.second] = 1 - ai_side;
    score_update(loc.first, loc.second, 1 - ai_side, situat);
    range_update(range, loc.first, loc.second);

    /*if (turn == 1)
    {
        if (board[7][7] == -1)
        {
            board[7][7] = ai_side;
            range_update(range, 7, 7);
            score_update(7, 7, ai_side, situat);
            if (PRINT) print_board();

            return std::make_pair(7, 7);
        }
        else
        {
            if (rand() % 2)
            {
                board[7][6] = ai_side;
                range_update(range, 7, 6);
                score_update(7, 6, ai_side, situat);
                if (PRINT) print_board();

                return std::make_pair(7, 6);
            }
            else
            {
                board[6][6] = ai_side;
                range_update(range, 6, 6);
                score_update(6, 6, ai_side, situat);
                if (PRINT) print_board();

                return std::make_pair(6, 6);
            }
        }
    }*/

    if (turn == 2 && ai_side == 1 && swap_or_not())
    {
        for (int i = 0; i < 15; ++i)
            for (int j = 0; j < 15; ++j)
                if (board[i][j] != -1)  board[i][j] = 1 - board[i][j];

        sit_swap();
        if (PRINT) print_board();

        return std::make_pair(-1, -1);
    }

    point p;
    p = maxmin(DEPTH);
    board[p.x][p.y] = ai_side;
    score_update(p.x, p.y, ai_side, situat);
    range_update(range, p.x, p.y);
    if(PRINT) print_board();

    return std::make_pair(p.x, p.y);
}
