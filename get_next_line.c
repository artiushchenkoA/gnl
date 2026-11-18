/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aartyush <aartyush@student.42prague.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/12 15:41:49 by aartyush          #+#    #+#             */
/*   Updated: 2026/11/18 13:35:04 by aartyush         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

static char	*free_and_null(char **ptr)
{
	if (ptr && *ptr)
	{
		free(*ptr);
		*ptr = NULL;
	}
	return (NULL);
}

static char	*read_and_store(int fd, char *stash)
{
	char	*buffer;
	ssize_t	bytes_read;
	size_t	stash_len;

	if (ft_strchr(stash, '\n'))
		return (stash);
	buffer = (char *)malloc(sizeof(char) * (BUFFER_SIZE + 1));
	if (!buffer)
		return (free_and_null(&stash));
	stash_len = ft_strlen(stash);
	while (1)
	{
		bytes_read = read(fd, buffer, BUFFER_SIZE);
		if (bytes_read <= 0)
			break ;
		buffer[bytes_read] = '\0';
		stash = append_buffer(stash, stash_len, buffer, bytes_read);
		if (!stash || ft_strchr(buffer, '\n'))
			break ;
		stash_len += bytes_read;
	}
	free(buffer);
	if (bytes_read == -1)
		return (free_and_null(&stash));
	return (stash);
}

static char	*extract_line(char *stash)
{
	char	*line;
	size_t	len;
	size_t	i;

	if (!stash || !stash[0])
		return (NULL);
	len = 0;
	while (stash[len] && stash[len] != '\n')
		len++;
	if (stash[len] == '\n')
		len++;
	line = (char *)malloc(sizeof(char) * (len + 1));
	if (!line)
		return (NULL);
	i = 0;
	while (i < len)
	{
		line[i] = stash[i];
		i++;
	}
	line[i] = '\0';
	return (line);
}

static char	*reposition_stash(char *stash)
{
	char	*new_stash;
	size_t	i;
	size_t	j;

	i = 0;
	while (stash[i] && stash[i] != '\n')
		i++;
	if (!stash[i] || !stash[i + 1])
		return (free_and_null(&stash));
	new_stash = (char *)malloc(sizeof(char) * (ft_strlen(stash) - i));
	if (!new_stash)
		return (free_and_null(&stash));
	i++;
	j = 0;
	while (stash[i])
		new_stash[j++] = stash[i++];
	new_stash[j] = '\0';
	free(stash);
	return (new_stash);
}

char	*get_next_line(int fd)
{
	static char	*stash;
	char		*line;

	if (fd < 0 || BUFFER_SIZE <= 0)
		return (free_and_null(&stash));
	stash = read_and_store(fd, stash);
	if (!stash)
		return (NULL);
	line = extract_line(stash);
	if (!line)
	{
		free_and_null(&stash);
		return (NULL);
	}
	stash = reposition_stash(stash);
	return (line);
}
