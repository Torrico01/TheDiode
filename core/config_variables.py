# ---------- ABSOLUTE PATHS ----------
# Project Path
PROJECT_PATH = "C:/Users/ticto/Documents/Projetos/Projetos Django/The Diode v2/TheDiode"
# Mosquitto Launcher
MOSQUITTO_RUN = "C:\\Users\\ticto\\Documents\\Projetos\\Projetos Django\\The Diode v2\\TheDiode\\core\\mosquitto_run.bat"
# Server MQTT Subscriber Launcher
MQTT_SUB_RUN = "C:\\Users\\ticto\\Documents\\Projetos\\Projetos Django\\The Diode v2\\TheDiode\\core\\mqtt_sub_run.bat"

# ---------- MQTT PARAMETERS ----------
BROKER_ADDRESS = "localhost"
BROKER_PORT = 1883
# ---------- Topics ----------
# Painel de Armazenamento Modular
class PainelArmazenamentoModularTopico:
    topico  = "PainelArmazenamentoModular"
    modulo  = "PainelArmazenamentoModular/Modulo"
    display = "PainelArmazenamentoModular/Display"
PAINEL_TOPIC = PainelArmazenamentoModularTopico()