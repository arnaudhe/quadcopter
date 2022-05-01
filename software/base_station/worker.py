from threading import Thread
from time import sleep

class ThreadedWorker(Thread):

    """
    Simple enhancement of python class Thread
    It adds multiple starting prevention and a "stop" method
    Worker can inherit from this class and implement the "_run" method
    """

    def __init__(self, target = None):
        if target:
            super().__init__(target=target)
        else:
            super().__init__(target=self._run)
        self._running = False

    def _run(self):
        """Must be overloaded, displays an error message if not done"""
        print('ERROR. _run method must be overloaded')

    def start(self):
        """Start the run routine if not already done"""
        if not self._running:
            self._running = True
            super().start()

    def stop(self):
        """Stop the run routine if started, and waits for the end of execution"""
        if self._running:
            self._running = False
            self.join()

    def running(self):
        """Return if the worker has been started"""
        return self._running


class PeriodicWorker(ThreadedWorker):

    """
    Simple periodic task based on a built-in python Thread
    Worker can inherit from this class and implement the "_run" method
    which must be non-blocking.
    Note : worker won't have an exact period, but a sleep of period between
    two "_run" metod execution
    """

    def __init__(self, period):
        super().__init__(target=self._periodic_run)
        self._running = False
        self._period  = period

    def _run(self):
        """Must be overloaded, displays an error message if not done"""
        print('ERROR. _run method must be overloaded')

    def _periodic_run(self):
        """Onternal run loop"""
        while self.running():
            self._run()
            sleep(self._period)
