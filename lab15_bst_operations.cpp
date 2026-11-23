#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <climits>
using namespace std;

// Облік вузлів.
int gMade = 0, gFreed = 0;
int liveNodes() { return gMade - gFreed; }

struct Node { int key; Node* left; Node* right; };
struct BST { Node* root; };

void initT(BST& t) { t.root = nullptr; }
Node* newNode(int k) { Node* n = new Node; gMade++; n->key = k; n->left = nullptr; n->right = nullptr; return n; }
void freeNode(Node* n) { delete n; gFreed++; }

void freeTree(Node* n) { if (!n) return; freeTree(n->left); freeTree(n->right); freeNode(n); }
void clearT(BST& t) { freeTree(t.root); t.root = nullptr; }

// Пошук ітеративно: повертає вузол або nullptr; comparisons – кількість порівнянь ключів; path – шлях від кореня.
Node* search(const BST& t, int key, long long& comparisons, vector<int>* path = nullptr)
{
    comparisons = 0;
    Node* cur = t.root;
    while (cur != nullptr)
    {
        if (path) path->push_back(cur->key);
        comparisons++;
        if (key == cur->key) return cur;
        cur = (key < cur->key) ? cur->left : cur->right;
    }
    return nullptr;
}

// Рекурсивний пошук для порівняння.
Node* searchRec(Node* n, int key, long long& comparisons)
{
    if (!n) return nullptr;
    comparisons++;
    if (key == n->key) return n;
    return searchRec(key < n->key ? n->left : n->right, key, comparisons);
}

// Вставлення: повторні ключі заборонено (дерево не змінюється, повертається false).
bool insertKey(BST& t, int key)
{
    Node** link = &t.root;
    while (*link != nullptr)
    {
        if (key == (*link)->key) return false;
        link = (key < (*link)->key) ? &(*link)->left : &(*link)->right;
    }
    *link = newNode(key);
    return true;
}

// Видалення з трьома випадками. Для двох нащадків замінник – найбільший ключ лівого піддерева.
bool removeKey(BST& t, int key)
{
    Node** link = &t.root;
    while (*link != nullptr && (*link)->key != key)
        link = (key < (*link)->key) ? &(*link)->left : &(*link)->right;
    if (*link == nullptr) return false;
    Node* target = *link;
    if (target->left == nullptr) { *link = target->right; freeNode(target); return true; }      // листок або один нащадок (праворуч)
    if (target->right == nullptr) { *link = target->left; freeNode(target); return true; }      // один нащадок (ліворуч)
    // два нащадки: знайти замінник – максимум лівого піддерева
    Node** repLink = &target->left;
    while ((*repLink)->right != nullptr) repLink = &(*repLink)->right;
    Node* rep = *repLink;
    *repLink = rep->left;          // замінник має не більше одного нащадка (лівого)
    target->key = rep->key;        // дані замінника переносяться до цільового вузла
    freeNode(rep);
    return true;
}

// Симетричний обхід.
void inorder(const Node* n, vector<int>& out) { if (!n) return; inorder(n->left, out); out.push_back(n->key); inorder(n->right, out); }

// Перевірка діапазонів: для кожного вузла ключ має бути в межах (lo, hi), успадкованих від предків.
const char* checkRange(const Node* n, long long lo, long long hi, int depth, int& count)
{
    if (!n) return "";
    if (depth > 1000) return "можливий цикл";
    count++;
    if (n->key <= lo || n->key >= hi) return "порушено діапазон";
    const char* e = checkRange(n->left, lo, n->key, depth + 1, count);
    if (e[0]) return e;
    return checkRange(n->right, n->key, hi, depth + 1, count);
}

const char* checkBST(const BST& t)
{
    int count = 0;
    const char* e = checkRange(t.root, LLONG_MIN, LLONG_MAX, 0, count);
    if (e[0]) return e;
    vector<int> io; inorder(t.root, io);
    for (size_t i = 1; i < io.size(); i++) if (io[i] <= io[i - 1]) return "порушено порядок обходу";
    return "";
}

int countNodes(const Node* n) { return n ? 1 + countNodes(n->left) + countNodes(n->right) : 0; }
int heightOf(const Node* n) { return n ? 1 + max(heightOf(n->left), heightOf(n->right)) : 0; }

string joinVec(const vector<int>& v)
{
    string s;
    for (size_t i = 0; i < v.size(); i++) { if (i) s += ", "; s += to_string(v[i]); }
    return v.empty() ? "порожньо" : s;
}

bool report(int number, const char* title, bool ok, const BST& t)
{
    cout << "Тест " << setw(2) << number << " (" << title << "): " << (ok ? "успішно" : "помилка")
         << ", вузлів=" << countNodes(t.root) << ", інваріанти=" << (checkBST(t)[0] ? checkBST(t) : "ок")
         << ", активних вузлів=" << liveNodes() << endl;
    return ok && checkBST(t)[0] == 0 && liveNodes() == 0;
}

