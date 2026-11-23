#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
using namespace std;

// Облік вузлів усіх трьох типів.
int gMade = 0, gFreed = 0;
int liveNodes() { return gMade - gFreed; }

// ---------- Двонапрямлений список ----------
struct DNode { int key; DNode* prev; DNode* next; };
struct DList { DNode* head; DNode* tail; int count; };

void initD(DList& l) { l.head = nullptr; l.tail = nullptr; l.count = 0; }

DNode* newD(int key) { DNode* n = new DNode; gMade++; n->key = key; n->prev = nullptr; n->next = nullptr; return n; }
void freeD(DNode* n) { delete n; gFreed++; }

DNode* dAt(const DList& l, int pos)
{
    if (pos < 0 || pos >= l.count) return nullptr;
    DNode* cur = l.head;
    for (int i = 0; i < pos; i++) cur = cur->next;
    return cur;
}

// Вставлення з оновленням обох зв’язків з сусідами.
bool dInsert(DList& l, int pos, int key)
{
    if (pos < 0 || pos > l.count) return false;
    DNode* n = newD(key);
    DNode* nextNode = (pos == l.count) ? nullptr : dAt(l, pos);
    DNode* prevNode = (nextNode == nullptr) ? l.tail : nextNode->prev;
    n->prev = prevNode; n->next = nextNode;
    if (prevNode) prevNode->next = n; else l.head = n;
    if (nextNode) nextNode->prev = n; else l.tail = n;
    l.count++;
    return true;
}

// Видалення: спочатку з’єднуємо сусідів, потім звільняємо вузол.
bool dRemoveAt(DList& l, int pos)
{
    DNode* t = dAt(l, pos);
    if (!t) return false;
    DNode* p = t->prev; DNode* q = t->next;
    if (p) p->next = q; else l.head = q;
    if (q) q->prev = p; else l.tail = p;
    freeD(t);
    l.count--;
    return true;
}

// Прямий і зворотний обходи: повертають послідовність ключів.
vector<int> dForward(const DList& l)
{
    vector<int> v;
    for (DNode* c = l.head; c; c = c->next) v.push_back(c->key);
    return v;
}
vector<int> dBackward(const DList& l)
{
    vector<int> v;
    for (DNode* c = l.tail; c; c = c->prev) v.push_back(c->key);
    return v;
}

// Перевірка взаємності зв’язків і меж.
const char* dCheck(const DList& l)
{
    if (l.head == nullptr)
        return (l.tail == nullptr && l.count == 0) ? "" : "некоректний порожній стан";
    if (l.head->prev != nullptr) return "зовнішній попередній зв’язок початку не порожній";
    if (l.tail->next != nullptr) return "зовнішній наступний зв’язок кінця не порожній";
    int n = 0; DNode* prev = nullptr;
    for (DNode* c = l.head; c; c = c->next, n++)
    {
        if (c->prev != prev) return "порушена взаємність зв’язків";
        if (n > l.count) return "можливий цикл";
        prev = c;
    }
    if (n != l.count) return "неправильна кількість";
    if (prev != l.tail) return "хибний кінець";
    return "";
}

void clearD(DList& l)
{
    DNode* c = l.head;
    while (c) { DNode* nx = c->next; freeD(c); c = nx; }
    initD(l);
}

void buildD(DList& l, const vector<int>& vals)
{
    clearD(l);
    for (int v : vals) dInsert(l, l.count, v);
}

// ---------- Циклічний однонапрямлений список ----------
struct CNode { int key; CNode* next; };
struct CList { CNode* head; int count; };

void initC(CList& l) { l.head = nullptr; l.count = 0; }
CNode* newC(int key) { CNode* n = new CNode; gMade++; n->key = key; n->next = nullptr; return n; }
void freeC(CNode* n) { delete n; gFreed++; }

