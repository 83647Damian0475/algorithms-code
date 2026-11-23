#include <iostream>
#include <iomanip>
#include <fstream>
using namespace std;

// Одиниця порівняння для всіх алгоритмів: зіставлення двох значень.
// ---------- MergeSort (практична №7) ----------
struct MS { long long cmp, writes, merges; int maxDepth; };
void resetMS(MS& s) { s.cmp = 0; s.writes = 0; s.merges = 0; s.maxDepth = 0; }

void mergeRange(int a[], int buf[], int lo, int mid, int hi, MS& s, bool trace)
{
    int i = lo, j = mid + 1, k = lo;
    while (i <= mid && j <= hi)
    {
        s.cmp++;
        if (a[i] <= a[j]) { buf[k++] = a[i++]; }
        else              { buf[k++] = a[j++]; }
        s.writes++;
    }
    while (i <= mid) { buf[k++] = a[i++]; s.writes++; }
    while (j <= hi)  { buf[k++] = a[j++]; s.writes++; }
    for (int t = lo; t <= hi; t++) { a[t] = buf[t]; s.writes++; }
    s.merges++;
    if (trace)
    {
        cout << "  злиття [" << lo << ".." << hi << "]: результат:";
        for (int t = lo; t <= hi; t++) cout << " " << a[t];
        cout << endl;
    }
}

void mergeSortRec(int a[], int buf[], int lo, int hi, int depth, MS& s, bool trace)
{
    if (depth > s.maxDepth) s.maxDepth = depth;
    if (lo >= hi) return;
    int mid = (lo + hi) / 2;
    if (trace) cout << "поділ глибина " << depth << ": [" << lo << ".." << hi << "] -> [" << lo << ".." << mid << "] і [" << mid + 1 << ".." << hi << "]" << endl;
    mergeSortRec(a, buf, lo, mid, depth + 1, s, trace);
    mergeSortRec(a, buf, mid + 1, hi, depth + 1, s, trace);
    mergeRange(a, buf, lo, mid, hi, s, trace);
}

void mergeSort(int a[], int n, MS& s, bool trace = false)
{
    resetMS(s);
    if (n <= 1) return;
    int* buf = new int[n];
    mergeSortRec(a, buf, 0, n - 1, 0, s, trace);
    delete[] buf;
}

// ---------- Інші алгоритми (спільна одиниця: порівняння) ----------
long long insertionCmp(int a[], int n)
{
    long long c = 0;
    for (int i = 1; i < n; i++) { int key = a[i]; int j = i - 1;
        while (j >= 0) { c++; if (a[j] > key) { a[j + 1] = a[j]; j--; } else break; }
        a[j + 1] = key; }
    return c;
}
long long selectionCmp(int a[], int n)
{
    long long c = 0;
    for (int i = 0; i < n - 1; i++) { int m = i;
        for (int j = i + 1; j < n; j++) { c++; if (a[j] < a[m]) m = j; }
        if (m != i) { int t = a[i]; a[i] = a[m]; a[m] = t; } }
    return c;
}
long long bubbleCmp(int a[], int n)
{
    long long c = 0;
    for (int i = 0; i < n - 1; i++) { bool sw = false;
        for (int j = 0; j < n - 1 - i; j++) { c++; if (a[j] > a[j + 1]) { int t = a[j]; a[j] = a[j + 1]; a[j + 1] = t; sw = true; } }
        if (!sw) break; }
    return c;
}
long long shellCmp(int a[], int n)
{
    long long c = 0;
    for (int gap = n / 2; gap >= 1; gap /= 2)
        for (int i = gap; i < n; i++) { int x = a[i]; int j = i;
            while (j >= gap) { c++; if (a[j - gap] > x) { a[j] = a[j - gap]; j -= gap; } else break; }
            a[j] = x; }
    return c;
}
long long quickLomutoCmp(int a[], int lo, int hi)
{
    if (lo >= hi) return 0;
    long long c = 0;
    int p = a[hi], i = lo - 1;
    for (int j = lo; j < hi; j++) { c++; if (a[j] < p) { i++; int t = a[i]; a[i] = a[j]; a[j] = t; } }
    int t = a[i + 1]; a[i + 1] = a[hi]; a[hi] = t;
    return c + quickLomutoCmp(a, lo, i) + quickLomutoCmp(a, i + 2, hi);
}
long long heapCmp(int a[], int n)
{
    long long c = 0;
    auto sift = [&](int size, int i) {
        while (true) { int l = 2 * i + 1, r = 2 * i + 2, big = i;
            if (l < size) { c++; if (a[l] > a[big]) big = l; }
            if (r < size) { c++; if (a[r] > a[big]) big = r; }
            if (big == i) break;
            int t = a[i]; a[i] = a[big]; a[big] = t; i = big; } };
    for (int i = n / 2 - 1; i >= 0; i--) sift(n, i);
    for (int end = n - 1; end >= 1; end--) { int t = a[0]; a[0] = a[end]; a[end] = t; sift(end, 0); }
    return c;
}

