#ifndef _UTILITY_H_
#define _UTILITY_H_

// Converts a decimal to BCD (binary coded decimal)
byte DecToBcd(byte value);
char* PStringToRAM(const char* str);


/*
 * FixedLengthAccumulator
 *
 * Copied from Datalogger project (https://github.com/re-innovation/DataLogger)
 * Wrapper for a char buffer to allow easy creation one char at a time.
 * More control than strncpy, less powerful than full-blown String class
 */

class FixedLengthAccumulator
{
    public:
        FixedLengthAccumulator(char * buffer, uint16_t length);
        ~FixedLengthAccumulator();
        bool writeChar(char c);
        bool writeString(const char * s);
        bool writeLine(const char * s);
    
        void remove(uint32_t chars);
                
        void reset(void);
        char * c_str(void);
        
        bool isFull(void);
        void attach(char * buffer, uint16_t length);
        void detach(void);
        uint16_t length(void);

    private:
        char * m_buffer;
        uint16_t m_maxLength;
        uint16_t m_writeIndex;
};

#endif