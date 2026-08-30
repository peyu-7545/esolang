#include <bits/stdc++.h>
using namespace std;

// 乱数生成するための準備
random_device seed_gen;
mt19937 engine(seed_gen());
uniform_int_distribution<int> distribution(0, 3);

struct PairHash {
    template <class T1, class T2>
    size_t operator()(const pair<T1, T2>& p) const {
        // 簡易的なハッシュ結合の例（Boostなどのハッシュ結合を模倣）
        auto h1 = hash<T1>{}(p.first);
        auto h2 = hash<T2>{}(p.second);
        return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h2 >> 2));
    }
};

struct Stack {
	deque<double> data;
	double reg = NAN; // register
	bool isReversed = false; // 反転したか

	Stack() {}
	Stack(const vector<double>& initElement) : data(initElement.begin(), initElement.end()) {}

	void push(double x) {
		if (isReversed) {
			data.push_front(x);
		} else {
			data.push_back(x);
		}
	}

	double pop() {
		if (data.empty()) throw runtime_error("stackに要素がない状態でpopした");
		double val;

		if (isReversed) {
			val = data.front();
			data.pop_front();
		} else {
			val = data.back();
			data.pop_back();
		}

		return val;
	}

	double top() {
		if (data.empty()) throw runtime_error("stackに要素がない状態でtopを取得した");

		if (isReversed) {
			return data.front();
		} else {
			return data.back();
		}
	}

	void rotate(bool isRight) {
		if (isRight ^ isReversed) {
			// どちらか一方がtrueのとき
			// すなわち、右回転で反転していない or 左回転で反転=右回転
			data.push_front(data.back());
			data.pop_back();
		} else {
			// 左回転
			data.push_back(data.front());
			data.pop_front();
		}
	}

	void reverse() { isReversed = !isReversed; }

	void swapTopAndRegister() {
		if (isnan(reg)) {
			// top -> reg
			reg = pop();
		} else {
			// reg -> top
			push(reg);
			reg = NAN;
		}
	}

	size_t size() {
		return data.size();
	}

	// 末尾のx個の要素をdstの末尾に移動させる
	void moveBackElements(Stack& dst, int x) {
	}
};

struct Pointer2D {
	int i, j, di, dj;

	Pointer2D() : i(0), j(0), di(0), dj(1) {}

	void jump(int newI, int newJ) {
		i = newI;
		j = newJ;
	}

	void setDirection(int newDi, int newDj) {
		di = newDi;
		dj = newDj;
	}

	void setRandomDirection() {
		static array<int, 5> directions = {0, 1, 0, -1, 0};
		int randIndex = distribution(engine);
		setDirection(directions[randIndex], directions[randIndex + 1]);
	}
	
	void advancePointer(int width, int height) {
		i += di;
		j += dj;

		// 箱の外へ行った場合はクランプ
		if (i < 0) i += height;
		if (height <= i) i -= height;
		if (j < 0) j += width;
		if (width <= j) j -= width;
	}
};

struct FishRuntime {
	unordered_map<pair<int, int>, char, PairHash> codeBox; // プログラム(座標から命令への写像)
	vector<Stack> stacks; // データを格納するStackの配列
	Pointer2D ip; // 命令ポインタ
	int width, height; // codeBoxの範囲
	char stringMode = ' '; // ' か " のとき、それがもう一度来るまで命令を文字としてpushする
	istream& ist; // 入力を受け取るストリーム
	ostream& ost; // 出力を書き込むストリーム

	// コンストラクタ
	// const string& code: 実行するプログラム
	// const vector<double>& initialStack: スタックの初期状態の要素
	// istream& ist: 入力を受け取るストリーム(デフォルトでは)
	// ostream& ost: 出力を書き込むストリーム
	FishRuntime(const string& code, const vector<double>& initialStack = {}, istream& ist = cin, ostream& ost = cout) 
		: stacks(1, Stack(initialStack)), ist(ist), ost(ost) {
		int i = 0, j = 0;
		int maxWidth = 0;

		for (char c : code) {
			if (c == '\n') {
				if (j > maxWidth) maxWidth = j;
				i++;
				j = 0;
			} else {
				codeBox[{i, j}] = c;
				j++;
			}
		}

		width = maxWidth;
		height = i;

		cout << setprecision(numeric_limits<double>::max_digits10);
	}

	void run() {
		while (step());
	}

