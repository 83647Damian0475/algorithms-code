#include <iostream>
#include <iomanip>
using namespace std;

int linearSearch(const int arr[], int size, int key, long long& comparisons)
{
    comparisons = 0;
    for (int i = 0; i < size; i++)
    {
        comparisons++;
        if (arr[i] == key)
            return i;
    }
    return -1;
}

int binarySearch(const int arr[], int size, int key, long long& comparisons)
{
    comparisons = 0;
    int left = 0;
    int right = size - 1;
    while (left <= right)
    {
        int mid = left + (right - left) / 2;
        comparisons++;
        if (arr[mid] == key)
            return mid;
        if (arr[mid] < key)
            left = mid + 1;
        else
            right = mid - 1;
    }
    return -1;
}

bool runCheck(int number, const int arr[], int size, int key, int expected)
{
    long long c1, c2;
    int r1 = linearSearch(arr, size, key, c1);
    int r2 = binarySearch(arr, size, key, c2);
    bool ok = (r1 == expected) && (r2 == expected);
    cout << "Тест " << setw(2) << number << ": ";
    if (ok) cout << "успішно" << endl;
    else cout << "помилка (очікувано " << expected << ", прямий " << r1 << ", бінарний " << r2 << ")" << endl;
    return ok;
}

static int bigArray[100000];

void printVariant()
{
    int original[7] = { 31, 5, 14, 27, 9, 2, 18 };
    int sorted[7]   = { 2, 5, 9, 14, 18, 27, 31 };
    int keys[3] = { 31, 2, 20 };
    cout << "\n=== Варіант 2: пошук ключів ===" << endl;
    for (int k = 0; k < 3; k++)
    {
        long long c1, c2;
        int r1 = linearSearch(original, 7, keys[k], c1);
        int r2 = binarySearch(sorted, 7, keys[k], c2);
        cout << keys[k] << "\t" << r1 << " / " << c1 << "\t" << r2 << " / " << c2 << endl;
    }
}

bool runAllTests()
{
    cout << "\n=== Автоматизовані перевірки ===" << endl;
    int one[1] = { 8 };
    int seven[7] = { 2, 5, 8, 11, 14, 17, 20 };
    int empty[1] = { 0 };
    int v2[7] = { 2, 5, 9, 14, 18, 27, 31 };
    int two[2] = { 3, 9 };
    bool all = true;
    all &= runCheck(1, one, 1, 8, 0);
    all &= runCheck(2, one, 1, 3, -1);
    all &= runCheck(3, seven, 7, 2, 0);
    all &= runCheck(4, seven, 7, 11, 3);
    all &= runCheck(5, seven, 7, 20, 6);
    all &= runCheck(6, seven, 7, 9, -1);
    all &= runCheck(7, seven, 7, -4, -1);
    all &= runCheck(8, seven, 7, 100, -1);
    all &= runCheck(9, empty, 0, 5, -1);
    all &= runCheck(10, v2, 7, 31, 6);
    all &= runCheck(11, v2, 7, 2, 0);
    all &= runCheck(12, v2, 7, 20, -1);
    all &= runCheck(13, two, 2, 9, 1);
    all &= runCheck(14, two, 2, 5, -1);
    // власний набір даних D (n = 15) для найкращого, середнього та найгіршого випадків
    int D[15] = { 3, 8, 12, 19, 23, 30, 37, 44, 52, 61, 70, 79, 88, 96, 105 };
    all &= runCheck(15, D, 15, 44, 7);   // найкращий: середній елемент
    all &= runCheck(16, D, 15, 3, 0);    // перший елемент
    all &= runCheck(17, D, 15, 105, 14); // останній елемент
    all &= runCheck(18, D, 15, 50, -1);  // відсутній між елементами
    all &= runCheck(19, D, 15, 1, -1);   // відсутній менший за мінімум
    all &= runCheck(20, D, 15, 200, -1); // відсутній більший за максимум
    return all;
}

