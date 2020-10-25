# Voice over Bluetooth Low Energy SoC Example Application

This example demonstrates the bare minimum needed for a C application
that allows to transmit Voice over Bluetooth Low Energy (VoBLE).
The application starts advertising after boot and restarts advertising after
a connection is closed.
Voice transmission starts when BTN0 button is pressed and stops when BTN0
is released. The client is notified about the transmission status with the
"Transfer Status" characteristic.
Depending on the configuration audio data stream can be filtered and encoded
to ADPCM format. The stream is written into "Audio Data" characteristic.

VoBLE Service contains following configuration characteristics:
* Sample Rate     - Set sample rate 8 or 16 (kHz)
* Filter Enable   - Enable/Disable filtering
* Encoding Enable - Enable/Disable encoding

An example NCP host application called "voice" is available which
demonstrates how to connect to this example, sets correct configuration and
stores audio data into file.
