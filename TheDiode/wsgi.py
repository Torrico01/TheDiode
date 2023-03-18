"""
WSGI config for TheDiode project.

It exposes the WSGI callable as a module-level variable named ``application``.

For more information on this file, see
https://docs.djangoproject.com/en/3.1/howto/deployment/wsgi/
"""

from core.config_variables import *
import os

from django.core.wsgi import get_wsgi_application

os.environ.setdefault('DJANGO_SETTINGS_MODULE', 'TheDiode.settings')

#os.startfile(MOSQUITTO_RUN) # Calls the python script for the MQTT Broker Mosquitto to run locally
#os.startfile(MQTT_SUB_RUN) # Calls the python script for subscribing the server to every node

application = get_wsgi_application()
