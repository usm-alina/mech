#include <vector>
#include <cmath>
#include <exception>
#include <tuple>
#include <utility>
#include <algorithm>
#include "class_time.cpp" 

extern "C" {
#include "sofa.h"
#include "sofam.h"
}

struct DailyEOP{
    double mjd;
    double dut; 
    double xp;
    double yp;
};


class EOP {
    std::vector<DailyEOP> data_;
    double mjd0;

    std::pair<std::size_t, double> get_index(const double mjd) const
    {
        const std::size_t d = std::floor(mjd - mjd0);
        const double frac = mjd - mjd0 - d;
        return {d, frac};
    };

    public:

    // Выполняет интерполяцию dut
    double dut(double mjd) const;

    // Выполняет интерполяцию (xp, yp)
    std::tuple<double, double> pole(const double mjd) const {
        const auto [d, frac] = get_index(mjd);
        if (d + 1 >= data_.size() || mjd < mjd0) return {0.0, 0.0};

        const double xp = std::lerp(data_[d].xp, data_[d + 1].xp, frac);
        const double yp = std::lerp(data_[d].yp, data_[d + 1].yp, frac);
        return {xp, yp};
    }

};

double EOP::dut(const double mjd) const {
    const auto [d, frac] = get_index(mjd);
    if (d+1 >= data_.size() || mjd<mjd0) return 0;
    const double shift = 
        (data_[d+1].dut > data_[d].dut + 0.7 ? -1 :
        (data_[d+1].dut < data_[d].dut - 0.7 ? 1 : 0));
    return std::lerp(data_[d].dut, data_[d+1].dut + shift,
     frac);
}

template<typename EOP>
class TimeConverter{

    const EOP& eop_;
    public:

    TimeConverter(const EOP& eop) : eop_{eop} {}

    Time<Scale::TT>  utc_to_tt (const Time<Scale::UTC>& t) const {
        int iy, im, id;
        double fd;
        iauJd2cal(t.jdInt(), t.jdPart(), &iy, &im, &id, &fd);
        double dat;
        iauDat(iy, im, id, fd, &dat);
        return Time<Scale::TT>(t.jdInt(), t.jdPart() + (dat + 32.184) / DAYSEC);
    }
    Time<Scale::UTC> tt_to_utc (const Time<Scale::TT>&  t) const {
        int iy, im, id;
        double fd;
        iauJd2cal(t.jdInt(), t.jdPart(), &iy, &im, &id, &fd);
        double dat;
        iauDat(iy, im, id, fd, &dat);
        return Time<Scale::UTC>(t.jdInt(), t.jdPart() - (dat + 32.184) / DAYSEC);
    }

    Time<Scale::UT1> utc_to_ut1(const Time<Scale::UTC>& t) const {
        const double dut = eop_.dut(t.mjd());
        return Time<Scale::UT1>(t.jdInt(), t.jdPart() + dut / DAYSEC);
    }
    Time<Scale::UTC> ut1_to_utc(const Time<Scale::UT1>& t) const {
        const double dut = eop_.dut(t.mjd());
        return Time<Scale::UTC>(t.jdInt(), t.jdPart() - dut / DAYSEC);
    }

    Time<Scale::TAI> utc_to_tai(const Time<Scale::UTC>& t) const { 
        int iy, im, id;
        double fd;
        iauJd2cal(t.jdInt(), t.jdPart(), &iy, &im, &id, &fd);
        double dat;
        iauDat(iy, im, id, fd, &dat);  
        return Time<Scale::TAI>(t.jdInt(), t.jdPart() + dat / DAYSEC);
    }
    Time<Scale::UTC> tai_to_utc(const Time<Scale::TAI>& t) const { 
        int iy, im, id;
        double fd;
        iauJd2cal(t.jdInt(), t.jdPart(), &iy, &im, &id, &fd);
        double dat;
        iauDat(iy, im, id, fd, &dat);  
        return Time<Scale::UTC>(t.jdInt(), t.jdPart() - dat / DAYSEC);
    }

    Time<Scale::TT> ut1_to_tt(const Time<Scale::UT1>& t) const {return tai_to_tt(utc_to_tai(ut1_to_utc(t)));}
    Time<Scale::UT1> tt_to_ut1(const Time<Scale::TT>& t) const {return utc_to_ut1(tai_to_utc(tt_to_tai(t)));}

    Time<Scale::TAI> ut1_to_tai(const Time<Scale::UT1>& t) const {return utc_to_tai(ut1_to_utc(t));}
    Time<Scale::UT1> tai_to_ut1(const Time<Scale::TAI>& t) const {return utc_to_ut1(tai_to_utc(t));}

    Time<Scale::TAI> tt_to_tai (const Time<Scale::TT>&  t) const {return Time<Scale::TAI>(t.jdInt(), t.jdPart() - 32.184 / DAYSEC);}
    Time<Scale::TT>  tai_to_tt (const Time<Scale::TAI>& t) const {return Time<Scale::TT>(t.jdInt(), t.jdPart() + 32.184 / DAYSEC);}

    template<Scale To, Scale From>
    Time<To> convert(const Time<From>& t) const {
        if constexpr (To == Scale::TT && From == Scale::UTC) {return utc_to_tt(t);}
        else if constexpr (To == Scale::UTC && From == Scale::TT) {return tt_to_utc(t);}

        else if constexpr (To == Scale::UT1 && From == Scale::UTC) {return utc_to_ut1(t);}
        else if constexpr (To == Scale::UTC && From == Scale::UT1) {return ut1_to_utc(t);}

        else if constexpr (To == Scale::TAI && From == Scale::UTC) {return utc_to_tai(t);}
        else if constexpr (To == Scale::UTC && From == Scale::TAI) {return tai_to_utc(t);}

        else if constexpr (To == Scale::TT && From == Scale::UT1) {return ut1_to_tt(t);}
        else if constexpr (To == Scale::UT1 && From == Scale::TT) {return tt_to_ut1(t);}

        else if constexpr (To == Scale::TAI && From == Scale::UT1) {return ut1_to_tai(t);}
        else if constexpr (To == Scale::UT1 && From == Scale::TAI) {return tai_to_ut1(t);}

        else if constexpr (To == Scale::TAI && From == Scale::TT) {return tt_to_tai(t);}
        else if constexpr (To == Scale::TT && From == Scale::TAI) {return tai_to_tt(t);}

        else {static_assert(sizeof(To) == 0, "неподдерживаемый переход");}
    }
};