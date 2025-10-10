/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   output_error.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ybutkov <ybutkov@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/08 17:43:42 by ybutkov           #+#    #+#             */
/*   Updated: 2025/10/10 12:41:06 by ybutkov          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"
#include "libft.h"
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

void output_error(const char *message)
{
	write(STDERR_FILENO, PIPEX, ft_strlen(PIPEX));
	write(STDERR_FILENO, strerror(errno), ft_strlen(strerror(errno)));
	write(STDERR_FILENO, COLON, ft_strlen(COLON));
	write(STDERR_FILENO, message, ft_strlen(message));
	// printf("%s%s%s\n", PIPEX, strerror(errno), message);
}
