#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <cstring>
using namespace std;

// Одиниці: порівняння = зіставлення одного символу тексту або зразка; зміщення = перехід до коротшого префікса.
struct KStat { long long cmp; long long shifts; long long matches; };

// Префікс-функція: pi[i] – довжина найдовшого власного префікса p[0..i], що є суфіксом p[0..i].
void prefixFunction(const char p[], int m, int pi[], long long& cmp)
{
    cmp = 0;
    if (m == 0) return;
    pi[0] = 0;
    int k = 0;
    for (int i = 1; i < m; i++)
    {
        while (k > 0 && (cmp++, p[i] != p[k])) k = pi[k - 1];
        cmp++;
        if (p[i] == p[k]) k++;
        pi[i] = k;
    }
}

// Пошук КМП. Текст не повертається назад: після невідповідності змінюється лише довжина збігу q.
int kmpSearch(const char t[], int n, const char p[], int m, const int pi[], int out[], int cap,
              KStat& s, bool firstOnly)
{
    s.cmp = 0; s.shifts = 0; s.matches = 0;
    if (m == 0) { if (cap > 0) out[0] = 0; s.matches = 1; return 1; }
    if (m > n) return 0;
    int q = 0, k = 0;
    for (int i = 0; i < n; i++)
    {
        while (q > 0 && (s.cmp++, t[i] != p[q])) { q = pi[q - 1]; s.shifts++; }
        s.cmp++;
        if (t[i] == p[q]) q++;
        if (q == m)
        {
            if (k < cap) out[k] = i - m + 1;
            k++; s.matches++;
            if (firstOnly) return k;
            q = pi[m - 1];
        }
    }
    return k;
}

// Прямий пошук для порівняння (та сама одиниця – зіставлення символів).
int naiveSearch(const char t[], int n, const char p[], int m, int out[], int cap, long long& cmp)
{
    cmp = 0;
    if (m == 0) { if (cap > 0) out[0] = 0; return 1; }
    if (m > n) return 0;
    int k = 0;
    for (int i = 0; i <= n - m; i++)
    {
        int j = 0;
        while (j < m) { cmp++; if (t[i + j] != p[j]) break; j++; }
        if (j == m) { if (k < cap) out[k] = i; k++; }
    }
    return k;
}

bool checkPrefix(int number, const char p[], const vector<int>& expected)
{
    int m = (int)strlen(p);
    int pi[64]; long long c;
    prefixFunction(p, m, pi, c);
    bool ok = ((int)expected.size() == m);
    for (int i = 0; ok && i < m; i++) if (pi[i] != expected[i]) ok = false;
    cout << "Префікс\tТест " << setw(2) << number << ": " << (ok ? "успішно" : "помилка");
    if (!ok) { cout << " (знайдено:"; for (int i = 0; i < m; i++) cout << " " << pi[i]; cout << ")"; }
    cout << endl;
    return ok;
}

bool checkSearch(int number, const char t[], const char p[], int first, const vector<int>& all)
{
    int n = (int)strlen(t), m = (int)strlen(p);
    int pi[64]; long long c;
    prefixFunction(p, m, pi, c);
    int out[64]; KStat s1, s2;
    int f = kmpSearch(t, n, p, m, pi, out, 1, s1, true);
    int firstPos = (f > 0) ? out[0] : -1;
    int cnt = kmpSearch(t, n, p, m, pi, out, 64, s2, false);
    bool ok = (firstPos == first) && (cnt == (int)all.size());
    for (int i = 0; ok && i < cnt; i++) if (out[i] != all[i]) ok = false;
    cout << "Пошук\tТест " << setw(2) << number << ": " << (ok ? "успішно" : "помилка")
         << " (перше=" << firstPos << ", усі=" << cnt << ")" << endl;
    return ok;
}

