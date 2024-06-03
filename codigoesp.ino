#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char* ssid = "*********";
const char* password = "*********";

ESP8266WebServer server(80);

const int pinBotao = D1;
const int pinLED = D2;
const int pinSensorForca = D0;
const int categoriasDeForca[] = {0, 50, 60, 90, 110}; // Tempos limite em milissegundos
const char* nomeCategorias[] = {"Esboçado", "Fraco", "Regular", "Bom", "Normal"}; // Nomes das categorias

bool botaoPressionado = false;
unsigned long timeInicio = 0;
unsigned long timeFinal = 0;
unsigned int timeTotal = 0;

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="pt-br">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Analisador fisioterapeutico</title>
    <style>
        body{
            background: linear-gradient(to right, #87ceeb, #ffffff);
            font-family: Arial, sans-serif;
        }
        table {
            width: 50%;
            border-collapse: collapse;
            margin: 20px 0;
        }
        table, th, td {
            border: 1px solid black;
        }
        th, td{
            padding: 10px;
            text-align: left;
            font-size: 20px;
        }
        th{
            background-color: #f2f2f2;
        }
        h2{
            text-decoration: underline;
        }
        .button-container {
            text-align: center;
            margin-top: 20px;
        }
        button {
            padding: 10px 20px;
            font-size: 16px;
            cursor: pointer;
        }
    </style>
</head>
<body>
    <h1>Análise do paciente</h1>
    <h2>Arthur Mendes</h2>
    <table>
        <thead>
            <tr>
                <th>Tempo Decorrido:</th>
            </tr>
        </thead>
        <tbody>
            <tr>
                <th>Tempo Total:</th>
            </tr>
        </tbody>
        <tbody>
            <tr>
                <th>Força Lida:</th>
            </tr>
            <tbody>
                <tr>
                    <th>Categoria de Força:</th>
                </tr>
            </tbody>
        </tbody>
    </table>
    <div class="button-container">
        <button onclick="window.print()">Imprimir</button>
    </div>
</body>
</html>
)rawliteral";

void handleRoot() {
  server.send(200, "text/html", index_html);
}


void setup() {
 pinMode(pinBotao, INPUT_PULLUP);
 pinMode(pinLED, OUTPUT);

  Serial.begin(9600);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println(".");
  }

  Serial.println("");
  Serial.println("WiFi conectado!");
  Serial.println("IP local: ");
  Serial.println(WiFi.localIP());
  server.on("/", handleRoot);
  server.begin();
}
void loop() {
  
  server.handleClient();
  
  if (digitalRead(pinBotao) == LOW && !botaoPressionado) {
    botaoPressionado = true;
    timeInicio = millis(); // Marca o tempo inicial quando apertar botão
    timeTotal = millis();

    Serial.println("Iniciando medição de tempo...");

    // Lê a leitura do sensor de força
    int leituraSensorForca = analogRead(pinSensorForca);

    // Converte a leitura do sensor em uma medida de força
    float forca = map(leituraSensorForca, 0, 1023, 0, 100);

    // Exemplo de cálculo de força em newtons
    float forcaNewton = forca;

    int categoriaAtual = 0;
    for (int i = 0; i < sizeof(categoriasDeForca) / sizeof(categoriasDeForca[0]); i++) {
      if (forcaNewton <= categoriasDeForca[i]) {
        categoriaAtual = i;
        break;
      }
    }

    Serial.print("Força lida: ");
    Serial.print(forcaNewton);
    Serial.println(" N");
    Serial.print(nomeCategorias[categoriaAtual]);
    Serial.println(")");

    delay(1000); // Evitar leituras rápidas
  }

  if (digitalRead(pinBotao) == HIGH && botaoPressionado) {
    botaoPressionado = false;
    timeFinal = millis(); // Marca o tempo final quando soltar o botão
    unsigned long timeDecorrido = timeFinal - timeInicio;
    Serial.print("Tempo decorrido: ");
    Serial.print(timeDecorrido / 1000);
    Serial.println(" Segundos");
    Serial.print("Tempo total decorrido: ");
    Serial.print(timeTotal / 1000);
    Serial.println(" Segundos");
  }
}
