g++ -o ./assembler ./src/assembler_main.cpp ./src/parser.cpp ./src/lexer.cpp ./src/section.cpp ./src/symbol.cpp ./src/assembler.cpp 
g++ -o ./linker ./src/linker_main.cpp ./src/parser.cpp ./src/lexer.cpp ./src/section.cpp ./src/symbol.cpp ./src/assembler.cpp ./src/linker.cpp 
g++ -o ./emulator ./src/emulator_main.cpp ./src/emulator.cpp
