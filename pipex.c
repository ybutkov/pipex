/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ybutkov <ybutkov@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/04 13:10:57 by ybutkov           #+#    #+#             */
/*   Updated: 2025/10/12 19:37:24 by ybutkov          ###   ########.fr       */
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

// Заглушка для примера
void	handle_error(const char *msg)
{
	perror(msg);
	exit(EXIT_FAILURE);
}

char	*collect_args(int argc, char **argv)
{
	char	*res;
	int		i;

	i = 1;

	while(i < argc)
	{
		res = ft_strjoin(res, argv[i]);
		i++;
	}
	return (res);
}

int	main(int argc, char **argv, char **envp)
{
	t_shell	*shell;
	int		i;
	int		param_count;
	int		exit_status;
	char	**commands;
	char	*str;

	if (argc < 5)
		return (EXIT_FAILURE);
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
	// free_str_array(commands);
	shell->free(shell);

	// printf("Exit status: %d\n", exit_status);

	return (exit_status);
}
