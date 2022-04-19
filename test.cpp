#include <bits/stdc++.h>

int main() {
    // bf: +++[-]>>+++

    int index = 0;
    char cells[32000];

    cells[index]++;
    cells[index]++;
    cells[index]++;

    while (cells[index]) {
        cells[index]--;
    }
    index++;
    index++;

    cells[index]++;
    cells[index]++;
    cells[index]++;

    return 0;
}