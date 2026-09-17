#include <compare>
#include <cmath>

class Time {
    double jdInt_ = 0.0; //целые сутки!!!!
    double jdFrac_ = 0.0;
public:
    Time(double jdInt = 0, double jdFrac = 0) noexcept
        : jdInt_(jdInt), jdFrac_(jdFrac) 
    {
        double extra = std::floor(jdFrac_);
        jdInt_  += extra;
        jdFrac_ -= extra;

        if (jdFrac_ < 0.0) {
            jdFrac_ += 1.0;
            jdInt_  -= 1.0;
        }
    }
    
    static Time fromJD (double jd) noexcept {
        double i = std::floor(jd);
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

double operator-(const Time& a, const Time& b) noexcept {
    return (a.jd() - b.jd()) * 86400.0;
}

Time operator-(const Time& t, double secs) noexcept {
    double days = secs / 86400.0;
    return Time(t.jdInt(), t.jdPart() - days);
}

Time operator+(const Time& t, double secs) noexcept {
    double days = secs / 86400.0;
    return Time(t.jdInt(), t.jdPart() + days);
}