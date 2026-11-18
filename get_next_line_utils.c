/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aartyush <aartyush@student.42prague.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/12 15:43:23 by aartyush          #+#    #+#             */
/*   Updated: 2026/11/18 13:09:39 by aartyush         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

size_t	ft_strlen(const char *s)
{
	size_t	i;

	if (!s)
		return (0);
	i = 0;
	while (s[i])
		i++;
	return (i);
}

char	*ft_strchr(const char *s, int c)
{
	if (!s)
		return (NULL);
	while (*s)
	{
		if (*s == (char)c)
			return ((char *)s);
		s++;
	}
	if ((char)c == '\0')
		return ((char *)s);
	return (NULL);
}

char	*append_buffer(char *stash, size_t stash_len, char *buf, size_t buf_len)
{
	char	*str;
	size_t	i;
	size_t	j;

	str = (char *)malloc(stash_len + buf_len + 1);
	if (!str)
	{
		free(stash);
		return (NULL);
	}
	i = 0;
	while (i < stash_len)
	{
		str[i] = stash[i];
		i++;
	}
	j = 0;
	while (j < buf_len)
		str[i++] = buf[j++];
	str[i] = '\0';
	free(stash);
	return (str);
}
