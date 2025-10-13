/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shell_build.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ybutkov <ybutkov@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/13 16:47:24 by ybutkov           #+#    #+#             */
/*   Updated: 2025/10/13 17:57:56 by ybutkov          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell_internal.h"

static void	handle_heredoc(t_shell *shell, char **commands, int *i,
		t_ast_node *curr_node)
{
	t_redir			*redirect;
	t_shell_node	*node;
	t_ast_node		*ast_node;
	t_cmd			*cmd;

	redirect = create_redir(REDIR_HEREDOC, commands[*i]);
	node = create_shell_node(NODE_REDIR_HEREDOC, redirect);
	ast_node = create_ast_node(node);
	curr_node->set_left(curr_node, ast_node);
	(*i)++;
	cmd = create_cmd_from_raw_str(commands[*i], shell);
	node = create_shell_node(NODE_CMD, cmd);
	ast_node->set_left(ast_node, create_ast_node(node));
	(*i)++;
}

void	put_shell_node_to_ast(t_ast_node **curr_node, t_shell_node *node, int i,
		int argc)
{
	t_ast_node		*ast_node;
	t_shell_node	*pipe_node;
	t_ast_node		*pipe_ast;

	ast_node = create_ast_node(node);
	if (i == 1)
		(*curr_node)->set_left((*curr_node), ast_node);
	else if (i == argc - 2)
		(*curr_node)->set_right((*curr_node), ast_node);
	else
	{
		pipe_node = create_shell_node(NODE_PIPE, NULL);
		pipe_ast = create_ast_node(pipe_node);
		(*curr_node)->set_right((*curr_node), pipe_ast);
		pipe_ast->set_left(pipe_ast, ast_node);
		*curr_node = pipe_ast;
	}
}

static void	add_output_redirect(t_shell_node *node, char **commands, int argc)
{
	t_redir	*redirect;
	t_list	*redir;

	if (!ft_strcmp(HERE_DOC, commands[0]))
		redirect = create_redir(REDIR_APPEND, commands[argc - 1]);
	else
		redirect = create_redir(REDIR_OUT, commands[argc - 1]);
	redir = ft_lstnew(redirect);
	if (node->data.cmd->redirs)
		ft_lstadd_back(&node->data.cmd->redirs, redir);
	else
		node->data.cmd->redirs = redir;
}

static t_ast_node	*init_ast_tree(t_shell *shell)
{
	t_shell_node	*node;
	t_ast_node		*ast_node;

	node = create_shell_node(NODE_PIPE, NULL);
	ast_node = create_ast_node(node);
	shell->ast->set_root(shell->ast, ast_node);
	return (shell->ast->get_root(shell->ast));
}

void	build_shell(t_shell *shell, char **commands, int argc)
{
	t_shell_node	*node;
	t_ast_node		*curr_node;
	t_cmd			*cmd;
	t_redir			*redirect;
	int				i;

	curr_node = init_ast_tree(shell);
	i = 1;
	if (!ft_strcmp(HERE_DOC, commands[0]))
		handle_heredoc(shell, commands, &i, curr_node);
	else
		redirect = create_redir(REDIR_IN, commands[0]);
	while (i < argc - 1)
	{
		cmd = create_cmd_from_raw_str(commands[i], shell);
		if (!cmd)
			continue ;
		node = create_shell_node(NODE_CMD, cmd);
		if (i == 1)
			node->data.cmd->redirs = ft_lstnew(redirect);
		if (i == argc - 2)
			add_output_redirect(node, commands, argc);
		put_shell_node_to_ast(&curr_node, node, i, argc);
		i++;
	}
}
