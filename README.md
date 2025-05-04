# Atividade 06-05
Este é o repositório que armazena a tarefa solicitada no dia 29/04. Todos os arquivos necessários à execução já foram criados, de modo que basta seguir as instruções abaixo para executá-lo em seu dispositivo.

## Como Usar
1. Para acessar a atividade armazenada, clone este repositório para seu dispositivo executando o seguinte comando no terminal de um ambiente adequado, como o VS Code, após criar um repositório local: git clone https://github.com/nivaldojunior037/Atividade-06-05-EmbarcaTech

2. Após isso, importe como um projeto a pasta que contém os arquivos clonados em um ambiente como o VS Code e compile o código existente para que os demais arquivos necessários sejam criados em seu dispositivo

3. Ao fim da compilação, será gerado um arquivo .uf2 na pasta build do seu repositório. Esse arquivo deve ser copiado para a BitDogLab em modo BOOTSEL para que ele seja corretamente executado.

### Como Executar o Código
1. O código é executado automaticamente quando o arquivo .uf2 é copiado para a BitDogLab. Ele simula um semáforo por meio da matriz de LEDs 5x5 e do LED RGB central da placa. Além disso, ele também conta com um modo noturno e com sinalização sonora para acessibilidade de pessoas com deficiência visual. 

2. O código é iniciado no modo normal, alternando entre verde, amarelo e vermelho. Uma mensagem para cada cor é mostrada no display OLED. Cada cor também possui um sinal sonoro específico: beep de 1 segundo com intervalos curtos para verde, beep de 0,3 segundos com intervalos curtos para amarelo e beep de 0,5 segundos com intervalos longos para vermelho.

3. O modo noturno pode ser acionado por meio do botão A. Ele é identificado pela luz amarela piscando de forma intermitente, com beeps de 1,5 segundos e intervalos de 2 segundos, como solicitado na atividade. A mensagem no display também é específica para esse modo, indicando que se trata do modo noturno. O modo normal é retomado se o botão é pressionado novamente. 

4. O ciclo verde-amarelo-vermelho está unido dentro das condicionais do código, de modo que o modo noturno só pode ser acionado ao fim de um ciclo desses. Sendo assim, basta acionar o botão em qualquer momento de um ciclo e aguardar que o modo noturno será acionado tão logo este ciclo se encerre. O mesmo é válido para retornar ao modo normal. Essa situação poderia ser alterada apenas ao colocar as partes normais verde, amarela e vermelha em diferentes condicionais, todas acionadas pela mesma condição. Contudo, neste código, optou-se pela robustez, de modo que eles foram unidos. 

#### Link do vídeo
Segue o link do Drive com o vídeo onde é demonstrada a utilização do código: https://drive.google.com/file/d/15bkOi6jdDBOCTk8FC15Y1on9LTaQ-UfD/view?usp=sharing
