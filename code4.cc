int x;
int y;
int resultado;
int valor;

int soma(int a, int b);
int testeEscopo(int valor);

begin
write "Digite o valor de X:";
read x;

write "\nDigite o valor de Y:";
read y;

resultado = soma(x, y);
write "\nResultado da soma:";
write resultado;

resultado = testeEscopo(resultado);
write "\nResultado apos teste de escopo:";
write resultado;

end

int soma(int a, int b)
begin
int r;

r = a + b;
return r;

end

int testeEscopo(int valor)
begin
int x;

x = valor * 2;         
return x;

end