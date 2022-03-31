from pathlib import Path
import re
import subprocess

def read_make_output( path : Path = Path( "/tmp/makeout" ) ) -> [ str ]: 
    make_output = []
    with open( str( path ), 'r' ) as file_stream: 
        make_output = file_stream.read_lines()
    return make_output

def format_lines( lines : [ str ], 
            find_code_regex : str = r".+[0-9]+:[0-9]+:([^'.]|'constexpr')+'", 
            format_buffer_file : Path = Path( "/tmp/makeoutbuffer" ) ) -> [ str ]: 
    find_code_regex = re.compile( find_code_regex )
    format_buffer_file = str( format_buffer_file )
    for line in lines: 
        code_match = re.match( find_code_regex, line )
        if code_match:
            code = line[ code_match.end() : -1 ]
            with open( format_buffer_file, 'w' ) as buffer_file: 
                buffer_file.write( code )
            formatted_code = subprocess.run( [ 
                            "clang-format", 
                            format_buffer_file, 
                    ], 
                    capture_output=True 
                 ).stdout.decode( "utf-8" )
            yield line.replace( 
                    code, 
                    '\n' + formatted_code
                )
        #]

def main(): 
    test_string = "/root/workdir/Source/Main.cpp:28:5:   in 'constexpr' expansion of 'ctpg::parser<ctpg::nterm<long unsigned int>, std::tuple<ctpg::char_term, ctpg::char_term, ctpg::char_term, ctpg::char_term, ctpg::char_term, ctpg::char_term>, std::tuple<ctpg::nterm<long unsigned int>, ctpg::nterm<long unsigned int>, ctpg::nterm<long unsigned int> >, std::tuple<ctpg::detail::rule<<lambda(auto:65)>, ctpg::nterm<long unsigned int>, ctpg::char_term>, ctpg::detail::rule<<lambda(auto:66, auto:67)>, ctpg::nterm<long unsigned int>, ctpg::regex_term<RawTemplateArray<'[', '0', '-', '9', ']', '[', '0', '-', '9', ']', '*', '\000'>::array>, ctpg::char_term> >, ctpg::use_generated_lexer>(ConstantExpression<to_size_t, long unsigned int, RawTemplateArray<'N', 'a', 't', 'u', 'r', 'a', 'l', 'N', 'u', 'm', 'b', 'e', 'r', '\000'>()>::factor, ConstantExpression<to_size_t, long unsigned int, RawTemplateArray<'N', 'a', 't', 'u', 'r', 'a', 'l', 'N', 'u', 'm', 'b', 'e', 'r', '\000'>()>::terms, ConstantExpression<to_size_t, long unsigned int, RawTemplateArray<'N', 'a', 't', 'u', 'r', 'a', 'l', 'N', 'u', 'm', 'b', 'e', 'r', '\000'>()>::nterms, ctpg::rules(Rules&& ...) [with Rules = {ctpg::detail::rule<<lambda(auto:65)>, ctpg::nterm<long unsigned int>, ctpg::char_term>, ctpg::detail::rule<<lambda(auto:66, auto:67)>, ctpg::nterm<long unsigned int>, ctpg::regex_term<RawTemplateArray<'[', '0', '-', '9', ']', '[', '0', '-', '9', ']', '*', '\000'>::array>, ctpg::char_term>}](ctpg::detail::rule<F, L, R>::operator>=(F1&&) [with F1 = <lambda(auto:66, auto:67)>; F = std::nullptr_t; L = ctpg::nterm<long unsigned int>; R = {ctpg::regex_term<RawTemplateArray<'[', '0', '-', '9', ']', '[', '0', '-', '9', ']', '*', '\000'>::array>, ctpg::char_term}](<lambda closure object><lambda(auto:66, auto:67)>{})))'"
    print( [ i for i in format_lines( [ test_string ] ) ][ 0 ] )

if __name__ == "__main__": 
    main()