void experiment()
{
    cout << "\n=== Порівняльний експеримент ===" << endl;
    int sizes[4] = { 10, 100, 1000, 100000 };
    for (int s = 0; s < 4; s++)
    {
        int n = sizes[s];
        for (int i = 0; i < n; i++) bigArray[i] = i;
        int keys[3] = { 0, n - 1, n };
        char names[3] = { 'A', 'B', 'C' };
        for (int k = 0; k < 3; k++)
        {
            long long c1, c2;
            linearSearch(bigArray, n, keys[k], c1);
            binarySearch(bigArray, n, keys[k], c2);
            cout << n << "\t" << names[k] << "\t" << c1 << "\t" << c2 << endl;
        }
    }
}

void averages()
{
    cout << "\n=== Середні ===" << endl;
    cout << fixed << setprecision(3);
    int sizes[4] = { 10, 100, 1000, 100000 };
    for (int s = 0; s < 4; s++)
    {
        int n = sizes[s];
        for (int i = 0; i < n; i++) bigArray[i] = i;
        long long sum1 = 0, sum2 = 0;
        for (int i = 1; i <= 1000; i++)
        {
            int key = (int)(((long long)i * 37) % n);
            long long c1, c2;
            linearSearch(bigArray, n, key, c1);
            binarySearch(bigArray, n, key, c2);
            sum1 += c1; sum2 += c2;
        }
        cout << n << "\t" << sum1 / 1000.0 << "\t" << sum2 / 1000.0 << endl;
    }
}

void unsortedDemo()
{
    cout << "\n=== Невпорядкований ===" << endl;
    int bad[6] = { 12, 3, 19, 5, 8, 1 };
    int good[6] = { 1, 3, 5, 8, 12, 19 };
    for (int i = 0; i < 6; i++)
    {
        long long c;
        int r = binarySearch(bad, 6, bad[i], c);
        cout << "bad key " << bad[i] << " exp " << i << " got " << r << " cmp " << c << endl;
    }
    for (int i = 0; i < 6; i++)
    {
        long long c;
        int r = binarySearch(good, 6, good[i], c);
        cout << "good key " << good[i] << " exp " << i << " got " << r << " cmp " << c << endl;
    }
}


void customDataset()
{
    cout << "\n=== Власний набір даних D (n = 15) ===" << endl;
    int D[15] = { 3, 8, 12, 19, 23, 30, 37, 44, 52, 61, 70, 79, 88, 96, 105 };
    // прогноз (зроблено аналітично до запуску): індекс і порівняння
    int keys[6]     = { 3, 44, 105, 50, 200, 1 };
    int predLinCmp[6] = { 1, 8, 15, 15, 15, 15 };
    int predBinCmp[6] = { 4, 1, 4, 4, 4, 4 };
    cout << "Ключ\tПрогноз(пр.)\tФакт(пр.)\tПрогноз(бін.)\tФакт(бін.)" << endl;
    for (int k = 0; k < 6; k++)
    {
        long long c1, c2;
        linearSearch(D, 15, keys[k], c1);
        binarySearch(D, 15, keys[k], c2);
        cout << keys[k] << "\t" << predLinCmp[k] << "\t\t" << c1
             << "\t\t" << predBinCmp[k] << "\t\t" << c2 << endl;
    }
    // середнє за всіма 15 присутніми ключами: прогноз 49/15 ≈ 3,267 для бінарного, (15+1)/2 = 8 для прямого
    long long s1 = 0, s2 = 0;
    for (int i = 0; i < 15; i++)
    {
        long long c1, c2;
        linearSearch(D, 15, D[i], c1);
        binarySearch(D, 15, D[i], c2);
        s1 += c1; s2 += c2;
    }
    cout << fixed << setprecision(3);
    cout << "Середнє по всіх присутніх ключах: прямий " << s1 / 15.0
         << ", бінарний " << s2 / 15.0 << " (прогноз: 8,000 і 3,267)" << endl;
    cout.unsetf(ios::fixed);
}

int main()
{
    printVariant();
    customDataset();
    if (!runAllTests()) { cout << "ERR" << endl; return 1; }
    experiment();
    averages();
    unsortedDemo();
    return 0;
}
