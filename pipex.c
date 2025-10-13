/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ybutkov <ybutkov@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/04 13:10:57 by ybutkov           #+#    #+#             */
/*   Updated: 2025/10/13 20:44:35 by ybutkov          ###   ########.fr       */
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

// static char	**get_commands_from_argv(int argc, char **argv, int *param_count)
// {
// 	char	**commands;
// 	int		i;

// 	commands = malloc(argc * sizeof(char *));
// 	if (!commands)
// 		return (NULL);
// 	*param_count = 0;
// 	i = 1;
// 	while (i < argc)
// 	{
// 		if (ft_strlen(argv[i]) == 0)
// 		{
// 			i++;
// 			continue ;
// 		}
// 		commands[*param_count] = ft_strdup(argv[i]);
// 		if (!commands[*param_count])
// 		{
// 			free_str_array(commands);
// 			return (NULL);
// 		}
// 		(*param_count)++;
// 		i++;
// 	}
// 	commands[*param_count] = NULL;
// 	return (commands);
// }

int	main(int argc, char **argv, char **envp)
{
	t_shell	*shell;
	int		param_count;
	int		exit_status;
	char	**commands;
	int		i;
	char	*str;

	if (argc < 5)
		return (EXIT_FAILURE);
	// commands = get_commands_from_argv(argc, argv, &param_count);
	// if (!commands || param_count < 4)
	// 	return (EXIT_FAILURE);
	commands = malloc(argc * sizeof(char *));
	i = 1;
	param_count = 0;
	while (i < argc)
	{
		if (ft_strlen(argv[i]) > 0)
		{
			str = ft_strdup(argv[i]);
			if (!str)
			{
				while (--param_count >= 0)
					free(commands[param_count]);
				free(commands);
				return (EXIT_FAILURE);
			}
			commands[param_count++] = str;
		}
		i++;
	}
	commands[param_count] = NULL;
	shell = create_shell(envp);
	shell->build(shell, commands, param_count);
	free_str_array(commands);
	shell->execute(shell);
	exit_status = shell->ctx->last_exit_status;
	shell->free(shell);
	return (exit_status);
}
