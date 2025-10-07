/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ybutkov <ybutkov@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/04 13:10:57 by ybutkov           #+#    #+#             */
/*   Updated: 2025/10/07 16:56:03 by ybutkov          ###   ########.fr       */
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
	int i;

	char **commands;
	commands = malloc(argc * sizeof(char *));
	i = 1;
	while (i < argc)
	{
		commands[i - 1] = argv[i];
		i++;
	}
	commands[i - 1] = NULL;

	shell = create_shell(envp);
	shell->build(shell, commands);
	shell->execute(shell);
	// execute_ast(root);
	// pipex_n_commands(commands, 3, "infile.txt", "outfile.txt");
	return (0);
}
