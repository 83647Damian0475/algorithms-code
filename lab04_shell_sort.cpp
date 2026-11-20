#include <iostream>
#include <iomanip>
#include <fstream>
using namespace std;

// Правило: порівняння = зіставлення двох значень; переміщення = запис значення до елемента масиву або тимчасової змінної.
struct Stats { long long cmp; long long mov; int steps; };

// ---------- Базові вставки (з практичної №2) ----------
void insertionSort(int a[], int n, Stats& s)
{
    s.cmp = 0; s.mov = 0; s.steps = 1;
    for (int i = 1; i < n; i++)
    {
        int key = a[i]; s.mov++;
        int j = i - 1;
        while (j >= 0)
        {
            s.cmp++;
            if (a[j] > key) { a[j + 1] = a[j]; s.mov++; j--; }
            else break;
        }
        a[j + 1] = key; s.mov++;
    }
}

// ---------- Послідовності кроків ----------
// A: ділення навпіл (n/2, n/4, ..., 1)
int stepsHalving(int n, int out[])
{
    int k = 0;
    for (int g = n / 2; g >= 1; g /= 2) out[k++] = g;
    return k;
}

// B: 1, 4, 13, 40, ... (кожен крок = попередній * 3 + 1), менші за n; використовуються у зворотному порядку
int stepsTriple(int n, int out[])
{
    int seq[64], m = 0;
    for (long long g = 1; g < n; g = g * 3 + 1) seq[m++] = (int)g;
    int k = 0;
    for (int i = m - 1; i >= 0; i--) out[k++] = seq[i];
    return k;
}

