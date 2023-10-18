import threading
from math import *
import queue
import ctypes
import traceback


class ThreadCalc:
    def __init__(self, fr, to, step, p):
        self.queue = queue.Queue()
        self.ID: int = -1
        self.fr = fr
        self.to = to
        self.step = step
        self.p = p

    def clacInThread(self):
        try:
            result = []
            totalSteps = floor((self.to - self.fr) / self.step) + 1
            for i in range(totalSteps):
                nowX = self.fr + self.step * i
                result.append(osc(nowX, self.p))
            self.queue.put(result)
        except SystemExit:
            return
        except Exception as e:
            self.queue.put("Error in running: "+str(e))

    def start(self):
        self._thread = threading.Thread(
            target=ThreadCalc.clacInThread, args=(self,))
        self._thread.start()
        self.ID = self._thread.ident

    def wait(self, sec=None):
        self._thread.join(timeout=sec)
        if not self._thread.is_alive():
            if not self.queue.empty():
                return self.queue.get_nowait()

    def stop(self):
        if self._thread.is_alive():
            ctypes.pythonapi.PyThreadState_SetAsyncExc(
                ctypes.c_long(self.ID), ctypes.py_object(SystemExit))
        self._thread.join()


threadCalcObj = None


def cppStart(fr, to, step, p):
    global threadCalcObj
    if (threadCalcObj == None):
        threadCalcObj = ThreadCalc(fr, to, step, p)
        threadCalcObj.start()
    else:
        return "Error: already running"


def cppStop():
    global threadCalcObj
    if (threadCalcObj != None):
        threadCalcObj.stop()
        threadCalcObj = None
    else:
        return "Error: Can not stop, not running"


def cppWait(ms):
    global threadCalcObj
    if (threadCalcObj != None):
        if ms >= 0:
            result = threadCalcObj.wait(sec=ms / 1000)
        else:
            result = threadCalcObj.wait()
        if (result == None):
            cppStop()
            return f"Error: Time Out! (Waited for {int(ms)}ms)"
        else:
            threadCalcObj = None
            return result
    else:
        return "Error: Can not wait, not running"


def cppFormatTB(tb):
    tb_list = traceback.format_tb(tb, 32)
    return "".join(tb_list)


def osc(x, p):
    x += p
    return sin(x)
