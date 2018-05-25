# palindrome reads ARRAY_MAX numbers from stdin and inserts them into
# two arrays in opposite orders. Then checks whether these two arrays
# are equal and prints either 1 or 0, according to the result.
program palindrome;

const
    ARRAY_MAX = 4;

var
    # Program arrays
    in, reversed: array [0..ARRAY_MAX] of integer;

    # Auxiliary variables
    read, i: integer;

begin
    writeln(ARRAY_MAX + 1);
    for i := 0 to ARRAY_MAX do begin
        read := readln();
        in[i] := read;
        reversed[ARRAY_MAX - i] := read;
    end;

    for i := 0 to ARRAY_MAX do begin
        if in[i] <> reversed[i] then begin
            writeln(0);
            exit;
        end;
    end;

    writeln(1);
end.