// Сортування Шелла з заданими кроками. Для кожного кроку — сортування вставками з відступом gap.
void shellSort(int a[], int n, const int steps[], int ns, Stats& s, bool trace = false)
{
    s.cmp = 0; s.mov = 0; s.steps = 0;
    if (n <= 1) return;
    for (int g = 0; g < ns; g++)
    {
        int gap = steps[g];
        for (int i = gap; i < n; i++)
        {
            int x = a[i]; s.mov++;
            int j = i;
            while (j >= gap)
            {
                s.cmp++;
                if (a[j - gap] > x) { a[j] = a[j - gap]; s.mov++; j -= gap; }
                else break;
            }
            a[j] = x; s.mov++;
        }
        s.steps++;
        if (trace)
        {
            cout << "Крок " << s.steps << " (крок = " << gap << "): ";
            for (int i = 0; i < n; i++) cout << a[i] << (i + 1 < n ? " " : "");
            cout << endl;
        }
    }
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

enum Seq { HALF, TRIPLE };
const char* seqName(Seq q) { return q == HALF ? "Шелл: ділення навпіл" : "Шелл: потроєння + 1"; }

void runShell(Seq q, int a[], int n, Stats& s, bool trace = false)
{
    int steps[64], ns;
    if (q == HALF) ns = stepsHalving(n, steps);
    else ns = stepsTriple(n, steps);
    shellSort(a, n, steps, ns, s, trace);
}

bool checkShell(Seq q, int number, const int input[], const int expected[], int n)
{
    int a[64];
    for (int i = 0; i < n; i++) a[i] = input[i];
    long long c0, s0, q0, c1, s1, q1;
    fingerprint(a, n, c0, s0, q0);
    Stats st;
    runShell(q, a, n, st);
    fingerprint(a, n, c1, s1, q1);
    bool ok = isSorted(a, n) && c0 == c1 && s0 == s1 && q0 == q1;
    for (int i = 0; i < n; i++) if (a[i] != expected[i]) ok = false;
    cout << seqName(q) << "\tТест " << setw(2) << number << ": " << (ok ? "успішно" : "помилка") << endl;
    return ok;
}

bool runAllTests()
{
    cout << "\n=== Автоматизовані перевірки ===" << endl;
    struct Case { int n; int in[24]; int out[24]; };
    Case cs[12] = {
        {0, {0}, {0}},
        {1, {4}, {4}},
        {2, {1, 2}, {1, 2}},
        {2, {2, 1}, {1, 2}},
        {5, {1,2,3,4,5}, {1,2,3,4,5}},
        {5, {5,4,3,2,1}, {1,2,3,4,5}},
        {5, {7,2,9,1,5}, {1,2,5,7,9}},
        {5, {3,1,3,2,1}, {1,1,2,3,3}},
        {5, {0,-6,8,-2,4}, {-6,-2,0,4,8}},
        {4, {6,6,6,6}, {6,6,6,6}},
        {6, {1,2,4,3,5,6}, {1,2,3,4,5,6}},
        {5, {9,-1,9,0,-1}, {-1,-1,0,9,9}},
    };
    // власний 1: непарна кількість елементів
    int own1[7] = {9, 4, 7, 1, 8, 2, 5}, own1e[7] = {1, 2, 4, 5, 7, 8, 9};
    // власний 2: 20 елементів, значення (i*11+3) mod 23 — різні; очікуваний результат через лічильний масив
    int own2[20], own2e[20], cntv[23] = {0};
    for (int i = 0; i < 20; i++) { own2[i] = (i * 11 + 3) % 23; cntv[own2[i]]++; }
    int k = 0;
    for (int v = 0; v < 23; v++) for (int c = 0; c < cntv[v]; c++) own2e[k++] = v;

    bool all = true;
    int number = 1;
    for (int c = 0; c < 12; c++)
    {
        all &= checkShell(HALF,   number, cs[c].in, cs[c].out, cs[c].n);
        all &= checkShell(TRIPLE, number, cs[c].in, cs[c].out, cs[c].n);
        number++;
    }
    all &= checkShell(HALF,   13, own1, own1e, 7);
    all &= checkShell(TRIPLE, 13, own1, own1e, 7);
    all &= checkShell(HALF,   14, own2, own2e, 20);
    all &= checkShell(TRIPLE, 14, own2, own2e, 20);
    return all;
}

// Варіант 2: 31, 7, 16, 2, 27, 10, 5, 24, 13, 1, 29, 8, 20, 4, 18 (n = 15)
void variantRun()
{
    cout << "\n=== Варіант 2: проміжні стани ===" << endl;
    int orig[15] = { 31, 7, 16, 2, 27, 10, 5, 24, 13, 1, 29, 8, 20, 4, 18 };
    int steps[64], ns;

    ns = stepsHalving(15, steps);
    cout << "Кроки (ділення навпіл): ";
    for (int i = 0; i < ns; i++) cout << steps[i] << (i + 1 < ns ? " → " : "\n");
    int a1[15]; for (int i = 0; i < 15; i++) a1[i] = orig[i];
    Stats s1; shellSort(a1, 15, steps, ns, s1, true);
    cout << "Підсумок: порівнянь=" << s1.cmp << ", переміщень=" << s1.mov << ", кроків=" << s1.steps << endl;

    ns = stepsTriple(15, steps);
    cout << "Кроки (потроєння + 1): ";
    for (int i = 0; i < ns; i++) cout << steps[i] << (i + 1 < ns ? " → " : "\n");
    int a2[15]; for (int i = 0; i < 15; i++) a2[i] = orig[i];
    Stats s2; shellSort(a2, 15, steps, ns, s2, true);
    cout << "Підсумок: порівнянь=" << s2.cmp << ", переміщень=" << s2.mov << ", кроків=" << s2.steps << endl;

    int a3[15]; for (int i = 0; i < 15; i++) a3[i] = orig[i];
    Stats s3; insertionSort(a3, 15, s3);
    cout << "Звичайні вставки: порівнянь=" << s3.cmp << ", переміщень=" << s3.mov << endl;
}

void makeSet(int type, int n, int a[])
{
    for (int i = 0; i < n; i++)
    {
        if (type == 0) a[i] = i + 1;
        else if (type == 1) a[i] = n - i;
        else if (type == 2) a[i] = i + 1;
        else a[i] = (i * 37 + 11) % 101;
    }
    if (type == 2)
        for (int t = 0; t < 5; t++)
        {
            int p = t * (n / 5);
            if (p + 1 < n) { int tmp = a[p]; a[p] = a[p + 1]; a[p + 1] = tmp; }
        }
}

void experiment()
{
    cout << "\n=== Експеримент ===" << endl;
    cout << "n\tНабір\tВставки(порівн./перем.)\tHALF(порівн./перем./кроків)\tTRIPLE(порівн./перем./кроків)\tПеревірка" << endl;
    const char* types[4] = { "зростання", "спадання", "майже впорядкований", "змішаний" };
    int sizes[3] = { 100, 1000, 10000 };
    ofstream csv("results.csv");
    csv << "n,set,alg,cmp,mov,steps,check\n";
    for (int si = 0; si < 3; si++)
    {
        int n = sizes[si];
        for (int t = 0; t < 4; t++)
        {
            int* a = new int[n];
            int steps[64], ns;
            // звичайні вставки
            makeSet(t, n, a); Stats sI; insertionSort(a, n, sI); bool okI = isSorted(a, n);
            // Шелл: ділення навпіл
            makeSet(t, n, a); ns = stepsHalving(n, steps); Stats sA; shellSort(a, n, steps, ns, sA); bool okA = isSorted(a, n);
            // Шелл: потроєння + 1
            makeSet(t, n, a); ns = stepsTriple(n, steps); Stats sB; shellSort(a, n, steps, ns, sB); bool okB = isSorted(a, n);
            bool ok = okI && okA && okB;
            cout << n << "\t" << types[t] << "\t" << sI.cmp << " / " << sI.mov
                 << "\t" << sA.cmp << " / " << sA.mov << " / " << sA.steps
                 << "\t" << sB.cmp << " / " << sB.mov << " / " << sB.steps
                 << "\t" << (ok ? "успішно" : "помилка") << endl;
            csv << n << "," << types[t] << ",Вставки," << sI.cmp << "," << sI.mov << ",1," << (okI ? "успішно" : "помилка") << "\n";
            csv << n << "," << types[t] << ",Шелл-ділення," << sA.cmp << "," << sA.mov << "," << sA.steps << "," << (okA ? "успішно" : "помилка") << "\n";
            csv << n << "," << types[t] << ",Шелл-потроєння," << sB.cmp << "," << sB.mov << "," << sB.steps << "," << (okB ? "успішно" : "помилка") << "\n";
            delete[] a;
        }
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
