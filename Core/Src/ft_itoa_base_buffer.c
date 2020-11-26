/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_itoa_base_buffer.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nboulaye <nboulaye@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2015/11/26 18:34:35 by nboulaye          #+#    #+#             */
/*   Updated: 2016/08/30 20:28:38 by nboulaye         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int		ft_itoa_base_buffer(long double val, int base, char *buf)
{
	int				nbdigit;
	int				nbdigit_ret;
	unsigned long	unsignn;

	if (base < 2 || base > 16)
		return (-1);
	unsignn = (val < 0) ? -val : val;
	nbdigit = (val < 0 && base == 10) ? 2 : 1;
	while ((unsignn = unsignn / base))
		nbdigit++;
	unsignn = (val < 0) ? -val : val;
	ft_bzero(buf, nbdigit + 1);
	nbdigit_ret = nbdigit;
	buf[--nbdigit] = (unsignn % base > 9) ? (unsignn % base + 'a' - 10)
		: (unsignn % base + '0');
	while ((unsignn = unsignn / base) > 0)
		buf[--nbdigit] = (unsignn % base > 9) ? (unsignn % base + 'a' - 10)
			: (unsignn % base + '0');
	if (val < 0 && base == 10)
		buf[0] = '-';
	return (nbdigit_ret);
}
