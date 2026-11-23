#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstring>
using namespace std;

// Облік ресурсів: кожне виділення і звільнення блоку пам’яті фіксується.
int gAllocs = 0, gFrees = 0;
int liveBlocks() { return gAllocs - gFrees; }

struct Book { char title[96]; char author[96]; long long year; };
struct BookArray { Book* data; int size; int cap; };

enum GrowMode { PLUS_ONE, DOUBLING };
long long gReallocs = 0, gCopied = 0;

void initArray(BookArray& a) { a.data = nullptr; a.size = 0; a.cap = 0; }

void freeArray(BookArray& a)
{
    if (a.data != nullptr) { delete[] a.data; gFrees++; }
    a.data = nullptr; a.size = 0; a.cap = 0;
}

bool reallocTo(BookArray& a, int newCap)
{
    if (newCap <= 0 || newCap < a.size) return false;
    Book* nb = new Book[newCap]; gAllocs++;
    for (int i = 0; i < a.size; i++) { nb[i] = a.data[i]; gCopied++; }
    if (a.data != nullptr) { delete[] a.data; gFrees++; }
    a.data = nb; a.cap = newCap; gReallocs++;
    return true;
}

bool addBook(BookArray& a, const Book& b, GrowMode mode)
{
    if (a.size == a.cap)
    {
        int newCap = (mode == DOUBLING) ? (a.cap == 0 ? 1 : a.cap * 2) : a.size + 1;
        if (!reallocTo(a, newCap)) return false;
    }
    a.data[a.size++] = b;
    return true;
}

bool removeAt(BookArray& a, int idx)
{
    if (idx < 0 || idx >= a.size) return false;
    if (a.size == 1) { freeArray(a); return true; }
    Book* nb = new Book[a.size - 1]; gAllocs++;
    int k = 0;
    for (int i = 0; i < a.size; i++) if (i != idx) { nb[k++] = a.data[i]; gCopied++; }
    delete[] a.data; gFrees++;
    a.data = nb; a.size--; a.cap = a.size;
    return true;
}

// Пошук першого збігу за другим полем (автор). Повертає індекс або -1.
int findByAuthor(const BookArray& a, const char* author)
{
    for (int i = 0; i < a.size; i++) if (strcmp(a.data[i].author, author) == 0) return i;
    return -1;
}

// Числовий підсумок за третім полем (рік): сума значень.
long long sumYears(const BookArray& a)
{
    long long s = 0;
    for (int i = 0; i < a.size; i++) s += a.data[i].year;
    return s;
}

// Додаткова операція варіанта 2: найновіша книга (найбільший рік); -1, якщо масив порожній.
int newestIndex(const BookArray& a)
{
    if (a.size == 0) return -1;
    int best = 0;
    for (int i = 1; i < a.size; i++) if (a.data[i].year > a.data[best].year) best = i;
    return best;
}

Book makeBook(const char* t, const char* au, long long y)
{
    Book x; strncpy(x.title, t, 95); x.title[95] = 0;
    strncpy(x.author, au, 95); x.author[95] = 0;
    x.year = y; return x;
}

int testCounter = 0;
bool report(int number, const char* title, bool ok)
{
    cout << "Тест " << setw(2) << number << " (" << title << "): " << (ok ? "успішно" : "помилка")
         << ", активних блоків=" << liveBlocks() << endl;
    return ok && liveBlocks() == 0;
}

