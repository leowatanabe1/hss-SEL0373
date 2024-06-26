from flask import Flask, Response
import cv2

app = Flask(__name__)

def gen_frames():
    cap = cv2.VideoCapture(0)  # Inicia a captura de vídeo usando a webcam
    while True:
        success, frame = cap.read()  # Lê um frame da webcam
        if not success:
            break
        else:
            ret, buffer = cv2.imencode('.jpg', frame) # Codifica o frame como JPEG
            frame = buffer.tobytes() # Converte o buffer do frame JPEG em bytes
            yield (b'--frame\r\n'
                   b'Content-Type: image/jpeg\r\n\r\n' + frame + b'\r\n')
            # Aqui gera uma resposta HTTP para transmitir uma sequência de frames como um fluxo de vídeo

@app.route('/')  # Quando acessar a rota principal, chama a função video_feed()
def video_feed():
    return Response(gen_frames(),
                    mimetype='multipart/x-mixed-replace; boundary=frame')
# Configura o envio contínuo de imagens da webcam para o navegador, permitindo o streaming de vídeo em tempo real

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=8080)
# Inicia o servidor Flask, tornando-o acessível na porta 8080