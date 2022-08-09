from config_variables import *
import paho.mqtt.client as mqttClient

import time, os, re, sys, subprocess
import django, random

# Import Django modules
sys.path.append(PROJECT_PATH)
os.environ.setdefault('DJANGO_SETTINGS_MODULE', 'TheDiode.settings')
django.setup()
from subscriptions.models import PainelArmazenamentoModular

Connected = False #global variable for the state of the connection

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
        global Connected                #Use global variable
        Connected = True                #Signal connection 
    else:
        print("Connection failed")

def save_mqtt_in_db(message, db_class, db_propriety, operator):
    print("Mensagem em " + message.topic + ": " + message.payload.decode("utf-8"))
    value = operator(message.payload.decode())
    db_query = db_class.objects.first()
    db_query.__dict__[db_propriety] = value
    db_query.save()

def on_message(client, userdata, message):
    if PAINEL_TOPIC.modulo == message.topic: 
        save_mqtt_in_db(message, PainelArmazenamentoModular, 'modulo', int)
    if PAINEL_TOPIC.display == message.topic: 
        save_mqtt_in_db(message, PainelArmazenamentoModular, 'display', float)
        
  
def mqtt_sub_main():
    os.startfile(MOSQUITTO_RUN)
    time.sleep(1)
    
    broker_address= BROKER_ADDRESS  #Broker address
    port = BROKER_PORT              #Broker port
    user = "-"                   #Connection username
    password = "-"               #Connection password
    
    client_id = f'python-mqtt-{random.randint(0, 100)}'
    client = mqttClient.Client(client_id)               #create new instance
    #client.username_pw_set(user, password=password)   #set username and password
    client.on_connect= on_connect                      #attach function to callback
    client.on_message= on_message                      #attach function to callback

    print ("Connecting to broker")
    client.connect(broker_address, port)          #connect to broker

    #print ("Starting the loop")
    #client.loop_start()        #start the loop
    
    #while Connected != True:    #Wait for connection
    #    pass
    
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
    if python_process < 4:
        # Start MQTT Broker (Mosquitto) and server subscription
        mqtt_sub_main()

if __name__ == '__main__':
    main()