#ifndef DATA_H_
#define DATA_H_

//#include <ctypes.h>
#include <string>
#include <algorithm>

#include "../../base/copyable.h"

namespace ws{

class Date : public Copyable
          // public boost::less_than_comparable<Date>,
          // public boost::equality_comparable<Date>
{
 public:

  struct YearMonthDay
  {
    int year; // [1900..2500]
    int month;  // [1..12]
    int day;  // [1..31]
  };

  static constexpr const int kDaysPerWeek = 7;
  static const int kJulianDayOf1970_01_01;

  ///
  /// Constucts an invalid Date.
  ///
  Date()
    : julianDayNumber_(0)
  {}

  ///
  /// Constucts a yyyy-mm-dd Date.
  ///
  /// 1 <= month <= 12
  Date(int year, int month, int day);

  ///
  /// Constucts a Date from Julian Day Number.
  ///
  explicit Date(int julianDayNum)
    : julianDayNumber_(julianDayNum)
  {}

  ///
  /// Constucts a Date from struct tm
  ///
  explicit Date(const struct tm&);

  // default copy/assignment/dtor are Okay

  void swap(Date& that) noexcept
  {
    std::swap(julianDayNumber_, that.julianDayNumber_);
  }

  bool valid() const noexcept { return julianDayNumber_ > 0; }

  ///
  /// Converts to yyyy-mm-dd format.
  ///
  std::string toIsoString() const;

  struct YearMonthDay yearMonthDay() const;

  int year() const noexcept
  {
    return yearMonthDay().year;
  }

  int month() const noexcept
  {
    return yearMonthDay().month;
  }

  int day() const noexcept
  {
    return yearMonthDay().day;
  }

  // [0, 1, ..., 6] => [Sunday, Monday, ..., Saturday ]
  int weekDay() const noexcept
  {
    return (julianDayNumber_+1) % kDaysPerWeek;
  }

  int julianDayNumber() const noexcept { return julianDayNumber_; }

 private:
  int julianDayNumber_;
};

inline bool operator<(Date x, Date y)
{
  return x.julianDayNumber() < y.julianDayNumber();
}

inline bool operator==(Date x, Date y)
{
  return x.julianDayNumber() == y.julianDayNumber();
}

};

#endif //DATA_H_