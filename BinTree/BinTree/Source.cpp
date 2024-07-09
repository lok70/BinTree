#include <iostream>
#include <set>
#include <iterator>
#include <vector>
#include <list>
#include <algorithm>
#include <random>
#include "BStree.h"
#include "Set.h"
#include <chrono>
#include <fstream>
#include <stdlib.h> 
#include <time.h> 
#include <intrin.h>

#pragma intrinsic(__rdtsc)
using namespace std;
using namespace std::chrono;

template<typename InputIter>
void print(InputIter first, InputIter last) {

	if (std::is_same<typename iterator_traits<InputIter>::iterator_category, std::random_access_iterator_tag>::value) {
		cout << "Random access iterator range : ";
		while (first != last)
			cout << *first++ << " ";
		cout << endl;
	}
	else {
		cout << "Any input iterator range : ";
		while (first != last)
			cout << *first++ << " ";
		cout << endl;
	}
}


list<size_t> get_random_list(size_t digit, size_t count)
{
    //Проверка разряда
    if (digit < 1)
        throw std::invalid_argument("Разряд должен быть больше 0");
    size_t n = pow(10, digit - 1); // Создаю минимальное число нужного разряда
    list<size_t> list;

    for (size_t i = 1; i <= count; i++)
    {
        // Получить случайное число - формула
        size_t num = n + rand() % (n * 10 - n);
        list.push_back(num); //Добавляю в список
    }
    return list;
}

bool isEven(int value)
{
	return value % 2 == 0;
}

void Eratosthenes(bool massiv[], int N)
{
	int i, j;
	for (j = 2; j <= N; j++) 
		massiv[j] = true;
	j = 2;
	while (j * j <= N)
	{
		i = j * j;
		if (massiv[j])
			while (i <= N)
			{
				massiv[i] = false;
				i = i + j;
			}
		j = j + 1;
	}
	cout << "Список простых чисел: ";
	for (j = 2; j <= N; j++)
	{
		if (massiv[j] == true) cout << " " << j;
	}
}

