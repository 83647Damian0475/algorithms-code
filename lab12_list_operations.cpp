#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
using namespace std;

// Облік вузлів (створення і звільнення).
int gMade = 0, gFreed = 0;
int liveNodes() { return gMade - gFreed; }

struct Node { int key; Node* next; };
struct List { Node* head; Node* tail; int count; };

void initList(List& l) { l.head = nullptr; l.tail = nullptr; l.count = 0; }

Node* newNode(int key) { Node* n = new Node; gMade++; n->key = key; n->next = nullptr; return n; }
void freeNode(Node* n) { delete n; gFreed++; }

// Вузол на позиції pos (від 0). Повертає nullptr, якщо позиція недопустима.
Node* nodeAt(const List& l, int pos)
{
    if (pos < 0 || pos >= l.count) return nullptr;
    Node* cur = l.head;
    for (int i = 0; i < pos; i++) cur = cur->next;
    return cur;
}

// Вставлення на позицію 0..count. Повертає false, якщо позиція недопустима (список не змінюється).
bool insertAt(List& l, int pos, int key)
{
    if (pos < 0 || pos > l.count) return false;
    Node* n = newNode(key);
    if (pos == 0)
    {
        n->next = l.head; l.head = n;
        if (l.tail == nullptr) l.tail = n;
    }
    else
    {
        Node* prev = nodeAt(l, pos - 1);
        n->next = prev->next; prev->next = n;
        if (n->next == nullptr) l.tail = n;
    }
    l.count++;
    return true;
}

// Видалення за позицією. Попередній вузол знаходиться до звільнення цільового.
bool removeAt(List& l, int pos)
{
    if (pos < 0 || pos >= l.count) return false;
    Node* target;
    if (pos == 0)
    {
        target = l.head;
        l.head = target->next;
    }
    else
    {
        Node* prev = nodeAt(l, pos - 1);
        target = prev->next;
        prev->next = target->next;
        if (target == l.tail) l.tail = prev;
    }
    if (l.head == nullptr) l.tail = nullptr;
    freeNode(target);
    l.count--;
    return true;
}

// Пошук першого збігу. Повертає позицію або -1; visited – кількість переглянутих вузлів.
int findFirst(const List& l, int key, int& visited)
{
    visited = 0;
    int pos = 0;
    for (const Node* cur = l.head; cur != nullptr; cur = cur->next, pos++)
    {
        visited++;
        if (cur->key == key) return pos;
    }
    return -1;
}

// Усі збіги: повертає кількість, позиції записуються у вектор.
int findAll(const List& l, int key, vector<int>& positions)
{
    positions.clear();
    int pos = 0;
    for (const Node* cur = l.head; cur != nullptr; cur = cur->next, pos++)
        if (cur->key == key) positions.push_back(pos);
    return (int)positions.size();
}

// Видалення першого збігу.
bool removeFirstKey(List& l, int key)
{
    int v; int p = findFirst(l, key, v);
    if (p < 0) return false;
    return removeAt(l, p);
}

// Видалення всіх збігів: обхід із збереженням попереднього вузла, без звернення до звільненої пам’яті.
int removeAllKey(List& l, int key)
{
    int removed = 0;
    Node* prev = nullptr;
    Node* cur = l.head;
    while (cur != nullptr)
    {
        Node* nxt = cur->next;
        if (cur->key == key)
        {
            if (prev == nullptr) l.head = nxt; else prev->next = nxt;
            if (cur == l.tail) l.tail = prev;
            freeNode(cur);
            l.count--; removed++;
        }
        else prev = cur;
        cur = nxt;
    }
    if (l.head == nullptr) l.tail = nullptr;
    return removed;
}

// Вставлення перед або після першого збігу. Повертає false, якщо ключа немає.
bool insertBefore(List& l, int target, int key)
{
    int v; int p = findFirst(l, target, v);
    if (p < 0) return false;
    return insertAt(l, p, key);
}
bool insertAfter(List& l, int target, int key)
{
    int v; int p = findFirst(l, target, v);
    if (p < 0) return false;
    return insertAt(l, p + 1, key);
}

void clearList(List& l)
{
    Node* cur = l.head;
    while (cur != nullptr) { Node* nxt = cur->next; freeNode(cur); cur = nxt; }
    initList(l);
}

