/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   output_error.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ybutkov <ybutkov@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/08 17:43:42 by ybutkov           #+#    #+#             */
/*   Updated: 2025/10/12 20:32:34 by ybutkov          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"
#include "libft.h"
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

void output_error(const char *obj, char *error)
{
	write(STDERR_FILENO, PIPEX, ft_strlen(PIPEX));
	if (!error)
		write(STDERR_FILENO, strerror(errno), ft_strlen(strerror(errno)));
	else
		write(STDERR_FILENO, error, ft_strlen(error));
	write(STDERR_FILENO, COLON, ft_strlen(COLON));
	write(STDERR_FILENO, obj, ft_strlen(obj));
	write(STDERR_FILENO, "\n", 1);
	// printf("%s%s%s\n", PIPEX, strerror(errno), message);
}