// Вставлення у цикл на позицію 0..count-1; при count == 0 створюється вузол, що вказує на себе.
void cInsert(CList& l, int pos, int key)
{
    CNode* n = newC(key);
    if (l.count == 0) { n->next = n; l.head = n; l.count = 1; return; }
    CNode* prev = l.head;
    for (int i = 0; i < pos - 1 && i < l.count - 1; i++) prev = prev->next;
    if (pos == 0)
    {
        CNode* last = l.head;
        for (int i = 1; i < l.count; i++) last = last->next;
        n->next = l.head; last->next = n; l.head = n;
    }
    else { n->next = prev->next; prev->next = n; }
    l.count++;
}

// Обхід заданої кількості кроків від початку; повертає ключі та адресу останнього вузла.
vector<int> cSteps(const CList& l, int steps)
{
    vector<int> v;
    CNode* c = l.head;
    for (int i = 0; i < steps && c; i++) { v.push_back(c->key); c = c->next; }
    return v;
}

// Перевірка: після count кроків від початку має бути повернення до початку, без передчасного повторення.
const char* cCheck(const CList& l)
{
    if (l.head == nullptr) return (l.count == 0) ? "" : "некоректний порожній стан";
    CNode* c = l.head; int n = 0;
    do { c = c->next; n++; if (n > l.count) return "незамкнений або довший цикл"; } while (c != l.head);
    if (n != l.count) return "неправильна довжина циклу";
    return "";
}

void clearC(CList& l)
{
    if (l.head == nullptr) { initC(l); return; }
    CNode* c = l.head;
    for (int i = 0; i < l.count; i++) { CNode* nx = c->next; freeC(c); c = nx; }
    initC(l);
}

// ---------- Циклічний двонапрямлений список ----------
struct C2Node { int key; C2Node* prev; C2Node* next; };
struct C2List { C2Node* head; int count; };

void initC2(C2List& l) { l.head = nullptr; l.count = 0; }
C2Node* newC2(int key) { C2Node* n = new C2Node; gMade++; n->key = key; n->prev = n; n->next = n; return n; }
void freeC2(C2Node* n) { delete n; gFreed++; }

void c2Insert(C2List& l, int key)
{
    C2Node* n = newC2(key);
    if (l.head == nullptr) { l.head = n; }
    else
    {
        C2Node* last = l.head->prev;
        n->prev = last; n->next = l.head;
        last->next = n; l.head->prev = n;
    }
    l.count++;
}

vector<int> c2Forward(const C2List& l, int steps)
{
    vector<int> v; C2Node* c = l.head;
    for (int i = 0; i < steps && c; i++) { v.push_back(c->key); c = c->next; }
    return v;
}
vector<int> c2Backward(const C2List& l, int steps)
{
    vector<int> v; C2Node* c = l.head;
    for (int i = 0; i < steps && c; i++) { v.push_back(c->key); c = c->prev; }
    return v;
}

// Перевірка: прямий зв’язок має відповідати зворотному, а обхід повертається до початку.
const char* c2Check(const C2List& l)
{
    if (l.head == nullptr) return (l.count == 0) ? "" : "некоректний порожній стан";
    C2Node* c = l.head; int n = 0;
    do {
        if (c->next->prev != c || c->prev->next != c) return "порушена взаємність у циклі";
        c = c->next; n++;
        if (n > l.count) return "незамкнений цикл";
    } while (c != l.head);
    if (n != l.count) return "неправильна довжина";
    return "";
}

void clearC2(C2List& l)
{
    if (l.head == nullptr) { initC2(l); return; }
    C2Node* c = l.head;
    for (int i = 0; i < l.count; i++) { C2Node* nx = c->next; freeC2(c); c = nx; }
    initC2(l);
}

// ---------- Тести ----------
bool report(int number, const char* title, bool ok)
{
    cout << "Тест " << setw(2) << number << " (" << title << "): " << (ok ? "успішно" : "помилка")
         << ", активних вузлів=" << liveNodes() << endl;
    return ok && liveNodes() == 0;
}

