# Em caso de mudança de IP:
# 1) Mudar IP no arquivo de launch.bat do servidor
# 2) Mudar IP no arquivo de config_variables do broker
# 3) Mudar IP no arquivo mosquitto.conf para escutar a porta do novo IP

# ---------- ABSOLUTE PATHS ----------
# Project Path
PROJECT_PATH = "C:/Users/ticto/Documents/Projetos/Projetos Django/The Diode v2/TheDiode"
# Mosquitto Launcher
MOSQUITTO_RUN = "C:\\Users\\ticto\\Documents\\Projetos\\Projetos Django\\The Diode v2\\TheDiode\\core\\mosquitto_run.bat"
# Server MQTT Subscriber Launcher
MQTT_SUB_RUN = "C:\\Users\\ticto\\Documents\\Projetos\\Projetos Django\\The Diode v2\\TheDiode\\core\\mqtt_sub_run.bat"

# ---------- JSON CONFIG FILES ----------
PAINEL_CONFIG_JSON = "C:\\Users\\ticto\\Documents\\Projetos\\Projetos Django\\The Diode v2\\TheDiode\\core\\static\\core\\painel.json"

# ---------- MQTT PARAMETERS ----------
BROKER_ADDRESS = "192.168.43.83" #"192.168.42.12"
BROKER_PORT = 1883
# ---------- Topics ----------
# Painel de Armazenamento Modular
class PainelDeArmazenamentoModularTopico:
    projeto = "PainelDeArmazenamentoModular"
    modulo  = "PainelDeArmazenamentoModular/Modulo"
    outDisp = "PainelDeArmazenamentoModular/Modulo/Outputs/Display"
PAINEL_TOPIC = PainelDeArmazenamentoModularTopico()
# ---------- IDs ----------
SERVER_ID = 1
PAINEL_ID = 2