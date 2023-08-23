# ---------- ABSOLUTE PATHS ----------
# Project Path
PROJECT_PATH = "C:/Users/ticto/Documents/Projetos/Projetos Django/The Diode v2/TheDiode"
# Mosquitto Launcher
MOSQUITTO_RUN = "C:\\Users\\ticto\\Documents\\Projetos\\Projetos Django\\The Diode v2\\TheDiode\\core\\mosquitto_run.bat"
# Server MQTT Subscriber Launcher
MQTT_SUB_RUN = "C:\\Users\\ticto\\Documents\\Projetos\\Projetos Django\\The Diode v2\\TheDiode\\core\\mqtt_sub_run.bat"

# ---------- MQTT PARAMETERS ----------
BROKER_ADDRESS = "192.168.15.16"
BROKER_PORT = 1883
# ---------- Topics ----------
# Communication
class CommunicationTopic:
    communication = "Communication"
    connection = "Connection"
COMMUNICATION_TOPIC = CommunicationTopic()
# Modular Storage Panel Base
class ModularStoragePanelBaseTopic:
    project = "ModularStoragePanelBase" 
    module = "Module" # Specify the module of the Modular Storage Panel Base selected
    outScreen = "Outputs/Screen" # Specify the value of the Modular Storage Panel Base screen
PANEL_BASE_TOPIC = ModularStoragePanelBaseTopic()
# Modular Storage Panel
class ModularStoragePanelTopic:
    project = "ModularStoragePanel"
    configuration = "Configuration" # Specify all the stored data in Json format
    outDisplay = "Outputs/Display" # Specify the value of the Modular Storage Panel display
PANEL_TOPIC = ModularStoragePanelTopic()

# ---------- Dictionary => Project Number ID - Project Name ----------
project_dict = {
    '1': 'The Diode',
    '2': 'Painel Base 1',
    '3': 'Capacitor Eletrolitico 1',
    '4': 'Capacitor Eletrolitico 2'
}