/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shell_execute.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ybutkov <ybutkov@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/07 12:20:01 by ybutkov           #+#    #+#             */
/*   Updated: 2025/10/13 20:14:57 by ybutkov          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "error.h"
#include "get_next_line.h"
#include "libft.h"
#include "parcer.h"
#include "shell.h"
#include "utils.h"
#include <fcntl.h>
#include <stdio.h>
#include <sys/wait.h>

int			execute_shell_node(t_ast_node *node, t_shell *shell, int input_fd,
				int output_fd);

void	open_file_and_dup2(char *filename, int flags, int dup_fd,
		t_shell *shell)
{
	int	fd;
	int	mode;

	if (flags & O_CREAT)
		mode = 0644;
	else
		mode = 0;
	fd = open(filename, flags, mode);
	if (fd == -1)
		output_error_and_exit(filename, NULL, shell, EXIT_FAILURE);
	dup2(fd, dup_fd);
	close(fd);
}

static void	apply_redirect(t_cmd *cmd, t_shell *shell)
{
	t_list	*redir;
	t_redir	*redirect;

	if (!cmd)
		return ;
	redir = cmd->redirs;
	while (redir)
	{
		redirect = (t_redir *)redir->content;
		if (redirect->type == REDIR_IN)
			open_file_and_dup2(redirect->target, O_RDONLY, STDIN_FILENO, shell);
		else if (redirect->type == REDIR_OUT)
			open_file_and_dup2(redirect->target, O_WRONLY | O_CREAT | O_TRUNC,
				STDOUT_FILENO, shell);
		else if (redirect->type == REDIR_APPEND)
			open_file_and_dup2(redirect->target, O_WRONLY | O_CREAT | O_APPEND,
				STDOUT_FILENO, shell);
		else if (redirect->type == REDIR_HEREDOC)
		{
		}
		redir = redir->next;
	}
}

static int	execute_CMD_child(t_cmd *cmd, t_shell *shell, int input_fd,
		int output_fd)
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
		output_error_and_exit(cmd->argv[0], CMD_NOT_FOUND_MSG, shell,
			EXIT_CMD_NOT_FOUND);
	execve(cmd->path, cmd->argv, shell->ctx->envp);
	shell->free(shell);
	exit(EXIT_FAILURE);
}

static int	execute_CMD(t_cmd *cmd, t_shell *shell, int input_fd, int output_fd)
{
	pid_t	pid;
	int		status;

	pid = fork();
	if (pid == -1)
		output_error_and_exit("fork", NULL, shell, EXIT_FAILURE);
	if (pid == 0)
		return (execute_CMD_child(cmd, shell, input_fd, output_fd));
	else
	{
		if (input_fd != STDIN_FILENO)
			close(input_fd);
		if (output_fd != STDOUT_FILENO)
			close(output_fd);
		waitpid(pid, &status, 0);
		shell->ctx->last_exit_status = WEXITSTATUS(status);
		return (WEXITSTATUS(status));
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

static int	is_delimiter(char *line, char *delimeter)
{
	char	*new_line_char;

	new_line_char = ft_strchr(line, '\n');
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

static void	collect_heredoc_input(char *target, int write_fd)
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

void	close_fds(int fds[2])
{
	close(fds[0]);
	close(fds[1]);
}

int	execute_redir_heredoc(t_ast_node *ast_node, t_shell *shell, int in_fd,
		int old_fd_out)
{
	t_shell_node	*node;
	int				heredoc_pipe[2];
	pid_t			pid_writer;
	int				status;
	int				ret_code;

	ret_code = 1;
	node = (t_shell_node *)ast_node->get_content(ast_node);
	(void)in_fd;
	if (pipe(heredoc_pipe) == -1)
		return (EXIT_FAILURE);
	pid_writer = fork();
	if (pid_writer == -1)
		return (close_fds(heredoc_pipe), ret_code);
	if (pid_writer == 0)
	{
		close(heredoc_pipe[0]);
		collect_heredoc_input(node->data.redir->target, heredoc_pipe[1]);
		close(heredoc_pipe[1]);
		shell->free(shell);
		exit(EXIT_SUCCESS);
	}
	else
	{
		close(heredoc_pipe[1]);
		waitpid(pid_writer, &status, 0);
		ret_code = execute_shell_node(ast_node->get_left(ast_node), shell,
				heredoc_pipe[0], old_fd_out);
		close(heredoc_pipe[0]);
		return (ret_code);
	}
}

int	execute_child_PIPE(t_ast_node *node, t_shell *shell, int in_fd,
	int pipe_fds[2])
{
	close(pipe_fds[0]);
	if (node->get_left(node))
		execute_shell_node(node->get_left(node), shell, in_fd, pipe_fds[1]);
	close(pipe_fds[1]);
	shell->free(shell);
	exit(EXIT_SUCCESS);
}

int	execute_PIPE(t_ast_node *node, t_shell *shell, int in_fd, int out_fd)
{
	int		status_code;
	int		pipe_fds[2];
	pid_t	left_pid;
	int		left_status;

	status_code = 0;
	if (pipe(pipe_fds) == -1)
		return (EXIT_FAILURE);
	left_pid = fork();
	if (left_pid == -1)
	{
		perror("fork");
		close_fds(pipe_fds);
		return (EXIT_FAILURE);
	}
	if (left_pid == 0)
		return (execute_child_PIPE(node, shell, in_fd, pipe_fds));
	close(pipe_fds[1]);
	if (node->get_right(node))
		status_code = execute_shell_node(node->get_right(node), shell,
				pipe_fds[0], out_fd);
	close(pipe_fds[0]);
	waitpid(left_pid, &left_status, 0);
	return (status_code);
}

int	execute_shell_node(t_ast_node *node, t_shell *shell, int in_fd, int out_fd)
{
	t_shell_node	*shell_node;

	shell_node = (t_shell_node *)node->get_content(node);
	if (shell_node->type == NODE_PIPE)
		return (execute_PIPE(node, shell, in_fd, out_fd));
	else if (shell_node->type == NODE_CMD)
		return (execute_CMD(shell_node->data.cmd, shell, in_fd, out_fd));
	else if (shell_node->type == NODE_REDIR_HEREDOC)
		return (execute_redir_heredoc(node, shell, in_fd, out_fd));
	return (1);
}
