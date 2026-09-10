#include <compare>

class Time {
    double jdInt_;
    double jdFrac_;

public:
    Time(double jd1 = 0, jd2 = 0) noexcept;
    
    static Time fromJD (double jd) noexcept;
    static Time fromMJD (double mjd) noexcept;

    double jdInt () const noexcept;
    double jdPart () const noexcept;
    double jd () const noexcept;
    double mjd () const noexcept;

    auto operator <=>(const Time& other) const noexcept = default;

    friend double operator -(const Time& first, const Time& second) noexcept;
    friend Time operator -(const Time& first, double secs) noexcept;
    friend Time operator +(const Time& first, double secs) noexcept;

};