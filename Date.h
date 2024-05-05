#ifndef DATE_H
#define DATE_H

#include <iostream>

class Date
{
 public:
  int year;
  int month;
  int day;
  Date(int y, int m, int d);
  Date();
  long differenceInDays(const Date& other) const;
  std::chrono::system_clock::time_point timePoint;
  // friend double operator-(const Date& d1, const Date& d2);
  // friend std::ostream& operator<<(std::ostream& os, const Date& date);
  // friend std::istream& operator>>(std::istream& is, Date& date);
  bool operator>=(const Date& rhs) const;
  bool operator<=(const Date& rhs) const;
  bool operator==(const Date& rhs) const;
};

double operator-(const Date& d1, const Date& d2);
std::ostream& operator<<(std::ostream& os, const Date& date);
std::istream& operator>>(std::istream& is, Date& date);

#endif
