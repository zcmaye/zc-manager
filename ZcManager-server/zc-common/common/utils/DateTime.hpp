#pragma once

namespace zc::dt::detail {
	template<typename ...Args>
	std::string format(std::string_view fmt, Args&& ...args) {
		return std::vformat(fmt, std::make_format_args(std::forward<Args>(args)...));
	}
}

/** 日期类 */
class Date {
public:
	Date() :Date(0, 0, 0) {}
	Date(std::chrono::year_month_day date)
		:_ymd(date) {
	}
	Date(int year, int month, int day)
		:_ymd(std::chrono::year{ year }, std::chrono::month{ (unsigned int)month }, std::chrono::day{ (unsigned int)day })
	{
	}

	Date addDays(int64_t ndays) const {
		return Date(std::chrono::year_month_day(std::chrono::sys_days(_ymd) + std::chrono::days(ndays)));
	}
	Date addDuration(std::chrono::days ndays) const {
		return Date(std::chrono::year_month_day(std::chrono::sys_days(_ymd) + ndays));
	}
	Date addMonths(int nmonths) const {
		return Date(_ymd + std::chrono::months(nmonths));
	}
	Date addYears(int nyears) const {
		return Date(_ymd + std::chrono::years(nyears));
	}

	int year()const { return static_cast<int>(_ymd.year()); }
	int month()const { return static_cast<unsigned int>(_ymd.month()); }
	int day()const { return static_cast<unsigned int>(_ymd.day()); }
	std::chrono::sys_days toSysDays()const {
		return std::chrono::sys_days{ _ymd };
	}
	std::chrono::year_month_day toYearMonthDay()const {
		return _ymd;
	}
	operator std::chrono::year_month_day()const {
		return _ymd;
	}

	/**
	 * 返回星期几.
	 */
	int dayOfWeek() const {
		std::chrono::weekday wd(std::chrono::sys_days{ _ymd });
		return wd.iso_encoding();
	}
	/**
	 * 返回当前日期是本年中的第几天.
	 */
	int dayOfYear() const {
		auto start_of_year = std::chrono::year_month_day{ _ymd.year(),std::chrono::month(1),std::chrono::day(1) };
		auto day_of_year = (toSysDays() - std::chrono::sys_days{ start_of_year }).count() + 1;
		return day_of_year;
	}

	/**
	 * 返回本月有多少天.
	 */
	int daysInMonth() const {
		auto ymdl = std::chrono::year_month_day_last{ _ymd.year(),std::chrono::month_day_last{_ymd.month()} };
		return (unsigned int)ymdl.day();
	}
	/**
	 * 返回本年有多少天.
	 */
	int daysInYear() const {
		auto start_of_year = std::chrono::sys_days{ std::chrono::year_month_day{_ymd.year(),std::chrono::month(1),std::chrono::day(1) } };
		auto start_of_next_year = std::chrono::sys_days{ std::chrono::year_month_day{_ymd.year() + std::chrono::years{1},std::chrono::month(1),std::chrono::day(1)} };
		return (start_of_next_year - start_of_year).count();
	}
	/**
	 * 返回两个日期时间相差的天数(d - this).
	 */
	int64_t daysTo(Date d) const {
		return (d.toSysDays() - toSysDays()).count();
	}

	void setDate(int year, int month, int day) {
		_ymd = std::chrono::year_month_day(std::chrono::year{ year }, std::chrono::month{ (unsigned int)month }, std::chrono::day{ (unsigned int)day });
	}

	bool isNull() const { return year() && month() && day(); }
	bool isValid() const { return _ymd.ok(); }

	std::string toString(const std::string& fmt = "%Y-%m-%d")const {
		return zc::dt::detail::format("{:" + fmt + "}", _ymd);
	}
public:
	static Date currentDate()
	{
		//获取当前时间
		auto now = std::chrono::system_clock::now();
		//获取当前时区
		auto zone = std::chrono::current_zone();
		//转成当前时区的时间
		std::chrono::zoned_time sh_time(zone, now);
		auto local_time = sh_time.get_local_time();
		//转成天数
		auto today = std::chrono::time_point_cast<std::chrono::days>(local_time);
		return Date(std::chrono::year_month_day(today));
	}

	static bool isLeapYear(int year) {
		return std::chrono::year{ year }.is_leap();
	}
private:
	std::chrono::year_month_day _ymd;
};

