/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   libft.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: no <no@42.fr>                              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2015/11/29 16:49:16 by nboulaye          #+#    #+#             */
/*   Updated: 2018/05/02 03:02:38 by no               ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBFT_H
# define LIBFT_H

# include <string.h>

long int	ft_atoi(const char *src);
void		ft_bzero(void *s, size_t n);
int			ft_isdigit(int c);
int			ft_isprint(int c);
int			ft_itoa_base_buffer(long double val, int base, char *buf);
void		*ft_memcpy(void *dst, const void *str, size_t n);
void		*ft_memset(void *s, int c, size_t n);
size_t		ft_strlen(const char *str);
char		*ft_strstr(const char *s1, const char *s2);
char		*ft_strtolower(char *str);
char		*ft_strncat(char *s1, const char *s2, size_t n);
int			ft_tolower(int c);
int			ft_toupper(int c);
int			ft_abs(int i);
void		ft_capitaliz(char *str);

#endif
