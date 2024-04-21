# Copiar as libraries de User/Documents/Arduino/Library para usar corretamente algumas funções daqui

import paho.mqtt.client as mqttClient
from config_variables import *

import time, os, re, sys, subprocess
import django, random, json

# Import Django modules
sys.path.append(PROJECT_PATH)
os.environ.setdefault('DJANGO_SETTINGS_MODULE', 'TheDiode.settings')
django.setup()
from projects.models import *
from components.models import *
from storage.models import *

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


def save_mqtt_in_db(message, db_class, db_name, db_propriety, operator):
    value = operator(message.payload.decode())
    db_query = db_class.objects.get(name=db_name)
    db_query.__dict__[db_propriety] = value
    db_query.save()

def json_publish_formatter(base_name):
    jsonDictAll = {}
    for storagePanel in ModularStoragePanel3x3.objects.all():
        if (storagePanel.base.name == base_name):
            jsonDict = {}
            for count in range(1,10):
                slot = getattr(storagePanel,'slot_'+str(count))
                jsonDict[str(count) + '. ' + slot.name] = {
                    "ID": str(Category.objects.get(id=slot.tipo.id).id) + "/" + str(slot.tipo.id) + "/" + str(slot.id),
                    "Lim": str(slot.limite),
                    "Qtd": str(slot.quantidade)
                }
            jsonDictAll[storagePanel.name] = jsonDict 
    return(jsonDictAll)

def on_message(client, userdata, message):
    print("Mensagem em " + message.topic + ": " + message.payload.decode("utf-8"))

    if (COMMUNICATION_TOPIC.communication in message.topic):
        if (COMMUNICATION_TOPIC.connection in message.topic):
            split_message = message.topic.split('/')
            requester_project_number = split_message[2]
            try: requester_project_name = project_dict[requester_project_number]
            except: requester_project_name = ""

    if (PANEL_BASE_TOPIC.project in message.topic):
        if (PANEL_BASE_TOPIC.module in message.topic):
            split_message = message.topic.split('/')
            requested_project_name = split_message[1]
            requester_project_number = split_message[3]
            try: requester_project_name = project_dict[requester_project_number]
            except: requester_project_name = ""
            if requester_project_name: save_mqtt_in_db(message, ModularStoragePanelBase, requested_project_name, 'module', int)
    
    if (PANEL_TOPIC.project in message.topic):
        if (PANEL_TOPIC.configuration in message.topic):
            split_message = message.topic.split('/')
            requested_project_name = split_message[1]
            requester_project_number = split_message[3]
            try: requester_project_name = project_dict[requester_project_number]
            except: requester_project_name = ""
            if (message.payload.decode("utf-8") == "Request"):
                jsonDictAll = json_publish_formatter(requester_project_name)
                for key in jsonDictAll:
                    if key == requested_project_name:
                        jsonToSend = json.dumps(jsonDictAll[key])
                        client.publish(PANEL_TOPIC.project + "/" + key + "/" + PANEL_TOPIC.configuration + "/1", jsonToSend)

        if (PANEL_TOPIC.outDisplay in message.topic):
            split_message = message.topic.split('/')
            requested_project_name = split_message[1]
            requester_project_number = split_message[4]
            try: requester_project_name = project_dict[requester_project_number]
            except: requester_project_name = ""
            if requester_project_name: save_mqtt_in_db(message, ModularStoragePanel3x3, requested_project_name, 'display_7_segment', int)

    if (RGB_FRAME_TOPIC.project in message.topic):
        if (RGB_FRAME_TOPIC.outRGBStrip in message.topic):
            split_message = message.topic.split('/')
            requested_project_name = split_message[1]
            requester_project_number = split_message[4]
            try: requester_project_name = project_dict[requester_project_number]
            except: requester_project_name = ''
            if (requester_project_name and message.payload.decode("utf-8") == "Request"):
                # Get RGB frame json configuration
                rgbFrameObject = RGBFrame.objects.get(name=requester_project_name)
                seqpatt = rgbFrameObject.current_sequences
                jsonDictAll = rgbFrameObject.rgb_strip
                seqPattJson = jsonDictAll[str(seqpatt)]
                seqpatt_id = seqPattJson['id']
                # Get RGB frame sequence json configuration
                rgbFrameSeqObject = RGBFrameSequences.objects.get(id=seqpatt_id)
                sequence = rgbFrameSeqObject.current_sequence
                jsonSeqPatt = rgbFrameSeqObject.rgb_strip
                sequence += 1
                if (sequence >= len(jsonSeqPatt)):
                    sequence = 0
                    seqpatt += 1
                    if (seqpatt >= len(jsonDictAll)): seqpatt = 0
                # Update seqpatt value and save it
                db_query_frame = RGBFrame.objects.get(name=requester_project_name)
                db_query_seqpatt = RGBFrameSequences.objects.get(id=seqpatt_id)
                db_query_frame.current_sequences = seqpatt # number of sequence currently being transmitted
                db_query_frame.save()
                db_query_seqpatt.current_sequence = sequence
                db_query_seqpatt.save()
                # Send to project
                jsonSequence = jsonSeqPatt[str(sequence)]
                if (jsonSequence["Type"] == 1):
                    if ("Twinkle" in rgbFrameSeqObject.name):
                        jsonSequence["Name"] = "twinkle"
                jsonToSend = json.dumps(jsonSequence)
                client.publish(RGB_FRAME_TOPIC.project + "/" + requested_project_name + "/" + RGB_FRAME_TOPIC.outRGBStrip + "/1", jsonToSend)


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
    if python_process <= 5:
        # Start MQTT Broker (Mosquitto) and server subscription
        print("Calling server subscriber main loop")
        time.sleep(3)
        mqtt_sub_main()
    else:
        print("Exiting server subscriber main loop")
        time.sleep(3)
        

if __name__ == '__main__':
    main()