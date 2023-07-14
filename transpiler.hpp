#pragma once

#include <map>
#include <string>
#include <type_traits>
#include <utility>

#include <fmt/core.h>

/*! \class Transpiler
 *  \brief Takes JavaScript code and turns it into garbage that still runs in a JS interpreter the same way
 *
 *  Turns JavaScript into code that consists only of {, (, ), +, !, [, ],=, >, /, - and } and still runs the same.
 */
class Transpiler
{
  public:
    template <typename T>
        requires std::is_convertible_v<T, std::string>
    Transpiler(T&& inputCode) noexcept : m_inputCode{std::forward<T>(inputCode)}
    {
        using fmt::format;
        // adding an empty array to almost anything in JS results in that thing to be turned into a string

        // This is BS. Of course there will be magic numbers to index that crap.
        // NOLINTBEGIN(readability-magic-numbers, cppcoreguidelines-avoid-magic-numbers)

        // from "(+{})" which evaluates to NaN
        auto lambdaNan = [](int x) { return format("(+{{}}+[])[{}]", FromNumber(x)); };
        m_map.insert({'N', lambdaNan(0)});
        m_map.insert({'a', lambdaNan(1)});

        // from "({}+[])" which evaluates to "[object Object]"
        auto lambdaObjectObject = [](int x) { return format("({{}}+[])[{}]", FromNumber(x)); };
        m_map.insert({'o', lambdaObjectObject(1)});
        m_map.insert({'b', lambdaObjectObject(2)});
        m_map.insert({'j', lambdaObjectObject(3)});
        m_map.insert({'e', lambdaObjectObject(4)});
        m_map.insert({'c', lambdaObjectObject(5)});
        m_map.insert({'t', lambdaObjectObject(6)});
        m_map.insert({' ', lambdaObjectObject(7)});
        m_map.insert({'O', lambdaObjectObject(8)});
        m_map.insert({'[', lambdaObjectObject(0)});
        m_map.insert({']', lambdaObjectObject(14)});

        // from "(![]+[])" which evaluates to "false"
        auto lambdaFalse = [](int x) { return format("(![]+[])[{}]", FromNumber(x)); };
        m_map.insert({'f', lambdaFalse(0)});
        m_map.insert({'l', lambdaFalse(2)});
        m_map.insert({'s', lambdaFalse(3)});

        // from "(!![]+[])" which evaluates to true
        auto lambdaTrue = [](int x) { return format("(!![]+[])[{}]", FromNumber(x)); };
        m_map.insert({'r', lambdaTrue(1)});
        m_map.insert({'u', lambdaTrue(2)});

        // from "((+!![]/+[])+[])" which evaluates to 1/0 + [], which results in "Infinity"
        auto lambdaInfinity = [](int x) { return format("((+!![]/+[])+[])[{}]", FromNumber(x)); };
        m_map.insert({'I', lambdaInfinity(0)});
        m_map.insert({'n', lambdaInfinity(1)});
        m_map.insert({'i', lambdaInfinity(3)});
        m_map.insert({'y', lambdaInfinity(7)});

        // with what we have so far we can spell "constructor"

        // from "([]+([]+[])['constructor'])" which evaluates to
        // "function String() {\n    [native code]\n}"
        // anything after () is unreliable.
        auto lambdaFunctionStr = [this](int x)
        { return format("([]+([]+[])[{}])[{}]", this->FromString("constructor"), FromNumber(x)); };
        m_map.insert({'S', lambdaFunctionStr(9)});
        m_map.insert({'g', lambdaFunctionStr(14)});
        m_map.insert({'(', lambdaFunctionStr(15)});
        m_map.insert({')', lambdaFunctionStr(16)});

        // from "([]+(/-/)['constructor'])" which evaluates to
        // "function RegExp() {\n    [native code]\n}"
        // anything after () is unreliable.
        auto lambdaRegExp = [this](int x)
        { return format("([]+(/-/)[{}])[{}]", this->FromString("constructor"), FromNumber(x)); };
        m_map.insert({'R', lambdaRegExp(9)});
        m_map.insert({'E', lambdaRegExp(12)});
        m_map.insert({'x', lambdaRegExp(13)});
        m_map.insert({'p', lambdaRegExp(14)});

        // from "(/\\/+[])" which evaluates to "/\/"
        auto lambdaSlashes = [](int x) { return format("(/\\\\/+[])[{}]", FromNumber(x)); };
        m_map.insert({'/', lambdaSlashes(0)});
        m_map.insert({'\\', lambdaSlashes(1)});

        // From "(#)['toString'](#+1)", which turns a number into a string in base #+1
        // We want to use the minimum base to reduce code size.
        // for example 13 in base 14 is "d"
        auto lambdaNumberBase = [this](int x)
        { return format("({})[{}]({})", FromNumber(x), this->FromString("toString"), FromNumber(x + 1)); };
        m_map.insert({'d', lambdaNumberBase(13)});
        m_map.insert({'h', lambdaNumberBase(17)});
        m_map.insert({'m', lambdaNumberBase(22)});

        // we can now spell "fromString" and "return escape"

        // from "((()=>{})['constructor']('return escape')()('\\'))" which evaluates to "%5C"
        // This returns the escape sequence for backslash. sadly this involves creating a function from string
        // which modern browsers do not like because it is insecure.
        // It still works in node though.
        auto lambdaBsEscape = [this](int x)
        {
            return format(
              "((()=>{{}})[{}]({})()({}))[{}]",
              this->FromString("constructor"),
              this->FromString("return escape"),
              this->FromString("\\"),
              FromNumber(x)
            );
        };
        m_map.insert({'%', lambdaBsEscape(0)});
        m_map.insert({'C', lambdaBsEscape(2)});

        // we can now spell "fromCharCode", which is all we need.

        // NOLINTEND(readability-magic-numbers, cppcoreguidelines-avoid-magic-numbers)
    }

    Transpiler()                                      = delete;     // Can't construct without code input
    virtual ~Transpiler()                             = default;    // future proofing
    Transpiler(const Transpiler&)                     = delete;     // covered by Forwarding constructor
    Transpiler(Transpiler&&)                          = delete;     // -/-
    auto operator= (const Transpiler&) -> Transpiler& = default;    // Copy operator=
    auto operator= (Transpiler&&) -> Transpiler&      = default;    // move operator=

    /**
     * @brief Turns a character into a string of bullshit that evaluates to the same character in JS
     *
     * @return A bullshit string that in JS evaluates the same way as the code does.
     */
    [[nodiscard]]
    auto Transpile() const -> std::string;

  protected:
    /**
     * @brief Turns a number into a string of bullshit that evaluates to that same number in JS
     *
     * @param number The number to be converted
     *
     * @return A javascript string that evaluates into the number when executed
     */
    [[nodiscard]]
    static auto FromNumber(int number) -> std::string;


    /**
     * @brief Turns a string into JS equivalent bullshit
     *
     * @param str The string to turn into BS
     *
     * @return A string of BS that evaluates to the input string in JS
     */
    [[nodiscard]]
    auto FromString(const std::string& str) const -> std::string;
  private:
    std::string                 m_inputCode; /*!< Code passed to the constructor */

    static const std::string    ZERO;        /*!< A bullshit string that evaluates to 0 in JS */
    static const std::string    ONE;         /*!< A bullshit string that evaluates to 1 in JS */
    std::map<char, std::string> m_map;
    std::map<char, std::string> mutable m_cacheMap;
};
