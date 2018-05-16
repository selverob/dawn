program arrayAverage;

function arrTest();
var
    Arr: array [-5 .. 5 ] of integer;
    i: integer;
begin
    Arr[-2] := readln();
    for i := -4 to 4 do
        writeln(Arr[i]);
end;

begin
    arrTest();
end.
