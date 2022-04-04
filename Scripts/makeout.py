#! /bin/python3

from pathlib import Path
import re
import subprocess

DEFAULT_MAKE_OUTPUT_PATH = Path( "/tmp/makeout" )
DEFAULT_FORMATTED_MAKE_OUTPUT_BUFFER_PATH = Path( "/tmp/makeoutbuffer" )
DEFAULT_FIND_CODE_REGEX = r"(.+[0-9]+:[0-9]+:)+([^'.]|'constexpr'|)+'"
#r".+[0-9]+:[0-9]+:([^'.]|'constexpr')+'"
DEFAULT_BUILD_COMMAND = [ "make" ]

DEFAULT_SANITIZE_REPLACEMENT_MAP = {
    '‘' : "'", 
    '’' : "'"
}

def run_make( 
            output_path : Path = DEFAULT_MAKE_OUTPUT_PATH, 
            build_command : [ str ] = DEFAULT_BUILD_COMMAND ): 
    with open( str( output_path ), 'wb' ) as output_buffer: 
        result = subprocess.run( build_command, 
                stdout = output_buffer, 
                stderr = output_buffer, 
            )

def read_make_output( path : Path = DEFAULT_MAKE_OUTPUT_PATH ) -> [ str ]: 
    make_output = []
    with open( str( path ), 'r' ) as file_stream: 
        make_output = file_stream.readlines()
    return make_output

def sanitize( 
        lines : [ str ], 
        replacement_map : dict = DEFAULT_SANITIZE_REPLACEMENT_MAP ) -> [ str ]: 
    sanitized_lines = lines
    for to_replace in replacement_map: 
        sanitized_lines = [ 
                line.replace( to_replace, replacement_map[ to_replace ] ) 
                for line in sanitized_lines 
            ]
    return sanitized_lines
        

def format_lines( lines : [ str ], 
            find_code_regex : str = DEFAULT_FIND_CODE_REGEX, 
            format_buffer_file : Path = DEFAULT_FORMATTED_MAKE_OUTPUT_BUFFER_PATH ) -> [ str ]: 
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
                 ).stdout.decode()
            yield line.replace( 
                    code, 
                    "'\n" + formatted_code[ : -1 ]
                )
        else: 
            yield line

def format_output( 
        output_path : Path = DEFAULT_MAKE_OUTPUT_PATH, 
        build_command : [ str ] = DEFAULT_BUILD_COMMAND, 
        find_code_regex : str = DEFAULT_FIND_CODE_REGEX, 
        format_buffer_file : Path = DEFAULT_FORMATTED_MAKE_OUTPUT_BUFFER_PATH, 
        replacement_map : dict = DEFAULT_SANITIZE_REPLACEMENT_MAP ) -> [ str ]: 
    run_make( output_path, build_command )
    return [ line for line in format_lines( 
            sanitize( read_make_output( output_path ), replacement_map ), 
            find_code_regex, 
            format_buffer_file 
        ) ]

def main(): 
    with open( 'make_output.cpp', 'w' ) as output: 
        for line in format_output(): 
            output.write( line )

if __name__ == "__main__": 
    main()
