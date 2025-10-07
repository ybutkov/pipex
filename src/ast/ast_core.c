/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast_core.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ybutkov <ybutkov@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/06 17:29:26 by ybutkov           #+#    #+#             */
/*   Updated: 2025/10/06 18:12:32 by ybutkov          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ast.h"
#include <stdlib.h>

static void free_ast(t_ast *ast)
{
	if (!ast)
		return ;
	// Освободить узлы и их содержимое здесь, если необходимо
	free(ast);
}

static void	set_root(t_ast *ast, t_ast_node *root)
{
	if (!ast)
		return ;
	ast->root = root;
}

static t_ast_node	*get_root(t_ast *ast)
{
	if (!ast)
		return (NULL);
	return (ast->root);
}

t_ast	*create_ast_tree(void)
{
	t_ast	*ast;

	ast = malloc(sizeof(t_ast));
	if (!ast)
		return (NULL);
	ast->root = NULL;
	ast->free = free_ast;
	ast->set_root = set_root;
	ast->get_root = get_root;
	return (ast);
}
