// -*- mode: C++; c-indent-level: 4; c-basic-offset: 4; tab-width: 8 -*-
//
// RcppBDT.cpp: Rcpp and Boost Date_Time glue
//
// Copyright (C) 2010 - 2011 Dirk Eddelbuettel and Romain Francois
//
// This file is part of RcppBDT.
//
// RcppBDT is free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//                           
// RcppBDT is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with RcppBDT.  If not, see <http://www.gnu.org/licenses/>.

#include <RcppBDT.h>

// define template specialisations for as and wrap
namespace Rcpp {
    template <> boost::gregorian::date as( SEXP dtsexp ) throw(not_compatible) {
        Rcpp::Date dt(dtsexp);
        return boost::gregorian::date(dt.getYear(), dt.getMonth(), dt.getDay());
    }

    template <> SEXP wrap(const boost::gregorian::date &d) throw(not_compatible) {
        boost::gregorian::date::ymd_type ymd = d.year_month_day();     // convert to y/m/d struct
        return Rcpp::wrap(Rcpp::Date( ymd.year, ymd.month, ymd.day ));
    }
}

#if UseWithStrings
    void date_print(boost::gregorian::date *d) {
	std::cout << *d << std::endl;
    }

    // set the date d to the date from the supplied string s
    void date_fromString(boost::gregorian::date *d, std::string s) { 
	*d = boost::gregorian::date(boost::gregorian::from_string(s)); 
    }
    void date_fromUndelString(boost::gregorian::date *d, std::string s) { 
	*d = boost::gregorian::date(boost::gregorian::from_undelimited_string(s)); 
    }
    // return the date to R as an R type
    Rcpp::Date Date_fromString(boost::gregorian::date *d, std::string s) { 
	return Rcpp::wrap(boost::gregorian::date(boost::gregorian::from_string(s))); 
    }
    Rcpp::Date Date_fromUndelString(boost::gregorian::date *d, std::string s) { 
	return Rcpp::wrap(boost::gregorian::date(boost::gregorian::from_undelimited_string(s))); 
    }
#endif

// these set the date from the clock, in local or universal time
void date_localDay(boost::gregorian::date *d) { *d = boost::gregorian::date(boost::gregorian::day_clock::local_day()); }
void date_utcDay(boost::gregorian::date *d) { *d = boost::gregorian::date(boost::gregorian::day_clock::universal_day()); }
Rcpp::Date Date_localDay(boost::gregorian::date *d) { return Rcpp::wrap(boost::gregorian::date(boost::gregorian::day_clock::local_day())); }
Rcpp::Date Date_utcDay(boost::gregorian::date *d) { return Rcpp::wrap(boost::gregorian::date(boost::gregorian::day_clock::universal_day())); }

// these extract the requested date portion or representation as an integer
int date_year(boost::gregorian::date *d) { return static_cast<int>( d->year() ); }
int date_month(boost::gregorian::date *d) { return static_cast<int>( d->month() ); }
int date_day(boost::gregorian::date *d) { return static_cast<int>( d->day() ); }
int date_dayofweek(boost::gregorian::date *d) { return static_cast<int>( d->day_of_week() ); }
int date_dayofyear(boost::gregorian::date *d) { return static_cast<int>( d->day_of_year() ); }

#if UseWithStrings
    // these extract the requested date portion or representation as an integer
    std::string date_toString(boost::gregorian::date *d) { return boost::gregorian::to_simple_string(*d); }
    std::string date_toIsoString(boost::gregorian::date *d) { return boost::gregorian::to_iso_string(*d); }
    std::string date_toExtIsoString(boost::gregorian::date *d) { return boost::gregorian::to_iso_extended_string(*d); }
#endif

//Date date_toDate(date *d) { // earlier form before wrap()
//    date::ymd_type ymd = d->year_month_day();
//    return Rcpp::Date( ymd.year, ymd.month, ymd.day );
//}
Rcpp::Date date_toDate(boost::gregorian::date *d) { return Rcpp::wrap(*d); } // thanks to wrap() template above
void date_fromDate(boost::gregorian::date *d, SEXP dt) { *d = Rcpp::as<boost::gregorian::date>(dt); } // thanks to as

// construct end-of-month and first-of-next-month
void date_endOfMonth(boost::gregorian::date *d) { *d = d->end_of_month(); } // not sure why I cannot call end_of_month directly
void date_firstOfNextMonth(boost::gregorian::date *d) { *d = d->end_of_month() + boost::gregorian::days(1); }
// return end-of-month and first-of-next-month for given date
Rcpp::Date Date_endOfMonth(boost::gregorian::date *d) { return Rcpp::wrap(d->end_of_month()); }
Rcpp::Date Date_firstOfNextMonth(boost::gregorian::date *d) { 
    boost::gregorian::date dt = d->end_of_month() + boost::gregorian::days(1);
    return Rcpp::wrap(dt); 
}

static const boost::gregorian::greg_weekday friday(boost::gregorian::Friday);
void date_endOfBizWeek(boost::gregorian::date *d) { *d += days_until_weekday(*d, friday); }
Rcpp::Date Date_endOfBizWeek(boost::gregorian::date *d) { 
    boost::gregorian::date dt = *d;
    dt += days_until_weekday(*d, friday);
    return Rcpp::wrap(dt); 
}
void date_addDays(boost::gregorian::date *d, unsigned len) { *d = *d + boost::gregorian::date_duration(len); }
void date_subtractDays(boost::gregorian::date *d, unsigned len) { *d = *d - boost::gregorian::date_duration(len); }
// no Rcpp-returning functions here as we can add/substract at the R level already

