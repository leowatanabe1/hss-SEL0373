# hss-Home Security System

Projeto elaborado para a disciplina SEL0373-Projetos de iot.
Docente: Jose Roberto Boffino de Almeida Monteiro
Alunos:
- Gabriele Namie Okabayashi 11884755
- Leonardo Koiti Watanabe 11800591
- Paulo Roberto de Abreu Chagas Neto 11233911

## Resumo do projeto

O projeto é um sistema de securança da casa que integra uma camera de segurança na porta da casa (WebCam), a campainha da casa (um Push-Buttom) e a trava de segurança (um LED), tudo isso, sendo possível ser controlado pelo usuário por meio de qualquer dispositivo conectado à internet. Esse projeto envolve o uso de uma Raspberry, responsável pelo video streaming da Web-Cam, e duas ESP32, responsáveis pela campainha e pela tranca. 
# Como funciona o sistema de segurança
1. **Pressão do Botão da Campainha**:
   - Quando uma visita pressiona o botão da campainha, um sinal é enviado ao sistema de segurança.

2. **Notificação ao Usuário**:
   - O usuário do sistema recebe uma mensagem no WhatsApp notificando sobre a presença de uma visita.

3. **Verificação da Imagem**:
   - O usuário pode acessar um site na internet para visualizar a imagem da visita.

4. **Ação do Usuário**:
   - Se o usuário reconhecer a pessoa, ele pode apertar um botão no site para abrir a fechadura e permitir a entrada da visita.

Materiais:
- Raspberry Pi 3 Model B+
- ESP32
- USB WebCam
- Protoboard
- LED
- PushButtom
- Jumpers

##

## Streaming de Vídeo

A parte de Streaming de vídeo envolveu o uso da Raspberry com uma USB WebCam. Vale ressaltar que tentou-se usar o Módulo Pi Camera, mas com os frágeis conectores e as más condições que estavam, o projeto evoluiu melhor com a WebCam.

O primeiro passo é a atualização do sistema.

```
sudo apt update
sudo apt upgrade
```

E depois instalar fswebcam, necessário para capturar imagens da webcam.

```
sudo apt install fswebcam
```

Uma forma de verificar o funcionamento é verificar se o dispositivo foi detectado, sendo listado com o comando:

```
ls /dev/video*
```

Ou tirar uma foto com o comando:

```
fswebcam image.jpg
```

Verificado o funcionamento da WebCam, o próximo passo é o Streaming de vídeo para um localhost. Para isso, vai ser utilizado Python Flask e a biblioteca OpenCV, que devem ser instalados:

```
sudo apt install python3 python3-pip
pip3 install Flask
sudo apt install libatlas-base-dev
pip3 install opencv-python-headless
```

