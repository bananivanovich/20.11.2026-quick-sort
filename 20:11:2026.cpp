#include <iostream>
#include <stdexcept>
#include <string>
#include <cmath>

//базовый класс
class Item {
public:
    virtual bool operator<(const Item&) = 0;
    virtual ~Item() { }
};

//число
class Real : public Item {
private:
    double value;
public:
    Real(double v = 0) : value(v) {}

    double Value() const { return value; }

    bool operator<(const Item& other) override {
        //типы совпадают
        const Real* r = dynamic_cast<const Real*>(&other);
        if (!r)
            throw std::runtime_error("Real::operator<: raznye tip");
        return value < r->value;
    }
};

//папа
class Pair : public Item {
private:
    double first;
    double second;
public:
    Pair(double a = 0, double b = 0) : first(a), second(b) {}

    double First()  const { return first; }
    double Second() const { return second; }

    bool operator<(const Item& other) override {
        const Pair* p = dynamic_cast<const Pair*>(&other);
        if (!p)
            throw std::runtime_error("Pair::operator<: raznye tip");

        // Лексикограф. сравнение
        if (first < p->first || first > p->first)
            return first < p->first;
        return second < p->second;
    }
};

//проверка отношений
void CheckOrder(Item** items, size_t count) {
    for (size_t i = 0; i < count; ++i) {
        for (size_t j = i + 1; j < count; ++j) {
            bool ij = items[i]->operator<(*items[j]);
            bool ji = items[j]->operator<(*items[i]);
            if (ij && ji) {
                throw std::runtime_error(
                    "error otnoshenie");
            }
        }
    }
}

//QuickSort

static void Swap(Item*& a, Item*& b) {
    Item* t = a;
    a = b;
    b = t;
}

// Разбиение Хоара
static size_t Partition(Item** items, size_t lo, size_t hi) {
    Item* pivot = items[lo + (hi - lo) / 2];
    size_t i = lo;
    size_t j = hi;

    while (true) {
        while (items[i]->operator<(*pivot)) ++i;
        while (pivot->operator<(*items[j])) {
            if (j == 0) break;
            --j;
        }
        if (i >= j) return j;
        Swap(items[i], items[j]);
        ++i;
        if (j == 0) break;
        --j;
    }
    return j;
}

static void QuickSortImpl(Item** items, size_t lo, size_t hi) {
    if (lo >= hi) return;
    size_t p = Partition(items, lo, hi);
    QuickSortImpl(items, lo, p);
    if (p + 1 <= hi)
        QuickSortImpl(items, p + 1, hi);
}
//end

void QuickSort(Item** items, size_t count) {
    if (items == nullptr || count < 2) return;

    CheckOrder(items, count);

    QuickSortImpl(items, 0, count - 1);
}

class BrokenItem : public Item {
public:
    bool operator<(const Item&) override {
        return true;
    }
};

static void PrintItems(const std::string& title, Item** items, size_t n) {
    std::cout << title << ": ";
    for (size_t i = 0; i < n; ++i) {
        if (Real* r = dynamic_cast<Real*>(items[i]))
            std::cout << r->Value() << " ";
        else if (Pair* p = dynamic_cast<Pair*>(items[i]))
            std::cout << "(" << p->First() << "," << p->Second() << ") ";
        else
            std::cout << "? ";
    }
    std::cout << "\n";
}

int main() {
    try {
        //пример числа
        {
            const size_t N = 8;
            Item* arr[N] = {
                new Real(3.14), new Real(-1.0), new Real(2.71), new Real(0.0),  new Real(10.5), new Real(-7.2), new Real(5.5),  new Real(1.0)
            };
            PrintItems("до ", arr, N);
            QuickSort(arr, N);
            PrintItems("после ", arr, N);
            for (size_t i = 0; i < N; ++i) delete arr[i];
        }

        //Пример пары
        {
            const size_t N = 6;
            Item* arr[N] = {
                new Pair(2, 5), new Pair(1, 9), new Pair(2, 1), new Pair(3, 0), new Pair(1, 3), new Pair(2, 3)
            };
            PrintItems("до ", arr, N);
            QuickSort(arr, N);
            PrintItems("после ", arr, N);
            for (size_t i = 0; i < N; ++i) delete arr[i];
        }

        //типы
        {
            Item* arr[2] = { new Real(1.0), new Pair(1, 2) };
            QuickSort(arr, 2);
            delete arr[0]; delete arr[1];
        }
    }
    catch (const std::exception& e) {
        std::cout << "смешанные типы: " << e.what() << "\n";
    }

    //отношение порядка
    try {
        const size_t N = 3;
        Item* arr[N] = { new BrokenItem(), new BrokenItem(), new BrokenItem() };
        QuickSort(arr, N);
        for (size_t i = 0; i < N; ++i) delete arr[i];
    }
    catch (const std::exception& e) {
        std::cout << "порядок: " << e.what() << "\n";
    }

    return 0;
}