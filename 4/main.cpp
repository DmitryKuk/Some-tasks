#include <iostream>
#include <memory>
#include <vector>



template<class T>
class forward_list
{
public:
	forward_list(const std::vector<T> &v):
		size_{v.size()}
	{
		auto *node = &this->head_;
		for (const T &x: v) {
			*node = std::make_unique<forward_list<T>::node>(x);
			node = &(*node)->next;
		}
	}
	
	
	
	// Time: O(n), memory: O(1)
	void repack()
	{
		if (this->size_ == 0)
			return;
		
		
		// Split current list; time: O(n) [* 1/2], memory: O(1)
		size_t left_size = this->size_ / 2 + ((this->size_ % 2 == 0)? 0: 1);
		
		auto *node = &this->head_;
		for (size_t i = 0; i < left_size - 1; ++i)	// Skip half of list
			node = &(*node)->next;
		
		auto right = std::move((*node)->next);	// right points to the trailing part of list
		
		
		// Reverse right part; time: O(n) [* 1/2], memory: O(1)
		reverse(right);
		
		
		// Merge two parts; time: O(n), memory: O(1)
		node = &this->head_;
		auto left = std::move(this->head_);
		while (right != nullptr) {	// Because right list may be shorter
			*node = std::move(left);
			left = std::move((*node)->next);
			node = &(*node)->next;
			
			*node = std::move(right);
			right = std::move((*node)->next);
			node = &(*node)->next;
		}
		
		if (left != nullptr)	// Trailing left element
			*node = std::move(left);
	}
	
	
	
	// Time: O(n), memory: O(1)
	void print(std::ostream &stream = std::cout) const
	{
		print(this->head_, stream);
	}
private:
	struct node
	{
		std::unique_ptr<node> next = nullptr;
		T value;
		
		node(const T &value): value{value} {}
		node(T &&value): value{std::move(value)} {}
	};	// struct node
	
	
	std::unique_ptr<node> head_;
	size_t size_;
	
	
	
	// Time: O(n), memory: O(1)
	static void reverse(std::unique_ptr<forward_list<T>::node> &head)
	{
		std::unique_ptr<forward_list<T>::node> prev = nullptr;
		while (head != nullptr) {
			auto next = std::move(head->next);
			head->next = std::move(prev);
			prev = std::move(head);
			head = std::move(next);
		}
		head = std::move(prev);
	}
	
	
	
	// Time: O(n), memory: O(1)
	static void print(const std::unique_ptr<forward_list<T>::node> &head, std::ostream &stream = std::cout)
	{
		auto *node = &head;
		while (*node != nullptr) {
			stream << ' ' << (*node)->value;
			node = &(*node)->next;
		}
		stream << std::endl;
	}
};	// class list



int main()
{
	// Press Ctrl+D to stop input
	
	// Input data
	std::vector<int> v;
	{
		int x;
		while (std::cin >> x)
			v.push_back(x);
	}
	
	
	forward_list<int> list{v};
	
	list.print();
	list.repack();
	list.print();
	
	return 0;
}
