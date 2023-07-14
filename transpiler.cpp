#include "transpiler.hpp"

#include <sstream>
#include <string>
#include <vector>

#include <fmt/core.h>

// type coerce the empty array into a number with + false with !, then negate that, then turn true into a number
const std::string Transpiler::ZERO = "+[]";
// type coerce the empty array into boolean false with !, then negate that,
// then turn true into a number
const std::string Transpiler::ONE  = "+!![]";

[[nodiscard]]
auto Transpiler::FromNumber(int number) -> std::string
{
    if (number == 0)
    {
        return ZERO;
    }
    if (number == 1)
    {
        return ONE;
    }

    // result is stored in here
    std::string result;

    if (number >= (1 << 4))
    {
        // add and multiply algorithm
        // transform the number to the form of
        // 2^6+2^5 + 1 + 1 + 1
        // code gets larger with this method for numbers < 16
        bool first = true;
        while (number != 0)
        {
            // get highest bit in number
            int highestBit = 0;
            while ((number >> ++highestBit) > 0)
            {
                /* empty */
            }
            highestBit--;

            // subtract the highest bit from the number;
            int mask = ~(1 << highestBit);
            number &= mask;
            [[likely]]
            if (!first)
            {
                result.append("+");
            }
            first = false;

            result.append("((");
            result.append(FromNumber(2));
            result.append(")**(");
            result.append(FromNumber(highestBit));
            result.append("))");
        }
    }
    else
    {
        for (unsigned int i = 0; i < static_cast<unsigned int>(number); i++)
        {
            [[likely]]
            if (i != 0)
            {
                result.append(std::string("+ ") + ONE);
            }
            else
            {
                result.append(ONE);
            }
        }
    }
    return result;
}


auto Transpiler::FromString(const std::string& str) const -> std::string
{
    std::vector<std::string> tokens;
    for (const char CHR : str)
    {
        if (m_map.contains(CHR))
        {
            tokens.push_back(m_map.at(CHR));
        }
        else if (m_cacheMap.contains(CHR))
        {
            tokens.push_back(m_cacheMap.at(CHR));
        }
        else
        {
            std::string charRepr;
            // `([]+[])['constructor']['fromCharCode'](charCode)`

            // []+[] evaluates to empty string
            charRepr.append("([]+[])");

            // get toString() function with ['constructor']
            charRepr.append("[");
            charRepr.append(FromString("constructor"));
            charRepr.append("]");

            // get the fromCharCode function
            charRepr.append("[");
            charRepr.append(FromString("fromCharCode"));
            charRepr.append("]");

            // call the function with our charCode
            charRepr.append("(");
            charRepr.append(FromNumber(static_cast<int>(CHR)));
            charRepr.append(")");

            tokens.push_back(charRepr);
            m_cacheMap.insert({CHR, charRepr});
        }
    }
    std::ostringstream result;
    bool               first = true;
    for (const auto& token : tokens)
    {
        [[unlikely]]
        if (first)
        {
            result << token;
            first = false;
        }
        else
        {
            result << "+" << token;
        }
    }
    return result.str();
}

auto Transpiler::Transpile() const -> std::string
{
    std::string result;
    // `(()=>{})[${fromString('constructor')}](${fromString(code)})()`;
    result.append("(()=>{})");    // an empty function

    // append the indexing string ['constructor'] to the empty function
    result.append("[");
    result.append(FromString("constructor"));
    result.append("]");

    // append the constructor argument, which is our code
    result.append("(");
    result.append(FromString(m_inputCode));
    result.append(")");

    // call that function we just constructed to execute our code
    result.append("()");
    return result;
}
