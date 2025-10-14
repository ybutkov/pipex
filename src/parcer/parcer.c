/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parcer.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ybutkov <ybutkov@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/04 13:07:34 by ybutkov           #+#    #+#             */
/*   Updated: 2025/10/14 21:27:05 by ybutkov          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include "parcer.h"
#include "utils.h"
#include <stdlib.h>

t_token	*create_token(t_token_type type, char *value)
{
	t_token	*token;

	token = malloc(sizeof(t_token));
	if (!token)
		return (NULL);
	token->type = type;
	token->value = value;
	token->next = NULL;
	return (token);
}

void	skip_spaces(char **str)
{
	while (**str && in_set(**str, SPACES))
		(*str)++;
}

char	*get_word(char **str)
{
	char	quote;
	char	*start;

	quote = 0;
	start = *str;
	if (**str && in_set(**str, SPACES))
	{
		skip_spaces(str);
	}
	while (**str)
	{
		if (!quote && in_set(**str, QUOTES))
			quote = **str;
		else if (quote && quote == **str)
			quote = 0;
		else if (!quote && (in_set(**str, SPECIAL_CHARS) || in_set(**str,
					SPACES)))
			break ;
		(*str)++;
	}
	if (in_set(*start, QUOTES))
		return (ft_substr(start, 1, *str - start - 2));
	return (ft_substr(start, 0, *str - start));
}

t_token	*tokenize(char *input)
{
	t_token	*head;
	t_token	*curr_token;
	t_token	*token;

	head = NULL;
	curr_token = NULL;
	while (*input)
	{
		if (*input == '\0')
			break ;
		if (in_set(*input, SPECIAL_CHARS))
		{
			input++;
			continue ;
		}
		else
			token = create_token(TOKEN_WORD, get_word(&input));
		if (!head)
			head = token;
		else
			curr_token->next = token;
		curr_token = token;
		skip_spaces(&input);
	}
	return (head);
}

char	**parse_command(char *command)
{
	t_token	*token;
	t_token	*temp;
	int		token_count;
	char	**res;
	int		i;

	token = tokenize(command);
	token_count = 0;
	temp = token;
	while (temp)
	{
		token_count++;
		temp = temp->next;
	}
	res = malloc((token_count + 1) * sizeof(char *));
	res[token_count] = NULL;
	i = 0;
	while (token)
	{
		res[i++] = token->value;
		temp = token->next;
		free(token);
		token = temp;
	}
	return (res);
}
