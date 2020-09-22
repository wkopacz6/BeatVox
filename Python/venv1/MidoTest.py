import mido
msg = mido.Message('note_on', channel=0, note=36, velocity=100)
port = mido.open_output('IAC Driver Bus 1')
port.send(msg)

print(msg)