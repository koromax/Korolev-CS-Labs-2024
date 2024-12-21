(co-author @telekolia)
# DZ - реализация алгоритмов шифрования RSA

> Семестровое домашнее задание для студентов курса "Основы программирования" 1 курса кафедры ИУ5 МГТУ им Н.Э. Баумана.

## Задание
1. Ознакомится с теоретическим материалом «Алгоритм RSA» по ссылкам:

    1.1. https://habr.com/ru/articles/745820/

    1.2. https://www.geeksforgeeks.org/rsa-and-digital-signatures/

2. Выполнить реализацию алгоритма RSA на языке программирования: С++.
3. Используя реализацию, решить задачу: Известен открытый ключ алгоритма RSA:
(e, N) = (53, 299). Некоторое сообщение (число) m зашифровано этим открытым
ключом: e(m) = 171. Необходимо расшифровать сообщение и вывести получившееся
число.

## Теория
### Общая информация
Протокол RSA позволяет двум субъектам обмениваться информацией в зашифрованом виде и декодировать её у себя с помощью двух разных ключей. Один ключ обычно известен большому кругу лиц (Открытый ключ), а другой держится в секрете (Закрытый ключ), как и некоторые числовые значения используемые в генерации ключа (*d, Ф(N), p и q*).
### Получение ключей
1. Берётся два простых числа *p* и *q* и получается число *N*.
- N - это публичный ключ в нашем протоколе.
- Для него найдём значение функции эйлера Ф(N):
    ~~~
    Ф(N) = (p - 1)(q - 1)
    ~~~
    > Ф(N) особенен тем, что a^Ф(N) = 1 mod N

2. Из чисел *a* < *N*, таких что НОД(a, N) = 1 возьмём *e*, такой что НОД(e, Ф(N)) = 1. Таким образом мы получили публичный ключ *(e, N)*.

3. Тогда утверждается что
    ~~~
    есть d < N : e * d = 1 mod Ф(N)

    или (идентичные записи)

    есть d, k : e * d - Ф(N) * k = 1
    ~~~
- Мы получили закрытый ключ *(d, N)*.

### Кодирование/Декодирование
1. Кодирование:
 - Назовём сообщением некоторое число m(возьмём его из чисел *a*, смотри выше), чтобы его закодировать выполним следующую операцию:
    ~~~
    C = encode(m) = m^e mod N
    ~~~
2. Декодирование:
- Чтобы декодировать
    ~~~
    decode(C) = C^d mod N = (m^e mod N)^d mod N = m^(e * d) mod N =

    вспомним, что e * d = 1 + Ф(N) * k. Тогда получим:

    = m^(1 + Ф(N) * k) mod N = m * m^(Ф(N) * k) mod N

    вспомним, что a^Ф(N) = 1 mod N, как раз наше m из таких чисел a. Тогда получим:

    = m * 1 = m
    ~~~

## Как работает код
### Функции
#### 1. bool IsPrime(int n)
- Проверка числа n на простоту

#### 2. bool FindPQ(int N, int& p, int& q)
- Поиск p и q перебором:
- Если перебираемое i простое и делитель N, и (N / i) - простое, мы нашли **p = i**, **q = N / i**
- Если перебираемое i простое и делитель N, но (N / i) - не простое, то у N больше двух делителей, ака плохие вводные данные
    - конечно введенные данные всегда равны N = 299, но проверка не помешает

#### 3. int EEA(int a, int b, int& x, int& y)
украдено с [e-maxx](http://e-maxx.ru/algo/export_extended_euclid_algorithm)
- Расширенный алгоритм Евклида
- В коде нужен для нахождения коэффициента при _**e**_
- Не понятно зачем? Читай теорию

#### 4. int ExpPower(int a, int p, int mod)
Ну это я сделал типа умный
- Рекурсивное квадрадратное возведение в степень (с модулем)
- Читай [тут](https://en.wikipedia.org/wiki/Exponentiation_by_squaring)

#### 5. int main()
Самое главное и самое непонятное
- задаем переменные _e_, _n_, _enc_
- находим p и q
- находим Ф(N)
- находим коэффициент при _e_
- возводим _enc_ ^ d

Наглядная иллюстрация
![картинка с решением](https://sun9-62.userapi.com/impg/AN5stgCCXPdgRqY0c8ttZIjHxJ9qxAKlsNmPVQ/Y2Cwk-73i9Q.jpg?size=1620x2160&quality=95&sign=b92311f7959d55de2f4daf921f5c7142&type=album)
