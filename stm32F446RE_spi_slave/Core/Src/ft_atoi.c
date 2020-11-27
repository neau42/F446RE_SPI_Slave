/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atoi.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nboulaye <nboulaye@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2015/11/24 22:32:21 by nboulaye          #+#    #+#             */
/*   Updated: 2017/02/20 22:02:19 by nboulaye         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

long int		ft_atoi(const char *str)
{
	unsigned long int	absnbr;
	char				invers;

	absnbr = 0;
	invers = 0;
	while (*str == ' ' || (*str <= 13 && *str >= 9))
		str++;
	if (*str == '-' || *str == '+')
	{
		if (*str == '-')
			invers = 1;
		str++;
	}
	while (*str >= '0' && *str <= '9')
	{
		absnbr *= 10;
		absnbr += *str - 48;
		str++;
	}
	return (invers == 0) ? (absnbr) : (-absnbr);
}
