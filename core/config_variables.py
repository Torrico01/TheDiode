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
    connection = "Connection" # Specify the status of the sender device (ON or OFF)
COMMUNICATION_TOPIC = CommunicationTopic()
# Message structure: communication_topic/other_topic/id_of_sender message

# Modular Storage Panel Base
class ModularStoragePanelBaseTopic:
    project = "ModularStoragePanelBase" 
    module = "Module" # Specify the module of the Modular Storage Panel Base selected (int)
    outScreen = "Outputs/Screen" # Specify the value of the Modular Storage Panel Base screen (?)
PANEL_BASE_TOPIC = ModularStoragePanelBaseTopic()
# Message structure: project_topic/name_of_project/other_topic/id_of_sender message

# Modular Storage Panel
class ModularStoragePanelTopic:
    project = "ModularStoragePanel"
    configuration = "Configuration" # Specify all the stored data in Json format
    outDisplay = "Outputs/Display" # Specify the value of the Modular Storage Panel display
PANEL_TOPIC = ModularStoragePanelTopic()
# Message structure: project_topic/name_of_project/other_topic/id_of_sender message

# RGB Frame
class RGBFrameTopic:
    project = "RGBFrame"
    outRGBStrip = "Outputs/RGBStrip" # Specify the value of the RGB Strip Json configuration
RGB_FRAME_TOPIC = RGBFrameTopic()
# Message structure: project_topic/name_of_project/other_topic/id_of_sender message

# ---------- Dictionary => Project Number ID - Project Name ----------
project_dict = {
    '1': 'The Diode',
    '2': 'Painel Base 1', # Modular Storage Panel Base
    '3': 'Capacitor Eletrolitico 1', # Modular Storage Panel
    '4': 'Capacitor Eletrolitico 2', # Modular Storage Panel
    '5': 'RGB Frame 1' # RGB Frame
}