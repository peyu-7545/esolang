#include <iostream>
#include <fstream>
#include <string>
#include <cstdint>
#include <unordered_map>
#include <stack>
#include <cstdlib>
#include <format>

void run(const std::string &program) {
	uint8_t data[30000];
	int instPtr = 0, dataPtr = 0;

	std::unordered_map<int, int> brackets;
	std::stack<int> st;

	bool match = true;

	for (int i = 0; i < program.size(); i++) {
		if (program[i] == '[') {
			st.push(i);
		} else if (program[i] == ']') {
			if (st.empty()) {
				match = false;
				break;
			}
			brackets[i] = st.top();
			brackets[st.top()] = i;
			st.pop();
		}
	}

	if (!match || !st.empty()) {
		std::cerr << "\x1b[1;31m" << "brackets do not match" << "\x1b[0m" << std::endl;
		exit(1);
	}

	while (instPtr < program.size()) {

		char inst = program[instPtr];

		switch (inst) {
			case '>': dataPtr++; break;
			case '<': dataPtr--; break;
			case '+': data[dataPtr]++; break;
			case '-': data[dataPtr]--; break;
			case '.': std::cout << char(data[dataPtr]); break;
			case ',': data[dataPtr] = std::cin.get(); break;
			case '[': if (data[dataPtr] == 0) instPtr = brackets[instPtr]; break;
			case ']': if (data[dataPtr] != 0) instPtr = brackets[instPtr]; break;
			case '%':
				for (int i = 0; i < 8; i++) {
					std::cerr << std::format("{:02x}", data[i]) << ' ';
				}
				std::cerr << std::endl;
				break;
		}

		instPtr++;
	}
}

int main(int argc, char *argv[]) {

	if (argc < 2) {
		std::cerr << "\x1b[1;31m" << "no input brainfuck file" << "\x1b[0m" << std::endl;
		exit(1);
	}

	std::ifstream ifs(argv[1]);

	if (!ifs) {
		std::cerr << "\x1b[1;31m" << argv[1] << ": file not found" << "\x1b[0m" << std::endl;
		exit(1);
	}

	std::string program, str;
	while (getline(ifs, str)) {
		program += str + '\n';
	}

	run(program);
}