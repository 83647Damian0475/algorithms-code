#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstring>
using namespace std;

// Облік вузлів: кожне створення і звільнення фіксується.
int gMade = 0, gFreed = 0;
int liveNodes() { return gMade - gFreed; }

struct Book { char title[96]; char author[96]; long long year; };
struct Node { Book data; Node* next; };

struct List { Node* head; Node* tail; int count; };

void initList(List& l) { l.head = nullptr; l.tail = nullptr; l.count = 0; }

Book makeBook(const char* t, const char* a, long long y)
{
    Book b; strncpy(b.title, t, 95); b.title[95] = 0;
    strncpy(b.author, a, 95); b.author[95] = 0; b.year = y; return b;
}

// Додавання в кінець: новий вузол стає останнім, tail оновлюється.
void pushBack(List& l, const Book& b)
{
    Node* n = new Node; gMade++;
    n->data = b; n->next = nullptr;
    if (l.tail == nullptr) { l.head = n; l.tail = n; }
    else { l.tail->next = n; l.tail = n; }
    l.count++;
}

// Додавання на початок: новий вузол стає першим; tail змінюється лише для порожнього списку.
void pushFront(List& l, const Book& b)
{
    Node* n = new Node; gMade++;
    n->data = b; n->next = l.head;
    l.head = n;
    if (l.tail == nullptr) l.tail = n;
    l.count++;
}

// Перегляд: окремий робочий покажчик, початок списку не змінюється.
int traverse(const List& l, bool print)
{
    int idx = 0;
    for (const Node* cur = l.head; cur != nullptr; cur = cur->next)
    {
        if (print) cout << "  " << idx + 1 << ". " << cur->data.title << " | " << cur->data.author << " | " << cur->data.year << endl;
        idx++;
    }
    return idx;
}

// Діагностика: адреси вузлів і зв’язків. Для останнього вузла явно вказано відсутність наступного.
void diagnostics(const List& l)
{
    int idx = 1;
    for (const Node* cur = l.head; cur != nullptr; cur = cur->next, idx++)
    {
        cout << "  вузол " << idx << ": адреса=" << (const void*)cur << ", наступний=";
        if (cur->next != nullptr) cout << (const void*)cur->next << endl;
        else cout << "немає (кінець)" << endl;
    }
}

// Перевірка структури з обмеженням кроків. Повертає порожній рядок, якщо все правильно, інакше опис помилки.
const char* checkStructure(const List& l)
{
    if (l.head == nullptr)
    {
        if (l.tail != nullptr || l.count != 0) return "порожній стан некоректний";
        return "";
    }
    int limit = l.count + 1, steps = 0;
    const Node* cur = l.head;
    const Node* last = nullptr;
    while (cur != nullptr)
    {
        steps++;
        if (steps > limit) return "можливий цикл";
        last = cur;
        cur = cur->next;
    }
    if (steps != l.count) return "неправильна кількість вузлів";
    if (last != l.tail) return "хибний кінець списку";
    return "";
}

// Очищення: зберігаємо адресу наступного до звільнення поточного.
void clearList(List& l)
{
    Node* cur = l.head;
    while (cur != nullptr)
    {
        Node* nxt = cur->next;
        delete cur; gFreed++;
        cur = nxt;
    }
    initList(l);
}

bool report(int number, const char* title, bool ok, const List& l)
{
    cout << "Тест " << setw(2) << number << " (" << title << "): " << (ok ? "успішно" : "помилка")
         << ", вузлів=" << l.count << ", активних вузлів=" << liveNodes() << endl;
    return ok && liveNodes() == 0;
}

