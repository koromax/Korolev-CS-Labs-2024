#include <iostream>

namespace {
const int kMinHours = 0;
const int kMaxHours = 23;
const int kMinMinutes = 0;
const int kMaxMinutes = 59;

const int kNightBegin = 0;
const int kNightEnd = 5;
const int kMorningBegin = 5;
const int kMorningEnd = 12;
const int kDaytimeBegin = 12;
const int kDaytimeEnd = 18;
const int kEveningBegin = 18;
const int kEveningEnd = 24;

const int kDecimalBase = 10;
const int k12hFormatBase = 12;

const int kHoursNominativeSingular = 1;
const int kHoursGenitiveSingularLowerLimit = 2;
const int kHoursGenitiveSingularUpperLimit = 4;

const int kMinutesNominativeSingular = 1;
const int kMinutesNominativePlural = 11;
const int kMinutesNominativePluralLowerLimit = 2;
const int kMinutesNominativePluralUpperLimit = 4;
const int kMinutesGenitivePluralLowerLimit = 10;
const int kMinutesGenitivePluralUpperLimit = 20;
}  // namespace

int main(int, char**) {
    std::cout << "Введите число часов из интервала [0; 23] и число минут из интервала [0; 59]:";
    int hours = 0;
    int minutes = 0;
    std::cin >> hours >> minutes;

    if ((hours < kMinHours || hours > kMaxHours) || (minutes < kMinMinutes || minutes > kMaxMinutes)) {
        std::cout << "неверные данные\n";
        return 1;
    }

    if (minutes == kMinMinutes) {
        if (hours == kNightBegin) {
            std::cout << "полночь\n";
            return 0;
        }
        if (hours == kDaytimeBegin) {
            std::cout << "полдень\n";
            return 0;
        }
    }

    int hours12hFormat = (hours > k12hFormatBase) ? (hours - k12hFormatBase) : hours;
    int minutesLastDigit = minutes % kDecimalBase;

    std::cout << hours12hFormat << " ";
    if ((hours12hFormat >= kHoursGenitiveSingularLowerLimit && hours12hFormat <= kHoursGenitiveSingularUpperLimit)) {
        std::cout << "часа";
    } else if (hours12hFormat == kHoursNominativeSingular) {
        std::cout << "час";
    } else {
        std::cout << "часов";
    }

    if (minutes != kMinMinutes) {
        std::cout << " " << minutes << " ";
        if (minutesLastDigit == kMinutesNominativeSingular && minutes != kMinutesNominativePlural) {
            std::cout << "минута";
        } else if ((minutesLastDigit >= kMinutesNominativePluralLowerLimit && minutesLastDigit <= kMinutesNominativePluralUpperLimit) &&
                   (minutes < kMinutesGenitivePluralLowerLimit || minutes > kMinutesGenitivePluralUpperLimit)) {
            std::cout << "минуты";
        } else {
            std::cout << "минут";
        }
    }

    if (hours >= kNightBegin && hours < kNightEnd) {
        std::cout << " ночи";
    } else if (hours >= kMorningBegin && hours < kMorningEnd) {
        std::cout << " утра";
    } else if (hours >= kDaytimeBegin && hours < kDaytimeEnd) {
        std::cout << " дня";
    } else if (hours >= kEveningBegin && hours < kEveningEnd) {
        std::cout << " вечера";
    }

    if (minutes == kMinMinutes) {
        std::cout << " ровно";
    }
    std::cout << "\n";

    return 0;
}
