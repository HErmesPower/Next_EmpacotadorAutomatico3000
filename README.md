# 📦 Next Empacotador Automático 3000

Bem-vindo ao repositório do **Next Empacotador Automático 3000**! Este projeto é um sistema inteligente de controle de servos baseado em distância (Time-of-Flight), desenvolvido para automatizar o empacotamento com extrema suavidade e precisão.

## 🚀 Como Funciona?

O sistema utiliza três sensores de distância **VL53L0X** (lasers de alta precisão) para monitorar o ambiente. A distância lida pelos sensores controla diretamente a posição de três **Servo Motores**.

Para garantir um funcionamento **estável e sem ruídos ("tranquinhos")**, o código implementa duas lógicas de proteção em tempo real:
1. **Média Acumulada de 1 Segundo**: Em vez de reagir a cada micro-mudança, o Arduino lê os sensores mais de 30 vezes por segundo, soma tudo e, exatamente a cada 1 segundo, tira uma média perfeita da distância.
2. **Zona Morta (Deadband)**: O microcontrolador ignora mudanças de distância que resultem em movimentos menores do que 2 graus. Isso impede que os servos fiquem tremendo e gastando energia à toa.

## 🔌 Esquema de Ligações (Conexões)

### 📏 Sensores de Distância (VL53L0X)
Os sensores se comunicam via protocolo **I2C**. Como todos vêm de fábrica com o mesmo endereço (`0x29`), utilizamos pinos digitais no modo `XSHUT` para ligar um por um no código e trocar seus endereços dinamicamente.

*   **VCC:** 3.3V ou 5V (depende do seu módulo, verifique a placa)
*   **GND:** GND
*   **SDA:** Pino SDA do Arduino (Uno/Nano: A4, Mega: 20)
*   **SCL:** Pino SCL do Arduino (Uno/Nano: A5, Mega: 21)

Pinos de Controle (XSHUT):
*   **Sensor 1:** Pino Digital `8` -> (Endereço alterado para `0x30`)
*   **Sensor 2:** Pino Digital `9` -> (Endereço alterado para `0x31`)
*   **Sensor 3:** Pino Digital `10` -> (Endereço alterado para `0x32`)

### ⚙️ Servo Motores
*Atenção: Sempre utilize uma fonte de energia externa (como pilhas ou uma fonte de bancada) para os servos. O pino 5V do Arduino não fornece corrente suficiente para 3 motores! Lembre-se de conectar o GND da fonte com o GND do Arduino.*

*   **Servo 1 (Rotação Contínua 360º):** Pino Digital `5` 
    *(Nota: Servos de 360º controlam direção/velocidade em vez de posição. O valor de angulo 90 faz ele parar.)*
*   **Servo 2 (Padrão 180º):** Pino Digital `6`
*   **Servo 3 (Padrão 180º):** Pino Digital `7`

## 🛠️ Tecnologias Utilizadas
- [PlatformIO](https://platformio.org/) (Ambiente de desenvolvimento)
- [Biblioteca VL53L0X] para leitura I2C.
- `Servo.h` nativa do Arduino.

## ⚠️ Observações de Hardware
Caso deseje que o **Servo 1** vá para uma posição exata e pare (como os outros), ele precisará ser substituído fisicamente por um servo de posição padrão (180 graus), visto que modelos 360º não têm controle posicional interno.