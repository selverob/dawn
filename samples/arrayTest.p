program arrayAverage;

var I, TEMP, NUM, SUM : integer;
var X : array [-20 .. 20] of integer;
begin
  for I := -20 to 20 do begin
    X[I] := 0;
  end;

  NUM := readln();
  
  for I := 0 to NUM - 1 do begin
    TEMP := readln();
    X[TEMP] := X[TEMP] + 1;
  end;

  SUM := 0;
  for I := 20 downto -20 do begin
    SUM := SUM + I * X[I];
  end;
  writeln(SUM / NUM);
end.
