program factorialCycle;

var
    n: integer;
    f: integer;
begin
    f := 1;
    n := readln();
    while(n >= 2) do begin
        f := f * n;
	    n := n - 1;
    end;
    writeln(f);
end.
