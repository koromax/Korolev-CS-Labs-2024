#include "MatrixPrint.h"

#include <termios.h>
#include <unistd.h>
#include <algorithm>
#include <iomanip>
#include <iostream>

namespace {
void ClearTerminal() {
    std::cout << "\x1B[2J\x1B[H";
}

template<typename VAL>
int CalculateColumnWidth(VAL** m, int precision, int rows, int columns) {
    //         (-)  0   .   000000001
    int width = 1 + 1 + 1 + precision;
    for (int row = 0; row < rows; ++row) {
        for (int column = 0; column < columns; ++column) {
            int digits = 1 + 1 + 1 + precision;

            int temp = static_cast<int>(m[row][column]);
            if (temp < 0) {
                ++digits;
                temp *= -1;
            }

            while (temp > 10) {
                temp /= 10;
                ++digits;
            }

            if (digits > width) {
                width = digits;
            }
        }
    }

    return width;
}
}  // namespace

namespace MatrixPrint {

template<typename VAL>
void Printer(int firstColumn, VAL** m, int precision, int width, int rows, int columns) {
    ClearTerminal();
    std::cout << std::fixed << std::setprecision(precision) << std::right;

    int columnsPerLine = std::min(columns, (80 - 4) / width);
    int tableWidth = 1 + (width + 1) * columnsPerLine + 1;

    // curls and column numbers
    std::cout << std::left << std::setw(width - precision) << " /";
    for (int column = firstColumn; column < std::min(firstColumn + columnsPerLine, columns); ++column) {
        std::cout << std::setw(precision) << column;
        if (column != std::min(firstColumn + columnsPerLine, columns) - 1) {
            std::cout << std::setw(width - precision + 1) << ' ';
        }
    }
    std::cout << " \\ " << '\n' << std::right;

    // matrix
    for (int row = 0; row < rows; ++row) {
        std::cout << '|';
        for (int column = firstColumn; column < std::min(firstColumn + columnsPerLine, columns); ++column) {
            std::cout << std::setw(width) << m[row][column] << ' ';
        }
        std::cout << '|' << '\n';
    }

    // curls and instructions
    std::cout << std::left << std::setw(width - precision) << " \\" << std::right << "[</>] Scroll  [^/⌵] Precision  [ESCx3] Exit"
              << std::setw(tableWidth - width + precision - 43) << "/ " << '\n';
}

template<typename VAL>
void PrintMatrix(VAL** m, int precision, int rows, int columns) {
    struct termios old_tio = {};
    tcgetattr(STDIN_FILENO, &old_tio);
    struct termios new_tio = old_tio;
    new_tio.c_lflag &= (~ICANON);
    new_tio.c_lflag &= (~ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &new_tio);

    int width = CalculateColumnWidth(m, precision, rows, columns);
    int columnsPerLine = std::min(columns, (80 - 4) / width);

    int firstColumn = 0;
    Printer(firstColumn, m, precision, width, rows, columns);

    char buf[3] = "  ";
    char keystroke = ' ';
    while (fread(&keystroke, 1, 1, stdin)) {
        buf[0] = buf[1];
        buf[1] = buf[2];
        buf[2] = keystroke;
        if (buf[0] == '\033' && buf[2] == 'C' && firstColumn < columns - columnsPerLine) {
            Printer(++firstColumn, m, precision, width, rows, columns);
        } else if (buf[0] == '\033' && buf[2] == 'D' && firstColumn != 0) {
            Printer(--firstColumn, m, precision, width, rows, columns);
        } else if (buf[0] == '\033' && buf[2] == 'A' && precision < 8) {
            width = CalculateColumnWidth(m, ++precision, rows, columns);
            columnsPerLine = std::min(columns, (80 - 4) / width);
            Printer(firstColumn, m, precision, width, rows, columns);
        } else if (buf[0] == '\033' && buf[2] == 'B' && precision > 3) {
            width = CalculateColumnWidth(m, --precision, rows, columns);
            columnsPerLine = std::min(columns, (80 - 4) / width);
            firstColumn = std::min(firstColumn, columns - columnsPerLine);
            Printer(firstColumn, m, precision, width, rows, columns);
        } else if (buf[0] == '\033' && buf[1] == '\033' && buf[2] == '\033') {
            ClearTerminal();
            tcsetattr(STDIN_FILENO, TCSANOW, &old_tio);
            return;
        }
    }

    tcsetattr(STDIN_FILENO, TCSANOW, &old_tio);
}
template void PrintMatrix<int>(int** m, int precision, int rows, int columns);
template void PrintMatrix<double>(double** m, int precision, int rows, int columns);

}  // namespace MatrixPrint
