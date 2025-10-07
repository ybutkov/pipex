/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shell_core.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ybutkov <ybutkov@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/07 12:20:01 by ybutkov           #+#    #+#             */
/*   Updated: 2025/10/07 22:10:00 by ybutkov          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"
#include <sys/wait.h>
#include "libft.h"

static void	*free_shell(t_shell *shell)
{
	if (!shell)
		return (NULL);
	if (shell->ast)
		shell->ast->free(shell->ast);
	free(shell);
	return (NULL);
}

void init_test_commands()
{
	// This function can be used to initialize any test commands if needed
	// 	node = malloc(sizeof(t_shell_node));
	// node->type = NODE_PIPE;
	// node->data.cmd = NULL;
	// ast_node = create_ast_node(node);
	// shell->ast->set_root(shell->ast, ast_node);
	// root = shell->ast->get_root(shell->ast);

	// node = malloc(sizeof(t_shell_node));
	// node->type = NODE_CMD;
	// node->data.cmd = malloc(sizeof(t_cmd));
	// commands = malloc(3 * sizeof(char *));
	// commands[0] = ft_strdup("/usr/bin/cat");
	// commands[1] = ft_strdup("pipex.c");
	// commands[2] = NULL;
	// node->data.cmd->argv = commands;
	// node->data.cmd->path = commands[0];
	// ast_node = create_ast_node(node);
	// root->set_left(root, ast_node);


	// node = malloc(sizeof(t_shell_node));
	// node->type = NODE_CMD;
	// node->data.cmd = malloc(sizeof(t_cmd));
	// commands = malloc(3 * sizeof(char *));
	// commands[0] = ft_strdup("/usr/bin/wc");
	// commands[1] = ft_strdup("-l");
	// commands[2] = NULL;
	// node->data.cmd->argv = commands;
	// node->data.cmd->path = commands[0];
	// ast_node = create_ast_node(node);
	// root->set_right(root, ast_node);

}

#include <stdio.h>
static void	*build_shell(t_shell *shell, char **commands)
{
	t_shell_node	*node;
	t_ast_node		*ast_node;
	t_ast_node		*curr_node;
	char			*full_path;
	char			**cmd_argv;

	(void)shell;
	// (void)input;

	// init_test_commands();

	node = malloc(sizeof(t_shell_node));
	node->type = NODE_PIPE;
	node->data.cmd = NULL;
	ast_node = create_ast_node(node);
	shell->ast->set_root(shell->ast, ast_node);
	curr_node = shell->ast->get_root(shell->ast);

	// commands = ft_split(input, ' ');
	cmd_argv = ft_split(*commands, ' ');
	commands++;
	while (*commands)
	{
		node = malloc(sizeof(t_shell_node));
		node->type = NODE_CMD;
		node->data.cmd = malloc(sizeof(t_cmd));
		node->data.cmd->argv = cmd_argv;
		full_path = get_cmd_path(cmd_argv[0], shell->ctx->envp);
		node->data.cmd->path = full_path;
		// node->data.cmd->path = cmd_argv[0];
		ast_node = create_ast_node(node);
		curr_node->set_left(curr_node, ast_node);


		cmd_argv = ft_split(*commands, ' ');
		commands++;
		if (!*commands)
		{
			node = malloc(sizeof(t_shell_node));
			node->type = NODE_CMD;
			node->data.cmd = malloc(sizeof(t_cmd));
			node->data.cmd->argv = cmd_argv;
			full_path = get_cmd_path(cmd_argv[0], shell->ctx->envp);
			if (full_path)
				node->data.cmd->path = full_path;
			else
				exit(EXIT_FAILURE);
			ast_node = create_ast_node(node);
			curr_node->set_right(curr_node, ast_node);
			curr_node = curr_node->get_right(curr_node);
			break ;
		}
		node = malloc(sizeof(t_shell_node));
		node->type = NODE_PIPE;
		node->data.cmd = NULL;
		ast_node = create_ast_node(node);
		curr_node->set_right(curr_node, ast_node);
		curr_node = curr_node->get_right(curr_node);
	}
	return (NULL);
}

static void	execute_CMD(t_cmd *cmd, t_ctx *ctx, int input_fd, int output_fd)
{
	pid_t	pid;
	int		status;

	pid = fork();
	if (pid == -1)
	{
		perror("fork");
		return ;
	}
	if (pid == 0)
	{
		if (input_fd != STDIN_FILENO)
		{
			dup2(input_fd, STDIN_FILENO);
			close(input_fd);
		}
		if (output_fd != STDOUT_FILENO)
		{
			dup2(output_fd, STDOUT_FILENO);
			close(output_fd);
		}
		execve(cmd->path, cmd->argv, ctx->envp);
		exit(EXIT_FAILURE); // execve failed
	}
	waitpid(pid, &status, 0);
}

static void	execute_shell_node(t_ast_node *node, t_ctx *ctx, int input_fd,
		int output_fd)
{
	t_shell_node	*shell_node;
	int				pipe_fds[2];

	shell_node = (t_shell_node *)node->get_content(node);
	if (shell_node->type == NODE_PIPE)
	{
		pipe(pipe_fds);
		execute_shell_node(node->get_left(node), ctx, input_fd, pipe_fds[1]);
		close(pipe_fds[1]);
		execute_shell_node(node->get_right(node), ctx, pipe_fds[0], output_fd);
		close(pipe_fds[0]);
	}
	else if (shell_node->type == NODE_CMD)
	{
		execute_CMD(shell_node->data.cmd, ctx, input_fd, output_fd);
	}
	else
	{
	}
	// Handle other node types (e.g., redirections) as needed
}

static void	execute_shell(t_shell *shell)
{
	t_ast_node	*root;

	root = shell->ast->get_root(shell->ast);
	execute_shell_node(root, shell->ctx, STDIN_FILENO, STDOUT_FILENO);
}

t_shell	*create_shell(char **envp)
{
	t_shell	*shell;
	t_ctx	*ctx;

	shell = malloc(sizeof(t_shell));
	if (!shell)
		return (NULL);
	ctx = malloc(sizeof(t_ctx));
	if (!ctx)
	{
		free(shell);
		return (NULL);
	}
	shell->ctx = ctx;
	ctx->envp = envp;
	shell->ast = create_ast_tree();
	if (!shell->ast)
	{
		free(shell);
		return (NULL);
	}
	shell->free = free_shell;
	shell->build = build_shell;
	shell->execute = execute_shell;
	return (shell);
}
