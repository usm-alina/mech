#include <compare>

class Time {
public:
    double jdInt_ = 0.0; //целые сутки!!!!
    double jdFrac_ = 0.0;
    Time(double jdInt = 0, double jdFrac = 0) noexcept
        : jdInt_(jdInt), jdFrac_(jdFrac) {}
    
    static Time fromJD (double jd) noexcept {
        double i = static_cast<long long>(jd); //целую часть выделить
        return Time(i, jd - i);
    }

    static Time fromMJD (double mjd) noexcept {
        return fromJD(mjd + 2400000.5);
    }

    double jdInt () const noexcept {return jdInt_;}
    double jdPart () const noexcept {return jdFrac_;}
    double jd () const noexcept {return jdInt_ + jdFrac_;}
    double mjd () const noexcept {return jd() - 2400000.5;}

    auto operator <=>(const Time& other) const noexcept = default;

};

double operator -(const Time& first, const Time& second) noexcept { 
        double dInt  = first.jdInt_  - second.jdInt_;
        double dFrac = first.jdFrac_ - second.jdFrac_;
        return (dInt + dFrac) * 86400.0; // в секундах!!
}

Time operator -(const Time& first, double secs) noexcept {
    double days  = secs / 86400.0;
    double dInt  = static_cast<long long>(days);
    double dFrac = days - dInt;

    Time dif;
    dif.jdInt_  = first.jdInt_  - dInt;
    dif.jdFrac_ = first.jdFrac_ - dFrac;

    if (dif.jdFrac_ < 0.0) {
        dif.jdFrac_ += 1.0;
        dif.jdInt_  -= 1.0;
    }
    return dif;
}

Time operator +(const Time& first, double secs) noexcept {
    double days  = secs / 86400.0;
    double dInt  = static_cast<long long>(days);
    double dFrac = days - dInt;

    Time sum;
    sum.jdInt_  = first.jdInt_  + dInt;
    sum.jdFrac_ = first.jdFrac_ + dFrac;

    if (sum.jdFrac_ >= 1.0) {
        sum.jdFrac_ -= 1.0;
        sum.jdInt_  += 1.0;
    }
    return sum;
}