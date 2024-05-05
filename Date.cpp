#include "Date.h"

Date::Date()
{
  auto now = std::chrono::system_clock::now();
  timePoint = now;
}

Date::Date(int y, int m, int d)
{
  std::tm t = {};
  t.tm_year = y - 1900; // Year since 1900
  t.tm_mon = m - 1;     // Month of the year (0-11)
  t.tm_mday = d;        // Day of the month (1-31)
  std::time_t tt = std::mktime(&t);
  timePoint = std::chrono::system_clock::from_time_t(tt);
}

void Date::addMonths(int months)
{
  auto days = std::chrono::system_clock::to_time_t(timePoint);
  std::tm *date_tm = std::localtime(&days);

  date_tm->tm_mon += months; // Add months

  // Normalize the tm structure
  mktime(date_tm);

  // Convert back to time_point
  timePoint = std::chrono::system_clock::from_time_t(mktime(date_tm));
}

long Date::differenceInDays(const Date &other) const
{
  // auto duration = timePoint - other.timePoint;
  // return std::chrono::duration_cast<std::chrono::days>(duration).count();
  auto duration = timePoint - other.timePoint;
  return std::chrono::duration_cast<std::chrono::hours>(duration).count() / 24;
}

void addMonths(int months);

bool Date::operator>=(const Date &rhs) const
{
  return timePoint >= rhs.timePoint;
}

bool Date::operator<=(const Date &rhs) const
{
  return timePoint <= rhs.timePoint;
}

bool Date::operator==(const Date &rhs) const
{
  return timePoint == rhs.timePoint;
}

double operator-(const Date &d1, const Date &d2)
{
  int yearDiff = d1.year - d2.year;
  int monthDiff = (d1.month - d2.month);
  int dayDiff = d1.day - d2.day;
  return yearDiff + monthDiff / 12.0 + dayDiff / 365.0;
}

// std::ostream& operator<<(std::ostream& os, const Date& d)
// {
//   os << d.year << " " << d.month << " " << d.day << std::endl;
//   return os;
// }

// std::istream& operator>>(std::istream& is, Date& d)
// {
//   is >> d.year >> d.month >> d.day;
//   return is;
// }

std::ostream &operator<<(std::ostream &os, const Date &date)
{
  std::time_t t = std::chrono::system_clock::to_time_t(date.timePoint);
  std::tm *ptm = std::localtime(&t);
  os << (1900 + ptm->tm_year) << "-"
     << std::setw(2) << std::setfill('0') << (ptm->tm_mon + 1) << "-"
     << std::setw(2) << std::setfill('0') << ptm->tm_mday;
  return os;
}

std::istream &operator>>(std::istream &is, Date &date)
{
  int y, m, d;
  char delim1, delim2;
  if (is >> y >> delim1 >> m >> delim2 >> d && delim1 == '-' && delim2 == '-')
  {
    date = Date(y, m, d);
  }
  else
  {
    is.setstate(std::ios::failbit);
  }
  return is;
}
