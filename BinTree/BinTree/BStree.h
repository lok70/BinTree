#pragma once
#include <iostream>
#include <cassert>
#include <queue>
#include <stack>
#include <vector>
#include <string>
#include <iterator>
#include <memory>
#include <memory_resource>
#include <initializer_list>
#include <functional>
#include <list>
#include <exception>
#include <set>

using namespace std;

template<typename T, typename Compare = std::less<T>, typename Allocator = std::allocator<T>>
class Binary_Search_Tree {
	//  Объект для сравнения ключей. Должен удовлетворять требованию строго слабого порядка, т.е. иметь свойства:
//    1. Для любого x => cmp(x,x) == false (антирефлексивность)
//    2. Если cmp(x,y) == true  =>  cmp(y,x) == false (асимметричность)
//    3. Если cmp(x,y) == cmp(y,z) == true  =>  cmp(x,z) == true  (транзитивность)
//    4. Если cmp(x,y) == cmp(y,z) == false  =>  cmp(x,z) == false  (транзитивность несравнимости)
//  Этим свойством обладает, к примеру, оператор <. Если нужно использовать оператор <= , который не обладает
//     нужными свойствами, то можно использовать его отрицание и рассматривать дерево как инвертированное от требуемого.
	// Объект для сравнения ключей, должен удовлетворять требованию строго слабого порядка (антирефлексивность, ассиметричность, 
	// транзитивность, транзитивность несравнимости)
	Compare cmp = Compare();

	//  Узел бинарного дерева, хранит ключ, три указателя и признак nil для обозначения фиктивной вершины
	struct Node {
		T data;
		Node* left;
		Node* right;
		Node* parent;
		Node(T value = T(), Node* p = nullptr, Node* l = nullptr, Node* r = nullptr) : parent(p), data(value), left(l), right(r) {}
		bool isdummy;
	};

	//  Стандартные контейнеры позволяют указать пользовательский аллокатор, который используется для
	//  выделения и освобождения памяти под узлы (реализует замену операций new/delete). К сожалению, есть 
	//  типичная проблема – при создании дерева с ключами типа T параметром шаблона традиционно указывается
	//  std::allocator<T>, который умеет выделять память под T, а нам нужен аллокатор для выделения/освобождения
	//  памяти под Node, т.е. std::allocator<Node>. Поэтому параметр шаблона аллокатора нужно изменить
	//  с T на Node, что и делается ниже. А вообще это одна из самых малополезных возможностей - обычно мы
	//  пользовательские аллокаторы не пишем, это редкость.

	//  Определяем тип аллокатора для Node (Allocator для T нам не подходит)
	typedef typename std::allocator_traits<Allocator>::template rebind_alloc<Node> AllocType;

	//  Аллокатор для выделения памяти под объекты Node
	AllocType Alc;

	
	void clone(Node* from, Node* other_dummy)
	{
		if (from == other_dummy)
			return;
		
		insert(from->data);
		clone(from->right, other_dummy);
		clone(from->left, other_dummy);
	}

public:
	// Определение необходимых типов для контейнера
	typedef T key_type;
	typedef Compare key_compare;
	typedef Compare value_compare;
	typedef T value_type;
	typedef Allocator allocator_type;
	typedef size_t size_type;
	typedef std::ptrdiff_t difference_type;
	typedef value_type& reference;
	typedef typename std::allocator_traits<Allocator>::pointer const_reference;
	typedef value_type* pointer;
	typedef typename std::allocator_traits<Allocator>::const_pointer const_pointer;
	class iterator;
	typedef iterator const_iterator;
	typedef std::reverse_iterator<iterator> reverse_iterator;
	typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

private:
	// Указатель на фиктивную вершину
	Node* dummy;

	// Размер дерева
	size_type tree_size = 0;

	inline Node* make_dummy() 
	{
		dummy = Alc.allocate(1);

		std::allocator_traits<AllocType>::construct(Alc, &(dummy->parent));
		dummy->parent = dummy;

		std::allocator_traits<AllocType>::construct(Alc, &(dummy->left));
		dummy->left = dummy;

		std::allocator_traits<AllocType>::construct(Alc, &(dummy->right));
		dummy->right = dummy;

		dummy->isdummy = true;

		return dummy;
	}

