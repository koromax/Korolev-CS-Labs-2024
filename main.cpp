#include <iostream>

namespace {
const int maxMinutes = 59;
const int night = 0;
const int morning = 5;
const int noon = 12;
const int evening = 18;
const int dayCycleLength = 24;

const int hourRuleException = 1;
const int hoursARuleLBound = 2;
const int hoursARuleRBound = 4;

const int minutesARuleException = 11;
const int minutesUIRuleLBound = 2;
const int minutesUIRuleRBound = 4;
const int minutesUIRuleExceptionLBound = 10;
const int minutesUIRuleExceptionRBound = 20;
const int minDivide = 10;
}  // namespace

int main(int, char**) {
    int hours = 0;
    int minutes = 0;
    std::cin >> hours >> minutes;

    int hours_12hFormat = (hours > noon) ? (hours - noon) : hours;
    int minutesSecondNumber = minutes % minDivide;

    if (hours < 0 or dayCycleLength <= hours) {
        std::cout << "Часов больше чем надо\n";
        return 0;
    }
    if (minutes < 0 or maxMinutes < minutes) {
        std::cout << "Минут больше чем надо\n";
        return 0;
    }

    if (minutes == night) {
        if (hours == night or hours == dayCycleLength) {
            std::cout << "полночь\n";
            return 0;
        }
        if (hours == noon) {
            std::cout << "полдень\n";
            return 0;
        }
    }

    std::cout << hours_12hFormat << " ";
    std::cout << "час";
    if ((hoursARuleLBound <= hours_12hFormat and hours_12hFormat <= hoursARuleRBound)) {
        std::cout << "а";
    } else if (hours_12hFormat != hourRuleException) {
        std::cout << "ов";
    }

    if (minutes != 0) {
        std::cout << " " << minutes << " минут";
    }

    if (minutesSecondNumber == 1 and minutes != minutesARuleException) {
        std::cout << "а";
    } else if ((minutesUIRuleLBound <= minutesSecondNumber and minutesSecondNumber <= minutesUIRuleRBound) and
               (minutes < minutesUIRuleExceptionLBound or minutes > minutesUIRuleExceptionRBound)) {
        std::cout << "ы";
    }

    if (night <= hours and hours < morning) {
        std::cout << " ночи";
    } else if (morning <= hours and hours < noon) {
        std::cout << " утра";
    } else if (noon <= hours and hours < evening) {
        std::cout << " дня";
    } else if (evening <= hours and hours < dayCycleLength) {
        std::cout << " вечера";
    }

    if (minutes == 0) {
        std::cout << " ровно\n";
    } else {
        std::cout << "\n";
    }

    return 0;
}
