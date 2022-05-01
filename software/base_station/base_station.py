#!/usr/bin/env python3

from xbox import XBoxController
from logger import Logger
from scope import Scope
from controller import UdpController, RadioController
from gui import BaseStationGui
from udp import UdpBroker
from heartbeat import Heartbeat
from radio_command import RadioCommand
from radio import RadioBroker
import argparse
import json
import time
import sys
import os

def start_telemetry(controller, period, channels):
    controller.write('telemetry.values', ';'.join(channels))
    controller.write('telemetry.period', (int)(period * 1000))
    controller.write('telemetry.enable', True)

def stop_telemetry(controller):
    controller.write('telemetry.enable', False)

with open(os.path.join(os.path.dirname(__file__), os.path.pardir, 'data_model', 'data_model.json'), 'r') as json_file:
    data_model = json.load(json_file)

parser = argparse.ArgumentParser(description='Process some integers.')
parser.add_argument('backend', type=str, choices = ['udp', 'radio'])
parser.add_argument('-n', '--name',   type=str, help='quadcopter name', default="kwadcopter")
parser.add_argument('-l', '--logger', action='store_true', help='Enable logging')
parser.add_argument('-g', '--gui',    action='store_true', help='Enable gui')
parser.add_argument('-r', '--remote', action='store_true', help='Enable xbox remote controller')
parser.add_argument('-s', '--scope',  help='start a scope with provided configuration file', dest='scope_config')
parser.add_argument('-o', '--output', action='store_true', help='Output telemetry file in file')
args = parser.parse_args()

if not args.gui and not args.remote and not args.logger and not args.scope_config:
    print('No frontend (gui, remote, logger, scope) attached to controller. Please check command-line arguments.')
    sys.exit(1)

if args.remote:
    if XBoxController.detect():
        xbox = XBoxController()
        xbox.start()
    else:
        print('XBox remote not found.')
        sys.exit(1)

if args.backend == 'udp':
    broker = UdpBroker()
else:
    broker = RadioBroker(0)

broker.start()

heartbeat = Heartbeat(broker, args.name)

if not heartbeat.wait_heartbeat():
    print('Abort')
    broker.stop()
    sys.exit(1)

address = heartbeat.get_quadcopter_address()
print(f'[main] Quadcopter "{heartbeat.get_quadcopter_name()}" detected at {address}')

broker.set_quadcopter_address(address)

heartbeat.start()

if args.backend == 'udp':
    controller = UdpController(broker)
else:
    controller = RadioController(data_model, broker)

if args.remote:
    radio_command = RadioCommand(xbox, broker)
    radio_command.start()

if args.logger:
    logger = Logger(broker)
    logger.start()

if args.gui or args.scope_config:
    from PyQt5.QtWidgets import QApplication
    app = QApplication([])
    if args.gui:
        w = BaseStationGui(controller, data_model)
    if args.scope_config:
        scope = Scope.from_config(args.scope_config, args.output)
        scope.start()
        start_telemetry(controller, scope.period, scope.channels)
    print('[main] Qt Application started, close all windows or hit ctrl+q to stop')
    app.exec_()
    print('[main] Shutdown requested')
    if args.scope_config:
        scope.stop()
        stop_telemetry(controller)
else:
    print('[main] Application started, hit ctrl+c to stop')
    while True:
        try:
            time.sleep(1.0)
        except KeyboardInterrupt:
            print('[main] Shutdown requested')
            break

if args.remote:
    radio_command.stop()
    xbox.stop()

if args.logger:
    logger.stop()

heartbeat.stop()
broker.stop()

sys.exit(0)