	inline Node* make_Node(const value_type& elem, Node* parent, Node* left, Node* right) 
	{
		Node* new_Node = Alc.allocate(1);

		std::allocator_traits<AllocType>::construct(Alc, &(new_Node->parent));
		new_Node->parent = parent;

		std::allocator_traits<AllocType>::construct(Alc, &(new_Node->left));
		new_Node->left = left;

		std::allocator_traits<AllocType>::construct(Alc, &(new_Node->right));
		new_Node->right = right;

		std::allocator_traits<AllocType>::construct(Alc, &(new_Node->data));
		new_Node->data = elem;

		new_Node->isdummy = false;

		return new_Node;
	}

	inline void delete_dummy(Node* n) {
		std::allocator_traits<AllocType>::destroy(Alc, &(n->parent));
		std::allocator_traits<AllocType>::destroy(Alc, &(n->left));
		std::allocator_traits<AllocType>::destroy(Alc, &(n->right));
		std::allocator_traits<AllocType>::deallocate(Alc, n, 1);
	}

	inline void delete_Node(Node* n) {
		std::allocator_traits<AllocType>::destroy(Alc, &(n->data));
		delete_dummy(n);
	}

public:
	class iterator : public std::iterator<std::input_iterator_tag, Node>
	{
		friend class Binary_Search_Tree;
	protected:
		Node* p;

		explicit iterator(Node* pp) : p(pp) {}

		Node*& _p() {
			return p;
		}

		iterator parent() const noexcept {
			return iterator(p->parent);
		}
		iterator left() const noexcept {
			return iterator(p->left);
		}
		iterator right() const noexcept {
			return iterator(p->right);
		}
		bool is_left() const noexcept {
			return p->parent->left == p;
		}
		bool is_right() const noexcept {
			return p->parent->right == p;
		}
		iterator get_min() {
			Node* cur = p;
			if (cur->isdummy) 
				return iterator(cur);
			while (!cur->left->isdummy) 
				cur = cur->left;
			return iterator(cur);
		}
		iterator get_max() {
			Node* cur = p;
			if (cur->isdummy) 
				return iterator(cur);
			while (!cur->right->isdummy) 
				cur = cur->right;
			return iterator(cur);
		}
		//Поиск для заданного (в виде указателя или итератора) элемента следующего элементa
		Node* next()
		{
			Node* cur = p;
			if (cur->right->right == cur) 
				return cur->right;
			if (cur->right->parent == cur)
			{
				cur = cur->right;
				while (cur->left->parent == cur)
					cur = cur->left;
			}
			else
			{
				while (cur->parent->left != cur)
					cur = cur->parent;
				cur = cur->parent;
			}
			return cur;
		}
		//Поиск для заданного (в виде указателя или итератора) элемента предыдущего элементa
		Node* previous()
		{
			Node* cur = p;
			if (cur->left->left == cur) 
				return cur->right;
			if (cur->left->parent == cur)
			{
				cur = cur->left;
				while (cur->right->parent == cur)
					cur = cur->right;
			}
			else
			{
				while (cur->parent->right != cur)
					cur = cur->parent;
				cur = cur->parent;
			}
			return cur;
		}
	public:
	 	typedef std::bidirectional_iterator_tag iterator_category;
		typedef Binary_Search_Tree::value_type value_type;
		typedef Binary_Search_Tree::difference_type difference_type;
		typedef Binary_Search_Tree::pointer pointer;
		typedef Binary_Search_Tree::reference reference;

		bool isdummy() const { 
			return p->isdummy; 
		}

		T& operator*() const {
			return p->data;
		}

