int x;
int y;
int igual;
int diferente;

begin
    write "Informe o valor de X:";
    read x;

    write "Informe o valor de Y:";
    read y;

    igual = 0;
    diferente = 0;

    if (x == y) begin
        igual = 1;
    end

    if (x != y) begin
        diferente = 1;
    end

    write igual;
    write diferente;
end