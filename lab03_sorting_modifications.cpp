#include <iostream>
#include <iomanip>
#include <fstream>
using namespace std;

// Правило підрахунку: порівняння = зіставлення двох значень; переміщення = присвоєння елементу масиву або допоміжній змінній.
struct Stats { long long cmp; long long mov; int passes; };

// ---------- Базові алгоритми (з практичної №2) для порівняння ----------
void insertionSort(int a[], int n, Stats& s)
{
    s.cmp = 0; s.mov = 0; s.passes = 0;
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
        s.passes++;
    }
}

void bubbleSort(int a[], int n, Stats& s)
{
    s.cmp = 0; s.mov = 0; s.passes = 0;
    for (int i = 0; i < n - 1; i++)
    {
        bool swapped = false;
        for (int j = 0; j < n - 1 - i; j++)
        {
            s.cmp++;
            if (a[j] > a[j + 1])
            {
                int t = a[j]; s.mov++;
                a[j] = a[j + 1]; s.mov++;
                a[j + 1] = t; s.mov++;
                swapped = true;
            }
        }
        s.passes++;
        if (!swapped) break;
    }
}

// ---------- Практична №3 ----------
// Пошук позиції вставлення бінарним пошуком у відсортованій частині a[0..len-1].
// Нове значення ставиться ПІСЛЯ наявних рівних (верхня межа). Порівняння рахуємо окремо.
int findInsertPos(const int a[], int len, int x, long long& cmp)
{
    int lo = 0, hi = len;
    while (lo < hi)
    {
        int mid = lo + (hi - lo) / 2;
        cmp++;
        if (a[mid] <= x) lo = mid + 1;
        else hi = mid;
    }
    return lo;
}

void binaryInsertionSort(int a[], int n, Stats& s)
{
    s.cmp = 0; s.mov = 0; s.passes = 0;
    for (int i = 1; i < n; i++)
    {
        int x = a[i]; s.mov++;              // збереження значення у допоміжну змінну
        long long c = 0;
        int pos = findInsertPos(a, i, x, c);
        s.cmp += c;
        for (int j = i; j > pos; j--) { a[j] = a[j - 1]; s.mov++; }   // звільнення місця
        a[pos] = x; s.mov++;                // вставлення
        s.passes++;
    }
}

