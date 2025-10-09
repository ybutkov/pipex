/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shell_core.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ybutkov <ybutkov@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/07 12:20:01 by ybutkov           #+#    #+#             */
/*   Updated: 2025/10/09 20:33:32 by ybutkov          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include "shell.h"
#include "error.h"
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
	if (node->free)
		node->free(node);
}

static void	free_shell(t_shell *shell)
{
	if (!shell)
		return ;
	if (shell->ast)
		shell->ast->free(shell->ast, free_shell_node_bridge);

	if (shell->ctx)
		free(shell->ctx);
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
	char			*full_path;
	char			**cmd_argv;
	char			*redirs[2];
	int				i;

	node = create_shell_node(NODE_PIPE, NULL);
	ast_node = create_ast_node(node);

	// node = malloc(sizeof(t_shell_node));
	// node->type = NODE_PIPE;
	// node->data.cmd = NULL;
	// ast_node = create_ast_node(node);
	shell->ast->set_root(shell->ast, ast_node);
	curr_node = shell->ast->get_root(shell->ast);

	// node = malloc(sizeof(t_shell_node));
	// node->type = NODE_REDIR_IN;
	// node->data.redir = malloc(sizeof(t_redir));
	// node->data.redir->type = REDIR_IN;
	// node->data.redir->target = ft_strdup(*commands);
	// commands++;
	// ast_node = create_ast_node(node);
	// curr_node->set_left(curr_node, ast_node);
	// curr_node = curr_node->get_left(curr_node);
	// cmd_argv = ft_split(*commands, ' ');
	// commands++;
	// node = malloc(sizeof(t_shell_node));
	// node->type = NODE_CMD;
	// node->data.cmd = malloc(sizeof(t_cmd));
	// node->data.cmd->argv = cmd_argv;
	// full_path = get_cmd_path(cmd_argv[0], shell->ctx->envp);
	// node->data.cmd->path = full_path;
	// ast_node = create_ast_node(node);
	// curr_node->set_left(curr_node, ast_node);

	//cmd_argv = ft_split(commands[0], ' ');
	// redirs[0] = cmd_argv[0];
	//free_str_array(cmd_argv++);
	redirs[0] = commands[0];
	i = 1;
	while (i < argc - 1)
	{
		cmd_argv = ft_split(commands[i], ' ');
		full_path = get_cmd_path(cmd_argv[0], shell->ctx->envp);
		cmd = create_cmd(cmd_argv, full_path);
		node = create_shell_node(NODE_CMD, cmd);

		// node = malloc(sizeof(t_shell_node));
		// node->type = NODE_CMD;
		// node->data.cmd = malloc(sizeof(t_cmd));
		// cmd_argv = ft_split(commands[i], ' ');
		// node->data.cmd->argv = cmd_argv;
		// full_path = get_cmd_path(cmd_argv[0], shell->ctx->envp);
		// node->data.cmd->path = full_path;

		if (redirs[0])
		{
			redir = malloc(sizeof(t_list));
			redir->content = malloc(sizeof(t_redir));
			redir->next = NULL;
			((t_redir *)redir->content)->type = REDIR_IN;
			((t_redir *)redir->content)->target = redirs[0];
			node->data.cmd->redirs = redir;
			redirs[0] = NULL;
		}
		ast_node = create_ast_node(node);
		curr_node->set_left(curr_node, ast_node);
		i++;
		cmd_argv = ft_split(commands[i], ' ');
		if (i == argc - 2)
		{
			full_path = get_cmd_path(cmd_argv[0], shell->ctx->envp);
			cmd = create_cmd(cmd_argv, full_path);
			node = create_shell_node(NODE_CMD, cmd);
			ast_node = create_ast_node(node);
			curr_node->set_right(curr_node, ast_node);
			curr_node = curr_node->get_right(curr_node);
			break ;
		}
		if (i < argc - 1)
		{
			node = create_shell_node(NODE_PIPE, NULL);
			ast_node = create_ast_node(node);
			curr_node->set_right(curr_node, ast_node);
			curr_node = curr_node->get_right(curr_node);
		}
	}

	t_redir *redirect = malloc(sizeof(t_redir));
	redirect->type = REDIR_OUT;
	redirect->target = commands[argc - 1];
	redir = ft_lstnew(redirect);
	node->data.cmd->redirs = redir;

	// redir = malloc(sizeof(t_list));
	// redir->content = malloc(sizeof(t_redir));
	// ((t_redir *)redir->content)->type = REDIR_OUT;
	// ((t_redir *)redir->content)->target = commands[argc - 1];
	// node->data.cmd->redirs = redir;
	free(commands);
	return (NULL);
}



static void	apply_redirect(t_cmd *cmd)
{
	t_list	*redir;
	t_redir	*redirect;
	int		fd;

	// printf("------------------------------apply_redirect\n");
	// printf("cmd->redirs: %p\n", cmd->redirs->content);
	// fflush(stdout);
	redir = cmd->redirs;
	while (redir)
	{
		redirect = (t_redir *)redir->content;
		// printf("redirect->type: %d, target: %s\n", redirect->type, redirect->target);
		// fflush(stdout);
		if (redirect->type == REDIR_IN)
		{
			fd = open(redirect->target, O_RDONLY);
			if (fd == -1)
			{
				perror(redirect->target);
				// output_error(redirect->target);
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
				// output_error(redirect->target);
				// exit(EXIT_FAILURE);
				perror(redirect->target);
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
		// printf("execute_CMD\n"); fflush(stdout);
		apply_redirect(cmd);
		if (access(cmd->path, X_OK) != 0)
		{

			exit(EXIT_CMD_NOT_FOUND);
		}
		// apply_redirect(cmd);
		execve(cmd->path, cmd->argv, ctx->envp);
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
		// Implement output redirection handling
	}
	else if (shell_node->type == NODE_REDIR_APPEND)
	{
		// Implement append redirection handling
	}
	else if (shell_node->type == NODE_REDIR_HEREDOC)
	{
		// Implement heredoc handling
	}
	//  other node types
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