// Побудова дерева послідовною вставкою.
void buildT(BST& t, const vector<int>& keys) { clearT(t); for (int k : keys) insertKey(t, k); }

bool runAllTests()
{
    cout << "\n=== Автоматизовані перевірки ===" << endl;
    bool all = true;
    auto inorderOf = [](const BST& t) { vector<int> v; inorder(t.root, v); return v; };
    { BST t; initT(t); // 1
      long long c; bool ok = (search(t, 5, c) == nullptr && c == 0) && inorderOf(t).empty();
      clearT(t); all &= report(1, "порожнє: пошук 5", ok, t); }
    { BST t; initT(t); // 2
      bool ok = insertKey(t, 5) && inorderOf(t) == vector<int>{5};
      clearT(t); all &= report(2, "порожнє: вставити 5", ok, t); }
    { BST t; initT(t); buildT(t, {5}); // 3
      bool ok = removeKey(t, 5) && t.root == nullptr;
      clearT(t); all &= report(3, "видалити єдиний корінь", ok, t); }
    { BST t; initT(t); buildT(t, {5, 3, 7}); // 4
      bool ok = insertKey(t, 4) && inorderOf(t) == vector<int>{3, 4, 5, 7};
      clearT(t); all &= report(4, "вставити 4", ok, t); }
    { BST t; initT(t); buildT(t, {5, 3, 7}); // 5
      bool ok = (insertKey(t, 5) == false) && inorderOf(t) == vector<int>{3, 5, 7};
      clearT(t); all &= report(5, "повторний ключ відхиляється", ok, t); }
    { BST t; initT(t); buildT(t, {5, 3, 7}); // 6
      bool ok = removeKey(t, 3) && inorderOf(t) == vector<int>{5, 7};
      clearT(t); all &= report(6, "видалити листок 3", ok, t); }
    { BST t; initT(t); buildT(t, {5, 3, 7, 6}); // 7
      bool ok = removeKey(t, 7) && inorderOf(t) == vector<int>{3, 5, 6};
      clearT(t); all &= report(7, "видалити вузол з одним нащадком", ok, t); }
    { BST t; initT(t); buildT(t, {5, 3, 8, 7, 9}); // 8
      bool ok = removeKey(t, 8) && inorderOf(t) == vector<int>{3, 5, 7, 9};
      clearT(t); all &= report(8, "видалити вузол з двома нащадками", ok, t); }
    { BST t; initT(t); buildT(t, {5, 3, 8, 7, 9}); // 9
      bool ok = removeKey(t, 5) && inorderOf(t) == vector<int>{3, 7, 8, 9};
      clearT(t); all &= report(9, "видалити корінь з двома нащадками", ok, t); }
    { BST t; initT(t); buildT(t, {5, 3, 7}); // 10
      bool ok = (removeKey(t, 10) == false) && inorderOf(t) == vector<int>{3, 5, 7};
      clearT(t); all &= report(10, "видалити відсутній ключ", ok, t); }
    { BST t; initT(t); buildT(t, {1, 2, 3, 4}); // 11
      vector<int> path; long long c; bool ok = (search(t, 4, c, &path) != nullptr && path.size() == 4);
      clearT(t); all &= report(11, "пошук у вироджене праворуч: 4 вузли шляху", ok, t); }
    { BST t; initT(t); buildT(t, {4, 2, 6, 1, 3, 5, 7}); // 12
      vector<int> path; long long c; bool ok = (search(t, 7, c, &path) != nullptr && path.size() == 3);
      clearT(t); all &= report(12, "пошук 7: 3 вузли шляху", ok, t); }
    { BST t; initT(t); buildT(t, {5, 3, 7, 6}); // 13
      bool ok = removeKey(t, 5) && inorderOf(t) == vector<int>{3, 6, 7};
      clearT(t); all &= report(13, "видалити корінь 5", ok, t); }
    { BST t; initT(t); buildT(t, {2, 1}); // 14
      bool ok = removeKey(t, 2) && inorderOf(t) == vector<int>{1};
      clearT(t); all &= report(14, "видалити корінь 2 з лівим нащадком", ok, t); }
    { BST t; initT(t); buildT(t, {2, 3}); // 15
      bool ok = removeKey(t, 2) && inorderOf(t) == vector<int>{3};
      clearT(t); all &= report(15, "видалити корінь 2 з правим нащадком", ok, t); }
    // власні сценарії
    { BST t; initT(t); buildT(t, {5, 3, 7}); // 16: повторне видалення
      bool ok = removeKey(t, 3) && (removeKey(t, 3) == false) && inorderOf(t) == vector<int>{5, 7};
      clearT(t); all &= report(16, "власний: повторне видалення", ok, t); }
    { BST t; initT(t); buildT(t, {10, 5, 15, 3, 7, 12}); // 17: замінник з одним нащадком
      // видаляємо 5: лівий підкорінь 3 (без нащадків). Для замінника з лівим нащадком: 8 має лівого нащадка 7
      buildT(t, {10, 5, 15, 3, 8, 7, 12});
      bool ok = removeKey(t, 10) && inorderOf(t) == vector<int>{3, 5, 7, 8, 12, 15};
      clearT(t); all &= report(17, "власний: замінник з одним нащадком", ok, t); }
    { BST t; initT(t); buildT(t, {4, 2, 6, 1, 3, 5, 7}); // 18: три послідовні видалення кореня
      bool ok = removeKey(t, 4) && removeKey(t, 5) && removeKey(t, 3) && inorderOf(t) == vector<int>{1, 2, 6, 7};
      clearT(t); all &= report(18, "власний: три видалення", ok, t); }
    { BST t; initT(t); buildT(t, {3, 1, 2}); // 19: очищення і повторне створення
      clearT(t); buildT(t, {9, 4}); bool ok = inorderOf(t) == vector<int>{4, 9};
      clearT(t); all &= report(19, "власний: очищення і повторне створення", ok, t); }
    { BST t; initT(t); buildT(t, {5, 2, 8, 1, 3, 7, 9}); // 20: видалення всіх ключів
      bool ok = true;
      for (int k : {5, 2, 8, 1, 3, 7, 9}) ok &= removeKey(t, k);
      ok &= (t.root == nullptr);
      clearT(t); all &= report(20, "власний: видалення всіх ключів", ok, t); }
    return all;
}

