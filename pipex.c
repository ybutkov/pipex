/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ybutkov <ybutkov@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/04 13:10:57 by ybutkov           #+#    #+#             */
/*   Updated: 2025/10/04 20:35:55 by ybutkov          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

void	execute_cmd(char *cmd_with_args[]);
void	handle_error(const char *msg);

void	pipex_n_commands(char *cmds[][3], int num_cmds, char *infile,
		char *outfile)
{
	int		pipe_fd[2];
	int		prev_pipe_read_end;
	pid_t	pid;
	int		i;
	int		infile_fd;
	int		outfile_fd;

	prev_pipe_read_end = -1;
	infile_fd = open(infile, O_RDONLY);
	outfile_fd = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (infile_fd == -1 || outfile_fd == -1)
		handle_error("File open error");
	for (i = 0; i < num_cmds; i++)
	{
		if (i < num_cmds - 1)
		{
			if (pipe(pipe_fd) == -1)
				handle_error("Pipe creation error");
		}
		pid = fork();
		if (pid == -1)
			handle_error("Fork error");
		printf("Forked process PID: %d for command %d\n", pid, i + 1);
		if (pid == 0)
		{
			if (i == 0)
			{
				dup2(infile_fd, STDIN_FILENO);
			}
			else
			{
				dup2(prev_pipe_read_end, STDIN_FILENO);
				close(prev_pipe_read_end);
			}
			if (i == num_cmds - 1)
			{
				dup2(outfile_fd, STDOUT_FILENO);
			}
			else
			{
				dup2(pipe_fd[1], STDOUT_FILENO);
				close(pipe_fd[0]);
				close(pipe_fd[1]);
			}
			close(infile_fd);
			close(outfile_fd);
			execute_cmd(cmds[i]);
		}
		if (prev_pipe_read_end != -1)
			close(prev_pipe_read_end);
		if (i < num_cmds - 1)
		{
			prev_pipe_read_end = pipe_fd[0];
			close(pipe_fd[1]);
		}
	}
	close(infile_fd);
	close(outfile_fd);
	while (wait(NULL) > 0)
		;
}

// Заглушка для примера
void	handle_error(const char *msg)
{
	perror(msg);
	exit(EXIT_FAILURE);
}

// Заглушка для примера
void	execute_cmd(char *cmd_with_args[])
{
	char	*path;

	// Предполагаем, что 0-й элемент - это путь
	// printf("Executing: %s\n", cmd_with_args[0]);
	path = cmd_with_args[0];
	if (execve(path, cmd_with_args, NULL) == -1)
		handle_error("Execve error");
}

t_ast	*create_cmd_node(char *cmd_with_args[])
{
	t_ast	*node;
	t_cmd	*cmd;

	node = malloc(sizeof(t_ast));
	if (!node)
		handle_error("Memory allocation error");
	cmd = malloc(sizeof(t_cmd));
	if (!cmd)
	{
		free(node);
		handle_error("Memory allocation error");
	}
	cmd->argv = cmd_with_args;
	cmd->path = get_cmd_path(cmd_with_args[0], NULL);
	cmd->redir_in = NULL;
	cmd->redir_out = NULL;
	node->type = CMD;
	node->cmd = cmd;
	node->left = NULL;
	node->right = NULL;
	return (node);
}

// Рекурсивное выполнение AST

void	execute_ast(t_ast *node)
{
	if (!node)
		return ;
	if (node->type == CMD)
	{
		// входной редирект
		if (node->cmd->redir_in->file)
		{
			int fd = open(node->cmd->redir_in->file, O_RDONLY);
			if (fd < 0)
			{
				perror("open infile");
				exit(1);
			}
			dup2(fd, STDIN_FILENO);
			close(fd);
		}
		// выходной редирект

		if (node->cmd->redir_out->file)
		{
			int fd = open(node->cmd->redir_out->file,
					O_WRONLY | O_CREAT | O_TRUNC, 0644);
			if (fd < 0)
			{
				perror("open outfile");
				exit(1);
			}
			dup2(fd, STDOUT_FILENO);
			close(fd);
		}
		execve(node->cmd->path, node->cmd->argv, NULL);
		perror("execve failed");
		exit(1);
	}
	else if (node->type == PIPE)
	{
		int fd[2];
		if (pipe(fd) == -1)
		{
			perror("pipe");
			exit(1);
		}
		pid_t pid = fork();
		if (pid == -1)
		{
			perror("fork");
			exit(1);
		}
		if (pid == 0) // левый дочерний
		{
			dup2(fd[1], STDOUT_FILENO);
			close(fd[0]);
			close(fd[1]);
			execute_ast(node->left);
			exit(0); // на всякий случай
		}
		else // родитель
		{
			dup2(fd[0], STDIN_FILENO);
			close(fd[0]);
			close(fd[1]);
			execute_ast(node->right);
			waitpid(pid, NULL, 0);
		}
	}
}
#define MAX_CMDS 4

int	main(int argc, char **argv, char **envp)
{
	char	*cmd1[3] = {"/usr/bin/cat", "-e", NULL};
	char	*cmd3[3] = {"/usr/bin/wc", "-l", NULL};
	t_ast	*nd1;
	t_ast	*nd3;
	t_ast	*root;

	(void)argc;
	(void)argv;
	(void)envp;
	// Структура команд: { "путь_к_программе", "аргумент1", NULL }
	char *commands[MAX_CMDS][3] = {
		{"/usr/bin/cat", NULL, NULL}, {"/usr/bin/grep", "main", NULL},
			{"/usr/bin/wc", "-l", NULL}, {NULL, NULL, NULL}
		// Последний элемент должен быть NULL-терминирован в реальном коде
	};
	(void)commands;
	// char *cmd[3] = {"/usr/bin/cat", "pipex.c", NULL};
	// char *path = get_cmd_path("cat", envp);
	// printf("Resolved path: %s\n", path);
	// char *cmd2[3] = {"/usr/bin/grep", "cmd", NULL};
	nd1 = create_cmd_node(cmd1);
	nd3 = create_cmd_node(cmd3);
	root = malloc(sizeof(t_ast));
	root->type = PIPE;
	root->cmd = NULL;
	root->left = nd1;
	root->right = nd3;
	execute_ast(root);
	// pipex_n_commands(commands, 3, "infile.txt", "outfile.txt");
	return (0);
}