bool isSorted(const int a[], int n) { for (int i = 1; i < n; i++) if (a[i] < a[i - 1]) return false; return true; }
void fingerprint(const int a[], int n, long long& cnt, long long& sum, long long& sq)
{ cnt = n; sum = 0; sq = 0; for (int i = 0; i < n; i++) { sum += a[i]; sq += (long long)a[i] * a[i]; } }

bool checkMerge(int number, const int input[], const int expected[], int n)
{
    int a[64]; for (int i = 0; i < n; i++) a[i] = input[i];
    long long c0, s0, q0, c1, s1, q1; fingerprint(a, n, c0, s0, q0);
    MS s; mergeSort(a, n, s);
    fingerprint(a, n, c1, s1, q1);
    bool ok = isSorted(a, n) && c0 == c1 && s0 == s1 && q0 == q1;
    for (int i = 0; i < n; i++) if (a[i] != expected[i]) ok = false;
    cout << "MergeSort\tТест " << setw(2) << number << ": " << (ok ? "успішно" : "помилка") << endl;
    return ok;
}

// Перевірка окремого злиття: ліва і права частини впорядковані, результат — злиття.
bool checkMergeOnly(int number, int a[], int lo, int mid, int hi, const int expected[])
{
    int buf[64]; MS s; resetMS(s);
    mergeRange(a, buf, lo, mid, hi, s, false);
    int len = hi - lo + 1; bool ok = true;
    for (int i = 0; i < len; i++) if (a[lo + i] != expected[i]) ok = false;
    cout << "Злиття\tТест " << setw(2) << number << ": " << (ok ? "успішно" : "помилка") << endl;
    return ok;
}

bool runAllTests()
{
    cout << "\n=== Перевірки злиття ===" << endl;
    bool all = true;
    { int a[] = {1, 4, 7, 2, 3, 9}; int e[] = {1, 2, 3, 4, 7, 9}; all &= checkMergeOnly(1, a, 0, 2, 5, e); }
    { int a[] = {5, 1, 2}; int e[] = {1, 2, 5}; all &= checkMergeOnly(2, a, 0, 0, 2, e); }
    { int a[] = {1, 2, 3, 4}; int e[] = {1, 2, 3, 4}; all &= checkMergeOnly(3, a, 0, 1, 3, e); }
    { int a[] = {3, 4, 1, 2}; int e[] = {1, 2, 3, 4}; all &= checkMergeOnly(4, a, 0, 1, 3, e); }
    { int a[] = {2, 2, 2, 1, 2}; int e[] = {1, 2, 2, 2, 2}; all &= checkMergeOnly(5, a, 0, 2, 4, e); }
    { int a[] = {-5, 0, 3, -2, 1}; int e[] = {-5, -2, 0, 1, 3}; all &= checkMergeOnly(6, a, 0, 2, 4, e); }

    cout << "\n=== Автоматизовані перевірки MergeSort ===" << endl;
    struct Case { int n; int in[16]; int out[16]; };
    Case cs[12] = {
        {0, {0}, {0}}, {1, {7}, {7}}, {2, {1, 2}, {1, 2}}, {2, {2, 1}, {1, 2}},
        {5, {1,2,3,4,5}, {1,2,3,4,5}}, {5, {5,4,3,2,1}, {1,2,3,4,5}}, {5, {4,1,5,2,3}, {1,2,3,4,5}},
        {5, {3,1,3,2,1}, {1,1,2,3,3}}, {5, {0,-5,8,-2,4}, {-5,-2,0,4,8}}, {4, {6,6,6,6}, {6,6,6,6}},
        {6, {9,-1,9,0,-1,5}, {-1,-1,0,5,9,9}}, {7, {8,3,7,2,6,1,5}, {1,2,3,5,6,7,8}},
    };
    for (int c = 0; c < 12; c++) all &= checkMerge(c + 1, cs[c].in, cs[c].out, cs[c].n);
    int own1[15], own1e[15];
    for (int i = 0; i < 15; i++) own1[i] = (i * 7 + 3) % 15;
    { int cnt[15] = {0}; for (int i = 0; i < 15; i++) cnt[own1[i]]++; int k = 0; for (int v = 0; v < 15; v++) for (int c = 0; c < cnt[v]; c++) own1e[k++] = v; }
    all &= checkMerge(13, own1, own1e, 15);
    int own2[16], own2e[16];
    for (int i = 0; i < 16; i++) { own2[i] = 16 - i; own2e[i] = i + 1; }
    all &= checkMerge(14, own2, own2e, 16);
    return all;
}