// Шейкерне сортування: прямий і зворотний прохід разом - окремі проходи. Межі left і right.
void shakerSort(int a[], int n, Stats& s)
{
    s.cmp = 0; s.mov = 0; s.passes = 0;
    int left = 0, right = n - 1;
    while (left < right)
    {
        bool swapped = false;
        for (int j = left; j < right; j++)              // прямий прохід
        {
            s.cmp++;
            if (a[j] > a[j + 1])
            {
                int t = a[j]; s.mov++; a[j] = a[j + 1]; s.mov++; a[j + 1] = t; s.mov++;
                swapped = true;
            }
        }
        right--;
        s.passes++;
        if (!swapped) break;                             // дострокове завершення
        swapped = false;
        for (int j = right; j > left; j--)              // зворотний прохід
        {
            s.cmp++;
            if (a[j - 1] > a[j])
            {
                int t = a[j]; s.mov++; a[j] = a[j - 1]; s.mov++; a[j - 1] = t; s.mov++;
                swapped = true;
            }
        }
        left++;
        s.passes++;
        if (!swapped) break;
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

enum Alg { BIN_INS, SHAKER };
const char* algName(Alg x) { return x == BIN_INS ? "Бінарні вставки" : "Шейкерне"; }

void runSort(Alg alg, int a[], int n, Stats& s)
{
    if (alg == BIN_INS) binaryInsertionSort(a, n, s);
    else shakerSort(a, n, s);
}

bool checkSort(Alg alg, int number, const int input[], const int expected[], int n)
{
    int a[64];
    for (int i = 0; i < n; i++) a[i] = input[i];
    long long c0, s0, q0, c1, s1, q1;
    fingerprint(a, n, c0, s0, q0);
    Stats st;
    runSort(alg, a, n, st);
    fingerprint(a, n, c1, s1, q1);
    bool ok = true;
    for (int i = 0; i < n; i++) if (a[i] != expected[i]) ok = false;
    if (c0 != c1 || s0 != s1 || q0 != q1) ok = false;
    if (!isSorted(a, n)) ok = false;
    cout << algName(alg) << "\tТест " << setw(2) << number << ": " << (ok ? "успішно" : "помилка") << endl;
    return ok;
}

bool checkPosition(int number, const int a[], int len, int x, int expectedPos)
{
    long long c = 0;
    int p = findInsertPos(a, len, x, c);
    bool ok = (p == expectedPos);
    cout << "Позиція\tТест " << setw(2) << number << ": " << (ok ? "успішно" : "помилка")
         << " (x=" << x << ", очікувано " << expectedPos << ", отримано " << p << ", порівнянь " << c << ")" << endl;
    return ok;
}

bool runAllTests()
{
    cout << "\n=== Автоматизовані перевірки сортування ===" << endl;
    struct Case { int n; int in[10]; int out[10]; };
    Case cs[12] = {
        {0, {0}, {0}},
        {1, {5}, {5}},
        {5, {1,2,3,4,5}, {1,2,3,4,5}},
        {5, {5,4,3,2,1}, {1,2,3,4,5}},
        {5, {4,2,5,1,3}, {1,2,3,4,5}},
        {5, {3,1,3,2,1}, {1,1,2,3,3}},
        {5, {0,-4,7,-1,3}, {-4,-1,0,3,7}},
        {4, {8,8,8,8}, {8,8,8,8}},
        {2, {2,1}, {1,2}},
        {6, {1,2,3,5,4,6}, {1,2,3,4,5,6}},
        {2, {100,-100}, {-100,100}},         // власний 1
        {5, {3,2,1,0,-1}, {-1,0,1,2,3}},     // власний 2
    };
    bool all = true;
    for (int c = 0; c < 12; c++)
    {
        all &= checkSort(BIN_INS, c + 1, cs[c].in, cs[c].out, cs[c].n);
        all &= checkSort(SHAKER,  c + 1, cs[c].in, cs[c].out, cs[c].n);
    }
    cout << "\n=== Перевірка позицій вставлення ===" << endl;
    int A[4] = {1, 3, 5, 7};
    int B[4] = {1, 3, 3, 7};
    all &= checkPosition(1, A, 4, 0, 0);
    all &= checkPosition(2, A, 4, 4, 2);
    all &= checkPosition(3, A, 4, 8, 4);
    all &= checkPosition(4, B, 4, 3, 3);   // після наявних рівних
    all &= checkPosition(5, A, 0, 5, 0);   // порожня ділянка
    return all;
}

// Варіант 2: 31, 8, 19, 3, 25, 11, 6, 29, 1, 17, 13, 4
void variantRun()
{
    cout << "\n=== Варіант 2 ===" << endl;
    int orig[12] = { 31, 8, 19, 3, 25, 11, 6, 29, 1, 17, 13, 4 };
    for (int alg = 0; alg < 2; alg++)
    {
        int a[12];
        for (int i = 0; i < 12; i++) a[i] = orig[i];
        Stats st;
        runSort((Alg)alg, a, 12, st);
        cout << algName((Alg)alg) << ": порівнянь=" << st.cmp << ", переміщень=" << st.mov
             << ", проходів=" << st.passes << ", результат:";
        for (int i = 0; i < 12; i++) cout << " " << a[i];
        cout << endl;
    }
}

// Набори: зростання, спадання, майже впорядкований, змішаний (наперед задана формула).
void makeSet(int type, int n, int a[])
{
    for (int i = 0; i < n; i++)
    {
        if (type == 0) a[i] = i + 1;
        else if (type == 1) a[i] = n - i;
        else if (type == 2) a[i] = i + 1;
        else a[i] = (i * 37 + 11) % 101;
    }
    if (type == 2)   // майже впорядкований: поміняти 5 пар сусідніх елементів
        for (int t = 0; t < 5; t++)
        {
            int p = t * (n / 5);
            if (p + 1 < n) { int tmp = a[p]; a[p] = a[p + 1]; a[p + 1] = tmp; }
        }
}

void experiment()
{
    cout << "\n=== Експеримент ===" << endl;
    cout << "n\tНабір\tАлгоритм\tПорівняння\tПереміщення\tПроходи\tПеревірка" << endl;
    const char* types[4] = { "зростання", "спадання", "майже впорядкований", "змішаний" };
    const char* algs[4] = { "Вставки", "Бінарні вставки", "Бульбашкове", "Шейкерне" };
    int sizes[3] = { 10, 100, 1000 };
    ofstream csv("results.csv");
    csv << "n,set,alg,cmp,mov,passes,check\n";
    for (int si = 0; si < 3; si++)
    {
        int n = sizes[si];
        for (int t = 0; t < 4; t++)
            for (int alg = 0; alg < 4; alg++)
            {
                int* a = new int[n];
                makeSet(t, n, a);
                Stats st;
                if (alg == 0) insertionSort(a, n, st);
                else if (alg == 1) binaryInsertionSort(a, n, st);
                else if (alg == 2) bubbleSort(a, n, st);
                else shakerSort(a, n, st);
                bool ok = isSorted(a, n);
                cout << n << "\t" << types[t] << "\t" << algs[alg] << "\t" << st.cmp << "\t\t" << st.mov
                     << "\t\t" << st.passes << "\t" << (ok ? "успішно" : "помилка") << endl;
                csv << n << "," << types[t] << "," << algs[alg] << "," << st.cmp << "," << st.mov << ","
                    << st.passes << "," << (ok ? "успішно" : "помилка") << "\n";
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
