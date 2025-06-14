#pragma once

#include <kernel/Printer.h>
#include <kernel/tty.h>

#define kprint		Printer::print<TTY::putchar>
#define kprintln	Printer::println<TTY::putchar>

#if 0

#include <stdint.h>
#include <string.h>

template<typename T>
static void kprint_signed(T value)
{
	if (value == 0)
	{
		terminal_putchar('0');
		return;
	}

	char  buffer[32];
	char* ptr = buffer + sizeof(buffer);
	bool  sign = false;
	
	if (value < 0)
	{
		sign = true;
		*(--ptr) = ((10 - (value % 10)) % 10) + '0';
		value = -(value / 10);
	}

	while (value)
	{
		*(--ptr) = (value % 10) + '0';
		value /= 10;
	}
	if (sign)
		*(--ptr) = '-';

	terminal_write(ptr, sizeof(buffer) - (ptr - buffer));
}

template<typename T>
static void kprint_unsigned(T value)
{
	if (value == 0)
	{
		terminal_putchar('0');
		return;
	}

	char  buffer[32];
	char* ptr = buffer + sizeof(buffer);

	while (value)
	{
		*(--ptr) = (value % 10) + '0';
		value /= 10;
	}

	terminal_write(ptr, sizeof(buffer) - (ptr - buffer));
}

template<typename T>
static void kprint_floating(T value, int percision)
{
	uint64_t int_part = (uint64_t)value;
	T frac_part = value - (T)int_part;

	kprint_signed(int_part);

	terminal_write(".", 1);

	while (percision > 0)
	{
		frac_part *= 10;
		if (percision == 1)
			frac_part += 0.5;
		char digit = (uint8_t)frac_part % 10 + '0';
		terminal_write(&digit, 1);
		percision--;
	}
}

template<typename T>
static void kprint_val(T);

static void kprint(const char* format)
{
	terminal_writestring(format);
}

template<typename Arg, typename... Args>
static void kprint(const char* format, Arg arg, Args... args)
{
	const char* next = strstr(format, "{}");
	if (next == NULL)
	{
		terminal_write(format, strlen(format));
		return;
	}
	terminal_write(format, next - format);
	kprint_val<Arg>(arg);
	kprint(next + 2, args...);
}

template<> void kprint_val(short     int value)				{ kprint_signed(value); }
template<> void kprint_val(          int value)				{ kprint_signed(value); }
template<> void kprint_val(long      int value)				{ kprint_signed(value); }
template<> void kprint_val(long long int value)				{ kprint_signed(value); }

template<> void kprint_val(unsigned short     int value)	{ kprint_unsigned(value); }
template<> void kprint_val(unsigned           int value)	{ kprint_unsigned(value); }
template<> void kprint_val(unsigned long      int value)	{ kprint_unsigned(value); }
template<> void kprint_val(unsigned long long int value)	{ kprint_unsigned(value); }

template<> void kprint_val(float value)						{ kprint_floating(value, 3); }
template<> void kprint_val(double value)					{ kprint_floating(value, 3); }
template<> void kprint_val(long double value)				{ kprint_floating(value, 3); }

template<> void kprint_val(         char value)				{ terminal_putchar(value); }
template<> void kprint_val(signed   char value)				{ kprint_signed(value); }
template<> void kprint_val(unsigned char value)				{ kprint_unsigned(value); }

template<> void kprint_val(const char* value)				{ terminal_writestring(value); }
template<> void kprint_val(char*       value)				{ terminal_writestring(value); }

static char bits_to_hex(uint8_t val)
{
	val = val & 0xF;
	if (val < 10)
		return val + '0';
	return val + 'a' - 10;
}

template<> void kprint_val(void* value)
{
	terminal_write("0x", 2);
	uint32_t addr = (uint32_t)value;
	for (int i = sizeof(void*) * 8 - 4; i >= 0; i -= 4)
		terminal_putchar(bits_to_hex(addr >> i));
}

#endif