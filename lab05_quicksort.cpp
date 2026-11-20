#include <iostream>
#include <iomanip>
#include <fstream>
using namespace std;

// Статистика: порівняння значень, обміни (один обмін двох елементів = 1), рекурсивні виклики, максимальна глибина.
struct Stats { long long cmp; long long swaps; long long calls; int maxDepth; };

void resetStats(Stats& s) { s.cmp = 0; s.swaps = 0; s.calls = 0; s.maxDepth = 0; }

void swapElems(int a[], int i, int j, Stats& s)
{
    if (i == j) return;
    int t = a[i]; a[i] = a[j]; a[j] = t;
    s.swaps++;
}

// ---------- Схема Ломуто: опорний — останній елемент; повертає остаточну позицію опорного ----------
int lomutoPartition(int a[], int lo, int hi, Stats& s, bool trace, int depth)
{
    int pivot = a[hi];
    int i = lo - 1;
    for (int j = lo; j < hi; j++)
    {
        s.cmp++;
        if (a[j] < pivot) { i++; swapElems(a, i, j, s); }
    }
    swapElems(a, i + 1, hi, s);
    if (trace)
    {
        cout << "  [Ломуто, глибина " << depth << ", межі " << lo << ".." << hi << ", опорне " << pivot
             << "] позиція опорного = " << i + 1 << ", масив:";
        for (int k = lo; k <= hi; k++) cout << " " << a[k];
        cout << endl;
    }
    return i + 1;
}

void quickLomuto(int a[], int lo, int hi, Stats& s, bool trace, int depth = 0)
{
    s.calls++;
    if (depth > s.maxDepth) s.maxDepth = depth;
    if (lo >= hi) return;
    int p = lomutoPartition(a, lo, hi, s, trace, depth);
    quickLomuto(a, lo, p - 1, s, trace, depth + 1);
    quickLomuto(a, p + 1, hi, s, trace, depth + 1);
}

// ---------- Схема Гоара: опорний — середній елемент; повертає межу j (ділянки [lo..j] і [j+1..hi]) ----------
int hoarePartition(int a[], int lo, int hi, Stats& s, bool trace, int depth)
{
    int pivot = a[(lo + hi) / 2];
    int i = lo - 1, j = hi + 1;
    while (true)
    {
        do { i++; s.cmp++; } while (a[i] < pivot);
        do { j--; s.cmp++; } while (a[j] > pivot);
        if (i >= j)
        {
            if (trace)
            {
                cout << "  [Гоар, глибина " << depth << ", межі " << lo << ".." << hi << ", опорне " << pivot
                     << "] межа j = " << j << ", масив:";
                for (int k = lo; k <= hi; k++) cout << " " << a[k];
                cout << endl;
            }
            return j;
        }
        swapElems(a, i, j, s);
    }
}

void quickHoare(int a[], int lo, int hi, Stats& s, bool trace, int depth = 0)
{
    s.calls++;
    if (depth > s.maxDepth) s.maxDepth = depth;
    if (lo >= hi) return;
    int p = hoarePartition(a, lo, hi, s, trace, depth);
    quickHoare(a, lo, p, s, trace, depth + 1);
    quickHoare(a, p + 1, hi, s, trace, depth + 1);
}

bool isSorted(const int a[], int n)
{
    for (int i = 1; i < n; i++) if (a[i] < a[i - 1]) return false;
    return true;
}
void fingerprint(const int a[], int n, long long& cnt, long long& sum, long long& sq)
{
    cnt = n; sum = 0; sq = 0;
    for (int i = 0; i < n; i++) { sum += a[i]; sq += (long long)a[i] * a[i]; }
}

enum Scheme { LOMUTO, HOARE };
const char* schemeName(Scheme x) { return x == LOMUTO ? "Ломуто" : "Гоар"; }

void runScheme(Scheme sc, int a[], int n, Stats& s, bool trace = false)
{
    resetStats(s);
    if (sc == LOMUTO) quickLomuto(a, 0, n - 1, s, trace);
    else quickHoare(a, 0, n - 1, s, trace);
}

