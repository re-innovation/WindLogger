/*
 * utility.cpp
 *
 * Application utility functionality for Wind Data logger
 *
 * Matt Little/James Fowkes
 * August 2015
 */

/************ External Libraries*****************************/
#include <Arduino.h>

/************ Application Libraries*****************************/
#include "utility.h"

/* 
 * Defines
 */

#define MAX_STRING 130      // Sets the maximum length of string probably could be lower

/* 
 * Private Variables
 */

static char s_progmemBuffer[MAX_STRING];  // A buffer to hold the string when pulled from program memory


/* 
 * Public Functions
 */

/***************************************************
 *  Name:        DecToBcd
 *
 *  Returns:     BCD value
 *
 *  Parameters:  Decimal value
 *
 *  Description: Turns decimal value into BCD encoded value (e.g 12 => 0x12)
 *
 ***************************************************/
byte DecToBcd(byte value)
{
  return (value / 10 * 16 + value % 10);
}

/***************************************************
 *  Name:        PStringToRAM
 *
 *  Returns:     Pointer to local string buffer
 *
 *  Parameters:  Pointer to string in PROGMEM.
 *
 *  Description: Copies string from flash to RAM and returns pointer to the RAM copy
 *
 ***************************************************/
char* PStringToRAM(const char* str) {
	strcpy_P(s_progmemBuffer, str);
	return s_progmemBuffer;
}

/* FixedLengthAccumulator class 
 * Copied from Datalogger project (https://github.com/re-innovation/DataLogger)
 */

/*
 * FixedLengthAccumulator::FixedLengthAccumulator
 *
 * Init the accumulator using a buffer and length of that buffer (INCLUDING space for terminating '\0')
 */

FixedLengthAccumulator::FixedLengthAccumulator(char * buffer, uint16_t length)
{
    if (buffer)
    {
        attach(buffer, length);
    }
    else
    {
        detach();
    }
}

FixedLengthAccumulator::~FixedLengthAccumulator() {}

/*
 * FixedLengthAccumulator::writeChar
 *
 * If there is space in the buffer, add the char to the string
 * Returns true if char was written, false if not
 */

bool FixedLengthAccumulator::writeChar(char c)
{
    if (m_buffer && m_writeIndex < m_maxLength)
    {
        m_buffer[m_writeIndex++] = c;
        m_buffer[m_writeIndex] = '\0';
        return true;
    }
    return false;
}

/*
 * FixedLengthAccumulator::writeString
 *
 * Copies chars from s until s is exhausted or the accumulator is full
 * Therefore, this function may only copy a partial length of s
 * Returns true if ALL of s was copied;
 */

bool FixedLengthAccumulator::writeString(const char * s)
{
    if (!s) { return false; }
    
    while(*s && (m_writeIndex < m_maxLength))
    {
        m_buffer[m_writeIndex++] = *s++;
        m_buffer[m_writeIndex] = '\0';
    }
    
    return (*s == '\0');
}

/*
 * FixedLengthAccumulator::writeLine
 *
 * As per writeString, but appends "\r\n" in addition to copying from s
 */

bool FixedLengthAccumulator::writeLine(const char * s)
{
    bool success = true;
    success &= writeString(s);
    success &= writeString("\r\n");
    return success;
}

/*
 * FixedLengthAccumulator::reset
 *
 * Makes the buffer appear to be an empty string
 */

void FixedLengthAccumulator::reset(void)
{
    m_writeIndex = 0;
    m_buffer[m_writeIndex] = '\0';
}

/*
 * FixedLengthAccumulator::c_str
 *
 * Returns pointer to the actual buffer
 */

char * FixedLengthAccumulator::c_str(void)
{
    return m_buffer;
}

/*
 * FixedLengthAccumulator::attach
 *
 * Redirects the accumulator to point at a new buffer
 */

void FixedLengthAccumulator::attach(char * buffer, uint16_t length)
{
    if (buffer)
    {
        m_buffer = buffer;
        m_maxLength = length-1;
        reset();
    }
}

/*
 * FixedLengthAccumulator::full
 *
 * Returns true if the accumulator is full
 */
 
bool FixedLengthAccumulator::isFull(void)
{
    return m_writeIndex == m_maxLength;
}

/*
 * FixedLengthAccumulator::detach
 *
 * Leaves the accumulator floating (and therefore safe)
 */

void FixedLengthAccumulator::detach(void)
{
    m_buffer = NULL;
    m_maxLength = 0;
    m_writeIndex = 0;
}

/*
 * FixedLengthAccumulator::length
 *
 * Returns the current length of the written buffer based on write index
 */

uint16_t FixedLengthAccumulator::length(void)
{
    return m_writeIndex;
}

/*
 * FixedLengthAccumulator::remove
 *
 * Removes chars from the end of the buffer by placing a '\0' in the appropriate position
 */

void FixedLengthAccumulator::remove(uint32_t chars)
{
    if (chars > m_writeIndex)
    {
        m_writeIndex = 0;
    }
    else
    {
        m_writeIndex -= chars;
    }

    m_buffer[m_writeIndex] = '\0';
}