bool runAllTests()
{
    cout << "\n=== Перевірка префікс-функції ===" << endl;
    bool all = true;
    all &= checkPrefix(1, "a", {0});
    all &= checkPrefix(2, "aaaa", {0, 1, 2, 3});
    all &= checkPrefix(3, "abcd", {0, 0, 0, 0});
    all &= checkPrefix(4, "ababaca", {0, 0, 1, 2, 3, 0, 1});
    all &= checkPrefix(5, "aabaaab", {0, 1, 0, 1, 2, 2, 3});
    all &= checkPrefix(6, "abcababcab", {0, 0, 0, 1, 2, 1, 2, 3, 4, 5});

    cout << "\n=== Пошук КМП ===" << endl;
    all &= checkSearch(7,  "algorithm", "rith", 4, {4});
    all &= checkSearch(8,  "aaaa", "aa", 0, {0, 1, 2});
    all &= checkSearch(9,  "abababa", "aba", 0, {0, 2, 4});
    all &= checkSearch(10, "abc", "abcd", -1, {});
    all &= checkSearch(11, "mississippi", "issi", 1, {1, 4});
    all &= checkSearch(12, "abcabcabd", "abcabd", 3, {3});
    all &= checkSearch(13, "no matches here", "xyz", -1, {});
    all &= checkSearch(14, "", "a", -1, {});
    all &= checkSearch(15, "abcabc", "abc", 0, {0, 3});       // власний
    all &= checkSearch(16, "aaaaa", "aaaaaa", -1, {});        // власний: зразок довший за текст
    return all;
}

// Варіант 16: текст aabaaabaaab, зразок aaab
void variantRun()
{
    cout << "\n=== Варіант 2 ===" << endl;
    const char* text = "aaaaabaaaaab";
    const char* pat = "aaaab";
    int n = (int)strlen(text), m = (int)strlen(pat);
    int pi[64]; long long cpf;
    prefixFunction(pat, m, pi, cpf);
    cout << "Префікс-функція зразка aaaab:";
    for (int i = 0; i < m; i++) cout << " " << pi[i];
    cout << " (порівнянь " << cpf << ")" << endl;
    int out[64]; KStat s; int c = kmpSearch(text, n, pat, m, pi, out, 64, s, false);
    cout << "КМП: усі входження:";
    for (int i = 0; i < c; i++) cout << " " << out[i];
    cout << ", порівнянь=" << s.cmp << ", зміщень=" << s.shifts << endl;
    long long nc; int out2[64];
    int c2 = naiveSearch(text, n, pat, m, out2, 64, nc);
    cout << "Прямий пошук: усі входження:";
    for (int i = 0; i < c2; i++) cout << " " << out2[i];
    cout << ", порівнянь=" << nc << endl;
}

// Експеримент: текст із символів a, зразок aaaab (як у практичній №8)
void experiment()
{
    cout << "\n=== Експеримент: КМП проти прямого пошуку ===" << endl;
    cout << "n\tСценарій\tКМП порівн.\tКМП зміщень\tПрямий порівн.\tЗбігів" << endl;
    const char* pat = "aaaab";
    int m = 5;
    int pi[64]; long long cpf;
    prefixFunction(pat, m, pi, cpf);
    int sizes[3] = { 100, 1000, 10000 };
    const char* names[4] = { "A (на початку)", "B (у середині)", "C (у кінці)", "D (відсутній)" };
    ofstream csv("results.csv");
    csv << "n,scenario,kmp_cmp,kmp_shifts,naive_cmp,matches\n";
    for (int si = 0; si < 3; si++)
        for (int sc = 0; sc < 4; sc++)
        {
            int n = sizes[si];
            char* t = new char[n + 1];
            for (int i = 0; i < n; i++) t[i] = 'a';
            t[n] = '\0';
            int pos = (sc == 0) ? 0 : (sc == 1) ? n / 2 : (sc == 2) ? n - 5 : -1;
            if (pos >= 0) t[pos + 4] = 'b';
            int out[64]; KStat s; int c = kmpSearch(t, n, pat, m, pi, out, 64, s, false);
            long long nc; naiveSearch(t, n, pat, m, out, 64, nc);
            cout << n << "\t" << names[sc] << "\t" << s.cmp << "\t\t" << s.shifts << "\t\t" << nc << "\t\t" << c << endl;
            csv << n << "," << names[sc] << "," << s.cmp << "," << s.shifts << "," << nc << "," << c << "\n";
            delete[] t;
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