bool runAllTests()
{
    cout << "\n=== Автоматизовані перевірки ===" << endl;
    bool all = true;
    { DList l; initD(l); // 1
      bool ok = dForward(l).empty() && dBackward(l).empty() && dCheck(l)[0] == 0;
      clearD(l); all &= report(1, "порожній: обхід", ok); }
    { DList l; initD(l); buildD(l, {5}); // 2
      bool ok = dForward(l) == vector<int>{5} && dBackward(l) == vector<int>{5} && dCheck(l)[0] == 0;
      clearD(l); all &= report(2, "один вузол: два обходи", ok); }
    { DList l; initD(l); buildD(l, {1, 2, 3}); // 3
      bool ok = dBackward(l) == vector<int>{3, 2, 1};
      clearD(l); all &= report(3, "зворотний обхід", ok); }
    { DList l; initD(l); buildD(l, {1, 3}); dInsert(l, 1, 2); // 4
      bool ok = dForward(l) == vector<int>{1, 2, 3} && dBackward(l) == vector<int>{3, 2, 1} && dCheck(l)[0] == 0;
      clearD(l); all &= report(4, "вставити 2 у середину", ok); }
    { DList l; initD(l); buildD(l, {1, 2, 3}); dRemoveAt(l, 1); // 5
      bool ok = dForward(l) == vector<int>{1, 3} && dBackward(l) == vector<int>{3, 1} && dCheck(l)[0] == 0;
      clearD(l); all &= report(5, "видалити 2 із середини", ok); }
    { DList l; initD(l); buildD(l, {7}); dRemoveAt(l, 0); // 6
      bool ok = l.head == nullptr && l.tail == nullptr && dCheck(l)[0] == 0;
      clearD(l); all &= report(6, "видалити єдиний вузол", ok); }
    { CList c; initC(c); cInsert(c, 0, 4); // 7
      bool ok = cCheck(c)[0] == 0 && cSteps(c, 4) == vector<int>{4, 4, 4, 4};
      clearC(c); all &= report(7, "циклічний: 4 кроки з одним вузлом", ok); }
    { CList c; initC(c); cInsert(c, 0, 3); cInsert(c, 1, 2); cInsert(c, 2, 1); // 8
      bool ok = cCheck(c)[0] == 0 && cSteps(c, 5) == vector<int>{3, 2, 1, 3, 2};
      clearC(c); all &= report(8, "циклічний: 5 кроків", ok); }
    { CList c; initC(c); cInsert(c, 0, 1); cInsert(c, 1, 2); cInsert(c, 2, 3); // 9
      bool ok = (c.count == 3 && cCheck(c)[0] == 0);
      clearC(c); all &= report(9, "циклічний: вставка в цикл", ok); }
    { C2List d; initC2(d); c2Insert(d, 1); c2Insert(d, 2); c2Insert(d, 3); // 10
      bool ok = c2Forward(d, 3) == vector<int>{1, 2, 3} && c2Check(d)[0] == 0;
      clearC2(d); all &= report(10, "циклічний двонапрямлений: прямий цикл", ok); }
    { C2List d; initC2(d); c2Insert(d, 1); c2Insert(d, 2); c2Insert(d, 3); // 11
      bool ok = c2Backward(d, 3) == vector<int>{1, 3, 2} && c2Check(d)[0] == 0;
      clearC2(d); all &= report(11, "циклічний двонапрямлений: зворотний цикл", ok); }
    { C2List d; initC2(d); c2Insert(d, 1); // 12
      bool ok = d.head->next == d.head && d.head->prev == d.head && c2Check(d)[0] == 0;
      clearC2(d); all &= report(12, "один вузол: зв’язки ведуть до себе", ok); }
    { DList l; initD(l); buildD(l, {1, 2, 3, 4, 5, 6, 7, 8, 9, 10}); // 13
      clearD(l); bool ok = (l.count == 0); all &= report(13, "очищення двонапрямленого (10 вузлів)", ok); }
    { CList c; initC(c); for (int i = 0; i < 10; i++) cInsert(c, c.count, i); // 13b
      clearC(c); bool ok = (c.head == nullptr); all &= report(14, "очищення циклічного (10 вузлів)", ok); }
    { C2List d; initC2(d); for (int i = 0; i < 10; i++) c2Insert(d, i); // 13c
      clearC2(d); bool ok = (d.head == nullptr); all &= report(15, "очищення циклічного двонапрямленого (10 вузлів)", ok); }
    { CList c; initC(c); cInsert(c, 0, 1); cInsert(c, 1, 2); cInsert(c, 2, 3); // 16: контроль 100 кроків без зависання
      vector<int> s = cSteps(c, 100);
      bool ok = (s.size() == 100 && s[99] == s[99 % 3] && cCheck(c)[0] == 0);
      clearC(c); all &= report(16, "циклічний: 100 кроків без зависання", ok); }
    // власні тести
    { DList l; initD(l); buildD(l, {1, 2, 3}); dInsert(l, 0, 0); dInsert(l, 4, 9); // 17: вставка на обох межах
      bool ok = dForward(l) == vector<int>{0, 1, 2, 3, 9} && dBackward(l) == vector<int>{9, 3, 2, 1, 0};
      clearD(l); all &= report(17, "власний: вставка на обох межах", ok); }
    { DList l; initD(l); buildD(l, {1, 2, 3}); dRemoveAt(l, 0); dRemoveAt(l, l.count - 1); // 18: видалення на обох межах
      bool ok = dForward(l) == vector<int>{2} && dBackward(l) == vector<int>{2} && dCheck(l)[0] == 0;
      clearD(l); all &= report(18, "власний: видалення на обох межах", ok); }
    { DList l; initD(l); buildD(l, {1, 2}); clearD(l); buildD(l, {3, 4}); // 19: два послідовні очищення
      clearD(l); buildD(l, {5}); bool ok = (dForward(l) == vector<int>{5});
      clearD(l); all &= report(19, "власний: два послідовні очищення", ok); }
    { DList l; initD(l); buildD(l, {1, 2, 3}); // 20: навмисно пошкоджений зв’язок
      l.head->next->prev = nullptr; // пошкодження
      bool detected = (dCheck(l)[0] != 0);
      l.head->next->prev = l.head; // відновлення перед очищенням
      clearD(l); all &= report(20, "власний: виявлення пошкодження зв’язку", detected); }
    return all;
}

