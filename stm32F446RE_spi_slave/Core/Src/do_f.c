/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   do_f.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nboulaye <nboulaye@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/08/22 03:17:58 by nboulaye          #+#    #+#             */
/*   Updated: 2016/08/31 15:42:53 by nboulaye         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

static void	add_float_in_buf(int t[4], t_buf *b, char *buf, int i)
{
	double		reste;

	reste = 0.0;
	while (t[1]++ <= 23 && (i *= 2))
		if (t[0] & (0x1 << (23 - t[1])))
			reste += ((float)(1.0 / i));
	if (!(b->flag->f_length & F_L_MOINS))
		while (--b->flag->f_lenmin > (b->flag->f_precision + t[2] + t[3]))
			(b->flag->f_length & F_L_ZERO) ? add_in_buf(b, '0') :
			add_in_buf(b, ' ');
	if (t[3] && (--b->flag->f_lenmin || 1))
		add_in_buf(b, '-');
	while (*buf && (--b->flag->f_lenmin || 1))
		add_in_buf(b, *(buf++));
	add_in_buf(b, '.');
	while (b->flag->f_precision-- > 0 && (--b->flag->f_lenmin || 1))
	{
		reste *= 10.0;
		add_in_buf(b, (((int)reste) + '0'));
		reste -= (int)reste;
	}
	while (b->flag->f_lenmin-- > 0)
		(b->flag->f_length & F_L_ZERO) ? add_in_buf(b, '0') :
		add_in_buf(b, ' ');
}

/*
**	int[0]		t[0] == mantisse;
**	int[1]		t[1] == EXPO;
**	int[2]		t[2] == nbdigit_entier;
**	int[2]		t[3] == isneg;
*/

void		do_f(t_buf *b, float fl, char type)
{
	char	buf[32];
	double	entier;
	int		t[4];

	(void)type;
	if (b->flag->f_precision < 0)
		b->flag->f_precision = 6;
	ft_memcpy(&t[0], &fl, (sizeof(double)));
	t[3] = (t[0] & 0x80000000) ? 1 : 0;
	t[1] = ((t[0] & 0X7F800000) >> 23) - 0x7F;
	t[0] = t[0] - (t[0] & 0xFF800000) + 0x800000;
	entier = (t[0]) >> (23 - t[1]);
	t[2] = ft_itoa_base_buffer(entier, 10, buf);
	add_float_in_buf(t, b, buf, 1);
}
