#include "Lexer.h"
#include <iostream>
#include <fstream>
#include <sstream>

int main(int argc, char* argv[]) {
    std::string path = "D:/Ki5_Nam3_2026/MiniLangInterpreter/tests/sample_programs/test1.txt";
    if (argc > 1) path = argv[1];

    std::ifstream file(path, std::ios::binary);  // đọc binary để tránh mọi biến đổi
    if (!file.is_open()) {
        std::cerr << "Khong the mo file: " << path << "\n";
        return 1;
    }

    // Kiểm tra kích thước file
    file.seekg(0, std::ios::end);
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::cout << "Kich thuoc file: " << size << " bytes\n";

    if (size == 0) {
        std::cerr << "File rong!\n";
        return 1;
    }

    std::string sourceCode(size, '\0');
    file.read(&sourceCode[0], size);

    std::cout << "--- BAT DAU PHAN TICH TU VUNG (DFA LEXER) ---\n";
    Lexer lexer(sourceCode);
    std::vector<Token> tokens = lexer.tokenize();

    for (const auto& tok : tokens) {
        tok.print();
    }

    if (!lexer.getErrors().empty()) {
        std::cout << "\n--- DANH SACH LOI ---\n";
        for (const auto& err : lexer.getErrors()) {
            std::cerr << err << "\n";
        }
    }
    else {
        std::cout << "\n>> Phan tich tu vung hoan tat thanh cong, khong co loi!\n";
    }

    return 0;
}