/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: no <no@student.42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/04/27 16:09:45 by nboulaye          #+#    #+#             */
/*   Updated: 2020/11/26 15:57:47 by no               ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_PRINTF_H
# define FT_PRINTF_H

# include <unistd.h>
# include <stdarg.h>
# include <wchar.h>
# include <limits.h>
# include <locale.h>
# include <inttypes.h>
# include "stm32f4xx_hal.h"
# include "libft.h"

# define BUF_LEN 0x80
# define INIT_BUF_FT 0xFF01
# define INIT_BUF_FT_SPRINTF 0xFF02
# define RETURN_NB_FLUSH 0xFF03
# define REINIT_BUF_FT 0xFF04

# define F_C_H  0x01
# define F_C_HH 0x02
# define F_C_L  0x04
# define F_C_LL 0x08
# define F_C_J  0x10
# define F_C_Z  0x20

# define F_L_DIEZE 0x01
# define F_L_ZERO  0x02
# define F_L_PLUS  0x04
# define F_L_MOINS 0x08
# define F_L_SPACE 0x10

typedef struct	s_flags
{
	int				f_precision;
	int				f_lenmin;
	char			f_char;
	char			f_length;
}				t_flags;

typedef struct	s_buf
{
	char			*buf;
	int				i;
	t_flags			*flag;
}				t_buf;

int				add_in_buf(t_buf *b, int c);
int				chk_flags(char **format, t_flags *flag, va_list *pa);
void			read_next_arg(t_buf *b, char **format, va_list *pa);

void			do_d(t_buf *b, va_list *pa, char type);
void			do_f(t_buf *b, float fl, char type);
void			do_u(t_buf *b, va_list *pa, char type);
void			do_o(t_buf *b, va_list *pa, char type);
void			do_x(t_buf *b, va_list *pa, char type);
void			do_p(t_buf *b, va_list *pa);
void			do_c(t_buf *b, va_list *pa, char type);
void			do_long_s(t_buf *b, va_list *pa, char *buf);
void			do_s(t_buf *b, va_list *pa, char type);
void			do_not_specifier(t_buf *b, char type);
void			do_b(t_buf *b, va_list *pa, char type);

int				ft_printf(const char *format, ...);
int				ft_fdprintf(int fd, const char *format, ...);
int				ft_sprintf(char *str, const char *format, ...);

#endif
