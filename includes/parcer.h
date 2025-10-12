/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parcer.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ybutkov <ybutkov@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/12 16:53:25 by ybutkov           #+#    #+#             */
/*   Updated: 2025/10/12 18:15:30 by ybutkov          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARCER_H
# define PARCER_H

# define SINGLE_QUOTE '\''
# define DOUNLE_QUOTE '\"'
# define QUOTES "\'\""
# define SPACES " \t\n\r\v\f"
# define SPECIAL_CHARS "<>|&"

typedef enum s_token_type
{
	TOKEN_WORD,
	TOKEN_END
}					t_token_type;

typedef struct s_token
{
	t_token_type	type;
	char			*value;
	struct s_token	*next;
}					t_token;

t_token				*create_token(t_token_type type, char *value);

#endif