// Варіант 2: 45, 25, 65, 15, 35, 55, 75; пошук 35; вставити 50; видалити 25
void variantRun()
{
    cout << "\n=== Варіант 2 ===" << endl;
    BST t; initT(t); buildT(t, {45, 25, 65, 15, 35, 55, 75});
    cout << "Початкова симетрія: " << joinVec([&]{ vector<int> v; inorder(t.root, v); return v; }()) << endl;
    long long c; vector<int> path;
    Node* f = search(t, 35, c, &path);
    cout << "Пошук 35: " << (f ? "знайдено" : "не знайдено") << ", порівнянь=" << c << ", шлях: " << joinVec(path) << endl;
    bool ins = insertKey(t, 50);
    vector<int> v1; inorder(t.root, v1);
    cout << "Вставити 50: " << (ins ? "успішно" : "відмова") << ", симетрія: " << joinVec(v1) << endl;
    bool rem = removeKey(t, 25);
    vector<int> v2; inorder(t.root, v2);
    cout << "Видалити 25 (два нащадки, замінник 15): " << (rem ? "успішно" : "відмова") << ", симетрія: " << joinVec(v2) << endl;
    cout << "Перевірка інваріантів: " << (checkBST(t)[0] ? checkBST(t) : "коректно") << endl;
    cout << "Вузлів=" << countNodes(t.root) << ", висота=" << heightOf(t.root) << endl;
    clearT(t);
    cout << "Після очищення: активних вузлів=" << liveNodes() << endl;
}

// Експеримент: однакові ключі 1..15, три порядки вставлення.
void experiment()
{
    cout << "\n=== Експеримент: форма дерева ===" << endl;
    cout << "Форма\tВисота\tСередньо порівнянь (наявні)\tПорівнянь (відсутній 16)" << endl;
    vector<pair<const char*, vector<int>>> forms = {
        {"майже збалансована", {8, 4, 12, 2, 6, 10, 14, 1, 3, 5, 7, 9, 11, 13, 15}},
        {"зростаюча (вироджена)", {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}},
        {"перемішана", {9, 3, 14, 1, 7, 11, 5, 13, 2, 8, 4, 12, 6, 10, 15}},
    };
    ofstream csv("results.csv");
    csv << "form,height,avg_present,absent_cmp\n";
    for (auto& f : forms)
    {
        BST t; initT(t); buildT(t, f.second);
        long long total = 0;
        for (int k = 1; k <= 15; k++) { long long c; search(t, k, c); total += c; }
        long long ca; search(t, 16, ca);
        double avg = total / 15.0;
        cout << f.first << "\t" << heightOf(t.root) << "\t" << fixed << setprecision(3) << avg << "\t\t\t\t" << ca << endl;
        csv << f.first << "," << heightOf(t.root) << "," << avg << "," << ca << "\n";
        clearT(t);
    }
}

int main()
{
    bool ok = runAllTests();
    if (!ok) { cout << "\nЄ помилки - сценарій не виконується." << endl; return 1; }
    variantRun();
    experiment();
    cout << "\nФінальний баланс: створено=" << gMade << ", звільнено=" << gFreed
         << ", активних вузлів=" << liveNodes() << endl;
    return 0;
}
