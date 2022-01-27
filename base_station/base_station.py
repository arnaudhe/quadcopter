#!/usr/bin/env python3

if __name__ == '__main__':

    from xbox import XBoxController
    from logger import UdpLogger
    from scope import Scope
    from controller import QuadcopterUdpController
    from gui import BaseStationGui
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
    parser.add_argument('-l', '--logger', action='store_true', help='Enable logging')
    parser.add_argument('-g', '--gui',    action='store_true', help='Enable gui')
    parser.add_argument('-r', '--remote', action='store_true', help='Enable xbox remote controller')
    parser.add_argument('-s', '--scope',  help='start a scope with provided configuration file', dest='scope_config')
    parser.add_argument('-o', '--output', action='store_true', help='output telemetry file in file')
    args = parser.parse_args()

    if not args.gui and not args.remote and not args.logger and not args.scope_config:
        print('No frontend (gui, remote, logger, scope) attached to controller. Please check command-line arguments.')
        sys.exit(1)

    controller = QuadcopterUdpController(data_model = data_model)

    if args.remote:
        xbox = XBoxController()
        xbox.start()
        controller.attach_xbox_controller(xbox)

    if args.logger:
        logger = UdpLogger()

    if args.gui or args.scope_config:
        from PyQt5.QtWidgets import QApplication
        app = QApplication([])
        if args.gui:
            w = BaseStationGui(controller)
        if args.scope_config:
            scope = Scope.from_config(args.scope_config, args.output)
            scope.start()
            start_telemetry(controller, scope.period, scope.channels)
        print('[MAIN] Qt Application started, close all windows or hit ctrl+q to stop')
        app.exec_()
        print('[MAIN] Shutdown requested')
        if args.scope_config:
            scope.stop()
            stop_telemetry(controller)
    else:
        print('[MAIN] Application started, hit ctrl+c to stop')
        while True:
            try:
                time.sleep(1.0)
            except KeyboardInterrupt:
                print('[MAIN] Shutdown requested')
                break

    if args.remote:
        xbox.stop()

    if args.logger:
        logger.shutdown()

    controller.shutdown()

    sys.exit(0)