// Варіант 16 (n = 16): 40, 17, 25, 12, 36, 20, 9, 32, 15, 28, 8, 38, 22, 13, 31, 11
void variantRun()
{
    cout << "\n=== Варіант 2: трасування MergeSort (n = 16) ===" << endl;
    int orig[16] = { 30, 8, 19, 3, 26, 11, 5, 28, 1, 17, 9, 23, 6, 15, 4, 21 };
    int a[16]; for (int i = 0; i < 16; i++) a[i] = orig[i];
    MS s; mergeSort(a, 16, s, true);
    cout << "Статистика: порівнянь=" << s.cmp << ", записів=" << s.writes << ", злиттів=" << s.merges
         << ", макс. глибина=" << s.maxDepth << endl;
    cout << "Результат:"; for (int i = 0; i < 16; i++) cout << " " << a[i]; cout << endl;

    cout << "\n=== Порівняння всіх алгоритмів на варіанті 2 (порівняння значень) ===" << endl;
    struct Algo { const char* name; long long cmp; };
    Algo rows[7];
    int t[16];
    auto copyOrig = [&](int* x) { for (int i = 0; i < 16; i++) x[i] = orig[i]; };
    copyOrig(t); rows[0] = { "Вставками", insertionCmp(t, 16) };
    copyOrig(t); rows[1] = { "Вибором", selectionCmp(t, 16) };
    copyOrig(t); rows[2] = { "Бульбашкове", bubbleCmp(t, 16) };
    copyOrig(t); rows[3] = { "Шелла (ділення навпіл)", shellCmp(t, 16) };
    copyOrig(t); rows[4] = { "QuickSort (Ломуто)", quickLomutoCmp(t, 0, 15) };
    copyOrig(t); rows[5] = { "HeapSort", heapCmp(t, 16) };
    copyOrig(t); MS sm; mergeSort(t, 16, sm); rows[6] = { "MergeSort", sm.cmp };
    for (int i = 0; i < 7; i++) cout << rows[i].name << "\t" << rows[i].cmp << endl;
}

void makeSet(int type, int n, int a[])
{
    for (int i = 0; i < n; i++)
    {
        if (type == 0) a[i] = i + 1;
        else if (type == 1) a[i] = n - i;
        else if (type == 2) a[i] = (i * 37 + 11) % 997;
        else a[i] = i % 10;
    }
}

void experiment()
{
    cout << "\n=== Експеримент: порівняння значень (n, набір, алгоритми) ===" << endl;
    cout << "n\tНабір\tВставки\tВибір\tБульб.\tШелл\tQuick\tHeap\tMerge" << endl;
    const char* types[4] = { "зростання", "спадання", "змішаний", "багато повторень" };
    int sizes[3] = { 100, 1000, 10000 };
    ofstream csv("results.csv");
    csv << "n,set,insertion,selection,bubble,shell,quick,heap,merge,merge_writes,merge_depth\n";
    for (int si = 0; si < 3; si++)
        for (int ti = 0; ti < 4; ti++)
        {
            int n = sizes[si];
            int* a = new int[n]; int* t = new int[n];
            long long c[7];
            makeSet(ti, n, a); for (int i = 0; i < n; i++) t[i] = a[i]; c[0] = insertionCmp(t, n);
            makeSet(ti, n, a); for (int i = 0; i < n; i++) t[i] = a[i]; c[1] = selectionCmp(t, n);
            makeSet(ti, n, a); for (int i = 0; i < n; i++) t[i] = a[i]; c[2] = bubbleCmp(t, n);
            makeSet(ti, n, a); for (int i = 0; i < n; i++) t[i] = a[i]; c[3] = shellCmp(t, n);
            makeSet(ti, n, a); for (int i = 0; i < n; i++) t[i] = a[i]; c[4] = quickLomutoCmp(t, 0, n - 1);
            makeSet(ti, n, a); for (int i = 0; i < n; i++) t[i] = a[i]; c[5] = heapCmp(t, n);
            makeSet(ti, n, a); for (int i = 0; i < n; i++) t[i] = a[i]; MS sm; mergeSort(t, n, sm); c[6] = sm.cmp;
            bool ok = isSorted(t, n);
            cout << n << "\t" << types[ti];
            for (int k = 0; k < 7; k++) cout << "\t" << c[k];
            cout << "\t" << (ok ? "успішно" : "помилка") << endl;
            csv << n << "," << types[ti];
            for (int k = 0; k < 7; k++) csv << "," << c[k];
            csv << "," << sm.writes << "," << sm.maxDepth << "\n";
            delete[] a; delete[] t;
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
