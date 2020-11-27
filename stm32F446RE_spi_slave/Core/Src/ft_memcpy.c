/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memcpy.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nboulaye <nboulaye@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2015/11/25 16:24:10 by nboulaye          #+#    #+#             */
/*   Updated: 2018/05/01 09:59:23 by nboulaye         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void		*ft_fast_memcpy(void *dst, const void *src, size_t n)
{
	size_t			*d;
	const size_t	*s;
	size_t			i;
	size_t			len;

	d = dst;
	s = src;
	i = 0;
	len = n / sizeof(size_t);
	while (i < len)
	{
		d[i] = s[i];
		i++;
	}
	return (dst);
}

void		*ft_memcpy(void *dst, const void *src, size_t n)
{
	char		*d;
	const char	*s;
	size_t		i;

	if ((size_t)dst % sizeof(size_t) == 0 &&
			(size_t)src % sizeof(size_t) == 0 && n % sizeof(size_t) == 0)
		return (ft_fast_memcpy(dst, src, n));
	i = 0;
	d = dst;
	s = src;
	while (i < n)
	{
		d[i] = s[i];
		i++;
	}
	return (dst);
}