bool runAllTests()
{
    cout << "\n=== Автоматизовані перевірки ===" << endl;
    bool all = true;
    Book A = makeBook("Кобзар", "Шевченко", 1840);
    Book B = makeBook("Лісова пісня", "Коцюбинський", 1913);
    Book C = makeBook("Гайдамаки", "Шевченко", 1841);
    Book D = makeBook("Земля", "Довженко", 1930);

    auto firstTitle = [](const List& l) -> const char* { return l.head ? l.head->data.title : ""; };
    auto lastTitle  = [](const List& l) -> const char* { return l.tail ? l.tail->data.title : ""; };

    { List l; initList(l); // 1
      bool ok = traverse(l, false) == 0 && checkStructure(l)[0] == 0;
      clearList(l); all &= report(1, "порожній список: перегляд", ok, l); }
    { List l; initList(l); pushBack(l, A); // 2
      bool ok = traverse(l, false) == 1 && strcmp(firstTitle(l), "Кобзар") == 0 && checkStructure(l)[0] == 0;
      clearList(l); all &= report(2, "один вузол у кінець", ok, l); }
    { List l; initList(l); pushFront(l, A); // 3
      bool ok = traverse(l, false) == 1 && strcmp(lastTitle(l), "Кобзар") == 0;
      clearList(l); all &= report(3, "один вузол на початок", ok, l); }
    { List l; initList(l); pushBack(l, A); pushBack(l, B); pushBack(l, C); // 4
      bool ok = (strcmp(firstTitle(l), "Кобзар") == 0 && strcmp(lastTitle(l), "Гайдамаки") == 0 && traverse(l, false) == 3);
      clearList(l); all &= report(4, "три вузли у кінець", ok, l); }
    { List l; initList(l); pushFront(l, A); pushFront(l, B); pushFront(l, C); // 5
      bool ok = (strcmp(firstTitle(l), "Гайдамаки") == 0 && strcmp(lastTitle(l), "Кобзар") == 0);
      clearList(l); all &= report(5, "три вузли на початок", ok, l); }
    { List l; initList(l); pushBack(l, A); pushBack(l, A); pushBack(l, A); // 6
      bool ok = traverse(l, false) == 3;
      clearList(l); all &= report(6, "однакові значення", ok, l); }
    { List l; initList(l); pushBack(l, makeBook("x", "y", -2)); pushBack(l, makeBook("z", "w", 0)); pushBack(l, makeBook("v", "u", 4)); // 7
      bool ok = (l.head->data.year == -2 && l.tail->data.year == 4 && traverse(l, false) == 3);
      clearList(l); all &= report(7, "від’ємні і нульові значення у кінець", ok, l); }
    { List l; initList(l); pushFront(l, makeBook("x", "y", -2)); pushFront(l, makeBook("z", "w", 0)); pushFront(l, makeBook("v", "u", 4)); // 8
      bool ok = (l.head->data.year == 4 && l.tail->data.year == -2);
      clearList(l); all &= report(8, "від’ємні і нульові значення на початок", ok, l); }
    { List l; initList(l); for (int i = 0; i < 10; i++) pushBack(l, makeBook("T", "A", i)); // 9
      bool ok = (l.count == 10 && l.head->data.year == 0 && l.tail->data.year == 9);
      clearList(l); all &= report(9, "10 вузлів у кінець", ok, l); }
    { List l; initList(l); for (int i = 0; i < 10; i++) pushFront(l, makeBook("T", "A", i)); // 10
      bool ok = (l.count == 10 && l.head->data.year == 9 && l.tail->data.year == 0);
      clearList(l); all &= report(10, "10 вузлів на початок", ok, l); }
    { List l; initList(l); pushBack(l, A); pushBack(l, B); pushBack(l, C); // 11
      int first = traverse(l, false), second = traverse(l, false);
      bool ok = (first == second && first == 3 && strcmp(firstTitle(l), "Кобзар") == 0);
      clearList(l); all &= report(11, "два перегляди однаковий результат", ok, l); }
    { List l; initList(l); // 12
      for (int i = 0; i < 100; i++) pushBack(l, makeBook("T", "A", i));
      bool ok = (l.count == 100);
      clearList(l);
      ok = ok && l.head == nullptr && l.tail == nullptr && l.count == 0;
      all &= report(12, "100 вузлів: створення й очищення", ok, l); }
    // власні тести
    { List l; initList(l); pushBack(l, A); pushBack(l, B); clearList(l); // 13
      pushBack(l, D); pushBack(l, C);
      bool ok = (traverse(l, false) == 2 && strcmp(firstTitle(l), "Земля") == 0 && checkStructure(l)[0] == 0);
      clearList(l); all &= report(13, "власний: повторне створення після очищення", ok, l); }
    { bool ok = true; // 14: два послідовні цикли створення і очищення
      List l; initList(l);
      for (int r = 0; r < 2; r++)
      {
          for (int i = 0; i < 5; i++) pushBack(l, A);
          if (traverse(l, false) != 5) ok = false;
          clearList(l);
      }
      all &= report(14, "власний: два цикли створення і очищення", ok, l); }
    { List l; initList(l); pushBack(l, A); pushBack(l, B); pushBack(l, C); // 15: діагностика і перевірка структури
      bool ok = (checkStructure(l)[0] == 0);
      // штучно пошкоджуємо кількість, щоб перевірити, що помилка виявляється
      l.count = 5; bool detected = (checkStructure(l)[0] != 0);
      l.count = 3;
      clearList(l); all &= report(15, "власний: діагностика і виявлення помилки", ok && detected, l); }
    return all;
}

// Варіант 2: книга — назва, автор, рік (умовні дані для демонстрації)
void variantRun()
{
    cout << "\n=== Варіант 2: створення у двох режимах ===" << endl;
    Book books[4] = {
        makeBook("Кобзар", "Шевченко", 1840),
        makeBook("Лісова пісня", "Коцюбинський", 1913),
        makeBook("Гайдамаки", "Шевченко", 1841),
        makeBook("Земля", "Довженко", 1930) };
    List a; initList(a);
    for (int i = 0; i < 4; i++) pushBack(a, books[i]);
    cout << "У кінець (порядок введення):" << endl; traverse(a, true);
    List b; initList(b);
    for (int i = 0; i < 4; i++) pushFront(b, books[i]);
    cout << "На початок (обернений порядок):" << endl; traverse(b, true);
    cout << "Діагностика списку (у кінець):" << endl; diagnostics(a);
    cout << "Перевірка структури: " << (checkStructure(a)[0] == 0 ? "коректна" : checkStructure(a)) << endl;
    clearList(a); clearList(b);
    cout << "Після очищення: активних вузлів=" << liveNodes() << endl;
}

void experiment()
{
    cout << "\n=== Експеримент: операції створення і перегляду ===" << endl;
    cout << "Вузлів\tСтворення (вузлів)\tПерегляд (кроків)" << endl;
    int sizes[3] = { 10, 100, 1000 };
    ofstream csv("results.csv");
    csv << "nodes,create_ops,traverse_steps\n";
    for (int i = 0; i < 3; i++)
    {
        List l; initList(l);
        int before = gMade;
        for (int k = 0; k < sizes[i]; k++) pushBack(l, makeBook("T", "A", k));
        int created = gMade - before;
        int steps = traverse(l, false);
        cout << sizes[i] << "\t" << created << "\t\t\t" << steps << endl;
        csv << sizes[i] << "," << created << "," << steps << "\n";
        clearList(l);
    }
}

int main()
{
    bool ok = runAllTests();
    if (!ok) { cout << "\nЄ помилки - експеримент не виконується." << endl; return 1; }
    variantRun();
    experiment();
    cout << "\nФінальний баланс: створено=" << gMade << ", звільнено=" << gFreed
         << ", активних вузлів=" << liveNodes() << endl;
    return 0;
}
