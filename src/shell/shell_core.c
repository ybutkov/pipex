/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shell_core.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ybutkov <ybutkov@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/07 12:20:01 by ybutkov           #+#    #+#             */
/*   Updated: 2025/10/10 20:08:36 by ybutkov          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include "shell.h"
#include "error.h"
#include "utils.h"
#include <fcntl.h>
#include <sys/wait.h>


void		execute_shell_node(t_ast_node *node, t_ctx *ctx, int input_fd,
				int output_fd);

static void free_shell_node_bridge(void *content)
{
	t_shell_node *node;

	if (!content)
		return ;
	node = (t_shell_node *)content;
	if (node && node->free)
		node->free(node);
}

static void	free_shell(t_shell *shell)
{
	if (!shell)
		return ;
	if (shell->ast)
	{
		shell->ast->free(shell->ast, free_shell_node_bridge);
		shell->ast = NULL;
	}

	if (shell->ctx)
	{
		shell->ctx->free_ctx(shell->ctx);
		shell->ctx = NULL;
	}
	free(shell);
}

#include <stdio.h>

static void	*build_shell(t_shell *shell, char **commands, int argc)
{
	t_shell_node	*node;
	t_ast_node		*ast_node;
	t_ast_node		*curr_node;
	t_list			*redir;
	t_cmd 			*cmd;
	t_redir			*redirect;
	t_shell_node	*pipe_node;
	t_ast_node		*pipe_ast;

	char			*full_path;
	char			**cmd_argv;
	char			*redirs[2];
	int				i;

	node = create_shell_node(NODE_PIPE, NULL);
	ast_node = create_ast_node(node);
	shell->ast->set_root(shell->ast, ast_node);
	curr_node = shell->ast->get_root(shell->ast);

	redirs[0] = ft_strdup(commands[0]);

	i = 1;
	while (i < argc - 1)
	{
		cmd_argv = ft_split(commands[i], ' ');
		if (!cmd_argv)
			continue;
		full_path = get_cmd_path(cmd_argv[0], shell->ctx->envp);
		cmd = create_cmd(cmd_argv, full_path);
		node = create_shell_node(NODE_CMD, cmd);

		if (i == 1)
		{
			redirect = create_redir(REDIR_IN, redirs[0]);
			node->data.cmd->redirs = ft_lstnew(redirect);
		}

		if (i == argc - 2)
		{
			redirect = create_redir(REDIR_OUT, ft_strdup(commands[argc - 1]));
			redir = ft_lstnew(redirect);
			if (node->data.cmd->redirs)
				ft_lstadd_back(&node->data.cmd->redirs, redir);
			else
				node->data.cmd->redirs = redir;
		}

		ast_node = create_ast_node(node);
		if (i == 1)
			curr_node->set_left(curr_node, ast_node);
		else if (i == argc -2)
			curr_node->set_right(curr_node, ast_node);
		else
		{
			pipe_node = create_shell_node(NODE_PIPE, NULL);
			pipe_ast = create_ast_node(pipe_node);
			curr_node->set_right(curr_node, pipe_ast);
			pipe_ast->set_left(pipe_ast, ast_node);
			curr_node = pipe_ast;
		}
		i++;
	}

	return (NULL);
}

static void	apply_redirect(t_cmd *cmd)
{
	t_list	*redir;
	t_redir	*redirect;
	int		fd;

	// cmd->redirs ?
	if (!cmd)
		return ;

	redir = cmd->redirs;
	while (redir)
	{
		redirect = (t_redir *)redir->content;
		if (redirect->type == REDIR_IN)
		{
			fd = open(redirect->target, O_RDONLY);
			if (fd == -1)
			{
				output_error(redirect->target);
				// exit_from_child(EXIT_FAILURE);
				exit(EXIT_FAILURE);
			}
			dup2(fd, STDIN_FILENO);
			close(fd);
		}
		else if (redirect->type == REDIR_OUT)
		{
			fd = open(redirect->target, O_WRONLY | O_CREAT | O_TRUNC, 0644);
			if (fd == -1)
			{
				output_error(redirect->target);
				// exit_from_child(EXIT_FAILURE);
				exit(EXIT_FAILURE);
			}
			dup2(fd, STDOUT_FILENO);
			close(fd);
		}
		redir = redir->next;
	}
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
		apply_redirect(cmd);
		if (access(cmd->path, X_OK) != 0)
		{
			// exit_from_child(EXIT_CMD_NOT_FOUND);
			exit(EXIT_CMD_NOT_FOUND);
		}
		// apply_redirect(cmd);
		execve(cmd->path, cmd->argv, ctx->envp);
		// exit_from_child(EXIT_FAILURE);
		exit(EXIT_FAILURE); // execve failed
	}
	waitpid(pid, &status, 0);
	ctx->last_exit_status = WEXITSTATUS(status);
}

// static void	execute_redir_in(t_ast_node *node, t_ctx *ctx, int input_fd,
// 		int output_fd)
// {
// 	t_shell_node	*shell_node;
// 	int				fd;
// 	(void)input_fd;
// 	shell_node = (t_shell_node *)node->get_content(node);
// 	if (shell_node->type != NODE_REDIR_IN)
// 		return ;
// 	printf("execute_redir_in\n"); fflush(stdout);
// 	fd = open(shell_node->data.redir->target, O_RDONLY);
// 	if (fd == -1)
// 	{
// 		perror("open");
// 		//exit
// 		return ;
// 	}
// 	execute_shell_node(node->get_left(node), ctx, fd, output_fd);
// 	close(fd);
// }

void	execute_shell_node(t_ast_node *node, t_ctx *ctx, int input_fd,
		int output_fd)
{
	t_shell_node	*shell_node;
	int				pipe_fds[2];

	shell_node = (t_shell_node *)node->get_content(node);
	if (shell_node->type == NODE_PIPE)
	{
		pipe(pipe_fds);
		if (node->get_left(node))
			execute_shell_node(node->get_left(node), ctx, input_fd,
				pipe_fds[1]);
		close(pipe_fds[1]);
		if (node->get_right(node))
			execute_shell_node(node->get_right(node), ctx, pipe_fds[0],
				output_fd);
		close(pipe_fds[0]);
	}
	else if (shell_node->type == NODE_CMD)
	{
		execute_CMD(shell_node->data.cmd, ctx, input_fd, output_fd);
	}
	else if (shell_node->type == NODE_REDIR_IN)
	{
		// execute_redir_in(node, ctx, input_fd, output_fd);
	}
	else if (shell_node->type == NODE_REDIR_OUT)
	{

	}
	else if (shell_node->type == NODE_REDIR_APPEND)
	{

	}
	else if (shell_node->type == NODE_REDIR_HEREDOC)
	{

	}
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
	ctx = create_ctx(envp);
	if (!ctx)
	{
		free(shell);
		return (NULL);
	}
	shell->ctx = ctx;
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
