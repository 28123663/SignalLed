from building import * 

# get current dir path
cwd = GetCurrentDir()

# init src and inc vars
src = []
inc = [cwd + '/inc']

# add Signal Led common include
inc = inc + [cwd]

# add Signal Led source code
src = src + ['src/signal_led.c']

# add Signal Led test code
if GetDepend('SIGNAL_LED_USING_SAMPLE'):
    src = src + ['sample/led_sample.c']

# add group to IDE project
group = DefineGroup('SignalLed', src, depend = ['PKG_USING_SIGNAL_LED'], CPPPATH = inc)

Return('group')