		iterator& operator++() {
			p = next();
			return *this;
		}
		iterator operator++(int) {
			iterator answ = *this;
			++* this;
			return answ;
		}
		iterator& operator--() {
			p = previous();
			return *this;
		}
		iterator operator--(int) {
			iterator answ = *this;
			--* this;
			return answ;
		}
		friend bool operator == (const iterator& it_1, const iterator& it_2)
		{
			if (it_1.isdummy() && it_2.isdummy()) return true;
			else if (it_1.isdummy() || it_2.isdummy()) return false;
			return *it_1 == *it_2;
		}
		friend bool operator != (const iterator& it_1, const iterator& it_2)
		{
			return !(it_1 == it_2);
		}

		//  Эти операции не допускаются между прямыми и обратными итераторами
		const iterator& operator=(const reverse_iterator& it) = delete;
		bool operator==(const reverse_iterator& it) = delete;
		bool operator!=(const reverse_iterator& it) = delete;
		iterator(const reverse_iterator& it) = delete;
	};

	iterator begin() const noexcept { return iterator(dummy->left); }
	iterator end() const noexcept { return iterator(dummy); }

	reverse_iterator rbegin() const	noexcept { return reverse_iterator(iterator(end())); }
	reverse_iterator rend() const noexcept { return reverse_iterator(iterator(begin())); }

	Binary_Search_Tree(Compare comparator = Compare(), AllocType alloc = AllocType())
		: dummy(make_dummy()), cmp(comparator), Alc(alloc) {}

	Binary_Search_Tree(std::initializer_list<T> il) : dummy(make_dummy())
	{
		for (const auto& x : il)
			insert(x);
	}

	Binary_Search_Tree(list<T> il) : dummy(make_dummy())
	{
		for (const auto& x : il)
			insert(x);
	}

	AllocType get_allocator() const noexcept { return Alc; }
	key_compare key_comp() const noexcept { return cmp; }
	value_compare value_comp() const noexcept { return cmp; }

	inline bool empty() const noexcept { return tree_size == 0; }

private:
	template <class RandomIterator>
	void ordered_insert(RandomIterator first, RandomIterator last, iterator position) 
	{
		if (first >= last) return;
		RandomIterator center = first + (last - first) / 2;
		iterator new_pos = insert(position, *center);  //  итератор на вставленный элемент
		ordered_insert(first, center, position);
		ordered_insert(center + 1, last, ++position);
	}

public:
	template <class InputIterator>
	Binary_Search_Tree(InputIterator first, InputIterator last, Compare comparator = Compare(), AllocType alloc = AllocType()) : dummy(make_dummy()), cmp(comparator), Alc(alloc)
	{
		//  Проверка - какой вид итераторов нам подали на вход
		if (std::is_same<typename std::iterator_traits<InputIterator>::iterator_category, typename std::random_access_iterator_tag>::value) {
			//  Если итератор произвольного доступа, то есть надежда, что диапазон отсортирован
			//    а даже если и нет - не важно, всё равно попробуем метод деления пополам для вставки
			ordered_insert(first, last, end());
		}
		else
			std::for_each(first, last, [this](T x) { insert(x); });
	}

	Binary_Search_Tree(const Binary_Search_Tree& tree) : dummy(make_dummy())
	{	//  Размер задаём
		tree_size = tree.tree_size;
		if (tree.empty()) return;

		dummy->parent = recur_copy_tree(tree.dummy->parent, tree.dummy);
		dummy->parent->parent = dummy;

		//  Осталось установить min и max
		dummy->left = iterator(dummy->parent).get_min()._p();
		dummy->right = iterator(dummy->parent).get_max()._p();
	}

private:
	//  Рекурсивное копирование дерева
	Node* recur_copy_tree(Node* source, const Node* source_dummy)
	{
		//  Сначала создаём дочерние поддеревья
		Node* left_sub_tree;
		if (source->left != source_dummy)
			left_sub_tree = recur_copy_tree(source->left, source_dummy);
		else
			left_sub_tree = dummy;

		Node* right_sub_tree;
		if (source->right != source_dummy)
			right_sub_tree = recur_copy_tree(source->right, source_dummy);
		else
			right_sub_tree = dummy;

		//  Теперь создаём собственный узел
		Node* current = make_Node(source->data, nullptr, left_sub_tree, right_sub_tree);
		//  Устанавливаем родителей
		if (source->right != source_dummy)
			current->right->parent = current;
		if (source->left != source_dummy)
			current->left->parent = current;
		//  Ну и всё, можно возвращать
		return current;
	}
public:
	const Binary_Search_Tree& operator=(const Binary_Search_Tree& tree)
	{
		if (this == &tree) return *this;

		Binary_Search_Tree tmp{ tree };
		swap(tmp);

		return *this;
	}