	bool step() {

		char instr = getInstruction(ip.i, ip.j);

		cerr << "命令: " << instr << " -> ";

		bool isRunning = true;

		if (stringMode != ' ') {
			if (instr == stringMode) {
				// 閉じクォートが来たので終了
				stringMode = ' ';
			} else {
				// 命令を文字としてpushする
				stacks.back().push(instr);
			}
		} else {
			// 命令を実行
			isRunning = doInstruction(instr);
		}

		for (int i = 0; i < stacks.back().data.size(); i++) {
			int op;
			if (stacks.back().isReversed) {
				op = stacks.back().data[stacks.back().data.size() - 1 - i];
			} else {
				op = stacks.back().data[i];
			}
			cerr << op << " ";
		}

		cerr << endl;

		ip.advancePointer(width, height);

		return isRunning;
	}

	bool doInstruction(char instr) {
		try {

			double x, y, z;
			
			switch (instr) {

				// 方向を強制
				case '>': ip.setDirection(0, 1); break; // 右を向く
				case 'v': ip.setDirection(1, 0); break; // 下を向く
				case '<': ip.setDirection(0, -1); break; // 左を向く
				case '^': ip.setDirection(-1, 0); break; // 上を向く

				// 方向転換
				case '\\': ip.setDirection(ip.dj, ip.di); break; // i = j で鏡映
				case '/': ip.setDirection(-ip.dj, -ip.di); break; // i = -j で鏡映
				case '|': ip.setDirection(ip.di, -ip.dj); break; // 水平方向だけ反転
				case '_': ip.setDirection(-ip.di, ip.dj); break; // 垂直方向だけ反転
				case '#': ip.setDirection(-ip.di, -ip.dj); break; // どの向きでも反転
				case 'x': ip.setRandomDirection(); break; // ランダムな方向に反転

				// スキップ
				case '!': ip.advancePointer(width, height); break; // 次の命令をスキップ
				case '?': if (stacks.back().pop() == 0) ip.advancePointer(width, height); break; // popして0なら次の命令をスキップ

				// ジャンプ
				case '.': x = stacks.back().pop(), y = stacks.back().pop(); ip.jump(x, y); break; // yとxをpopして(x, y)へジャンプ
				
				// 数値
				case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
					stacks.back().push(instr - '0'); break; // 16進数値と解釈してpush
				case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
					stacks.back().push(instr - 'a' + 10); break; // 16進数値と解釈してpush

				// 演算
				case '+': y = stacks.back().pop(), x = stacks.back().pop(); stacks.back().push(x + y); break; // yとxをpopしてx+yをpush
				case '-': y = stacks.back().pop(), x = stacks.back().pop(); stacks.back().push(x - y); break; // x-yをpush
				case '*': y = stacks.back().pop(), x = stacks.back().pop(); stacks.back().push(x * y); break; // x*yをpush
				case ',': y = stacks.back().pop(), x = stacks.back().pop(); if (y == 0.0) throw runtime_error("0で割った"); stacks.back().push(x / y); break; // x/yをpush
				case '%': y = stacks.back().pop(), x = stacks.back().pop(); if (y == 0.0) throw runtime_error("0で割った余りを求めた"); stacks.back().push(fmod(x, y)); break; // x%yをpush
				case '=': y = stacks.back().pop(), x = stacks.back().pop(); stacks.back().push(x == y); break; // x==yをpush
				case ')': y = stacks.back().pop(), x = stacks.back().pop(); stacks.back().push(x > y); break; // x>yをpush
				case '(': y = stacks.back().pop(), x = stacks.back().pop(); stacks.back().push(x < y); break; // x<yをpush

				// 文字列
				case '\'': case '"': stringMode = instr; break; // 閉じクォートが来るまで命令をpush

				// stack
				case ':': stacks.back().push(stacks.back().top()); break; // topを複製
				case '~': stacks.back().pop(); break; // pop
				case '$': y = stacks.back().pop(), x = stacks.back().pop(); stacks.back().push(y); stacks.back().push(x); break; // 上位2個を反転
				case '@': z = stacks.back().pop(), y = stacks.back().pop(), x = stacks.back().pop(); stacks.back().push(z); stacks.back().push(x); stacks.back().push(y); break; // 上位3個を上方向にrotate
				case '}': stacks.back().rotate(true); break; // 全体を上方向にrotate
				case '{': stacks.back().rotate(false); break; // 全体を下方向にrotate
				case 'r': stacks.back().reverse(); break; // reverse
				case 'l': stacks.back().push(stacks.back().size()); break; // sizeをpush
				case '[': addStackAndMove(stacks.back().pop()); break; // 新しいスタックを作ってpop()個の値を移動
				case ']': removeStackAndMove(); break; // 現在のスタックを削除し、値を全て前のスタックのtopに移動

				// io
				case 'o': outputAsChar(stacks.back().pop()); break; // popし、文字として出力
				case 'n': outputAsNum(stacks.back().pop()); break; // popし、値として出力
				case 'i': stacks.back().push(input()); break; // 一文字読んでpush.ないときは-1をpush

				// register
				case '&': stacks.back().swapTopAndRegister(); break; // registerが空ならtopからregisterに、そうでないならregisterからtopに移す

				// codeBox
				case 'g': y = stacks.back().pop(), x = stacks.back().pop(); stacks.back().push(instructionToNum(getInstruction(y, x))); break; // 
				case 'p': y = stacks.back().pop(), x = stacks.back().pop(); setInstruction(y, x, stacks.back().pop()); break; //

				// halt
				case ';': return false; // プログラムを終了
				
				// nop
				case ' ': break; // 何もしない

				default: throw runtime_error("無効な命令が呼び出された"); // 無効な命令
			}

			return true;

		} catch (const exception& e) {
			cout << "something smell fishy...(" << e.what() << ")" << endl;
			return false;
		}
	}