inline bool operator!=(const Date& lhs, const Date& rhs) { return lhs.toYearMonthDay() != rhs.toYearMonthDay(); }
inline bool operator<(const Date& lhs, const Date& rhs) { return lhs.toYearMonthDay() < rhs.toYearMonthDay(); }
inline bool operator<=(const Date& lhs, const Date& rhs) { return lhs.toYearMonthDay() <= rhs.toYearMonthDay(); }
inline bool operator==(const Date& lhs, const Date& rhs) { return lhs.toYearMonthDay() == rhs.toYearMonthDay(); }
inline bool operator>(const Date& lhs, const Date& rhs) { return lhs.toYearMonthDay() > rhs.toYearMonthDay(); }
inline bool operator>=(const Date& lhs, const Date& rhs) { return lhs.toYearMonthDay() >= rhs.toYearMonthDay(); }

/** 时间类 */
class Time {
public:
	Time() = default;
	Time(std::chrono::hh_mm_ss<std::chrono::milliseconds> time)
		:_hms(time) {
	}
	Time(int h, int m, int s = 0, int ms = 0) {
		setHMS(h, m, s, ms);
	}

	/**
	 * 在当前时间基础上添加指定的毫秒数.
	 */
	Time addMSecs(int ms) const {
		auto total_ms = _hms.to_duration() + std::chrono::milliseconds(ms);
		total_ms = total_ms % std::chrono::hours(24);
		if (total_ms.count() < 0)
			total_ms += std::chrono::hours(24);

		return Time(std::chrono::hh_mm_ss{ total_ms });
	}
	/**
	 * 在当前时间基础上添加指定的秒数.
	 */
	Time addSecs(int s) const {
		return addMSecs(s * 1000);
	}
	/**
	 * 设置时间.
	 */
	bool setHMS(int h, int m, int s, int ms = 0) {
		if (!isValid(h, m, s, ms)) {
			return false;
		}
		const auto _Dur = std::chrono::hours(h) + std::chrono::minutes(m) + std::chrono::seconds(s) + std::chrono::milliseconds(ms);
		_hms = std::chrono::hh_mm_ss{ _Dur };
		return true;
	};
	int hour() const { return _hms.hours().count(); }
	int minute() const { return _hms.minutes().count(); }
	int second() const { return _hms.seconds().count(); }
	int msec() const { return _hms.subseconds().count(); }

	bool isNull() const { return !hour() && !minute() && !second(); }
	bool isValid() const { return !_hms.is_negative(); }

	/**
	 * 将时间转为毫秒.
	 */
	int msecsSinceStartOfDay() const { return _hms.to_duration().count(); }
	/**
	 * 计算t和当前时间间隔的毫秒数.
	 */
	int msecsTo(Time t) const { return (std::chrono::duration_cast<std::chrono::milliseconds>(t._hms.to_duration()) - std::chrono::duration_cast<std::chrono::milliseconds>(_hms.to_duration())).count(); }
	/**
	 * 计算t和当前时间间隔的秒数.
	 */
	int secsTo(Time t) const { return (std::chrono::duration_cast<std::chrono::seconds>(t._hms.to_duration()) - std::chrono::duration_cast<std::chrono::seconds>(_hms.to_duration())).count(); }
	/**
	 * 将时间转成字符串.
	 */
	std::string toString(const std::string& fmt = "%H-%M-%S")const {
		return zc::dt::detail::format("{:" + fmt + "}", _hms);
	}
public:
	static Time currentTime()
	{
		//获取当前时间
		auto now = std::chrono::system_clock::now();
		//获取当前时区
		auto zone = std::chrono::current_zone();
		//转成当前时区的时间
		std::chrono::zoned_time sh_time(zone, now);
		auto local_time = sh_time.get_local_time();
		//转成天数
		auto today = std::chrono::time_point_cast<std::chrono::days>(local_time);
		return Time(std::chrono::hh_mm_ss(std::chrono::duration_cast<std::chrono::milliseconds>(local_time - today)));
	}
	// 判断时间是否有效（静态方法）
    static bool isValid(int h, int m, int s, int ms = 0) {
        return (h >= 0 && h < 24) &&
               (m >= 0 && m < 60) &&
               (s >= 0 && s < 60) &&
               (ms >= 0 && ms < 1000);
    }
private:
	std::chrono::hh_mm_ss<std::chrono::milliseconds> _hms;
};

inline bool operator!=(const Time& lhs, const Time& rhs) { return lhs.msecsSinceStartOfDay() != rhs.msecsSinceStartOfDay(); }
inline bool operator<(const Time& lhs, const Time& rhs) { return lhs.msecsSinceStartOfDay() < rhs.msecsSinceStartOfDay(); }
inline bool operator<=(const Time& lhs, const Time& rhs) { return lhs.msecsSinceStartOfDay() <= rhs.msecsSinceStartOfDay(); }
inline bool operator==(const Time& lhs, const Time& rhs) { return lhs.msecsSinceStartOfDay() == rhs.msecsSinceStartOfDay(); }
inline bool operator>(const Time& lhs, const Time& rhs) { return lhs.msecsSinceStartOfDay() > rhs.msecsSinceStartOfDay(); }
inline bool operator>=(const Time& lhs, const Time& rhs) { return lhs.msecsSinceStartOfDay() >= rhs.msecsSinceStartOfDay(); }

