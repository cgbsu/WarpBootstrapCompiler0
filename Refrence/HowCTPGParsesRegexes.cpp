    template<typename Builder>
    constexpr auto create_regex_parser(Builder& b)
    {
        using slice = utils::slice;
        using namespace ftors;

        constexpr nterm<slice> expr("expr");
        constexpr nterm<slice> alt("alt");
        constexpr nterm<slice> concat("concat");
        constexpr nterm<slice> q_expr("q_expr");
        constexpr nterm<slice> primary("primary");
        constexpr nterm<size32_t> number("number");

        constexpr custom_term regex_digit_09("regex_digit_09", [](auto sv) { return size32_t(sv[0]) - '0'; });
        constexpr custom_term regex_primary("regex_primary", string_view_to_subset);

        return parser(
            expr,
            terms(regex_digit_09, regex_primary, '*', '+', '?', '|', '(', ')', '{', '}'),
            nterms(expr, alt, concat, q_expr, primary, number),
            rules(
                number(regex_digit_09),
                number(number, regex_digit_09) >= [](size32_t n, size32_t x){ return n * 10 + x; },
                primary(regex_digit_09) >= [&b](size32_t number){ return b.primary_char(char(number + '0')); },
                primary(regex_primary) >= [&b](const auto& s){ return b.primary_subset(s); },
                primary('(', expr, ')') >= _e2,
                q_expr(primary),
                q_expr(primary, '*') >= [&b](slice s, skip) { return b.star(s); },
                q_expr(primary, '+') >= [&b](slice s, skip) { return b.plus(s); },
                q_expr(primary, '?') >= [&b](slice s, skip) { return b.opt(s); },
                q_expr(primary, '{', number, '}') >= [&b](slice s, skip, size32_t n, skip) { return b.rep(s, n); },
                concat(q_expr),
                concat(concat, q_expr) >= [&b](slice s1, slice s2) { return b.cat(s1, s2); },
                alt(concat),
                alt(alt, '|', alt) >= [&b](slice s1, skip, slice s2) { return b.alt(s1, s2); },
                expr(alt)
            ),
            use_lexer<regex_lexer>{}
        );
    }



// This is truely an excelent refrence



    namespace regex
{
    class regex_lexer
    {
    public:
        constexpr regex_lexer()
        {
            //0              1             2 3 4 5 6 7 8 9
            //regex_digit_09 regex_primary * + ? | ( ) { }

            specials[utils::char_to_idx('*')] = 2;
            specials[utils::char_to_idx('+')] = 3;
            specials[utils::char_to_idx('?')] = 4;
            specials[utils::char_to_idx('|')] = 5;
            specials[utils::char_to_idx('(')] = 6;
            specials[utils::char_to_idx(')')] = 7;
            specials[utils::char_to_idx('{')] = 8;
            specials[utils::char_to_idx('}')] = 9;
        }

        template<typename Iterator, typename ErrorStream>
        constexpr auto match(
            match_options options,
            source_point sp,
            Iterator start,
            Iterator end,
            ErrorStream& error_stream)
        {
            if (start == end)
                return not_recognized(start);

            char c = *start;

            auto res = [&](size16_t idx, size_t len)
            { return recognized(idx, len, options, sp, start, error_stream); };

            if (specials[utils::char_to_idx(c)] != 0)
                return res(specials[utils::char_to_idx(c)], 1);

            if (utils::is_dec_digit(c))
                return res(0, 1);

            size_t len = 0;
            bool ok = match_primary(start, end, len);
            if (ok)
                return res(1, len);

            return not_recognized(start);
        }

    private:
        size16_t specials[meta::distinct_chars_count] = {};

        template<typename Iterator>
        constexpr bool match_primary(Iterator start, Iterator end, size_t& len)
        {
            len = 0;
            bool ok = match_escaped(start, end, len);
            if (!ok)
                return false;
            if (len != 0)
                return true;

            ok = match_range(start, end, len);
            if (!ok)
                return false;
            if (len != 0)
                return true;

            ok = utils::is_printable(*start);
            if (ok)
                len = 1;
            return ok;
        }

        template<typename Iterator>
        constexpr bool match_range(Iterator start, Iterator end, size_t& len)
        {
            char c = *start;
            if (c == '[')
            {
                len = 1;
                ++start;
                if (start == end)
                {
                    len = 0;
                    return false;
                }
                c = *start;
                if (c == '^')
                {
                    len = 2;
                    ++start;
                }
                if (start == end)
                {
                    len = 0;
                    return false;
                }
                while (start != end && *start != ']')
                {
                    size_t item_len = 0;
                    bool ok = match_range_item(start, end, item_len);
                    if (!ok)
                    {
                        len = 0;
                        return false;
                    }
                    len += item_len;
                    start += item_len;
                }
                if (start == end)
                {
                    len = 0;
                    return false;
                }
                len++;
            }
            return true;
        }

