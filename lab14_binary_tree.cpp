#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <string>
using namespace std;

// Облік вузлів: створення і звільнення.
int gMade = 0, gFreed = 0;
int liveNodes() { return gMade - gFreed; }

struct Node { int val; Node* left; Node* right; };

Node* newNode(int v) { Node* n = new Node; gMade++; n->val = v; n->left = nullptr; n->right = nullptr; return n; }

void freeTree(Node* n)
{
    if (!n) return;
    freeTree(n->left);
    freeTree(n->right);
    delete n; gFreed++;
}

// ---------- Створення за префіксним описом ('#' – порожнє піддерево) ----------
Node* parsePrefix(const vector<string>& tok, size_t& i)
{
    if (i >= tok.size()) return nullptr;
    string t = tok[i++];
    if (t == "#") return nullptr;
    Node* n = newNode(stoi(t));
    n->left = parsePrefix(tok, i);
    n->right = parsePrefix(tok, i);
    return n;
}

Node* buildFromPrefix(const string& desc)
{
    istringstream in(desc);
    vector<string> tok; string w;
    while (in >> w) tok.push_back(w);
    size_t i = 0;
    return parsePrefix(tok, i);
}

// ---------- Вставка за правилом: менше — ліворуч, більше — праворуч ----------
Node* insertBST(Node* root, int v)
{
    if (!root) return newNode(v);
    if (v < root->val) root->left = insertBST(root->left, v);
    else root->right = insertBST(root->right, v);
    return root;
}

// ---------- Рекурсивні обходи ----------
void preorder(const Node* n, vector<int>& out) { if (!n) return; out.push_back(n->val); preorder(n->left, out); preorder(n->right, out); }
void inorder(const Node* n, vector<int>& out) { if (!n) return; inorder(n->left, out); out.push_back(n->val); inorder(n->right, out); }
void postorder(const Node* n, vector<int>& out) { if (!n) return; postorder(n->left, out); postorder(n->right, out); out.push_back(n->val); }

// Ітеративний симетричний обхід на власному стеку з масиву.
void inorderIterative(const Node* root, vector<int>& out)
{
    const Node* stack[256]; int top = 0;
    const Node* cur = root;
    while (cur != nullptr || top > 0)
    {
        while (cur != nullptr) { stack[top++] = cur; cur = cur->left; }
        cur = stack[--top];
        out.push_back(cur->val);
        cur = cur->right;
    }
}

// ---------- Характеристики ----------
int countNodes(const Node* n) { return n ? 1 + countNodes(n->left) + countNodes(n->right) : 0; }
int countLeaves(const Node* n)
{
    if (!n) return 0;
    if (!n->left && !n->right) return 1;
    return countLeaves(n->left) + countLeaves(n->right);
}
// Висота: порожнє дерево – 0, один вузол – 1.
int height(const Node* n) { return n ? 1 + max(height(n->left), height(n->right)) : 0; }

// Перевірка: кількість вузлів, рекурсивний і ітеративний обходи однакові, немає повторного посилання.
bool hasRepeatedRef(const Node* n, vector<const Node*>& seen)
{
    if (!n) return false;
    for (const Node* s : seen) if (s == n) return true;
    seen.push_back(n);
    return hasRepeatedRef(n->left, seen) || hasRepeatedRef(n->right, seen);
}

string fmt(const vector<int>& v)
{
    ostringstream o;
    for (size_t i = 0; i < v.size(); i++) { if (i) o << ", "; o << v[i]; }
    return v.empty() ? "порожньо" : o.str();
}

bool reportTree(int number, const char* title, Node* root, const vector<int>& pre, const vector<int>& in, const vector<int>& post)
{
    vector<int> p, i2, q, it;
    preorder(root, p); inorder(root, i2); postorder(root, q); inorderIterative(root, it);
    vector<const Node*> seen;
    bool ok = (p == pre) && (i2 == in) && (q == post) && (it == in) && !hasRepeatedRef(root, seen);
    cout << "Дерево " << setw(2) << number << " (" << title << "): " << (ok ? "успішно" : "помилка")
         << ", вузлів=" << countNodes(root) << ", листків=" << countLeaves(root) << ", висота=" << height(root) << endl;
    freeTree(root);
    return ok && liveNodes() == 0;
}

