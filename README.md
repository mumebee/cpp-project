# cpp-project
MuKaMu's computer programming project
For windows run with ./build.bat OR g++ -std=c++17 src/server.cpp src/functions.cpp -I./libs -I./libs/nlohmann -o server.exe -lws2_32
For macOS run with ./server OR g++ -std=c++17 src/server.cpp src/functions.cpp -I./libs -I./libs/nlohmann -o server -pthread -lssl -lcrypto