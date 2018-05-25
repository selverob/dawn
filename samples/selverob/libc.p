# libc sample shows interfacing with C standard library
# we declare abs and rand functions which are then linked
# into the program by dawnc.
program libc;

function abs(n: integer): integer; forward;
function rand(): integer; forward;

var
    i: integer;
begin
    for i := -1 to 1 do
        writeln(abs(i));

    for i := 0 to 10 do
        writeln(rand());
end.