bool runAllTests()
{
    cout << "\n=== Автоматизовані перевірки ===" << endl;
    bool all = true;
    struct Case { int n; const char* desc; vector<int> pre, in, post; const char* title; };
    vector<Case> cs = {
        {1, "#", {}, {}, {}, "порожнє"},
        {2, "5 # #", {5}, {5}, {5}, "один вузол"},
        {3, "2 1 # # 3 # #", {2, 1, 3}, {1, 2, 3}, {1, 3, 2}, "два піддерева"},
        {4, "3 2 1 # # # #", {3, 2, 1}, {1, 2, 3}, {1, 2, 3}, "вироджене ліворуч"},
        {5, "1 # 2 # 3 # #", {1, 2, 3}, {1, 2, 3}, {3, 2, 1}, "вироджене праворуч"},
        {6, "4 2 # # 6 # #", {4, 2, 6}, {2, 4, 6}, {2, 6, 4}, "неповний"},
        {7, "5 3 2 # # 4 # # 7 6 # # 8 # #", {5, 3, 2, 4, 7, 6, 8}, {2, 3, 4, 5, 6, 7, 8}, {2, 4, 3, 6, 8, 7, 5}, "змішаний"},
        {8, "1 2 # # 3 4 # # #", {1, 2, 3, 4}, {2, 1, 4, 3}, {2, 4, 3, 1}, "розгалуження"},
        {9, "0 -2 # # 5 # #", {0, -2, 5}, {-2, 0, 5}, {-2, 5, 0}, "від’ємні числа"},
        {10, "4 4 # # 4 # #", {4, 4, 4}, {4, 4, 4}, {4, 4, 4}, "повтори"},
    };
    for (auto& c : cs) all &= reportTree(c.n, c.title, buildFromPrefix(c.desc), c.pre, c.in, c.post);
    // власні дерева: очікування формуються з окремої перевірки (sorted inorder)
    struct Own { int n; const char* desc; const char* title; };
    vector<Own> own = {
        {11, "4 2 1 # # 3 # # 6 5 # # 7 # #", "власне: повне за структурою"},
        {12, "5 4 3 2 1 # # # # # #", "власне: вироджене ліворуч, 5 вузлів"},
        {13, "1 # 2 # 3 # 4 # 5 # #", "власне: вироджене праворуч, 5 вузлів"},
        {14, "6 3 1 # # 4 # # 8 # 9 # #", "власне: неповне"},
        {15, "4 4 4 # # # #", "власне: повтори значень"},
    };
    for (auto& o : own)
    {
        Node* r = buildFromPrefix(o.desc);
        vector<int> p, i2, q, it;
        preorder(r, p); inorder(r, i2); postorder(r, q); inorderIterative(r, it);
        bool ok = (i2 == it) && (static_cast<int>(i2.size()) == countNodes(r)) && (static_cast<int>(p.size()) == countNodes(r));
        cout << "Дерево " << setw(2) << o.n << " (" << o.title << "): " << (ok ? "успішно" : "помилка")
             << ", вузлів=" << countNodes(r) << ", листків=" << countLeaves(r) << ", висота=" << height(r) << endl;
        freeTree(r);
        all &= (ok && liveNodes() == 0);
    }
    return all;
}

// Варіант 2: 45, 25, 65, 15, 35, 55, 75 вставляються в порядку введення
void variantRun()
{
    cout << "\n=== Варіант 2 ===" << endl;
    vector<int> seq = {45, 25, 65, 15, 35, 55, 75};
    Node* root = nullptr;
    for (int v : seq) root = insertBST(root, v);
    vector<int> p, i2, q;
    preorder(root, p); inorder(root, i2); postorder(root, q);
    cout << "Прямий обхід:       " << fmt(p) << endl;
    cout << "Симетричний обхід:  " << fmt(i2) << endl;
    cout << "Зворотний обхід:    " << fmt(q) << endl;
    vector<int> itv; inorderIterative(root, itv);
    cout << "Ітеративний симетричний: " << (itv == i2 ? "збігається з рекурсивним" : "не збігається") << endl;
    cout << "Вузлів=" << countNodes(root) << ", листків=" << countLeaves(root)
         << ", внутрішніх=" << countNodes(root) - countLeaves(root) << ", висота=" << height(root) << endl;
    freeTree(root);
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
