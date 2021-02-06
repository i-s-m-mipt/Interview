#include <chrono>
#include <exception>
#include <fstream>
#include <iostream>
#include <memory>
#include <set>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace initi // text
{
	class initi_exception : public std::exception // for futher development
	{
	public:

		explicit initi_exception(const std::string & message) noexcept :
			std::exception(message.c_str())
		{}

		explicit initi_exception(const char * const message) noexcept :
			std::exception(message)
		{}

		~initi_exception() noexcept = default;
	};

	// =========================================================================

	template < typename T, typename Compare = std::less < T > >
	class Container
	{
	public:

		using key_type = T;
		using key_type = T;
		using key_compare = Compare;
		using reference = key_type & ;
		using const_reference = const key_type & ;

	private:

		enum class Color
		{
			RED,
			BLACK
		};

	private:

		struct Node
		{
			Node(key_type v, std::shared_ptr < Node > p, Color c = Color::RED) : 
				key(v), parent(p), color(c)
			{}

			~Node() noexcept = default;

			const auto is_leaf() const noexcept 
			{
				return (!left && !right);
			}

			key_type key;

			std::shared_ptr < Node > parent; // no weak ptr

			std::shared_ptr < Node > left  = nullptr;
			std::shared_ptr < Node > right = nullptr;
			
			Color color;

			std::size_t left_counter  = 0U; // Additional memory consumtion, (16 bytes/Node) 
			std::size_t right_counter = 0U; // provides search by index complexity = O(logN)
		};

	public:

		explicit Container(const key_compare & comparator = key_compare()) :
			m_comparator(comparator)
		{}

		~Container() noexcept
		{
			uninitialize(m_root);
		}

	private:

		void uninitialize(std::shared_ptr < Node > root);

	public:

		const auto size() const noexcept
		{
			return (m_root->left_counter + m_root->right_counter + !m_root->is_leaf());
		}

		reference at(std::size_t index)
		{
			if (index >= size())
			{
				throw std::out_of_range("index is out of range");
			}

			return search(index)->key;
		}

		const_reference at(std::size_t index) const
		{
			if (index >= size())
			{
				throw std::out_of_range("index is out of range");
			}

			return search(index)->key;
		}

		reference operator[] (std::size_t index)
		{
			return search(index)->key;
		}

		const_reference operator[] (std::size_t index) const
		{
			return search(index)->key;
		}

	private:

		std::shared_ptr < Node > search(std::size_t index) const
		{
			return search(index, m_root);
		}

		std::shared_ptr < Node > search(std::size_t index, std::shared_ptr < Node > root) const;

	public:

		std::shared_ptr < Node > insert(key_type key);

		void erase(std::size_t index)
		{
			if (index >= size())
			{
				return;
			}

			erase_implementation(search(index, m_root));
		}

	private:

		std::shared_ptr < Node > make_leaf(std::shared_ptr < Node > parent) const
		{
			return std::make_shared < Node > (key_type(), parent, Color::BLACK);
		}

		void increment_counters(std::shared_ptr < Node > node) const;

		void decrement_counters(std::shared_ptr < Node > node) const;

		void insert_verification(std::shared_ptr < Node > node);

		void erase_implementation(std::shared_ptr < Node > node);

		void erase_verification(std::shared_ptr < Node > node);

	private:

		void rotate_left(std::shared_ptr < Node > node);

		void rotate_right(std::shared_ptr < Node > node);

	public:

		bool is_valid() const 
		{
			return ((!m_root) || (m_root->color == Color::BLACK && 0 < is_valid(m_root)));
		}

	private:

		int is_valid(std::shared_ptr < Node > root) const;

	private:

		std::size_t m_size = 0U;

		key_compare m_comparator;

		std::shared_ptr < Node > m_root = make_leaf(nullptr);
	};

	template < typename T, typename Compare >
	void Container < T, Compare > ::uninitialize(std::shared_ptr < Node > root)
	{
		if (root->left)
		{
			uninitialize(root->left);
		}

		if (root->right)
		{
			uninitialize(root->right);
		}

		root->parent = nullptr;
	}

	template < typename T, typename Compare >
	std::shared_ptr < typename Container < T, Compare > ::Node > 
		Container < T, Compare > ::search(std::size_t index, std::shared_ptr < Node > root) const
	{
		if (root->left_counter == index)
		{
			return root;
		}

		if (root->left_counter > index)
		{
			return search(index, root->left);
		}
		else
		{
			return search(index - root->left_counter - 1, root->right);
		}
	}

	template < typename T, typename Compare >
	std::shared_ptr < typename Container < T, Compare > ::Node > 
		Container < T, Compare > ::insert(key_type key)
	{
		std::shared_ptr < Node > current = m_root;
		std::shared_ptr < Node > parent = nullptr;

		while (!current->is_leaf())
		{
			if (key == current->key)
			{
				return current;
			}

			parent = current;

			if (m_comparator(key, current->key))
			{
				current = current->left;
			}
			else
			{
				current = current->right;
			}
		}

		auto new_node = std::make_shared < Node >(key, parent);

		new_node->left = make_leaf(new_node);
		new_node->right = make_leaf(new_node);

		if (parent)
		{
			if (m_comparator(key, parent->key))
			{
				parent->left = new_node;
			}
			else
			{
				parent->right = new_node;
			}
		}
		else
		{
			m_root = new_node;
		}

		increment_counters(new_node);

		insert_verification(new_node);

		return new_node;
	}

	template < typename T, typename Compare >
	void Container < T, Compare > ::increment_counters(std::shared_ptr < Node > node) const
	{
		if (node == m_root)
		{
			return;
		}
		else
		{
			if (node == node->parent->left)
			{
				++node->parent->left_counter;
			}
			else
			{
				++node->parent->right_counter;
			}

			increment_counters(node->parent);
		}
	}

	template < typename T, typename Compare >
	void Container < T, Compare > ::decrement_counters(std::shared_ptr < Node > node) const
	{
		if (node == m_root)
		{
			return;
		}
		else
		{
			if (node == node->parent->left)
			{
				--node->parent->left_counter;
			}
			else
			{
				--node->parent->right_counter;
			}

			decrement_counters(node->parent);
		}
	}

	template < typename T, typename Compare >
	void Container < T, Compare > ::insert_verification(std::shared_ptr < Node > node)
	{
		while (node != m_root && node->parent->color == Color::RED)
		{
			if (node->parent == node->parent->parent->left)
			{
				if (node->parent->parent->right->color == Color::RED)
				{
					node->parent->color = Color::BLACK;
					node->parent->parent->right->color = Color::BLACK;

					node->parent->parent->color = Color::RED;

					node = node->parent->parent;
				}
				else
				{
					if (node == node->parent->right)
					{
						node = node->parent;
						rotate_left(node);
					}

					node->parent->color = Color::BLACK;
					node->parent->parent->color = Color::RED;

					rotate_right(node->parent->parent);
				}
			}
			else
			{
				if (node->parent->parent->left->color == Color::RED)
				{
					node->parent->color = Color::BLACK;
					node->parent->parent->left->color = Color::BLACK;

					node->parent->parent->color = Color::RED;

					node = node->parent->parent;
				}
				else
				{
					if (node == node->parent->left)
					{
						node = node->parent;
						rotate_right(node);
					}

					node->parent->color = Color::BLACK;
					node->parent->parent->color = Color::RED;

					rotate_left(node->parent->parent);
				}
			}
		}

		m_root->color = Color::BLACK;
	}

	template < typename T, typename Compare >
	void Container < T, Compare > ::erase_implementation(std::shared_ptr < Node > node)
	{
		std::shared_ptr < Node > parent;
		std::shared_ptr < Node > child;

		if (node->left->is_leaf() || node->right->is_leaf())
		{
			parent = node;
		}
		else
		{
			parent = node->right;

			while (!parent->left->is_leaf())
			{
				parent = parent->left;
			}
		}

		child = (!parent->left->is_leaf() ? parent->left : parent->right);

		child->parent = parent->parent;

		if (parent->parent)
		{
			if (parent == parent->parent->left)
			{
				parent->parent->left = child;
			}
			else
			{
				parent->parent->right = child;
			}

			decrement_counters(child);
		}
		else
		{
			m_root = child;
		}

		if (parent != node)
		{
			node->key = parent->key;
		}

		if (parent->color == Color::BLACK)
		{
			erase_verification(child);
		}
	}

	template < typename T, typename Compare >
	void Container < T, Compare > ::erase_verification(std::shared_ptr < Node > node)
	{
		while (node != m_root && node->color == Color::BLACK)
		{
			if (node == node->parent->left)
			{
				auto brother = node->parent->right; // remove for higher performance

				if (brother->color == Color::RED)
				{
					brother->color = Color::BLACK;
					node->parent->color = Color::RED;

					rotate_left(node->parent);

					brother = node->parent->right;
				}
				if (brother->left->color == Color::BLACK &&
					brother->right->color == Color::BLACK)
				{
					brother->color = Color::RED;

					node = node->parent;
				}
				else
				{
					if (brother->right->color == Color::BLACK)
					{
						brother->left->color = Color::BLACK;
						brother->color = Color::RED;

						rotate_right(brother);

						brother = node->parent->right;
					}

					brother->color = node->parent->color;

					node->parent->color = Color::BLACK;
					brother->right->color = Color::BLACK;

					rotate_left(node->parent);

					node = m_root;
				}
			}
			else
			{
				auto brother = node->parent->left; // remove for higher performance

				if (brother->color == Color::RED)
				{
					brother->color = Color::BLACK;
					node->parent->color = Color::RED;

					rotate_right(node->parent);

					brother = node->parent->left;
				}
				if (brother->right->color == Color::BLACK &&
					brother->left->color == Color::BLACK)
				{
					brother->color = Color::RED;

					node = node->parent;
				}
				else
				{
					if (brother->left->color == Color::BLACK)
					{
						brother->right->color = Color::BLACK;
						brother->color = Color::RED;

						rotate_left(brother);

						brother = node->parent->left;
					}

					brother->color = node->parent->color;

					node->parent->color = Color::BLACK;
					brother->left->color = Color::BLACK;

					rotate_right(node->parent);

					node = m_root;
				}
			}
		}

		node->color = Color::BLACK;
	}

	template < typename T, typename Compare >
	void Container < T, Compare > ::rotate_left(std::shared_ptr < Node > node)
	{
		auto other = node->right;

		node->right_counter = other->left_counter;
		other->left_counter = node->right_counter + 1 + node->left_counter;

		node->right = other->left;
		node->right->parent = node;

		if (!other->left->is_leaf())
		{
			other->left->parent = node;
		}

		other->parent = node->parent;

		if (node->parent)
		{
			if (node == node->parent->left)
			{
				node->parent->left = other;
			}
			else
			{
				node->parent->right = other;
			}
		}
		else
		{
			m_root = other;
		}

		other->left = node;

		node->parent = other;
	}

	template < typename T, typename Compare >
	void Container < T, Compare > ::rotate_right(std::shared_ptr < Node > node)
	{
		auto other = node->left;

		node->left_counter = other->right_counter;
		other->right_counter = node->left_counter + 1 + node->right_counter;

		node->left = other->right;
		node->left->parent = node;

		if (!other->right->is_leaf())
		{
			other->right->parent = node;
		}

		other->parent = node->parent;

		if (node->parent)
		{
			if (node == node->parent->right)
			{
				node->parent->right = other;
			}
			else
			{
				node->parent->left = other;
			}
		}
		else
		{
			m_root = other;
		}

		other->right = node;

		node->parent = other;
	}

	template < typename T, typename Compare >
	int Container < T, Compare > ::is_valid(std::shared_ptr < Node > root) const
	{
		int is_black = 1;

		int left_black_height = 0;
		int right_black_height = 0;

		if (!root)
		{
			return 1;
		}

		if (root->color == Color::RED)
		{
			if ((root->left  && root->left->color == Color::RED) ||
				(root->right && root->right->color == Color::RED))
			{
				return -1;
			}

			is_black = 0;
		}

		if ((left_black_height = is_valid(root->left)) < 0)
		{
			return -1;
		}

		if ((right_black_height = is_valid(root->right)) < 0)
		{
			return -1;
		}

		if (left_black_height != right_black_height)
		{
			return -1;
		}

		return is_black + left_black_height;
	}

} // namespace initi

