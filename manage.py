#!/usr/bin/env python
"""Django's command-line utility for administrative tasks."""
import os
import sys

def run_server():
    """Run administrative tasks."""
    os.environ.setdefault('DJANGO_SETTINGS_MODULE', 'TheDiode.settings')
    try:
        from django.core.management import execute_from_command_line
    except ImportError as exc:
        raise ImportError(
            "Couldn't import Django. Are you sure it's installed and "
            "available on your PYTHONPATH environment variable? Did you "
            "forget to activate a virtual environment?"
        ) from exc
    execute_from_command_line(sys.argv)

def run_mqtt():
    os.startfile("C:\\Users\\ticto\\Documents\\Projetos\\Projetos Django\\The Diode v2\\TheDiode\\core\\mqtt_sub_run.bat")

def main():
    run_server()

if __name__ == '__main__':
    main()
