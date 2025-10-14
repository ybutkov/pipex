/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ybutkov <ybutkov@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/04 13:10:57 by ybutkov           #+#    #+#             */
/*   Updated: 2025/10/14 13:29:31 by ybutkov          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ast.h"
#include "pipex.h"
#include "shell.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

char	*collect_args(int argc, char **argv)
{
	char	*res;
	int		i;

	i = 1;
	while (i < argc)
	{
		res = ft_strjoin(res, argv[i]);
		i++;
	}
	return (res);
}

static int	is_arg_not_empty(char *arg)
{
	char	*trimmed;

	trimmed = ft_strtrim(arg, " \t\n");
	if (trimmed && ft_strlen(trimmed) == 0)
		return (free(trimmed), 0);
	if (trimmed)
		free(trimmed);
	return (1);
}

static char	**get_commands_from_argv(int argc, char **argv, int *param_count)
{
	char	**commands;
	int		i;

	commands = malloc(argc * sizeof(char *));
	if (!commands)
		return (NULL);
	*param_count = 0;
	i = 1;
	while (i < argc)
	{
		if (is_arg_not_empty(argv[i]))
		{
			commands[*param_count] = ft_strdup(argv[i]);
			if (!commands[*param_count])
			{
				free_str_array(commands);
				return (NULL);
			}
			(*param_count)++;
		}
		i++;
	}
	commands[*param_count] = NULL;
	return (commands);
}

int	main(int argc, char **argv, char **envp)
{
	t_shell	*shell;
	int		param_count;
	int		exit_status;
	char	**commands;

	if (argc < 5)
		return (EXIT_FAILURE);
	commands = get_commands_from_argv(argc, argv, &param_count);
	if (!commands || param_count < 4)
	{
		if (commands)
			free_str_array(commands);
		return (EXIT_FAILURE);
	}
	shell = create_shell(envp);
	shell->build(shell, commands, param_count);
	free_str_array(commands);
	shell->execute(shell);
	exit_status = shell->ctx->last_exit_status;
	shell->free(shell);
	return (exit_status);
}
