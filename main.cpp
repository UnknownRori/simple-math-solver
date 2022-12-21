#include <iostream>
#include <stack>
#include <vector>
#include <sstream>
#include <tuple>
#include <string>

#define EXIT_SUCCESS 0

/**
 * @brief Definition of custom type
 *
 */
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef double f32;
typedef long double f64;

/**
 * @brief Define Type of Token
 *
 */
enum TokenType
{
    Number,
    Plus,
    Subtract,
    Multiply,
    Divide,
    OpenParenthesis,
    CloseParenthesis
};

/**
 * @brief Define the type of Error
 *
 */
enum ErrorKind
{
    None,
    SyntaxError,
    ParseIntError,
};

class Token
{
private:
    TokenType m_type;
    i32 m_precedence;
    i64 m_value;

public:
    Token() = default;
    Token(TokenType type, i64 value) : m_type(type), m_value(value) {}
    Token(TokenType type) : m_type(type) {}
    Token(TokenType type, i32 precedence) : m_type(type), m_precedence(precedence) {}

    const TokenType &get_token() const { return this->m_type; }
    const i64 &get_value() const { return this->m_value; }
    const i32 &get_precedence() const { return this->m_precedence; }

    friend std::ostream &operator<<(std::ostream &os, const Token &token)
    {
        if (token.m_type == TokenType::Number)
            os << " '" << token.m_value << "' ";
        else if (token.m_type == TokenType::Plus)
            os << " '+' ";
        else if (token.m_type == TokenType::Subtract)
            os << " '-' ";
        else if (token.m_type == TokenType::Multiply)
            os << " '*' ";
        else if (token.m_type == TokenType::Divide)
            os << " '/' ";
        else if (token.m_type == TokenType::OpenParenthesis)
            os << " '(' ";
        else if (token.m_type == TokenType::CloseParenthesis)
            os << " ')' ";

        return os;
    }
};

class MathSolver
{
public:
    MathSolver() = default;

    /**
     * @brief Evaluate Math Expressions
     *
     * @param __src
     * @return i64
     */
    auto evaluate(const std::string &__src) -> i64
    {
        auto tokenized = this->tokenizer(__src);

        std::cout << "Tokenizing " << __src << std::endl;
        for (auto &&i : tokenized)
        {
            std::cout << i << std::endl;
        }

        auto [res, err] = this->parse(tokenized);

        std::cout << "Evaluating : \n";
        while (!res.empty())
        {
            std::cout << res.top() << std::endl;
            res.pop();
        }

        return -1;
    }

private:
    /**
     * @brief Split the string into tokens
     *
     * @param __src
     * @return std::vector<std::Token>
     */
    auto tokenizer(const std::string &__src) -> std::vector<Token>
    {
        std::vector<std::string> result;

        std::stringstream ss(__src);

        std::string temp;
        while (std::getline(ss, temp, ' '))
        {
            if (temp != "")
                result.push_back(temp);
        }

        std::vector<Token> tokens;
        for (const auto &token : result)
        {
            auto [num, err] = this->parse_int(token);
            if (err != ErrorKind::ParseIntError)
            {
                tokens.push_back(Token(TokenType::Number, num));
                continue;
            }

            if (token == "+")
                tokens.push_back(Token(TokenType::Plus, 1));
            else if (token == "-")
                tokens.push_back(Token(TokenType::Subtract, 1));
            else if (token == "*")
                tokens.push_back(Token(TokenType::Multiply, 2));
            else if (token == "/")
                tokens.push_back(Token(TokenType::Divide, 2));
            else if (token == "(")
                tokens.push_back(Token(TokenType::OpenParenthesis));
            else if (token == ")")
                tokens.push_back(Token(TokenType::CloseParenthesis));
        }

        return tokens;
    }

    /**
     * @brief Convert tokenized string into tokens array using Shunting yard algorithm
     *
     * @param __src
     * @param __dst
     * @return const std::tuple<std::stack<Token>, ErrorKind>
     */
    auto parse(const std::vector<Token> &__src) -> std::tuple<std::stack<Token>, ErrorKind>
    {
        std::stack<Token> operator_stack;
        std::stack<Token> output;
        std::tuple<std::stack<Token>, ErrorKind> ret(output, ErrorKind::SyntaxError);

        std::cout << "Parsing\n";
        for (auto &token : __src)
        {
            std::cout << token << "\n";
            if (token.get_token() == TokenType::Number)
            {
                output.push(token);
                continue;
            }

            if (token.get_token() == TokenType::OpenParenthesis)
            {
                operator_stack.push(token);
                continue;
            }

            if (token.get_token() == TokenType::CloseParenthesis)
            {
                while (true)
                {
                    if (operator_stack.top().get_token() != TokenType::OpenParenthesis)
                    {

                        output.push(operator_stack.top());
                        operator_stack.pop();
                    }
                    else
                    {
                        operator_stack.pop();
                        break;
                    }
                }
                continue;
            }

            while (!operator_stack.empty())
            {
                if (operator_stack.top().get_token() != TokenType::OpenParenthesis && token.get_precedence() < operator_stack.top().get_precedence())
                {

                    output.push(operator_stack.top());
                    operator_stack.pop();
                }
                else
                {
                    break;
                }
            }

            operator_stack.push(token);
        }

        std::cout << "Put all into Out\n";
        while (!operator_stack.empty())
        {
            std::cout << operator_stack.top() << std::endl;
            output.push(operator_stack.top());
            operator_stack.pop();
        }

        std::get<0>(ret) = output;
        std::get<1>(ret) = ErrorKind::None;

        return ret;
    }

    /**
     * @brief Parse string into a i64
     *
     * @param __src
     * @return std::tuple<i64, ErrorKind>
     */
    auto parse_int(const std::string &__src) -> std::tuple<i64, ErrorKind>
    {
        try
        {
            i64 result = std::stoll(__src);
            std::tuple<i64, ErrorKind> ret(result, ErrorKind::None);
            return ret;
        }
        catch (std::invalid_argument &err)
        {
            std::tuple<i64, ErrorKind> ret(-1, ErrorKind::ParseIntError);
            return ret;
        }
    }
};

static inline auto input(std::string &__dst, const char *__msg) -> void;

int main(int argc, char **argv)
{
    std::string buffer;

    std::cout << "=== Simple Math Solver ===\n";

    input(buffer, ">> ");

    // if (buffer == "exit" || buffer == "Exit")

    auto solver = MathSolver();
    auto result = solver.evaluate(buffer);

    std::cout << "Result\t: " << result << std::endl;

    return EXIT_SUCCESS;
}

static inline auto input(std::string &__dst, const char *__msg) -> void
{
    std::cout << __msg;
    std::getline(std::cin, __dst);
}
