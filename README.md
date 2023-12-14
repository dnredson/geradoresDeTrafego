Os geradores de tráfego permitem a criação de tráfego sintético para simulação de cenários reais e análise de delays. 
Cada versão do gerador de tráfego é capaz de produzir pacotes com timestamp em milisegundos do momento da criação do pacote. Os pacotes são recebidos por um manipulador que irá coletar o timestamp do momento de recebimento do pacote e mostrar a diferença.

Todos os simuladores foram desenvolvidos em C, operando por padrão na porta 12345 e geram tráfego com chegada obedecendo a distribuição de poisson.
Neste repositório estão disponíveis:

Geradores de tráfego UDP (Linux)
Geradores de tráfego UDP (Windows)
Geradores de tráfego TCP 

A utilização de todos os geradores ocorre da mesma forma:
#Compilação para linux e exemplo
gcc geradorUDP.c -o geradorUDP
gcc manipuladorUDP.c -o manipuladorUDP

# Inicia o manipulador para recebimento de pacotes
./manipuladorUDP 

# Inicia o gerador para envio de pacotes com os parâmetros
./gerador tempo média destino

Tempo: duração do experimento
Média: média para a distribuição de poisson que define quantos pacotes em média serão enviados por segundo
Destino: destinatário onde está sendo executado o manipulador

A saída do manipulador será dada pela variável D| <delay>
como por exemplo:
D| 5  \\ indica delay de 5 milisegundos

Para armazenamento das saídas, utilizar o padrão do sistema operacional >>
Ex:
./manipulador >> saida.log
