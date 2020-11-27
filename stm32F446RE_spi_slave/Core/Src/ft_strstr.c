/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strstr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nboulaye <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2015/11/25 12:50:10 by nboulaye          #+#    #+#             */
/*   Updated: 2015/11/29 19:57:18 by nboulaye         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char				*ft_strstr(const char *s1, const char *s2)
{
	size_t			i;
	size_t			j;
	size_t			k;

	i = 0;
	j = 0;
	k = 0;
	if (!s2[i])
		return ((char *)&s1[i]);
	while (s1[i])
	{
		if (s1[i] == s2[j])
		{
			k = i;
			while (s1[k++] == s2[j++])
			{
				if (!s2[j])
					return ((char *)&s1[i]);
			}
		}
		j = 0;
		i++;
	}
	return (NULL);
}
