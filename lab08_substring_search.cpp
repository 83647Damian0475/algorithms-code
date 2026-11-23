#include <cstring>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
using namespace std;

// Одиниця порівняння: зіставлення одного елемента основної послідовності з елементом зразка.
struct Stat { long long cmp; long long positions; long long matches; };

char lowerCh(char c, bool ignoreCase)
{
    if (!ignoreCase) return c;
    if (c >= 'A' && c <= 'Z') return (char)(c - 'A' + 'a');
    return c;
}

// Пошук першого входження. Початок допустимий, якщо зразок поміщається в текст.
int findFirst(const char t[], int n, const char p[], int m, Stat& s, bool ignoreCase)
{
    s.cmp = 0; s.positions = 0; s.matches = 0;
    if (m == 0) return 0;
    if (m > n) return -1;
    for (int i = 0; i <= n - m; i++)
    {
        s.positions++;
        int j = 0;
        while (j < m)
        {
            s.cmp++;
            if (lowerCh(t[i + j], ignoreCase) != lowerCh(p[j], ignoreCase)) break;
            j++;
        }
        if (j == m) return i;
    }
    return -1;
}

// Пошук усіх входжень, включно з накладеними: після збігу наступний початок — i + 1.
int findAll(const char t[], int n, const char p[], int m, int out[], int cap, Stat& s, bool ignoreCase)
{
    s.cmp = 0; s.positions = 0; s.matches = 0;
    if (m == 0) { if (cap > 0) out[0] = 0; return 1; }
    if (m > n) return 0;
    int k = 0;
    for (int i = 0; i <= n - m; i++)
    {
        s.positions++;
        int j = 0;
        while (j < m)
        {
            s.cmp++;
            if (lowerCh(t[i + j], ignoreCase) != lowerCh(p[j], ignoreCase)) break;
            j++;
        }
        if (j == m) { if (k < cap) out[k] = i; k++; s.matches++; }
    }
    return k;
}

// Варіант для числових масивів: та сама логіка з цілими значеннями.
int findFirstInt(const int t[], int n, const int p[], int m, Stat& s)
{
    s.cmp = 0; s.positions = 0; s.matches = 0;
    if (m == 0) return 0;
    if (m > n) return -1;
    for (int i = 0; i <= n - m; i++)
    {
        s.positions++;
        int j = 0;
        while (j < m) { s.cmp++; if (t[i + j] != p[j]) break; j++; }
        if (j == m) return i;
    }
    return -1;
}

int findAllInt(const int t[], int n, const int p[], int m, int out[], int cap)
{
    if (m == 0) { if (cap > 0) out[0] = 0; return 1; }
    if (m > n) return 0;
    int k = 0;
    for (int i = 0; i <= n - m; i++)
    {
        int j = 0;
        while (j < m) { if (t[i + j] != p[j]) break; j++; }
        if (j == m) { if (k < cap) out[k] = i; k++; }
    }
    return k;
}

bool checkText(int number, const char* text, const char* pat, int first, const vector<int>& all)
{
    int n = (int)strlen(text), m = (int)strlen(pat);
    Stat s1, s2; int out[64];
    int f = findFirst(text, n, pat, m, s1, false);
    int c = findAll(text, n, pat, m, out, 64, s2, false);
    int allCount = (int)all.size();
    bool ok = (f == first) && (c == allCount);
    for (int i = 0; ok && i < allCount; i++) if (out[i] != all[i]) ok = false;
    cout << "Текст\tТест " << setw(2) << number << ": " << (ok ? "успішно" : "помилка")
         << " (перше=" << f << ", усі=" << c << ")" << endl;
    return ok;
}

bool checkInt(int number, const int t[], int n, const int p[], int m, int first, const int all[], int allCount)
{
    Stat s1; int out[64];
    int f = findFirstInt(t, n, p, m, s1);
    int c = findAllInt(t, n, p, m, out, 64);
    bool ok = (f == first) && (c == allCount);
    for (int i = 0; ok && i < allCount; i++) if (out[i] != all[i]) ok = false;
    cout << "Числа\tТест " << setw(2) << number << ": " << (ok ? "успішно" : "помилка") << endl;
    return ok;
}

