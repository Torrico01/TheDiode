# Copiar as libraries de User/Documents/Arduino/Library para usar corretamente algumas funções daqui

import paho.mqtt.client as mqttClient
from config_variables import *

import time, os, re, sys, subprocess
import django, random, json

# Import Django modules
sys.path.append(PROJECT_PATH)
os.environ.setdefault('DJANGO_SETTINGS_MODULE', 'TheDiode.settings')
django.setup()
from projetos.models import PainelArmazenamentoModular


def get_processes_running():
    tasks = subprocess.check_output(['tasklist'])
    tasks = tasks.decode("cp1252").split("\r\n")
    processes = []
    for task in tasks:
        m = re.match("(.+?) +(\d+) (.+?) +(\d+) +(\d+.* K).*",task)
        if m is not None:
            processes.append({"image":m.group(1),
                        "pid":m.group(2),
                        "session_name":m.group(3),
                        "session_num":m.group(4),
                        "mem_usage":m.group(5)
                        })
    return processes

def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print("Connected to broker")
    else:
        print("Connection failed")


def save_mqtt_in_db(message, db_class, db_propriety, operator):
    value = operator(message.payload.decode())
    db_query = db_class.objects.first()
    db_query.__dict__[db_propriety] = value
    db_query.save()

def json_publish_formatter(jsonFilePath):
    jsonFile = open(jsonFilePath)
    jsonObj = json.load(jsonFile)
    jsonList = []
    for jsonHeader in jsonObj:
        jsonValue = jsonObj[jsonHeader]
        jsonValueString = json.dumps(jsonValue)
        jsonList.append(jsonValueString)
    return jsonList

def on_message(client, userdata, message):
    print("Mensagem em " + message.topic + ": " + message.payload.decode("utf-8"))
    if PAINEL_TOPIC.modulo == message.topic: 
        save_mqtt_in_db(message, PainelArmazenamentoModular, 'modulo', int)
        jsonMsgList = json_publish_formatter(PAINEL_CONFIG_JSON)
        client.publish(PAINEL_TOPIC.modulo + "/BancoDeDados/1", jsonMsgList[0])
    if PAINEL_TOPIC.outDisp == message.topic: 
        save_mqtt_in_db(message, PainelArmazenamentoModular, 'display', float)
        
def on_publish(client, userdata, mid):
    print("Published py1!")
  
def mqtt_sub_main():    
    broker_address= BROKER_ADDRESS  #Broker address
    port = BROKER_PORT              #Broker port
    user = "-"                   #Connection username
    password = "-"               #Connection password
    
    client_id = f'python-mqtt-1'
    client = mqttClient.Client(client_id)              #create new instance
    #client.username_pw_set(user, password=password)   #set username and password
    client.on_connect= on_connect                      #attach function to callback
    client.on_message= on_message                      #attach function to callback
    #client.on_publish= on_publish

    print ("Connecting to broker")
    client.connect(broker_address, port)          #connect to broker
    
    # =======================
    # Subscripe to all topics
    client.subscribe("#") 

    try:
        client.loop_forever()
    
    except KeyboardInterrupt:
        print ("exiting")
        client.disconnect()
        client.loop_stop()

def main():
    # Check number of python instances. If greater than 3 (server takes 2 instances and this script 1), don't start MQTT
    processes = get_processes_running()
    python_process = 0
    for p in processes:
        if "python" in p["image"]:
            python_process+=1
    if python_process <= 3:
        # Start MQTT Broker (Mosquitto) and server subscription
        print("Calling server subscriber main loop")
        time.sleep(3)
        mqtt_sub_main()
    else:
        print("Exiting server subscriber main loop")
        time.sleep(3)
        

if __name__ == '__main__':
    main()