bool runAllTests()
{
    cout << "\n=== Автоматизовані перевірки ===" << endl;
    bool all = true;
    Book b1 = makeBook("Кобзар", "Шевченко", 1840);
    Book b2 = makeBook("Лісова пісня", "Коцюбинський", 1913);
    Book b3 = makeBook("Гайдамаки", "Шевченко", 1841);
    Book b4 = makeBook("Земля", "Довженко", 1930);
    Book b5 = makeBook("Тигролови", "Франко", 1900);
    Book b6 = makeBook("Тіні забутих предків", "Коцюбинський", 1911);
    GrowMode M = DOUBLING;

    { BookArray a; initArray(a); // 1
      bool ok = (a.size == 0 && newestIndex(a) == -1 && sumYears(a) == 0);
      freeArray(a); all &= report(1, "порожня структура: перегляд", ok); }
    { BookArray a; initArray(a); // 2
      bool ok = (removeAt(a, 0) == false && a.size == 0);
      freeArray(a); all &= report(2, "порожня структура: видалення", ok); }
    { BookArray a; initArray(a); addBook(a, b1, M); // 3
      bool ok = (findByAuthor(a, "Шевченко") == 0);
      freeArray(a); all &= report(3, "один елемент: пошук", ok); }
    { BookArray a; initArray(a); addBook(a, b1, M); // 4
      bool ok = (removeAt(a, 0) && a.size == 0 && a.data == nullptr);
      all &= report(4, "один елемент: видалення", ok); }
    { BookArray a; initArray(a); addBook(a, b1, M); addBook(a, b2, M); addBook(a, b3, M); // 5
      bool ok = addBook(a, b4, M) && a.size == 4 && strcmp(a.data[3].title, "Земля") == 0 && strcmp(a.data[0].title, "Кобзар") == 0;
      freeArray(a); all &= report(5, "три елементи: додавання", ok); }
    { BookArray a; initArray(a); addBook(a, b1, M); addBook(a, b2, M); addBook(a, b3, M); // 6
      bool ok = removeAt(a, 0) && a.size == 2 && strcmp(a.data[0].title, "Лісова пісня") == 0 && strcmp(a.data[1].title, "Гайдамаки") == 0;
      freeArray(a); all &= report(6, "видалення першого", ok); }
    { BookArray a; initArray(a); addBook(a, b1, M); addBook(a, b2, M); addBook(a, b3, M); // 7
      bool ok = removeAt(a, 1) && a.size == 2 && strcmp(a.data[0].title, "Кобзар") == 0 && strcmp(a.data[1].title, "Гайдамаки") == 0;
      freeArray(a); all &= report(7, "видалення середнього", ok); }
    { BookArray a; initArray(a); addBook(a, b1, M); addBook(a, b2, M); addBook(a, b3, M); // 8
      bool ok = removeAt(a, 2) && a.size == 2 && strcmp(a.data[1].title, "Лісова пісня") == 0;
      freeArray(a); all &= report(8, "видалення останнього", ok); }
    { BookArray a; initArray(a); addBook(a, b1, M); addBook(a, b2, M); addBook(a, b3, M); // 9
      bool ok = (removeAt(a, -1) == false && a.size == 3);
      freeArray(a); all &= report(9, "індекс −1", ok); }
    { BookArray a; initArray(a); addBook(a, b1, M); addBook(a, b2, M); addBook(a, b3, M); // 10
      bool ok = (removeAt(a, 3) == false && a.size == 3);
      freeArray(a); all &= report(10, "індекс, рівний розміру", ok); }
    { BookArray a; initArray(a); // 11: повна місткість, додавання збільшує місткість
      addBook(a, b1, M); addBook(a, b2, M); addBook(a, b3, M); addBook(a, b4, M);
      int capBefore = a.cap;
      bool ok = (a.size == a.cap) && addBook(a, b5, M) && a.cap > capBefore && a.size == 5;
      freeArray(a); all &= report(11, "повна місткість: додавання", ok); }
    { BookArray a; initArray(a); // 12: 100 елементів, очищення
      for (int i = 0; i < 100; i++) addBook(a, b2, M);
      bool ok = (a.size == 100);
      while (a.size > 0) removeAt(a, 0);
      ok = ok && a.size == 0 && a.data == nullptr;
      all &= report(12, "100 елементів: очищення", ok); }
    { BookArray a; initArray(a); // 13 (власний): повторне заповнення після очищення
      addBook(a, b1, M); addBook(a, b2, M); addBook(a, b3, M);
      while (a.size > 0) removeAt(a, 0);
      addBook(a, b4, M); addBook(a, b5, M);
      bool ok = (a.size == 2 && strcmp(a.data[0].title, "Земля") == 0);
      freeArray(a); all &= report(13, "власний: повторне заповнення", ok); }
    { BookArray a; initArray(a); // 14 (власний): 50 чергувань додавання і видалення
      bool ok = true;
      for (int i = 0; i < 50; i++) { addBook(a, b1, M); if (!removeAt(a, a.size - 1)) ok = false; }
      ok = ok && a.size == 0;
      freeArray(a); all &= report(14, "власний: 50 чергувань", ok); }
    { BookArray a; initArray(a); addBook(a, b1, M); addBook(a, b2, M); // 15 (власний): невдалий пошук
      int before = a.size;
      bool ok = (findByAuthor(a, "Невідомий") == -1 && a.size == before);
      freeArray(a); all &= report(15, "власний: невдалий пошук", ok); }
    return all;
}