// =============================================================================

using namespace std;
using namespace chrono;

using write_sequence = vector<string>;

using test_pair = pair<uint64_t, string>;
using modify_sequence = vector<test_pair>;
using read_sequence = vector<test_pair>;

ifstream& operator >> (ifstream& _is, test_pair& _key)
{
	_is >> _key.first;
	_is >> _key.second;

	return _is;
}

template <typename S>
S get_sequence(const string& _file_name)
{
	ifstream infile(_file_name);
	S result;

	typename S::value_type item;

	while (infile >> item)
	{
		result.emplace_back(move(item));
	}

	return result;
}

class storage
{
public:
	void insert(const string& _str)
	{
		c.insert(_str); // DONE
	}
	void erase(uint64_t _index)
	{
		c.erase(_index); // DONE
	}
	const string& get(uint64_t _index)
	{
		return c[_index]; // DONE, also has c.at(_index) with check
	}

	initi::Container < std::string > c; // DONE
};

int main()
{
	write_sequence write = get_sequence<write_sequence>("write.txt");
	modify_sequence modify = get_sequence<modify_sequence>("modify.txt");
	read_sequence read = get_sequence<read_sequence>("read.txt");

	storage st;

	std::cout << "insertion ... " << std::endl;

	for (const string& item : write)
	{
		st.insert(item);
	}
	
	std::cout << "insertion completed" << std::endl;

	uint64_t progress = 0;
	uint64_t percent = modify.size() / 100;

	time_point<system_clock> time;
	nanoseconds total_time(0);

	modify_sequence::const_iterator mitr = modify.begin();
	read_sequence::const_iterator ritr = read.begin();

	for (; mitr != modify.end() && ritr != read.end(); ++mitr, ++ritr)
	{
		time = system_clock::now();
		st.erase(mitr->first);
		st.insert(mitr->second);
		const string& str = st.get(ritr->first);
		total_time += system_clock::now() - time;

		if (ritr->second != str)
		{
			cout << "test failed" << endl;
			return 1;
		}

		if (++progress % (5 * percent) == 0)
		{
			cout << "time: " << duration_cast<milliseconds>(total_time).count()
				<< "ms progress: " << progress << " / " << modify.size() << "\n";
		}
	}

	system("pause");

	return 0;
}
