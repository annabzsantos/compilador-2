int x;
int y;
int resultado;                      

int calcula(int a, int b);            

begin
    write "Informe o valor de X:";
    read x;

    write "\nInforme o valor de Y:";
    read y;

    calcula(x, y);
    write resultado;
end

int calcula(int a, int b)
begin
    int r;
    r = a;                             
    return r;
end