// Перевірка інваріантів: кількість, кінець, відсутність циклу (обмеження кроків).
const char* checkInvariants(const List& l)
{
    if (l.head == nullptr)
    {
        if (l.tail != nullptr || l.count != 0) return "порожній стан некоректний";
        return "";
    }
    int limit = l.count + 1, steps = 0;
    const Node* cur = l.head; const Node* last = nullptr;
    while (cur != nullptr) { steps++; if (steps > limit) return "можливий цикл"; last = cur; cur = cur->next; }
    if (steps != l.count) return "неправильна кількість";
    if (last != l.tail) return "хибний кінець";
    return "";
}

// Знімок послідовності ключів для порівняння до і після невдалої операції.
vector<int> snapshot(const List& l)
{
    vector<int> v;
    for (const Node* cur = l.head; cur != nullptr; cur = cur->next) v.push_back(cur->key);
    return v;
}

// Створення списку з масиву значень.
void buildFrom(List& l, const vector<int>& vals)
{
    clearList(l);
    for (int v : vals) insertAt(l, l.count, v);
}

bool sameSeq(const vector<int>& a, const vector<int>& b) { return a == b; }

bool report(int number, const char* title, bool ok, const List& l)
{
    cout << "Тест " << setw(2) << number << " (" << title << "): " << (ok ? "успішно" : "помилка")
         << ", вузлів=" << l.count << ", інваріанти=" << (checkInvariants(l)[0] ? checkInvariants(l) : "ок")
         << ", активних вузлів=" << liveNodes() << endl;
    return ok && liveNodes() == 0 && checkInvariants(l)[0] == 0;
}

bool runAllTests()
{
    cout << "\n=== Автоматизовані перевірки ===" << endl;
    bool all = true;
    { List l; initList(l); // 1
      int v; bool ok = (findFirst(l, 5, v) == -1 && v == 0);
      clearList(l); all &= report(1, "порожній: пошук 5", ok, l); }
    { List l; initList(l); // 2
      bool ok = insertAt(l, 0, 5) && snapshot(l) == vector<int>{5};
      clearList(l); all &= report(2, "порожній: вставити 5 на 0", ok, l); }
    { List l; initList(l); buildFrom(l, {5}); // 3
      bool ok = removeAt(l, 0) && snapshot(l).empty() && l.tail == nullptr;
      clearList(l); all &= report(3, "видалити єдиний вузол", ok, l); }
    { List l; initList(l); buildFrom(l, {1, 3}); // 4
      bool ok = insertAt(l, 1, 2) && snapshot(l) == vector<int>{1, 2, 3};
      clearList(l); all &= report(4, "вставити 2 на 1", ok, l); }
    { List l; initList(l); buildFrom(l, {1, 2}); // 5
      bool ok = insertAt(l, 0, 0) && snapshot(l) == vector<int>{0, 1, 2};
      clearList(l); all &= report(5, "вставити 0 на 0", ok, l); }
    { List l; initList(l); buildFrom(l, {1, 2}); // 6
      bool ok = insertAt(l, 2, 3) && snapshot(l) == vector<int>{1, 2, 3} && l.tail->key == 3;
      clearList(l); all &= report(6, "вставити 3 на 2 (кінець)", ok, l); }
    { List l; initList(l); buildFrom(l, {1, 2, 3}); // 7
      bool ok = removeAt(l, 0) && snapshot(l) == vector<int>{2, 3};
      clearList(l); all &= report(7, "видалити позицію 0", ok, l); }
    { List l; initList(l); buildFrom(l, {1, 2, 3}); // 8
      bool ok = removeAt(l, 1) && snapshot(l) == vector<int>{1, 3};
      clearList(l); all &= report(8, "видалити позицію 1", ok, l); }
    { List l; initList(l); buildFrom(l, {1, 2, 3}); // 9
      bool ok = removeAt(l, 2) && snapshot(l) == vector<int>{1, 2} && l.tail->key == 2;
      clearList(l); all &= report(9, "видалити останню позицію", ok, l); }
    { List l; initList(l); buildFrom(l, {1, 2, 2, 3}); // 10
      bool ok = removeFirstKey(l, 2) && snapshot(l) == vector<int>{1, 2, 3};
      clearList(l); all &= report(10, "видалити перше 2", ok, l); }
    { List l; initList(l); buildFrom(l, {2, 2, 1, 2}); // 11
      bool ok = (removeAllKey(l, 2) == 3) && snapshot(l) == vector<int>{1} && l.tail->key == 1;
      clearList(l); all &= report(11, "видалити всі 2", ok, l); }
    { List l; initList(l); buildFrom(l, {1, 2, 3}); // 12
      vector<int> before = snapshot(l);
      bool ok = (insertAt(l, 5, 9) == false) && snapshot(l) == before;
      clearList(l); all &= report(12, "вставити на позицію 5 (відмова)", ok, l); }
    { List l; initList(l); buildFrom(l, {1, 2, 3}); // 13
      vector<int> before = snapshot(l);
      bool ok = (removeAt(l, -1) == false) && snapshot(l) == before;
      clearList(l); all &= report(13, "видалити позицію −1 (відмова)", ok, l); }
    { List l; initList(l); buildFrom(l, {1, 2, 3}); // 14
      bool ok = insertAfter(l, 3, 9) && snapshot(l) == vector<int>{1, 2, 3, 9} && l.tail->key == 9;
      clearList(l); all &= report(14, "вставити 9 після 3", ok, l); }
    { List l; initList(l); buildFrom(l, {1, 2, 3}); // 15
      bool ok = insertBefore(l, 1, 0) && snapshot(l) == vector<int>{0, 1, 2, 3};
      clearList(l); all &= report(15, "вставити 0 перед 1", ok, l); }
    // власні тести
    { List l; initList(l); buildFrom(l, {4, 4, 4}); // 16: послідовні однакові ключі
      bool ok = (removeAllKey(l, 4) == 3) && snapshot(l).empty();
      clearList(l); all &= report(16, "власний: послідовні однакові ключі", ok, l); }
    { List l; initList(l); buildFrom(l, {1, 2}); removeAllKey(l, 1); removeAllKey(l, 2); // 17: повторне заповнення
      buildFrom(l, {7, 8}); bool ok = snapshot(l) == vector<int>{7, 8};
      clearList(l); all &= report(17, "власний: повторне заповнення після видалення", ok, l); }
    { List l; initList(l); buildFrom(l, {1, 2, 3, 4, 5}); // 18: пошук усіх збігів
      buildFrom(l, {6, 2, 6, 2, 6}); vector<int> pos; findAll(l, 6, pos); bool ok = pos == vector<int>{0, 2, 4};
      clearList(l); all &= report(18, "власний: пошук усіх збігів", ok, l); }
    { List l; initList(l); buildFrom(l, {1, 2, 3}); // 19: видалення після останнього вузла
      bool ok = (removeAt(l, 3) == false) && snapshot(l) == vector<int>{1, 2, 3};
      clearList(l); all &= report(19, "власний: позиція count (відмова)", ok, l); }
    { List l; initList(l); buildFrom(l, {3, 1, 4, 1, 5, 9, 2, 6, 5, 3}); // 20: серія змішаних операцій
      insertAt(l, 0, 7); insertAt(l, l.count, 8); insertAt(l, 5, 0);
      removeAt(l, 2); removeAllKey(l, 1); insertAfter(l, 9, 4); removeFirstKey(l, 5);
      insertBefore(l, 6, 2); removeAt(l, 0);
      bool ok = (checkInvariants(l)[0] == 0);
      clearList(l); all &= report(20, "власний: серія змішаних операцій", ok, l); }
    return all;
}

