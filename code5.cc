int x;
int soma(int a, int b, int c, int d, int e);

begin
    x = soma(10, 20, 30, 40, 50);
    write x;
end

int soma(int a, int b, int c, int d, int e)
begin
    int resultado;
    resultado = a + b + c + d + e;
    return resultado;
end