// Варіант 2: книга — назва, автор, рік (умовні дані для демонстрації)
void variantRun()
{
    cout << "\n=== Варіант 2 ===" << endl;
    BookArray a; initArray(a);
    addBook(a, makeBook("Кобзар", "Шевченко", 1840), DOUBLING);
    addBook(a, makeBook("Лісова пісня", "Коцюбинський", 1913), DOUBLING);
    addBook(a, makeBook("Гайдамаки", "Шевченко", 1841), DOUBLING);
    addBook(a, makeBook("Земля", "Довженко", 1930), DOUBLING);
    addBook(a, makeBook("Тигролови", "Франко", 1900), DOUBLING);
    addBook(a, makeBook("Тіні забутих предків", "Коцюбинський", 1911), DOUBLING);
    cout << "№\tНазва\tАвтор\tРік" << endl;
    for (int i = 0; i < a.size; i++)
        cout << i << "\t" << a.data[i].title << "\t" << a.data[i].author << "\t" << a.data[i].year << endl;
    cout << "Пошук за автором «Шевченко»: перший індекс=" << findByAuthor(a, "Шевченко") << endl;
    cout << "Сума років=" << sumYears(a) << endl;
    int m = newestIndex(a);
    cout << "Найновіша книга: " << a.data[m].title << " (" << a.data[m].year << ")" << endl;
    cout << "Розмір=" << a.size << ", місткість=" << a.cap << endl;
    freeArray(a);
    cout << "Після звільнення: активних блоків=" << liveBlocks() << endl;
}

void capacityExperiment()
{
    cout << "\n=== Збільшення на 1 проти подвоєння ===" << endl;
    cout << "Додавань\tРежим\tПеревиділень\tПеренесено елементів\tАктивних блоків" << endl;
    int counts[3] = { 10, 100, 1000 };
    ofstream csv("results.csv");
    csv << "adds,mode,reallocs,copied,live\n";
    Book sample = makeBook("Книга", "Автор", 2000);
    for (int ci = 0; ci < 3; ci++)
        for (int mode = 0; mode < 2; mode++)
        {
            gReallocs = 0; gCopied = 0;
            BookArray a; initArray(a);
            for (int i = 0; i < counts[ci]; i++) addBook(a, sample, mode == 0 ? PLUS_ONE : DOUBLING);
            freeArray(a);
            const char* mname = (mode == 0) ? "+1" : "подвоєння";
            cout << counts[ci] << "\t" << mname << "\t" << gReallocs << "\t\t" << gCopied << "\t\t\t" << liveBlocks() << endl;
            csv << counts[ci] << "," << mname << "," << gReallocs << "," << gCopied << "," << liveBlocks() << "\n";
        }
}

int main()
{
    bool ok = runAllTests();
    if (!ok) { cout << "\nЄ помилки - експеримент не виконується." << endl; return 1; }
    variantRun();
    capacityExperiment();
    cout << "\nФінальний баланс: виділень=" << gAllocs << ", звільнень=" << gFrees
         << ", активних блоків=" << liveBlocks() << endl;
    return 0;
}
