#include <iostream>
#include <unistd.h>

int main() {
    std::cout << "Crash test application started\n";
    std::cout << "Application will crash in 3 seconds...\n";

    sleep(3);

    int* ptr = nullptr;
    *ptr = 100;

    return 0;
}