// Варіант 2: список 8, 3, 6, 1; видалити позицію 0; циклічний обхід 9 кроків
void variantRun()
{
    cout << "\n=== Варіант 2 ===" << endl;
    DList l; initD(l); buildD(l, {8, 3, 6, 1});
    cout << "Початковий список (прямий): ";
    for (int k : dForward(l)) cout << k << " ";
    cout << endl;
    dRemoveAt(l, 0);
    cout << "Після видалення позиції 0 (прямий):  ";
    for (int k : dForward(l)) cout << k << " ";
    cout << endl;
    cout << "Після видалення позиції 0 (зворотний): ";
    for (int k : dBackward(l)) cout << k << " ";
    cout << endl;
    cout << "Перевірка двонапрямленого списку: " << (dCheck(l)[0] == 0 ? "коректна" : dCheck(l)) << endl;
    CList c; initC(c); for (int k : dForward(l)) cInsert(c, c.count, k);
    cout << "Циклічний однонапрямлений, 9 кроків: ";
    for (int k : cSteps(c, 9)) cout << k << " ";
    cout << "| перевірка: " << (cCheck(c)[0] == 0 ? "коректна" : cCheck(c)) << endl;
    C2List d; initC2(d); for (int k : dForward(l)) c2Insert(d, k);
    cout << "Циклічний двонапрямлений, 9 кроків вперед: ";
    for (int k : c2Forward(d, 9)) cout << k << " ";
    cout << endl;
    cout << "Циклічний двонапрямлений, 9 кроків назад:  ";
    for (int k : c2Backward(d, 9)) cout << k << " ";
    cout << "| перевірка: " << (c2Check(d)[0] == 0 ? "коректна" : c2Check(d)) << endl;
    clearD(l); clearC(c); clearC2(d);
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
