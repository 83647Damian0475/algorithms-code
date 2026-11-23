#include <iostream>
#include <iomanip>
#include <fstream>
using namespace std;

// Статистика: порівняння значень, обміни (один обмін = 1), виклики відновлення купи.
struct Stats { long long cmp; long long swaps; long long calls; };
void resetStats(Stats& s) { s.cmp = 0; s.swaps = 0; s.calls = 0; }

void swapElems(int a[], int i, int j, Stats& s)
{
    if (i == j) return;
    int t = a[i]; a[i] = a[j]; a[j] = t;
    s.swaps++;
}

// Ітеративне відновлення властивості max-heap для вузла i в межах активного розміру size.
void siftDown(int a[], int size, int i, Stats& s)
{
    s.calls++;
    while (true)
    {
        int l = 2 * i + 1, r = 2 * i + 2, big = i;
        if (l < size) { s.cmp++; if (a[l] > a[big]) big = l; }
        if (r < size) { s.cmp++; if (a[r] > a[big]) big = r; }
        if (big == i) break;
        swapElems(a, i, big, s);
        i = big;
    }
}

// Побудова купи знизу вгору: від останнього внутрішнього вузла до кореня.
void buildHeap(int a[], int n, Stats& s)
{
    for (int i = n / 2 - 1; i >= 0; i--) siftDown(a, n, i, s);
}

// Повне сортування: статистика побудови та вилучення окремо.
void heapSort(int a[], int n, Stats& build, Stats& extract, bool trace = false)
{
    resetStats(build); resetStats(extract);
    buildHeap(a, n, build);
    if (trace)
    {
        cout << "Після побудови купи:";
        for (int i = 0; i < n; i++) cout << " " << a[i];
        cout << endl;
    }
    for (int end = n - 1; end >= 1; end--)
    {
        swapElems(a, 0, end, extract);
        siftDown(a, end, 0, extract);
        if (trace)
        {
            cout << "Вилучення " << (n - end) << " (активна купа " << end << " елементів):";
            for (int i = 0; i < n; i++) cout << " " << a[i];
            cout << endl;
        }
    }
}

// Перевірка властивості max-heap для a[0..size-1]; повертає індекс порушення або -1.
int firstHeapViolation(const int a[], int size)
{
    for (int i = 0; i < size; i++)
    {
        int l = 2 * i + 1, r = 2 * i + 2;
        if (l < size && a[i] < a[l]) return i;
        if (r < size && a[i] < a[r]) return i;
    }
    return -1;
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

bool checkSort(int number, const int input[], const int expected[], int n)
{
    int a[64];
    for (int i = 0; i < n; i++) a[i] = input[i];
    long long c0, s0, q0, c1, s1, q1;
    fingerprint(a, n, c0, s0, q0);
    Stats b, e;
    heapSort(a, n, b, e);
    fingerprint(a, n, c1, s1, q1);
    bool ok = isSorted(a, n) && c0 == c1 && s0 == s1 && q0 == q1;
    for (int i = 0; i < n; i++) if (a[i] != expected[i]) ok = false;
    cout << "HeapSort\tТест " << setw(2) << number << ": " << (ok ? "успішно" : "помилка") << endl;
    return ok;
}

bool checkHeapProp(int number, const int a[], int size, bool expectValid)
{
    int v = firstHeapViolation(a, size);
    bool ok = expectValid ? (v == -1) : (v != -1);
    cout << "Купа\tТест " << setw(2) << number << ": " << (ok ? "успішно" : "помилка")
         << " (порушення в індексі " << v << ")" << endl;
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
        {7, {10,7,8,4,2,1,3}, {1,2,3,4,7,8,10}},
    };
    bool all = true;
    for (int c = 0; c < 12; c++) all &= checkSort(c + 1, cs[c].in, cs[c].out, cs[c].n);
    // власні набори
    int own1[8] = {8, 3, 5, 3, 9, 1, 8, 2}, own1e[8] = {1, 2, 3, 3, 5, 8, 8, 9};
    int own2[16], own2e[16];
    for (int i = 0; i < 16; i++) own2[i] = 16 - i;
    for (int i = 0; i < 16; i++) own2e[i] = i + 1;
    all &= checkSort(13, own1, own1e, 8);
    all &= checkSort(14, own2, own2e, 16);

    cout << "\n=== Перевірка властивості купи ===" << endl;
    int H1[7] = {9, 7, 8, 4, 5, 6, 3};     // правильна
    int H2[1] = {5};                       // правильна (один вузол)
    int H3[5] = {10, 9, 8, 7, 6};          // правильна
    int H4[7] = {5, 9, 3, 4, 2, 1, 0};     // порушена у кореня
    int H5[5] = {9, 7, 8, 10, 1};          // порушена в лівій гілці (7 < 10)
    int H6[6] = {9, 8, 7, 6, 5, 9};        // порушена (правий нащадок 9 більший за 7)
    all &= checkHeapProp(15, H1, 7, true);
    all &= checkHeapProp(16, H2, 1, true);
    all &= checkHeapProp(17, H3, 5, true);
    all &= checkHeapProp(18, H4, 7, false);
    all &= checkHeapProp(19, H5, 5, false);
    all &= checkHeapProp(20, H6, 6, false);
    return all;
}

// Варіант 16 (n = 15): 40, 17, 25, 12, 36, 20, 9, 32, 15, 28, 8, 38, 22, 13, 31
void variantRun()
{
    cout << "\n=== Варіант 2: трасування ===" << endl;
    int orig[15] = { 30, 8, 19, 3, 26, 11, 5, 28, 1, 17, 9, 23, 6, 15, 4 };
    int a[15];
    for (int i = 0; i < 15; i++) a[i] = orig[i];
    Stats b, e;
    heapSort(a, 15, b, e, true);
    cout << "Побудова: порівнянь=" << b.cmp << ", обмінів=" << b.swaps << ", викликів=" << b.calls << endl;
    cout << "Вилучення: порівнянь=" << e.cmp << ", обмінів=" << e.swaps << ", викликів=" << e.calls << endl;
    cout << "Усього: порівнянь=" << (b.cmp + e.cmp) << ", обмінів=" << (b.swaps + e.swaps)
         << ", викликів=" << (b.calls + e.calls) << endl;
    cout << "Результат:";
    for (int i = 0; i < 15; i++) cout << " " << a[i];
    cout << endl;
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
    cout << "\n=== Експеримент ===" << endl;
    cout << "n\tНабір\tПобудова(порівн./обміни)\tУсього(порівн./обміни)\tПеревірка" << endl;
    const char* types[4] = { "зростання", "спадання", "змішаний", "багато повторень" };
    int sizes[3] = { 100, 1000, 10000 };
    ofstream csv("results.csv");
    csv << "n,set,build_cmp,build_swaps,total_cmp,total_swaps,check\n";
    for (int si = 0; si < 3; si++)
        for (int t = 0; t < 4; t++)
        {
            int n = sizes[si];
            int* a = new int[n];
            makeSet(t, n, a);
            Stats b, e;
            heapSort(a, n, b, e);
            bool ok = isSorted(a, n);
            cout << n << "\t" << types[t] << "\t" << b.cmp << " / " << b.swaps
                 << "\t\t" << (b.cmp + e.cmp) << " / " << (b.swaps + e.swaps)
                 << "\t" << (ok ? "успішно" : "помилка") << endl;
            csv << n << "," << types[t] << "," << b.cmp << "," << b.swaps << ","
                << (b.cmp + e.cmp) << "," << (b.swaps + e.swaps) << "," << (ok ? "успішно" : "помилка") << "\n";
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