bool runAllTests()
{
    cout << "\n=== Автоматизовані перевірки ===" << endl;
    bool all = true;
    all &= checkText(1,  "algorithm", "algo", 0, vector<int>{0});
    all &= checkText(2,  "algorithm", "rith", 4, vector<int>{4});
    all &= checkText(3,  "algorithm", "thm", 6, vector<int>{6});
    all &= checkText(4,  "algorithm", "sort", -1, vector<int>{});
    all &= checkText(5,  "aaaa", "aa", 0, vector<int>{0, 1, 2});
    all &= checkText(6,  "abababa", "aba", 0, vector<int>{0, 2, 4});
    all &= checkText(7,  "abc", "abcd", -1, vector<int>{});
    all &= checkText(8,  "Data data DATA", "data", 5, vector<int>{5});
    all &= checkText(9,  "one two two", "two", 4, vector<int>{4, 8});
    all &= checkText(10, "", "a", -1, vector<int>{});
    all &= checkText(11, "banana", "ana", 1, vector<int>{1, 3});
    all &= checkText(12, "xyzxyz", "yz", 1, vector<int>{1, 4});
    all &= checkText(13, "aaaaabaaaaabaaaa", "aaaab", 1, vector<int>{1, 7});
    all &= checkText(14, "abcabcabd", "abd", 6, vector<int>{6});
    all &= checkText(15, "mississippi", "issi", 1, vector<int>{1, 4});
    all &= checkText(16, "aaaaaaa", "aaa", 0, vector<int>{0, 1, 2, 3, 4});
    all &= checkText(17, "text", "", 0, vector<int>{0});   // порожній зразок = вхідження на позиції 0
    cout << "\n=== Числові послідовності ===" << endl;
    int T1[5] = {1, 2, 1, 2, 1}, P1[3] = {1, 2, 1};
    int T2[3] = {3, 3, 3}, P2[2] = {3, 3};
    int T3[3] = {1, 2, 3}, P3[1] = {4};
    int E1[2] = {0, 2}, E2[2] = {0, 1};
    all &= checkInt(18, T1, 5, P1, 3, 0, E1, 2);
    all &= checkInt(19, T2, 3, P2, 2, 0, E2, 2);
    all &= checkInt(20, T3, 3, P3, 1, -1, nullptr, 0);
    return all;
}

// Варіант 16: основний текст aaaaabaaaaabaaaa, зразок aaaab
void variantRun()
{
    cout << "\n=== Варіант 2 ===" << endl;
    const char* text = "binary search searches sorted data";
    const char* pat = "search";
    int n = (int)strlen(text), m = (int)strlen(pat);
    Stat s1, s2; int out[64];
    int f = findFirst(text, n, pat, m, s1, false);
    int c = findAll(text, n, pat, m, out, 64, s2, false);
    cout << "Перше входження: " << f << ", порівнянь=" << s1.cmp << ", позицій перевірено=" << s1.positions << endl;
    cout << "Усі входження (" << c << "):";
    for (int i = 0; i < c; i++) cout << " " << out[i];
    cout << ", порівнянь=" << s2.cmp << ", позицій перевірено=" << s2.positions << endl;
    // режим без урахування регістру
    const char* text2 = "Binary SEARCH search";
    int n2 = (int)strlen(text2);
    Stat s3; int c3 = findAll(text2, n2, pat, m, out, 64, s3, true);
    cout << "Без урахування регістру (текст Binary SEARCH search): збігів=" << c3 << ", порівнянь=" << s3.cmp << endl;
    Stat s4; int c4 = findAll(text2, n2, pat, m, out, 64, s4, false);
    cout << "З урахуванням регістру (той самий текст): збігів=" << c4 << ", порівнянь=" << s4.cmp << endl;
}

// Експеримент: текст із символів 'a'; зразок "aaaab" ставиться в позиції p (A, B, C) або відсутній (D).
void experiment()
{
    cout << "\n=== Експеримент ===" << endl;
    cout << "n\tСценарій\tПерше\tПорівнянь\tПозицій\tЗбігів" << endl;
    const char* pat = "aaaab";
    int m = 5;
    int sizes[3] = { 100, 1000, 10000 };
    const char* names[4] = { "A (на початку)", "B (у середині)", "C (у кінці)", "D (відсутній)" };
    ofstream csv("results.csv");
    csv << "n,scenario,first,cmp,positions,matches\n";
    for (int si = 0; si < 3; si++)
        for (int sc = 0; sc < 4; sc++)
        {
            int n = sizes[si];
            char* t = new char[n + 1];
            for (int i = 0; i < n; i++) t[i] = 'a';
            t[n] = '\0';
            int p = (sc == 0) ? 0 : (sc == 1) ? n / 2 : (sc == 2) ? n - 5 : -1;
            if (p >= 0) t[p + 4] = 'b';
            Stat s1; int f = findFirst(t, n, pat, m, s1, false);
            Stat s2; int out[1]; int c = findAll(t, n, pat, m, out, 1, s2, false);
            cout << n << "\t" << names[sc] << "\t" << f << "\t" << s1.cmp << "\t\t" << s1.positions << "\t" << c << endl;
            csv << n << "," << names[sc] << "," << f << "," << s1.cmp << "," << s1.positions << "," << c << "\n";
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