Com as instalações realizadas, o código Python [stream_code.py](https://github.com/leowatanabe1/hss-SEL0373/blob/main/stream_code.py) realiza o stream no http://localhost:8080.

Contudo, essa stream fica visível a apenas dispositivos conectados à mesma rede, sendo que pro projeto interessa visualizar em qualquer lugar. Para isso, algumas alternativas podem ser usadas, no caso o serveo é uma ferramenta que expoe serviços locais na internet através de um túnel SSH, sem a necessidade de configurar um servidor externo ou mexer nas configurações do roteador. Primeiro, deve-se instalar o pacote SSH:

```
sudo apt install openssh-client
```

Depois disso, basta usar o código:

```
ssh -R SEU_SUBDOMINIO:80:localhost:PORTA_LOCAL serveo.net
```

Trocando o subdomínio desejado e o port usado. No caso, nosso subdomínio será hss e nosso port é o 8080. Assim:

```
ssh -R hss:80:localhost:8080 serveo.net
```

A primeiro momento, o código não deve funcionar porque é preciso registra-se no serveo para usar um subdomínio, com uma conta google ou github (deve aparecer um link no terminal, basta seguir o link e registra-se). A stream será feita na URL hss.serveo.net . 

No entanto, a stream ainda não vai aparecer no site. Isso porque deve estar puxando o código html padrão da Raspberry localizado em /var/www/html/index.html, que geralmente é onde arquivos de página inicial são armazenados em servidores web baseados em Linux. Assim, altere esse arquivo pelo [index.html](https://github.com/leowatanabe1/hss-SEL0373/blob/main/index.html), que apenas puxa o vídeo do http://localhost:8080. 

Feito isso, o vídeo stream deve estar sendo executando normalmente no hss.serveo.net. 
## Broker MQTT na Nuvem com Mosquitto - Acesso em Qualquer Lugar (Digital Ocean)
Para a publicação e subscrição de dados diferentes utiliza-se o protocolo de comunicação MQTT com o Cloud MQTT Broker hosteado pelo serviço de nuvem do Digital Ocean
1. **Configurar Droplet no Digital Ocean**:
   - Crie um novo Droplet com Ubuntu.
   - Acesse o Droplet via SSH.

2. **Instalar Mosquitto**:
   - Atualize a lista de pacotes:
     
     ```bash
     sudo apt update
     ```
   - Instale o Mosquitto:
     
     ```bash
     sudo apt install mosquitto mosquitto-clients
     ```

3. **Configurar Mosquitto**:
   - Edite o arquivo de configuração:
     
     ```bash
     sudo nano /etc/mosquitto/mosquitto.conf
     ```
   - Permita acesso remoto e configure a autenticação.

4. **Habilitar e Iniciar Mosquitto**:
   - Habilite o serviço:
     
     ```bash
     sudo systemctl enable mosquitto
     ```
   - Inicie o serviço:
     
     ```bash
     sudo systemctl start mosquitto
     ```

5. **Testar Conexão**:
   - Use um cliente MQTT para conectar e testar o broker.
## Configuração do Node-red
Vamos utilizar o node-red para facilitar o projeto e ter uma ui
1. **Configurar o Node-red na virtual machine**:
   - baixar npm:
     ```bash
     sudo apt install npm -y
     ```
   - baixar node-red:
     ```bash
     sudo npm install -g --unsafe-perm node-red
     ```
   - Abrir o firewall na porta do node-red
     ```bash
     sudo ufw allow 1880
     ```
   - Iniciar node-red
     ```bash
     node-red start
     ```
2. **Configurar o Node-red para rodar na inicialização**:
   - vá para os arquivos de configuração:
     ```bash
     sudo nano /etc/systemd/system/nodered.service
     ```
   - Altere os arquivos para eles ficarem como abaixo:
     ```bash
      [Unit]
      Description=Node-RED
      After=syslog.target network.target

      [Service]
      ExecStart=/usr/local/bin/node-red --max-old-space-size=128 -v
      Restart=on-failure
      KillSignal=SIGINT
      
      # log output to syslog as 'node-red'
      SyslogIdentifier=node-red
      StandardOutput=syslog
      
      # non-root user to run as
      #WorkingDirectory=/home/rui/
      #User=rui
      #Group=rui
      
      # if using a root user
      WorkingDirectory=/root/
      User=root
      Group=root
      
      [Install]
      WantedBy=multi-user.target
     ```
   - ative o arquivo quando inicializar e reinicialize o sistema:
     ```bash
     sudo systemctl enable nodered.service
     sudo reboot
     ```
3. **Acessar o node-red e configurar o fluxo**:
   - Vá ao endereço http://SEU-IP:1880
   - Importe o fluxo 'flow do node-red' no repositório
   - baixe os módulos 'node-red-contrib-whatsapp-cmb' e 'node-red-dashboard' nas opções de gerenciar paleta (canto superior direito)
   - Adicionar os dados necessários em cada nó
      - seus dados no nó do whatsapp (https://www.callmebot.com/blog/free-api-whatsapp-messages/)
      - IP, tópico e QoS desejado nos nós de mqtt
      - Endereço de IP da sua stream de vídeo no nó de template
4. **Acessar o node-red dashboard**:
   Por fim voce pode ir ao endereço http://SEU-IP:1880/ui para acessar a dashboard do node-red onde voce poderá ver a stream e apertar o botão para abrir a porta
## Configuração da ESP32 para comunicação da campainha e fechadura
Utilizou-se uma ESP32 para:
1. Comunicação da Campainha
2. Controle da fechadura
   
Quando acionado o Switch (que representa a campainha), a ESP32 deve enviar uma mensagem pro Broker MQTT, informando que há alguém na porta.
Já quando permitido a entrada, o Broker deve enviar uma mensagem à ESP32 para acender o LED (abrir a porta). 

O código usado é o [esp32.ino](https://github.com/leowatanabe1/hss-SEL0373/blob/main/esp32.ino
)
A imagem a seguir mostra o circuito montado para representar a pressão da campainha com o botão switch e destranca da porta com o led acendendo
<img src="/circuitos.jpg">
Rodando o arquivo "esp32.ino" em ambiente Arduino.ide pode-se realizar a comunicação da ESP32 para o MQTT broker
