/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   output_error.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ybutkov <ybutkov@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/08 17:43:42 by ybutkov           #+#    #+#             */
/*   Updated: 2025/10/14 13:34:49 by ybutkov          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include "pipex.h"
#include "shell.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#ifdef __APPLE__

void	output_error(const char *obj, char *error)
{
	write(STDERR_FILENO, PIPEX, ft_strlen(PIPEX));
	write(STDERR_FILENO, obj, ft_strlen(obj));
	write(STDERR_FILENO, COLON, ft_strlen(COLON));
	if (error)
		write(STDERR_FILENO, error, ft_strlen(error));
	else
		write(STDERR_FILENO, strerror(errno), ft_strlen(strerror(errno)));
	write(STDERR_FILENO, "\n", 1);
}

#elif defined(__linux__)

void	output_error(const char *obj, char *error)
{
	write(STDERR_FILENO, PIPEX, ft_strlen(PIPEX));
	if (error)
		write(STDERR_FILENO, error, ft_strlen(error));
	else
		write(STDERR_FILENO, strerror(errno), ft_strlen(strerror(errno)));
	write(STDERR_FILENO, COLON, ft_strlen(COLON));
	write(STDERR_FILENO, obj, ft_strlen(obj));
	write(STDERR_FILENO, "\n", 1);
}

#endif

void	output_error_and_exit(const char *obj, char *error, t_shell *shell,
		int exit_error_code)
{
	output_error(obj, error);
	if (shell)
		shell->free(shell);
	exit(exit_error_code);
}