	size_type size() const { return tree_size; }

	// Обмен содержимым двух контейнеров
	void swap(Binary_Search_Tree& other) noexcept {
		std::swap(dummy, other.dummy);

		//  Обмен размера множеств
		std::swap(tree_size, other.tree_size);
	}

	//  Вставка элемента по значению. 
	std::pair<iterator, bool> insert(const T& value)
	{
		Node* curr = dummy->parent;
		Node* prev = dummy;
		while (!curr->isdummy) {
			prev = curr;
			if (cmp(value, curr->data)) curr = curr->left;
			else curr = curr->right;
		}
		Node* new_Node = make_Node(value, prev, dummy, dummy);
		if (tree_size == 0) {
			prev->parent = new_Node;
			prev->left = new_Node;
			prev->right = new_Node;
		}
		else if (cmp(value, prev->data))
			prev->left = new_Node;
		else
			prev->right = new_Node;

		if (cmp(value, dummy->left->data)) {
			dummy->left = new_Node;
		}
		if (cmp(dummy->right->data, value)) {
			dummy->right = new_Node;
		}
		tree_size++;
		return std::make_pair(iterator(new_Node), true);
	}

	iterator insert(const_iterator position, const value_type& x) {
		Node* new_Node;
		iterator prev = position;
		prev--;

		if (prev.isdummy()) {
			new_Node = position._p()->left = make_Node(x, position._p(), dummy, dummy);
		}
		else {
			new_Node = prev._p()->right = make_Node(x, position._p(), dummy, prev._p()->right);
		}
		if (tree_size == 0) {
			dummy->parent = new_Node;
			dummy->left = new_Node;
			dummy->right = new_Node;
		}

		if (cmp(x, dummy->left->data)) {
			dummy->left = new_Node;
		}
		if (cmp(dummy->right->data, x)) {
			dummy->right = new_Node;
		}
		tree_size++;

		return iterator(new_Node);
	}

	//  Не самый лучший вариант.
	template <class InputIterator>
	void insert(InputIterator first, InputIterator last) {
		while (first != last) insert(*first++);
	}

	iterator lower_bound(const value_type& key) {
		iterator current{ dummy->parent }, result{ dummy->parent };

		while (current != end()) {
			if (!cmp(*current, key)) {
				result = current;
				current = current.left();
			}
			else
				current = current.right();
		}
		return result;
	}

	const_iterator lower_bound(const value_type& key) const {
		return const_iterator(const_cast<Binary_Search_Tree*>(this)->lower_bound(key));
	}

	iterator upper_bound(const value_type& key) {

		iterator current{ dummy->parent }, result{ dummy->parent };

		while (current != end()) {
			if (cmp(key, *current)) {
				result = current;
				current = current.left();
			}
			else {
				current = current.right();
			}
		}

		return result;
	}

	const_iterator upper_bound(const value_type& key) const {
		return const_iterator(const_cast<Binary_Search_Tree*>(this)->upper_bound(key));
	}

	size_type count(const value_type& key) const {
		return find(key) != end() ? 1 : 0;
	}

	std::pair<const_iterator, const_iterator> equal_range(const value_type& key) const {
		return std::make_pair(lower_bound(key), upper_bound(key));
	}
protected:
	//  Удаление листа дерева. Возвращает количество удалённых элементов
	size_type delete_leaf(iterator p) {
		Node* p_old = p._p();
		if (p.parent()._p()->right == p_old) // Если лист правый у родителя
		{
			if (p_old->right->isdummy)
			{
				dummy->right = p_old->parent;
				p_old->parent->right = dummy->right;
			}
			else
				p_old->parent->right = nullptr;
		}
		else // Если лист левый у родителя
		{
			if (p_old->left->isdummy)
			{
				dummy->left = p_old->parent;
				p_old->parent->left = dummy->left;
			}
			else
				p_old->parent->left = nullptr;
		}
		tree_size--;
		delete_Node(p_old);
		return 1;
	}

