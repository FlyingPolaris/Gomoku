import subprocess
import timeout_decorator
import time
import numpy as np
import sys
from flask import Flask, render_template, request, jsonify

app = Flask(__name__,static_url_path='')
# def win(id):
#     if id == 2:
#         sys.stdout.write('draw\n')
#     else:
#         sys.stdout.write('ai' + str(id) + ' wins!\n')
#     sys.exit(0)

class AI:
    def __init__(self, path, id):
        self.path = path
        if path == 'human':
            self.human = 1
        else:
            self.human = 0
        self.id = id

    def send(self, message):
        value = str(message) + '\n'
        value = bytes(value, 'UTF-8')
        self.proc.stdin.write(value)
        self.proc.stdin.flush()

    def receive(self):
        return self.proc.stdout.readline().strip().decode()

    #@timeout_decorator.timeout(seconds = 5, use_signals = True)
    def init(self):
        if self.human == 0:
            self.proc = subprocess.Popen(self.path,
                                         stdin=subprocess.PIPE,
                                         stdout=subprocess.PIPE)
            self.send(self.id)
            self.name = self.receive()

    #@timeout_decorator.timeout(seconds = 5, use_signals = True)
    def action(self, a, b):
        self.send(str(a) + ' ' + str(b))
        value = self.receive().split(' ')
        return int(value[0]), int(value[1])


class Board:
    def __init__(self):
        self.board = -np.ones((15, 15), dtype=int)

    # def show(self):
    #     for i in range(15):
    #         for j in range(15):
    #             if self.board[i][j] == -1:
    #                 sys.stdout.write('_ ')
    #             else:
    #                 sys.stdout.write(str(self.board[i][j]) + ' ')
    #         sys.stdout.write('\n')

    def action(self, side, turn, x, y):
        if turn == 2 and side == 1 and x == -1 and y == -1:
            self.board = np.where(self.board != -1, 1 - self.board, self.board)
        else:
            self.board[x][y] = side

    # def full(self):
    #     return len(np.where(self.board == -1)[0]) == 0

    # def check_win(self, side, turn, x, y):
    #     if turn == 2 and side == 1 and x == -1 and y == -1:
    #         return 0
    #     if x < 0 or x >= 15 or y < 0 or y >= 15 or self.board[x][y] != -1:
    #         return -1
    #
    #     # 8 Directions
    #     dx = [1, 1, -1, -1, 0, 0, 1, -1]
    #     dy = [1, -1, 1, -1, 1, -1, 0, 0]
    #     for xx in range(0, 15):
    #         for yy in range(0, 15):
    #
    #             for i in range(8):
    #                 curx, cury = xx, yy
    #                 flg = True
    #                 for _ in range(5):
    #                     if curx < 0 or curx >= 15 or cury < 0 or cury >= 15:
    #                         flg = False
    #                         break
    #
    #                     if self.board[curx][cury] != side and (curx != x or cury != y):
    #                         flg = False
    #                         break
    #                     curx, cury = curx + dx[i], cury + dy[i]
    #                     # print(_, curx, cury, self.board[curx][cury], side)
    #                 if flg:
    #                     return 1
    #
    #     return 0


def try_init(ai0, ai1):
    # try:
        ai0.init()
    # except:
    #     sys.stderr.write('Time out: ai0 timeout in init function\n')
    #     win(1)
    # try:
        ai1.init()
    # except:
    #     sys.stderr.write('Time out: ai1 timeout in init function\n')
    #     win(0)


# def judge():
#     board = Board()
#     ai0, ai1 = AI(sys.argv[1], 0), AI(sys.argv[2], 1)
#     try_init(ai0, ai1)
#     a, b = -1, -1
#     for turn in range(1, 15 * 15 + 1):
#         board.show()
#         if turn == 1:
#             a, b = ai0.action(-1, -1)
#         else:
#             a, b = ai0.action(a, b)
#         sys.stderr.write('ai0 take action: [' + str(a) + ' ' + str(b) + ']\n')
#         ret = board.check_win(0, turn, a, b)
#         board.action(0, turn, a, b)
#         board.show()
#         if ret == -1:
#             win(1)
#         elif ret == 1:
#             win(0)
#         elif board.full():
#             win(2)
#
#         a, b = ai1.action(a, b)
#         if turn == 2 and a == -1 and b == -1:
#             sys.stderr.write('ai1 flips the board\n')
#         else:
#             sys.stderr.write('ai1 take action: [' + str(a) + ' ' + str(b) + ']\n')
#         ret = board.check_win(1, turn, a, b)
#         board.action(1, turn, a, b)
#         if ret == -1:
#             win(0)
#         elif ret == 1:
#             win(1)
#         elif board.full():
#             win(2)
#
#     win(2)

@app.route('/')
def index():
    return render_template("gomokus.html")

@app.route('/start_game', methods=['GET','POST'])
def start_game():
    global side
    global board
    global turn
    global ai0, ai1
    global x, y
    x = -1
    y = -1
    turn = 0
    string_from_js = ""

    string_from_js = request.args['order_string']
    side = 1 - int(string_from_js)
    board = Board()
    
    a1 = ('human')
    b1 = ["./code"]
      
    ai0, ai1 = AI(a1, 0), AI(b1, 1)
    try_init(ai0, ai1)

    return "游戏开始"

@app.route('/coordinate_from_js_to_py', methods=['GET','POST'])
def coordinate_from_js_to_py():
    global side
    global x
    global y
    global board
    global ai0
    global turn
    
    turn += 1
    string_from_js = ""
    string_from_js = request.args['coordinate_string']
    coordinate = string_from_js.split('#')
    x = int(coordinate[0])
    y = int(coordinate[1])  
    print(x)
    print(y)

    board.action(side, turn, x, y)

    return "下子"

@app.route('/coordinate_from_py_to_js', methods=['GET','POST'])
def coordinate_from_py_to_js():
    global x
    global y
    global side
    global board
    global ai1

    x, y = ai1.action(x, y)
    board.action(side, turn, x, y)

    coordinate = "".join(str(x) + "#" + str(y))
    coordinate_json = {"coordinate_string": coordinate}
    return jsonify(coordinate_json)


if __name__ == '__main__':
    app.run()
    
