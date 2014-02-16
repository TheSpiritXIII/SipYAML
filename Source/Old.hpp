/*!
 *	Returns the encoding size that the indicated character takes to represent as
 *	UTF-8. 
**/
uint8_t utf8_encoding_size(int8_t *c)
{
	if (!((*c) & 0x80))
	{
		return 0;
	}
	if (!((*c) & 0x20))
	{
		return 1;
	}
	if (!((*c) & 0x10))
	{
		return 2;
	}
	if (!((*c) & 0x08))
	{
		return 3;
	}
}

/*!
 *	Returns the encoding size that the indicated character takes to represent as
 *	UTF-16 in little endian.
**/
inline uint8_t utf16_encoding_size_le(int16_t *c)
{
	return ((*c) & 0x8000);
}

/*!
 *	Returns the encoding size that the indicated character takes to represent as
 *	UTF-16 mode in big endian.
**/
inline uint8_t utf16_encoding_size_be(int16_t *c)
{
	return ((*c) & 0x0080);
}

/*!
 *	Returns the encoding size that the indicated character takes to represent as
 *	UTF-32 mode in little endian.
**/
inline uint8_t utf32_encoding_size_le(int32_t *c)
{
	return ((*c) & 0x80000000);
}

/*!
 *	Returns the encoding size that the indicated character takes to represent as
 *	UTF-32 mode in big endian.
**/
inline uint8_t utf32_encoding_size_le(int32_t *c)
{
	return ((*c) & 0x00008000);
}