
// vi: tabstop=4:expandtab

#include "utils/strprintf.hh"

/// A simple scoped array class
struct SimpleScopedArray
{
        SimpleScopedArray(int len)
        : buffer((char *)malloc(len))
        {
        }

        ~SimpleScopedArray()
        {
                free((void *)buffer);
        }

        void realloc(int len)
        {
                buffer = (char *)::realloc(buffer, len);
        }

        char *buffer;
};

std::string utils::strvprintf(const char *fmt, va_list va)
{
        va_list args;
        int len = 128;
        SimpleScopedArray helper(len);

        while(1) {
        va_copy(args, va);
                int n = vsnprintf(helper.buffer,len,fmt,args);
        va_end (args);

                if(n > -1 and n < len) {
                        break;
                }

                if(n > -1) {
                        len = n+1;
                } else {
                        len = 2*len;
                }

                helper.realloc(len);
        }

        return std::string(helper.buffer);
}

std::string utils::strprintf (const char *fmt, ...)
{
    va_list  args;
    va_start(args, fmt);
    std::string s = strvprintf(fmt, args);
    va_end  (args);

    return s;
}

