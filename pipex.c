/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ybutkov <ybutkov@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/04 13:10:57 by ybutkov           #+#    #+#             */
/*   Updated: 2025/10/09 20:17:15 by ybutkov          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"
#include "ast.h"
#include "shell.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

void	execute_cmd(char *cmd_with_args[]);
void	handle_error(const char *msg);

// Заглушка для примера
void	handle_error(const char *msg)
{
	perror(msg);
	exit(EXIT_FAILURE);
}

int	main(int argc, char **argv, char **envp)
{
	(void)argc;
	(void)argv;
	// // char *cmd[3] = {"/usr/bin/cat", "pipex.c", NULL};

	// // Структура команд: { "путь_к_программе", "аргумент1", NULL }
	// char *commands[MAX_CMDS][3] = {
	// 	{"/usr/bin/cat", NULL, NULL},
	// 	{"/usr/bin/grep", "main", NULL},
	// 	{"/usr/bin/wc", "-l", NULL},
	// 	{NULL, NULL, NULL}
	// 	// Последний элемент должен быть NULL-терминирован в реальном коде
	// };
	// (void)commands;



	t_shell	*shell;
	int		i;
	int		param_count;
	int		exit_status;

	char **commands;
	commands = malloc(argc * sizeof(char *));
	i = 1;
	param_count = 0;
	while (i < argc)
	{
		if (ft_strlen(argv[i]) > 0)
			commands[param_count++] = argv[i];
		i++;
	}
	commands[param_count] = NULL;
	if (param_count < 4)
	{
		return (EXIT_FAILURE);
	}

	shell = create_shell(envp);
	shell->build(shell, commands, param_count);
	shell->execute(shell);
	exit_status = shell->ctx->last_exit_status;
	shell->free(shell);
	printf("Exit status: %d\n", exit_status);
	// execute_ast(root);
	// pipex_n_commands(commands, 3, "infile.txt", "outfile.txt");
	return (exit_status);
}