	// stackを追加してx個の要素を移動
	void addStackAndMove(int x) {

		if (x == 0) return;
		if (stacks.size() == 0 || stacks.back().size() < x) throw runtime_error("移動する要素が存在しない");

		stacks.emplace_back();

		auto& src = stacks[stacks.size() - 2].data;
		auto& dst = stacks[stacks.size() - 1].data;

		// srcからx個の要素を順序を保ってdstに移動

		bool dstReversed = stacks[stacks.size() - 1].isReversed;
		auto it = dstReversed ? dst.begin() : dst.end();

		if (stacks[stacks.size() - 2].isReversed) {
			dst.insert(it, make_move_iterator(src.rend() - x), make_move_iterator(src.rend()));
			src.erase(src.begin(), src.begin() + x);
		} else {
			dst.insert(it, make_move_iterator(src.end() - x), make_move_iterator(src.end()));
			src.erase(src.end() - x, src.end());
		}
	}

	// stackを削除して要素を下位に移動
	void removeStackAndMove() {
		if (stacks.empty()) throw runtime_error("消すstackが存在しない");

		if (stacks.size() > 1) {
			// 下位stackに移動させる
			auto& src = stacks[stacks.size() - 1].data;
			auto& dst = stacks[stacks.size() - 2].data;

			bool dstReversed = stacks[stacks.size() - 2].isReversed;
			auto it = dstReversed ? dst.begin() : dst.end();

			if (stacks[stacks.size() - 1].isReversed) {
				dst.insert(it, make_move_iterator(src.rbegin()), make_move_iterator(src.rend()));
				src.erase(src.begin(), src.end());
			} else {
				dst.insert(it, make_move_iterator(src.begin()), make_move_iterator(src.end()));
				src.erase(src.begin(), src.end());
			}
		}

		stacks.pop_back();
	}

	// 文字/数値として出力に書き込む
	void outputAsChar(double val) { ost << char(val); }
	void outputAsNum(double val) { ost << val; }

	// 入力から一文字読んでその文字コードを返す(読めないときは-1)
	double input() {
		char c;
		return ist.get(c) ? c : -1;
	}

	// 座標(i, j)にある命令を取得する
	char getInstruction(int i, int j) const {
		auto it = codeBox.find({i, j});
		return it == codeBox.end() ? ' ' : it->second;
	}

	// 指定した座標の命令を書き換える
	void setInstruction(int i, int j, char newInstr) {
		codeBox[{i, j}] = newInstr;

		// 箱のサイズを変更
		if (i > height) height = i;
		if (j > width) width = j;
	}

	double instructionToNum(char instr) {
		return instr == ' ' ? 0 : instr;
	}
};

int main() {
	string code, str;

	while (getline(cin, str)) {
		code += str + "\n";
	}

	stringstream ist("1145140/10=");
	stringstream ost;

	FishRuntime fish(code, {}, ist, ost);

	fish.run();

	cout << ost.str() << endl;
}