bool checkSort(Scheme sc, int number, const int input[], const int expected[], int n)
{
    int a[64];
    for (int i = 0; i < n; i++) a[i] = input[i];
    long long c0, s0, q0, c1, s1, q1;
    fingerprint(a, n, c0, s0, q0);
    Stats st;
    runScheme(sc, a, n, st);
    fingerprint(a, n, c1, s1, q1);
    bool ok = isSorted(a, n) && c0 == c1 && s0 == s1 && q0 == q1;
    for (int i = 0; i < n; i++) if (a[i] != expected[i]) ok = false;
    cout << schemeName(sc) << "\tТест " << setw(2) << number << ": " << (ok ? "успішно" : "помилка") << endl;
    return ok;
}

// Перевірка розбиття: Ломуто — опорне стоїть на місці p, ліворуч ≤, праворуч ≥.
bool checkLomutoPart(int number, const int in[], int n)
{
    int a[64]; for (int i = 0; i < n; i++) a[i] = in[i];
    Stats s; resetStats(s);
    int pivotVal = a[n - 1];
    int p = lomutoPartition(a, 0, n - 1, s, false, 0);
    bool ok = (a[p] == pivotVal);
    for (int i = 0; i < p; i++) if (a[i] > pivotVal) ok = false;
    for (int i = p + 1; i < n; i++) if (a[i] < pivotVal) ok = false;
    cout << "Розбиття Ломуто\tТест " << setw(2) << number << ": " << (ok ? "успішно" : "помилка") << " (p=" << p << ")" << endl;
    return ok;
}

// Перевірка розбиття: Гоар — ліворуч від j усі ≤ опорного, праворуч — ≥ опорного.
bool checkHoarePart(int number, const int in[], int n)
{
    int a[64]; for (int i = 0; i < n; i++) a[i] = in[i];
    Stats s; resetStats(s);
    int pivotVal = a[(n - 1) / 2];
    int j = hoarePartition(a, 0, n - 1, s, false, 0);
    bool ok = (j >= 0 && j < n - 1);
    for (int i = 0; i <= j && ok; i++) if (a[i] > pivotVal) ok = false;
    for (int i = j + 1; i < n && ok; i++) if (a[i] < pivotVal) ok = false;
    cout << "Розбиття Гоара\tТест " << setw(2) << number << ": " << (ok ? "успішно" : "помилка") << " (j=" << j << ")" << endl;
    return ok;
}

bool runAllTests()
{
    cout << "\n=== Автоматизовані перевірки сортування ===" << endl;
    struct Case { int n; int in[16]; int out[16]; };
    Case cs[12] = {
        {0, {0}, {0}},
        {1, {7}, {7}},
        {2, {1, 2}, {1, 2}},
        {2, {2, 1}, {1, 2}},
        {5, {1,2,3,4,5}, {1,2,3,4,5}},
        {5, {5,4,3,2,1}, {1,2,3,4,5}},
        {5, {4,1,5,2,3}, {1,2,3,4,5}},
        {5, {3,1,3,2,1}, {1,1,2,3,3}},
        {5, {0,-5,8,-2,4}, {-5,-2,0,4,8}},
        {4, {6,6,6,6}, {6,6,6,6}},
        {6, {9,-1,9,0,-1,5}, {-1,-1,0,5,9,9}},
        {5, {3,2,1,4,5}, {1,2,3,4,5}},
    };
    // власний 1: 8 елементів із повтореннями
    int own1[8] = {8, 3, 5, 3, 9, 1, 8, 2}, own1e[8] = {1, 2, 3, 3, 5, 8, 8, 9};
    // власний 2: 16 елементів, спадання з парами
    int own2[16], own2e[16];
    for (int i = 0; i < 16; i++) own2[i] = 16 - i;
    for (int i = 0; i < 16; i++) own2e[i] = i + 1;

    bool all = true;
    for (int c = 0; c < 12; c++)
    {
        all &= checkSort(LOMUTO, c + 1, cs[c].in, cs[c].out, cs[c].n);
        all &= checkSort(HOARE,  c + 1, cs[c].in, cs[c].out, cs[c].n);
    }
    all &= checkSort(LOMUTO, 13, own1, own1e, 8);
    all &= checkSort(HOARE,  13, own1, own1e, 8);
    all &= checkSort(LOMUTO, 14, own2, own2e, 16);
    all &= checkSort(HOARE,  14, own2, own2e, 16);

    cout << "\n=== Перевірка розбиттів ===" << endl;
    int P1[6] = {2, 5, 1, 4, 3, 6};     // опорне — найбільше (6)
    int P2[6] = {1, 2, 3, 4, 5, 0};     // опорне — найменше
    int P3[5] = {4, 4, 4, 4, 4};        // повторювані значення
    int P4[5] = {5, 1, 4, 2, 3};        // опорне — середнє
    all &= checkLomutoPart(1, P1, 6);
    all &= checkLomutoPart(2, P2, 6);
    all &= checkLomutoPart(3, P3, 5);
    all &= checkLomutoPart(4, P4, 5);
    all &= checkHoarePart(5, P1, 6);
    all &= checkHoarePart(6, P2, 6);
    all &= checkHoarePart(7, P3, 5);
    all &= checkHoarePart(8, P4, 5);
    return all;
}

