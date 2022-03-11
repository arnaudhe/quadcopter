from worker import ThreadedWorker
from broker import Broker
import time

class Heartbeat(ThreadedWorker):

    """
    This class implement quadcopter detection and address retrieving, based on heartbeat frames periodically sent
    by the quadcopter.
    It relies on a UDP or radio broker, and register to their 'heartbeat' channel to receive quadcopter advertisement heartbeats.
    When quadcopter detected, use this class to get the quacopter address to be able to send it commands
    """

    CHANNEL = 'heartbeat'
    PERIOD = 1.0
    LOST_COUNT_MAX = 5

    def __init__(self, broker: Broker, quadcopter_name: str):
        super().__init__()
        self._broker = broker
        self._broker.register_channel(Heartbeat.CHANNEL)
        self._quadcopter_address = None
        self._quadcopter_name = quadcopter_name
        self._lost_count = 0
    
    def _run(self):
        """Internal run"""
        print('[heartbeat] Started')
        while self.running():
            if self.wait_heartbeat(blocking = False):
                if self._lost_count == Heartbeat.LOST_COUNT_MAX:
                    print(f'[heartbeat] Retrieved signal from {self._quadcopter_name}!')
                self._lost_count = 0
            else:
                if self._lost_count < Heartbeat.LOST_COUNT_MAX:
                    self._lost_count += 1
                    if self._lost_count == Heartbeat.LOST_COUNT_MAX:
                        print(f'[heartbeat] Lost signal from {self._quadcopter_name}')
        print('[heartbeat] Stopped')

    def wait_heartbeat(self, blocking = True):
        """
        Use this method in blocking mode to detect quadcopter advertisings
        and dynamically retrieve its name and address.
        This address can next be used to send data to quadcopter
        This method is also used internally in non-blocking mode to know if we
        still receive periodical heartbeat, ie if quadcopter is still in link range
        """
        result = False
        if blocking:
            print(f'[heartbeat] Waiting for heartbeat from {self._quadcopter_name}')
        while True:
            try:
                if self._broker.received_frame_pending(Heartbeat.CHANNEL):
                    address, data = self._broker.receive_from(Heartbeat.CHANNEL)
                    quadcopter_name = data.decode('utf-8')
                    if quadcopter_name == self._quadcopter_name:
                        self._quadcopter_address = address
                        result = True
                else:
                    time.sleep(Heartbeat.PERIOD)
            except KeyboardInterrupt:
                result = False
                break
            if (blocking == False) or (result == True):
                break
        return result

    def get_quadcopter_name(self) -> str:
        """
        When quadcopter have been detected, returns its name advertised in heartbeat frames.
        Make sure to have detected quadcopter first with wait_heartbeat method
        """
        return self._quadcopter_name

    def get_quadcopter_address(self):
        """
        When quadcopter have been detected, returns its address (IP: str or radio: int)
        Use this method to have the address, which enables sending frames to the 
        Make sure to have detected quadcopter first with wait_heartbeat method
        """
        return self._quadcopter_address

    def is_lost(self) -> bool:
        """
        The a communication fail status with quadcopter, ie lost of heatbeats receiving
        Call method "start" first to be able to detect communication lost.
        """
        if self._lost_count == Heartbeat.LOST_COUNT_MAX:
            return True
        else:
            return False