int main() {
	setlocale(LC_ALL, "Russian");



	{
		cout << "------------Шаблон------------" << endl;
		const size_t sz = 15;
		vector<int> v;
		v.reserve(sz);
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> dis(0, +1000);
		generate_n(back_inserter(v), sz, [&]() {return (dis(gen) % 5000); });
		sort(v.begin(), v.end());
		cout << "\n -------------------------------- \n";
		copy(v.begin(), v.end(), ostream_iterator<int>(cout, " "));
		cout << "\n -------------------------------- \n";
		set<int> bb(v.begin(), v.end());
		print(bb.begin(), bb.end());
		Binary_Search_Tree<int> bst;
		cout << bst.size() << endl;
		cout << "--------------------------------- \n \n";
	}

	{
		cout << "------------Проверка итератора------------" << endl;
		Binary_Search_Tree<int> numbers { 10, 50, 40, 20, 30 };
		for (auto it = numbers.begin(); it != numbers.end(); it++)
			std::cout << *it << ' ';
		std::cout << '\n';
		cout << "Возьмём последний элемент (первый элемент с конца): " << *numbers.rbegin() << endl;
		cout << "Проверяем итераторы с помощью алгоритма std::find" << endl;
		cout << "std::find 20: " << *std::find(numbers.begin(), numbers.end(), 20) << endl;
		cout << "std::find 21: " << *std::find(numbers.begin(), numbers.end(), 21) << endl;
		cout << "Проверяем итераторы с помощью алгоритма std::count_if" << endl;
		cout << "std::count_if четные: " << std::count_if(numbers.begin(), numbers.end(), isEven) << endl;
		cout << "--------------------------------- \n \n";
	}

	{

		Binary_Search_Tree<size_t> tree1{ 1, 78, 9 , 10, 5, 21, 3 };
		Binary_Search_Tree<long long> tree2{ 1, -78, 9 , 10, 5, 21, 3, 0 ,-9 };
		Binary_Search_Tree<size_t> treeNull();

		{
			cout << "Задание 1" << endl;

			cout << "Дерево: "; tree1.printInfixRecur();
			cout << "Добавляем лист 8" << endl;
			tree1.addLeaf(8);
			cout << "Дерево: "; tree1.printInfixRecur();
			cout << endl;

			cout << "Дерево: "; tree2.printInfixRecur();
			cout << "Добавляем лист -79" << endl;
			tree2.addLeaf(-79);
			cout << "Дерево: "; tree2.printInfixRecur();
			cout << endl;

			cout << "Дерево: "; tree2.printInfixRecur();
			cout << "Добавляем лист 60" << endl;
			tree2.addLeaf(60);
			cout << "Дерево: "; tree2.printInfixRecur();
			cout << endl;

			cout << endl;
		}

		{
			cout << "Задание 2" << endl;

			cout << "Дерево: "; tree1.printInfixRecur();
			cout << "Ищем 8: ";
			auto x = tree1.findRecur(8);
			if (x != nullptr)
			{
				cout << x->data << endl;
			}
			else
			{
				cout << "Не нашли" << endl;
			}
			cout << endl;

			cout << "Дерево: "; tree1.printInfixRecur();
			cout << "Ищем 111: ";
			x = tree1.findRecur(111);
			if (x != nullptr)
			{
				cout << x->data << endl;
			}
			else {
				cout << "Не нашли" << endl;
			}
			cout << endl;

			cout << "Дерево: "; tree1.printInfixRecur();
			cout << "Ищем 11: ";
			x = tree1.findRecur(11);
			if (x != nullptr)
			{
				cout << x->data << endl;
			}
			else {
				cout << "Не нашли" << endl;
			}
			cout << endl;

			cout << endl;
		}


		{
			cout << "Задание 3" << endl;

			cout << "Дерево: "; tree1.printInfixRecur();
			auto x = tree1.max();
			if (x != nullptr)
				cout << "Максимальный элемент: " << x->data << endl;
			else
				cout << "Пустое дерево" << endl;
			x = tree1.min();
			if (x != nullptr)
				cout << "Минимальный элемент: " << x->data << endl;
			else
				cout << "Пустое дерево" << endl;
			cout << endl;

			cout << "Дерево: "; tree2.printInfixRecur();
			auto y = tree2.max();
			if (y != nullptr)
				cout << "Максимальный элемент: " << y->data << endl;
			else
				cout << "Пустое дерево" << endl;
			y = tree2.min();
			if (y != nullptr)
				cout << "Минимальный элемент: " << y->data << endl;
			else
				cout << "Пустое дерево" << endl;
			cout << endl;

			cout << endl;
		}

		{
			cout << "Задание 4" << endl;

			cout << "Дерево: "; tree1.printInfixRecur();
			cout << "Ищем первое меньше либо равное 5: ";
			cout << *tree1.findLessOrEqual(5) << endl;
			cout << endl;

			cout << "Дерево: "; tree2.printInfixRecur();
			cout << "Ищем первое больше либо равное 20: ";
			cout << *tree2.findGraterOrEqual(20) << endl;
			cout << endl;

			cout << "Дерево: "; tree2.printInfixRecur();
			cout << "Ищем первое больше либо равное 1000: ";
			cout << *tree2.findGraterOrEqual(1000) << endl;
			cout << endl;

			cout << endl;
		}

		{
			cout << "Задание 5" << endl;

			cout << "Дерево: "; tree1.printInfixRecur();
			cout << "Удаляем лист 1" << endl;
			tree1.deleteLeaf(1);
			cout << "Дерево: "; tree1.printInfixRecur();
			cout << endl;

			cout << "Дерево: "; tree2.printInfixRecur();
			cout << "Удаляем лист 21" << endl;
			tree2.deleteLeaf(21);
			cout << "Дерево: "; tree2.printInfixRecur();
			cout << endl;

			cout << "Дерево: "; tree2.printInfixRecur();
			cout << "Удаляем лист 10" << endl;
			tree2.deleteLeaf(10);
			cout << "Дерево: "; tree2.printInfixRecur();

			cout << endl;
		}
		
		{
			cout << "Задание 6" << endl;

			cout << "Инфиксный рекурсивный вывод на экран" << endl;
			cout << "Дерево: "; tree1.printInfixRecur();
			cout << "Дерево: "; tree2.printInfixRecur();

			cout << endl;
		}

		{
			cout << "Задание 7" << endl;

			cout << "Постфиксный с помощью стека вывод на экран" << endl;
			cout << "Дерево: "; tree1.printPostfixStack();
			cout << "Дерево: "; tree2.printPostfixStack();

			cout << endl;
		}

		{
			cout << "Задание 8" << endl;

			cout << "Вывод на экран «по слоям» – с использованием очереди" << endl;
			cout << "Дерево: " << endl;
			tree1.printLayersQueue();
			cout << "Дерево: " << endl;
			tree2.printLayersQueue();

			cout << endl;
		}

		{
			cout << "Задание 9" << endl;

			tree1 = Binary_Search_Tree<size_t>({ 3, 8, 5, 9, 10, 78 });
			Binary_Search_Tree<size_t> tree11({ 3, 5, 8, 9, 10, 78 });
			cout << "Дерево: "; tree1.printInfixRecur();
			cout << "Дерево: "; tree11.printInfixRecur();
			bool equals = tree1 == tree11;
			cout << "Равны ли деревья: " << equals << endl;
			cout << endl;

			Binary_Search_Tree<size_t> tree12({ 1, 78, 9 , 10, 5, 21, 4 });
			cout << "Дерево: "; tree1.printInfixRecur();
			cout << "Дерево: "; tree12.printInfixRecur();
			equals = tree1 == tree12;
			cout << "Равны ли деревья: " << equals << endl;

			cout << endl;
		}


		{
			cout << "Задание 10" << endl;

			cout << "Поиск для заданного (в виде указателя или итератора) элемента следующего и предыдущего элементов" << endl;
			cout << "Дерево: "; tree1.printInfixRecur();
			cout << "Элемент 3. Следующий элемент: ";
			cout << *tree1.findNextForIterator(tree1.begin()) << endl;

			cout << endl;
		}
	}

	




	{
		cout << "------------Проверка времени------------" << endl;
        //Сравнение со стандартным списком на быстродействие 
        {
            ofstream out("results.csv"); //связываем csv файл с потоком данных
            out << "Ticks : ;";
            out << "find on my : ;";  out << "find on standart : ;"; //Печатаю названия функций
			out << "count_if on my : ;";  out << "count_if on standart : ;";
			out << "insert on set : ;";  out << "insert on mySet : ;";
            out << endl;

            for (auto i = 10; i <= 10000; i *= 10) //Цикл по кол-ву элементов в списках
            {
                for (auto j = 1; j <= 9; j += 3) //Цикл по разряду чисел в списках
                {
                    for (auto x = 1; x <= 3; x++) //Цикл по кол-ву списков на которых я хочу проверить
                    {
                        list<size_t> sl = get_random_list(j, i);
						Binary_Search_Tree<size_t> st = Binary_Search_Tree<size_t>(sl);
						Set<size_t> ms = Set<size_t>(sl);
						set<size_t> s;
						for (const auto& x : sl)
							s.insert(x);

                        out << "Count: " << i << "; ";

                        //Сравнение std::find на листе
                        {
                            unsigned __int64 ticks = __rdtsc(); //Оценка времени выполнения факторизации в тиках
							find(sl.begin(), sl.end(), 1);
                            ticks = __rdtsc() - ticks; // Находим разницу между начальным количеством тиков и конечным
                            out << ticks << "; ";
                        }

						//Сравнение std::find на итераторе дерева
						{
							unsigned __int64 ticks = __rdtsc(); //Оценка времени выполнения факторизации в тиках
							find(st.begin(), st.end(), 1);
							ticks = __rdtsc() - ticks; // Находим разницу между начальным количеством тиков и конечным
							out << ticks << "; ";
						}

						sl = get_random_list(j, i);
						st = Binary_Search_Tree<size_t>(sl);

						//Сравнение std::count на листе
						{
							unsigned __int64 ticks = __rdtsc(); //Оценка времени выполнения факторизации в тиках
							count_if(sl.begin(), sl.end(), isEven);
							ticks = __rdtsc() - ticks; // Находим разницу между начальным количеством тиков и конечным
							out << ticks << "; ";
						}

						//Сравнение std::count на итераторе дерева
						{
							unsigned __int64 ticks = __rdtsc(); //Оценка времени выполнения факторизации в тиках
							count_if(st.begin(), st.end(), isEven);
							ticks = __rdtsc() - ticks; // Находим разницу между начальным количеством тиков и конечным
							out << ticks << "; ";
						}

						//Сравнение set
						{
							unsigned __int64 ticks = __rdtsc(); //Оценка времени выполнения факторизации в тиках
							s.insert(85);
							ticks = __rdtsc() - ticks; // Находим разницу между начальным количеством тиков и конечным
							out << ticks << "; ";
						}

						//Сравнение mySet
						{
							unsigned __int64 ticks = __rdtsc(); //Оценка времени выполнения факторизации в тиках
							ms.insert(85);
							ticks = __rdtsc() - ticks; // Находим разницу между начальным количеством тиков и конечным
							out << ticks << "; ";
						}

                        out << "Digit: " << j << "; ";
                        out << endl;
                    }
                }
            }
        }
	}

	#ifdef _WIN32
		system("pause");
	#endif 
	return 0;
}