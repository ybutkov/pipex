/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   output_error.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ybutkov <ybutkov@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/08 17:43:42 by ybutkov           #+#    #+#             */
/*   Updated: 2025/10/08 17:54:58 by ybutkov          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"
#include <stdio.h>
#include <string.h>
#include <errno.h>

void output_error(const char *message)
{
	printf("%s%s%s\n", PIPEX, strerror(errno), message);
}
