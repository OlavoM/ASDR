{ programa le uma sequencia de números inteiros 
e encontra o maior } 
Program exemplo; 
  integer num, maior, cont, qtd; 
begin 
  read(qtd); 
  cont:=0; 
  maior := 0; #inicializa a variavel maior com 0 
  while ( cont < qtd ) do 
  begin 
    read(num); 
    if( num > maior ) then 
      maior := num; 
  
      cont := cont + 1
    end; 
    write(maior) # imprime o maior valor 
end;

