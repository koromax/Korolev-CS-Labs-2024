#include <iostream>

namespace {
const int kMinAllowedValueOfHours = 0;
const int kMaxAllowedValueOfHours = 23;
const int kMinAllowedValueOfMinutes = 0;
const int kMaxAllowedValueOfMinutes = 59;

const int kHourWhenNightBegins = 0;
const int kHourWhenNightEnds = 5;
const int kHourWhenMorningBegins = 5;
const int kHourWhenMorningEnds = 12;
const int kHourWhenDaytimeBegins = 12;
const int kHourWhenDaytimeEnds = 18;
const int kHourWhenEveningBegins = 18;
const int kHourWhenEveningEnds = 24;

const int kDivisorToDetermineTheLastDigitOfANumberInBase10 = 10;
const int kLastHourWhenNoSubtractionIsNeeded = 12;
const int kAmountOfHoursToSubtract = 12;

const int kMinutesWhenHourIsExact = 0;
const int kMidnightHour = 0;
const int kNoonHour = 12;

const int kHoursGenitiveCaseException = 1;
const int kHoursGenitiveCaseLowerLimit = 2;
const int kHoursGenitiveCaseUpperLimit = 4;

const int kMinutesNominativeCaseCriteria = 1;
const int kMinutesNominativeCaseException = 11;
const int kMinutesGenitiveCaseLowerLimit = 2;
const int kMinutesGenitiveCaseUpperLimit = 4;
const int kMinutesGenitiveCaseExceptionLowerLimit = 10;
const int kMinutesGenitiveCaseExceptionUpperLimit = 20;
}  // namespace

int main(int, char**) {
    std::cout << "Введите время в 24-часовом формате: ";
    int hours = 0;
    int minutes = 0;
    std::cin >> hours >> minutes;

    if ((hours < kMinAllowedValueOfHours || hours > kMaxAllowedValueOfHours) ||
        (minutes < kMinAllowedValueOfMinutes || minutes > kMaxAllowedValueOfMinutes)) {
        std::cout << "неверные данные\n";
        return 1;
    }

    if (minutes == kMinutesWhenHourIsExact) {
        if (hours == kMidnightHour) {
            std::cout << "полночь\n";
            return 0;
        }
        if (hours == kNoonHour) {
            std::cout << "полдень\n";
            return 0;
        }
    }

    int hours12hFormat = (hours > kLastHourWhenNoSubtractionIsNeeded) ? (hours - kAmountOfHoursToSubtract) : hours;
    int minutesSecondDigit = minutes % kDivisorToDetermineTheLastDigitOfANumberInBase10;

    std::cout << hours12hFormat << " ";
    if ((hours12hFormat >= kHoursGenitiveCaseLowerLimit && hours12hFormat <= kHoursGenitiveCaseUpperLimit)) {
        std::cout << "часа";
    } else if (hours12hFormat == kHoursGenitiveCaseException) {
        std::cout << "час";
    } else {
        std::cout << "часов";
    }

    if (minutes != kMinutesWhenHourIsExact) {
        std::cout << " " << minutes << " ";
        if (minutesSecondDigit == kMinutesNominativeCaseCriteria && minutes != kMinutesNominativeCaseException) {
            std::cout << "минута";
        } else if ((minutesSecondDigit >= kMinutesGenitiveCaseLowerLimit && minutesSecondDigit <= kMinutesGenitiveCaseUpperLimit) &&
                   (minutes < kMinutesGenitiveCaseExceptionLowerLimit || minutes > kMinutesGenitiveCaseExceptionUpperLimit)) {
            std::cout << "минуты";
        } else {
            std::cout << "минут";
        }
    }

    if (kHourWhenNightBegins <= hours && kHourWhenNightEnds > hours) {
        std::cout << " ночи";
    } else if (kHourWhenMorningBegins <= hours && kHourWhenMorningEnds > hours) {
        std::cout << " утра";
    } else if (kHourWhenDaytimeBegins <= hours && kHourWhenDaytimeEnds > hours) {
        std::cout << " дня";
    } else if (kHourWhenEveningBegins <= hours && kHourWhenEveningEnds > hours) {
        std::cout << " вечера";
    }

    if (minutes == kMinutesWhenHourIsExact) {
        std::cout << " ровно";
    }
    std::cout << "\n";

    return 0;
}