// Варіант 2 (n = 15): 31, 8, 19, 3, ... з тієї ж таблиці: 31, 8, 19, 3, 26, 12, 5, 28, 1, 17, 10, 23, 6, 15, 4
void variantRun()
{
    cout << "\n=== Варіант 2: трасування (n = 15) ===" << endl;
    int orig[15] = { 31, 8, 19, 3, 26, 12, 5, 28, 1, 17, 10, 23, 6, 15, 4 };
    int a[15];

    cout << "Схема Ломуто:" << endl;
    for (int i = 0; i < 15; i++) a[i] = orig[i];
    Stats sL; runScheme(LOMUTO, a, 15, sL, true);
    cout << "Підсумок Ломуто: порівнянь=" << sL.cmp << ", обмінів=" << sL.swaps
         << ", викликів=" << sL.calls << ", макс. глибина=" << sL.maxDepth << endl;
    cout << "Результат: ";
    for (int i = 0; i < 15; i++) cout << a[i] << (i + 1 < 15 ? " " : "\n");

    cout << "Схема Гоара:" << endl;
    for (int i = 0; i < 15; i++) a[i] = orig[i];
    Stats sH; runScheme(HOARE, a, 15, sH, true);
    cout << "Підсумок Гоар: порівнянь=" << sH.cmp << ", обмінів=" << sH.swaps
         << ", викликів=" << sH.calls << ", макс. глибина=" << sH.maxDepth << endl;
    cout << "Результат: ";
    for (int i = 0; i < 15; i++) cout << a[i] << (i + 1 < 15 ? " " : "\n");
}

void makeSet(int type, int n, int a[])
{
    for (int i = 0; i < n; i++)
    {
        if (type == 0) a[i] = i + 1;          // зростання
        else if (type == 1) a[i] = n - i;     // спадання
        else if (type == 2) a[i] = (i * 37 + 11) % 997;   // змішаний (повторюваний)
        else a[i] = i % 10;                   // багато однакових значень
    }
}

void experiment()
{
    cout << "\n=== Експеримент ===" << endl;
    cout << "n\tНабір\tЛоматуто(порівн./обміни/глибина)\tГоар(порівн./обміни/глибина)\tПеревірка" << endl;
    const char* types[4] = { "зростання", "спадання", "змішаний", "багато однакових" };
    int sizes[3] = { 100, 1000, 10000 };
    ofstream csv("results.csv");
    csv << "n,set,scheme,cmp,swaps,depth,check\n";
    for (int si = 0; si < 3; si++)
        for (int t = 0; t < 4; t++)
        {
            int n = sizes[si];
            int* a = new int[n];
            makeSet(t, n, a);
            Stats sL; runScheme(LOMUTO, a, n, sL);
            bool okL = isSorted(a, n);
            makeSet(t, n, a);
            Stats sH; runScheme(HOARE, a, n, sH);
            bool okH = isSorted(a, n);
            bool ok = okL && okH;
            cout << n << "\t" << types[t] << "\t" << sL.cmp << " / " << sL.swaps << " / " << sL.maxDepth
                 << "\t" << sH.cmp << " / " << sH.swaps << " / " << sH.maxDepth
                 << "\t" << (ok ? "успішно" : "помилка") << endl;
            csv << n << "," << types[t] << ",Ломуто," << sL.cmp << "," << sL.swaps << "," << sL.maxDepth << "," << (okL ? "успішно" : "помилка") << "\n";
            csv << n << "," << types[t] << ",Гоар," << sH.cmp << "," << sH.swaps << "," << sH.maxDepth << "," << (okH ? "успішно" : "помилка") << "\n";
            delete[] a;
        }
}

int main()
{
    bool ok = runAllTests();
    if (!ok) { cout << "\nЄ помилки - експеримент не виконується." << endl; return 1; }
    variantRun();
    experiment();
    return 0;
}
