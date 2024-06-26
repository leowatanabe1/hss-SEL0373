# hss-Home Security System

Projeto elaborado para a disciplina SEL0373-Projetos de iot.
Docente: Jose Roberto Boffino de Almeida Monteiro
Alunos:
Gabi
Leonardo Koiti Watanabe 11800591
Paulo

## Resumo do projeto

O projeto é um sistema de securança da casa que integra uma camera de segurança na porta da casa (WebCam), a campainha da casa (um Push-Buttom) e a trava de segurança (um LED), tudo isso, sendo possível ser controlado pelo usuário por meio de qualquer dispositivo conectado à internet. Esse projeto envolve o uso de uma Raspberry, responsável pelo video streaming da Web-Cam, e duas ESP32, responsáveis pela campainha e pela tranca. 

Materiais:
- Raspberry Pi 3 Model B+
- 2x ESP32
- USB WebCam
- Protoboard
- LED
- PushButtom
- Jumpers

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
