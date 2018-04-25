# This module just holds a convience function for calling the dac8532_set program
# It expects to be run on a bash shell on a RPi
#
# Matt Ebert 04/2018

from subprocess import Popen, PIPE
import os.path


def set_voltage(channel, voltage):
	if(channel >= 2 or channel < 0):
		raise KeyError
	script = os.path.join(os.path.abspath(os.path.dirname(__file__)), 'dac8532_set')
	p = Popen(map(str, [script, channel, voltage]), stdout=PIPE)
	output, err = p.communicate()
	output = output.strip()
	if p.returncode:
		print("Process exited on error with msg: `{}`".format(output))
		raise IOError


if __name__ == "__main__":
	# run test
	channel = 0
	voltage = 4  # V
	try:
		set_voltage(channel, voltage)
	except IOError:
		print("Error writing DAC")
	else:
		print("DAC channel: {}, voltage {} V".format(channel, voltage))
