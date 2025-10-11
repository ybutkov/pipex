/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shell_core.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ybutkov <ybutkov@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/07 12:20:01 by ybutkov           #+#    #+#             */
/*   Updated: 2025/10/11 19:47:30 by ybutkov          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include "shell.h"
#include "error.h"
#include "utils.h"
#include "get_next_line.h"
#include <fcntl.h>
#include <sys/wait.h>


int	execute_shell_node(t_ast_node *node, t_shell *shell, int input_fd,
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
	char			*first_arg;
	int				i;

	node = create_shell_node(NODE_PIPE, NULL);
	ast_node = create_ast_node(node);
	shell->ast->set_root(shell->ast, ast_node);
	curr_node = shell->ast->get_root(shell->ast);

	first_arg = ft_strdup(commands[0]);

	i = 1;
	while (i < argc - 1)
	{
		if (i == 1 && !ft_strcmp(HERE_DOC, first_arg))
		{
			char *delim = ft_strdup(commands[i]);
			redirect = create_redir(REDIR_HEREDOC, delim);
			node = create_shell_node(NODE_REDIR_HEREDOC, redirect);
			ast_node = create_ast_node(node);
			curr_node->set_left(curr_node, ast_node);
			i++;
			cmd_argv = ft_split(commands[i], ' ');
			full_path = get_cmd_path(cmd_argv[0], shell->ctx->envp);
			cmd = create_cmd(cmd_argv, full_path);
			node = create_shell_node(NODE_CMD, cmd);
			ast_node->set_left(ast_node, create_ast_node(node));
			i++;
			continue ;
		}

		cmd_argv = ft_split(commands[i], ' ');
		if (!cmd_argv)
			continue ;
		full_path = get_cmd_path(cmd_argv[0], shell->ctx->envp);
		cmd = create_cmd(cmd_argv, full_path);
		node = create_shell_node(NODE_CMD, cmd);

		if (i == 1)
		{
			redirect = create_redir(REDIR_IN, first_arg);
			node->data.cmd->redirs = ft_lstnew(redirect);
		}


		if (i == argc - 2)
		{
			if (!ft_strcmp(HERE_DOC, first_arg))
				redirect = create_redir(REDIR_APPEND, ft_strdup(commands[argc - 1]));
			else
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
	if (!ft_strcmp(HERE_DOC, first_arg))
		free(first_arg);
	return (NULL);
}

static void	apply_redirect(t_cmd *cmd, t_shell *shell)
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
				shell->free(shell);
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
				shell->free(shell);
				exit(EXIT_FAILURE);
			}
			dup2(fd, STDOUT_FILENO);
			close(fd);
		}
		else if (redirect->type == REDIR_HEREDOC)
		{

		}
		else if (redirect->type == REDIR_APPEND)
		{
			fd = open(redirect->target, O_WRONLY | O_CREAT | O_APPEND, 0644);
			if (fd == -1)
			{
				output_error(redirect->target);
				shell->free(shell);
				exit(EXIT_FAILURE);
			}
			dup2(fd, STDOUT_FILENO);
			close(fd);
		}
		redir = redir->next;
	}
}

static int	execute_CMD(t_cmd *cmd, t_shell *shell, int input_fd, int output_fd)
{
	pid_t	pid;
	int		status;

	pid = fork();
	if (pid == -1)
	{
		perror("fork");
		return (EXIT_FAILURE);
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
		apply_redirect(cmd, shell);
		if (!cmd->path || access(cmd->path, X_OK) != 0)
		{
			shell->free(shell);
			exit(EXIT_CMD_NOT_FOUND);
		}
		execve(cmd->path, cmd->argv, shell->ctx->envp);
		shell->free(shell);
		exit(EXIT_FAILURE);
	}
	else
	{
		if (input_fd != STDIN_FILENO)
			close(input_fd);
		if (output_fd != STDOUT_FILENO)
			close(output_fd);
		waitpid(pid, &status, 0);
		shell->ctx->last_exit_status = WEXITSTATUS(status);
		return (status);
	}
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

int	is_delimiter(char *line, char *delimeter)
{
	char *new_line_char;

	new_line_char = ft_strchr(line, '\n');
	// printf("%s >%c<", line, *new_line_char); fflush(stdout);
	if (new_line_char)
		*new_line_char = '\0';
	if (ft_strcmp(line, delimeter) == 0)
	{
		if (new_line_char)
			*new_line_char = '\n';
		return (1);
	}
	if (new_line_char)
		*new_line_char = '\n';
	return (0);
}

static void collect_heredoc_input(char *target, int write_fd)
{
	char	*line;

	while (1)
	{
		ft_putstr_fd("heredoc> ", STDOUT_FILENO);
		line = get_next_line(STDIN_FILENO);
		if (!line)
			break ;
		if (is_delimiter(line, target))
		{
			free(line);
			break ;
		}
		write(write_fd, line, ft_strlen(line));
		free(line);
	}
}

int	execute_redir_heredoc(t_ast_node *ast_node, t_shell *shell, int in_fd,
		int old_fd_out)
{
	int heredoc_pipe[2];
	pid_t pid_writer;
	int status;
	int ret_code = 1;

	t_shell_node *node = (t_shell_node *)ast_node->get_content(ast_node);
	(void)in_fd;

	if (pipe(heredoc_pipe) == -1)
		return (EXIT_FAILURE);
	pid_writer = fork();
	if (pid_writer == -1)
	{
		close(heredoc_pipe[0]);
		close(heredoc_pipe[1]);
		return (ret_code);
	}
	if (pid_writer == 0)
	{
		// --- ПРОЦЕСС WRITER ---       
		// Закрываем конец чтения
		close(heredoc_pipe[0]);

		// [ВАША ФУНКЦИЯ]: Сбор текста с терминала и запись в pipe[1] 
		collect_heredoc_input(node->data.redir->target, heredoc_pipe[1]);
		// Закрываем конец записи
		close(heredoc_pipe[1]);
		shell->free(shell);
		exit(EXIT_SUCCESS);
	}
	else
	{
// --- РОДИТЕЛЬСКИЙ ПРОЦЕСС (ШЕЛЛ) ---               
// 1. Закрыть конец записи в РОДИТЕЛЕ (Предотвращение зависания)
		close(heredoc_pipe[1]);
		// 2. Ждать завершения Writer'а       
		waitpid(pid_writer, &status, 0);
// 3. Рекурсивный вызов для потомка
// old_fd_in игнорируется. 
		ret_code = execute_shell_node(ast_node->get_left(ast_node), shell,
// Новый fd_in (конец чтения пайпа) 
			heredoc_pipe[0],
			// Старый fd_out
			old_fd_out);
// 4. Закрыть конец чтения
		close(heredoc_pipe[0]);
		return (ret_code);
	}
}

int	execute_shell_node(t_ast_node *node, t_shell *shell, int in_fd,
		int out_fd)
{
	t_shell_node	*shell_node;
	int				pipe_fds[2];
	int				status_code;

	shell_node = (t_shell_node *)node->get_content(node);
	if (shell_node->type == NODE_PIPE)
	{
		pipe(pipe_fds);
		if (node->get_left(node))
			execute_shell_node(node->get_left(node), shell, in_fd, pipe_fds[1]);
		close(pipe_fds[1]);
		if (node->get_right(node))
			status_code = execute_shell_node(node->get_right(node), shell,
				pipe_fds[0], out_fd);
		close(pipe_fds[0]);
		return (status_code);
	}
	else if (shell_node->type == NODE_CMD)
	{
		return (execute_CMD(shell_node->data.cmd, shell, in_fd, out_fd));
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
		return (execute_redir_heredoc(node, shell, in_fd, out_fd));
	}
	return (1);
}

static void	execute_shell(t_shell *shell)
{
	t_ast_node	*root;

	root = shell->ast->get_root(shell->ast);
	execute_shell_node(root, shell, STDIN_FILENO, STDOUT_FILENO);
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
