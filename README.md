Os geradores de tráfego permitem a criação de tráfego sintético para simulação de cenários reais e análise de delays. 
Cada versão do gerador de tráfego é capaz de produzir pacotes com timestamp em milisegundos do momento da criação do pacote. Os pacotes são recebidos por um manipulador que irá coletar o timestamp do momento de recebimento do pacote e mostrar a diferença.

Os geradores TCP e UDP foram desenvolvidos em C, operando por padrão na porta 12345 e geram tráfego com chegada obedecendo a distribuição de poisson.

O geradore MQTT foi desenvolvido em js e deve ser executado com node


Neste repositório estão disponíveis:


Geradores de tráfego UDP

Geradores de tráfego TCP 

Geradores de tráfego MQTT

Geradores de tráfego em formato XML


A utilização de todos os geradores ocorre da mesma forma:

#Compilação para linux e exemplo

gcc geradorUDP.c -o geradorUDP -lm

gcc manipuladorUDP.c -o manipuladorUDP


# Inicia o manipulador para recebimento de pacotes

./manipuladorUDP 

# Inicia o gerador para envio de pacotes com os parâmetros

./gerador tempo média destino

Tempo: duração do experimento

Média: média para a distribuição de poisson que define quantos pacotes em média serão enviados por segundo

Destino: destinatário onde está sendo executado o manipulador


A saída do manipulador será dada pelo formato "D| <delay>" ,como por exemplo:

D| 5  \\ indica delay de 5 milisegundos


Para armazenamento das saídas, utilizar o padrão do sistema operacional >>

Ex:

./manipulador >> saida.log


# Gerador HTTP

O gerador HTTP permite que se defina o cabeçalho Content-type, que deve ser passado como parâmetro. Compilação do gerador e manipulador são as mesmas dos demais códigos em c.

Exemplo de utilização para o gerador HTTP:

./geradorHTTP <duracao_em_segundos> <mensagens_por_segundo> <ip_destino> <content-type>


# Gerador MQTT

Para o gerador MQTT foi utilizado o node pois as biblitecas MQTT disponíveis para python geram conflitos com versões mais recentes do python


Instalar o Nodejs

instalar os pacotes necessários: 

sudo npm install mqtt


#Utilizando o gerador MQTT

Inicie primeiro o receptor:

node manipuladorMQTT.js <endereco_broker> <tópico>


Inicie o gerador:

node geradorMQTT.js <endereco_broker> <tópico> <mensagens_segundo> <duracao>


