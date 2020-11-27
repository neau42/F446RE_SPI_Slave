/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   read_arg.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nboulaye <nboulaye@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/04/27 16:25:34 by nboulaye          #+#    #+#             */
/*   Updated: 2016/08/31 13:56:43 by nboulaye         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

static void	init_flag(t_flags *flag)
{
	flag->f_char = 0;
	flag->f_length = 0;
	flag->f_precision = -1;
	flag->f_lenmin = 0;
}

void		read_next_arg(t_buf *b, char **format, va_list *pa)
{
	t_flags		flag;

	init_flag(&flag);
	b->flag = &flag;
	chk_flags(format, b->flag, pa);
	if (**format == 'c' || **format == 'C')
		do_c(b, pa, **format);
	else if (**format == 'b' || **format == 'B')
		do_b(b, pa, **format);
	else if (**format == 'd' || **format == 'i' || **format == 'D')
		do_d(b, pa, **format);
	else if (**format == 'u' || **format == 'U')
		do_u(b, pa, **format);
	else if (**format == 'o' || **format == 'O')
		do_o(b, pa, **format);
	else if (**format == 'x' || **format == 'X')
		do_x(b, pa, **format);
	else if (**format == 'p')
		do_p(b, pa);
	else if (**format == 'f' || **format == 'F')
		do_f(b, va_arg(*pa, double), **format);
	else if (**format == 's' || **format == 'S')
		do_s(b, pa, **format);
	else
		do_not_specifier(b, **format);
}