        template<typename Iterator>
        constexpr bool match_range_item(Iterator start, Iterator end, size_t& len)
        {
            len = 0;
            bool ok = match_escaped(start, end, len);
            if (!ok)
                return false;

            if (len == 0)
            {
                ok = utils::is_printable(*start);
                if (!ok)
                    return false;
                len = 1;
            }

            start += len;

            if (*start == '-')
            {
                len++;

                ++start;
                if (start == end || *start == ']')
                    return false;

                size_t range_end_len = 0;
                bool escaped_ok = match_escaped(start, end, range_end_len);
                if (!escaped_ok)
                    return false;

                if (range_end_len == 0)
                {
                    ok = utils::is_printable(*start);
                    if (!ok)
                        return false;
                    len += 1;
                }
                else
                    len += range_end_len;
            }

            return ok;
        }

        template<typename Iterator>
        constexpr bool match_escaped(Iterator start, Iterator end, size_t& len)
        {
            char c = *start;

            if (c == '\\')
            {
                ++start;
                if (start == end)
                    return false;
                c = *start;
                len = 2;
                if (c == 'x')
                {
                    ++start;
                    if (start == end)
                        return true;
                    c = *start;
                    if (!utils::is_hex_digit(c))
                        return true;
                    ++start;
                    len = 3;
                    if (start == end)
                        return true;
                    c = *start;
                    if (!utils::is_hex_digit(c))
                        return true;
                    len = 4;
                    return true;
                }
                return utils::is_printable(c);
            }
            return true;
        }

        template<typename Iterator, typename ErrorStream>
        constexpr auto recognized(
            size16_t idx,
            size_t len,
            match_options options,
            source_point sp,
            Iterator start,
            ErrorStream& error_stream)
        {
            Iterator term_end = start;
            if (options.verbose)
                error_stream << sp << " LEXER MATCH: Recognized " << idx << " \n";
            term_end += len;
            sp.update(start, term_end);
            return recognized_term<Iterator>{term_end, idx};
        }

        template<typename Iterator>
        constexpr auto not_recognized(Iterator start) const
        {
            return recognized_term<Iterator>{start, uninitialized16};
        }
    };

    constexpr char regex_char(std::string_view sv, size_t& len)
    {
        if (sv[0] == '\\')
        {
            if (sv[1] == 'x')
            {
                if (sv.size() == 2 || !utils::is_hex_digit(sv[2]))
                {
                    len = 2;
                    return 0;
                }
                else if (sv.size() == 3 || !utils::is_hex_digit(sv[3]))
                {
                    len = 3;
                    return hex_digits_to_char('0', sv[2]);
                }
                else
                {
                    len = 4;
                    return hex_digits_to_char(sv[2], sv[3]);
                }
            }
            else
            {
                len = 2;
                return sv[1];
            }
        }
        else
        {
            len = 1;
            return sv[0];
        }
    }

    constexpr char_subset string_view_to_subset(std::string_view sv)
    {
        char_subset cs;

        if (sv[0] == '.')
            cs.flip();
        else if (sv[0] == '[')
        {
            bool flip = false;
            size_t i = 1;
            if (sv[1] == '^')
            {
                i++;
                flip = true;
            }
            while (sv[i] != ']')
            {
                size_t len = 0;
                char c1 = regex_char(sv.substr(i), len);
                i += len;
                if (sv[i] == '-')
                {
                    ++i;
                    char c2 = regex_char(sv.substr(i), len);
                    cs.add_range(char_range{c1, c2});
                }
                else
                    cs.set(utils::char_to_idx(c1));
            }
            if (flip)
                cs.flip();
        }
        else
        {
            size_t len = 0;
            cs.set(utils::char_to_idx(regex_char(sv, len)));
        }

        return cs;
    }

