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

		std::shared_ptr < Node > search(std::size_t index)
		{
			return search(index, m_root);
		}

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

				if (m_comparator(value, current->value))
				{
					current = current->left;
					++(parent->left_count);
				}
				else
				{
					current = current->right;
					++(parent->right_count);
				}
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

		void remove(std::size_t index)
		{
			auto node = search(index, m_root);

			if (!result)
			{
				throw std::runtime_error("cannot remove node with index " + std::to_string(index));
			}

			remove(node);
		}

		void remove(std::shared_ptr < Node > node) 
		{
			std::shared_ptr < Node > parent;
			std::shared_ptr < Node > child;

			if (!node)
			{
				return;
			}
			
			if (!node->left || !node->right) 
			{
				parent = node;
			}
			else 
			{
				parent = node->right;

				while (!parent->left)
				{
					parent = parent->left;
				}
			}

			if (parent->left)
			{
				child = parent->left;
			}
			else
			{
				child = parent->right;
			}
				
			if (child)
			{
				child->parent = parent->parent;
			}
			
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
			}
			else
			{
				m_root = child;
			}

			if (parent != node)
			{
				node->value = parent->value;
			}

			if (parent->color == Color::BLACK)
			{
				remove_verification(child);
			}
		}

	private:

		std::shared_ptr < Node > search(std::size_t index, std::shared_ptr < Node > root)
		{
			if (root->left_count == index)
			{
				return root;
			}

			if (root->left_count > index)
			{
				search(index, root->left);
			}
			else
			{
				if (root->right)
				{
					search(index - root->left_count - 1, root->right);
				}
				else
				{
					return nullptr;
				}
			}
		}

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

		void remove_verification(std::shared_ptr < Node > node) 
		{
			while (node != m_root && node->color == Color::BLACK) 
			{
				if (node == node->parent->left) 
				{
					auto brother = node->parent->right;

					if (brother->color == Color::RED) 
					{
						brother->color = Color::BLACK;
						node->parent->color = Color::RED;

						rotate_left(node->parent);
						brother = node->parent->right;
					}
					if (brother->left->color == Color::BLACK && brother->right->color == Color::BLACK) 
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
					auto brother = node->parent->left;

					if (brother->color == Color::RED) 
					{
						brother->color = Color::BLACK;
						node->parent->color = Color::RED;

						rotate_right(node->parent);
						brother = node->parent->left;
					}
					if (brother->right->color == Color::BLACK && brother->left->color == Color::BLACK) 
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

	private:

		void rotate_left(std::shared_ptr < Node > node) 
		{
			auto other = node->right;

			node->right_count = other->left_count;
			other->left_count = node->right_count + 1 + node->left_count;

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

			node->left_count = other->right_count;
			other->right_count = node->left_count + 1 + node->right_count;

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