import pigpio
import time

pi = pigpio.pi()
pi.set_mode(18, pigpio.OUTPUT)

pi.hardware_PWM(18, 50, 0.025*1000000)
#duty: 0.025(0.5ms)~0.12(2.4ms)
#Center: 0.0725(1.45ms)

time.sleep(2)

pi.write(18,0)
pi.stop()