    template<typename Builder>
    constexpr auto create_regex_parser(Builder& b)
    {
        using slice = utils::slice;
        using namespace ftors;

        constexpr nterm<slice> expr("expr");
        constexpr nterm<slice> alt("alt");
        constexpr nterm<slice> concat("concat");
        constexpr nterm<slice> q_expr("q_expr");
        constexpr nterm<slice> primary("primary");
        constexpr nterm<size32_t> number("number");

        constexpr custom_term regex_digit_09("regex_digit_09", [](auto sv) { return size32_t(sv[0]) - '0'; });
        constexpr custom_term regex_primary("regex_primary", string_view_to_subset);

        return parser(
            expr,
            terms(regex_digit_09, regex_primary, '*', '+', '?', '|', '(', ')', '{', '}'),
            nterms(expr, alt, concat, q_expr, primary, number),
            rules(
                number(regex_digit_09),
                number(number, regex_digit_09) >= [](size32_t n, size32_t x){ return n * 10 + x; },
                primary(regex_digit_09) >= [&b](size32_t number){ return b.primary_char(char(number + '0')); },
                primary(regex_primary) >= [&b](const auto& s){ return b.primary_subset(s); },
                primary('(', expr, ')') >= _e2,
                q_expr(primary),
                q_expr(primary, '*') >= [&b](slice s, skip) { return b.star(s); },
                q_expr(primary, '+') >= [&b](slice s, skip) { return b.plus(s); },
                q_expr(primary, '?') >= [&b](slice s, skip) { return b.opt(s); },
                q_expr(primary, '{', number, '}') >= [&b](slice s, skip, size32_t n, skip) { return b.rep(s, n); },
                concat(q_expr),
                concat(concat, q_expr) >= [&b](slice s1, slice s2) { return b.cat(s1, s2); },
                alt(concat),
                alt(alt, '|', alt) >= [&b](slice s1, skip, slice s2) { return b.alt(s1, s2); },
                expr(alt)
            ),
            use_lexer<regex_lexer>{}
        );
    }

    template<size_t N>
    constexpr size32_t analyze_dfa_size(const char (&pattern)[N])
    {
        dfa_size_analyzer a;
        auto p = create_regex_parser(a);
        buffers::cstring_buffer buffer(pattern);
        detail::no_stream s{};
        auto res = p.parse(
            parse_options{}.set_skip_whitespace(false),
            buffer,
            s);
        if (!res.has_value())
            throw std::runtime_error("invalid regex");
        return res.value().n;
    }

    template<typename Stream>
    constexpr void write_regex_parser_diag_msg(Stream& s)
    {
        regex::dfa_size_analyzer a;
        auto p = regex::create_regex_parser(a);
        p.write_diag_str(s);
    }

    template<auto& Pattern>
    class expr
    {
    public:
        static const size32_t dfa_size = analyze_dfa_size(Pattern);

        constexpr expr()
        {
            dfa_builder<dfa_size> b(sm);
            auto p = create_regex_parser(b);
            detail::no_stream stream{};
            auto s = p.parse(
                parse_options{}.set_skip_whitespace(false),
                buffers::cstring_buffer(Pattern),
                stream
            );
            if (!s.has_value())
                throw std::runtime_error("invalid regex");
            b.mark_end_states(s.value(), 0);
        }

        template<typename Stream>
        constexpr void debug_parse(Stream& s) const
        {
            regex::dfa_size_analyzer a;
            auto p = regex::create_regex_parser(a);
            p.parse(
                parse_options{}.set_skip_whitespace(false).set_verbose(),
                buffers::cstring_buffer(Pattern),
                s
            );
        }

        template<size_t N>
        constexpr bool match(const char (&str)[N]) const
        {
            return match(buffers::cstring_buffer(str));
        }

        template<typename Buffer>
        constexpr bool match(const Buffer& buf) const
        {
            detail::no_stream s;
            return match(buf, s);
        }

        template<typename Buffer, typename Stream>
        constexpr bool match(const Buffer& buf, Stream& s) const
        {
            return match(match_options{}, buf, s);
        }

        template<typename Buffer, typename Stream>
        constexpr bool match(match_options opts, const Buffer& buf, Stream& s) const
        {
            auto res = dfa_match(sm, opts, source_point{}, buf.begin(), buf.end(), s);
            if (res.term_idx == 0 && res.it == buf.end())
                return true;
            else
            {
                if (res.term_idx == 0)
                    s << "Leftover text after recognition: " << buf.get_view(res.it, buf.end()) << "\n";
                else
                    s << "Unexpected char: " << utils::c_names.name(*res.it) << "\n";
                return false;
            }
        }

        template<typename Stream>
        constexpr void write_diag_str(Stream& stream) const
        {
            regex::write_dfa_diag_str(sm, stream);
        }

    private:
        dfa<dfa_size> sm;
    };
}

template<auto& Pattern>
class regex_term : public term
{
public:
    using internal_value_type = std::string_view;

    static const size_t dfa_size = regex::analyze_dfa_size(Pattern);
    static const bool is_trivial = false;

    static const size_t pattern_size = std::size(Pattern);

    constexpr regex_term(associativity a = associativity::no_assoc) :
        regex_term(nullptr, 0, a)
    {}
