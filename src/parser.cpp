#define BOOST_SPIRIT_USE_PHOENIX_V3
#define BOOST_RESULT_OF_USE_DECLTYPE 1

#include <string>
#include <memory>
#include <exception>
#include <cstddef>

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_as.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/spirit/include/phoenix_bind.hpp>

#include "parser.hpp"
#include "ast.hpp"

namespace dachs {
namespace syntax {

using std::size_t;

namespace detail {
    template<class Iterator>
    auto position_of(Iterator begin, Iterator current)
    {
        size_t line = 1;
        size_t col = 1;
        for (; begin != current; ++begin) {
            if (*begin == '\n') {
                col = 1;
                line++;
                continue;
            }
            col++;
        }
        return std::make_pair(line, col);
    }
} // namespace detail

namespace qi = boost::spirit::qi;
namespace phx = boost::phoenix;
namespace ascii = boost::spirit::ascii;

using qi::_1;
using qi::_2;
using qi::_3;
using qi::_val;
using qi::lit;
using phx::bind;

// Helpers {{{
namespace detail {
    template<class NodeType>
    struct make_shared_impl {
        template<class... Holders>
        auto operator()(Holders &&... holders) const
        {
            return std::make_shared<NodeType>(std::forward<Holders>(holders)...);
        }
    };
} // namespace detail

template<class NodeType, class... Args>
auto make_shared(Args &&... args)
{
    return phx::bind(detail::make_shared_impl<NodeType>{}, std::forward<Args>(args)...);
}
// }}}

template<class Iterator>
class grammar : public qi::grammar<Iterator, ast::node::program(), ascii::blank_type /*FIXME*/> {
    template<class Value, class... Extra>
    using rule = qi::rule<Iterator, Value, ascii::blank_type, Extra...>;

public:
    grammar() : grammar::base_type(program)
    {
        // FIXME: Temporary
        program =
            (
                literal > (qi::eol | qi::eoi)
            )
            [
                _val = make_shared<ast::node_type::program>(_1)
            ];

        literal =
            (
                  ('\'' > qi::char_ > '\'')
                | qi::as_string[qi::lexeme['"' > *(qi::char_ - '"') > '"']]
                | qi::real_parser<double, qi::strict_real_policies<double>>()
                | integer_literal
                | array_literal
                | tuple_literal
            )
            [
                _val = make_shared<ast::node_type::literal>(_1)
            ];

        integer_literal =
            (
                  (qi::lexeme[qi::uint_ >> 'u']) | qi::int_
            )
            [
                _val = make_shared<ast::node_type::integer_literal>(_1)
            ];

        // FIXME: Temporary
        array_literal =
            (
                lit('[') >> ']'
            )
            [
                _val = make_shared<ast::node_type::array_literal>()
            ];

        // FIXME: Temporary
        tuple_literal =
            (
                '(' >> +lit(',') >> ')'
            )
            [
                _val = make_shared<ast::node_type::tuple_literal>()
            ];

        qi::on_error<qi::fail>
        (
            program,
            // qi::_2 : end of string to parse
            // qi::_3 : iterator at failed point
            // qi::_4 : what failed?
            std::cerr
                << phx::val( "Error: Expecting " )
                << qi::_4
                << phx::val( "\nhere:\n\"" )
                << phx::construct<std::string>( _3, _2 ) // TODO: get line and col from iterators
                << phx::val( "\"" )
                << std::endl
        );
    }

    ~grammar()
    {}

private:
    rule<ast::node::program()> program;
    rule<ast::node::literal()> literal;
    rule<ast::node::integer_literal()> integer_literal;
    rule<ast::node::array_literal()> array_literal;
    rule<ast::node::tuple_literal()> tuple_literal;
};

ast::ast parser::parse(std::string const& code)
{
    auto itr = std::begin(code);
    auto const end = std::end(code);
    grammar<decltype(itr)> spiritual_parser;
    ast::node::program root;

    if (!qi::phrase_parse(itr, end, spiritual_parser, ascii::blank, root) || itr != end) {
        auto const pos = detail::position_of(std::begin(code), itr);
        throw std::runtime_error{"line:" + std::to_string(pos.first) + ", col:" + std::to_string(pos.second)};
    }

    return {root};
}

} // namespace syntax
} // namespace dachs