// Варіант 2: список 8, 3, 6, 1, 3; пошук 6; вставити 7 на позицію 0; видалити всі 3
void variantRun()
{
    cout << "\n=== Варіант 2: журнал операцій ===" << endl;
    List l; initList(l); buildFrom(l, {8, 3, 6, 1, 3});
    auto show = [&](const char* step) {
        cout << step << ": [";
        bool first = true;
        for (int k : snapshot(l)) { cout << (first ? "" : ", ") << k; first = false; }
        cout << "], вузлів=" << l.count << ", інваріанти=" << (checkInvariants(l)[0] ? checkInvariants(l) : "ок") << endl;
    };
    show("Початковий список");
    int visited; int pos = findFirst(l, 6, visited);
    cout << "Пошук 6: позиція=" << pos << ", переглянуто вузлів=" << visited << endl;
    insertAt(l, 0, 7);
    show("Вставити 7 на позицію 0");
    int removed = removeAllKey(l, 3);
    cout << "Видалити всі 3: видалено=" << removed << endl;
    show("Після видалення");
    clearList(l);
    cout << "Після очищення: активних вузлів=" << liveNodes() << endl;
}

int main()
{
    bool ok = runAllTests();
    if (!ok) { cout << "\nЄ помилки - сценарій не виконується." << endl; return 1; }
    variantRun();
    cout << "\nФінальний баланс: створено=" << gMade << ", звільнено=" << gFreed
         << ", активних вузлів=" << liveNodes() << endl;
    return 0;
}
