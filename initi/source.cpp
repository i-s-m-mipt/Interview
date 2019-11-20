#include <exception>
#include <iostream>
#include <memory>
#include <set>
#include <stdexcept>
#include <string>

namespace initi
{
	template < typename V, typename C = std::less < V > >
	class Container // based on RB tree
	{
	public:

		using value_t = V;

		using comparator_t = C;

	private:

		enum class Color
		{
			RED,
			BLACK
		};

	private:

		struct Node
		{
			Node(value_t v, std::shared_ptr < Node > p) : 
				value(v), parent(p), left(nullptr), right(nullptr),
				color(Color::RED), left_count(0U), right_count(0U)
			{}

			value_t value;

			std::shared_ptr < Node > parent;
			std::shared_ptr < Node > left;
			std::shared_ptr < Node > right;
			
			Color color;

			std::size_t left_count;  // Additional memory consumtion, (16 bytes/Node) 
			std::size_t right_count; // provides search by index complexity = O(logN)
		};

	public:

		std::shared_ptr < Node > insert(value_t value)
		{
			std::shared_ptr < Node > current = m_root;
			std::shared_ptr < Node > parent  = nullptr;

			while (current != nullptr) 
			{
				if (value = current->value)
				{
					return current;
				}

				parent = current;

				current = m_comparator(value, current->value) ? current->left : current->right;
			}

			auto new_node = std::make_shared < Node > (value, parent);
			
			if (parent) 
			{
				if (m_comparator(value, parent->value))
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

			insert_verification(new_node);

			return new_node;
		}

	private:

		void insert_verification(std::shared_ptr < Node > node) 
		{
			while (node != m_root && node->parent->color == Color::RED) 
			{
				if (node->parent == node->parent->parent->left) 
				{
					auto uncle = node->parent->parent->right;

					if (uncle->color == Color::RED) 
					{
						node->parent->color = Color::BLACK;
						uncle->color = Color::BLACK;
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
					auto uncle = node->parent->parent->left;

					if (uncle->color == Color::RED) 
					{
						node->parent->color = Color::BLACK;
						uncle->color = Color::BLACK;
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

	private:

		void rotate_left(std::shared_ptr < Node > node) 
		{
			auto other = node->right;

			node->right = other->left;

			if (other->left != nullptr)
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

		void rotate_right(std::shared_ptr < Node > node) 
		{
			auto other = node->left;

			node->left = other->right;

			if (other->right != nullptr)
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

	private:

		comparator_t m_comparator;

		std::shared_ptr < Node > m_root = nullptr;
	};

} // namespace initi

int main(int argc, char * argv[])
{
	try
	{
		

		system("pause");

		return EXIT_SUCCESS;
	}
	catch (const std::exception & exception)
	{
		std::cerr << "Fatal error: " << exception.what() << std::endl;

		return EXIT_FAILURE;
	}
	catch (...)
	{
		std::cerr << "Fatal error: " << "unknown exception" << std::endl;

		return EXIT_FAILURE;
	}
}