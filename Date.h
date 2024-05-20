#ifndef DATE_H
#define DATE_H

#include <iostream>

#include <sstream>

class Date
{
 public:
  int year;
  int month;
  int day;
  Date(int y, int m, int d) : year(y), month(m), day(d) {};
  Date() : year(0), month(0), day(0) {};


  std::string toString() const {
      std::ostringstream oss;
      oss << year << "-" << (month < 10 ? "0" : "") << month << "-" << (day < 10 ? "0" : "") << day;
      return oss.str();
  }
};

double operator-(const Date& d1, const Date& d2);
std::ostream& operator<<(std::ostream& os, const Date& date);
std::istream& operator>>(std::istream& is, Date& date);

#endif
