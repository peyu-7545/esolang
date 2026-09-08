#include <bits/stdc++.h>
using namespace std;

void runBF(const string& program, istream& ist, ostream& ost) {
	vector<uint8_t> data(30000, 0);
	int instPtr = 0, dataPtr = 0;

	unordered_map<int, int> brackets;
	stack<int> st;
	for (int i = 0; i < program.size(); i++) {
		if (program[i] == '[') {
			st.push(i);
		} else if (program[i] == ']') {
			brackets[i] = st.top();
			brackets[st.top()] = i;
			st.pop();
		}
	}

	while (instPtr < program.size()) {
		char inst = program[instPtr];
		// cerr << inst << endl;
		switch (inst) {
			case '>': dataPtr++; break;
			case '<': dataPtr--; break;
			case '+': data[dataPtr]++; break;
			case '-': data[dataPtr]--; break;
			case '.': ost << char(data[dataPtr]); break;
			case ',': data[dataPtr] = ist.get(); break;
			case '[': if (data[dataPtr] == 0) instPtr = brackets[instPtr]; break;
			case ']': if (data[dataPtr] != 0) instPtr = brackets[instPtr]; break;
			default: instPtr++; continue;
		}

		instPtr++;

		// for (int i = 0; i < 20; i++) cerr << (int)data[i] << " ";
		// cerr << endl;
	}
}

struct BFRuntime {
	const string program;
	vector<uint8_t> data;
	istream& ist;

	BFRuntime(const string& program, istream& ist) : program(program), data(30000), ist(ist) {}

	void run() {

	}
};

int main(int argc, char* argv[]) {

	if (argc < 2) {
		cerr << ".bfファイルを指定してください" << endl;
		return 1;
	}

	ifstream ifs(argv[1]);

	if (!ifs) {
		cerr << "ファイルを開けませんでした" << endl;
		return 1;
	}

	string code, str;
	while (getline(ifs, str)) {
		code += str + "\n";
	}

	// 入力
	stringstream ist(2 <  argc ? argv[2] : "");

	// cerr << ist.str() << endl;

	runBF(code, ist, cout);
}