/** 日期时间类 */
class DateTime {
public:
	DateTime() {}
	DateTime(std::chrono::system_clock::duration dur)
		:_tp(dur)
	{}
	DateTime(Date date, Time time)
		:_tp(date.toSysDays().time_since_epoch() + std::chrono::milliseconds(time.msecsSinceStartOfDay()))
	{
	}

	Date date()const { 
		auto today = std::chrono::duration_cast<std::chrono::days>(_tp.time_since_epoch());
		return Date(std::chrono::year_month_day(std::chrono::sys_days{ today }));
	}
	Time time()const { 
		auto today = std::chrono::duration_cast<std::chrono::days>(_tp.time_since_epoch());
		auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(_tp.time_since_epoch() - today);
		return Time(std::chrono::hh_mm_ss{ ms });
	}

	DateTime addDays(int64_t ndays) const {
		return DateTime((_tp + std::chrono::days(ndays)).time_since_epoch());
	};
	DateTime addDuration(std::chrono::milliseconds msecs) const {
		return DateTime((_tp + msecs).time_since_epoch());
	}
	DateTime addMSecs(int64_t msecs) const {
		return DateTime((_tp + std::chrono::milliseconds(msecs)).time_since_epoch());
	}
	DateTime addMonths(int nmonths) const {
		//计算天数
		auto days = std::chrono::duration_cast<std::chrono::days>((_tp + std::chrono::months(nmonths)).time_since_epoch());
		//计算时间
		auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(_tp.time_since_epoch()) - std::chrono::duration_cast<std::chrono::days>(_tp.time_since_epoch());
		return DateTime(days + ms);
	}
	DateTime addYears(int nyears) const {
		//计算天数
		auto days = std::chrono::duration_cast<std::chrono::days>((_tp + std::chrono::years(nyears)).time_since_epoch());
		//计算时间
		auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(_tp.time_since_epoch()) - std::chrono::duration_cast<std::chrono::days>(_tp.time_since_epoch());
		return DateTime(days + ms);
	}

	DateTime addSecs(int64_t s) const {
		return DateTime((_tp + std::chrono::seconds(s)).time_since_epoch());
	}

	bool isNull() const { return _tp.time_since_epoch().count() == 0; }
	bool isValid() const { return !isNull(); }

	std::string toString(const std::string& fmt = "%Y-%m-%d %H:%M:%S")const {
		return zc::dt::detail::format("{:" + fmt + "}", _tp);
	}
	int64_t toSecsSinceEpoch() const {
		return std::chrono::duration_cast<std::chrono::seconds>(_tp.time_since_epoch()).count();
	}
    explicit operator const std::chrono::system_clock::time_point& ()const { return _tp; };
public:
	static DateTime currentDateTime() {
		//获取当前时间
		auto now = std::chrono::system_clock::now();
		//获取当前时区
		auto zone = std::chrono::current_zone();
		//转成当前时区的时间
		std::chrono::zoned_time sh_time(zone, now);
		auto local_time = sh_time.get_local_time();
		//转成天数
		//auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(local_time.time_since_epoch());
		return DateTime(local_time.time_since_epoch());
	}
private:
	std::chrono::system_clock::time_point _tp;
};

inline bool operator!=(const DateTime& lhs, const DateTime& rhs) { return static_cast<std::chrono::system_clock::time_point>(lhs) != static_cast<std::chrono::system_clock::time_point>(rhs); }
inline bool operator<(const DateTime& lhs, const DateTime& rhs)  { return static_cast<std::chrono::system_clock::time_point>(lhs) < static_cast<std::chrono::system_clock::time_point>(rhs); }
inline bool operator<=(const DateTime& lhs, const DateTime& rhs) { return static_cast<std::chrono::system_clock::time_point>(lhs) <= static_cast<std::chrono::system_clock::time_point>(rhs); }
inline bool operator==(const DateTime& lhs, const DateTime& rhs) { return static_cast<std::chrono::system_clock::time_point>(lhs) == static_cast<std::chrono::system_clock::time_point>(rhs); }
inline bool operator>(const DateTime& lhs, const DateTime& rhs)  { return static_cast<std::chrono::system_clock::time_point>(lhs) > static_cast<std::chrono::system_clock::time_point>(rhs); }
inline bool operator>=(const DateTime& lhs, const DateTime& rhs) { return static_cast<std::chrono::system_clock::time_point>(lhs) >= static_cast<std::chrono::system_clock::time_point>(rhs); }
