/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ybutkov <ybutkov@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/08 17:55:18 by ybutkov           #+#    #+#             */
/*   Updated: 2025/10/13 20:15:40 by ybutkov          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ERROR_H
# define ERROR_H

# include "shell.h"

void	output_error(const char *obj, char *error);
void	output_error_and_exit(const char *obj, char *error, t_shell *shell,
			int exit_error_code);

#endif
