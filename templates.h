#pragma once
#include <vector>
// template implementation of a swap function
template <class T>
void swap(T& a, T& b)
{
	T temp = a;
	a = b;
	b = temp;
}

// template implementation of bubble sort for a vector of pointers
template <class T> 
void sort(T** arr, int size)
{
	for (int i = 0; i < size - 1; i++)
	{
		for (int j = 0; j < size - 1 - i; j++)
		{
			if (*arr[j] < *arr[j + 1])
				swap(arr[j], arr[j + 1]);
		}
	}
}


// an implementation of an STL-like linked list
template <class T>
class List
{
private:
	class Node
	{
	public:
		T data;
		Node* next;
		Node() : data(T()), next(nullptr) {}
		Node(const T& other) : data(other), next(nullptr) {}
	};
	Node* _head, * _tail;
	int length;

public:
	List() : length(0), _head(nullptr), _tail(nullptr) {}
	List(const List& other) { *this = other; }
	~List() { clear(); }

	const List& operator=(const List& other)
	{
		if (this != &other)
		{
			if (length)
				clear();
			Node* temp = other._head;
			while (temp)
			{
				push_back(temp->data);
				temp = temp->next;
			}
			length = other.length;
		}
		return *this;
	}

	void push_front(const T& value)
	{
		Node* newNode = new Node(value);
		if (!length)
		{
			_head = _tail = newNode;
		}
		else
		{
			newNode->next = _head;
			_head = newNode;
		}
		length++;
	}

	void push_back(const T& value)
	{
		Node* newNode = new Node(value);
		if (!length)
		{
			_head = _tail = newNode;
		}
		else
		{
			_tail->next = newNode;
			_tail = newNode;
		}
		length++;
	}

	void pop_back()
	{
		if (!length)
			return;
		if (_head == _tail)
		{
			delete _head;
			_head = _tail = nullptr;
			length--;
			return;
		}

		Node* temp = _head;
		while (temp->next != _tail)
		{
			temp = temp->next;
		}
		delete temp->next;
		_tail = temp; _tail->next = nullptr;
		length--;
	}

	void pop_front()
	{
		if (!length)
			return;
		if (_head == _tail)
		{
			delete _head;
			_head = _tail = nullptr;
			length--;
			return;
		}

		Node* temp = _head;
		_head = _head->next;
		delete temp;
		length--;
	}

	const T& front() const { return _head->data; }
	const T& back()	 const { return _tail->data; }
	int      size()  const { return length; }
	bool     empty() const { return length == 0; }

	void     clear()
	{
		Node* temp = _head;
		while (temp)
		{
			_head = _head->next;
			delete temp;
			temp = _head;
		}
		_head = _tail = nullptr;
		length = 0;
	}

	// standard STL iterator implementation (no duplication for const)
	template <bool is_const>
	class base_iterator
	{
		friend class List;
	private:
		Node* itrNode;

	public:
		using iterator_category = std::forward_iterator_tag;
		using different_type = std::ptrdiff_t;
		using value_type = std::conditional_t<is_const, const T, T>;
		using pointer = value_type*;
		using reference = value_type&;

		base_iterator(Node* node) : itrNode(node) {}

		// we want to use the default copy constructor
		base_iterator(const base_iterator&) = default;

		// and the default assignment operator
		constexpr base_iterator& operator=(const base_iterator&) = default;

		// we want to allow construction of const_iterator from iterator
		friend class base_iterator<true>;
		template <bool _is_const = is_const, class = std::enable_if_t<_is_const>>
		base_iterator(const base_iterator<false>& other) : itrNode(other._node) {}

		// comparison with another iterator
		bool operator==(const base_iterator& other) const
		{
			return (itrNode == other.itrNode);
		}

		bool operator!=(const base_iterator& other) const
		{
			return (itrNode != other.itrNode);
		}

		reference operator*()
		{
			return itrNode->data;
		}

		// increment iterator methods
		base_iterator& operator++()
		{
			itrNode = itrNode->next;
			return *this;
		}

		base_iterator operator++(int)
		{
			base_iterator temp(*this);
			itrNode = itrNode->next;
			return temp;
		}
	};

	using iterator = base_iterator<false>;
	using const_iterator = base_iterator<true>;

	iterator insert(const iterator& pos, const T& val)
	{
		iterator itr = begin(), prevItr(nullptr);
		if (pos == itr)
		{
			push_front(val);
			return iterator(_head);
		}
		Node* newNode = new Node(val);
		while (itr != pos)
		{
			prevItr = itr;
			++itr;
		}
		prevItr.itrNode->next = newNode;
		newNode->next = itr.itrNode;
		prevItr.itrNode = newNode;
		length++;
		return prevItr;
	}

	// returns iterator to first element after deleted element/s
	iterator erase(const iterator& iter)
	{
		if (!length)
			return iterator(end());

		Node* temp = _head, * del = iter.itrNode;
		if (temp == del)
		{
			_head = _head->next;
			delete del;
			length--;
			return iterator(_head);
		}
		while (temp->next != iter.itrNode)
			temp = temp->next;

		temp->next = iter.itrNode->next;
		if (_tail == iter.itrNode)
			_tail = temp;
		delete iter.itrNode;
		length--;
		return iterator(temp->next);
	}

	iterator erase(const iterator& first, const iterator& last)
	{
		iterator rtrn(nullptr);

		if (first == last)
			rtrn = erase(first);
		else
		{
			iterator itr = first;
			while (itr != last)
				itr = erase(itr);
			rtrn = itr;
		}
		return rtrn;
	}

	iterator begin()
	{
		return iterator(_head);
	}

	iterator end()
	{
		return iterator(nullptr);
	}
	const_iterator begin() const
	{
		return const_iterator(_head);
	}

	const_iterator end() const
	{
		return const_iterator(nullptr);
	}
	const_iterator cbegin() const {
		return const_iterator(_head);
	}

	const_iterator cend() const
	{
		return const_iterator(nullptr);
	}
};