	//  Меняет местами текущий узел и максимальный в левом поддеревею Возвращает тот же итератор, но теперь он без правого поддерева
	iterator replace_with_max_left(iterator p)
	{
		Node* p_old = p._p();
		Node* x = p_old->left;
		while (x->right != nullptr)
			x = x->right;

		T oldData = p_old->data;
		p_old->data = x->data;
		x->data = oldData;

		p_old = x;
		p = (iterator)x;
		return p;
	}


public:
	//  Удаление элемента, заданного итератором. Возвращает количество удалённых элементов (для set - 0/1)
	iterator erase(iterator p) {
			Node* p_old = p._p();

			if (p_old == dummy->parent) //Если удаляемый элемент корень - 3 случая
			{
				if (dummy->parent->left->isdummy && dummy->parent->right->isdummy) // 0 потомков
				{
					dummy->left = dummy->right->parent = nullptr;
					delete_Node(p_old);
					--tree_size;
				}
				else if (dummy->parent->left->isdummy) // 1 правый потомок
				{
					dummy->parent = dummy->parent->right;
					dummy->left = dummy->parent;
					dummy->parent->parent = nullptr;
					delete_Node(p_old);
					--tree_size;
				}
				else if (dummy->parent->right->isdummy) // 1 левый потомк
				{
					dummy->parent = dummy->parent->left;
					dummy->right = dummy->parent;
					dummy->parent->parent = nullptr;
					delete_Node(p_old);
					--tree_size;
				}
				else // 2 потомка
				{
					Node* x = dummy->parent->left;
					while (x->right != nullptr)
						x = x->right;

					dummy->parent->left = x->left;
					dummy->parent->right = x->right;
					dummy->parent->parent = x->parent;
					dummy->parent = x;
					x->left = p_old->left;
					x->right = p_old->right;
					x->parent = nullptr;
					erase(p_old->data);
				}
			}
			else
			{
				if ((p_old->left == nullptr || p_old->left->isdummy)
					&& (p_old->right == nullptr || p_old->right->isdummy)) //Ситуация №1 Если узел - лист и у него нет потомков
				{
					delete_leaf(p);
				}
				else if (p_old->left == nullptr || p_old->left == dummy->left || p_old->right == nullptr || p_old->right == dummy->right)  //Ситуация №2  Если у узла один потомок
				{
					if (p.parent()._p()->right == p_old) // Если удаляемый узел правый у родителя
					{
						if (p_old->right == nullptr || p_old->right->isdummy) //Если его потомок слева(не справа) Рисунок 2,3
						{
							p.parent()._p()->left = p._p()->right;
							p._p()->left->parent = p.parent()._p();
							if (p_old->right == dummy->right)
							{
								Node* x = p_old->left;
								while (x->right != nullptr)
									x = x->right;
								dummy->right = x;
								x->right = dummy->right;
							}
						}
						else //Если его потомок справа Рисунок 1
						{
							p.parent()._p()->right = p._p()->right;
							p._p()->right->parent = p.parent()._p();
						}
					}
					else // Если удаляемый узел левый у родителя
					{
						if (p_old->left == nullptr || p_old->left->isdummy) //Если его потомок справа Рисунок 2,3
						{
							p.parent()._p()->right = p._p()->left;
							p._p()->right->parent = p.parent()._p();
							if (p_old->left == dummy->left)
							{
								Node* x = p_old->right;
								while (x->left != nullptr)
									x = x->left;
								dummy->left = x;
								x->left = dummy->left;
							}
						}
						else //Если его потомок слева Рисунок 1
						{
							p.parent()._p()->left = p._p()->left;
							p._p()->left->parent = p.parent()._p();
						}
					}
					tree_size--;
					delete_Node(p_old);
				}
				else //Ситуация №3 Если у узла два потомка
				{
					p = replace_with_max_left(p);
					erase(find(p._p()->data));
				}
			}
		return p;
	}