// with thanks to Whit Armstong for doing this in his rboostdatetime
typedef boost::gregorian::nth_day_of_the_week_in_month nth_dow;
void date_immDate(boost::gregorian::date *d, int mon, int year) {
    nth_dow ans_generator(nth_dow::third, boost::gregorian::Wednesday, mon);
    *d = ans_generator.get_date(year);
}
Rcpp::Date Date_immDate(boost::gregorian::date *d, int mon, int year) {
    nth_dow ans_generator(nth_dow::third, boost::gregorian::Wednesday, mon);
    return Rcpp::wrap(ans_generator.get_date(year));
}

Rcpp::Date Date_nthDayOfMthWeek(boost::gregorian::date *d, int nthday, int mthweek, int mon, int year) {
    nth_dow ans_generator(static_cast<boost::date_time::nth_kday_of_month<boost::gregorian::date>::week_num>(mthweek), nthday, mon);
    return Rcpp::wrap(ans_generator.get_date(year));
}

Rcpp::Date Date_lastDayOfWeekInMonth(boost::gregorian::date *d, int weekday, int mon, int year) {
    boost::gregorian::last_day_of_the_week_in_month lwdm(weekday, mon);
    return Rcpp::wrap(lwdm.get_date(year));
}

Rcpp::Date Date_firstDayOfWeekInMonth(boost::gregorian::date *d, int weekday, int mon, int year) {
    boost::gregorian::first_day_of_the_week_in_month fwdm(weekday, mon);
    return Rcpp::wrap(fwdm.get_date(year));
}

RCPP_MODULE(bdt) {

    using namespace boost::gregorian;
    using namespace Rcpp;

    // exposing a class (boost::gregorian::)date as "date" on the R side
    class_<date>("date")
	
    // constructors
    .constructor("default constructor")
    .constructor<int, int, int>("constructor from year, month, day")

    #if UseWithStrings
        // free functions defined above with date* as first argument
        .method("fromString", &date_fromString, "create a date from a delimited string")
        .method("fromUndelString", &date_fromUndelString, "create a date from an un delimited string")
        .method("getDateFromString", &Date_fromString, "return a date from a delimited string")
        .method("getDateFromUndelString", &Date_fromUndelString, "return a date from an un delimited string")
    #endif

    .method("setFromLocalClock", &date_localDay, "create a date from current local clock")
    .method("setFromUTC", &date_utcDay, "create a date from current universal clock")
    .method("getLocalClock", &Date_localDay, "return a date from local time")
    .method("getUTC", &Date_utcDay, "return a date from UTC time")

    //.method("print", &date_print, "print date to stdout")

    .method("getYear", &date_year, "returns the year")
    .method("getMonth", &date_month, "returns the month")
    .method("getDay", &date_day, "returns the day")
    .method("getDayOfWeek", &date_dayofweek, "returns the day of the week")
    .method("getDayOfYear", &date_dayofyear, "returns the day of the year")

    #if UseWithStrings
        .method("getString", &date_toString, "returns a string representation")
        .method("getIsoString", &date_toIsoString, "returns an ISO string representation")
        .method("getExtIsoString", &date_toExtIsoString, "returns an extended ISO string representation")
    #endif

    .method("getDate", &date_toDate, "returns an R Date object")
    .method("fromDate", &date_fromDate, "sets date from an R Date object")

    // member functions from the boost date class
    // -- does not work as there is another class in between  .method("year", &boost::gregorian::date::year)
    //                                                        .method("month", &date::month)
    .const_method("getWeekNumber", &date::week_number, "returns the number of the week")
    .const_method("getModJulian", &date::modjulian_day, "returns the modified Julian day")
    .const_method("getJulian", &date::julian_day, "returns the Julian day")

    .method("setEndOfMonth", &date_endOfMonth, "return the last day of the month for given date")
    .method("setFirstOfNextMonth", &date_firstOfNextMonth, "return the first day of the next month for given date")
    .method("getEndOfMonth", &Date_endOfMonth, "return the last day of the month for given date")
    .method("getFirstOfNextMonth", &Date_firstOfNextMonth, "return the first day of the next month for given date")

    .method("setEndOfBizWeek", &date_endOfBizWeek, "return the Friday of the week for given date")
    .method("getEndOfBizWeek", &Date_endOfBizWeek, "return the Friday of the week for given date")

    .method("addDays", &date_addDays, "adds days to given date")
    .method("subtractDays", &date_subtractDays, "subtract days from given date")

    .method("setIMMDate", &date_immDate, "sets third Wednesday in given month and year")
    .method("getIMMDate", &Date_immDate, "return third Wednesday in given month and year")

    .method("getNthDayMthWeek", &Date_nthDayOfMthWeek, "return nth week's given day-of-week in given month and year")

    .method("getLastDayOfWeekInMonth", &Date_lastDayOfWeekInMonth, "return date of last day-of-week in given month and year")
    .method("getFirstDayOfWeekInMonth", &Date_firstDayOfWeekInMonth, "return date of last day-of-week in given month and year")

    ;

}
