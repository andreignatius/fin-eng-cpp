#include <iostream>
#include "Date.h"

void testDefaultConstructor() {
    Date defaultDate;
    std::cout << "Default Constructor Test: " << defaultDate << std::endl; // !!!
}

void testParameterizedConstructor() {
    Date date1(2024, 6, 6);
    Date date2(2025, 12, 31);
    Date date3(2000, 1, 1);
    std::cout << "Parameterized Constructor Test 1: " << date1 << std::endl; // !!!
    std::cout << "Parameterized Constructor Test 2: " << date2 << std::endl; // !!!
    std::cout << "Parameterized Constructor Test 3: " << date3 << std::endl; // !!!
}

void testDateComparison() {
    Date date1(2024, 6, 6);
    Date date2(2025, 12, 31);
    Date date3(2024, 6, 6);

    std::cout << "Comparison Test: " << std::endl;
    std::cout << "date1 >= date2: " << (date1 >= date2) << std::endl; // !!!
    std::cout << "date1 <= date2: " << (date1 <= date2) << std::endl; // !!!
    std::cout << "date1 == date3: " << (date1 == date3) << std::endl; // !!!
}

void testDateDifference() {
    Date date1(2024, 6, 6);
    Date date2(2025, 6, 6);
    std::cout << "Difference in Days Test: " << date2.differenceInDays(date1) << " days" << std::endl; // !!!
}

int main() {
    testDefaultConstructor(); // !!!
    testParameterizedConstructor(); // !!!
    testDateComparison(); // !!!
    testDateDifference(); // !!!
    return 0;
}