	size_type erase(T elem) {
		iterator it = findEqual(elem);
		if (it.isdummy())
			return 0;
		erase(it);
		return 1;
	}

	
	iterator erase(const_iterator first, const_iterator last) {
		while (first != last)
			first = erase(first);
		return last;
	}

	//Если передавать по ссылкам,все хорошо. Конструктор копий принескольких деревьях ломается.
	friend bool operator== (const Binary_Search_Tree<T>& tree_1, const Binary_Search_Tree<T>& tree_2)
	{
		auto i = tree_1.begin(), ii = tree_2.begin();
		for (; (i != tree_1.end()) && (ii != tree_2.end()); ++i, ++ii)
		{
			if (*i != *ii)
				return false;
		}
		return i == tree_1.end() && ii == tree_2.end();
	}

	//  Очистка дерева (без удаления фиктивной вершины)
	void clear() {
		Free_Nodes(dummy->parent);
		tree_size = 0;
		dummy->parent = dummy->left = dummy->right = dummy;
	}

private:
	//  Рекурсивное удаление узлов дерева, не включая фиктивную вершину
	void Free_Nodes(Node* Node)
	{
		if (Node != dummy)
		{
			Free_Nodes(Node->left);
			Free_Nodes(Node->right);
			delete_Node(Node);
		}
	}

public:
	~Binary_Search_Tree()
	{
		clear(); // рекурсивный деструктор
		delete_dummy(dummy);
	}

	////////////////////////////////////Прошлая лабораторная, переделенная под новый шаблон////////////////////////////////////
	//---------------Задание 1---------------
	void addLeaf(const T& data)
	{
		insert(data);
	}

	//---------------Задание 2---------------
	Node* findRecur(const T& t) const
	{
		return findRecur(t, dummy->parent);
	}

	Node* findRecur(const T& t, Node* r) const
	{
		if (r == nullptr || r->isdummy)
			return nullptr;
		if (r->data == t)
			return r;
		auto x = findRecur(t, r->left);
		if (x == nullptr)
			x = findRecur(t, r->right);
		return x;
	}
	
	//---------------Задание 3---------------
	Node* min() const
	{
		return dummy->left;
	}

	Node* max() const
	{
		return dummy->right;
	}

	//---------------Задание 4---------------
	iterator findGraterOrEqual(T data)
	{
		auto p = iterator(dummy->left);
		while (p._p() != nullptr && !p._p()->isdummy)
		{
			if (*p >= data)
				return p;
			else ++p;
		}
		return end();
	}

	iterator findEqual(T data)
	{
		iterator p = iterator(dummy->parent);
		while (p._p() != nullptr && !p._p()->isdummy)
		{
			if (*p == data)
				return p;
			if (*p > data) --p;
			else ++p;

		}
		return end();
	}

	iterator findLessOrEqual(T data)
	{
		auto p = iterator(dummy->right);
		while (p._p() != nullptr && !p._p()->isdummy)
		{
			if (*p <= data)
				return p;
			else --p;
		}
		return end();
	}


	iterator find(T data)
	{
		auto p = iterator(dummy->left);
		while (p._p() != nullptr && !p._p()->isdummy)
		{
			if (*p == data)
				return p;
			else p++;
		}
		return end();
	}


	//---------------Задание 5---------------
	void deleteLeaf(const T& data)
	{
		erase(data);
	}

	//---------------Задание 6---------------
	void printInfixRecur() const
	{
		print(dummy->parent);
		cout << endl;
	}

	void print(const Node* r) const
	{
		if (r == nullptr || r->isdummy)
			return;
		print(r->left);
		cout << r->data << " ";
		print(r->right);
	}

