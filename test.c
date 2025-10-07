#include "ast.h"
#include "pipex.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Helper function to convert integer to string
char	*int_to_str(int value)
{
	char *str = malloc(12); // enough for int range
	if (!str)
		return (NULL);
	sprintf(str, "%d", value);
	return (str);
}

// Helper function to create a node with string content from integer
t_ast_node	*create_string_node(int value)
{
	char		*str_content;
	t_ast_node	*node;

	str_content = int_to_str(value);
	if (!str_content)
		return (NULL);
	node = create_ast_node(str_content);
	if (!node)
	{
		free(str_content);
		return (NULL);
	}
	return (node);
}

// Helper function to insert a value into the binary search tree
t_ast_node	*insert_into_tree(t_ast_node *root, int value)
{
	char		*root_str;
	int			root_value;
	t_ast_node	*left_child;
	t_ast_node	*right_child;

	if (!root)
	{
		return (create_string_node(value));
	}
	root_str = (char *)root->get_content(root);
	root_value = atoi(root_str);
	if (value < root_value)
	{
		left_child = insert_into_tree(root->get_left(root), value);
		root->set_left(root, left_child);
	}
	else if (value > root_value)
	{
		right_child = insert_into_tree(root->get_right(root), value);
		root->set_right(root, right_child);
	}
	// If value == root_value, we don't insert duplicates
	return (root);
}

// Function to print the tree structure (in-order traversal)
void	print_tree_inorder(t_ast_node *node)
{
	char	*content;

	if (!node)
		return ;
	print_tree_inorder(node->get_left(node));
	content = (char *)node->get_content(node);
	printf("%s ", content ? content : "NULL");
	print_tree_inorder(node->get_right(node));
}

// Function to print the tree structure (with indentation)
void	print_tree_structure(t_ast_node *node, int depth, char *prefix)
{
	char	*content;

	if (!node)
		return ;
	// Print current node
	for (int i = 0; i < depth; i++)
	{
		printf("  ");
	}
	content = (char *)node->get_content(node);
	printf("%s%s\n", prefix, content ? content : "NULL");
	// Print children
	if (node->get_left(node) || node->get_right(node))
	{
		if (node->get_left(node))
		{
			print_tree_structure(node->get_left(node), depth + 1, "L: ");
		}
		else
		{
			for (int i = 0; i <= depth; i++)
				printf("  ");
			printf("L: (empty)\n");
		}
		if (node->get_right(node))
		{
			print_tree_structure(node->get_right(node), depth + 1, "R: ");
		}
		else
		{
			for (int i = 0; i <= depth; i++)
				printf("  ");
			printf("R: (empty)\n");
		}
	}
}

// Function to print the entire tree
void	print_ast_tree(t_ast *tree)
{
	if (!tree || !tree->get_root(tree))
	{
		printf("Empty AST tree\n");
		return ;
	}
	printf("\nTree Structure (Visual):\n");
	printf("========================\n");
	print_tree_structure(tree->get_root(tree), 0, "Root: ");
	printf("\nIn-order traversal (should be sorted):\n");
	printf("======================================\n");
	print_tree_inorder(tree->get_root(tree));
	printf("\n");
}

// Function to free a node and its string content
void	free_string_node(t_ast_node *node)
{
	char	*content;

	if (!node)
		return ;
	content = (char *)node->get_content(node);
	if (content)
		free(content);
	free_string_node(node->get_left(node));
	free_string_node(node->get_right(node));
	free(node);
}

// Test function to create a sorted binary tree
void	test_sorted_tree(void)
{
	t_ast		*tree;
	t_ast_node	*root;
	int			values[] = {3, 11, 1, 5, 9, 13, 2, 4, 6, 8, 10, 12, 14};
	int			num_values;

	printf("=== Test: Sorted Binary Tree with String Content ===\n");
	tree = create_ast_tree();
	if (!tree)
	{
		printf("Failed to create AST tree\n");
		return ;
	}
	// Start with root value 7
	root = create_string_node(7);
	tree->set_root(tree, root);
	// Insert values to create a balanced-ish tree
	num_values = sizeof(values) / sizeof(values[0]);
	printf("Inserting values: 7 (root), ");
	for (int i = 0; i < num_values; i++)
	{
		printf("%d", values[i]);
		if (i < num_values - 1)
			printf(", ");
		root = insert_into_tree(root, values[i]);
	}
	printf("\n");
	print_ast_tree(tree);
	// Cleanup
	free_string_node(tree->get_root(tree));
	tree->free(tree);
}

// Test function to create a simple tree
void	test_simple_tree(void)
{
	t_ast		*tree;
	t_ast_node	*root;
	t_ast_node	*left;
	t_ast_node	*right;

	printf("\n=== Test: Simple Tree (7, 3, 11) ===\n");
	tree = create_ast_tree();
	if (!tree)
	{
		printf("Failed to create AST tree\n");
		return ;
	}
	// Create a simple tree: 7 as root, 3 as left, 11 as right
	root = create_string_node(7);
	left = create_string_node(3);
	right = create_string_node(11);
	root->set_left(root, left);
	root->set_right(root, right);
	tree->set_root(tree, root);
	left = create_string_node(1);
	right = create_string_node(2);
	root->set_left(root->left, left);
	root->set_right(root->left, right);

	print_ast_tree(tree);
	// Cleanup
	free_string_node(tree->get_root(tree));
	tree->free(tree);
}

// Test function to show tree operations
void	test_tree_operations(void)
{
	t_ast		*tree;
	t_ast_node	*root;
	t_ast_node	*left_child;
	t_ast_node	*right_child;
	t_ast_node	*retrieved_left;
	t_ast_node	*retrieved_right;

	printf("\n=== Test: Tree Operations ===\n");
	tree = create_ast_tree();
	if (!tree)
	{
		printf("Failed to create AST tree\n");
		return ;
	}
	// Create root with value 7
	root = create_string_node(7);
	tree->set_root(tree, root);
	printf("Created tree with root: %s\n", (char *)root->get_content(root));
	// Add some children
	left_child = create_string_node(3);
	right_child = create_string_node(11);
	root->set_left(root, left_child);
	root->set_right(root, right_child);
	printf("Added left child: %s\n",
		(char *)left_child->get_content(left_child));
	printf("Added right child: %s\n",
		(char *)right_child->get_content(right_child));
	// Test getting children
	retrieved_left = root->get_left(root);
	retrieved_right = root->get_right(root);
	printf("\nRetrieved left child: %s\n",
		retrieved_left ? (char *)retrieved_left->get_content(retrieved_left) : "NULL");
	printf("Retrieved right child: %s\n",
		retrieved_right ? (char *)retrieved_right->get_content(retrieved_right) : "NULL");
	print_ast_tree(tree);
	// Cleanup
	free_string_node(tree->get_root(tree));
	tree->free(tree);
}

int	main(void)
{
	printf("AST Binary Search Tree Testing Suite\n");
	printf("====================================\n");

	// test_tree_operations();
	test_simple_tree();
	// test_sorted_tree();

	printf("\nAll tests completed!\n");
	return (0);
}
