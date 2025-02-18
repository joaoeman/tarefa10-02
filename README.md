main no arquivo: adcproject.c

Link do vídeo: https://drive.google.com/file/d/1EFGkZmVcVggpGrWU5XxT8FOLGAxB1T-E/view?usp=drivesdk

O que é ADC?
O ADC (Conversor Analógico-Digital) é um componente eletrônico que converte sinais analógicos (como tensões variáveis) em valores digitais (números) que podem ser processados por um microcontrolador. No contexto deste projeto, o joystick analógico gera sinais de tensão variável em seus eixos X e Y, que são lidos pelo ADC do Raspberry Pi Pico. Esses valores são então utilizados para controlar a intensidade dos LEDs e a posição do quadrado no display.

Funcionamento Geral do Código
O código pode ser dividido em três partes principais: leitura do joystick, controle dos LEDs e atualização do display. Vamos explorar cada uma delas.

1. Leitura do Joystick
O joystick possui dois eixos (X e Y) que geram sinais analógicos conforme são movimentados. Esses sinais são lidos pelo ADC do Raspberry Pi Pico e convertidos em valores digitais. Esses valores representam a posição do joystick em cada eixo, variando de 0 a 4095 (12 bits de resolução). O código também verifica se o botão do joystick foi pressionado, alternando o estado de uma variável que controla o LED verde.

2. Controle dos LEDs
Com base nos valores lidos do joystick, o código calcula a intensidade dos LEDs vermelho e azul. Quando o joystick é movido para a esquerda ou direita, a intensidade do LED vermelho aumenta proporcionalmente. Da mesma forma, quando o joystick é movido para cima ou para baixo, a intensidade do LED azul aumenta. Se o joystick estiver na posição central, os LEDs são desligados. Além disso, o LED verde é acionado apenas quando o joystick está centralizado e o botão é pressionado.

3. Atualização do Display
O display OLED é utilizado para exibir um quadrado que se move conforme a posição do joystick. O código mapeia os valores analógicos do joystick para coordenadas na tela, permitindo que o quadrado se mova suavemente. Além disso, o sistema permite alternar entre três modos de borda no display: sem borda, borda simples e borda dupla. Essa alternância é controlada pelo botão do joystick.

Interação com o Usuário
O sistema é altamente interativo, respondendo às ações do usuário de forma dinâmica:

Movimento do Joystick: Controla a posição do quadrado no display e a intensidade dos LEDs.

Botão do Joystick: Alterna entre os modos de borda no display, e o led verde(ligando ou desligando).

Botão Externo: Liga ou desliga o pwm dos leds, independentemente da posição do joystick.
