program expressions;

const
    Multiplyer = 5;
var
    n: integer;

begin
    n := readln();
    n := (n - 1) * Multiplyer + 10;
    writeln(n);
end.