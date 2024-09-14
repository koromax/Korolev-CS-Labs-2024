#include <iostream>

namespace {
// const int maxHours = 23;
const int maxMinutes = 59;
const int night = 0;
const int morning = 5;
const int day = 12;
const int evening = 18;
const int dayCycleEnd = 24;

const int hourWhenNothingShouldBePrinted = 1;
const int hoursARuleLBound = 2;
const int hoursARuleRBound = 4;

const int minutesARuleException = 11;
const int minutesUIRuleLBound = 2;
const int minutesUIRuleRBound = 4;
const int minutesUIRuleExceptionLBound = 10;
const int minutesUIRuleExceptionRBound = 20;
}  // namespace

int main(int, char**) {
    int input1 = 0;
    int input2 = 0;
    std::cin >> input1 >> input2;

    const int hours = input1;
    const int minutes = input2;
    const int hours_12hFormat = (hours > 12) ? (hours - 12) : hours;

    if (hours > dayCycleEnd) {
        std::cout << "Часов больше чем надо\n";
        return 0;
    }
    if (minutes > maxMinutes or (hours == dayCycleEnd and minutes != 0)) {
        std::cout << "Минут больше чем надо\n";
        return 0;
    }

    if (minutes == night) {
        if (hours == night or hours == dayCycleEnd) {
            std::cout << "полночь\n";
            return 0;
        }
        if (hours == day) {
            std::cout << "полдень\n";
            return 0;
        }
    }

    std::cout << hours_12hFormat << " ";
    std::cout << "час";
    if (hours_12hFormat == hourWhenNothingShouldBePrinted) {
        std::cout << " ";
    } else if ((hoursARuleLBound <= hours_12hFormat and hours_12hFormat <= hoursARuleRBound)) {
        std::cout << "а" << " ";
    } else {
        std::cout << "ов" << " ";
    }

    if (minutes != 0) {
        std::cout << minutes << " минут";
    }

    if (minutes % 10 == 1 and minutes != minutesARuleException) {
        std::cout << "а" << " ";
    } else if ((minutesUIRuleLBound <= minutes % 10 and minutes % 10 <= minutesUIRuleRBound) and
               (minutes < minutesUIRuleExceptionLBound or minutes > minutesUIRuleExceptionRBound)) {
        std::cout << "ы" << " ";
    } else if (minutes != 0) {
        std::cout << " ";
    }

    if (night <= hours and hours < morning) {
        std::cout << "ночи";
    } else if (morning <= hours and hours < day) {
        std::cout << "утра";
    } else if (day <= hours and hours < evening) {
        std::cout << "дня";
    } else if (evening <= hours and hours < dayCycleEnd) {
        std::cout << "вечера";
    }

    if (minutes == 0) {
        std::cout << " ровно\n";
    } else {
        std::cout << "\n";
    }

    return 0;
}
