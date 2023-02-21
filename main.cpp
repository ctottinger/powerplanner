#include <iostream>
#include <string>
#include "getPowerPlanNames.h"

using namespace std;

int main() {
    for(auto & plan : getPowerPlanNames())
        std::cout << plan << endl;
    return 0;
}