# MAC_Lab1
C++ implementation of Indeterminate Equation Public-key Cryptosystem (Giophantus TM) proposal(NIST_PQC) as a practical task for lab1 of Modern Algebraic Cryptosystems course.

main.cpp contains Giophantus OOP realization using C++20 features (ensure your compiler supports C++20 features or
revisit the compiler flags to enable C++20 [ -std=c++20 ] ).
CmakeLists.txt contains build info.
Giophantus* is an executable compiled and built using Qt creator on Manjaro Linux.

For custom build try to use next:

a) On Linux or macOS:

g++ -std=c++20 -O2 -o giophantus main.cpp

b)On Windows(using MinGW):

g++ -std=c++20 -O2 -o giophantus.exe main.cpp

To run use:
cd /path/to/giophantus
sudo chmod giophantus a+x
./giophantus
