#from: https://stackoverflow.com/questions/410980/include-a-text-file-in-a-c-program-as-a-char
function(make_includeable input_file output_file) 
    file(READ ${input_file} content)
    set(delim "for_C++_include")
    set(content "R\"${delim}(${content})${delim}\"")
    file(WRITE ${output_file} "${content}")
endfunction(make_includeable)

#from: https://jonathanhamberg.com/post/cmake-file-embedding/
function(make_binary_includeable input_file output_file) 
    file(READ ${input_file} content HEX)

    # Separate into individual bytes.
    string(REGEX MATCHALL "([A-Fa-f0-9][A-Fa-f0-9])" SEPARATED_HEX ${content})

    set(output_c "{\n    ")

    set(counter 0)
    foreach (hex IN LISTS SEPARATED_HEX)
        string(APPEND output_c "0x${hex},")
        math(EXPR counter "${counter}+1")
        if (counter GREATER 16)
            string(APPEND output_c "\n    ")
            set(counter 0)
        endif ()
    endforeach ()

    string(APPEND output_c "\n}")

    file(WRITE ${output_file} "${output_c}")
endfunction(make_binary_includeable)
