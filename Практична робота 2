#include <iostream>
#include <iomanip>
using namespace std;

struct Stats { long long cmp; long long mov; int passes; };

// Правило: порівняння = зіставлення двох значень масиву; переміщення = одне присвоєння елементу масиву.
void swapCounted(int a[], int i, int j, Stats& s)
{
    int t = a[i];  s.mov++;   // запис у допоміжну змінну
    a[i] = a[j];   s.mov++;
    a[j] = t;      s.mov++;
}

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

void selectionSort(int a[], int n, Stats& s)
{
    s.cmp = 0; s.mov = 0; s.passes = 0;
    for (int i = 0; i < n - 1; i++)
    {
        int minIdx = i;
        for (int j = i + 1; j < n; j++)
        {
            s.cmp++;
            if (a[j] < a[minIdx]) minIdx = j;
        }
        if (minIdx != i) swapCounted(a, i, minIdx, s);
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
            if (a[j] > a[j + 1]) { swapCounted(a, j, j + 1, s); swapped = true; }
        }
        s.passes++;
        if (!swapped) break;   // дострокове завершення
    }
}

bool isSorted(const int a[], int n)
{
    for (int i = 1; i < n; i++) if (a[i] < a[i - 1]) return false;
    return true;
}

// Контроль складу масиву: кількість, сума і сума квадратів мають не змінитися.
void fingerprint(const int a[], int n, long long& cnt, long long& sum, long long& sq)
{
    cnt = n; sum = 0; sq = 0;
    for (int i = 0; i < n; i++) { sum += a[i]; sq += (long long)a[i] * a[i]; }
}

enum Alg { INS, SEL, BUB };
const char* algName(Alg x) { return x == INS ? "Вставки" : x == SEL ? "Вибір" : "Бульбашкове"; }

void runSort(Alg alg, int a[], int n, Stats& s)
{
    if (alg == INS) insertionSort(a, n, s);
    else if (alg == SEL) selectionSort(a, n, s);
    else bubbleSort(a, n, s);
}

// Перевірка одного набору для одного алгоритму; повертає true, якщо все коректно.
bool checkOne(Alg alg, int number, const int input[], const int expected[], int n)
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
    cout << algName(alg) << "\tТест " << setw(2) << number << ": " << (ok ? "успішно" : "помилка");
    if (!ok) {
        cout << " (масив:";
        for (int i = 0; i < n; i++) cout << " " << a[i];
        cout << ")";
    }
    cout << endl;
    return ok;
}

bool runAllTests()
{
    cout << "\n=== Автоматизовані перевірки ===" << endl;
    struct Case { int n; int in[10]; int out[10]; };
    Case cs[12] = {
        {0, {0}, {0}},
        {1, {7}, {7}},
        {5, {1,2,3,4,5}, {1,2,3,4,5}},
        {5, {5,4,3,2,1}, {1,2,3,4,5}},
        {5, {4,1,5,2,3}, {1,2,3,4,5}},
        {5, {3,1,3,2,1}, {1,1,2,3,3}},
        {5, {0,-5,8,-2,4}, {-5,-2,0,4,8}},
        {4, {6,6,6,6}, {6,6,6,6}},
        {2, {2,1}, {1,2}},
        {6, {9,-1,9,0,-1,5}, {-1,-1,0,5,9,9}},
        {2, {100,-100}, {-100,100}},          // власний тест 11
        {5, {3,2,1,0,-1}, {-1,0,1,2,3}},      // власний тест 12
    };
    bool all = true;
    int number = 1;
    for (int c = 0; c < 12; c++)
        for (int alg = 0; alg < 3; alg++)
            all &= checkOne((Alg)alg, number + c, cs[c].in, cs[c].out, cs[c].n);
    return all;
}

// Генератор без бібліотек: лінійний конгруентний.
unsigned int seed = 12345;
int nextRand(int maxV)
{
    seed = seed * 1103515245u + 12345u;
    return (int)((seed >> 16) % maxV);
}

void experiment()
{
    cout << "\n=== Експеримент (по 100 елементів) ===" << endl;
    cout << "Алгоритм\tТип набору\tПорівняння\tПереміщення\tПроходи\tПеревірка" << endl;
    const int N = 100;
    const char* types[3] = { "зростання", "спадання", "змішаний" };
    // набори
    int sets[3][N];
    for (int i = 0; i < N; i++) sets[0][i] = i + 1;
    for (int i = 0; i < N; i++) sets[1][i] = N - i;
    for (int i = 0; i < N; i++) sets[2][i] = nextRand(1000);
    for (int alg = 0; alg < 3; alg++)
        for (int t = 0; t < 3; t++)
        {
            int a[N];
            for (int i = 0; i < N; i++) a[i] = sets[t][i];
            Stats st;
            runSort((Alg)alg, a, N, st);
            cout << algName((Alg)alg) << "\t" << types[t] << "\t" << st.cmp << "\t\t" << st.mov
                 << "\t\t" << st.passes << "\t" << (isSorted(a, N) ? "успішно" : "помилка") << endl;
        }
}

void variantRun()
{
    cout << "\n=== Варіант 2 ===" << endl;
    int orig[10] = { 25, 7, 14, 3, 29, 10, 1, 22, 16, 5 };
    for (int alg = 0; alg < 3; alg++)
    {
        int a[10];
        for (int i = 0; i < 10; i++) a[i] = orig[i];
        Stats st;
        runSort((Alg)alg, a, 10, st);
        cout << algName((Alg)alg) << ": порівнянь=" << st.cmp << ", переміщень=" << st.mov
             << ", проходів=" << st.passes << ", результат:";
        for (int i = 0; i < 10; i++) cout << " " << a[i];
        cout << endl;
    }
}

int main()
{
    bool ok = runAllTests();
    if (!ok) { cout << "Є помилки - експеримент не виконується." << endl; return 1; }
    variantRun();
    experiment();
    return 0;
}