	//---------------Задание 7---------------
	void printPostfixStack()
	{
		stack<T> values;
		for (iterator i = begin(); i != end(); ++i) //Прохожусь с помощью итераторов в порядке, закидываю в стэк, получаю в обратном порядке
			values.push(*i);
		while (!values.empty())
		{
			cout << values.top() << " ";
			values.pop();
		}
		cout << endl;
	}

	//---------------Задание 8---------------
	void printLayersQueue() const
	{
		auto p = iterator(dummy->parent);
		queue<iterator> values;
		values.push(p);
		while (!values.empty())
		{
			queue<iterator> new_values;
			while (!values.empty())
			{
				iterator x = values.front();
				values.pop();
				cout << *x << " ";
				if (x.left()._p() != nullptr) //Если левый не пустой и не левый конец
				{
					if (!x.left()._p()->isdummy)
						new_values.push(iterator(x.left()));
				}
				if (x.right()._p() != nullptr) //Если левый не пустой и не правый конец
				{
					if (!x.right()._p()->isdummy)
						new_values.push(iterator(x.right()));
				}
			}
			cout << endl;
			values = new_values; // Присваиваю значения нового слоя
		}
	}

	//---------------Задание 9---------------

	friend bool equalRecur(Node* node1, Node* node2)
	{
		if (node1->isdummy && node2->isdummy)
			return true;
		if (node1->data == node2->data)
		{
			bool leftSide = false; 
			if (node1->left != nullptr && node2->left != nullptr)
			{
				leftSide = equalRecur(node1->left, node2->left);
			}
			bool rightSide = false;
			if (node1->right != nullptr && node2->right != nullptr)
			{
				rightSide = equalRecur(node1->right, node2->right);
			}
			return leftSide && rightSide;
		}
		else return false;
	}

	//---------------Задание 10---------------
	iterator findNextForIterator(iterator it)
	{
		return it++;
	}
};


template <class Key, class Compare, class Allocator>
void swap(Binary_Search_Tree<Key, Compare, Allocator>& x, Binary_Search_Tree<Key, Compare, Allocator>& y) noexcept(noexcept(x.swap(y))) {
	x.swap(y);
};


/*template <class Key, class Compare, class Allocator>
bool operator==(const Binary_Search_Tree<Key, Compare, Allocator>& x, const Binary_Search_Tree<Key, Compare, Allocator>& y) {
	typename Binary_Search_Tree<Key, Compare, Allocator>::const_iterator it1{ x.begin() }, it2{ y.begin() };
	while (it1 != x.end() && it2 != y.end() && *it1 == *it2) {
		++it1; ++it2;
	}

	return it1 == x.end() && it2 == y.end();
}
*/
template <class Key, class Compare, class Allocator>
bool operator<(const Binary_Search_Tree<Key, Compare, Allocator>& x, const Binary_Search_Tree<Key, Compare, Allocator>& y) {

	typename Binary_Search_Tree<Key, Compare, Allocator>::const_iterator it1{ x.begin() }, it2{ y.begin() };
	while (it1 != x.end() && it2 != y.end() && *it1 == *it2) {
		++it1; ++it2;
	}

	if (it1 == x.end())
		return it2 != y.end();

	return it2 != y.end() && *it1 < *it2;
}

template <class Key, class Compare, class Allocator>
bool operator!=(const Binary_Search_Tree<Key, Compare, Allocator>& x, const Binary_Search_Tree<Key, Compare, Allocator>& y) {
	return !(x == y);
}

template <class Key, class Compare, class Allocator>
bool operator>(const Binary_Search_Tree<Key, Compare, Allocator>& x, const Binary_Search_Tree<Key, Compare, Allocator>& y) {
	return y < x;
}

template <class Key, class Compare, class Allocator>
bool operator>=(const Binary_Search_Tree<Key, Compare, Allocator>& x, const Binary_Search_Tree<Key, Compare, Allocator>& y) {
	return !(x < y);
}

template <class Key, class Compare, class Allocator>
bool operator<=(const Binary_Search_Tree<Key, Compare, Allocator>& x, const Binary_Search_Tree<Key, Compare, Allocator>& y) {
	return   !(y < x);
}